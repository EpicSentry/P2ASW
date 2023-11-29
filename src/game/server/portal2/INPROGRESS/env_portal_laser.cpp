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
ConVar sv_laser_cube_autoaim( "sv_laser_cube_autoaim", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
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
	return 1;
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
				CSingleUserRecipientFilter filter( pPlayer ); // [esp+24h] [ebp-28h] BYREF
				//filter.__vftable = (CSingleUserRecipientFilter_vtbl *)&CSingleUserRecipientFilter::`vftable';

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
	CBasePlayer *pPlayer; // eax
	Vector vEyePosition; // [esp+38h] [ebp-30h] BYREF
	Vector vecNearestPoint; // [esp+44h] [ebp-24h] BYREF

	int i;
	for ( i = 1;; ++i)
	{
		pPlayer = UTIL_PlayerByIndex( i );
		if (!pPlayer || !pPlayer->IsConnected())
		{						
			goto LABEL_3;
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
	
			goto LABEL_2;
		if (m_vecNearestSoundSource[i].x == vec3_invalid.x && m_vecNearestSoundSource[i].y == vec3_invalid.y)
			break;
	LABEL_3:
		if (i == (MAX_PLAYERS-1))
			return;
	LABEL_4:
		;
	}
	if (m_vecNearestSoundSource[i].z == vec3_invalid.z)
	{
	LABEL_2:
		m_vecNearestSoundSource[i].x = vecNearestPoint.x;
		m_vecNearestSoundSource[i].y = vecNearestPoint.y;
		m_vecNearestSoundSource[i].z = vecNearestPoint.z;
		goto LABEL_3;
	}
	if (i != (MAX_PLAYERS-1))
		goto LABEL_4;
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
	Vector vecDir; // [esp+58h] [ebp-30h] BYREF
	Vector vecOrigin; // [esp+64h] [ebp-24h] BYREF

	VPROF("CPortalLaser::StrikeThink");

	m_bShouldSpark = false;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		m_vecNearestSoundSource[i].x = vec3_invalid.x;
		m_vecNearestSoundSource[i].y = vec3_invalid.y;
		m_vecNearestSoundSource[i].z = vec3_invalid.z;
	}
	
	if ( !m_hReflector )
	{
	LABEL_10:
		GetAttachment( m_iLaserAttachment, vecOrigin, &vecDir, 0, 0 );
		SetNextThink(gpGlobals->curtime + portal_laser_normal_update.GetFloat(), 0);
		goto LABEL_11;
	}


	if (!m_hReflector || !m_bFromReflectedCube)
		goto LABEL_10;

	if (UTIL_GetSchrodingerTwin(m_hReflector) )
	{
		m_bUseParentDir = true;

		m_angParentAngles = m_pParentLaser->m_angPortalExitAngles;

		AngleVectors(m_angParentAngles, &vecDir);
	}
	else
	{
		m_bUseParentDir = false;
		AngleVectors(GetAbsAngles(), &vecDir);
	}
	
	vecOrigin = ( vecDir * 22.0 ) + m_hReflector->WorldSpaceCenter();

	CPortalSimulator *SimulatorThatOwnsEntity = CPortalSimulator::GetSimulatorThatOwnsEntity( m_hReflector );
	if (SimulatorThatOwnsEntity)
	{
		if (SimulatorThatOwnsEntity->EntityIsInPortalHole( m_hReflector )
			&& ((((vecOrigin.x * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.x)
			+ (vecOrigin.y * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.y))
			+ (vecOrigin.z * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.z))
			- SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
		{
			if ((((m_hReflector->WorldSpaceCenter().x * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.x)
				+ (m_hReflector->WorldSpaceCenter().y *SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.y))
				+ ((m_hReflector->WorldSpaceCenter().z * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.z))
				- SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
			{
#if 0
				float v37 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[1][1];
				float v22 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[1][0];
				float v36 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[1][2];
				float v23 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[0][0];
				float v24 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[0][1];
				float v25 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[0][2];
				float v35 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[2][0];
				float v34 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[2][1];
				float v33 = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[2][2];
				vecOrigin.y = (((vecOrigin.x * v22) + (vecOrigin.y * v37))
					+ (vecOrigin.z * v36))
					+ SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[1][3];
				vecOrigin.z = ( ((v35 * vecOrigin.x) + (vecOrigin.y * v34)) + (vecOrigin.z * v33) ) + SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[2][3];
				vecOrigin.x = ( (v23 * vecOrigin.x) + ( vecOrigin.y * v24 ) + (vecOrigin.z * v25) ) + SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked.m[0][3];
				vecDir.x = ((v23 * vecDir.x) + (v24 * vecDir.y)) + (v25 * vecDir.z);
				vecDir.z = ((vecDir.x * v35) + (vecDir.y * v34)) + (vecDir.z * v33);
				vecDir.y = (v22 * vecDir.x + (v37 * vecDir.y)) + (v36 * vecDir.z);
#else
				//UNDONE: Which should we use?
#if 0
				vecDir = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked * vecDir;
#else
				UTIL_Portal_PointTransform( SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked, vecDir, vecDir );
#endif

				vecOrigin = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked * vecOrigin;
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

Vector CPortalLaser::ClosestPointOnLineSegment( Vector &vPos )
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
	trace_t tr; // [esp+BCh] [ebp-13Ch] BYREF
	Vector vecNewTermPoint; // [esp+190h] [ebp-68h] BYREF
	Vector vDir; // [esp+1A4h] [ebp-54h] BYREF
	Vector vecStartPos; // [esp+1B8h] [ebp-40h] BYREF
	Vector vecDirection_0; // [esp+1C4h] [ebp-34h] BYREF
	float flTotalBeamLength = 0.0; // [esp+1D0h] [ebp-28h] BYREF
	float flOtherBeamLength = 0.0;
	Ray_t ray;
	if ( new_portal_laser.GetInt() )
	{
		PortalLaserInfoList_t infoList;

		flTotalBeamLength = 0.0;
		bool v6 = !m_bAutoAimEnabled;
		vecDirection_0 = vecDirection;
		if ( v6 )
			goto LABEL_3;
		
		CBaseEntity *pHitTarget = TraceLaser( true, vecStart, vecDirection_0, flTotalBeamLength, tr, infoList, &vecStartPos);
		bool bAutoAimSuccess = false;
		if ( ShouldAutoAim(pHitTarget) )
		{
			float v9 = vecStart.x;
			float z = vecStart.z;
			float v11 = vecDirection.z;
			float v12 = flTotalBeamLength * vecDirection.x;
			vDir.y = (((vecDirection.y * flTotalBeamLength) + vecStart.y) + vecStartPos.y)
				- vecStart.y;
			vDir.z = (((v11 * flTotalBeamLength) + z) + vecStartPos.z) - z;
			vDir.x = ((v12 + v9) + vecStartPos.x) - v9;
			VectorNormalize(vDir);
			memset(&vecNewTermPoint, 0, sizeof(vecNewTermPoint));
			vec_t v43 = 0.0;
			bAutoAimSuccess = 0;
			if ( pHitTarget == TraceLaser( false, vecStart, vDir, flOtherBeamLength, tr, infoList, false ) )
			{
				ray.Init( tr.startpos, tr.endpos );

				flTotalBeamLength = flOtherBeamLength;
				vecDirection_0 = vDir;

				DamageEntitiesAlongLaser( infoList, true );
				bAutoAimSuccess = 1;
			}
			infoList.RemoveAll();
			if ( vecNewTermPoint.z < 0.0 )
			{
				v43 = vecNewTermPoint.x;
			}
			else
			{
				vecNewTermPoint.x = 0.0;
				vecNewTermPoint.y = 0.0;
				v43 = 0.0;
			}
		}
		infoList.RemoveAll();

		if ( !bAutoAimSuccess )
		{
		LABEL_3:
			memset(&vDir, 0, sizeof(vDir));
			vec_t v46 = 0.0;
			UTIL_ClearTrace( tr );

			PortalLaserInfoList_t hitInfoList;

			pHitTarget = TraceLaser( false, vecStart, vecDirection_0, flTotalBeamLength, tr, hitInfoList, 0);
			DamageEntitiesAlongLaser( hitInfoList, false );

			hitInfoList.RemoveAll();

			if ( vDir.z >= 0.0 )
			{
				vDir.x = 0.0;
				vDir.y = 0.0;
				v46 = 0.0;
				v6 = ( vecStart.x == m_vStartPoint.m_Value.x );
				bAutoAimSuccess = 0;
				goto LABEL_21;
			}
			bAutoAimSuccess = 0;
			v46 = vDir.x;
		}
		v6 = ( vecStart.x == m_vStartPoint.m_Value.x );
	LABEL_21:
		
		m_vStartPoint = vecStart;

		Vector vNewEndPoint = ( (vecDirection_0 * flTotalBeamLength) + vecStart );
		m_vEndPoint = vNewEndPoint;

		m_bIsAutoAiming = bAutoAimSuccess;
		if ( !tr.m_pEnt )
			goto LABEL_34;

		bool IsShadowClone = CPhysicsShadowClone::IsShadowClone( tr.m_pEnt );
		CBaseEntity *v15 = tr.m_pEnt;
		if ( IsShadowClone )
		{
			v15 = ((CPhysicsShadowClone*)(tr.m_pEnt))->GetClonedEntity();
		}
		CPropWeightedCube *SchrodingerTwin = UTIL_GetSchrodingerTwin(v15);
		if ( SchrodingerTwin )
			v15 = SchrodingerTwin;
		if ( !ReflectLaserFromEntity(v15) )
		{
		LABEL_34:
			RemoveChildLaser();
			if ( !pHitTarget
				|| pHitTarget->ClassMatches( "point_laser_target" ) )
			{
				if ( m_pPlacementHelper )
					UTIL_SetOrigin(m_pPlacementHelper, tr.endpos, 0);
				BeamDamage( tr );
			}
		}
		return;
	}
	CTraceFilterSimpleClassnameList traceFilter( pReflector, 0 );
	
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

	if ( tr.m_pEnt )
	{
		bool v26 = CPhysicsShadowClone::IsShadowClone(tr.m_pEnt);
		CBaseEntity *m_pEnt = tr.m_pEnt;
		if ( v26 )
		{
			m_pEnt = ((CPhysicsShadowClone*)tr.m_pEnt)->GetClonedEntity();
		}
		CPropWeightedCube *v28 = UTIL_GetSchrodingerTwin(m_pEnt);
		if ( v28 )
			m_pEnt = v28;
		if ( ReflectLaserFromEntity(m_pEnt) )
		{
			return;
		}
	}
	RemoveChildLaser();
	
	if ( m_pPlacementHelper )
		UTIL_SetOrigin( m_pPlacementHelper, tr.endpos );
	BeamDamage( tr );
}

CBaseEntity *CPortalLaser::TraceLaser( bool bIsFirstTrace, Vector &vecStart, Vector &vecDirection, float &flTotalBeamLength, trace_t &tr, PortalLaserInfoList_t &infoList, Vector *pVecAutoAimOffset )
{
	CBaseEntity *pHitEntity; // [esp+34h] [ebp-144h]

	flTotalBeamLength = 0.0;

	Vector vStart = vecStart;

	Vector vDir = vecDirection;

	CTraceFilterSimpleClassnameList traceFilter( m_hReflector, 0 );
	traceFilter.AddClassnameToIgnore("projected_wall_entity");
	traceFilter.AddClassnameToIgnore("player");
	traceFilter.AddClassnameToIgnore("point_laser_target");

	CUtlVector<CPortal_Base2D*> portalList;
	Vector vAutoAimOffset;

	Vector v18; // Stupid init and gotos!!
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
				goto LABEL_24;
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

	if (ShouldAutoAim(tr.m_pEnt))
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
			v18 = pHitEntity->WorldSpaceCenter(); // NOTE: This is just a best guess!
			CalcClosestPointOnLine( v18, tr.startpos, tr.endpos, vAutoAimOffset );
		LABEL_24:
			Vector v19 = pHitEntity->WorldSpaceCenter(); // NOTE: This is just a best guess!

			*pVecAutoAimOffset = v19 - vAutoAimOffset;
		}
	}
LABEL_25:
	if (ShouldAutoAim( pHitEntity ) && pVecAutoAimOffset)
	{
		vAutoAimOffset = *pVecAutoAimOffset;
		if (portalList.Count() > 0)
		{
			int v22 = 0;
			do
				UTIL_Portal_VectorTransform(
				portalList[v22++]->m_matrixThisToLinked,
				vAutoAimOffset,
				vAutoAimOffset);
			while (portalList.Count() > v22);
		}
		*pVecAutoAimOffset = vAutoAimOffset;
	}
	m_angPortalExitAngles = GetAbsAngles();
	int v33 = portalList.Count() - 1;
	if (portalList.Count() - 1 >= 0)
	{
		int v23 = portalList.Count() - 1;
		do
		{
			UTIL_Portal_AngleTransform( portalList[v23]->m_hLinkedPortal->MatrixThisToLinked(), m_angPortalExitAngles, m_angPortalExitAngles );
			--v33;
			--v23;
		} while (v33 != -1);
	}
	return pHitEntity;
}

void CPortalLaser::UpdateNextLaser( Vector &vecStart, Vector &vecDirection, CPropWeightedCube *pReflector )
{
	if ( AutoList().Count() < 30 )
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
	const Vector v16; // eax
	float v17; // xmm0_4
	Vector vecEnd; // [esp+4Ch] [ebp-7Ch]
	Vector vecDirection;
	Vector vecNearestPoint;
	Vector vecLineToLaser;
	Vector vecBounce;
	Vector vecPlayerVelocity;

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

		CFlaggedEntitiesEnum rayEnum( list, 1024, 0 );

		Ray_t ray;

		ray.Init( vecStart, vecEnd );

		partition->EnumerateElementsAlongRay( PARTITION_ENGINE_NON_STATIC_EDICTS, ray, false, &rayEnum );
				
		int nCount = rayEnum.GetCount();

		int v6 = 0;
		while (1)
		{
			CBaseEntity *pEntity = list[i];
			if ( sv_debug_laser.GetInt() )
			{	
				NDebugOverlay::BoxAngles( vecStart, pEntity->CollisionProp()->OBBMins(), pEntity->CollisionProp()->OBBMaxs(), pEntity->CollisionProp()->GetCollisionAngles(), 255, 255, 0, 0, 0.1);
			}
			
			vecPlayerVelocity = pEntity->GetAbsVelocity();
			Assert( pEntity );
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
			CalcClosestPointOnLineSegment(pEntity->GetAbsOrigin(), vecStart, vecEnd, vecNearestPoint, 0);
			vec_t v7 = pEntity->GetAbsOrigin().y - vecNearestPoint.y;
			vecLineToLaser.x = GetAbsOrigin().x - vecNearestPoint.x;
			vecLineToLaser.y = v7;
			vecLineToLaser.z = vecNearestPoint.z - vecNearestPoint.z;
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
			v17 = ((vecPlayerVelocity.x * vecLineToLaser.x) + (vecPlayerVelocity.y * vecLineToLaser.y))
				+ (vecPlayerVelocity.z * 0.0);
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
			if ( nCount < ++v6 )
				goto LABEL_29;
		}
		if (fabs(vecDirection.z) < 0.2000000029802322)
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
	PortalLaserInfo_t *v8 = &infoList[infoList.Count() - 1];
	v8->vecStart = vecStart;
	v8->vecEnd = vecEnd;

	float extents;
	if (bIsFirstTrace)
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
	const int FLAGS = 0;
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
				/*&& v16->IsAlive()*/) )
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
			CBaseEntity *v19 = victim.pVictim;
			//float flVictimFraction = victim.flFraction;
			info->sortedEntList.AddToTail( v19 );

			if (v19 != NULL)
			{
				bool v25;
				if ( ( (v25 = v19->ClassMatches("point_laser_target"), flFraction = victim.flFraction, v25 )) && !bIsTurret)
				{
					CPortalLaserTarget *v26 = dynamic_cast<CPortalLaserTarget*>( v19 );

					if ( v26 && v26->IsTerminalPoint() ) // IsTerminalPoint() was v26[909]...
					{
						vecOut = v26->WorldSpaceCenter();// FIXME!! Was: *(Vector *)(*(int(__thiscall **)(_BYTE *))(*(_DWORD *)v26 + 596))(v26);
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
				else if ( v19->ClassMatches( "npc_portal_turret_floor") )
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
	if (!m_bAutoAimEnabled)
		return false;
	
	if ( !pEntity || !pEntity->ClassMatches("point_laser_target") )
	{
		return false;
	}

	// In the original function, it's a dynamic cast, but we're doing an assert_cast/static_cast instead
	// because there is already a check above for point_laser_target, saves a few cycles I suppose.
	CPortalLaserTarget *pTarget = assert_cast<CPortalLaserTarget*>(pEntity);

	bool bCond = pTarget->IsTerminalPoint(); // Was pTarget[909], but wtf does this mean?
	if (!m_bFromReflectedCube)
		return bCond;

	return (sv_laser_cube_autoaim.GetInt() || g_pGameRules->IsMultiplayer()) && bCond;
}

bool CPortalLaser::IsOn( void )
{
	return m_pfnThink != NULL;
}

bool CPortalLaser::ReflectLaserFromEntity( CBaseEntity *pReflector )
{
	CPropWeightedCube *pCastedReflector = static_cast<CPropWeightedCube*>(pReflector);

	if (pReflector == m_hReflector)
	{
		if (m_pChildLaser)
		{
			m_pChildLaser->m_pParentLaser = NULL;
			UTIL_Remove(m_pChildLaser);
			m_pChildLaser = NULL;
		}
		return true;
	}
	else if (pReflector && (UTIL_IsReflectiveCube(pReflector) || UTIL_IsSchrodinger(pReflector)))
	{
		if ( pCastedReflector->HasLaser() )
		{
			if (pCastedReflector->GetLaser() != m_pChildLaser)
				RemoveChildLaser();
		}
		else
		{
			Vector vecForward;
			if (UTIL_GetSchrodingerTwin(pReflector))
			{
				AngleVectors( m_angPortalExitAngles, &vecForward );
			}
			else
			{
				QAngle reflectorAngles = pReflector->GetAbsAngles();
				AngleVectors(reflectorAngles, &vecForward);
			}
			Vector v8 = pReflector->WorldSpaceCenter();
			Vector vecOffset = v8 + (vecForward * 22.0);

			CPortalSimulator *SimulatorThatOwnsEntity = CPortalSimulator::GetSimulatorThatOwnsEntity(pReflector);
			//float *floatPortalSim = (float *)SimulatorThatOwnsEntity;
			if (SimulatorThatOwnsEntity)
			{

				if (SimulatorThatOwnsEntity->EntityIsInPortalHole(pReflector)
					&& (((( vecOffset.x * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.x)
					+ (vecOffset.y * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.y))
					+ (vecOffset.z * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.z))
					- SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
				{
					Vector vCenter = pReflector->WorldSpaceCenter();
					if (((((vCenter.y * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.y) +
						(vCenter.x * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.x)) +
						(vCenter.z * SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Normal.z))
						- SimulatorThatOwnsEntity->GetInternalData().Placement.PortalPlane.m_Dist) > 0.0)
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
						vecForward = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked * vecForward;
#else
						UTIL_Portal_PointTransform( SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked, vecForward, vecForward );
#endif

						vecOffset = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked * vecOffset;
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
					CTakeDamageInfo v8;
					v8.SetDamage( 1.0 );
					v8.SetDamageType( DMG_CRUSH ); // 1

					pAnimating->Event_Killed( v8 );
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
	CTakeDamageInfo info( this, this, flAmount * gpGlobals->frametime, 8, 0);
	// Note: WorldSpaceCenter() is just a best guess!!
	Vector v3 = pVictim->WorldSpaceCenter(); //(const Vector *)(*((int(__cdecl **)(CBaseEntity *))pVictim->_vptr_IHandleEntity + 150))(pVictim);

	Vector vecMeleeDir;
	vecMeleeDir.x = 1.0;
	vecMeleeDir.y = 0.0;
	vecMeleeDir.z = 0.0;

	CalculateMeleeDamageForce(&info, vecMeleeDir, v3, 1.0);
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
	float v14 = (v33 + v33) * 0.5;
	ray.Init(vecStart, vecEnd, Vector(-v14, -v14, -v14), Vector(v14, v14, v14));

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
				float flFraction[7];
				CalcClosestPointOnLineSegment(pEntity->GetAbsOrigin(), vecStart, vecEnd, vecPlayerVelocity, flFraction);
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
								if (flFraction[0] > 0.0)
								{
									LaserVictimInfo_t tempInfo;
									tempInfo.pVictim = pEntity;
									tempInfo.flFraction = flFraction[0];

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

	bool bReturn;
	if (vsrtVictims.Count() > 0)
	{
		bBestIsTurreta = false;
		for (int i = 0; i < vsrtVictims.Count(); ++i)
		{
			CBaseEntity *pVictim = vsrtVictims[i].pVictim;
			if (!pVictim)
				goto LABEL_19;
			if ((pVictim->ClassMatches("point_laser_target"))
				&& !bBestIsTurreta)
			{
				DamageEntity( pVictim, 1.0 );
				CPortalLaserTarget *pLaserTarget = dynamic_cast<CPortalLaserTarget*>(pVictim);
				if ( pLaserTarget && pLaserTarget->IsTerminalPoint() ) // Was pVictim[933]...
				{
					bReturn = true;
					
					// Note; WorldSpaceCenter() is just a best guess!
					if (pVecOut)
						*pVecOut = pLaserTarget->WorldSpaceCenter(); //*(Vector *)(*(int(__cdecl **)(_BYTE *))(*(_DWORD *)v25 + 600))(v25);
					goto LABEL_41;
				}
			}
			
			if ( pVictim->ClassMatches( "npc_portal_turret_floor" ) )
			{
				bBestIsTurreta = true;
				continue;
			}
			else
			{
			LABEL_19:
				float v20;
				float v21;
				float v22;
				// NOTE: IsPlayer() is just a best guess!
				if ( pVictim->IsPlayer() //(*((unsigned __int8(__cdecl **)(CBaseEntity *))pVictim->_vptr_IHandleEntity + 86))(pVictim)
					&& pVictim->GetMoveType() != MOVETYPE_NOCLIP)
				{
					Vector vecNearestPoint;
					vecPlayerVelocity = pVictim->GetAbsVelocity();
					VectorNormalize(vecPlayerVelocity);
					CalcClosestPointOnLineSegment(pVictim->GetAbsOrigin(), vecStart, vecEnd, vecNearestPoint, 0);
					vec_t v19 = pVictim->GetAbsOrigin().x - vecNearestPoint.x;

					Vector vecLineToLaser;
					vecLineToLaser.y = pVictim->GetAbsOrigin().y - vecNearestPoint.y;
					vecLineToLaser.x = v19;
					vecLineToLaser.z = vecNearestPoint.z - vecNearestPoint.z;
					VectorNormalize(vecLineToLaser);
					vecLineToLaser.z = 0.0;
					Vector vecBounce;
					if ((((vecPlayerVelocity.x * vecPlayerVelocity.x)
						+ (vecPlayerVelocity.y * vecPlayerVelocity.y))
						+ (vecPlayerVelocity.z * vecPlayerVelocity.z)) < 1.4210855e-14)
					{
						vecBounce.z = (0.0 * vecDirection.x) - (0.0 * vecDirection.y);
						v20 = vecDirection.y - (0.0 * vecDirection.z);
						v21 = vecBounce.z * 100.0;
						vecBounce.x = v20;
						v22 = (0.0 * vecDirection.z) - vecDirection.x;
						vecBounce.y = v22;
					LABEL_28:
						Vector vecPushVelocity;
						vecPushVelocity.z = v21;
						vecPushVelocity.x = v20 * 100.0;
						vecPushVelocity.y = 100.0 * v22;
						
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
						+ (0.0 * vecPlayerVelocity.z);
					vecBounce.x = ((vecLineToLaser.x * -2.0) * v31) + vecPlayerVelocity.x;
					vecBounce.y = ((-2.0 * vecLineToLaser.y) * v31) + vecPlayerVelocity.y;
					vecBounce.z = (v31 * -0.0) + vecPlayerVelocity.z;
					VectorNormalize(vecBounce);
					v20 = vecBounce.x;
					vecBounce.z = 0.0;
					v22 = vecBounce.y;
					if ((((vecLineToLaser.x * vecBounce.x) + (vecLineToLaser.y * vecBounce.y))
						+ (0.0 * vecLineToLaser.z)) >= 0.0)
					{
						v21 = 0.0;
						goto LABEL_28;
					}
				}
			}
		}
	}
	bReturn = false;
	if (pVecOut)
		*pVecOut = vecEnd;
LABEL_41:
	//vsrtVictims.m_Size = 0;
	//if (vsrtVictims.m_Memory.m_nGrowSize >= 0 && vsrtVictims.m_Memory.m_pMemory)
	//	(*(void(__cdecl **)(_DWORD, LaserVictimInfo_t *))(*g_pMemAlloc + 8))(g_pMemAlloc, vsrtVictims.m_Memory.m_pMemory);

	return bReturn;
}