//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "triggers.h"
#include "portal_player.h"
#include "weapon_portalgun.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "physobj.h"
#include "portal_grabcontroller_shared.h"
#include "model_types.h"
#include "rumble_shared.h"
#include "trigger_portal_cleanser.h"
#include "soundenvelope.h"
#include "UtlSortVector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

char* CTriggerPortalCleanser::s_szPlayerPassesTriggerFiltersThinkContext = "PlayerPassesTriggerFiltersThinkContext";


ConVar sv_portal_cleanser_think_rate( "sv_portal_cleanser_think_rate", "0.25", FCVAR_DEVELOPMENTONLY, "How often, in seconds should the portal cleanser think." );

BEGIN_DATADESC( FizzlerMultiOriginSoundPlayer )

	DEFINE_SOUNDPATCH( m_pSound ),
	DEFINE_THINKFUNC( RemoveThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( FizzlerMultiOriginSoundPlayer, DT_FizzlerMultiOriginSoundPlayer )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( fizzler_multiorigin_sound_player, FizzlerMultiOriginSoundPlayer );

FizzlerMultiOriginSoundPlayer::FizzlerMultiOriginSoundPlayer()
{
	m_pSound = NULL;
}

FizzlerMultiOriginSoundPlayer::~FizzlerMultiOriginSoundPlayer()
{
	CSoundEnvelopeController &Controller = CSoundEnvelopeController::GetController();
	if ( m_pSound )
	{
		Controller.Shutdown( m_pSound );
		Controller.SoundDestroy( m_pSound );
	}
}

void FizzlerMultiOriginSoundPlayer::Spawn()
{
	BaseClass::Spawn();
	SetThink( &FizzlerMultiOriginSoundPlayer::RemoveThink );
	SetNextThink( gpGlobals->curtime );
}

void CTriggerPortalCleanser::UpdateOnRemove( void )
{
	StopAmbientSounds();
	BaseClass::UpdateOnRemove();
}

int FizzlerMultiOriginSoundPlayer::UpdateTransmitState( void )
{
	// 8 was the value Ghidra gave us, fix me if this is inaccurate.
	return SetTransmitState( FL_EDICT_ALWAYS );
}

void CTriggerPortalCleanser::StartAmbientSounds( void )
{
	CPASFilter filter;
	filter.AddRecipientsByPAS( GetAbsOrigin() );
	EmitSound( filter, entindex(), "VFX.FizzlerStart", &GetAbsOrigin() );
}

void CTriggerPortalCleanser::StopAmbientSounds( void )
{
	CPASFilter filter;
	filter.AddRecipientsByPAS( GetAbsOrigin() );	
	EmitSound( filter, entindex(), "VFX.FizzlerDestroy", &GetAbsOrigin() );
}

void CTriggerPortalCleanser::Enable( void )
{
	BaseClass::Enable();
	FizzleTouchingPortals();
	if ( m_bVisible )
		StartAmbientSounds();
}

void CTriggerPortalCleanser::Disable( void )
{
	BaseClass::Disable();
	if ( m_bVisible )
		StopAmbientSounds();
}

void CTriggerPortalCleanser::ClearVortexObjects( void )
{
	this->m_VortexObjects[0].m_flDistanceSq = FLT_MAX;
	this->m_VortexObjects[0].m_hEnt = NULL;
	this->m_VortexObjects[1].m_flDistanceSq = FLT_MAX;
	this->m_VortexObjects[1].m_hEnt = NULL;
}

void FizzlerMultiOriginSoundPlayer::RemoveThink( void )
{
	if ( ITriggerPortalCleanserAutoList::AutoList().Count() )
		SetNextThink( gpGlobals->curtime );
	else
		UTIL_Remove(this);
}

FizzlerMultiOriginSoundPlayer *FizzlerMultiOriginSoundPlayer::Create( IRecipientFilter &filter, const char *soundName )
{
	FizzlerMultiOriginSoundPlayer *pProxy = (FizzlerMultiOriginSoundPlayer *)CreateEntityByName("fizzler_multiorigin_sound_player", -1, 1);
	if (pProxy)
	{

#if 0 // TODO: Which one is accurate?
		pProxy->Spawn();
#else
		DispatchSpawn( pProxy );
#endif
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		
		pProxy->m_pSound = controller.SoundCreate( filter, pProxy->entindex(), soundName );
		controller.Play( pProxy->m_pSound, 1.0, 100.0, 0 );

	}
	return pProxy;
}


// Creates a base entity with model/physics matching the parameter ent.
// Used to avoid higher level functions on a disolving entity, which should be inert
// and not react the way it used to (touches, etc).
// Uses simple physics entities declared in physobj.cpp
CBaseEntity* ConvertToSimpleProp ( CBaseEntity* pEnt )
{
	CBaseEntity *pRetVal = NULL;
	int modelindex = pEnt->GetModelIndex();
	const model_t *model = modelinfo->GetModel( modelindex );
	if ( model && modelinfo->GetModelType(model) == mod_brush )
	{
		pRetVal = CreateEntityByName( "simple_physics_brush" );
	}
	else
	{
		pRetVal = CreateEntityByName( "simple_physics_prop" );
	}

	CBaseAnimating *pRetValAsAnimating = pRetVal->GetBaseAnimating();

	if ( pRetValAsAnimating )
	{
		Assert(pEnt->GetBaseAnimating());

		CBaseAnimating *pEntAsAnimating = pEnt->GetBaseAnimating();
		
		// NOTE: These fixes may not be accurate...
		pRetValAsAnimating->m_nSkin = pEntAsAnimating->m_nSkin;
		pRetValAsAnimating->m_flModelScale = pEntAsAnimating->m_flModelScale;
	}

	pRetVal->KeyValue( "model", STRING(pEnt->GetModelName()) );
	pRetVal->SetAbsOrigin( pEnt->GetAbsOrigin() );
	pRetVal->SetAbsAngles( pEnt->GetAbsAngles() );
	pRetVal->Spawn();
	pRetVal->VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
	
	return pRetVal;
}

CleanserVortexTraceEnum::CleanserVortexTraceEnum( CBaseEntity **pList, int listMax, CBaseTrigger *pCleanser )
{
	m_pList = pList;
	m_count = 0;
	m_listMax = listMax;
	m_pCleanser = pCleanser;
}

bool CleanserVortexTraceEnum::AddToList( CBaseEntity *pEntity )
{
	bool result = false;
	
	if (m_count < this->m_listMax)
	{
		m_pList[m_count] = pEntity;
		result = true;
		++m_count;
	}
	return result;
}

IterationRetval_t CleanserVortexTraceEnum::EnumElement( IHandleEntity *pHandleEntity )
{

	if (staticpropmgr->IsStaticProp( pHandleEntity ))
		return ITERATION_CONTINUE;

	EHANDLE hEntity = pHandleEntity->GetRefEHandle();

	if (!hEntity)
		return ITERATION_CONTINUE;
	if (hEntity == m_pCleanser
		|| m_pCleanser && !m_pCleanser->PassesTriggerFilters( hEntity ) )
	{
		return ITERATION_CONTINUE;
	}
	if (!g_pszFizzlingVortexObjects[0])
		return ITERATION_CONTINUE;
	char **v7 = g_pszFizzlingVortexObjects;

	int i = 0;
	while (1)
	{
		const char *v8 = *v7;
		if ( hEntity->ClassMatches( v8 ) )
			break;
		v7 = &g_pszFizzlingVortexObjects[++i];
		if (!*v7)
			return ITERATION_CONTINUE;
	}

	if ( m_count >= m_listMax )
		return ITERATION_STOP;
	m_pList[m_count] = hEntity;
	++m_count;
	return ITERATION_CONTINUE;
}

BEGIN_DATADESC( CTriggerPortalCleanser )

	DEFINE_KEYFIELD(m_bVisible, FIELD_BOOLEAN, "Visible"),

	// Outputs
	DEFINE_OUTPUT( m_OnDissolve, "OnDissolve" ),
	DEFINE_OUTPUT( m_OnFizzle, "OnFizzle" ),
	DEFINE_OUTPUT( m_OnDissolveBox, "OnDissolveBox" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "FizzleTouchingPortals", InputFizzleTouchingPortals ),

	DEFINE_THINKFUNC( SearchThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CTriggerPortalCleanser, DT_TriggerPortalCleanser )
	SendPropBool( SENDINFO( m_bDisabled ) ),
	SendPropBool( SENDINFO( m_bVisible ) ),
	SendPropBool( SENDINFO( m_bObject1InRange ) ),
	SendPropBool( SENDINFO( m_bObject2InRange ) ),
	SendPropBool( SENDINFO( m_bPlayersPassTriggerFilters ) ),

	SendPropFloat( SENDINFO( m_flPortalShotTime ) ),
	
	SendPropEHandle( SENDINFO( m_hObject1 ) ),	
	SendPropEHandle( SENDINFO( m_hObject2 ) ),
END_SEND_TABLE()


LINK_ENTITY_TO_CLASS( trigger_portal_cleanser, CTriggerPortalCleanser );

IMPLEMENT_AUTO_LIST( ITriggerPortalCleanserAutoList );

CTriggerPortalCleanser::CTriggerPortalCleanser()
{
	m_bVisible = false;

	m_bObject1InRange = false;
	m_bObject2InRange = false;
	m_bUseScanline = true;

	m_bPlayersPassTriggerFilters = true;
	
	m_hObject1 = NULL;
	m_hObject2 = NULL;

}

CTriggerPortalCleanser::~CTriggerPortalCleanser()
{

}

void CTriggerPortalCleanser::SetPortalShot( void )
{
	m_flPortalShotTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerPortalCleanser::Spawn( void )
{	
	BaseClass::Spawn();
	Precache();

	m_bClientSidePredicted = true;

	InitTrigger();

	CRecipientFilter filter;
	filter.AddAllPlayers();
	filter.MakeReliable();

	s_FizzlerAmbientSoundPlayer = FizzlerMultiOriginSoundPlayer::Create( filter, "VFX.FizzlerLp" );

	s_FizzlerAmbientSoundPlayer->SetAbsOrigin( GetAbsOrigin() );

	if (m_bVisible)
	{
		RemoveEffects(EF_NODRAW);
	}
	
    SetTransmitState( 32 );
	SetThink( &CTriggerPortalCleanser::SearchThink );
    SetNextThink( sv_portal_cleanser_think_rate.GetFloat() + gpGlobals->curtime );
}

void CTriggerPortalCleanser::Precache( void )
{
	PrecacheScriptSound("VFX.FizzlerLp");
	PrecacheScriptSound("VFX.FizzlerStart");
	PrecacheScriptSound("VFX.FizzlerDestroy");
	PrecacheParticleSystem("cleanser_scanline");
}

void CTriggerPortalCleanser::Activate( void )
{
	BaseClass::Activate();
	if ( m_bVisible )
		StartAmbientSounds();
}

int CTriggerPortalCleanser::UpdateTransmitState( void )
{
	return SetTransmitState( 32 );
}

bool CTriggerPortalCleanser::IsCloserThanExistingObjects( FizzlerVortexObjectInfo_t &info, int &iIndex )
{
	int index;

	if ( m_VortexObjects[0].m_flDistanceSq >= info.m_flDistanceSq )
	{
	  index = 0;
	}
	else
	{
		index = 1;
		if ( this->m_VortexObjects[1].m_flDistanceSq < info.m_flDistanceSq )
			return false;
	}
	iIndex = index;
	return true;
}

void CTriggerPortalCleanser::FizzleTouchingPortals( void )
{
	Vector vMax;
	Vector vMin;		
	CollisionProp()->WorldSpaceAABB( &vMin, &vMax );

	int count = CProp_Portal_Shared::AllPortals.Count();

	Vector vBoxCenter = (vMax + vMin) * 0.5;
	Vector vBoxExtents = (vMax - vMin) * 0.5;
	
	for (int i = 0; i < count; ++i)
	{
		CProp_Portal *pPortal = CProp_Portal_Shared::AllPortals[i];
		if ( UTIL_IsBoxIntersectingPortal( vBoxCenter, vBoxExtents, pPortal, 0.0 ) )
		{
			pPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, true );
			pPortal->Fizzle();
		}
	}
}

void CTriggerPortalCleanser::SearchThink( void )
{
	// TODO:
}

void CTriggerPortalCleanser::Touch( CBaseEntity *pOther )
{
	if ( !PassesTriggerFilters( pOther ) )
		return;

	if ( pOther->IsPlayer() )
	{
		// Static cast is slightly more efficient than ToPortalPlayer
		CPortal_Player *pPlayer = static_cast<CPortal_Player*>( pOther );

		if ( pPlayer )
		{
			CWeaponPortalgun *pPortalgun = dynamic_cast<CWeaponPortalgun*>( pPlayer->Weapon_OwnsThisType( "weapon_portalgun" ) );

			if ( pPortalgun )
			{
				bool bFizzledPortal = false;

				if ( pPortalgun->CanFirePortal1() )
				{
					CProp_Portal *pPortal = CProp_Portal::FindPortal( pPortalgun->GetLinkageGroupID(), false );

					if ( pPortal && pPortal->IsActive() )
					{
						pPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, false );
						pPortal->Fizzle();
						pPortal->EmitSound( "Portal.FizzlerShimmy" );
						// HACK HACK! Used to make the gun visually change when going through a cleanser!
						pPortalgun->m_fEffectsMaxSize1 = 50.0f;

						bFizzledPortal = true;
					}
				}

				if ( pPortalgun->CanFirePortal2() )
				{
					CProp_Portal *pPortal = CProp_Portal::FindPortal( pPortalgun->GetLinkageGroupID(), true );

					if ( pPortal && pPortal->IsActive() )
					{
						pPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, false );
						pPortal->Fizzle();
						pPortal->EmitSound( "Portal.FizzlerShimmy" );
						// HACK HACK! Used to make the gun visually change when going through a cleanser!
						pPortalgun->m_fEffectsMaxSize2 = 50.0f;

						bFizzledPortal = true;
					}
				}

				if ( bFizzledPortal )
				{
					pPortalgun->SendWeaponAnim( ACT_VM_FIZZLE );

					pPortalgun->SetLastFiredPortal( 0 );
					m_OnFizzle.FireOutput( pOther, this );
					pPlayer->RumbleEffect( RUMBLE_RPG_MISSILE, 0, RUMBLE_FLAG_RESTART );
				}
			}
		}

		return;
	}

	CBaseAnimating *pBaseAnimating = dynamic_cast<CBaseAnimating*>( pOther );

	if ( pBaseAnimating && !pBaseAnimating->IsDissolving() )
	{
		int i = 0;

		while ( g_pszPortalNonCleansable[ i ] )
		{
			if ( FClassnameIs( pBaseAnimating, g_pszPortalNonCleansable[ i ] ) )
			{
				// Don't dissolve non cleansable objects
				return;
			}

			++i;
		}

		// The portal weight box, used for puzzles in the portal mod is differentiated by its name
		// always being 'box'. We use special logic when the cleanser dissolves a box so this is a special output for it.
		if ( pBaseAnimating->NameMatches( "box" ) )
		{
			m_OnDissolveBox.FireOutput( pOther, this );
		}

		Vector vOldVel;
		AngularImpulse vOldAng;
		pBaseAnimating->GetVelocity( &vOldVel, &vOldAng );

		IPhysicsObject* pOldPhys = pBaseAnimating->VPhysicsGetObject();

		if ( pOldPhys && ( pOldPhys->GetGameFlags() & FVPHYSICS_PLAYER_HELD ) )
		{
			CPortal_Player *pPlayer = (CPortal_Player *)GetPlayerHoldingEntity( pBaseAnimating );
			if( pPlayer )
			{
				// Modify the velocity for held objects so it gets away from the player
				pPlayer->ForceDropOfCarriedPhysObjects( pBaseAnimating );

				pPlayer->GetAbsVelocity();
				vOldVel = pPlayer->GetAbsVelocity() + Vector( pPlayer->EyeDirection2D().x * 4.0f, pPlayer->EyeDirection2D().y * 4.0f, -32.0f );
			}
		}
		
        pBaseAnimating->OnFizzled();

		// Swap object with an disolving physics model to avoid touch logic
		CBaseEntity *pDisolvingObj = ConvertToSimpleProp( pBaseAnimating );
		if ( pDisolvingObj )
		{
			// Remove old prop, transfer name and children to the new simple prop
			pDisolvingObj->SetName( pBaseAnimating->GetEntityName() );
			UTIL_TransferPoseParameters( pBaseAnimating, pDisolvingObj );
			TransferChildren( pBaseAnimating, pDisolvingObj );
			pDisolvingObj->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
			pBaseAnimating->AddSolidFlags( FSOLID_NOT_SOLID );
			pBaseAnimating->AddEffects( EF_NODRAW );

			IPhysicsObject* pPhys = pDisolvingObj->VPhysicsGetObject();
			if ( pPhys )
			{
				pPhys->EnableGravity( false );

				Vector vVel = vOldVel;
				AngularImpulse vAng = vOldAng;

				// Disolving hurts, damp and blur the motion a little
				vVel *= 0.5f;
				vAng.z += 20.0f;

				pPhys->SetVelocity( &vVel, &vAng );
			}

			pBaseAnimating->AddFlag( FL_DISSOLVING );
			UTIL_Remove( pBaseAnimating );
		}
		
		CBaseAnimating *pDisolvingAnimating = dynamic_cast<CBaseAnimating*>( pDisolvingObj );
		if ( pDisolvingAnimating ) 
		{
			pDisolvingAnimating->Dissolve( "", gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
		}

		m_OnDissolve.FireOutput( pOther, this );
	}
}

void CTriggerPortalCleanser::FizzleBaseAnimating( CTriggerPortalCleanser *pFizzler, CBaseAnimating *pBaseAnimating )
{
	if ( pBaseAnimating && !pBaseAnimating->IsDissolving() )
	{
		int i = 0;

		while ( g_pszPortalNonCleansable[ i ] )
		{
			if ( FClassnameIs( pBaseAnimating, g_pszPortalNonCleansable[ i ] ) )
			{
				// Don't dissolve non cleansable objects
				return;
			}

			++i;
		}

		Vector vOldVel;
		AngularImpulse vOldAng;
		pBaseAnimating->GetVelocity( &vOldVel, &vOldAng );

		IPhysicsObject* pOldPhys = pBaseAnimating->VPhysicsGetObject();

		if ( pOldPhys && ( pOldPhys->GetGameFlags() & FVPHYSICS_PLAYER_HELD ) )
		{
			CPortal_Player *pPlayer = (CPortal_Player *)GetPlayerHoldingEntity( pBaseAnimating );
			if( pPlayer )
			{
				// Modify the velocity for held objects so it gets away from the player
				pPlayer->ForceDropOfCarriedPhysObjects( pBaseAnimating );

				pPlayer->GetAbsVelocity();
				vOldVel = pPlayer->GetAbsVelocity() + Vector( pPlayer->EyeDirection2D().x * 4.0f, pPlayer->EyeDirection2D().y * 4.0f, -32.0f );
			}
		}

		// Swap object with an disolving physics model to avoid touch logic
		
        pBaseAnimating->OnFizzled();

		CBaseEntity *pDisolvingObj = ConvertToSimpleProp( pBaseAnimating );
		if ( pDisolvingObj )
		{
			// Remove old prop, transfer name and children to the new simple prop
			pDisolvingObj->SetName( pBaseAnimating->GetEntityName() );
			UTIL_TransferPoseParameters( pBaseAnimating, pDisolvingObj );
			TransferChildren( pBaseAnimating, pDisolvingObj );
			pDisolvingObj->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
			pBaseAnimating->AddSolidFlags( FSOLID_NOT_SOLID );
			pBaseAnimating->AddEffects( EF_NODRAW );

			IPhysicsObject* pPhys = pDisolvingObj->VPhysicsGetObject();
			if ( pPhys )
			{
				pPhys->EnableGravity( false );

				Vector vVel = vOldVel;
				AngularImpulse vAng = vOldAng;

				// Disolving hurts, damp and blur the motion a little
				vVel *= 0.5f;
				vAng.z += 20.0f;

				pPhys->SetVelocity( &vVel, &vAng );
			}

			pBaseAnimating->AddFlag( FL_DISSOLVING );
			UTIL_Remove( pBaseAnimating );
		}
		
		CBaseAnimating *pDisolvingAnimating = dynamic_cast<CBaseAnimating*>( pDisolvingObj );
		if ( pDisolvingAnimating ) 
		{
			pDisolvingAnimating->Dissolve( "", gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
		}
	}
}

void CTriggerPortalCleanser::InputFizzleTouchingPortals( inputdata_t &inputdata )
{
	FizzleTouchingPortals();
}