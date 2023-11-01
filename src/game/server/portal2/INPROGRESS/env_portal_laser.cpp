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

ConVar portal_laser_normal_update( "portal_laser_normal_update", "0.05f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar portal_laser_high_precision_update( "portal_laser_high_precision_update", "0.03f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_debug_laser( "sv_debug_laser", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar new_portal_laser( "new_portal_laser", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_laser_cube_autoaim( "sv_laser_cube_autoaim", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );



// constants

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

void UTIL_Portal_Laser_Prevent_Tilting( Vector &vDirection )
{
	if ( fabs(vDirection.z) < 0.1 )
	{
		vDirection.z = 0.0;
		VectorNormalize( vDirection );
	}
}

IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_EnvPortalLaser)

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
	for ( int i = 0; i != 33; ++i )
		m_pAmbientSound[i] = 0;

	CreateHelperEntities();

	if ( m_bStartOff )
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
		CBaseEntity::PrecacheScriptSound("LaserGreen.BeamLoop");
	else
		CBaseEntity::PrecacheScriptSound("Laser.BeamLoop");

	CBaseEntity::PrecacheScriptSound("Flesh.LaserBurn");
	CBaseEntity::PrecacheScriptSound("Player.PainSmall");

	PrecacheParticleSystem("laser_start_glow");
	PrecacheParticleSystem("reflector_start_glow");

	if ( !m_bFromReflectedCube )
	{
		if ( GetModelName().ToCStr() )
			CBaseEntity::PrecacheModel( GetModelName().ToCStr() );
		else
			CBaseEntity::PrecacheModel("models/props/laser_emitter.mdl");
	}
}
void CPortalLaser::UpdateOnRemove( void )
{
	if ( m_pPlacementHelper )
		UTIL_Remove( m_pPlacementHelper );
	for ( int i = 0; i != 33; ++i)
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
	CBasePlayer *pPlayer; // eax
	
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		pPlayer = UTIL_PlayerByIndex( i );
		if (pPlayer && pPlayer->IsConnected() && !pPlayer->IsSplitScreenPlayer())
		{
			if (!m_pSoundProxy[i])
			{
				m_pSoundProxy[i] = CreateEntityByName("info_target", -1, 1);
				

				m_pSoundProxy[i]->SetAbsOrigin( GetAbsOrigin() );
				m_pSoundProxy[i]->AddEFlags( 0x80u );
				m_pSoundProxy[i]->DispatchUpdateTransmitState();
			}

			CBaseEntity *pSoundProxy = m_pSoundProxy[i];

			if (!m_pSoundProxy[MAX_PLAYERS])
			{
				CSingleUserRecipientFilter filter( pPlayer ); // [esp+24h] [ebp-28h] BYREF
				//filter.__vftable = (CSingleUserRecipientFilter_vtbl *)&CSingleUserRecipientFilter::`vftable';
				
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
		if (i == 32)
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
	if (i != 32)
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
		m_pPlacementHelper = (CInfoPlacementHelper *)CreateEntityByName("info_placement_helper", -1, 1);

		m_pPlacementHelper->SetAbsOrigin( GetAbsOrigin() );

		m_pPlacementHelper->SetAbsAngles( GetAbsAngles() );

		m_pPlacementHelper->KeyValue("radius", "16");
		m_pPlacementHelper->KeyValue("hide_until_placed", "0");

		DispatchSpawn( m_pPlacementHelper, 1 );
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
	float m_fValue;

	m_bLaserOn = true;

	if ( m_pfnThink != NULL )
	{

		SetThink( &CPortalLaser::StrikeThink );

		if ( m_bFromReflectedCube )
			m_fValue = portal_laser_normal_update.GetFloat();
		else
			m_fValue = portal_laser_high_precision_update.GetFloat();
		
		SetNextThink( m_fValue + gpGlobals->curtime, 0);
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
			CSoundEnvelopeController &Controller = CSoundEnvelopeController::GetController();

			Controller.SoundDestroy( m_pAmbientSound[i] );
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


	for (int i = 0; i < 33; ++i)
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

		m_angParentAngles = m_angPortalExitAngles;

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
				vecDir = SimulatorThatOwnsEntity->GetInternalData().Placement.matThisToLinked * vecDir;
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

	for ( int i = 0; i != 33; ++i)
	{
		if (m_pSoundProxy[i])
			UTIL_SetOrigin(m_pSoundProxy[i], m_vecNearestSoundSource[i], 0);
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
	Vector vecNewTermPoint_0; // [esp+1A4h] [ebp-54h] BYREF
	Vector vecStartPos; // [esp+1B8h] [ebp-40h] BYREF
	Vector vecDirection_0; // [esp+1C4h] [ebp-34h] BYREF
	float flTotalBeamLength = 0.0; // [esp+1D0h] [ebp-28h] BYREF
	float flOtherBeamLength = 0.0;
	EHANDLE v51; // [esp+1D8h] [ebp-20h] BYREF
	EHANDLE v52; // [esp+1DCh] [ebp-1Ch] BYREF
	Ray_t ray;
	if ( new_portal_laser.GetInt() )
	{

		PortalLaserInfoList_t infoList;

		vec_t x = vecDirection.x;
		flTotalBeamLength = 0.0;
		bool v6 = !m_bAutoAimEnabled;
		vecDirection_0.x = x;
		vecDirection_0.y = vecDirection.y;
		vecDirection_0.z = vecDirection.z;
		if ( v6 )
			goto LABEL_3;
		
		CBaseEntity *pHitTarget = TraceLaser( 1, vecStart, vecDirection_0, flTotalBeamLength, tr, infoList, &vecStartPos);
		bool bAutoAimSuccess = false;
		if ( ShouldAutoAim(pHitTarget) )
		{
			float v9 = vecStart.x;
			float z = vecStart.z;
			float v11 = vecDirection.z;
			float v12 = flTotalBeamLength * vecDirection.x;
			vecNewTermPoint_0.y = (((vecDirection.y * flTotalBeamLength) + vecStart.y) + vecStartPos.y)
				- vecStart.y;
			vecNewTermPoint_0.z = (((v11 * flTotalBeamLength) + z) + vecStartPos.z) - z;
			vecNewTermPoint_0.x = ((v12 + v9) + vecStartPos.x) - v9;
			VectorNormalize(vecNewTermPoint_0);
			memset(&vecNewTermPoint, 0, sizeof(vecNewTermPoint));
			vec_t v43 = 0.0;
			bAutoAimSuccess = 0;
			if ( pHitTarget == TraceLaser( false, vecStart, vecNewTermPoint_0, flOtherBeamLength, tr, infoList, false ) )
			{

				ray.Init( tr.startpos, tr.endpos );

				flTotalBeamLength = flOtherBeamLength;
				vecDirection_0 = vecNewTermPoint_0;

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

		float v5 = 0.0;
		if ( !bAutoAimSuccess )
		{
		LABEL_3:
			memset(&vecNewTermPoint_0, 0, sizeof(vecNewTermPoint_0));
			vec_t v46 = 0.0;
			UTIL_ClearTrace( tr );

			PortalLaserInfoList_t hitInfoList;

			pHitTarget = TraceLaser( 0, vecStart, vecDirection_0, flTotalBeamLength, tr, hitInfoList, 0);
			DamageEntitiesAlongLaser( hitInfoList, 0 );

			hitInfoList.RemoveAll();

			if ( vecNewTermPoint_0.z >= 0.0 )
			{
				vecNewTermPoint_0.x = 0.0;
				v5 = vecStart.x;
				vecNewTermPoint_0.y = 0.0;
				v46 = 0.0;
				v6 = v5 == m_vStartPoint.m_Value.x;
				bAutoAimSuccess = 0;
				goto LABEL_21;
			}
			bAutoAimSuccess = 0;
			v46 = vecNewTermPoint_0.x;
		}
		v5 = vecStart.x;
		v6 = vecStart.x == m_vStartPoint.m_Value.x;
	LABEL_21:
		
		m_vStartPoint = vecStart;
		v5 = vecStart.x;
		float y = vecStart.y;
		float v8 = vecStart.z;

		float v13 = (vecDirection_0.x * flTotalBeamLength) + v5;
	
		vec_t v32 = (flTotalBeamLength * vecDirection_0.z) + v8;
		vec_t v33 = (vecDirection_0.y * flTotalBeamLength) + y;

		Vector vNewEndPoint( v13, v33, v32 );
		m_vEndPoint = vNewEndPoint;

		m_bIsAutoAiming = bAutoAimSuccess;
		if ( !tr.m_pEnt )
			goto LABEL_34;

		bool IsShadowClone = CPhysicsShadowClone::IsShadowClone(*(const CBaseEntity **)(&tr.m_pEnt));
		CBaseEntity *v15 = tr.m_pEnt;
		if ( IsShadowClone )
		{
			v15 = 0;
			v51 = ((CPhysicsShadowClone*)(tr.m_pEnt))->GetClonedEntity();

			v15 = v51;

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

	UTIL_TraceLine( vecStart, vecStart + ( vecDirection * MAX_TRACE_LENGTH ), 1174421505, &traceFilter, &tr );

	CPortalLaser::UpdateSoundPosition( tr.startpos, tr.endpos );
	flOtherBeamLength = 0.0;
	
	CPortal_Base2D *pFirstPortal;
	if ( UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, 0)
		&& flOtherBeamLength != 0.0
		&& pFirstPortal->IsActivedAndLinked() )
	{
		bool v24 = StrikeEntitiesAlongLaser( tr.startpos, tr.endpos, &vecNewTermPoint_0 );
		if ( v24 )
		{
			m_vStartPoint = vecStart;
			m_vEndPoint = vecNewTermPoint_0;
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
		goto LABEL_56;
	}
	if ( CPortalLaser::StrikeEntitiesAlongLaser( tr.startpos, tr.endpos, &vecNewTermPoint ) )
	{
		m_vStartPoint = vecStart;
		m_vEndPoint = vecNewTermPoint;

		goto LABEL_56;
	}

	m_vStartPoint = vecStart;
	m_vEndPoint = tr.endpos;

	if ( tr.m_pEnt )
	{
		bool v26 = CPhysicsShadowClone::IsShadowClone(tr.m_pEnt);
		CBaseEntity *m_pEnt = tr.m_pEnt;
		if ( v26 )
		{
			v52 = ((CPhysicsShadowClone*)tr.m_pEnt)->GetClonedEntity();
			m_pEnt = v52;
		}
		CPropWeightedCube *v28 = UTIL_GetSchrodingerTwin(m_pEnt);
		if ( v28 )
			m_pEnt = v28;
		if ( ReflectLaserFromEntity(m_pEnt) )
		{
		LABEL_56:
		//		return;
		//LABEL_58:
			return;
		}
	}
	RemoveChildLaser();
	
	if ( m_pPlacementHelper )
		UTIL_SetOrigin(m_pPlacementHelper, tr.endpos, 0);
	BeamDamage( tr );
}

CBaseEntity *CPortalLaser::TraceLaser( bool bIsFirstTrace, Vector &vecStart, Vector &vecDirection, float &flTotalBeamLength, trace_t &tr, PortalLaserInfoList_t &infoList, Vector *pVecAutoAimOffset )
{
	vec_t z; // edx
	const IHandleEntity *v9; // ecx
	vec_t v10; // eax
	CPortal_Base2D *v13; // edx
	bool v16; // al
	float v17; // xmm1_4
	Vector v18; // eax
	int v22; // esi
	int v23; // esi
	vec_t v29; // xmm0_4
	vec_t v30; // xmm2_4
	CBaseEntity *pHitEntity; // [esp+34h] [ebp-144h]
	int v33; // [esp+3Ch] [ebp-13Ch]
	Ray_t ray; // [esp+40h] [ebp-138h] BYREF
	Ray_t rayTransformed; // [esp+90h] [ebp-E8h] BYREF
	Vector vStart; // [esp+124h] [ebp-54h] BYREF
	Vector vDir; // [esp+130h] [ebp-48h] BYREF
	Vector vAutoAimOffset; // [esp+13Ch] [ebp-3Ch] BYREF
	Vector vStrike_0; // [esp+148h] [ebp-30h] BYREF
	CPortal_Base2D *pFirstPortal; // [esp+154h] [ebp-24h] BYREF
	CPortal_Base2D *src; // [esp+158h] [ebp-20h] BYREF
	EHANDLE v44; // [esp+15Ch] [ebp-1Ch] BYREF

	flTotalBeamLength = 0.0;
	vStart.x = vecStart.x;
	z = vecStart.z;
	vStart.y = vecStart.y;
	v9 = 0;
	vStart.z = z;
	vDir.x = vecDirection.x;
	v10 = vecDirection.z;
	vDir.y = vecDirection.y;
	vDir.z = v10;

	v9 = m_hReflector;

	CTraceFilterSimpleClassnameList traceFilter( v9, 0 );
	traceFilter.AddClassnameToIgnore("projected_wall_entity");
	traceFilter.AddClassnameToIgnore("player");
	traceFilter.AddClassnameToIgnore("point_laser_target");

	CUtlVector<CPortal_Base2D*> portalList;
	do
	{
		while (1)
		{
			UTIL_ClearTrace(tr);
			v16 = 1;

			UTIL_TraceLine( vStart, vStart + ( MAX_TRACE_LENGTH * vDir ), 1174421505, &traceFilter, &tr );

			UpdateSoundPosition( tr.startpos, tr.endpos );
			pFirstPortal = 0;
			if (!UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, 0)
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
					Vector v28 = pHitEntity->GetLocalOrigin(); // NOTE: This is just a best guess!
					v29 = v28.z - vStrike_0.z;
					v30 = v28.x - vStrike_0.x;
					pVecAutoAimOffset->y = v28.y - vStrike_0.y;
					pVecAutoAimOffset->z = v29;
					pVecAutoAimOffset->x = v30;
				}
				goto LABEL_25;
			}
			rayTransformed.m_pWorldAxisTransform = 0;
			UTIL_Portal_RayTransform(pFirstPortal->m_matrixThisToLinked, ray, rayTransformed);
			vDir = rayTransformed.m_Delta;
			VectorNormalize(vDir);
			UTIL_Portal_PointTransform(pFirstPortal->m_matrixThisToLinked, tr.endpos, vStart);
			
			v13 = pFirstPortal->m_hLinkedPortal;
			src = v13;
			portalList.InsertBefore( 0, src);
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
		v17 = tr.endpos.z - tr.startpos.z;
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
			v44 = ((CPhysicsShadowClone *)tr.m_pEnt)->GetClonedEntity();
			pHitEntity = v44;
		}
		else
		{
			pHitEntity = tr.m_pEnt;
		}
		if (pVecAutoAimOffset)
		{
			v18 = pHitEntity->GetLocalOrigin(); // NOTE: This is just a best guess!
			CalcClosestPointOnLine( v18, tr.startpos, tr.endpos, vAutoAimOffset, 0);
		LABEL_24:
			Vector v19 = pHitEntity->GetLocalOrigin(); // NOTE: This is just a best guess!

			pVecAutoAimOffset->x = v19.x - vAutoAimOffset.x;
			pVecAutoAimOffset->y = v19.y - vAutoAimOffset.y;
			pVecAutoAimOffset->z = v19.z - vAutoAimOffset.z;
		}
	}
LABEL_25:
	if (ShouldAutoAim( pHitEntity ) && pVecAutoAimOffset)
	{
		vAutoAimOffset = *pVecAutoAimOffset;
		if (portalList.Count() > 0)
		{
			v22 = 0;
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
	v33 = portalList.Count() - 1;
	if (portalList.Count() - 1 >= 0)
	{
		v23 = portalList.Count() - 1;
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
	vec_t v4; // xmm2_4
	vec_t v5; // xmm0_4
	int v6; // esi
	vec_t v7; // xmm1_4
	float x; // xmm4_4
	float y; // xmm3_4
	bool v11; // zf
	int v12; // eax
	const Vector v16; // eax
	float v17; // xmm0_4
	Vector vecEnd; // [esp+4Ch] [ebp-7Ch]
	bool bBlockTarget; // [esp+52h] [ebp-76h]
	int i; // [esp+54h] [ebp-74h]
	Vector vecStart; // [esp+5Ch] [ebp-6Ch]
	Vector vecDirection; // [esp+68h] [ebp-60h] BYREF
	Vector vecPlayerVelocity; // [esp+74h] [ebp-54h] BYREF
	Vector vecNearestPoint; // [esp+80h] [ebp-48h] BYREF
	Vector vecLineToLaser; // [esp+8Ch] [ebp-3Ch] BYREF
	Vector vecBounce; // [esp+98h] [ebp-30h] BYREF
	Vector vecPushVelocity; // [esp+A4h] [ebp-24h] BYREF
	
	if (infoList.Count() > 0)
	{
		i = 0;
		bBlockTarget = 0;
		
		PortalLaserInfo_t v3;
		while (1)
		{
			v3 = infoList[i];
			vecEnd = v3.vecEnd;
			v4 = v3.vecEnd.x - v3.vecStart.x;
			v5 = v3.vecEnd.z - v3.vecStart.z;
			vecDirection.y = v3.vecEnd.y - v3.vecStart.y;
			vecDirection.x = v4;
			vecDirection.z = v5;
			VectorNormalize(vecDirection);
			if (v3.sortedEntList.Count() > 0)
				break;
		LABEL_29:
			if (infoList.Count() <= ++i)
				return;
		}
		v6 = 0;
		vecStart = v3.vecStart;

		float v10;
		
		CBaseEntity *list[1024];

		CFlaggedEntitiesEnum rayEnum( list, 1024, 0 );

		Ray_t ray;

		ray.Init( vecStart, vecEnd );

		partition->EnumerateElementsAlongRay( PARTITION_ENGINE_NON_STATIC_EDICTS, ray, false, &rayEnum );

		int nCount = rayEnum.GetCount();

		while (1)
		{

			CBaseEntity *pEntity = list[i];
			if ( sv_debug_laser.GetInt() )
			{	
				NDebugOverlay::BoxAngles( vecStart, pEntity->CollisionProp()->OBBMins(), pEntity->CollisionProp()->OBBMaxs(), pEntity->CollisionProp()->GetCollisionAngles(), 255, 255, 0, 0, 0.1);
			}
			if (pEntity)
			{
				if ( pEntity->ClassMatches("point_laser_target") && !bBlockTarget )
				{
					if ( /* !*(_BYTE *)(pEntity + 933) || */ !m_bAutoAimEnabled || bAutoAim || m_bFromReflectedCube)
						DamageEntity( pEntity, 1.0);
					goto LABEL_25;
				}
				if ( pEntity->ClassMatches( "npc_portal_turret_floor" ) )
				{
					bBlockTarget = 1;
					goto LABEL_25;
				}
			}

			// FIXME: This might be important!!
			//if (!(*(unsigned __int8(__cdecl **)(int))(*(_DWORD *)pEntity + 344))(pEntity) || *(_BYTE *)(pEntity + 226) == 8)
			//	goto LABEL_25;
			
			if (!pEntity->GetGroundEntity())
				break;

			vecPlayerVelocity = pEntity->GetAbsVelocity();
			VectorNormalize(vecPlayerVelocity);
			CalcClosestPointOnLineSegment(pEntity->GetAbsOrigin(), vecStart, vecEnd, vecNearestPoint, 0);
			v7 = pEntity->GetAbsOrigin().y - vecNearestPoint.y;
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
				v11 = ( pEntity->GetFlags() & FL_ONGROUND ) == 0;
#endif
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
				v12 = 1125515264;
				if (this->m_bIsLethal.m_Value)
					v12 = 1203982336;
				DamageEntity( pEntity, *(float *)&v12 );
				pEntity->EmitSound("Flesh.LaserBurn", 0.0, 0);
				pEntity->EmitSound("Player.PainSmall", 0.0, 0);
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
  float x; // xmm0_4
  float y; // xmm2_4
  float z; // xmm3_4
  float v9; // xmm1_4
  float v10; // xmm5_4
  float v11; // xmm4_4
  float m_fValue; // xmm1_4
  int m_nValue; // eax
  bool v14; // al
  float v15; // xmm1_4
  float v16; // xmm7_4
  float v17; // xmm6_4
  float v18; // xmm5_4
  float v19; // xmm0_4
  float v20; // xmm7_4
  int v21; // eax
  int v22; // ebx
  CBaseEntity *v23; // eax
  bool v24; // zf
  int v25; // esi
  bool v26; // al
  CBaseEntity *v27; // ebx
  CPortalLaserTarget *v28; // eax
  Vector *v29; // eax
  const Vector *v31; // eax
  float v32; // [esp+4Ch] [ebp-8ECh]
  float v33; // [esp+50h] [ebp-8E8h]
  bool bBlockTarget; // [esp+57h] [ebp-8E1h]
  PortalLaserInfo_t info; // [esp+58h] [ebp-8E0h]
  int nCount; // [esp+5Ch] [ebp-8DCh]
  CBaseEntity *list[512]; // [esp+60h] [ebp-8D8h] BYREF
  Ray_t ray; // [esp+860h] [ebp-D8h] BYREF
  CUtlSortVector<LaserVictimInfo_t,CLaserVictimLess,CUtlVector<LaserVictimInfo_t,CUtlMemory<LaserVictimInfo_t,int> > > vsrtVictims; // [esp+8BCh] [ebp-7Ch] BYREF
  Vector vecNearest; // [esp+8D8h] [ebp-60h] BYREF
  Vector vecDirection; // [esp+8ECh] [ebp-4Ch] BYREF
  Vector vecMins; // [esp+8F8h] [ebp-40h] BYREF
  Vector vecMaxs; // [esp+904h] [ebp-34h] BYREF
  LaserVictimInfo_t victim; // [esp+910h] [ebp-28h] BYREF
  CBaseEntity *pEntity; // [esp+918h] [ebp-20h] BYREF
  float flFraction[7]; // [esp+91Ch] [ebp-1Ch] BYREF

  CUtlVector<CPortalLaser::PortalLaserInfo_t,CUtlMemory<CPortalLaser::PortalLaserInfo_t,int>>::InsertBefore(
    infoList,
    infoList->m_Size);
  info = infoList.Element( infoList.Count() - 1 );
  info.vecStart = vecStart;
  info.vecEnd = vecEnd;
  x = vecEnd.x;
  y = vecEnd.y;
  z = vecEnd.z;
  v9 = vecEnd.x - vecStart.x;
  v10 = y - vecStart.y;
  v11 = z - vecStart.z;
  vecDirection.x = v9;
  vecDirection.y = v10;
  vecDirection.z = v11;
  if ( bIsFirstTrace )
    m_fValue = (fsqrt(((v9 * v9) + (v10 * v10)) + (v11 * v11)) * 0.00390625) + 16.0;
  else
    m_fValue = sv_laser_tight_box.GetFloat();
  vecMaxs.x = m_fValue;
  vecMins.x = -m_fValue;
  vecMins.y = -m_fValue;
  m_nValue = sv_debug_laser.GetInt();
  vecMins.z = -m_fValue;
  vecMaxs.y = m_fValue;
  vecMaxs.z = m_fValue;
  if ( m_nValue )
  {
    VectorAngles(&vecDirection, (QAngle *const)&vecNearest);
    if ( bIsFirstTrace )
      NDebugOverlay::SweptBox(vecStart, vecEnd, &vecMins, &vecMaxs, (const QAngle *const)&vecNearest, 255, 0, 0, 0, 0.1);
    else
      NDebugOverlay::SweptBox(vecStart, vecEnd, &vecMins, &vecMaxs, (const QAngle *const)&vecNearest, 0, 255, 0, 0, 0.1);
    x = vecEnd.x;
    y = vecEnd.y;
    z = vecEnd.z;
  }
  v14 = 1;
  ray.m_pWorldAxisTransform = 0;
  v33 = vecStart.x;
  v32 = vecStart.y;
  v15 = vecStart.z;
  ray.m_Delta.x = x - vecStart.x;
  ray.m_Delta.y = y - v32;
  ray.m_Delta.z = z - v15;
  v16 = (vecMaxs.x + vecMins.x) * 0.5;
  v17 = (vecMaxs.y + vecMins.y) * 0.5;
  ray.m_Extents.y = (vecMaxs.y - vecMins.y) * 0.5;
  v18 = (vecMaxs.z + vecMins.z) * 0.5;
  ray.m_Extents.z = (vecMaxs.z - vecMins.z) * 0.5;
  ray.m_Extents.x = (vecMaxs.x - vecMins.x) * 0.5;
  v19 = v33 + v16;
  v20 = -v16;
  if ( (((ray.m_Delta.x * ray.m_Delta.x) + (ray.m_Delta.y * ray.m_Delta.y))
             + (ray.m_Delta.z * ray.m_Delta.z)) == 0.0 )
    v14 = 0;
  ray.m_Start.x = v19;
  ray.m_IsSwept = v14;
  ray.m_Start.y = v32 + v17;
  ray.m_IsRay = (((ray.m_Extents.x * ray.m_Extents.x) + (ray.m_Extents.y * ray.m_Extents.y))
                      + (ray.m_Extents.z * ray.m_Extents.z)) < 0.000001;
  ray.m_Start.z = v15 + v18;
  ray.m_StartOffset.x = v20;
  ray.m_StartOffset.y = -v17;
  ray.m_StartOffset.z = -v18;
  CFlaggedEntitiesEnum::CFlaggedEntitiesEnum((CFlaggedEntitiesEnum *const)&vecNearest, list, 512, 33562752);
  v21 = UTIL_EntitiesAlongRay(&ray, (CFlaggedEntitiesEnum *)&vecNearest);
  pEntity = 0;
  nCount = v21;
  memset(&vsrtVictims, 0, 25);
  if ( v21 > 0 )
  {
    v22 = 0;
    while ( 1 )
    {
      pEntity = list[v22];
      v23 = pEntity;
      if ( pEntity )
      {
        if ( pEntity->ClassMatches( "point_laser_target" ) )
          goto LABEL_16;
        v24 = !pEntity->ClassMatches( "point_laser_target" );
        v23 = pEntity;
        if ( !v24 )
          goto LABEL_16;
        if ( pEntity )
        {
          if ( pEntity->ClassMatches("npc_portal_turret_floor") )
            goto LABEL_16;
          v24 = !pEntity->ClassMatches("npc_portal_turret_floor");
          v23 = pEntity;
          if ( !v24 )
            goto LABEL_16;
        }
      }
      if ( (*((unsigned __int8 (__cdecl **)(CBaseEntity *))v23->_vptr_IHandleEntity + 86))(v23) )
        break;
LABEL_17:
      if ( ++v22 == nCount )
        goto LABEL_18;
    }
    v23 = pEntity;
LABEL_16:
    if ( (*((unsigned __int8 (__cdecl **)(CBaseEntity *))v23->_vptr_IHandleEntity + 70))(v23) )
    {
      v31 = (const Vector *)(*((int (__cdecl **)(CBaseEntity *))pEntity->_vptr_IHandleEntity + 150))(pEntity);
      CalcClosestPointOnLineSegment(v31, vecStart, vecEnd, &vecNearest, flFraction);
      if ( (!(*((unsigned __int8 (__cdecl **)(CBaseEntity *))pEntity->_vptr_IHandleEntity + 86))(pEntity)
         || sv_player_collide_with_laser.GetInt())
        && flFraction[0] > 0.0 )
      {
        victim.flFraction = flFraction[0];
        victim.pVictim = pEntity;
        CUtlSortVector<LaserVictimInfo_t,CLaserVictimLess,CUtlVector<LaserVictimInfo_t,CUtlMemory<LaserVictimInfo_t,int>>>::InsertNoSort(
          &vsrtVictims,
          &victim);
      }
    }
    goto LABEL_17;
  }
LABEL_18:
  CUtlSortVector<LaserVictimInfo_t,CLaserVictimLess,CUtlVector<LaserVictimInfo_t,CUtlMemory<LaserVictimInfo_t,int>>>::RedoSort(
    &vsrtVictims,
    1);
  if ( vsrtVictims.m_Size > 0 )
  {
    v25 = 0;
    bBlockTarget = 0;
    while ( 1 )
    {
      pEntity = vsrtVictims.m_Memory.m_pMemory[v25].pVictim;
      CUtlVector<CBaseEntity *,CUtlMemory<CBaseEntity *,int>>::InsertBefore(
        &info.sortedEntList,
        info.sortedEntList.Count(),
        &pEntity);
      v27 = pEntity;
      if ( !pEntity )
        goto LABEL_24;
      if ( ((v26 = pEntity->ClassMatches("point_laser_target"), v27 = pEntity, v26))
        && !bBlockTarget )
      {
        if ( v27 )
        {
			v28 = dynamic_cast<CPortalLaserTarget*>( v27 );
          if ( v28 )
          {
            if ( v28[933] )
            {
              if ( pVecOut )
              {
                v29 = (Vector *)(*(int (__cdecl **)(_BYTE *))(*(_DWORD *)v28 + 600))(v28);
                v27 = pEntity;
                *pVecOut = *v29;
              }
              goto LABEL_33;
            }
          }
        }
LABEL_24:
        if ( vsrtVictims.m_Size <= ++v25 )
          break;
      }
      else
      {
        if ( !v27
          || v27->ClassMatches( "npc_portal_turret_floor" ) )
        {
          goto LABEL_24;
        }
        ++v25;
        bBlockTarget = 1;
        if ( vsrtVictims.m_Size <= v25 )
          break;
      }
    }
  }
  v27 = 0;
  if ( pVecOut )
    *pVecOut = *vecEnd;
LABEL_33:
  vsrtVictims.m_Size = 0;
  if ( vsrtVictims.m_Memory.m_nGrowSize >= 0 && vsrtVictims.m_Memory.m_pMemory )
    (*(void (__cdecl **)(_DWORD, LaserVictimInfo_t *))(*g_pMemAlloc + 8))(g_pMemAlloc, vsrtVictims.m_Memory.m_pMemory);
  return v27;
}