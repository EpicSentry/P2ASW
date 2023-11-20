#include "cbase.h"
#include "c_portal_playerlocaldata.h"


BEGIN_RECV_TABLE_NOBASE( CPortalPlayerLocalData, DT_PortalLocal ) // Purpose: Not done (Line 16)

RecvPropBool( RECVINFO( m_bShowingViewFinder ) ),
RecvPropFloat( RECVINFO( m_flAirControlSupressionTime ) ),
RecvPropInt( RECVINFO( m_nLocatorEntityIndices ) ),

RecvPropInt( RECVINFO( m_nLocatorEntityIndices ) ), // FIXME: Needs to be array!

RecvPropBool( RECVINFO( m_bPlacingPhoto ) ),

RecvPropVector( RECVINFO( m_StickNormal ) ),
RecvPropVector( RECVINFO( m_OldStickNormal ) ),
RecvPropVector( RECVINFO( m_vPreUpdateVelocity ) ),
RecvPropVector( RECVINFO( m_Up ) ),
RecvPropVector( RECVINFO( m_vStickRotationAxis ) ),
RecvPropVector( RECVINFO( m_StandHullMin ) ),
RecvPropVector( RECVINFO( m_StandHullMax ) ),
RecvPropVector( RECVINFO( m_DuckHullMin ) ),
RecvPropVector( RECVINFO( m_DuckHullMax ) ),
RecvPropVector( RECVINFO( m_CachedStandHullMinAttempt ) ),
RecvPropVector( RECVINFO( m_CachedStandHullMaxAttempt ) ),
RecvPropVector( RECVINFO( m_CachedDuckHullMinAttempt ) ),
RecvPropVector( RECVINFO( m_CachedDuckHullMaxAttempt ) ),
RecvPropVector( RECVINFO( m_vLocalUp ) ),
RecvPropVector( RECVINFO( m_vEyeOffset ) ),

RecvPropQAngles( RECVINFO( m_qQuaternionPunch ) ),

RecvPropInt( RECVINFO( m_PaintedPowerType ) ),

// RecvPropDataTable( RECVINFO( m_PaintedPowerTimer ) ), // FIXME!

RecvPropFloat( RECVINFO( m_flCurrentStickTime ) ),
RecvPropFloat( RECVINFO( m_flAirInputScale ) ),

RecvPropInt( RECVINFO( m_nStickCameraState ) ),
RecvPropInt( RECVINFO( m_InAirState ) ),

RecvPropBool( RECVINFO( m_bDoneStickInterp ) ),
RecvPropBool( RECVINFO( m_bDoneCorrectPitch ) ),
RecvPropBool( RECVINFO( m_bJumpedThisFrame ) ),
RecvPropBool( RECVINFO( m_bBouncedThisFrame ) ),
RecvPropBool( RECVINFO( m_bDuckedInAir ) ),

RecvPropEHandle( RECVINFO( m_hTractorBeam ) ),

RecvPropBool( RECVINFO( m_bZoomedIn ) ),

RecvPropFloat( RECVINFO( m_fBouncedTime ) ),

RecvPropBool( RECVINFO( m_bPreventedCrouchJumpThisFrame ) ),

END_RECV_TABLE()


BEGIN_PREDICTION_DATA_NO_BASE( C_PortalPlayerLocalData )

END_PREDICTION_DATA()

const Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 );

C_PortalPlayerLocalData::C_PortalPlayerLocalData()
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