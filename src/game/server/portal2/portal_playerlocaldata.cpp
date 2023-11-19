//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "portal_playerlocaldata.h"
#include "portal_player.h"
#include "mathlib/mathlib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SEND_TABLE_NOBASE( CPortalPlayerLocalData, DT_PortalLocal ) // Purpose: Not done (Line 16)

SendPropBool( SENDINFO( m_bShowingViewFinder ) ),
SendPropFloat( SENDINFO( m_flAirControlSupressionTime ) ),
SendPropInt( SENDINFO( m_nLocatorEntityIndices ) ),

SendPropInt( SENDINFO( m_nLocatorEntityIndices ) ), // FIXME: Needs to be array!

SendPropBool( SENDINFO( m_bPlacingPhoto ) ),

SendPropVector( SENDINFO( m_StickNormal ) ),
SendPropVector( SENDINFO( m_OldStickNormal ) ),
SendPropVector( SENDINFO( m_vPreUpdateVelocity ) ),
SendPropVector( SENDINFO( m_Up ) ),
SendPropVector( SENDINFO( m_vStickRotationAxis ) ),
SendPropVector( SENDINFO( m_StandHullMin ) ),
SendPropVector( SENDINFO( m_StandHullMax ) ),
SendPropVector( SENDINFO( m_DuckHullMin ) ),
SendPropVector( SENDINFO( m_DuckHullMax ) ),
SendPropVector( SENDINFO( m_CachedStandHullMinAttempt ) ),
SendPropVector( SENDINFO( m_CachedStandHullMaxAttempt ) ),
SendPropVector( SENDINFO( m_CachedDuckHullMinAttempt ) ),
SendPropVector( SENDINFO( m_CachedDuckHullMaxAttempt ) ),
SendPropVector( SENDINFO( m_vLocalUp ) ),
SendPropVector( SENDINFO( m_vEyeOffset ) ),

SendPropQAngles( SENDINFO( m_qQuaternionPunch ) ),

SendPropInt( SENDINFO( m_PaintedPowerType ) ),

// SendPropDataTable( SENDINFO( m_PaintedPowerTimer ) ), // FIXME!

SendPropFloat( SENDINFO( m_flCurrentStickTime ) ),
SendPropFloat( SENDINFO( m_flAirInputScale ) ),

SendPropInt( SENDINFO( m_nStickCameraState ) ),
SendPropInt( SENDINFO( m_InAirState ) ),

SendPropBool( SENDINFO( m_bDoneStickInterp ) ),
SendPropBool( SENDINFO( m_bDoneCorrectPitch ) ),
SendPropBool( SENDINFO( m_bJumpedThisFrame ) ),
SendPropBool( SENDINFO( m_bBouncedThisFrame ) ),
SendPropBool( SENDINFO( m_bDuckedInAir ) ),

SendPropEHandle( SENDINFO( m_hTractorBeam ) ),

SendPropBool( SENDINFO( m_bZoomedIn ) ),

SendPropFloat( SENDINFO( m_fBouncedTime ) ),

SendPropBool( SENDINFO( m_bPreventedCrouchJumpThisFrame ) ),

END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC( CPortalPlayerLocalData )

DEFINE_FIELD( m_nStickCameraState, FIELD_INTEGER ),
DEFINE_FIELD( m_InAirState, FIELD_INTEGER ),
DEFINE_FIELD( m_PaintedPowerType, FIELD_INTEGER ),
DEFINE_FIELD( m_nLocatorEntityIndices, FIELD_INTEGER ),

DEFINE_FIELD( m_bShowingViewFinder, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bPlacingPhoto, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bDoneStickInterp, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bDoneCorrectPitch, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bAttemptHullResize, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bJumpedThisFrame, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bBouncedThisFrame, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bDuckedInAir, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bZoomedIn, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bPreventedCrouchJumpThisFrame, FIELD_BOOLEAN ),

DEFINE_FIELD( m_flAirControlSupressionTime, FIELD_FLOAT ),
DEFINE_FIELD( m_flAirInputScale, FIELD_FLOAT ),
DEFINE_FIELD( m_flCurrentStickTime, FIELD_FLOAT ),
DEFINE_FIELD( m_fBouncedTime, FIELD_FLOAT ),

DEFINE_FIELD( m_StickNormal, FIELD_VECTOR ),
DEFINE_FIELD( m_OldStickNormal, FIELD_VECTOR ),
DEFINE_FIELD( m_vPreUpdateVelocity, FIELD_VECTOR ),
DEFINE_FIELD( m_Up, FIELD_VECTOR ),
DEFINE_FIELD( m_vStickRotationAxis, FIELD_VECTOR ),
DEFINE_FIELD( m_StandHullMin, FIELD_VECTOR ),
DEFINE_FIELD( m_StandHullMax, FIELD_VECTOR ),
DEFINE_FIELD( m_DuckHullMin, FIELD_VECTOR ),
DEFINE_FIELD( m_DuckHullMax, FIELD_VECTOR ),
DEFINE_FIELD( m_CachedStandHullMinAttempt, FIELD_VECTOR ),
DEFINE_FIELD( m_CachedStandHullMaxAttempt, FIELD_VECTOR ),
DEFINE_FIELD( m_CachedDuckHullMinAttempt, FIELD_VECTOR ),
DEFINE_FIELD( m_CachedDuckHullMaxAttempt, FIELD_VECTOR ),
DEFINE_FIELD( m_vLocalUp, FIELD_VECTOR ),
DEFINE_FIELD( m_vEyeOffset, FIELD_VECTOR ),

DEFINE_FIELD( m_qQuaternionPunch, FIELD_VECTOR ),

DEFINE_FIELD( m_hTractorBeam, FIELD_EHANDLE ),

END_DATADESC()

const Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 );

CPortalPlayerLocalData::CPortalPlayerLocalData()
{
	m_StickNormal = DEFAULT_SURFACE_NORMAL;
	m_OldStickNormal = DEFAULT_SURFACE_NORMAL;
	m_Up = DEFAULT_SURFACE_NORMAL;
	m_vStickRotationAxis = vec3_origin;
	m_vLocalUp = DEFAULT_SURFACE_NORMAL;

	m_StandHullMin = VEC_HULL_MIN;
	m_StandHullMax = VEC_HULL_MAX;
	m_DuckHullMin = VEC_DUCK_HULL_MIN;
	m_DuckHullMax = VEC_DUCK_HULL_MAX;
	m_CachedStandHullMinAttempt = VEC_HULL_MIN;
	m_CachedStandHullMaxAttempt = VEC_HULL_MAX;
	m_CachedDuckHullMinAttempt = VEC_DUCK_HULL_MIN;
	m_CachedDuckHullMaxAttempt = VEC_DUCK_HULL_MAX;

	m_vEyeOffset = vec3_origin;

	m_qQuaternionPunch = vec3_angle;

	m_bShowingViewFinder = false;
	m_bPlacingPhoto = false;
	m_bDoneStickInterp = false;
	m_bDoneCorrectPitch = false;
	m_bJumpedThisFrame = false;
	m_bBouncedThisFrame = false;
	m_bDuckedInAir = false;
	m_bZoomedIn = false;
	m_bPreventedCrouchJumpThisFrame = false;

	m_vPreUpdateVelocity = vec3_origin;

	m_nStickCameraState = STICK_CAMERA_UPRIGHT;
	m_PaintedPowerType = NO_POWER;
	m_InAirState = ON_GROUND;

	m_bAttemptHullResize = false;

	m_flAirControlSupressionTime = 0.0;
	m_flAirInputScale = 1.0;
	m_flCurrentStickTime = 0.5;
	m_fBouncedTime = 0.0;

	m_hTractorBeam = NULL;

	m_nTractorBeamCount = 0;
}
