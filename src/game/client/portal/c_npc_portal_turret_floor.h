#ifndef NPC_PORTAL_TURRET_FLOOR_H
#define NPC_PORTAL_TURRET_FLOOR_H

#ifdef WIN32
#pragma once
#endif

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
#include <portal_grabcontroller_shared.h>

#define FLOOR_TURRET_PORTAL_EYE_ATTACHMENT 1
#define FLOOR_TURRET_PORTAL_LASER_ATTACHMENT 2
#define FLOOR_TURRET_PORTAL_LASER_RANGE 8192

#define FLOOR_TURRET_PORTAL_END_POINT_PULSE_SCALE 4.0f


class C_NPC_Portal_FloorTurret : public C_AI_BaseNPC, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS( C_NPC_Portal_FloorTurret, C_AI_BaseNPC );
	DECLARE_CLIENTCLASS();

	virtual ~C_NPC_Portal_FloorTurret( void );

	virtual void	Spawn( void );
	virtual void	ClientThink( void );

	bool	IsLaserOn( void ) { return m_pBeam != NULL; }
	void	LaserOff( void );
	void	LaserOn( void );
	float	LaserEndPointSize( void );

	bool	HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	QAngle	PreferredCarryAngles() { return QAngle(-45.0f,0.0f,0.0f); }
	bool	ShouldPredict();
	C_BasePlayer* GetPredictionOwner() { return GetPlayerHoldingEntity(this); }

private:
	CBeam	*m_pBeam;

	bool	m_bOutOfAmmo;
	bool	m_bLaserOn;
	int		m_sLaserHaloSprite;
	float	m_fPulseOffset;

	float	m_bBeamFlickerOff;
	float	m_fBeamFlickerTime;

};

#endif //NPC_PORTAL_TURRET_FLOOR_H
