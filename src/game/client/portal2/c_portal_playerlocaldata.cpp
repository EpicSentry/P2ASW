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

Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 ); // Used only in the constructor below

C_PortalPlayerLocalData::C_PortalPlayerLocalData()
{
	m_StickNormal = DEFAULT_SURFACE_NORMAL;
	m_Up = DEFAULT_SURFACE_NORMAL;
}