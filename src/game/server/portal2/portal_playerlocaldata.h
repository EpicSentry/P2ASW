//========= Copyright  1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef PORTAL_PLAYERLOCALDATA_H
#define PORTAL_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"

#include "portal_player_shared.h"   // PortalPlayerStatistics_t define
#include "portal_shareddefs.h"      // USE_SLOWTIME define

class CTrigger_TractorBeam;
//-----------------------------------------------------------------------------
// Purpose: Player specific data for Portal ( sent only to local player, too )
//-----------------------------------------------------------------------------
class CPortalPlayerLocalData
{
public:
    // Save/restore
    DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CPortalPlayerLocalData );
	DECLARE_EMBEDDED_NETWORKVAR();

    CPortalPlayerLocalData();

#if USE_SLOWTIME
    CNetworkVar( bool, m_bSlowingTime );
    CNetworkVar( float, m_flSlowTimeRemaining );
    CNetworkVar( float, m_flSlowTimeMaximum );
#endif // USE_SLOWTIME

	StickCameraState m_nStickCameraState;
	InAirState m_InAirState;
	PaintPowerType m_PaintedPowerType;

	int m_nLocatorEntityIndices;

	bool m_bShowingViewFinder;
	bool m_bPlacingPhoto;
	bool m_bDoneStickInterp;
	bool m_bDoneCorrectPitch;
	bool m_bAttemptHullResize;
	bool m_bJumpedThisFrame;
	bool m_bBouncedThisFrame;
	bool m_bDuckedInAir;
	bool m_bZoomedIn;
	bool m_bPreventedCrouchJumpThisFrame;

	float m_flAirControlSupressionTime;
	float m_flAirInputScale;
	float m_flCurrentStickTime;
	float m_fBouncedTime;

	CountdownTimer m_PaintedPowerTimer;
	CachedPaintPowerChoiceResult m_CachedPaintPowerChoiceResults[PAINT_POWER_TYPE_COUNT];
	
	Vector m_StickNormal;
	Vector m_OldStickNormal;
	Vector m_vPreUpdateVelocity;
	Vector m_Up;
	Vector m_vStickRotationAxis;

	Vector m_StandHullMin;
	Vector m_StandHullMax;
	Vector m_DuckHullMin;
	Vector m_DuckHullMax;
	Vector m_CachedStandHullMinAttempt;
	Vector m_CachedStandHullMaxAttempt;
	Vector m_CachedDuckHullMinAttempt;
	Vector m_CachedDuckHullMaxAttempt;

	Vector m_vLocalUp;
	Vector m_vEyeOffset;

	QAngle m_qQuaternionPunch;
	
	EHANDLE m_hTractorBeam; // Was probably CHandle<> instead
    int m_nTractorBeamCount;
};

EXTERN_SEND_TABLE(DT_PortalLocal);


#endif // PORTAL_PLAYERLOCALDATA_H
