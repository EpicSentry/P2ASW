//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "c_prop_portal.h"
#include "portal_shareddefs.h"
#include "clientsideeffects.h"
#include "tier0/vprof.h"
#include "materialsystem/ITexture.h"
#include "hud_macros.h"
#include "IGameSystem.h"
#include "view.h"						// For MainViewOrigin()
#include "clientleafsystem.h"			// For finding the leaves our portals are in
#include "portal_render_targets.h"		// Access to static references to Portal-specific render textures
#include "toolframework/itoolframework.h"
#include "toolframework_client.h"
#include "tier1/keyvalues.h"
#include "rendertexture.h"
#include "prop_portal_shared.h"
#include "particles_new.h"
#include "c_user_message_register.h"
#include "prediction.h"

#include "C_Portal_Player.h"

#include "c_pixel_visibility.h"

#include "glow_overlay.h"

#include "dlight.h"
#include "iefx.h"

#include "simple_keys.h"

#ifdef _DEBUG
#include "filesystem.h"
#endif

#include "debugoverlay_shared.h"


LINK_ENTITY_TO_CLASS( prop_portal, C_Prop_Portal );

#undef CProp_Portal

IMPLEMENT_CLIENTCLASS_DT( C_Prop_Portal, DT_Prop_Portal, CProp_Portal )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropVector( RECVINFO_NAME( m_angNetworkAngles, m_angRotation ) ),

	RecvPropVector( RECVINFO( m_ptOrigin ) ),
	RecvPropVector( RECVINFO( m_qAbsAngle ) ),

	RecvPropEHandle( RECVINFO(m_hLinkedPortal) ),
	RecvPropBool( RECVINFO(m_bActivated) ),
	RecvPropBool( RECVINFO(m_bIsPortal2) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_Prop_Portal )
	DEFINE_PRED_FIELD( m_bActivated, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	//DEFINE_PRED_FIELD( m_bOldActivatedState, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_hLinkedPortal, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_ptOrigin, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_qAbsAngle, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),

	//not actually networked fields. But we need them backed up and restored in the same way as the networked ones.	
	DEFINE_FIELD( m_vForward, FIELD_VECTOR ),
	DEFINE_FIELD( m_vRight, FIELD_VECTOR ),
	DEFINE_FIELD( m_vUp, FIELD_VECTOR ),
	DEFINE_FIELD( m_plane_Origin, FIELD_VECTOR4D ),
	//DEFINE_FIELD( m_matrixThisToLinked, FIELD_VMATRIX ),
END_PREDICTION_DATA()

bool g_bShowGhostedPortals = false;


ConVar cl_portal_teleportation_interpolation_fixup_method( "cl_portal_teleportation_interpolation_fixup_method", "1", 0, "0 = transform history only, 1 = insert discontinuity transform" );


void EntityPortalledMessageHandler( C_BaseEntity *pEntity, C_Portal_Base2D *pPortal, float fTime, bool bForcedDuck )
{

	Msg("ENTITY PORTALLED\n");
	Msg("ENTITY PORTALLED\n");
	Msg("ENTITY PORTALLED\n");
	Msg("ENTITY PORTALLED\n");
	Msg("ENTITY PORTALLED\n");

#if( PLAYERPORTALDEBUGSPEW == 1 )
	Warning( "EntityPortalledMessageHandler() %f -=- %f %i======================\n", fTime, engine->GetLastTimeStamp(), prediction->GetLastAcknowledgedCommandNumber() );
#endif
	// FIXME:
#if 0
	C_PortalGhostRenderable *pGhost = pPortal->GetGhostRenderableForEntity( pEntity );
	if( !pGhost )
	{
		//high velocity edge case. Entity portalled before it ever created a clone. But will need one for the interpolated origin history
		if( C_PortalGhostRenderable::ShouldCloneEntity( pEntity, pPortal, false ) )
		{
			pGhost = C_PortalGhostRenderable::CreateGhostRenderable( pEntity, pPortal );
			if( pGhost )
			{
				Assert( !pPortal->m_hGhostingEntities.IsValidIndex( pPortal->m_hGhostingEntities.Find( pEntity ) ) );
				pPortal->m_hGhostingEntities.AddToTail( pEntity );
				Assert( pPortal->m_GhostRenderables.IsValidIndex( pPortal->m_GhostRenderables.Find( pGhost ) ) );
				pGhost->PerFrameUpdate();
			}
		}
	}

	if( pGhost )
	{
		C_PortalGhostRenderable::CreateInversion( pGhost, pPortal, fTime );
	}
#endif
	if( pEntity->IsPlayer() )
	{
		((C_Portal_Player *)pEntity)->PlayerPortalled( pPortal, fTime, bForcedDuck );
		return;
	}	

	pEntity->AddEFlags( EFL_DIRTY_ABSTRANSFORM );

	VMatrix matTransform = pPortal->MatrixThisToLinked();

	CDiscontinuousInterpolatedVar< QAngle > &rotInterp = pEntity->GetRotationInterpolator();
	CDiscontinuousInterpolatedVar< Vector > &posInterp = pEntity->GetOriginInterpolator();


	if( cl_portal_teleportation_interpolation_fixup_method.GetInt() == 0 )
	{
		UTIL_TransformInterpolatedAngle( rotInterp, matTransform.As3x4(), fTime );
		UTIL_TransformInterpolatedPosition( posInterp, matTransform, fTime );
	}
	else
	{
		rotInterp.InsertDiscontinuity( matTransform.As3x4(), fTime );
		posInterp.InsertDiscontinuity( matTransform.As3x4(), fTime );
	}

	if ( pEntity->IsToolRecording() )
	{
		static EntityTeleportedRecordingState_t state;

		KeyValues *msg = new KeyValues( "entity_teleported" );
		msg->SetPtr( "state", &state );
		state.m_bTeleported = true;
		state.m_bViewOverride = false;
		state.m_vecTo = pEntity->GetAbsOrigin();
		state.m_qaTo = pEntity->GetAbsAngles();
		state.m_teleportMatrix = matTransform.As3x4();

		// Post a message back to all IToolSystems
		Assert( (int)pEntity->GetToolHandle() != 0 );
		ToolFramework_PostToolMessage( pEntity->GetToolHandle(), msg );

		msg->deleteThis();
	}
	
	C_Portal_Player* pPlayer = C_Portal_Player::GetLocalPortalPlayer();
	if ( pPlayer && pEntity == pPlayer->GetAttachedObject() )
	{
		C_BaseAnimating *pAnim = pEntity->GetBaseAnimating();	
		if ( pAnim && pAnim->IsUsingRenderOriginOverride() )
		{
			pPlayer->ResetHeldObjectOutOfEyeTransitionDT();
		}
	}
}


struct PortalTeleportationLogEntry_t
{
	CHandle<C_BaseEntity> hEntity;
	CHandle<C_Portal_Base2D> hPortal;
	float fTeleportTime;
	bool bForcedDuck;
};

static CThreadFastMutex s_PortalTeleportationLogMutex;
static CUtlVector<PortalTeleportationLogEntry_t> s_PortalTeleportationLog;

void RecieveEntityPortalledMessage( CHandle<C_BaseEntity> hEntity, CHandle<C_Portal_Base2D> hPortal, float fTime, bool bForcedDuck )
{
	PortalTeleportationLogEntry_t temp;
	temp.hEntity = hEntity;
	temp.hPortal = hPortal;
	temp.fTeleportTime = fTime;
	temp.bForcedDuck = bForcedDuck;

	s_PortalTeleportationLogMutex.Lock();
	s_PortalTeleportationLog.AddToTail( temp );
	s_PortalTeleportationLogMutex.Unlock();
}


void ProcessPortalTeleportations( void )
{
	s_PortalTeleportationLogMutex.Lock();
	for( int i = 0; i != s_PortalTeleportationLog.Count(); ++i )
	{
		PortalTeleportationLogEntry_t &entry = s_PortalTeleportationLog[i];

		C_Portal_Base2D *pPortal = entry.hPortal;
		if( pPortal == NULL )
			continue;

		//grab other entity's EHANDLE
		C_BaseEntity *pEntity = entry.hEntity;
		if( pEntity == NULL )
			continue;

		EntityPortalledMessageHandler( pEntity, pPortal, entry.fTeleportTime, entry.bForcedDuck );
	}
	s_PortalTeleportationLog.RemoveAll();
	s_PortalTeleportationLogMutex.Unlock();
}


static ConVar portal_demohack( "portal_demohack", "0", FCVAR_ARCHIVE, "Do the demo_legacy_rollback setting to help during demo playback of going through portals." );


void __MsgFunc_PortalFX_Surface(bf_read &msg)
{
	int iPortalEnt = msg.ReadShort();		
	C_Prop_Portal *pPortal= dynamic_cast<C_Prop_Portal*>( ClientEntityList().GetEnt( iPortalEnt ) );

	if( !pPortal )
	{
		Warning("!!! Failed to find portal %d !!!\n", iPortalEnt );
		return;
	}

	int iOwnerEnt = msg.ReadShort();		
	C_BaseEntity *pOwner = ClientEntityList().GetEnt( iOwnerEnt );

	int nTeam = msg.ReadByte();
	int nPortalNum = msg.ReadByte();
	int nEffect = msg.ReadByte();

	Vector vecOrigin;
	msg.ReadBitVec3Coord( vecOrigin );

	QAngle qAngle;
	msg.ReadBitAngles( qAngle );

	pPortal->CreateFizzleEffect( pOwner, nEffect, vecOrigin, qAngle, nTeam, nPortalNum );
}

USER_MESSAGE_REGISTER( PortalFX_Surface );

class C_PortalInitHelper : public CAutoGameSystem
{
	virtual bool Init()
	{
		//HOOK_MESSAGE( PlayerPortalled );
		HOOK_MESSAGE( PortalFX_Surface );
		
		if ( portal_demohack.GetBool() )
		{
			ConVarRef demo_legacy_rollback_ref( "demo_legacy_rollback" );
			demo_legacy_rollback_ref.SetValue( false ); //Portal demos are wrong if the eyes rollback as far as regular demos
		}
		// However, there are probably bugs with this when jump ducking, etc.
		return true;
	}
};
static C_PortalInitHelper s_PortalInitHelper;


C_Prop_Portal::C_Prop_Portal( void )
{
	TransformedLighting.m_LightShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
	CProp_Portal_Shared::AllPortals.AddToTail( this );
}

C_Prop_Portal::~C_Prop_Portal( void )
{
	CProp_Portal_Shared::AllPortals.FindAndRemove( this );
	g_pPortalRender->RemovePortal( this );

	for( int i = m_GhostRenderables.Count(); --i >= 0; )
	{
		delete m_GhostRenderables[i];
	}
	m_GhostRenderables.RemoveAll();
}

void C_Prop_Portal::Spawn( void )
{
	SetThink( &C_Prop_Portal::ClientThink );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	m_matrixThisToLinked.Identity(); //don't accidentally teleport objects to zero space
	BaseClass::Spawn();
}

void C_Prop_Portal::Activate( void )
{
	BaseClass::Activate();
}

void C_Prop_Portal::ClientThink( void )
{
	bool bDidAnything = false;
	if( m_fStaticAmount > 0.0f )
	{
		m_fStaticAmount -= gpGlobals->absoluteframetime;
		if( m_fStaticAmount < 0.0f ) 
			m_fStaticAmount = 0.0f;

		bDidAnything = true;
	}
	if( m_fSecondaryStaticAmount > 0.0f )
	{
		m_fSecondaryStaticAmount -= gpGlobals->absoluteframetime;
		if( m_fSecondaryStaticAmount < 0.0f ) 
			m_fSecondaryStaticAmount = 0.0f;

		bDidAnything = true;
	}

	if( m_fOpenAmount < 1.0f )
	{
		m_fOpenAmount += gpGlobals->absoluteframetime * 2.0f;
		if( m_fOpenAmount > 1.0f ) 
			m_fOpenAmount = 1.0f;

		bDidAnything = true;
	}

	if( bDidAnything == false )
	{
		SetNextClientThink( CLIENT_THINK_NEVER );
	}
}

bool C_Prop_Portal::Simulate()
{
	BaseClass::Simulate();

	//clear list of ghosted entities from last frame, and clear the clipping planes we put on them
	for( int i = m_hGhostingEntities.Count(); --i >= 0; )
	{
		C_BaseEntity *pEntity = m_hGhostingEntities[i].Get();

		if( pEntity != NULL )
			pEntity->m_bEnableRenderingClipPlane = false;
	}
	m_hGhostingEntities.RemoveAll();


	if( !IsActivedAndLinked() )
	{
		//remove all ghost renderables
		for( int i = m_GhostRenderables.Count(); --i >= 0; )
		{
			delete m_GhostRenderables[i];
		}
		
		m_GhostRenderables.RemoveAll();

		return true;
	}



	//Find objects that are intersecting the portal and mark them for later replication on the remote portal's side
	C_Portal_Player *pLocalPlayer = C_Portal_Player::GetLocalPlayer();
	C_BaseViewModel *pLocalPlayerViewModel = pLocalPlayer->GetViewModel();

	CBaseEntity *pEntsNearPortal[1024];
	int iEntsNearPortal = UTIL_EntitiesInSphere( pEntsNearPortal, 1024, GetNetworkOrigin(), PORTAL_HALF_HEIGHT, 0, PARTITION_CLIENT_NON_STATIC_EDICTS );

	if( iEntsNearPortal != 0 )
	{
		float fClipPlane[4];
		fClipPlane[0] = m_plane_Origin.normal.x;
		fClipPlane[1] = m_plane_Origin.normal.y;
		fClipPlane[2] = m_plane_Origin.normal.z;
		fClipPlane[3] = m_plane_Origin.dist - 0.3f;

		for( int i = 0; i != iEntsNearPortal; ++i )
		{
			CBaseEntity *pEntity = pEntsNearPortal[i];
			Assert( pEntity != NULL );

			bool bIsMovable = false;

			C_BaseEntity *pMoveEntity = pEntity;
			MoveType_t moveType = MOVETYPE_NONE;

			//unmoveables and doors can never get halfway in the portal
			while ( pMoveEntity )
			{
				moveType = pMoveEntity->GetMoveType();

				if ( !( moveType == MOVETYPE_NONE || moveType == MOVETYPE_PUSH ) )
				{
					bIsMovable = true;
					pMoveEntity = NULL;
				}
				else
					pMoveEntity = pMoveEntity->GetMoveParent();
			}

			if ( !bIsMovable )
				continue;

			Assert( dynamic_cast<C_Prop_Portal *>(pEntity) == NULL ); //should have been killed with (pEntity->GetMoveType() == MOVETYPE_NONE) check. Infinite recursion is infinitely bad.

			if( pEntity == pLocalPlayerViewModel )
				continue; //avoid ghosting view models

			bool bActivePlayerWeapon = false;

			C_BaseCombatWeapon *pWeapon = dynamic_cast<C_BaseCombatWeapon*>( pEntity );
			if ( pWeapon )
			{
				C_Portal_Player *pPortalPlayer = ToPortalPlayer( pWeapon->GetOwner() );
				if ( pPortalPlayer ) 
				{
					if ( pPortalPlayer->GetActiveWeapon() != pWeapon )
						continue; // don't ghost player owned non selected weapons
					else
						bActivePlayerWeapon = true;
				}
			}

			Vector ptEntCenter = pEntity->WorldSpaceCenter();
			if( bActivePlayerWeapon )
				ptEntCenter = pWeapon->GetOwner()->WorldSpaceCenter();

			if( (m_plane_Origin.normal.Dot( ptEntCenter ) - m_plane_Origin.dist) < -5.0f )
				continue; //entity is behind the portal, most likely behind the wall the portal is placed on

			if( !CProp_Portal_Shared::IsEntityTeleportable( pEntity ) )
				continue;

			if ( bActivePlayerWeapon )
			{
				if( !m_PortalSimulator.EntityHitBoxExtentIsInPortalHole( pWeapon->GetOwner() ) && 
					!m_PortalSimulator.EntityHitBoxExtentIsInPortalHole( pWeapon ) )
					continue;
			}
			else if( pEntity->IsPlayer() )
			{
				if( !m_PortalSimulator.EntityHitBoxExtentIsInPortalHole( (C_BaseAnimating*)pEntity ) )
					continue;
			}
			else
			{
				if( !m_PortalSimulator.EntityIsInPortalHole( pEntity ) )
					continue;
			}

			pEntity->m_bEnableRenderingClipPlane = true;
			memcpy( pEntity->m_fRenderingClipPlane, fClipPlane, sizeof( float ) * 4 );

			EHANDLE hEnt = pEntity;
			m_hGhostingEntities.AddToTail( hEnt );
		}
	}

	//now, fix up our list of ghosted renderables.
	{
		bool *bStillInUse = (bool *)stackalloc( sizeof( bool ) * (m_GhostRenderables.Count() + m_hGhostingEntities.Count()) );
		memset( bStillInUse, 0, sizeof( bool ) * (m_GhostRenderables.Count() + m_hGhostingEntities.Count()) );

		for( int i = m_hGhostingEntities.Count(); --i >= 0; )
		{
			C_BaseEntity *pRenderable = m_hGhostingEntities[i].Get();

			int j;
			for( j = m_GhostRenderables.Count(); --j >= 0; )
			{
				if( pRenderable == m_GhostRenderables[j]->m_pGhostedRenderable )
				{
					bStillInUse[j] = true;
					m_GhostRenderables[j]->PerFrameUpdate();
					break;
				}
			}
			
			if ( j >= 0 )
				continue;

			//newly added
			C_BaseEntity *pEntity = m_hGhostingEntities[i];

			bool bIsHeldWeapon = false;
			C_BaseCombatWeapon *pWeapon = dynamic_cast<C_BaseCombatWeapon*>( pEntity );
			if ( pWeapon && ToPortalPlayer( pWeapon->GetOwner() ) )
				bIsHeldWeapon = true;

			bool transparent = false;

			if (GetRenderAlpha() != 255)
				transparent = true;
				
			C_PortalGhostRenderable *pNewGhost = new C_PortalGhostRenderable( this,
																				pRenderable, 
																				transparent,
																				m_matrixThisToLinked, 
																				m_fGhostRenderablesClip,
																				(pEntity == pLocalPlayer || bIsHeldWeapon) );
			Assert( pNewGhost );

			bStillInUse[ m_GhostRenderables.AddToTail( pNewGhost ) ] = true;
			pNewGhost->PerFrameUpdate();

			// HACK - I just copied the CClientTools::OnEntityCreated code here,
			// since the ghosts aren't really entities - they don't have an entindex,
			// they're not in the entitylist, and they get created during Simulate(),
			// which isn't valid for real entities, since it changes the simulate list
			// -jd
			if ( ToolsEnabled() && clienttools->IsInRecordingMode() )
			{
				// Send deletion message to tool interface
				KeyValues *kv = new KeyValues( "created" );
				HTOOLHANDLE h = clienttools->AttachToEntity( pNewGhost );
				ToolFramework_PostToolMessage( h, kv );

				kv->deleteThis();
			}
		}

		//remove unused ghosts
		for ( int i = m_GhostRenderables.Count(); --i >= 0; )
		{
			if ( bStillInUse[i] )
				continue;

			// HACK - I just copied the CClientTools::OnEntityDeleted code here,
			// since the ghosts aren't really entities - they don't have an entindex,
			// they're not in the entitylist, and they get created during Simulate(),
			// which isn't valid for real entities, since it changes the simulate list
			// -jd
			C_PortalGhostRenderable *pGhost = m_GhostRenderables[i];
			if ( ToolsEnabled() )
			{
				HTOOLHANDLE handle = pGhost ? pGhost->GetToolHandle() : (HTOOLHANDLE)0;
				if ( handle != (HTOOLHANDLE)0 )
				{
					if ( clienttools->IsInRecordingMode() )
					{
						// Send deletion message to tool interface
						KeyValues *kv = new KeyValues( "deleted" );
						ToolFramework_PostToolMessage( handle, kv );
						kv->deleteThis();
					}

					clienttools->DetachFromEntity( pGhost );
				}
			}

			delete pGhost;
			m_GhostRenderables.FastRemove( i );
		}
	}

	//ensure the shared clip plane is up to date
	C_Prop_Portal *pLinkedPortal = m_hLinkedPortal.Get();

	m_fGhostRenderablesClip[0] = pLinkedPortal->m_plane_Origin.normal.x;
	m_fGhostRenderablesClip[1] = pLinkedPortal->m_plane_Origin.normal.y;
	m_fGhostRenderablesClip[2] = pLinkedPortal->m_plane_Origin.normal.z;
	m_fGhostRenderablesClip[3] = pLinkedPortal->m_plane_Origin.dist - 0.75f;

	return true;
}

void C_Prop_Portal::UpdateOnRemove( void )
{
	if( TransformedLighting.m_LightShadowHandle != CLIENTSHADOW_INVALID_HANDLE )
	{
		g_pClientShadowMgr->DestroyFlashlight( TransformedLighting.m_LightShadowHandle );
		TransformedLighting.m_LightShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
	}

	g_pPortalRender->RemovePortal( this );
	
	BaseClass::UpdateOnRemove();
}

void C_Prop_Portal::OnNewParticleEffect( const char *pszParticleName, CNewParticleEffect *pNewParticleEffect )
{
	if ( Q_stricmp( pszParticleName, "portal_1_overlap" ) == 0 || Q_stricmp( pszParticleName, "portal_2_overlap" ) == 0 )
	{
		float fClosestDistanceSqr = -1.0f;
		Vector vClosestPosition;

		int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
		if( iPortalCount != 0 )
		{
			C_Prop_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
			for( int i = 0; i != iPortalCount; ++i )
			{
				C_Prop_Portal *pTempPortal = pPortals[i];
				if ( pTempPortal != this && pTempPortal->m_bActivated )
				{
					Vector vPosition = pTempPortal->GetAbsOrigin();

					float fDistanceSqr = pNewParticleEffect->GetRenderOrigin().DistToSqr( vPosition );

					if ( fClosestDistanceSqr == -1.0f || fClosestDistanceSqr > fDistanceSqr )
					{
						fClosestDistanceSqr = fDistanceSqr;
						vClosestPosition = vPosition;
					}
				}
			}
		}

		if ( fClosestDistanceSqr != -1.0f )
		{
			pNewParticleEffect->SetControlPoint( 1, vClosestPosition );
		}
	}
}

void C_Prop_Portal::OnPreDataChanged( DataUpdateType_t updateType )
{
	//PreDataChanged.m_matrixThisToLinked = m_matrixThisToLinked;
	PreDataChanged.m_bIsPortal2 = m_bIsPortal2;
	PreDataChanged.m_bActivated = m_bActivated;
	PreDataChanged.m_vOrigin = GetNetworkOrigin();
	PreDataChanged.m_qAngles = GetNetworkAngles();
	PreDataChanged.m_hLinkedTo = m_hLinkedPortal.Get();

	BaseClass::OnPreDataChanged( updateType );
}

//ConVar r_portal_light_innerangle( "r_portal_light_innerangle", "90.0", FCVAR_CLIENTDLL );
//ConVar r_portal_light_outerangle( "r_portal_light_outerangle", "90.0", FCVAR_CLIENTDLL );
//ConVar r_portal_light_forward( "r_portal_light_forward", "0.0", FCVAR_CLIENTDLL );

void C_Prop_Portal::OnDataChanged( DataUpdateType_t updateType )
{
	C_Prop_Portal *pRemote = m_hLinkedPortal;
	m_pLinkedPortal = pRemote;
	GetVectors( &m_vForward, &m_vRight, &m_vUp );
	m_ptOrigin = GetNetworkOrigin();

	bool bPortalMoved = ( (PreDataChanged.m_vOrigin != m_ptOrigin ) ||
						(PreDataChanged.m_qAngles != GetNetworkAngles()) || 
						(PreDataChanged.m_bActivated == false) ||
						(PreDataChanged.m_bIsPortal2 != m_bIsPortal2) );

	bool bNewLinkage = ( (PreDataChanged.m_hLinkedTo.Get() != m_hLinkedPortal.Get()) );
	if( bNewLinkage )
		m_PortalSimulator.DetachFromLinked(); //detach now so moves are theoretically faster

	if( m_bActivated )
	{
		//generic stuff we'll need
		Vector vRemoteUp, vRemoteRight, vRemoteForward, ptRemoteOrigin;
		if( pRemote )
		{
			pRemote->GetVectors( &vRemoteForward, &vRemoteRight, &vRemoteUp );
			ptRemoteOrigin = pRemote->GetNetworkOrigin();
		}
		g_pPortalRender->AddPortal( this ); //will know if we're already added and avoid adding twice
		 
		if( bPortalMoved )
		{			
			Vector ptForwardOrigin = m_ptOrigin + m_vForward;// * 3.0f;
			Vector vScaledRight = m_vRight * (PORTAL_HALF_WIDTH * 0.95f);
			Vector vScaledUp = m_vUp * (PORTAL_HALF_HEIGHT  * 0.95f);

			m_PortalSimulator.MoveTo( GetNetworkOrigin(), GetNetworkAngles() );

			//update our associated portal environment
			//CPortal_PhysicsEnvironmentMgr::CreateEnvironment( this );

			m_fOpenAmount = 0.0f;
			//m_fStaticAmount = 1.0f; // This will cause the portal we are opening to show the static effect
			SetNextClientThink( CLIENT_THINK_ALWAYS ); //we need this to help open up

			//add static to the remote
			if( pRemote )
			{
				pRemote->m_fStaticAmount = 1.0f; // This will cause the other portal to show the static effect
				pRemote->SetNextClientThink( CLIENT_THINK_ALWAYS );
			}

			dlight_t *pFakeLight = NULL;
			ClientShadowHandle_t ShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
			if( pRemote )
			{
				pFakeLight = pRemote->TransformedLighting.m_pEntityLight;
				ShadowHandle = pRemote->TransformedLighting.m_LightShadowHandle;
				AssertMsg( (ShadowHandle == CLIENTSHADOW_INVALID_HANDLE) || (TransformedLighting.m_LightShadowHandle == CLIENTSHADOW_INVALID_HANDLE), "Two shadow handles found, should only have one shared handle" );
				AssertMsg( (pFakeLight == NULL) || (TransformedLighting.m_pEntityLight == NULL), "two lights found, should only have one shared light" );
				pRemote->TransformedLighting.m_pEntityLight = NULL;
				pRemote->TransformedLighting.m_LightShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
			}

			if( TransformedLighting.m_pEntityLight )
			{
				pFakeLight = TransformedLighting.m_pEntityLight;
				TransformedLighting.m_pEntityLight = NULL;
			}

			if( TransformedLighting.m_LightShadowHandle != CLIENTSHADOW_INVALID_HANDLE )
			{
				ShadowHandle = TransformedLighting.m_LightShadowHandle;
				TransformedLighting.m_LightShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
			}

			


			if( pFakeLight != NULL )
			{
				//turn off the light so it doesn't interfere with absorbed light calculations
				pFakeLight->color.r = 0;
				pFakeLight->color.g = 0;
				pFakeLight->color.b = 0;
				pFakeLight->flags = DLIGHT_NO_WORLD_ILLUMINATION | DLIGHT_NO_MODEL_ILLUMINATION;
				pFakeLight->radius = 0.0f;
				render->TouchLight( pFakeLight );
			}

			if( pRemote ) //now, see if we need to fake light coming through a portal
			{
#if 0
				Vector vLightAtRemotePortal( vec3_origin ), vLightAtLocalPortal( vec3_origin );

				if( pRemote ) //get lighting at remote portal
				{
					engine->ComputeLighting( ptRemoteOrigin, NULL, false, vLightAtRemotePortal, NULL );
				}

				//now get lighting at the local portal
				{
					engine->ComputeLighting( ptOrigin, NULL, false, vLightAtLocalPortal, NULL );
				}

				//Vector vLightDiff = vLightAtLocalPortal - vLightAtRemotePortal;
				//if( vLightDiff.Length() > 0.6f ) //a significant difference in lighting, remember that the light vectors are NOT normalized in length
				{
					//time to fake some light coming through the greater intensity portal to the lower intensity
					
					//are we transferring light from the local portal to the remote?
					bool bLocalToRemote = (vLightAtLocalPortal.Length() > vLightAtRemotePortal.Length());					

					Vector ptLightOrigin, vLightForward, vColor, vClampedColor;
					float fColorScale;
					{
						if( bLocalToRemote )
						{
							vColor = vLightAtLocalPortal;
							vLightForward = vRemoteForward;
							ptLightOrigin = ptRemoteOrigin;
						}
						else
						{
							vColor = vLightAtRemotePortal;
							vLightForward = vForward;
							ptLightOrigin = ptOrigin;
						}

						//clamp color values
						fColorScale = vColor.x;
						if( vColor.y > fColorScale )
							fColorScale = vColor.y;
						if( vColor.z > fColorScale )
							fColorScale = vColor.z;

						if( fColorScale > 1.0f )
							vClampedColor = vColor * (1.0f / fColorScale);
						else
							vClampedColor = vColor;
						
						/*if( vColor.x < 0.0f ) 
							vColor.x = 0.0f;
						if( vColor.x > 1.0f ) 
							vColor.x = 1.0f;

						if( vColor.y < 0.0f ) 
							vColor.y = 0.0f;
						if( vColor.y > 1.0f ) 
							vColor.y = 1.0f;

						if( vColor.z < 0.0f ) 
							vColor.z = 0.0f;
						if( vColor.z > 1.0f ) 
							vColor.z = 1.0f;*/
					}


					if( pFakeLight == NULL )
						pFakeLight = effects->CL_AllocElight( 0 ); //is there a difference between DLight and ELight when only lighting ents?

					if( pFakeLight != NULL ) //be absolutely sure that light allocation hasn't failed
					{
						if( bLocalToRemote )
						{
							//local light is greater, fake at remote portal
							pRemote->TransformedLighting.m_pEntityLight = pFakeLight;
							pFakeLight->key = pRemote->index;							
						}
						else
						{
							//remote light is greater, fake at local portal
							TransformedLighting.m_pEntityLight = pFakeLight;
							pFakeLight->key = index;					
						}

						pFakeLight->die = gpGlobals->curtime + 1e10;
						pFakeLight->flags = DLIGHT_NO_WORLD_ILLUMINATION;
						pFakeLight->minlight = 0.0f;
						pFakeLight->radius = 500.0f;
						pFakeLight->m_InnerAngle = 0.0f; //r_portal_light_innerangle.GetFloat();
						pFakeLight->m_OuterAngle = 120.0f; //r_portal_light_outerangle.GetFloat();
						pFakeLight->style = 0;
						
						pFakeLight->origin = ptLightOrigin;
						pFakeLight->m_Direction = vLightForward;

						pFakeLight->color.r = vClampedColor.x * 255;
						pFakeLight->color.g = vClampedColor.y * 255;
						pFakeLight->color.b = vClampedColor.z * 255;
						pFakeLight->color.exponent = ((signed int)(((*((unsigned int *)(&fColorScale))) & 0x7F800000) >> 23)) - 125; //strip the exponent from our maximum color
						//if( pFakeLight->color.exponent < 4 )
						//	pFakeLight->color.exponent = 4;

						render->TouchLight( pFakeLight );
					}

					FlashlightState_t state;
					{
						state.m_NearZ = 4.0f;
						state.m_FarZ = 500.0f;
						state.m_nSpotlightTextureFrame = 0;
						state.m_pSpotlightTexture = PortalDrawingMaterials::PortalLightTransfer_ShadowTexture;
						state.m_fConstantAtten = 0.0f;
						state.m_fLinearAtten = 500.0f;
						state.m_fQuadraticAtten = 0.0f;			
						state.m_fHorizontalFOVDegrees = 120.0f;
						state.m_fVerticalFOVDegrees = 120.0f;

						state.m_bEnableShadows = false;
						state.m_vecLightOrigin = ptLightOrigin;

						Vector vLightRight, vLightUp( 0.0f, 0.0f, 1.0f );
						if( fabs( DotProduct( vLightUp, vLightForward ) ) > 0.99f )
							vLightUp.Init( 0.0f, 1.0f, 0.0f );	// Don't want vLightUp and vLightForward to be parallel

						CrossProduct( vLightUp, vLightForward, vLightRight );
						VectorNormalize( vLightRight );
						CrossProduct( vLightForward, vLightRight, vLightUp );
						VectorNormalize( vLightUp );

						BasisToQuaternion( vLightForward, vLightRight, vLightUp, state.m_quatOrientation );

						state.m_Color[0] = vColor.x * 0.35f;
						state.m_Color[1] = vColor.y * 0.35f;
						state.m_Color[2] = vColor.z * 0.35f;
						state.m_Color[3] = 1.0f;
					}

					if( ShadowHandle != CLIENTSHADOW_INVALID_HANDLE )
					{
						g_pClientShadowMgr->UpdateFlashlightState( ShadowHandle, state ); //simpler update for existing handle
						g_pClientShadowMgr->UpdateProjectedTexture( ShadowHandle, true );
					}

					if( ShadowHandle == CLIENTSHADOW_INVALID_HANDLE )
					{
						ShadowHandle = g_pClientShadowMgr->CreateFlashlight( state );
						if( ShadowHandle != CLIENTSHADOW_INVALID_HANDLE )
							g_pClientShadowMgr->UpdateProjectedTexture( ShadowHandle, true );
					}


					if( bLocalToRemote )
						pRemote->TransformedLighting.m_LightShadowHandle = ShadowHandle;				
					else
						TransformedLighting.m_LightShadowHandle = ShadowHandle;					
				}
#endif
			}
		}
	}
	else
	{
		g_pPortalRender->RemovePortal( this );

		m_PortalSimulator.DetachFromLinked();

		if( TransformedLighting.m_pEntityLight )
		{
			TransformedLighting.m_pEntityLight->die = gpGlobals->curtime;
			TransformedLighting.m_pEntityLight = NULL;
		}

		if( TransformedLighting.m_LightShadowHandle != CLIENTSHADOW_INVALID_HANDLE )
		{
			g_pClientShadowMgr->DestroyFlashlight( TransformedLighting.m_LightShadowHandle );
			TransformedLighting.m_LightShadowHandle = CLIENTSHADOW_INVALID_HANDLE;
		}
	}

	if( (PreDataChanged.m_hLinkedTo.Get() != m_hLinkedPortal.Get()) && m_hLinkedPortal.Get() )
		m_PortalSimulator.AttachTo( &m_hLinkedPortal.Get()->m_PortalSimulator );
	

	BaseClass::OnDataChanged( updateType );
	
	if( bNewLinkage || bPortalMoved )
	{
		PortalMoved(); //updates link matrix and internals
	}

	if ( bPortalMoved )
	{
		UpdateOriginPlane();
	}

	if( bPortalMoved || bNewLinkage )
	{
		UpdateGhostRenderables();
		if( pRemote )
			pRemote->UpdateGhostRenderables();
	}
}

void C_Prop_Portal::UpdateGhostRenderables( void )
{
	//lastly, update all ghost renderables
	for( int i = m_GhostRenderables.Count(); --i >= 0; )
	{
		m_GhostRenderables[i]->m_matGhostTransform = m_matrixThisToLinked;;
	}
}

extern ConVar building_cubemaps;

int C_Prop_Portal::DrawModel( int flags, const RenderableInstance_t& instance )
{
	// Don't draw in cube maps because it makes an ugly colored splotch
	if( m_bActivated == false || building_cubemaps.GetBool() )
		return 0;

	int iRetVal = 0;

	C_Prop_Portal *pLinkedPortal = m_hLinkedPortal.Get();

	if ( pLinkedPortal == NULL )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS ); // we need this to help fade out
	}

	if ( !g_pPortalRender->ShouldUseStencilsToRenderPortals() )
	{
		DrawPortal();
	}

	if( WillUseDepthDoublerThisDraw() )
		m_fSecondaryStaticAmount = 0.0f;

	iRetVal = BaseClass::DrawModel( flags, instance );

	return iRetVal;
}


//-----------------------------------------------------------------------------
// Handle recording for the SFM
//-----------------------------------------------------------------------------
void C_Prop_Portal::GetToolRecordingState( KeyValues *msg )
{
	if ( !ToolsEnabled() )
		return;

	VPROF_BUDGET( "C_Prop_Portal::GetToolRecordingState", VPROF_BUDGETGROUP_TOOLS );
	BaseClass::GetToolRecordingState( m_bActivated, msg );

	if ( !m_bActivated )
	{
		BaseEntityRecordingState_t *pBaseEntity = (BaseEntityRecordingState_t*)msg->GetPtr( "baseentity" );
		pBaseEntity->m_bVisible = false;
	}
}

void C_Prop_Portal::UpdateOriginPlane( void )
{
	//setup our origin plane
	GetVectors( &m_plane_Origin.normal, NULL, NULL );
	m_plane_Origin.dist = m_plane_Origin.normal.Dot( GetAbsOrigin() );
	m_plane_Origin.signbits = SignbitsForPlane( &m_plane_Origin );

	Vector vAbsNormal;
	vAbsNormal.x = fabs(m_plane_Origin.normal.x);
	vAbsNormal.y = fabs(m_plane_Origin.normal.y);
	vAbsNormal.z = fabs(m_plane_Origin.normal.z);

	if( vAbsNormal.x > vAbsNormal.y )
	{
		if( vAbsNormal.x > vAbsNormal.z )
		{
			if( vAbsNormal.x > 0.999f )
				m_plane_Origin.type = PLANE_X;
			else
				m_plane_Origin.type = PLANE_ANYX;
		}
		else
		{
			if( vAbsNormal.z > 0.999f )
				m_plane_Origin.type = PLANE_Z;
			else
				m_plane_Origin.type = PLANE_ANYZ;
		}
	}
	else
	{
		if( vAbsNormal.y > vAbsNormal.z )
		{
			if( vAbsNormal.y > 0.999f )
				m_plane_Origin.type = PLANE_Y;
			else
				m_plane_Origin.type = PLANE_ANYY;
		}
		else
		{
			if( vAbsNormal.z > 0.999f )
				m_plane_Origin.type = PLANE_Z;
			else
				m_plane_Origin.type = PLANE_ANYZ;
		}
	}
}

void C_Prop_Portal::SetIsPortal2( bool bValue )
{
	m_bIsPortal2 = bValue;
}

bool C_Prop_Portal::IsActivedAndLinked( void ) const
{
	return ( m_bActivated && m_hLinkedPortal.Get() != NULL );
}

void C_Prop_Portal::CreateFizzleEffect( C_BaseEntity *pOwner, int iEffect, Vector vecOrigin, QAngle qAngles, int nTeam, int nPortalNum )
{
#if 1
	Color color = UTIL_Portal_Color_Particles( nPortalNum, nTeam );

	Vector vColor;
	vColor.x = color.r();
	vColor.y = color.g();
	vColor.z = color.b();

	CUtlReference<CNewParticleEffect> pEffect;
	if ( !pOwner )
		return;

	bool bCreated = false;

	switch ( iEffect )
	{
	case PORTAL_FIZZLE_SUCCESS:
		{
			pEffect = CNewParticleEffect::CreateOrAggregate( NULL, "portal_success", vecOrigin, NULL );
			bCreated = true;
		}
		break;

	case PORTAL_FIZZLE_BAD_SURFACE:
		{
			pEffect = CNewParticleEffect::CreateOrAggregate( NULL, "portal_badsurface", vecOrigin, NULL );
		}
		break;

	case PORTAL_FIZZLE_CLOSE:
		{
			pEffect = CNewParticleEffect::CreateOrAggregate( NULL, "portal_close", vecOrigin, NULL );
		}
		break;
	}

	if ( pEffect )
	{
		pEffect->SetControlPoint( 0, vecOrigin );

		Vector vecForward, vecRight, vecUp;
		AngleVectors( qAngles, &vecForward, &vecRight, &vecUp );
		pEffect->SetControlPointOrientation( 0, vecForward, vecRight, vecUp );

		pEffect->SetControlPoint( 2, vColor );
	}

	if ( bCreated )
	{
		//Color rgbaColor = Color( vColor.x, vColor.y, vColor.z, 255 );
		//CreateAttachedParticles( &rgbaColor );
	}
#endif
}


void C_Prop_Portal::DoFizzleEffect( int iEffect, bool bDelayedPos /*= true*/ )
{
#if 1
	if( prediction->InPrediction() && !prediction->IsFirstTimePredicted() )
		return; //early out if we're repeatedly creating particles. Creates way too many particles.

#if 0
	Vector vecOrigin = ( ( bDelayedPos ) ? ( m_vDelayedPosition ) : ( GetAbsOrigin() ) );
	QAngle qAngles = ( ( bDelayedPos ) ? ( m_qDelayedAngles ) : ( GetAbsAngles() ) );
#else
	Vector vecOrigin = ( ( bDelayedPos ) ? ( vec3_origin ) : ( GetAbsOrigin() ) );
	QAngle qAngles = ( ( bDelayedPos ) ? ( vec3_angle ) : ( GetAbsAngles() ) );
#endif

	Vector vForward, vUp;
	AngleVectors( qAngles, &vForward, &vUp, NULL );
	vecOrigin = vecOrigin + vForward * 1.0f;

	int nPortalNum = m_bIsPortal2 ? 2 : 1;
	int nTeam = GetTeamNumber();

	if ( iEffect != PORTAL_FIZZLE_SUCCESS && iEffect != PORTAL_FIZZLE_CLOSE )
		iEffect = PORTAL_FIZZLE_BAD_SURFACE;

	C_BasePlayer *pPlayer = GetPredictionOwner();
	if ( pPlayer )
		nTeam = pPlayer->GetTeamNumber();

	VectorAngles( vUp, vForward, qAngles );
	CreateFizzleEffect( pPlayer, iEffect, vecOrigin, qAngles, nTeam, nPortalNum );
#endif
}