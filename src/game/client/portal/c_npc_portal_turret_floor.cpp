//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "c_ai_basenpc.h"
#include "beam_shared.h"
#include "prop_portal_shared.h"
#include "player_pickup.h"
#include "c_npc_portal_turret_floor.h"


IMPLEMENT_CLIENTCLASS_DT( C_NPC_Portal_FloorTurret, DT_NPC_Portal_FloorTurret, CNPC_Portal_FloorTurret )

	RecvPropBool( RECVINFO( m_bOutOfAmmo ) ),
	RecvPropBool( RECVINFO( m_bLaserOn ) ),
	RecvPropBool( RECVINFO( m_bIsFiring ) ),
	RecvPropInt( RECVINFO( m_sLaserHaloSprite ) ),

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( npc_portal_turret_floor, C_NPC_Portal_FloorTurret )


C_NPC_Portal_FloorTurret::~C_NPC_Portal_FloorTurret( void )
{
	LaserOff();
}


void C_NPC_Portal_FloorTurret::Spawn( void )
{
	C_Beam *pBeam;

	SetThink( &C_NPC_Portal_FloorTurret::ClientThink );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	pBeam = C_Beam::BeamCreate("effects/redlaser1.vmt", 0.2);
	pBeam->SetRenderColor(255, 32, 32);
	SetRenderAlpha(255);
	pBeam->GetNoise();
	pBeam->GetWidth();
	pBeam->SetEndWidth(0.75f);
	pBeam->GetScrollRate();
	pBeam->GetFadeLength();
	pBeam->SetHaloScale(4.0f);
	pBeam->SetBeamFlag(FBEAM_REVERSED);
	m_beamHelper.Init(pBeam);

	m_fPulseOffset = RandomFloat( 0.0f, 2.0f * M_PI );

	m_bBeamFlickerOff = false;
	m_fBeamFlickerTime = 0.0f;

	BaseClass::Spawn();
}

void C_NPC_Portal_FloorTurret::ClientThink( void )
{
	if ( m_bOutOfAmmo && m_fBeamFlickerTime < gpGlobals->curtime )
	{
		m_fBeamFlickerTime = gpGlobals->curtime + RandomFloat( 0.05f, 0.3f );
		m_bBeamFlickerOff = !m_bBeamFlickerOff;
	}

	if ( m_bLaserOn && !m_bBeamFlickerOff )
		LaserOn();
	else
		LaserOff();
}

void C_NPC_Portal_FloorTurret::LaserOff( void )
{
	m_beamHelper.TurnOff();
}

void C_NPC_Portal_FloorTurret::LaserOn( void )
{
	C_Beam* pLastBeam;

	if ( !IsBoneAccessAllowed() )
	{
		LaserOff();
		return;
	}

	Vector vecMuzzle;
	QAngle angMuzzleDir;
	GetAttachment( FLOOR_TURRET_PORTAL_LASER_ATTACHMENT, vecMuzzle, angMuzzleDir );

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment( FLOOR_TURRET_PORTAL_EYE_ATTACHMENT, vecEye, angEyeDir );

	Vector vecMuzzleDir;
	AngleVectors( angEyeDir, &vecMuzzleDir );

	if ((vecMuzzleDir.x - m_vLastMuzzleDir.x) > 0.001f
		|| (vecMuzzleDir.y - m_vLastMuzzleDir.y) > 0.001f
		|| (vecMuzzleDir.z - m_vLastMuzzleDir.z) > 0.001f
		|| (vecMuzzle.x - m_vLastMuzzle.x) > 0.001f
		|| (vecMuzzle.y - m_vLastMuzzle.y) > 0.001f
		|| (vecMuzzle.z - m_vLastMuzzle.z) > 0.001f
		|| gpGlobals->curtime > this->m_flNextBeamUpdate)
	{
		UpdateBeam(vecMuzzle, vecMuzzleDir);
		m_flNextBeamUpdate = gpGlobals->curtime + 0.2;
	}
	if (m_beamHelper.GetLastBeam())
	{
		pLastBeam = m_beamHelper.GetLastBeam();
		pLastBeam->SetHaloScale(LaserEndPointSize());
		pLastBeam->SetHaloTexture(m_sLaserHaloSprite);
	}
}

float C_NPC_Portal_FloorTurret::LaserEndPointSize( void )
{
	return ( ( MAX( 0.0f, sinf( gpGlobals->curtime * M_PI + m_fPulseOffset ) ) ) * FLOOR_TURRET_PORTAL_END_POINT_PULSE_SCALE + 3.0f ) * ( IsX360() ? ( 3.0f ) : ( 1.5f ) );
}

bool C_NPC_Portal_FloorTurret::ShouldPredict()
{
	C_BasePlayer* pPredOwner = GetPlayerHoldingEntity(this);
	return (pPredOwner && pPredOwner->IsLocalPlayer()) ? true : BaseClass::ShouldPredict();
}

void C_NPC_Portal_FloorTurret::UpdateBeam(Vector& vecMuzzle, Vector& vecMuzzleDir)
{
	trace_t tr;
	Vector vEndPoint;

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);
	
	vEndPoint = vecMuzzle + vecMuzzleDir * 8192.0f;
	m_beamHelper.UpdatePoints(vecMuzzle, vEndPoint, MASK_SHOT, &traceFilter, &tr);
	m_vLastMuzzle = vecMuzzle;
	m_vLastMuzzleDir = vecMuzzleDir;
}
