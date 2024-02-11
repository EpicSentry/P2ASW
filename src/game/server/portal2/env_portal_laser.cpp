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
	DEFINE_KEYFIELD( m_bLaserOn, FIELD_BOOLEAN, "StartState" ),
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

#define	MASK_PORTAL_LASER (CONTENTS_SOLID|CONTENTS_MONSTER)

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
		if ( GetModelName().ToCStr() )
			PrecacheModel( GetModelName().ToCStr() );
		else
			PrecacheModel("models/props/laser_emitter.mdl");
	}
}

void CPortalLaser::UpdateOnRemove( void )
{
	if ( m_pPlacementHelper )
		UTIL_Remove( m_pPlacementHelper );
	for ( int i = 0; i != MAX_PLAYERS; ++i)
	{
		if (m_pSoundProxy[i])
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
	for (int i = 0; i < MAX_PLAYERS; ++i)
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

				//pSoundProxy[33] = (CBaseEntity *)m_pSoundProxy;

				controller.Play( m_pAmbientSound[i], 1.0, 100.0, 0 );
			}
		}
	}
}

void CPortalLaser::UpdateSoundPosition( Vector &vecStart, Vector &vecEnd )
{
	Vector vEyePosition;
	Vector vecNearestPoint;
	int i;
	for ( i = 1;; ++i)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
		if (!pPlayer || !pPlayer->IsConnected())
		{						
			goto PLAYER_TEST;
		}

		vEyePosition = pPlayer->EyePosition();		
		
		CalcClosestPointOnLineSegment( vEyePosition, vecStart, vecEnd, vecNearestPoint, 0 );
		
		if ( ( ( vEyePosition.x - m_vecNearestSoundSource[i].x ) * ( vEyePosition.x - m_vecNearestSoundSource[i].x )
			+ ( vEyePosition.y - m_vecNearestSoundSource[i].y ) * ( vEyePosition.y - m_vecNearestSoundSource[i].y )
			+ ( vEyePosition.z - m_vecNearestSoundSource[i].z ) * ( vEyePosition.z - m_vecNearestSoundSource[i].z ) )
				> 
			( ( vEyePosition.x - vecNearestPoint.x) * (vEyePosition.x - vecNearestPoint.x )
			+ ( vEyePosition.y - vecNearestPoint.y) * (vEyePosition.y - vecNearestPoint.y )
			+ ( vEyePosition.z - vecNearestPoint.z) * (vEyePosition.z - vecNearestPoint.z ) ) )
	
			goto SET_MEMBERS;
		if (m_vecNearestSoundSource[i].x == vec3_invalid.x && m_vecNearestSoundSource[i].y == vec3_invalid.y)
			break;
	PLAYER_TEST:
		if (i == (MAX_PLAYERS-1))
			return;
	LOOP_END:
		;
	}
	if (m_vecNearestSoundSource[i].z == vec3_invalid.z)
	{
	SET_MEMBERS:
		m_vecNearestSoundSource[i].x = vecNearestPoint.x;
		m_vecNearestSoundSource[i].y = vecNearestPoint.y;
		m_vecNearestSoundSource[i].z = vecNearestPoint.z;
		goto PLAYER_TEST;
	}
	if (i != (MAX_PLAYERS-1))
		goto LOOP_END;
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
	if (m_pChildLaser)
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
	VPROF("CPortalLaser::StrikeThink");
	Vector vecDir;
	Vector vecOrigin;

	m_bShouldSpark = false;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		m_vecNearestSoundSource[i].x = vec3_invalid.x;
		m_vecNearestSoundSource[i].y = vec3_invalid.y;
		m_vecNearestSoundSource[i].z = vec3_invalid.z;
	}
	
	if ( !m_hReflector || !m_bFromReflectedCube )
	{
		GetAttachment( m_iLaserAttachment, vecOrigin, &vecDir, 0, 0 );
		SetNextThink(gpGlobals->curtime + portal_laser_normal_update.GetFloat(), 0);
		goto LABEL_11;
	}

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
	if (pSimulator)
	{
		if (pSimulator->EntityIsInPortalHole( m_hReflector )
			&& ((((vecOrigin.x * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.x)
			+ (vecOrigin.y * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.y))
			+ (vecOrigin.z * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.z))
			- pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
		{
			if ((((m_hReflector->WorldSpaceCenter().x * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.x)
				+ (m_hReflector->WorldSpaceCenter().y *pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.y))
				+ ((m_hReflector->WorldSpaceCenter().z * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.z))
				- pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
			{
#if 0
				float v37 = pSimulator->GetInternalData().Placement.matThisToLinked.m[1][1];
				float v22 = pSimulator->GetInternalData().Placement.matThisToLinked.m[1][0];
				float v36 = pSimulator->GetInternalData().Placement.matThisToLinked.m[1][2];
				float v23 = pSimulator->GetInternalData().Placement.matThisToLinked.m[0][0];
				float v24 = pSimulator->GetInternalData().Placement.matThisToLinked.m[0][1];
				float v25 = pSimulator->GetInternalData().Placement.matThisToLinked.m[0][2];
				float v35 = pSimulator->GetInternalData().Placement.matThisToLinked.m[2][0];
				float v34 = pSimulator->GetInternalData().Placement.matThisToLinked.m[2][1];
				float v33 = pSimulator->GetInternalData().Placement.matThisToLinked.m[2][2];
				vecOrigin.y = (((vecOrigin.x * v22) + (vecOrigin.y * v37))
					+ (vecOrigin.z * v36))
					+ pSimulator->GetInternalData().Placement.matThisToLinked.m[1][3];
				vecOrigin.z = ( ((v35 * vecOrigin.x) + (vecOrigin.y * v34)) + (vecOrigin.z * v33) ) + pSimulator->GetInternalData().Placement.matThisToLinked.m[2][3];
				vecOrigin.x = ( (v23 * vecOrigin.x) + ( vecOrigin.y * v24 ) + (vecOrigin.z * v25) ) + pSimulator->GetInternalData().Placement.matThisToLinked.m[0][3];
				vecDir.x = ((v23 * vecDir.x) + (v24 * vecDir.y)) + (v25 * vecDir.z);
				vecDir.z = ((vecDir.x * v35) + (vecDir.y * v34)) + (vecDir.z * v33);
				vecDir.y = (v22 * vecDir.x + (v37 * vecDir.y)) + (v36 * vecDir.z);
#else
				//UNDONE: Which should we use?
#if 0
				vecDir = pSimulator->GetInternalData().Placement.matThisToLinked * vecDir;
#else
				UTIL_Portal_PointTransform( pSimulator->GetInternalData().Placement.matThisToLinked, vecDir, vecDir );
#endif

				vecOrigin = pSimulator->GetInternalData().Placement.matThisToLinked * vecOrigin;
#endif
			}
		}
	}

	SetNextThink( gpGlobals->curtime + portal_laser_high_precision_update.GetFloat() );

LABEL_11:
	UTIL_Portal_Laser_Prevent_Tilting(vecDir);
	TurnOnGlow();
	FireLaser( vecOrigin, vecDir, m_hReflector);
	CreateSoundProxies();

	for ( int i = 0; i != MAX_PLAYERS; ++i )
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
	Vector vecDirection_0;
	float flOtherBeamLength = 0.0;
	Ray_t ray;
	if ( new_portal_laser.GetInt() )
	{
		PortalLaserInfoList_t infoList;

		float flTotalBeamLength = 0.0;

		bool bAutoAimDisabled = !m_bAutoAimEnabled;
		vecDirection_0 = vecDirection;
		if ( bAutoAimDisabled )
			goto LABEL_AUTOAIMDISABLED;
		
		CBaseEntity *pTracedTarget = TraceLaser( true, vecStart, vecDirection_0, flTotalBeamLength, tr, infoList, &vecStartPos);
		bool bAutoAimSuccess = false;
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

		if ( !bAutoAimSuccess )
		{
		LABEL_AUTOAIMDISABLED:
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
	// This label seems unnecessary...
	//LABEL_SKIPAUTOAIM:
		
		m_vStartPoint = vecStart;

		m_vEndPoint = ( (vecDirection_0 * flTotalBeamLength) + vecStart );

		m_bIsAutoAiming = bAutoAimSuccess;
		if ( !tr.m_pEnt )
			goto LABEL_NOENTITY;

		CBaseEntity *pHitEntity = tr.m_pEnt;
		if ( CPhysicsShadowClone::IsShadowClone( pHitEntity ) )
		{
			pHitEntity = ((CPhysicsShadowClone*)(pHitEntity))->GetClonedEntity();
		}
		CPropWeightedCube *pSchrodingerTwin = UTIL_GetSchrodingerTwin(pHitEntity);
		if ( pSchrodingerTwin )
			pHitEntity = pSchrodingerTwin;
		if ( !ReflectLaserFromEntity( pHitEntity ) )
		{
		LABEL_NOENTITY:
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
		enginetrace->TraceRay( ray, 1174421505, &traceFilter, &tr );

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
				vecDirection_0 = rayTransformed.m_Delta;
				VectorNormalize(vecDirection_0);
				UTIL_Portal_PointTransform( pFirstPortal->m_matrixThisToLinked, tr.endpos, vecStartPos );
				UpdateNextLaser( vecStartPos, vecDirection_0, 0 );
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

	Vector vEntOrigin1; // Stupid init and gotos!!
	do
	{
		Vector vStrike_0;
		while (1)
		{
			UTIL_ClearTrace(tr);
		
			Ray_t ray;
			ray.Init( vStart, vStart + ( MAX_TRACE_LENGTH * vDir ) );
			enginetrace->TraceRay( ray, 1174421505, &traceFilter, &tr );

			UpdateSoundPosition( tr.startpos, tr.endpos );

			CPortal_Base2D *pFirstPortal = NULL;
			if (!UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, NULL)
				|| !pFirstPortal
				|| !pFirstPortal->IsActivedAndLinked())
			{
				break;
			}
			pHitEntity = GetEntitiesAlongLaser( tr.startpos, tr.endpos, vAutoAimOffset, infoList, bIsFirstTrace );
			CalcClosestPointOnLineSegment(vAutoAimOffset, tr.startpos, tr.endpos, vStrike_0, 0);
			flTotalBeamLength = sqrt(
				(((vStrike_0.x - tr.startpos.x)
				* (vStrike_0.x - tr.startpos.x))
				+ ((vStrike_0.y - tr.startpos.y)
				* (vStrike_0.y - tr.startpos.y)))
				+ ((vStrike_0.z - tr.startpos.z) * (vStrike_0.z - tr.startpos.z)))
				+ flTotalBeamLength;
			if (pHitEntity)
			{
				if (pVecAutoAimOffset)
				{
					Vector v28 = pHitEntity->WorldSpaceCenter(); // NOTE: This is just a best guess!

					*pVecAutoAimOffset = v28 - vStrike_0;
				}
				goto LABEL_25;
			}
			Ray_t rayTransformed;
			UTIL_Portal_RayTransform(pFirstPortal->m_matrixThisToLinked, ray, rayTransformed);
			vDir = rayTransformed.m_Delta;
			VectorNormalize(vDir);
			UTIL_Portal_PointTransform(pFirstPortal->m_matrixThisToLinked, tr.endpos, vStart);

			CPortal_Base2D *v13 = pFirstPortal->m_hLinkedPortal;
			portalList.InsertBefore( 0, v13 );
		}
		pHitEntity = GetEntitiesAlongLaser( tr.startpos, tr.endpos, vStrike_0, infoList, bIsFirstTrace );
		
		if (pHitEntity)
		{
			CalcClosestPointOnLineSegment(vStrike_0, tr.startpos, tr.endpos, vAutoAimOffset, 0);
			flTotalBeamLength = sqrt(
				(((vAutoAimOffset.x - tr.startpos.x)
				* (vAutoAimOffset.x - tr.startpos.x))
				+ ((vAutoAimOffset.y - tr.startpos.y)
				* (vAutoAimOffset.y - tr.startpos.y)))
				+ ((vAutoAimOffset.z - tr.startpos.z)
				* (vAutoAimOffset.z - tr.startpos.z)))
				+ flTotalBeamLength;
			if (pVecAutoAimOffset)
				goto GT_AUTOAIMOFFSET;
			goto LABEL_25;
		}
		float v17 = tr.endpos.z - tr.startpos.z;
		flTotalBeamLength = sqrt(
			(((tr.endpos.x - tr.startpos.x)
			* (tr.endpos.x - tr.startpos.x))
			+ ((tr.endpos.y - tr.startpos.y)
			* (tr.endpos.y - tr.startpos.y)))
			+ (v17 * v17))
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
			vEntOrigin1 = pHitEntity->WorldSpaceCenter(); // NOTE: This is just a best guess!
			CalcClosestPointOnLine( vEntOrigin1, tr.startpos, tr.endpos, vAutoAimOffset );
		GT_AUTOAIMOFFSET:
			Vector vEntOrigin2 = pHitEntity->WorldSpaceCenter(); // NOTE: This is just a best guess!

			*pVecAutoAimOffset = vEntOrigin2 - vAutoAimOffset;
		}
	}
LABEL_25:
	if ( ShouldAutoAim( pHitEntity ) && pVecAutoAimOffset )
	{
		vAutoAimOffset = *pVecAutoAimOffset;
		if (portalList.Count() > 0)
		{
			int i = 0;
			do
				UTIL_Portal_VectorTransform(
				portalList[i++]->m_matrixThisToLinked,
				vAutoAimOffset,
				vAutoAimOffset);
			while (portalList.Count() > i);
		}
		*pVecAutoAimOffset = vAutoAimOffset;
	}
	m_angPortalExitAngles = GetAbsAngles();
	if (portalList.Count() - 1 >= 0)
	{
		int v23 = portalList.Count() - 1;
		do
		{
			UTIL_Portal_AngleTransform( portalList[v23]->m_hLinkedPortal->MatrixThisToLinked(), m_angPortalExitAngles, m_angPortalExitAngles );
			--v23;
		} while (v23 != -1);
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
	float x; // xmm4_4
	float y; // xmm3_4
	Vector vecEnd; // [esp+4Ch] [ebp-7Ch]
	Vector vecDirection;
	Vector vecNearestPoint;
	Vector vecBounce;
	Vector vecPlayerVelocity;
	Vector vecLineToLaser;
	Vector vecPlayerPos;

	if (infoList.Count() > 0)
	{
		int i = 0;
		bool bBlockTarget = false;

		PortalLaserInfo_t v3;
		while (1)
		{
			v3 = infoList[i];
			vecEnd = v3.vecEnd;
			vecDirection = v3.vecEnd - v3.vecStart;
			VectorNormalize(vecDirection);
			if (v3.sortedEntList.Count() > 0)
				break;
		LABEL_29:
			if (infoList.Count() <= ++i)
				return;
		}
		Vector vecStart = v3.vecStart;

		float v10;
		
		CBaseEntity *list[1024];

#if 1
		const int FLAGS = FL_NPC | FL_CLIENT | FL_OBJECT;
#else
		const int FLAGS = 33562752;
#endif
		CFlaggedEntitiesEnum rayEnum( list, 1024, FLAGS );

		Ray_t ray;
		ray.Init( vecStart, vecEnd );

		int nCount = UTIL_EntitiesAlongRay( ray, &rayEnum );
		
		if ( nCount == 0 ) // p2asw: HACK HACK!!
			return;

		int j = 0;

		while (1)
		{
			if ( j == nCount ) // p2asw: HACK HACK!!
				break;
			CBaseEntity *pEntity = list[j];

			if ( !pEntity ) // p2asw: HACK HACK!!
				goto LABEL_25;

			if ( sv_debug_laser.GetInt() )
			{	
				NDebugOverlay::BoxAngles( vecStart, pEntity->CollisionProp()->OBBMins(), pEntity->CollisionProp()->OBBMaxs(), pEntity->CollisionProp()->GetCollisionAngles(), 255, 255, 0, 0, 0.1);
			}
			
			vecPlayerVelocity = pEntity->GetAbsVelocity();

			//if (pEntity) // Useless check? Investigate later
			{
				if ( pEntity->ClassMatches("point_laser_target") && !bBlockTarget )
				{
					if ( /* !*(_BYTE *)(pEntity + 933) || */ !m_bAutoAimEnabled || bAutoAim || m_bFromReflectedCube)
						DamageEntity( pEntity, 1.0);
					goto LABEL_25;
				}
				if ( pEntity->ClassMatches( "npc_portal_turret_floor" ) )
				{
					bBlockTarget = true;
					goto LABEL_25;
				}
			}

			// FIXME: This might be important!!
			//if (!(*(unsigned __int8(__cdecl **)(int))(*(_DWORD *)pEntity + 344))(pEntity) || *(_BYTE *)(pEntity + 226) == 8)
			//	goto LABEL_25;
			
			if (!pEntity->GetGroundEntity())
				break;

			VectorNormalize(vecPlayerVelocity);

			vecPlayerPos = pEntity->GetAbsOrigin();

			CalcClosestPointOnLineSegment(vecPlayerPos, vecStart, vecEnd, vecNearestPoint, 0);

			vecLineToLaser.x = vecPlayerPos.x - vecNearestPoint.x;
			vecLineToLaser.y = vecPlayerPos.y - vecNearestPoint.y;
			vecLineToLaser.z = 0.0;
			VectorNormalize(vecLineToLaser);
			vecLineToLaser.z = 0.0;
			
			if ((((vecPlayerVelocity.x * vecPlayerVelocity.x)
				+ (vecPlayerVelocity.y * vecPlayerVelocity.y))
				+ (vecPlayerVelocity.z * vecPlayerVelocity.z)) < 1.4210855e-14)
			{
				
				x = vecDirection.y - (0.0 * vecDirection.z);
				y = (0.0 * vecDirection.z) - vecDirection.x;
				vecBounce.z = (vecDirection.x * 0.0) - (vecDirection.y * 0.0);
				v10 = vecBounce.z * 100.0;
				vecBounce.x = x;
				vecBounce.y = y;
			LABEL_20:
#if 0
				v11 = (*(_BYTE *)(pEntity + 212) & 1) == 0;
#else // NOTE: This is just a best guess!!
				bool v11 = ( pEntity->GetFlags() & FL_ONGROUND ) == 0;
#endif
				Vector vecPushVelocity;

				vecPushVelocity.z = v10;
				vecPushVelocity.x = x * 100.0;
				vecPushVelocity.y = 100.0 * y;
				if (!v11)
				{
					pEntity->SetGroundEntity( NULL );
					pEntity->SetGroundChangeTime( gpGlobals->curtime + 0.5 );
					vecPushVelocity = vecPushVelocity + vecPushVelocity;
				}
				pEntity->SetAbsVelocity( vecPushVelocity );
				int v12 = 1125515264;
				if ( m_bIsLethal )
					v12 = 1203982336;

				DamageEntity( pEntity, *(float *)&v12 );
				pEntity->EmitSound( "Flesh.LaserBurn" );
				pEntity->EmitSound( "Player.PainSmall" );
				goto LABEL_25;
			}
			float v17 = ((vecPlayerVelocity.x * vecLineToLaser.x) + (vecPlayerVelocity.y * vecLineToLaser.y)) + (vecPlayerVelocity.z * 0.0);

			vecBounce.x = ((vecLineToLaser.x * -2.0) * v17) + vecPlayerVelocity.x;
			vecBounce.y = ((vecLineToLaser.y * -2.0) * v17) + vecPlayerVelocity.y;
			vecBounce.z = (v17 * -0.0) + vecPlayerVelocity.z;
			VectorNormalize(vecBounce);
			x = vecBounce.x;
			y = vecBounce.y;
			vecBounce.z = 0.0;
			if ((((vecLineToLaser.x * vecBounce.x) + (vecLineToLaser.y * vecBounce.y))
				+ (vecLineToLaser.z * 0.0)) >= 0.0)
			{
				v10 = 0.0;
				goto LABEL_20;
			}
		LABEL_25:
			if ( nCount < ++j )
				goto LABEL_29;
		}

		if ( fabs(vecDirection.z) < 0.2 )
			goto LABEL_25;
	}
}

CBaseEntity *CPortalLaser::GetEntitiesAlongLaser( Vector &vecStart, Vector &vecEnd, Vector &vecOut, PortalLaserInfoList_t &infoList, bool bIsFirstTrace )
{
#if 1
	LaserVictimSortVector_t vsrtVictims; // [esp+8BCh] [ebp-34h] BYREF
	LaserVictimInfo_t victim;

	infoList.InsertBefore( infoList.Count() );
	Vector returnVector = vecEnd;
	PortalLaserInfo_t *pLaserInfo = &infoList[infoList.Count() - 1];
	pLaserInfo->vecStart = vecStart;
	pLaserInfo->vecEnd = vecEnd;

	float extents;
	if ( bIsFirstTrace )
	{
		extents = (sqrt( DotProduct( vecStart, vecEnd ) ) * 0.00390625) + 16.0;
	}
	else
	{
		extents = sv_laser_tight_box.GetFloat();
	}

	Vector vecMins( -extents, -extents, -extents );
	Vector vecMaxs( extents, extents, extents );
	
	PortalLaserInfo_t *info = &infoList.Element( infoList.Count() - 1 );

	QAngle angNearest;
	Vector vecDirection = vecEnd - vecStart;

	if (sv_debug_laser.GetInt())
	{
		VectorAngles( vecDirection, angNearest );
		if (bIsFirstTrace)
			NDebugOverlay::SweptBox( vecStart, vecEnd, vecMins, vecMaxs, angNearest, 255, 0, 0, 0, 0.1 );
		else
			NDebugOverlay::SweptBox( vecStart, vecEnd, vecMins, vecMaxs, angNearest, 0, 255, 0, 0, 0.1 );
	}
	Ray_t ray;
	ray.Init( vecStart, vecEnd, vecMins, vecMaxs );

#if 1
	const int FLAGS = FL_NPC | FL_CLIENT | FL_OBJECT;
#else
	const int FLAGS = 33562752;
#endif

	CBaseEntity *list[512];
	CFlaggedEntitiesEnum flagEnts( list, 512, FLAGS );

	int count = UTIL_EntitiesAlongRay( ray, &flagEnts );

	memset(&vecMaxs, 0, sizeof(vecMaxs));
	memset(&vsrtVictims, 0, 13);
	victim.flFraction = 0.0;
	victim.pVictim = NULL;
	float flFraction = 0.0;
	if (count > 0)
	{
		int i = 0;
		do
		{
			CBaseEntity *v16 = list[i];
			if ((v16
				&& (v16->ClassMatches( "point_laser_target" )
				|| v16->ClassMatches("npc_portal_turret_floor")
				|| v16->IsPlayer())
				&& v16->IsAlive() ) )
			{
				CalcClosestPointOnLineSegment( v16->WorldSpaceCenter(), vecStart, vecEnd, vecDirection, &flFraction );
				if ((!v16->IsPlayer() || sv_player_collide_with_laser.GetInt())
					&& flFraction > 0.0)
				{
					victim.flFraction = flFraction;
					victim.pVictim = v16;
					vsrtVictims.InsertNoSort( victim );
				}
			}
			++i;
		} while (i < count);
		vsrtVictims.RedoSort( true );
		returnVector = vecEnd;
	}
	bool bIsTurret = false;
	if (vsrtVictims.Count() > 0)
	{
		int j = 0;
		while (1)
		{
			CBaseEntity *pVictim = victim.pVictim;
			//float flVictimFraction = victim.flFraction;
			info->sortedEntList.AddToTail( pVictim );

			if (pVictim != NULL)
			{
				bool v25;
				if ( ( (v25 = pVictim->ClassMatches("point_laser_target"), flFraction = victim.flFraction, v25 )) && !bIsTurret)
				{
					CPortalLaserTarget *pTarget = dynamic_cast<CPortalLaserTarget*>( pVictim );

					if ( pTarget && pTarget->IsTerminalPoint() ) // IsTerminalPoint() was v26[909]...
					{
						vecOut = pTarget->WorldSpaceCenter();// FIXME!! Was: *(Vector *)(*(int(__thiscall **)(_BYTE *))(*(_DWORD *)v26 + 596))(v26);
						// FIXME!!!
#						if 0
							if (vecMaxs.z >= 0.0)
						{
							if (LODWORD(vecMaxs.x))
								_g_pMemAlloc->Free(_g_pMemAlloc, (void *)LODWORD(vecMaxs.x));
						}
#						endif
							return victim.pVictim;
					}
				}
				else if ( pVictim->ClassMatches( "npc_portal_turret_floor") )
				{
					bIsTurret = true;
				}
			}
			if (++j >= vsrtVictims.Count())
			{
				returnVector = vecEnd;
				break;
			}
		}
	}
	vecOut = returnVector;
#endif // #if 0/1
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

	// In the original function, it's a dynamic cast, but we're doing an assert_cast/static_cast instead
	// because there is already a check above for point_laser_target, saves a few cycles I suppose.
	CPortalLaserTarget *pTarget = assert_cast<CPortalLaserTarget*>(pEntity);

	bool bTerminalPoint = pTarget->IsTerminalPoint(); // Was pTarget[909], but wtf does this mean?
	if (!m_bFromReflectedCube)
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
	else if ( pReflector && ( UTIL_IsReflectiveCube(pReflector) || UTIL_IsSchrodinger(pReflector) ) )
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

			CPortalSimulator *pSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity(pReflector);
			//float *floatPortalSim = (float *)pSimulator;
			if (pSimulator)
			{

				if (pSimulator->EntityIsInPortalHole(pReflector)
					&& (((( vecOffset.x * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.x)
					+ (vecOffset.y * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.y))
					+ (vecOffset.z * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.z))
					- pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
				{
					Vector vCenter = pReflector->WorldSpaceCenter();
					if (((((vCenter.y * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.y) +
						(vCenter.x * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.x)) +
						(vCenter.z * pSimulator->GetInternalData().Placement.PortalPlane.m_Normal.z))
						- pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
					{
#if 0					// Based on previous code, it's save to conclude that this was all matrix transform code.
						v12 = (((floatPortalSim[31] * vecOffset.y) + (floatPortalSim[30] * vecOffset.x))
							+ (floatPortalSim[32] * vecOffset.z))
							+ floatPortalSim[33];
						v13 = (((floatPortalSim[38] * vecOffset.x) + (vecOffset.y * floatPortalSim[39]))
							+ (vecOffset.z * floatPortalSim[40]))
							+ floatPortalSim[41];
						vecOffset.y = (((floatPortalSim[34] * vecOffset.x) + (floatPortalSim[35] * vecOffset.y))
							+ (floatPortalSim[36] * vecOffset.z))
							+ floatPortalSim[37];
						vecOffset.z = v13;
						vecOffset.x = v12;
						v14 = ((floatPortalSim[35] * vecForward.y) + (floatPortalSim[34] * vecForward.x))
							+ (floatPortalSim[36] * vecForward.z);
						v15 = ((floatPortalSim[38] * vecForward.x) + (vecForward.y * floatPortalSim[39]))
							+ (vecForward.z * floatPortalSim[40]);
						vecForward.x = ((floatPortalSim[30] * vecForward.x) + (floatPortalSim[31] * vecForward.y))
							+ (floatPortalSim[32] * vecForward.z);
						vecForward.y = v14;
						vecForward.z = v15;
#else				
						//UNDONE: Which should we use?
#if 0
						vecForward = pSimulator->GetInternalData().Placement.matThisToLinked * vecForward;
#else
						UTIL_Portal_PointTransform( pSimulator->GetInternalData().Placement.matThisToLinked, vecForward, vecForward );
#endif

						vecOffset = pSimulator->GetInternalData().Placement.matThisToLinked * vecOffset;
#endif
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
	else
	{
		return false;
	}
}

float LaserDamageAmount()
{
	return 150.0;
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

			CTakeDamageInfo info( this, this, gpGlobals->frametime * LaserDamageAmount(), 8, 0 );
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
					turretdmginfo.SetDamageType( DMG_CRUSH ); // 1

					pAnimating->Event_Killed( turretdmginfo );
					pAnimating->SetThink( NULL );
				}
				else
				{
					// BEST GUESS!! MAY NOT BE ACCURATE!!
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
	CTakeDamageInfo info( this, this, flAmount * gpGlobals->frametime, 8, 0 );
	// Note: WorldSpaceCenter() is just a best guess!!
	Vector vCenter = pVictim->WorldSpaceCenter(); //(const Vector *)(*((int(__cdecl **)(CBaseEntity *))pVictim->_vptr_IHandleEntity + 150))(pVictim);

	Vector vecMeleeDir;
	vecMeleeDir.x = 1.0;
	vecMeleeDir.y = 0.0;
	vecMeleeDir.z = 0.0;

	CalculateMeleeDamageForce( &info, vecMeleeDir, vCenter, 1.0 );
	pVictim->TakeDamage(info);
}

bool CPortalLaser::StrikeEntitiesAlongLaser( Vector &vecStart, Vector &vecEnd, Vector *pVecOut)
{
	Vector vecDirection = vecEnd - vecStart;
	float v33 = (sqrt(((vecDirection.x * vecDirection.x) + (vecDirection.y * vecDirection.y)) + (vecDirection.z * vecDirection.z))
		* 0.00390625)
		+ 16.0;
	VectorNormalize(vecDirection);

	Ray_t ray;
#if 0 // Dumb decompiler code?
	float extents = (v33 + v33) * 0.5;
#else
	float extents = v33;
#endif

	Vector Mins( -extents, -extents, -extents );
	Vector Maxs( extents, extents, extents );

	ray.Init(vecStart, vecEnd, Mins, Maxs);

	CBaseEntity *list[512];
	CFlaggedEntitiesEnum entEnum( list, 512, 33562752 );

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
#if 0 // Keeping the old code here for now, just in case.
				float v28 = DotProduct( vecPlayerVelocity, pEntity->GetAbsOrigin() );
#else
				float v28 = ((vecPlayerVelocity.x - pEntity->GetAbsOrigin().x)
					* (vecPlayerVelocity.x - pEntity->GetAbsOrigin().x))
					+ ((vecPlayerVelocity.y - pEntity->GetAbsOrigin().y)
					* (vecPlayerVelocity.y - pEntity->GetAbsOrigin().y));
#endif
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
					float v31 = ((vecPlayerVelocity.x * vecLineToLaser.x) + (vecPlayerVelocity.y * vecLineToLaser.y))
						+ (0.0 * vecPlayerVelocity.z); // Bad Code?
					vecBounce.x = ((vecLineToLaser.x * -2.0) * v31) + vecPlayerVelocity.x;
					vecBounce.y = ((-2.0 * vecLineToLaser.y) * v31) + vecPlayerVelocity.y;
					vecBounce.z = (v31 * -0.0) + vecPlayerVelocity.z; // Bad Code?
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
	//vsrtVictims.m_Size = 0;
	//if (vsrtVictims.m_Memory.m_nGrowSize >= 0 && vsrtVictims.m_Memory.m_pMemory)
	//	(*(void(__cdecl **)(_DWORD, LaserVictimInfo_t *))(*g_pMemAlloc + 8))(g_pMemAlloc, vsrtVictims.m_Memory.m_pMemory);

	return false;
}