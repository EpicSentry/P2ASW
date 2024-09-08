#include "cbase.h"
#include "env_portal_laser.h"
#include "prop_portal_shared.h"
#include "portal_util_shared.h"
#include "player.h" 
#include "particle_parse.h"
#include "ieffects.h"
#include "util_shared.h"
#include "prop_weightedcube.h"
#include "point_laser_target.h"
#include "physicsshadowclone.h"
#include "soundenvelope.h"
#include "recipientfilter.h"
#include "mathlib/mathlib.h"
#include "portal_player.h"

#include "tier1/utlsortvector.h"

#undef FCVAR_DEVELOPMENTONLY
#define FCVAR_DEVELOPMENTONLY FCVAR_NONE

ConVar portal_laser_normal_update( "portal_laser_normal_update", "0.05f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar portal_laser_high_precision_update( "portal_laser_high_precision_update", "0.03f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_debug_laser( "sv_debug_laser", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar new_portal_laser( "new_portal_laser", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_laser_cube_autoaim( "sv_laser_cube_autoaim", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_laser_tight_box( "sv_laser_tight_box", "1.25f", FCVAR_DEVELOPMENTONLY );

ConVar reflector_cube_disable_when_on_laser( "reflector_cube_disable_when_on_laser", "1", FCVAR_DEVELOPMENTONLY, "If the reflector cube should get disabled when left on the ground with a laser going through it." );

ConVar hitbox_damage_enabled( "hitbox_damage_enabled", "0", FCVAR_DEVELOPMENTONLY, "Enable/disable hitbox damage." ); // FIXME: IDA shows that the flags are "8194"!!
ConVar sv_player_collide_with_laser( "sv_player_collide_with_laser", "1", FCVAR_DEVELOPMENTONLY ); // FIXME: IDA shows that the flags are "0x4000"!!

	
// Best guess on my part, find the correct name for this later...
typedef CUtlSortVector<LaserVictimInfo_t, CLaserVictimLess> LaserVictimSortVector_t;

BEGIN_DATADESC(CPortalLaser)

	DEFINE_KEYFIELD( m_bNoPlacementHelper, FIELD_BOOLEAN, "NoPlacementHelper" ),
	DEFINE_KEYFIELD( m_bStartOff, FIELD_BOOLEAN, "StartState" ),
	DEFINE_KEYFIELD( m_bIsLethal, FIELD_BOOLEAN, "LethalDamage" ),
	DEFINE_KEYFIELD( m_bAutoAimEnabled, FIELD_BOOLEAN, "AutoAimEnabled" ),
	
	DEFINE_FIELD( m_bShouldSpark, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bUseParentDir, FIELD_BOOLEAN ),

	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

	DEFINE_THINKFUNC( StrikeThink )

END_DATADESC()

LINK_ENTITY_TO_CLASS( env_portal_laser, CPortalLaser );

IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_PortalLaser)

	SendPropEHandle( SENDINFO( m_hReflector ) ),
	
	SendPropVector( SENDINFO( m_vStartPoint ) ),
	SendPropVector( SENDINFO( m_vEndPoint ) ),
	
	SendPropBool( SENDINFO( m_bLaserOn ) ),
	SendPropBool( SENDINFO( m_bIsLethal ) ),
	SendPropBool( SENDINFO( m_bIsAutoAiming ) ),
	SendPropBool( SENDINFO( m_bShouldSpark ) ),
	SendPropBool( SENDINFO( m_bUseParentDir ) ),
	
	SendPropVector( SENDINFO( m_angParentAngles ) ),

END_SEND_TABLE()

IMPLEMENT_AUTO_LIST( IPortalLaserAutoList );

#define	MASK_PORTAL_LASER (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_DEBRIS|CONTENTS_HITBOX)

float LaserDamageAmount()
{
	return 150.0;
}

CPortalLaser::CPortalLaser( void )
{
	m_pParentLaser = NULL;
	m_pChildLaser = NULL;
	m_bFromReflectedCube = false;
	m_bAutoAimEnabled = true;
	m_bNoPlacementHelper = false;
	
	m_hReflector = NULL;
	
	m_bLaserOn = false;
	m_bIsLethal = false;

	m_bIsAutoAiming = false;
	m_bShouldSpark = false;

	m_vStartPoint = vec3_origin;
	m_vEndPoint = vec3_origin;
}

CPortalLaser::~CPortalLaser( void )
{

}

void CPortalLaser::Spawn( void )
{
	//CPortalPlayerInventory::ValidateInventoryPositions((vgui::ListViewPanel *)this);
	BaseClass::Spawn();
	
	const char *pszModelName = GetModelName().ToCStr();
	if (!pszModelName)
		pszModelName = "";

	m_bGlowInitialized = false;

	Precache();

	if ( !m_bFromReflectedCube )
	{
		if (*pszModelName)
		{
			SetModel( pszModelName );
		}
		else
		{
			SetModel( "models/props/laser_emitter.mdl" );
		}

		CollisionProp()->SetSolid( SOLID_VPHYSICS );
		m_iLaserAttachment = LookupAttachment( "laser_attachment" );
		if ( !m_iLaserAttachment )
		{
			const char *pszName = GetEntityNameAsCStr();		
			
			if (!pszName)
				pszName = "";

			Warning("env_portal_laser '%s' : model named '%s' does not have attachment 'laser_attachment'\n", pszName, pszModelName);
		}

		CreateVPhysics();

	}
	for ( int i = 0; i != MAX_PLAYERS; ++i )
		m_pAmbientSound[i] = NULL;

	CreateHelperEntities();

	if ( !m_bStartOff )
		TurnOn();

	SetFadeDistance( -1.0, 0.0 );
	SetGlobalFadeScale( 0.0 );
}

void CPortalLaser::Activate( void )
{
	CreateHelperEntities();
	BaseClass::Activate();
}

void CPortalLaser::Precache( void )
{
	if ( m_bIsLethal )
		PrecacheScriptSound("LaserGreen.BeamLoop");
	else
		PrecacheScriptSound("Laser.BeamLoop");

	PrecacheScriptSound("Flesh.LaserBurn");
	PrecacheScriptSound("Player.PainSmall");

	PrecacheParticleSystem("laser_start_glow");
	PrecacheParticleSystem("reflector_start_glow");

	if ( !m_bFromReflectedCube )
	{
		const char *pszModel = GetModelName().ToCStr();
		if ( *pszModel )
			PrecacheModel( pszModel );
		else
			PrecacheModel("models/props/laser_emitter.mdl");
	}
}

void CPortalLaser::UpdateOnRemove( void )
{
	if ( m_pPlacementHelper )
		UTIL_Remove( m_pPlacementHelper );

	for ( int i = 0; i < MAX_PLAYERS; ++i)
	{
		if ( m_pSoundProxy[i] )
			UTIL_Remove( m_pSoundProxy[i] );
	}
	
	TurnOff();
	BaseClass::UpdateOnRemove();
}

bool CPortalLaser::CreateVPhysics( void )
{
	VPhysicsInitStatic();
	return true;
}

void CPortalLaser::CreateSoundProxies( void )
{		
	for ( int i = 0; i < MAX_PLAYERS; ++i )
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
		if (pPlayer && pPlayer->IsConnected() && !pPlayer->IsSplitScreenPlayer())
		{
			if (!m_pSoundProxy[i])
			{
				m_pSoundProxy[i] = CreateEntityByName("info_target");
				
				m_pSoundProxy[i]->SetAbsOrigin( GetAbsOrigin() );
				m_pSoundProxy[i]->AddEFlags( 0x80u );
				m_pSoundProxy[i]->DispatchUpdateTransmitState();
				DispatchSpawn( m_pSoundProxy[i] );
			}

			CBaseEntity *pSoundProxy = m_pSoundProxy[i];

			if (!m_pSoundProxy[MAX_PLAYERS])
			{
				CSingleUserRecipientFilter filter( pPlayer );

				CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
				if ( m_bIsLethal )
				{
					m_pAmbientSound[i] = controller.SoundCreate( filter, pSoundProxy->entindex(), "LaserGreen.BeamLoop" );
				}
				else
				{

					if ( pSoundProxy )
						m_pAmbientSound[i] = controller.SoundCreate( filter, pSoundProxy->entindex(), "Laser.BeamLoop" );
					else
						m_pAmbientSound[i] = controller.SoundCreate( filter, 0, "Laser.BeamLoop");
				}

				controller.Play( m_pAmbientSound[i], 1.0, 100.0, 0 );
			}
		}
	}
}

void CPortalLaser::UpdateSoundPosition( Vector &vecStart, Vector &vecEnd )
{
	for ( int i = 1; i < MAX_PLAYERS; ++i)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
		if (!pPlayer || !pPlayer->IsConnected())
		{						
			continue;
		}

		Vector vecPlayer = pPlayer->EyePosition();

		Vector vecNearestPoint;
		CalcClosestPointOnLineSegment( vecPlayer, vecStart, vecEnd, vecNearestPoint );

		if ( ComputeVolume( vecPlayer, m_vecNearestSoundSource[i] ) > ComputeVolume( vecPlayer, vecNearestPoint ) ||
			m_vecNearestSoundSource[i] == vec3_invalid )
		{
			m_vecNearestSoundSource[i] = vecNearestPoint;
		}
	}
}

int CPortalLaser::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

void CPortalLaser::CreateHelperEntities( void )
{
	if ( !m_bNoPlacementHelper && !m_pPlacementHelper )
	{
		m_pPlacementHelper = (CInfoPlacementHelper *)CreateEntityByName( "info_placement_helper" );

		m_pPlacementHelper->SetAbsOrigin( GetAbsOrigin() );
		m_pPlacementHelper->SetAbsAngles( GetAbsAngles() );

		m_pPlacementHelper->KeyValue("radius", "16");
		m_pPlacementHelper->KeyValue("hide_until_placed", "0");

		DispatchSpawn( m_pPlacementHelper );
	}
	
	CreateSoundProxies();
}

void CPortalLaser::TurnOff( void )
{
	m_bLaserOn = false;

	RemoveChildLaser();
	TurnOffGlow();
	TurnOffLaserSound();
	SetNextThink( NO_THINK_CONTEXT );
	SetThink( NULL );
}

void CPortalLaser::TurnOn( void )
{
	m_bLaserOn = true;

	if ( m_pfnThink == NULL )
	{
		SetThink( &CPortalLaser::StrikeThink );

		float flThinkRate;
		if ( m_bFromReflectedCube )
			flThinkRate = portal_laser_normal_update.GetFloat();
		else
			flThinkRate = portal_laser_high_precision_update.GetFloat();
		
		SetNextThink( flThinkRate + gpGlobals->curtime );
	}
}

void CPortalLaser::RemoveChildLaser( void )
{
	if ( m_pChildLaser )
	{
		m_pChildLaser->m_pParentLaser = NULL;
		UTIL_Remove( m_pChildLaser );
		m_pChildLaser = NULL;
	}
}

void CPortalLaser::TurnOffGlow( void )
{
	m_bGlowInitialized = false;

	if ( !m_bFromReflectedCube )
	{
		StopParticleEffects( this );
	}
	else
	{
		StopParticleEffects( m_hReflector );
	}
}

void CPortalLaser::TurnOffLaserSound( void )
{
	for ( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if ( m_pAmbientSound[i] )
		{
			CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

			controller.SoundDestroy( m_pAmbientSound[i] );
			m_pAmbientSound[i] = NULL;
		}
	}
}

void CPortalLaser::StrikeThink(void)
{
	VPROF_BUDGET( "CPortalLaser::StrikeThink", VPROF_BUDGETGROUP_GAME );
	Vector vecDir;
	Vector vecOrigin;

	m_bShouldSpark = false;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		m_vecNearestSoundSource[i] = vec3_invalid;
	}
	
	if ( m_hReflector && m_bFromReflectedCube )
	{
		if (UTIL_GetSchrodingerTwin(m_hReflector) )
		{
			m_bUseParentDir = true;

			m_angParentAngles = m_pParentLaser->m_angPortalExitAngles;

			AngleVectors(m_angParentAngles, &vecDir);
		}
		else
		{
			m_bUseParentDir = false;
			AngleVectors( m_hReflector->GetAbsAngles(), &vecDir );
		}
	
		vecOrigin = ( vecDir * 22.0 ) + m_hReflector->WorldSpaceCenter();

		CPortalSimulator *pSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity( m_hReflector );
		if ( pSimulator )
		{
			if ( pSimulator->EntityIsInPortalHole( m_hReflector )
				&& ( DotProduct( vecOrigin, pSimulator->GetInternalData().Placement.PortalPlane.m_Normal ) -
				pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
			{
				if ((DotProduct( m_hReflector->WorldSpaceCenter(), pSimulator->GetInternalData().Placement.PortalPlane.m_Normal ) -
					pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
				{
					vecDir = pSimulator->GetInternalData().Placement.matThisToLinked * vecDir;
					UTIL_Portal_PointTransform( pSimulator->GetInternalData().Placement.matThisToLinked, vecOrigin, vecOrigin );
				}
			}
		}

		SetNextThink( gpGlobals->curtime + portal_laser_high_precision_update.GetFloat() );
	}
	else
	{
		GetAttachment( m_iLaserAttachment, vecOrigin, &vecDir );
		SetNextThink( gpGlobals->curtime + portal_laser_normal_update.GetFloat() );
	}

	UTIL_Portal_Laser_Prevent_Tilting(vecDir);
	TurnOnGlow();

	FireLaser( vecOrigin, vecDir, m_hReflector);
	CreateSoundProxies();

	for ( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if ( m_pSoundProxy[i] )
			UTIL_SetOrigin( m_pSoundProxy[i], m_vecNearestSoundSource[i] );
	}

	if (sv_debug_laser.GetBool())
		engine->Con_NPrintf( 0, "num lasers = %d", AutoList().Count());
}

Vector CPortalLaser::ClosestPointOnLineSegment( const Vector &vPos )
{
	Vector vResult;
	CalcClosestPointOnLineSegment( vPos, m_vStartPoint, m_vEndPoint, vResult );	
	return vResult;
}

void CPortalLaser::InputToggle( inputdata_t &inputdata)
{
	if ( m_pfnThink != NULL)
		TurnOff();
	else
		TurnOn();
}

void CPortalLaser::InputTurnOn( inputdata_t &inputdata )
{
	if ( m_pfnThink == NULL )
		TurnOn();
}

void CPortalLaser::InputTurnOff( inputdata_t &inputdata )
{
	if ( m_pfnThink != NULL )
		TurnOff();
}

void CPortalLaser::TurnOnGlow(void)
{
	if (!m_bGlowInitialized)
	{
		m_bGlowInitialized = true;

		if (!UTIL_IsSchrodinger(m_hReflector))
		{
			if (m_bFromReflectedCube)
			{
				DispatchParticleEffect("reflector_start_glow", PATTACH_ABSORIGIN_FOLLOW, m_hReflector, 0, 1, -1, 0, 1);
			}
			else
			{
				DispatchParticleEffect("laser_start_glow", PATTACH_POINT_FOLLOW, this, m_iLaserAttachment, 1, -1, 0, 1);
			}
		}
	}
}

void CPortalLaser::FireLaser( Vector &vecStart, Vector &vecDirection, CPropWeightedCube *pReflector )
{
	trace_t tr;
	Vector vecNewTermPoint;
	Vector vDir;
	Vector vecStartPos;
	float flOtherBeamLength = 0.0;
	Ray_t ray;
	if ( new_portal_laser.GetInt() )
	{
		Vector vecDirection_0;
		PortalLaserInfoList_t infoList;

		float flTotalBeamLength = 0.0;

		bool bAutoAimDisabled = !m_bAutoAimEnabled;
		vecDirection_0 = vecDirection;
		
		CBaseEntity *pTracedTarget = TraceLaser( true, vecStart, vecDirection_0, flTotalBeamLength, tr, infoList, &vecStartPos);
		bool bAutoAimSuccess = false;
		if ( !bAutoAimDisabled )
		{
			if ( ShouldAutoAim(pTracedTarget) )
			{
				vDir = ( ((vecDirection * flTotalBeamLength) + vecStart) + vecStartPos ) - vecStart;

				VectorNormalize( vDir );
				memset(&vecNewTermPoint, 0, sizeof(vecNewTermPoint));
				//vec_t v43 = 0.0;
				bAutoAimSuccess = false;
				if ( pTracedTarget == TraceLaser( false, vecStart, vDir, flOtherBeamLength, tr, infoList, false ) )
				{
					ray.Init( tr.startpos, tr.endpos );

					flTotalBeamLength = flOtherBeamLength;
					vecDirection_0 = vDir;

					DamageEntitiesAlongLaser( infoList, true );
					bAutoAimSuccess = true;
				}
				infoList.RemoveAll();
				if ( vecNewTermPoint.z < 0.0 )
				{
					//v43 = vecNewTermPoint.x;
				}
				else
				{
					vecNewTermPoint.x = 0.0;
					vecNewTermPoint.y = 0.0;
					//v43 = 0.0;
				}
			}
			infoList.RemoveAll();
		}

		if ( bAutoAimDisabled || !bAutoAimSuccess )
		{
			memset(&vDir, 0, sizeof(vDir));
			//vec_t vDirX = 0.0;
			UTIL_ClearTrace( tr );

			PortalLaserInfoList_t hitInfoList;

			pTracedTarget = TraceLaser( false, vecStart, vecDirection_0, flTotalBeamLength, tr, hitInfoList, 0);
			DamageEntitiesAlongLaser( hitInfoList, false );

			hitInfoList.RemoveAll();

			if ( vDir.z >= 0.0 )
			{
				vDir.x = 0.0;
				vDir.y = 0.0;
				//vDirX = 0.0;

				//bAutoAimDisabled = ( vecStart.x == m_vStartPoint.m_Value.x );
				//bAutoAimSuccess = false;
				//goto LABEL_SKIPAUTOAIM;
			}
			bAutoAimSuccess = false;
			//vDirX = vDir.x;
		}
		bAutoAimDisabled = ( vecStart.x == m_vStartPoint.m_Value.x );
		
		m_vStartPoint = vecStart;

		m_vEndPoint = ( (vecDirection_0 * flTotalBeamLength) + vecStart );

		m_bIsAutoAiming = bAutoAimSuccess;
		if ( tr.m_pEnt )
		{
			CBaseEntity *pHitEntity = tr.m_pEnt;
			if ( CPhysicsShadowClone::IsShadowClone( pHitEntity ) )
			{
				pHitEntity = ((CPhysicsShadowClone*)(pHitEntity))->GetClonedEntity();
			}
			CPropWeightedCube *pSchrodingerTwin = UTIL_GetSchrodingerTwin(pHitEntity);
			if ( pSchrodingerTwin )
				pHitEntity = pSchrodingerTwin;
		}
		if ( !tr.m_pEnt || !ReflectLaserFromEntity( tr.m_pEnt ) )
		{
			RemoveChildLaser();
			if ( !pTracedTarget || !pTracedTarget->ClassMatches( "point_laser_target" ) )
			{
				if ( m_pPlacementHelper )
					UTIL_SetOrigin( m_pPlacementHelper, tr.endpos );
				BeamDamage( tr );
			}
		}
	}
	else // UNDONE: Use the old laser method Portal 2 had
	{
		CTraceFilterSimpleClassnameList traceFilter( pReflector, COLLISION_GROUP_NONE );
	
		traceFilter.AddClassnameToIgnore("projected_wall_entity");
		traceFilter.AddClassnameToIgnore("player");
		traceFilter.AddClassnameToIgnore("point_laser_target");

		ray.Init( vecStart, vecStart + ( vecDirection * MAX_TRACE_LENGTH ) );
		enginetrace->TraceRay( ray, MASK_PORTAL_LASER, &traceFilter, &tr );

		UpdateSoundPosition( tr.startpos, tr.endpos );
		flOtherBeamLength = 0.0;
	
		CPortal_Base2D *pFirstPortal;
		if ( UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, 0)
			&& flOtherBeamLength != 0.0
			&& pFirstPortal->IsActivedAndLinked() )
		{
			if ( StrikeEntitiesAlongLaser( tr.startpos, tr.endpos, &vDir ) )
			{
				m_vStartPoint = vecStart;
				m_vEndPoint = vDir;
			}
			else
			{
				m_vStartPoint = vecStart;
				m_vEndPoint = tr.endpos;
			
				BeamDamage( tr );

				Ray_t rayTransformed;
				rayTransformed.m_pWorldAxisTransform = 0;
				UTIL_Portal_RayTransform( pFirstPortal->m_matrixThisToLinked, ray, rayTransformed );
				Vector delta = rayTransformed.m_Delta;
				VectorNormalize( delta );
				UTIL_Portal_PointTransform( pFirstPortal->m_matrixThisToLinked, tr.endpos, vecStartPos );
				UpdateNextLaser( vecStartPos, delta, 0 );
			}
			return;
		}
		if ( StrikeEntitiesAlongLaser( tr.startpos, tr.endpos, &vecNewTermPoint ) )
		{
			m_vStartPoint = vecStart;
			m_vEndPoint = vecNewTermPoint;
			return;
		}

		m_vStartPoint = vecStart;
		m_vEndPoint = tr.endpos;

		CBaseEntity *pHitEntity = tr.m_pEnt;
		if ( pHitEntity )
		{
			if ( CPhysicsShadowClone::IsShadowClone( pHitEntity ) )
			{
				pHitEntity = ((CPhysicsShadowClone*)pHitEntity)->GetClonedEntity();
			}
			CPropWeightedCube *pTwin = UTIL_GetSchrodingerTwin( pHitEntity );
			if ( pTwin )
				pHitEntity = pTwin;
			if ( ReflectLaserFromEntity( pHitEntity ) )
			{
				return;
			}
		}
		RemoveChildLaser();
	
		if ( m_pPlacementHelper )
			UTIL_SetOrigin( m_pPlacementHelper, tr.endpos );
		BeamDamage( tr );
	}
}

CBaseEntity *CPortalLaser::TraceLaser( bool bIsFirstTrace, Vector &vecStart, Vector &vecDirection, float &flTotalBeamLength, trace_t &tr, PortalLaserInfoList_t &infoList, Vector *pVecAutoAimOffset )
{
	CBaseEntity *pHitEntity;

	flTotalBeamLength = 0.0;

	Vector vStart = vecStart;

	Vector vDir = vecDirection;

	CTraceFilterSimpleClassnameList traceFilter( m_hReflector, COLLISION_GROUP_NONE );
	traceFilter.AddClassnameToIgnore("projected_wall_entity");
	traceFilter.AddClassnameToIgnore("player");
	traceFilter.AddClassnameToIgnore("point_laser_target");

	CUtlVector<CPortal_Base2D*> portalList;
	Vector vAutoAimOffset;

	do
	{
		Vector vStrike;
		while (1)
		{
			UTIL_ClearTrace(tr);
		
			Ray_t ray;
			ray.Init( vStart, vStart + ( MAX_TRACE_LENGTH * vDir ) );			
			enginetrace->TraceRay( ray, MASK_PORTAL_LASER, &traceFilter, &tr );

			UpdateSoundPosition( tr.startpos, tr.endpos );

			CPortal_Base2D *pFirstPortal = NULL;
			if (!UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, NULL)
				|| !pFirstPortal
				|| !pFirstPortal->IsActivedAndLinked())
			{
				break;
			}
			pHitEntity = GetEntitiesAlongLaser( tr.startpos, tr.endpos, &vAutoAimOffset, infoList, bIsFirstTrace );
			CalcClosestPointOnLineSegment(vAutoAimOffset, tr.startpos, tr.endpos, vStrike, 0);
			flTotalBeamLength = sqrt(
				(((vStrike.x - tr.startpos.x)
				* (vStrike.x - tr.startpos.x))
				+ ((vStrike.y - tr.startpos.y)
				* (vStrike.y - tr.startpos.y)))
				+ ((vStrike.z - tr.startpos.z) * (vStrike.z - tr.startpos.z)))
				+ flTotalBeamLength;
			if (pHitEntity)
			{
				if (pVecAutoAimOffset)
				{
					*pVecAutoAimOffset = pHitEntity->WorldSpaceCenter() - vStrike;
				}
				goto LABEL_25;
			}
			Ray_t rayTransformed;
			UTIL_Portal_RayTransform(pFirstPortal->m_matrixThisToLinked, ray, rayTransformed);
			vDir = rayTransformed.m_Delta;
			VectorNormalize(vDir);
			UTIL_Portal_PointTransform(pFirstPortal->m_matrixThisToLinked, tr.endpos, vStart);

			portalList.AddToHead( pFirstPortal->m_hLinkedPortal );
		}
		pHitEntity = GetEntitiesAlongLaser( tr.startpos, tr.endpos, &vStrike, infoList, bIsFirstTrace );
		
		if (pHitEntity)
		{
			CalcClosestPointOnLineSegment( vStrike, tr.startpos, tr.endpos, vAutoAimOffset );
			flTotalBeamLength = sqrt(
				(((vAutoAimOffset.x - tr.startpos.x)
				* (vAutoAimOffset.x - tr.startpos.x))
				+ ((vAutoAimOffset.y - tr.startpos.y)
				* (vAutoAimOffset.y - tr.startpos.y)))
				+ ((vAutoAimOffset.z - tr.startpos.z)
				* (vAutoAimOffset.z - tr.startpos.z)))
				+ flTotalBeamLength;
			if (pVecAutoAimOffset)
			{
				goto GT_AUTOAIMOFFSET;
			}
			goto LABEL_25;
		}

		flTotalBeamLength = sqrt(
			(
			((tr.endpos.x - tr.startpos.x)
			* (tr.endpos.x - tr.startpos.x))
			+ ((tr.endpos.y - tr.startpos.y)
			* (tr.endpos.y - tr.startpos.y))
			+ ((tr.endpos.z - tr.startpos.z)
			* (tr.endpos.z - tr.startpos.z))
			))
			+ flTotalBeamLength;

	} while (UTIL_Paint_Reflect(tr, vStart, vDir, REFLECT_POWER));

	if ( ShouldAutoAim( tr.m_pEnt ) )
	{
		if (CPhysicsShadowClone::IsShadowClone(tr.m_pEnt))
		{
			pHitEntity =((CPhysicsShadowClone *)tr.m_pEnt)->GetClonedEntity();
		}
		else
		{
			pHitEntity = tr.m_pEnt;
		}
		if (pVecAutoAimOffset)
		{
			CalcClosestPointOnLine( pHitEntity->WorldSpaceCenter(), tr.startpos, tr.endpos, vAutoAimOffset );
		GT_AUTOAIMOFFSET:
			*pVecAutoAimOffset = pHitEntity->WorldSpaceCenter() - vAutoAimOffset;
		}
	}
LABEL_25:
	if ( ShouldAutoAim( pHitEntity ) && pVecAutoAimOffset )
	{
		vAutoAimOffset = *pVecAutoAimOffset;
		for ( int i = 0; i < portalList.Count(); ++i )
		{
			UTIL_Portal_VectorTransform( portalList[i++]->m_matrixThisToLinked, vAutoAimOffset, vAutoAimOffset );
		}
		*pVecAutoAimOffset = vAutoAimOffset;
	}
	m_angPortalExitAngles = GetAbsAngles();
	for ( int i = portalList.Count()-1; i != -1; --i )
	{
		UTIL_Portal_AngleTransform( portalList[i]->m_hLinkedPortal->MatrixThisToLinked(), m_angPortalExitAngles, m_angPortalExitAngles );
	}
	return pHitEntity;
}

void CPortalLaser::UpdateNextLaser( Vector &vecStart, Vector &vecDirection, CPropWeightedCube *pReflector )
{
	if ( IPortalLaserAutoList::AutoList().Count() < 30 )
	{
		if (m_pChildLaser)
		{
			m_pChildLaser->m_hReflector->SetLaser( NULL );
			m_pChildLaser->TurnOffGlow();
			m_pChildLaser->m_hReflector = pReflector;
			m_pChildLaser->m_bFromReflectedCube = true;
			m_pChildLaser->TurnOnGlow();
			pReflector->SetLaser( m_pChildLaser );
		}
		else
		{
			m_pChildLaser = (CPortalLaser *)CreateEntityByName( "env_portal_laser" );

			m_pChildLaser->m_hReflector = pReflector;
			m_pChildLaser->m_bFromReflectedCube = true;
			pReflector->SetLaser( m_pChildLaser );
			m_pChildLaser->TurnOnGlow();
			m_pChildLaser->m_pParentLaser = this;			
			m_pChildLaser->m_bUseParentDir = UTIL_GetSchrodingerTwin( pReflector ) != NULL;
			m_pChildLaser->m_angParentAngles = m_angParentAngles;

			DispatchSpawn( m_pChildLaser );
		}
		m_pChildLaser->m_bAutoAimEnabled = m_bAutoAimEnabled;

		m_pChildLaser->FireLaser(vecStart, vecDirection, pReflector);
	}
}

void CPortalLaser::DamageEntitiesAlongLaser( const PortalLaserInfoList_t &infoList, bool bAutoAim )
{
	bool bBlockTarget = false;
	for ( int i = 0; i < infoList.Count(); ++i )
	{
		const PortalLaserInfo_t *laserInfo = &infoList[i];
		Vector vecDirection = laserInfo->vecEnd - laserInfo->vecStart;
		VectorNormalize( vecDirection );

		int count = laserInfo->sortedEntList.Count();
		for ( int j = 0; j < count; ++j )
		{
			CBaseEntity *pEntity = laserInfo->sortedEntList[j];
			if ( sv_debug_laser.GetBool() )
			{
				ICollideable *pCollideable = pEntity->GetCollideable();
				if ( pCollideable )
				{
					NDebugOverlay::BoxAngles( pCollideable->GetCollisionOrigin(), pCollideable->OBBMins(), pCollideable->OBBMaxs(), pCollideable->GetCollisionAngles(), 255, 255, 0, 0, 0.1 );
				}
			}

			if ( pEntity )
			{
				if ( pEntity->ClassMatches( "point_laser_target" ) && !bBlockTarget )
				{
					if ( !static_cast<CPortalLaserTarget*>( pEntity )->IsTerminalPoint() || !m_bAutoAimEnabled || bAutoAim || m_bFromReflectedCube )
					{
						DamageEntity( pEntity, 1.0);
					}
				}
				else if ( pEntity->ClassMatches( "npc_portal_turret_floor" ) )
				{
					bBlockTarget = true;
				}
				else if ( pEntity->IsPlayer() && pEntity->GetMoveType() != MOVETYPE_NOCLIP && ( pEntity->GetGroundEntity() || fabs(vecDirection.z) >= 0.2 ) )
				{
					Vector vecPlayerVelocity = pEntity->GetAbsVelocity();
					VectorNormalize( vecPlayerVelocity );

					Vector vecNearestPoint;
					CalcClosestPointOnLineSegment( pEntity->GetAbsOrigin(), laserInfo->vecStart, laserInfo->vecEnd, vecNearestPoint );

					Vector vecPlayerPos = pEntity->GetAbsOrigin();
					Vector vecLineToLaser = vecPlayerPos - vecNearestPoint;
					vecLineToLaser.z = 0;
					VectorNormalize( vecLineToLaser );
					vecLineToLaser.z = 0;

					Vector vecBounce;
					bool bDoDamage = false;
					if ( vecPlayerVelocity.LengthSqr() < 1.4210855e-14 )
					{
						vecBounce.x = vecDirection.y;
						vecBounce.y = -vecDirection.x;
						vecBounce.z = 0;
						bDoDamage = true;
					}
					else
					{
						float dot = (vecPlayerVelocity.x * vecLineToLaser.x) + (vecPlayerVelocity.y * vecLineToLaser.y);
						vecBounce = ((vecLineToLaser * -2.0) * dot) + vecPlayerVelocity;
						vecBounce.z = vecPlayerVelocity.z;
						VectorNormalize( vecBounce );
						vecBounce.z = 0;
						bDoDamage = (vecBounce.x * vecLineToLaser.x) + (vecBounce.y * vecLineToLaser.y) >= 0.0;
					}

					if ( bDoDamage )
					{
						Vector vecPushVelocity = vecBounce * 100;
						if ( ((pEntity->GetFlags() & FL_ONGROUND) != 0) )
						{
							pEntity->SetGroundEntity( NULL );
							pEntity->SetGroundChangeTime( gpGlobals->curtime + 0.5 );
							vecPushVelocity *= 2;
						}
						pEntity->SetAbsVelocity( vecPushVelocity );

						DamageEntity( pEntity, m_bIsLethal ? 100000 : LaserDamageAmount() );
						pEntity->EmitSound( "Flesh.LaserBurn" );
						pEntity->EmitSound( "Player.PainSmall" );
					}
				}
			}
		}
	}
}

CBaseEntity *CPortalLaser::GetEntitiesAlongLaser( Vector &vecStart, Vector &vecEnd, Vector *pVecOut, PortalLaserInfoList_t &infoList, bool bIsFirstTrace )
{
	infoList.AddToTail();
	PortalLaserInfo_t *info = &infoList[infoList.Count() - 1];
	info->vecStart = vecStart;
	info->vecEnd = vecEnd;

	float extents;
	if ( bIsFirstTrace )
	{
		extents = (sqrt( DotProduct( vecStart, vecEnd ) ) * (1 / 256)) + 16.0;
	}
	else
	{
		extents = sv_laser_tight_box.GetFloat();
	}

	Vector vecMins( -extents, -extents, -extents );
	Vector vecMaxs( extents, extents, extents );

	QAngle angNearest;
	Vector vecDirection = vecEnd - vecStart;

	if (sv_debug_laser.GetInt())
	{
		VectorAngles( vecDirection, angNearest );
		if ( bIsFirstTrace )
		{
			NDebugOverlay::SweptBox( vecStart, vecEnd, vecMins, vecMaxs, angNearest, 255, 0, 0, 0, 0.1 );
		}
		else
		{
			NDebugOverlay::SweptBox( vecStart, vecEnd, vecMins, vecMaxs, angNearest, 0, 255, 0, 0, 0.1 );
		}
	}
	Ray_t ray;
	ray.Init( vecStart, vecEnd, vecMins, vecMaxs );

	CBaseEntity *list[512];
	CFlaggedEntitiesEnum flagEnts( list, 512, FL_NPC | FL_CLIENT | FL_OBJECT );

	int count = UTIL_EntitiesAlongRay( ray, &flagEnts );

	LaserVictimSortVector_t vsrtVictims;
	memset( &vsrtVictims, 0, sizeof(vsrtVictims) );

	for ( int i = 0; i < count; ++i )
	{
		CBaseEntity *pEntity = list[i];
		if ((pEntity
			&& (pEntity->ClassMatches("point_laser_target")
			|| pEntity->ClassMatches("npc_portal_turret_floor")
			|| pEntity->IsPlayer())
			&& pEntity->IsAlive()))
		{
			float flFraction;
			Vector vecNearest;
			CalcClosestPointOnLineSegment( pEntity->WorldSpaceCenter(), vecStart, vecEnd, vecNearest, &flFraction );
			if ((!pEntity->IsPlayer() || sv_player_collide_with_laser.GetInt())
				&& flFraction > 0.0)
			{
				LaserVictimInfo_t victim;
				victim.flFraction = flFraction;
				victim.pVictim = pEntity;
				vsrtVictims.InsertNoSort( victim );
			}
		}
	}
	vsrtVictims.RedoSort( true );

	bool bBlockTarget = false;
	for ( int j = 0; j < vsrtVictims.Count(); ++j )
	{
		CBaseEntity *pVictim = vsrtVictims[j].pVictim;
		info->sortedEntList.AddToTail( pVictim );

		if (pVictim != NULL)
		{
			if ( pVictim->ClassMatches("point_laser_target") && !bBlockTarget )
			{
				CPortalLaserTarget *pTarget = dynamic_cast<CPortalLaserTarget*>( pVictim );
				if ( pTarget && pTarget->IsTerminalPoint() )
				{
					if ( pVecOut )
						*pVecOut = pTarget->WorldSpaceCenter();

					return pVictim;
				}
			}
			else if ( pVictim->ClassMatches( "npc_portal_turret_floor") )
			{
				bBlockTarget = true;
			}
		}
	}

	if ( pVecOut )
		*pVecOut = vecEnd;

	return NULL;
}

bool CPortalLaser::ShouldAutoAim( CBaseEntity *pEntity )
{
	if ( !m_bAutoAimEnabled )
		return false;
	
	if ( !pEntity || !pEntity->ClassMatches( "point_laser_target" ) )
	{
		return false;
	}

	CPortalLaserTarget *pTarget = dynamic_cast<CPortalLaserTarget*>( pEntity );

	bool bTerminalPoint = pTarget && pTarget->IsTerminalPoint();
	if ( !m_bFromReflectedCube )
		return bTerminalPoint;

	return ( sv_laser_cube_autoaim.GetInt() || g_pGameRules->IsMultiplayer() ) && bTerminalPoint;
}

bool CPortalLaser::IsOn( void )
{
	return m_pfnThink != NULL;
}

bool CPortalLaser::ReflectLaserFromEntity( CBaseEntity *pReflector )
{
	if ( pReflector == m_hReflector )
	{
		if (m_pChildLaser)
		{
			m_pChildLaser->m_pParentLaser = NULL;
			UTIL_Remove( m_pChildLaser );
			m_pChildLaser = NULL;
		}
		return true;
	}
	
	if ( pReflector && ( UTIL_IsReflectiveCube(pReflector) || UTIL_IsSchrodinger(pReflector) ) )
	{
		CPropWeightedCube *pCastedReflector = static_cast<CPropWeightedCube*>( pReflector );

		if ( pCastedReflector->HasLaser() )
		{
			if ( pCastedReflector->GetLaser() != m_pChildLaser )
				RemoveChildLaser();
		}
		else
		{
			Vector vecForward;
			if ( UTIL_GetSchrodingerTwin( pReflector ) )
			{
				AngleVectors( m_angPortalExitAngles, &vecForward );
			}
			else
			{
				QAngle reflectorAngles = pReflector->GetAbsAngles();
				AngleVectors(reflectorAngles, &vecForward);
			}

			Vector vecOffset = pReflector->WorldSpaceCenter() + (vecForward * 22.0);
			CPortalSimulator *pSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity( pReflector );
			if ( pSimulator )
			{
				if ( pSimulator->EntityIsInPortalHole( pReflector )
					&& ( DotProduct( vecOffset, pSimulator->GetInternalData().Placement.PortalPlane.m_Normal ) -
					pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
				{
					if ((DotProduct( pReflector->WorldSpaceCenter(), pSimulator->GetInternalData().Placement.PortalPlane.m_Normal ) -
						pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
					{
						vecForward = pSimulator->GetInternalData().Placement.matThisToLinked * vecForward;
						UTIL_Portal_PointTransform( pSimulator->GetInternalData().Placement.matThisToLinked, vecOffset, vecOffset );
					}
				}
			}
			UTIL_Portal_Laser_Prevent_Tilting(vecForward);
			UpdateNextLaser( vecOffset, vecForward, pCastedReflector );
		}
		if (!pCastedReflector->IsMovementDisabled()
			&& reflector_cube_disable_when_on_laser.GetInt()
			&& pCastedReflector->ShouldEnterDisabledState())
		{
			pCastedReflector->EnterDisabledState();
		}
		return true;
	}

	return false;
}

void CPortalLaser::BeamDamage( trace_t &tr )
{
	if (tr.fraction != 1.0)
	{
		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity)
		{
			ClearMultiDamage();
			Vector vecCenter = tr.endpos - GetAbsOrigin();
			VectorNormalize( vecCenter );
			CTakeDamageInfo info( this, this, gpGlobals->frametime * LaserDamageAmount(), DMG_BURN );
			CalculateMeleeDamageForce( &info, vecCenter, tr.endpos, 1.0 );
			pEntity->DispatchTraceAttack(info, vecCenter, &tr );
			ApplyMultiDamage();

			CBaseAnimating *pAnimating = pEntity->GetBaseAnimating();

			if ( pAnimating && ( (pAnimating->ClassMatches("npc_portal_turret_floor"))
				|| pAnimating->ClassMatches("npc_hover_turret") ) )
			{
				if (hitbox_damage_enabled.GetInt())
				{
					CTakeDamageInfo turretdmginfo;
					turretdmginfo.SetDamage( 1.0 );
					turretdmginfo.SetDamageType( DMG_CRUSH );

					pAnimating->Event_Killed( turretdmginfo );
					pAnimating->SetThink( NULL );
				}
				else
				{
					pAnimating->Ignite( 30.0, true ); 
				}
			}
			else if ( pEntity->GetMoveType() == MOVETYPE_VPHYSICS )
			{
				CPhysicsProp *pPhysProp = dynamic_cast<CPhysicsProp*>(pEntity);
				if (pPhysProp)
					pPhysProp->Ignite( 30.0, false );
			}
			m_bShouldSpark = true;
		}
	}
}

void CPortalLaser::DamageEntity( CBaseEntity *pVictim, float flAmount )
{
	CTakeDamageInfo info( this, this, flAmount * gpGlobals->frametime, DMG_BURN, 0 );
	Vector vecMeleeDir( 1.0, 0, 0 );
	
	CalculateMeleeDamageForce( &info, vecMeleeDir, pVictim->WorldSpaceCenter(), 1.0 );
	pVictim->TakeDamage(info);
}

bool CPortalLaser::StrikeEntitiesAlongLaser( Vector &vecStart, Vector &vecEnd, Vector *pVecOut )
{
	Vector vecDirection = vecEnd - vecStart;
	float v33 = (sqrt(((vecDirection.x * vecDirection.x) + (vecDirection.y * vecDirection.y)) + (vecDirection.z * vecDirection.z))
		* (1/256))
		+ 16.0;
	VectorNormalize(vecDirection);

	Ray_t ray;
	float extents = v33;

	Vector Mins( -extents, -extents, -extents );
	Vector Maxs( extents, extents, extents );

	ray.Init(vecStart, vecEnd, Mins, Maxs);

	CBaseEntity *list[512];
	CFlaggedEntitiesEnum entEnum( list, 512, FL_NPC | FL_CLIENT | FL_OBJECT );

	int count = UTIL_EntitiesAlongRay( ray, &entEnum );

	Vector vecPlayerVelocity;
	LaserVictimSortVector_t vsrtVictims;
	memset(&vsrtVictims, 0, 25);
	if (count > 0)
	{
		for ( int i = 0; i < count; ++i)
		{
			CBaseEntity *pEntity = list[i];
			if ((pEntity && (pEntity->ClassMatches("point_laser_target")) || pEntity->ClassMatches("npc_portal_turret_floor"))
				|| pEntity->IsPlayer()
				&& pEntity->IsAlive() )
			{
				float flFraction;
				CalcClosestPointOnLineSegment( pEntity->GetAbsOrigin(), vecStart, vecEnd, vecPlayerVelocity, &flFraction );
				float v28 = ((vecPlayerVelocity.x - pEntity->GetAbsOrigin().x)
					* (vecPlayerVelocity.x - pEntity->GetAbsOrigin().x))
					+ ((vecPlayerVelocity.y - pEntity->GetAbsOrigin().y)
					* (vecPlayerVelocity.y - pEntity->GetAbsOrigin().y));
				if (v28 <= 256.0 || m_bFromReflectedCube)
				{
					Vector HullMaxs;

					if (!pEntity->IsPlayer())
						goto LABEL_60;
					if (sv_player_collide_with_laser.GetInt())
					{
						int v32;
						if (pEntity->GetGroundEntity()
							|| (v32 = pEntity->WorldSpaceCenter().z,
							( (v32 + 8) - vecPlayerVelocity.z) < 0.0))
						{
							CPortal_Player *pPlayer = (CPortal_Player*)pEntity;

							HullMaxs = pPlayer->GetHullMaxs();

							if (vecPlayerVelocity.z <= (pEntity->GetAbsOrigin().z + HullMaxs.z) && v28 <= 256.0)
							{
							LABEL_60:
								if (flFraction > 0.0)
								{
									LaserVictimInfo_t tempInfo;
									tempInfo.pVictim = pEntity;
									tempInfo.flFraction = flFraction;

									vsrtVictims.InsertNoSort( tempInfo );
								}
							}
						}
					}
				}
			}
		}
	}
	vsrtVictims.RedoSort( true );

	bool bBestIsTurreta;

	if (vsrtVictims.Count() > 0)
	{
		bBestIsTurreta = false;
		for (int i = 0; i < vsrtVictims.Count(); ++i)
		{
			CBaseEntity *pVictim = vsrtVictims[i].pVictim;
			if (!pVictim)
				goto LABEL_NULLVICTIM;
			if ((pVictim->ClassMatches("point_laser_target"))
				&& !bBestIsTurreta)
			{
				DamageEntity( pVictim, 1.0 );
				CPortalLaserTarget *pLaserTarget = dynamic_cast<CPortalLaserTarget*>(pVictim);
				if ( pLaserTarget && pLaserTarget->IsTerminalPoint() ) // Was pVictim[933]...
				{
					// Note; WorldSpaceCenter() is just a best guess!
					if (pVecOut)
						*pVecOut = pLaserTarget->WorldSpaceCenter(); //*(Vector *)(*(int(__cdecl **)(_BYTE *))(*(_DWORD *)v25 + 600))(v25);
					return true;
				}
			}
			
			if ( pVictim->ClassMatches( "npc_portal_turret_floor" ) )
			{
				bBestIsTurreta = true;
				continue;
			}
			else
			{
			LABEL_NULLVICTIM: // This makes no sense... if the victim is NULL, wouldn't it crash if we tried to test for IsPlayer()???
				float flBounceX;
				float flBounceZ;
				float flBounceY;
				// NOTE: IsPlayer() is just a best guess!
				if ( pVictim->IsPlayer() //(*((unsigned __int8(__cdecl **)(CBaseEntity *))pVictim->_vptr_IHandleEntity + 86))(pVictim)
					&& pVictim->GetMoveType() != MOVETYPE_NOCLIP)
				{
					Vector vecNearestPoint;
					vecPlayerVelocity = pVictim->GetAbsVelocity();
					VectorNormalize(vecPlayerVelocity);
					CalcClosestPointOnLineSegment( pVictim->GetAbsOrigin(), vecStart, vecEnd, vecNearestPoint, 0 );
					
					Vector vecLineToLaser;
					vecLineToLaser.y = pVictim->GetAbsOrigin().y - vecNearestPoint.y;
					vecLineToLaser.x = pVictim->GetAbsOrigin().x - vecNearestPoint.x;
					vecLineToLaser.z = vecNearestPoint.z - vecNearestPoint.z;
					VectorNormalize(vecLineToLaser);
					vecLineToLaser.z = 0.0;
					Vector vecBounce;
					if ((((vecPlayerVelocity.x * vecPlayerVelocity.x)
						+ (vecPlayerVelocity.y * vecPlayerVelocity.y))
						+ (vecPlayerVelocity.z * vecPlayerVelocity.z)) < 1.4210855e-14)
					{
						vecBounce.z = (0.0 * vecDirection.x) - (0.0 * vecDirection.y);
						flBounceX = vecDirection.y - (0.0 * vecDirection.z); // Bad code?
						flBounceZ = vecBounce.z * 100.0;
						vecBounce.x = flBounceX;
						flBounceY = (0.0 * vecDirection.z) - vecDirection.x;
						vecBounce.y = flBounceY;
					LABEL_PLAYERPUSH:
						Vector vecPushVelocity;
						vecPushVelocity.z = flBounceZ;
						vecPushVelocity.x = flBounceX * 100.0;
						vecPushVelocity.y = 100.0 * flBounceY;
						
						if ((pVictim->GetFlags() & FL_DUCKING) != 0)
						{
							pVictim->SetGroundEntity( NULL );
							pVictim->SetGroundChangeTime( gpGlobals->curtime + 0.5 );
							vecPushVelocity = vecPushVelocity + vecPushVelocity;
						}
						pVictim->SetAbsVelocity( vecPushVelocity );
						int v23 = 1125515264;
						if (m_bIsLethal)
							v23 = 1203982336;

						DamageEntity(pVictim, *(float *)&v23);
						pVictim->EmitSound("Flesh.LaserBurn");
						pVictim->EmitSound("Player.PainSmall");
						continue;
					}
					float v31 = ((vecPlayerVelocity.x * vecLineToLaser.x) + (vecPlayerVelocity.y * vecLineToLaser.y));
					vecBounce.x = ((vecLineToLaser.x * -2.0) * v31) + vecPlayerVelocity.x;
					vecBounce.y = ((-2.0 * vecLineToLaser.y) * v31) + vecPlayerVelocity.y;
					vecBounce.z = vecPlayerVelocity.z;
					VectorNormalize(vecBounce);
					flBounceX = vecBounce.x;
					vecBounce.z = 0.0;
					flBounceY = vecBounce.y;
					if ((((vecLineToLaser.x * vecBounce.x) + (vecLineToLaser.y * vecBounce.y))
						+ (0.0 * vecLineToLaser.z)) >= 0.0)
					{
						flBounceZ = 0.0;
						goto LABEL_PLAYERPUSH;
					}
				}
			}
		}
	}
	if (pVecOut)
		*pVecOut = vecEnd;

	return false;
}