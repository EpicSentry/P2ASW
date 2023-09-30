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


IMPLEMENT_SERVERCLASS_ST( CPortalLaser, DT_PortalLaser )

SendPropBool( SENDINFO( m_bLaserOn ) ),
SendPropBool( SENDINFO( m_bShouldSpark ) ),
SendPropBool( SENDINFO( m_bIsAutoAiming ) ),
SendPropBool( SENDINFO( m_bIsLethal ) ),
SendPropBool( SENDINFO( m_bUseParentDir ) ),

END_SEND_TABLE();

LINK_ENTITY_TO_CLASS( env_portal_laser, CPortalLaser );

void UTIL_Portal_Laser_Prevent_Tilting( Vector &vDirection )
{
	if ( fabs(vDirection.z) < 0.1 )
	{
		vDirection.z = 0.0;
		VectorNormalize( vDirection );
	}
}

/*
IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_EnvPortalLaser)

SendPropBool(SENDINFO(m_bIsHittingPortal)),
SendPropVector(SENDINFO(v_vHitPos)),
SendPropVector(SENDINFO(vecNetOrigin)),
SendPropVector(SENDINFO(vecNetMuzzleDir)),

END_SEND_TABLE()
*/

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

CBaseEntity *CPortalLaser::GetEntity( void )
{
	return this;
}


Vector CPortalLaser::ClosestPointOnLineSegment( Vector vPos )
{
	return vec3_origin;
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
		CPortalLaser::TurnOn();
}

void CPortalLaser::InputTurnOff( inputdata_t &inputdata )
{
	if ( m_pfnThink != NULL )
		CPortalLaser::TurnOff();
}

void CPortalLaser::TurnOnGlow(void)
{
	if (!m_bGlowInitialized)
	{
		m_bGlowInitialized = true;

		if (!UTIL_IsSchrodinger(m_hReflector))
		{
			if (this->m_bFromReflectedCube)
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

//#define FIRELASER_LINUX
//#define FIRELASER_MAC

void CPortalLaser::FireLaser( Vector &vecStart, Vector &vecDirection, CBaseEntity *pReflector )
{
#if defined ( FIRELASER_LINUX )

	vec_t x; // eax
	float v5; // xmm2_4
	bool v6; // zf
	float y; // xmm3_4
	float v8; // xmm4_4
	float v9; // xmm3_4
	float z; // xmm4_4
	float v11; // xmm0_4
	float v12; // xmm2_4
	float v13; // xmm0_4
	bool IsShadowClone; // al
	CBaseEntity *v15; // esi
	CPropWeightedCube *SchrodingerTwin; // eax
	CInfoPlacementHelper *m_pPlacementHelper; // eax
	float v18; // xmm1_4
	bool v19; // al
	float v20; // xmm0_4
	float v21; // xmm4_4
	float v22; // xmm3_4

	bool v24; // al
	float v25; // xmm0_4
	CPropWeightedCube *v28; // eax
	char *v30; // edx
	char *v31; // edx
	vec_t v32; // [esp+20h] [ebp-1D8h]
	vec_t v33; // [esp+30h] [ebp-1C8h]
	CBaseEntity *pHitTarget; // [esp+50h] [ebp-1A8h]
	char bAutoAimSuccess; // [esp+57h] [ebp-1A1h]
	Ray_t ray; // [esp+60h] [ebp-198h] BYREF
	trace_t tr; // [esp+BCh] [ebp-13Ch] BYREF
	Ray_t rayTransformed; // [esp+110h] [ebp-E8h] BYREF
	CTraceFilterSimpleClassnameList *traceFilter(); // [esp+16Ch] [ebp-8Ch] BYREF
	Vector vecNewTermPoint; // [esp+190h] [ebp-68h] BYREF
	int v42; // [esp+19Ch] [ebp-5Ch]
	Vector vecNewTermPoint_0; // [esp+1A4h] [ebp-54h] BYREF
	int v45; // [esp+1B0h] [ebp-48h]
	vec_t v46; // [esp+1B4h] [ebp-44h]
	Vector vecStartPos; // [esp+1B8h] [ebp-40h] BYREF
	Vector vecDirection_0; // [esp+1C4h] [ebp-34h] BYREF
	float flTotalBeamLength; // [esp+1D0h] [ebp-28h] BYREF
	CPortal_Base2D *pFirstPortal; // [esp+1D4h] [ebp-24h] BYREF
	EHANDLE v51; // [esp+1D8h] [ebp-20h] BYREF

	if (new_portal_laser.GetBool())
	{
		x = vecDirection.x;
		flTotalBeamLength = 0.0;
		v6 = m_bAutoAimEnabled;
		vecDirection_0.x = x;
		vecDirection_0.y = vecDirection.y;
		vecDirection_0.z = vecDirection.z;
		if (v6)
			goto LABEL_3;
		memset(&traceFilter, 0, 20);
		pHitTarget = TraceLaser( 1, vecStart, vecDirection_0, flTotalBeamLength, (trace_t *const)&ray, (CPortalLaser::PortalLaserInfoList_t *const)&traceFilter, &vecStartPos );
		bAutoAimSuccess = 0;
		if ( ShouldAutoAim( pHitTarget ) )
		{
			v9 = vecStart.x;
			z = vecStart.z;
			v11 = vecDirection.z;
			v12 = flTotalBeamLength * vecDirection.x;
			vecNewTermPoint_0.y = (((vecDirection.y * flTotalBeamLength) + vecStart.y) + vecStartPos.y)
				- vecStart.y;
			vecNewTermPoint_0.z = (((v11 * flTotalBeamLength) + z) + vecStartPos.z) - z;
			vecNewTermPoint_0.x = ((v12 + v9) + vecStartPos.x) - v9;
			VectorNormalize(vecNewTermPoint_0);
			memset(&vecNewTermPoint, 0, sizeof(vecNewTermPoint));
			v42 = 0;

			bAutoAimSuccess = 0;
			if (pHitTarget == CPortalLaser::TraceLaser( 0, vecStart, vecNewTermPoint_0, (float *const)&pFirstPortal, &tr, (CPortalLaser::PortalLaserInfoList_t *const)&vecNewTermPoint, 0))
			{

				ray.Init( tr.startpos, tr.endpos );


				ray.m_Start.x = tr.startpos.x;
				ray.m_Start.y = tr.startpos.y;
				ray.m_Start.z = tr.startpos.z;
				*(Vector *)&ray.m_Start.w = tr.endpos;
				*(cplane_t *)&ray.m_Delta.z = tr.plane;
				ray.m_StartOffset.w = tr.fraction;
				ray.m_Extents = *(VectorAligned *)&tr.contents;
				ray.m_pWorldAxisTransform = *(const matrix3x4_t **)&tr.surface.surfaceProps;
				flTotalBeamLength = *(float *)&pFirstPortal;
				vecDirection_0 = vecNewTermPoint_0;
				DamageEntitiesAlongLaser( (const CPortalLaser::PortalLaserInfoList_t *const)&vecNewTermPoint, 1);
				bAutoAimSuccess = 1;
			}
			CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::RemoveAll((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *const)&vecNewTermPoint);
			if (vecNewTermPoint.z < 0.0)
			{

			}
			else
			{
				if (LODWORD(vecNewTermPoint.x))
				{
					(*(void(__cdecl **)(_DWORD, _DWORD))(*g_pMemAlloc + 8))(g_pMemAlloc, LODWORD(vecNewTermPoint.x));
					vecNewTermPoint.x = 0.0;
				}

				vecNewTermPoint.y = 0.0;

			}
		}
		CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::RemoveAll((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *const)&traceFilter);
		if (traceFilter.m_collisionGroup < 0)
		{
			traceFilter.m_PassClassnames.m_Memory.m_pMemory = (const char **)traceFilter._vptr_ITraceFilter;
		}
		else
		{
			if (traceFilter._vptr_ITraceFilter)
			{
				(*(void(__cdecl **)(_DWORD, int(**)(...)))(*g_pMemAlloc + 8))(g_pMemAlloc, traceFilter._vptr_ITraceFilter);
				traceFilter._vptr_ITraceFilter = 0;
			}
			traceFilter.m_pPassEnt = 0;
			traceFilter.m_pMemory = 0;
		}
		if (!bAutoAimSuccess)
		{
		LABEL_3:
			memset(&vecNewTermPoint_0, 0, sizeof(vecNewTermPoint_0));
			v45 = 0;
			v46 = 0.0;
			UTIL_ClearTrace((trace_t *const)&ray);
			pHitTarget = TraceLaser( 0, vecStart, &vecDirection_0, &flTotalBeamLength, (trace_t *const)&ray, (CPortalLaser::PortalLaserInfoList_t *const)&vecNewTermPoint_0, 0 );
			CPortalLaser::DamageEntitiesAlongLaser( (const CPortalLaser::PortalLaserInfoList_t *const)&vecNewTermPoint_0, 0);

			CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::RemoveAll((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *const)&vecNewTermPoint_0);
			if (vecNewTermPoint_0.z >= 0.0)
			{
				if (LODWORD(vecNewTermPoint_0.x))
				{
					(*(void(__cdecl **)(_DWORD, _DWORD))(*g_pMemAlloc + 8))(g_pMemAlloc, LODWORD(vecNewTermPoint_0.x));
					vecNewTermPoint_0.x = 0.0;
				}
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
		v6 = vecStart.x == this->m_vStartPoint.m_Value.x;
	LABEL_21:

		m_vStartPoint = vecStart;

		v13 = (vecDirection_0.x * flTotalBeamLength) + v5;
		if ((vecDirection_0.x * flTotalBeamLength) + v5 != m_vEndPoint.m_Value.x
			|| ((vecDirection_0.y * flTotalBeamLength) + y) != m_vEndPoint.m_Value.y
			|| ((vecDirection_0.z * flTotalBeamLength ) + v8) != m_vEndPoint.m_Value.z)
		{
			v32 = (flTotalBeamLength * vecDirection_0.z) + v8;
			v33 = (vecDirection_0.y * flTotalBeamLength) + y;
			CBaseEntity::NetworkStateChanged(this, &this->m_vEndPoint);
			m_vEndPoint.m_Value.x = (vecDirection_0.x * flTotalBeamLength) + v5;
			m_vEndPoint.m_Value.y = v33;
			m_vEndPoint.m_Value.z = v32;
		}

		if (this->m_bIsAutoAiming.m_Value != bAutoAimSuccess)
		{
			CBaseEntity::NetworkStateChanged(this, &this->m_bIsAutoAiming);
			this->m_bIsAutoAiming.m_Value = bAutoAimSuccess;
		}
		if (!tr.m_pEnt)
			goto LABEL_34;
		IsShadowClone = CPhysicsShadowClone::IsShadowClone(tr.m_pEnt);
		v15 = tr.m_pEnt;
		if (IsShadowClone)
		{
			v15 = 0;
			v51 = ( (CPhysicsShadowClone*) tr.m_pEnt)->GetClonedEntity();
			if (v51.m_Index != -1)
			{
				v30 = (char *)g_pEntityList + 24 * LOWORD(v51.m_Index);
				if (*((_DWORD *)v30 + 2) == HIWORD(v51.m_Index))
					v15 = (CBaseEntity *)*((_DWORD *)v30 + 1);
			}
		}
		SchrodingerTwin = UTIL_GetSchrodingerTwin(v15);
		if (SchrodingerTwin)
			v15 = SchrodingerTwin;
		if (!ReflectLaserFromEntity( v15 ))
		{
		LABEL_34:
			RemoveChildLaser();
			if (!pHitTarget
				|| !FClassnameIs(pHitTarget, "point_laser_target" )
				&& !pHitTarget->ClassMatches( "point_laser_target"))
			{
				m_pPlacementHelper = this->m_pPlacementHelper;
				if (m_pPlacementHelper)
					UTIL_SetOrigin( m_pPlacementHelper, ray.m_Start );

				BeamDamage( (trace_t *)&ray );
			}
		}
		return;
	}
	
	traceFilter.AddClassnameToIgnore("projected_wall_entity");
	traceFilter.AddClassnameToIgnore("player");
	traceFilter.AddClassnameToIgnore("point_laser_target");

	v18 = vecDirection.y;
	v19 = 1;
	ray.m_pWorldAxisTransform = 0;
	v20 = vecDirection.z;
	ray.m_IsRay = 1;
	v21 = vecStart.y;
	ray.m_Start.x = vecStart.x;
	v22 = vecStart.z;
	ray.m_Start.y = v21;
	ray.m_Start.z = v22;
	ray.m_Delta.y = ((v18 * MAX_TRACE_LENGTH) + v21) - v21;
	ray.m_Delta.z = ((v20 * MAX_TRACE_LENGTH) + v22) - v22;
	ray.m_Delta.x = (MAX_TRACE_LENGTH * vecDirection.x + ray.m_Start.x) - ray.m_Start.x;
	memset(&ray.m_Extents, 0, 12);
	memset(&ray.m_StartOffset, 0, 12);
	if ((((ray.m_Delta.x * ray.m_Delta.x) + (ray.m_Delta.y * ray.m_Delta.y))
		+ (ray.m_Delta.z * ray.m_Delta.z)) == 0.0)
		v19 = 0;
	ray.m_IsSwept = v19;
	enginetrace->TraceRay( ray, 1174421505, &traceFilter, &tr);

	if (r_visualizetraces.GetBool())
		DebugDrawLine(tr.startpos, tr.endpos, 255, 0, 0, 1, -1.0);

	UpdateSoundPosition( tr.startpos, tr.endpos);
	*(float *)&pFirstPortal = 0.0;
	if (UTIL_DidTraceTouchPortals(ray, tr, &pFirstPortal, 0)
		&& *(float *)&pFirstPortal != 0.0
		&& pFirstPortal->IsActivedAndLinked())
	{
		v24 = StrikeEntitiesAlongLaser( tr.startpos, tr.endpos, &vecNewTermPoint_0 );
		v25 = vecStart.x;
		if (v24)
		{
			m_vStartPoint = vecStart;
			m_vEndPoint = vecNewTermPoint_0;
		}
		else
		{
		
			m_vStartPoint = vecStart;
			m_vEndPoint = tr.endpos;

			BeamDamage( &tr );
			rayTransformed.m_pWorldAxisTransform = 0;
			UTIL_Portal_RayTransform( pFirstPortal->m_matrixThisToLinked, ray, rayTransformed );
			vecDirection_0 = rayTransformed.m_Delta;
			VectorNormalize(vecDirection_0);
			UTIL_Portal_PointTransform(pFirstPortal->m_matrixThisToLinked, tr.endpos, vecStartPos);
			UpdateNextLaser( vecStartPos, vecDirection_0, 0 );
		}
		goto LABEL_56;
	}
	if ( StrikeEntitiesAlongLaser(tr.startpos, tr.endpos, &vecNewTermPoint) )
	{
		m_vStartPoint = vecStart;
		m_vEndPoint = vecNewTermPoint;

		goto LABEL_56;
	}
	
	m_vStartPoint = vecStart;
	m_vEndPoint = tr.endpos;

	if (tr.m_pEnt)
	{
		CBaseEntity *pHitEntity = tr.m_pEnt;
		if ( CPhysicsShadowClone::IsShadowClone(pHitEntity) )
		{
			EHANDLE pCloned = ( (CPhysicsShadowClone *)pHitEntity )->GetClonedEntity();
			pHitEntity = pCloned;
		}
		v28 = UTIL_GetSchrodingerTwin(pHitEntity);
		if (v28)
			pHitEntity = v28;
		if ( ReflectLaserFromEntity( pHitEntity ) )
		{
		LABEL_56:
			traceFilter.m_PassClassnames.m_Size = 0;
			traceFilter._vptr_ITraceFilter = (int(**)(...))&off_BBE2C8;
			if (traceFilter.m_PassClassnames.m_Memory.m_nGrowSize < 0 || !traceFilter.m_PassClassnames.m_Memory.m_pMemory)
				return;
		LABEL_58:
			(*(void(__cdecl **)(_DWORD, const char **))(*g_pMemAlloc + 8))(
				g_pMemAlloc,
				traceFilter.m_PassClassnames.m_Memory.m_pMemory);
			return;
		}
	}
	
	RemoveChildLaser();
	if (m_pPlacementHelper)
		UTIL_SetOrigin(m_pPlacementHelper, tr.endpos);

	BeamDamage( &tr );
	traceFilter.m_PassClassnames.m_Size = 0;
	traceFilter._vptr_ITraceFilter = (int(**)(...))&off_BBE2C8;
	if (traceFilter.m_PassClassnames.m_Memory.m_nGrowSize >= 0 && traceFilter.m_PassClassnames.m_Memory.m_pMemory)
		goto LABEL_58;
#elif defined ( FIRELASER_MAC )


	char *p_vec; // esi
	CPortalLaser *v9; // edi
	Vector *p_vNewDirection; // esi
	CBaseEntity *v11; // eax
	CBaseEntity *v12; // ecx
	CBaseEntity *v13; // ebx
	bool v14; // al
	int v15; // eax
	bool v16; // cl
	float v17; // xmm0_4
	float v18; // xmm1_4
	float v19; // xmm2_4
	float x; // xmm3_4
	float y; // xmm4_4
	float z; // xmm5_4
	float v23; // xmm0_4
	bool v24; // al
	bool v25; // al
	float v26; // xmm0_4
	float v27; // xmm1_4
	//CPortalLaser *p_y; // edi
	float p_y;
	bool v29; // al
	float *v30; // ecx
	float v31; // xmm0_4
	float v32; // xmm1_4
	float *v33; // esi
	float *v34; // esi
	float *v35; // esi
	bool IsShadowClone; // al
	int v37; // ecx
	int v38; // edx
	CPropWeightedCube *SchrodingerTwin; // eax
	CPortalLaser *m_pChildLaser; // eax
	CBaseEntity *m_pPlacementHelper; // eax
	vec_t *v42; // eax
	bool v43; // zf
	vec_t *v44; // edi
	vec_t *v45; // ebx
	Vector *v46; // ebx
	vec_t v47; // xmm2_4
	vec_t v48; // xmm0_4
	CBaseEntity *v49; // ebx
	char v50; // cl
	float v51; // xmm0_4
	trace_t *m_pEnt; // esi
	float v53; // xmm1_4
	float v54; // xmm2_4
	CPortalLaser *v55; // ebx
	bool v56; // al
	int v57; // ecx
	int v58; // edx
	int v59; // eax
	CBaseEntity *v60; // eax
	float v61; // [esp+2Ch] [ebp-2ECh]
	float v62; // [esp+30h] [ebp-2E8h]
	float *v63; // [esp+34h] [ebp-2E4h]
	float v64; // [esp+34h] [ebp-2E4h]
	char v65; // [esp+38h] [ebp-2E0h]
	CBaseEntity *pEntity; // [esp+3Ch] [ebp-2DCh]
	CBaseEntity *pEntitya; // [esp+3Ch] [ebp-2DCh]
	EHANDLE v68; // [esp+48h] [ebp-2D0h] BYREF
	Vector vecNewTermPoint; // [esp+50h] [ebp-2C8h] BYREF
	Vector vecStartPos; // [esp+60h] [ebp-2B8h] BYREF
	Vector vec; // [esp+70h] [ebp-2A8h] BYREF
	Ray_t rayTransformed; // [esp+80h] [ebp-298h]
	Vector vecNewTermPoint_0; // [esp+D8h] [ebp-240h] BYREFpFirstPortal
	trace_t tr_0; // [esp+E8h] [ebp-230h] BYREF
	Ray_t ray; // [esp+140h] [ebp-1D8h] BYREF
	EHANDLE v78; // [esp+1B8h] [ebp-160h] BYREF
	PortalLaserInfoList_t secondInfoList_0; // [esp+1C0h] [ebp-158h] BYREF
	PortalLaserInfoList_t secondInfoList; // [esp+1E0h] [ebp-138h] BYREF
	float flNewBeamLength; // [esp+1FCh] [ebp-11Ch] BYREF
	Vector vNewDirection; // [esp+200h] [ebp-118h] BYREF
	trace_t tempTrace; // [esp+210h] [ebp-108h] BYREF
	PortalLaserInfoList_t firstInfoList; // [esp+270h] [ebp-A8h] BYREF
	Vector vDir; // [esp+290h] [ebp-88h] BYREF
	Vector vAutoAimOffset; // [esp+2A0h] [ebp-78h] BYREF
	trace_t tr; // [esp+2B0h] [ebp-68h] BYREF
	float flTotalBeamLength; // [esp+304h] [ebp-14h] BYREF
	Vector a3;

	p_vec = (char *)a2;
	if (!new_portal_laser.GetBool())
	{
		CTraceFilterSimpleClassnameList traceFilter( this, COLLISION_GROUP_NONE ); // [esp+190h] [ebp-188h] BYREF
		
		traceFilter.AddClassnameToIgnore("projected_wall_entity");
		traceFilter.AddClassnameToIgnore("player");
		traceFilter.AddClassnameToIgnore("point_laser_target");
		v17 = *(float *)((char *)&loc_2E513F + 5800977);
		v18 = *a4 * v17;
		v19 = a4[1] * v17;
		x = a3->x;
		y = a3->y;
		z = a3->z;
		v23 = (float)(v17 * a4[2]) + z;
		ray.m_Delta.x = (float)(v18 + x) - x;
		ray.m_Delta.y = (float)(v19 + y) - y;
		ray.m_Delta.z = v23 - z;
		ray.m_IsSwept = (float)((float)(ray.m_Delta.z * ray.m_Delta.z)
			+ (float)((float)(ray.m_Delta.y * ray.m_Delta.y) + (float)(ray.m_Delta.x * ray.m_Delta.x))) != 0.0;
		memset(&ray.m_Extents, 0, 12);
		ray.m_pWorldAxisTransform = 0;
		ray.m_IsRay = 1;
		memset(&ray.m_StartOffset, 0, 12);
		ray.m_Start.x = x;
		ray.m_Start.y = y;
		ray.m_Start.z = z;
		enginetrace->TraceRay( ray, 1174421505, &traceFilter, &tr_0 );

		if (r_visualizetraces.GetBool())
			DebugDrawLine(tr_0.startpos, tr_0.endpos, 255, 0, 0, true, 0);
		
		UpdateSoundPosition(tr_0.startpos, tr_0.endpos);
		CPortal_Base2D *pFirstPortal = NULL;
		v24 = UTIL_DidTraceTouchPortals(ray, tr_0, &pFirstPortal, NULL );
		if (pFirstPortal != 0 && v24 && pFirstPortal->IsActivedAndLinked())
		{
			v25 = StrikeEntitiesAlongLaser(tr_0.startpos, tr_0.endpos, &vecNewTermPoint_0);
			v26 = a3->x;
			v27 = a2->m_vStartPoint.m_Value.x;
			if (v25)
			{
				if (v26 == v27)
				{
					p_y = m_vStartPoint.m_Value.y;
					if (a3->y == a2->m_vStartPoint.m_Value.y && a3->z == a2->m_vStartPoint.m_Value.z)
						goto LABEL_59;
				}
				else
				{
					p_y = m_vStartPoint.m_Value.y;
				}

				m_vStartPoint = a3;

			LABEL_59:
				if (vecNewTermPoint_0.x == a2->m_vEndPoint.m_Value.x)
				{
					v44 = &a2->m_vEndPoint.m_Value.y;
					if (vecNewTermPoint_0.y == a2->m_vEndPoint.m_Value.y && vecNewTermPoint_0.z == a2->m_vEndPoint.m_Value.z)
						goto LABEL_73;
				}
				else
				{
					v44 = &a2->m_vEndPoint.m_Value.y;
				}
				CBaseEntity::NetworkStateChanged(a2, a2);
				a2->m_vEndPoint.m_Value.x = vecNewTermPoint_0.x;
				*v44 = vecNewTermPoint_0.y;
				a2->m_vEndPoint.m_Value.z = vecNewTermPoint_0.z;
			LABEL_73:
				traceFilter._vptr$ITraceFilter = (int(**)(void))(&`vtable for'CTraceFilterSimpleClassnameList + 2);
					CUtlVector<char const*, CUtlMemory<char const*, int>>::~CUtlVector((CUtlVector<const char *, CUtlMemory<const char *, int> > *)p_vec);
				return;
			}
			if (v26 == v27)
			{
				v42 = &a2->m_vStartPoint.m_Value.y;
				if (a3->y == a2->m_vStartPoint.m_Value.y && a3->z == a2->m_vStartPoint.m_Value.z)
				{
				LABEL_64:
					if (tr_0.endpos.x == *((float *)p_vec + 482))
					{
						v45 = (vec_t *)(p_vec + 1932);
						if (tr_0.endpos.y == *((float *)p_vec + 483) && tr_0.endpos.z == *((float *)p_vec + 484))
							goto LABEL_72;
					}
					else
					{
						v45 = (vec_t *)(p_vec + 1932);
					}
					CBaseEntity::NetworkStateChanged((CBaseEntity *)p_vec, p_vec);
					*((_DWORD *)p_vec + 482) = LODWORD(tr_0.endpos.x);
					*v45 = tr_0.endpos.y;
					*((_DWORD *)p_vec + 484) = LODWORD(tr_0.endpos.z);
				LABEL_72:
					BeamDamage( &tr_0 );
					rayTransformed.m_pWorldAxisTransform = 0;
					UTIL_Portal_RayTransform( pFirstPortal->m_matrixThisToLinked, ray, rayTransformed );
					vec = rayTransformed.m_Delta;
					p_vec = (char *)&vec;
					VectorNormalize(vec);
					UTIL_Portal_PointTransform(pFirstPortal->m_matrixThisToLinked, tr_0.endpos, vecStartPos);
					UpdateNextLaser(vecStartPos, vec, 0);
					goto LABEL_73;
				}
			}
			else
			{
				v42 = &a2->m_vStartPoint.m_Value.y;
			}
			pEntitya = (CBaseEntity *)v42;
			CBaseEntity::NetworkStateChanged(a2, a2);
			p_vec = (char *)a2;
			a2->m_vStartPoint.m_Value.x = a3->x;
			pEntitya->_vptr$IHandleEntity = (int(**)(void))LODWORD(a3->y);
			a2->m_vStartPoint.m_Value.z = a3->z;
			goto LABEL_64;
		}
		v29 = CPortalLaser::StrikeEntitiesAlongLaser(a2, &tr_0.startpos, &tr_0.endpos, &vecNewTermPoint);
		v30 = (float *)((char *)a2 + (_DWORD)&loc_588B8D - 5800977);
		v31 = a3->x;
		v32 = *v30;
		if (v29)
		{
			if (v31 == v32)
			{
				v33 = (float *)((char *)a2 + (_DWORD)&loc_588B8D - 5800973);
				if (a3->y == *v33 && a3->z == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977))
					goto LABEL_24;
			}
			else
			{
				v33 = (float *)((char *)a2 + (_DWORD)&loc_588B8D - 5800973);
			}
			CBaseEntity::NetworkStateChanged((CBaseEntity *)(v30 - 479), v30 - 479);
			*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B8D - 5800977) = (int(**)(void))LODWORD(a3->x);
			*v33 = a3->y;
			*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977) = (int(**)(void))LODWORD(a3->z);
		LABEL_24:
			if (vecNewTermPoint.x == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800973))
			{
				p_vec = (char *)a2 + (_DWORD)&loc_588B9A - 5800974;
				if (vecNewTermPoint.y == *(float *)p_vec
					&& vecNewTermPoint.z == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B9E - 5800974))
				{
					goto LABEL_73;
				}
			}
			else
			{
				p_vec = (char *)a2 + (_DWORD)&loc_588B9A - 5800974;
			}
			CBaseEntity::NetworkStateChanged(
				(CBaseEntity *)((char *)a2 + (_DWORD)&loc_588B95 - 5800973 - 1928),
				(char *)a2 + (_DWORD)&loc_588B95 - 5800973 - 1928);
			*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800973) = (int(**)(void))LODWORD(vecNewTermPoint.x);
			*(vec_t *)p_vec = vecNewTermPoint.y;
			*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B9E - 5800974) = (int(**)(void))LODWORD(vecNewTermPoint.z);
			goto LABEL_73;
		}
		if (v31 == v32)
		{
			v34 = (float *)((char *)a2 + (_DWORD)&loc_588B8D - 5800973);
			if (a3->y == *v34 && a3->z == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977))
			{
			LABEL_29:
				if (tr_0.endpos.x == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800973))
				{
					v35 = (float *)((char *)a2 + (_DWORD)&loc_588B9A - 5800974);
					if (tr_0.endpos.y == *v35
						&& tr_0.endpos.z == *(float *)((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B9E - 5800974))
					{
						goto LABEL_37;
					}
				}
				else
				{
					v35 = (float *)((char *)a2 + (_DWORD)&loc_588B9A - 5800974);
				}
				CBaseEntity::NetworkStateChanged(
					(CBaseEntity *)((char *)a2 + (_DWORD)&loc_588B95 - 5800973 - 1928),
					(char *)a2 + (_DWORD)&loc_588B95 - 5800973 - 1928);
				*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800973) = (int(**)(void))LODWORD(tr_0.endpos.x);
				*v35 = tr_0.endpos.y;
				*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B9E - 5800974) = (int(**)(void))LODWORD(tr_0.endpos.z);
			LABEL_37:
				if (!tr_0.m_pEnt)
					goto LABEL_45;
				IsShadowClone = CPhysicsShadowClone::IsShadowClone(tr_0.m_pEnt);
				p_vec = (char *)tr_0.m_pEnt;
				if (IsShadowClone)
				{
					v68 = ((CPhysicsShadowClone *)tr_0.m_pEnt)->CPhysicsShadowClone::GetClonedEntity();
					p_vec = 0;
					if (v68.m_Index != -1)
					{
						v37 = **(_DWORD **)((char *)&loc_4BCC23 + 5800977);
						v38 = 24 * LOWORD(v68.m_Index);
						if (*(_DWORD *)(v37 + v38 + 8) == HIWORD(v68.m_Index))
							p_vec = *(char **)(v37 + v38 + 4);
					}
				}
				SchrodingerTwin = UTIL_GetSchrodingerTwin((CBaseEntity *)a3);
				if (SchrodingerTwin)
					p_vec = (char *)SchrodingerTwin;
				if (!ReflectLaserFromEntity(a2))
				{
				LABEL_45:
					p_vec = (char *)a2;
					m_pChildLaser = a2->m_pChildLaser;
					if (m_pChildLaser)
					{
						m_pChildLaser->m_pParentLaser = 0;
						UTIL_Remove(m_pChildLaser);
						a2->m_pChildLaser = 0;
					}
					m_pPlacementHelper = a2->m_pPlacementHelper;
					if (m_pPlacementHelper)
						UTIL_SetOrigin(m_pPlacementHelper, tr_0.endpos, 0);
					BeamDamage( (trace_t *)a2 );
				}
				goto LABEL_73;
			}
		}
		else
		{
			v34 = (float *)((char *)a2 + (_DWORD)&loc_588B8D - 5800973);
		}
		CBaseEntity::NetworkStateChanged((CBaseEntity *)(v30 - 479), v30 - 479);
		*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B8D - 5800977) = (int(**)(void))LODWORD(a3->x);
		*v34 = a3->y;
		*(int(***)(void))((char *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977) = (int(**)(void))LODWORD(a3->z);
		goto LABEL_29;
	}
	flTotalBeamLength = 0.0;
	vDir.z = a4[2];
	*(_QWORD *)&vDir.x = *(_QWORD *)a4;
	v9 = a2;
	if (!a2->m_bAutoAimEnabled)
	{
	LABEL_79:
		memset(&secondInfoList_0, 0, sizeof(secondInfoList_0));
		UTIL_ClearTrace(&tr);
		v49 = TraceLaser( 0, a3, &vDir, &flTotalBeamLength, &tr, &secondInfoList_0, 0);
		DamageEntitiesAlongLaser(secondInfoList_0);
		pEntity = v49;
		CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::~CUtlVector((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *)v9);
		v46 = a3;
		v63 = &a3->y;
		v50 = 0;
		goto LABEL_80;
	}
	memset(&firstInfoList, 0, sizeof(firstInfoList));
	p_vNewDirection = a3;
	v11 = CPortalLaser::TraceLaser(a2, 1, a3, &vDir, &flTotalBeamLength, &tr, &firstInfoList, &vAutoAimOffset);
	v12 = v11;
	if (!*((_BYTE *)&a2->_vptr$IHandleEntity + (_DWORD)&loc_588B84 - 5800977))
	goto LABEL_78;
	if (!v11)
	goto LABEL_78;
	if (v11->m_iClassname.pszValue != "point_laser_target")
	{
		v13 = v11;
		v14 = CBaseEntity::ClassMatchesComplex(v11, (const char *)a3);
		v12 = v13;
		if (!v14)
			goto LABEL_78;
	}
	pEntity = v12;
	v15 = __dynamic_cast(v12, &`typeinfo for'CBaseEntity, *(_DWORD *)&algn_4BD6CA[5800982], 0);
		v16 = v15 && *(_BYTE *)(v15 + 929) != 0;
	if (a2->m_bFromReflectedCube && !sv_laser_cube_autoaim.m_pParent->m_Value.m_nValue)
	{
		v9 = a2;
		v43 = ((*((unsigned __int8(__cdecl **)(CGameRules *))g_pGameRules->_vptr$IGameSystem + 34))(g_pGameRules)& v16) == 0;
	}
	else
	{
		v43 = !v16;
		v9 = a2;
	}
	if (v43)
	{
	LABEL_78:
		CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::~CUtlVector((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *)p_vNewDirection);
		goto LABEL_79;
	}
	v46 = a3;
	v47 = (float)((float)((float)(a4[1] * flTotalBeamLength) + a3->y) + vAutoAimOffset.y) - a3->y;
	v48 = (float)((float)((float)(flTotalBeamLength * a4[2]) + a3->z) + vAutoAimOffset.z) - a3->z;
	vNewDirection.x = (float)((float)((float)(*a4 * flTotalBeamLength) + a3->x) + vAutoAimOffset.x) - a3->x;
	vNewDirection.y = v47;
	vNewDirection.z = v48;
	p_vNewDirection = &vNewDirection;
	VectorNormalize(&vNewDirection);
	memset(&secondInfoList, 0, sizeof(secondInfoList));
	if (pEntity != CPortalLaser::TraceLaser(v9, 0, a3, &vNewDirection, &flNewBeamLength, &tempTrace, &secondInfoList, 0))
	{
		CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::~CUtlVector((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *)&vNewDirection);
		goto LABEL_78;
	}
	tr = tempTrace;
	flTotalBeamLength = flNewBeamLength;
	vDir = vNewDirection;
	CPortalLaser::DamageEntitiesAlongLaser(v9, &secondInfoList, 1);
	v63 = &a3->y;
	CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::~CUtlVector((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *)&vNewDirection);
	CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int>>::~CUtlVector((CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > *)&vNewDirection);
	v50 = 1;
	LABEL_80:
	v51 = v46->x;
	if (v46->x == *(float *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B8D - 5800977))
	{
		v65 = v50;
		v53 = *v63;
		m_pEnt = (trace_t *)((char *)v9 + (_DWORD)&loc_588B8D - 5800973);
		if (*v63 == m_pEnt->startpos.x)
		{
			v54 = v46->z;
			if (v54 == *(float *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977))
				goto LABEL_85;
		}
	}
	else
	{
		v65 = v50;
		m_pEnt = (trace_t *)((char *)v9 + (_DWORD)&loc_588B8D - 5800973);
	}
	CBaseEntity::NetworkStateChanged(
		(CBaseEntity *)((char *)v9 + (_DWORD)&loc_588B8D - 5800977 - 1916),
		(char *)v9 + (_DWORD)&loc_588B8D - 5800977 - 1916);
	*(int(***)(void))((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B8D - 5800977) = (int(**)(void))LODWORD(v46->x);
	m_pEnt->startpos.x = v46->y;
	*(int(***)(void))((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800977) = (int(**)(void))LODWORD(v46->z);
	v51 = v46->x;
	v53 = v46->y;
	v54 = v46->z;
	LABEL_85:
	if ((float)((float)(vDir.x * flTotalBeamLength) + v51) != *(float *)((char *)&v9->_vptr$IHandleEntity
		+ (_DWORD)&loc_588B95
		- 5800973)
		|| (float)((float)(vDir.y * flTotalBeamLength) + v53) != *(float *)((char *)&v9->_vptr$IHandleEntity
		+ (_DWORD)&loc_588B9A
		- 5800974)
		|| (float)((float)(flTotalBeamLength * vDir.z) + v54) != *(float *)((char *)&v9->_vptr$IHandleEntity
		+ (_DWORD)&loc_588B9E
		- 5800974))
	{
		v64 = (float)(flTotalBeamLength * vDir.z) + v54;
		v61 = (float)(vDir.x * flTotalBeamLength) + v51;
		v62 = (float)(vDir.y * flTotalBeamLength) + v53;
		CBaseEntity::NetworkStateChanged(
			(CBaseEntity *)((char *)v9 + (_DWORD)&loc_588B95 - 5800973 - 1928),
			(char *)v9 + (_DWORD)&loc_588B95 - 5800973 - 1928);
		*(float *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B95 - 5800973) = v61;
		*(float *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B9A - 5800974) = v62;
		*(float *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B9E - 5800974) = v64;
	}
	v55 = (CPortalLaser *)pEntity;
	if (*((_BYTE *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588BA3 - 5800973) != v65)
	{
		m_pEnt = (trace_t *)((char *)v9 + (_DWORD)&loc_588BA3 - 5800973);
		CBaseEntity::NetworkStateChanged((CBaseEntity *)&m_pEnt[-24].worldSurfaceIndex, &m_pEnt[-24].worldSurfaceIndex);
		LOBYTE(m_pEnt->startpos.x) = v65;
		v55 = (CPortalLaser *)pEntity;
	}
	if (!tr.m_pEnt)
	goto LABEL_97;
	v56 = CPhysicsShadowClone::IsShadowClone(tr.m_pEnt);
	m_pEnt = (trace_t *)tr.m_pEnt;
	if (v56)
	{
		CPhysicsShadowClone::GetClonedEntity(&v78, (CPhysicsShadowClone_0 *)tr.m_pEnt);
		m_pEnt = 0;
		if (v78.m_Index != -1)
		{
			v57 = **(_DWORD **)((char *)&loc_4BCC23 + 5800977);
			v58 = 24 * LOWORD(v78.m_Index);
			if (*(_DWORD *)(v57 + v58 + 8) == HIWORD(v78.m_Index))
				m_pEnt = *(trace_t **)(v57 + v58 + 4);
		}
	}
	UTIL_GetSchrodingerTwin(v55);
	if (!ReflectLaserFromEntity(v55))
	{
	LABEL_97:
		v59 = *(int *)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_5888DA - 5800974);
		if (v59)
		{
			*(_DWORD *)(v59 + 1224) = 0;
			UTIL_Remove((CBaseEntity *)v59);
			*(int(***)(void))((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_5888DA - 5800974) = 0;
		}
		if (!v55
			|| v55->GetClassname() != "point_laser_target"
			&& !v55->ClassMatches((const char *)m_pEnt))
		{
			v60 = *(CBaseEntity **)((char *)&v9->_vptr$IHandleEntity + (_DWORD)&loc_588B75 - 5800977);
			if (v60)
				UTIL_SetOrigin(v60, tr.endpos, 0);
			BeamDamage(m_pEnt);
		}
	}
	

#endif
}