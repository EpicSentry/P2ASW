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
#include "trigger_tractorbeam_shared.h"

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

	CNetworkVar( StickCameraState, m_nStickCameraState );
	CNetworkVar( InAirState, m_InAirState );
	CNetworkVar( PaintPowerType, m_PaintedPowerType );

	CNetworkVar( int, m_nLocatorEntityIndices ); // FIXME: Needs to be array!

	CNetworkVar( bool, m_bShowingViewFinder );
	CNetworkVar( bool, m_bPlacingPhoto );
	CNetworkVar( bool, m_bDoneStickInterp );
	CNetworkVar( bool, m_bDoneCorrectPitch );
	CNetworkVar( bool, m_bJumpedThisFrame );
	CNetworkVar( bool, m_bBouncedThisFrame );
	CNetworkVar( bool, m_bDuckedInAir );
	CNetworkVar( bool, m_bZoomedIn );
	CNetworkVar( bool, m_bPreventedCrouchJumpThisFrame );

	bool m_bAttemptHullResize;

	CNetworkVar( float, m_flAirControlSupressionTime );
	CNetworkVar( float, m_flAirInputScale );
	CNetworkVar( float, m_flCurrentStickTime );
	CNetworkVar( float, m_fBouncedTime );

	// Fixme?
	//CNetworkVar( CountdownTimer, m_PaintedPowerTimer );

	CountdownTimer m_PaintedPowerTimer;

	CachedPaintPowerChoiceResult m_CachedPaintPowerChoiceResults[PAINT_POWER_TYPE_COUNT];
	
	CNetworkVector( m_StickNormal );
	CNetworkVector( m_OldStickNormal );
	CNetworkVector( m_vPreUpdateVelocity );
	CNetworkVector( m_Up );
	CNetworkVector( m_vStickRotationAxis );
	
	CNetworkVector( m_StandHullMin );
	CNetworkVector( m_StandHullMax );
	CNetworkVector( m_DuckHullMin );
	CNetworkVector( m_DuckHullMax );
	CNetworkVector( m_CachedStandHullMinAttempt );
	CNetworkVector( m_CachedStandHullMaxAttempt );
	CNetworkVector( m_CachedDuckHullMinAttempt );
	CNetworkVector( m_CachedDuckHullMaxAttempt );
	
	CNetworkVector( m_vLocalUp );	
	CNetworkVector( m_vEyeOffset );

	CNetworkQAngle( m_qQuaternionPunch );
	
#ifndef NO_TRACTOR_BEAM
	CNetworkHandle( CTrigger_TractorBeam, m_hTractorBeam )
#else
	CNetworkHandle( CBaseEntity, m_hTractorBeam ) // Change to CTrigger_TractorBeam when we're ready
#endif

    int m_nTractorBeamCount;
};

EXTERN_SEND_TABLE(DT_PortalLocal);


#endif // PORTAL_PLAYERLOCALDATA_H
