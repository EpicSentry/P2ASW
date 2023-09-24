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

/*
    SendPropBool(&g_SendProps_191[1], "m_bShowingViewFinder", 4, 1);
    SendPropFloat(&g_SendProps_191[2], "m_flAirControlSupressionTime", 8, 4, 32, 0, 0.0, -121121.12, SendProxy_FloatToFloat, 0x80u);
    SendPropInt(&v1, "m_nLocatorEntityIndices", 12, 4, -1, 0, 0, 0x80u);

    SendPropArray3(&g_SendProps_191[3], "m_nLocatorEntityIndices", 12, 4, 16, v1, SendProxy_DataTableToDataTable, 128); // v1 ??

    SendPropBool(&g_SendProps_191[4], "m_bPlacingPhoto", 76, 1);

    SendPropVector(&g_SendProps_191[5], "m_StickNormal", 1048736, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[6], "m_OldStickNormal", 1048748, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[7], "m_vPreUpdateVelocity", 1048760, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[8], "m_Up", 1048772, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[9], "m_vStickRotationAxis", 1048784, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[10], "m_StandHullMin", 1048796, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[11], "m_StandHullMax", 1048808, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[12], "m_DuckHullMin", 1048820, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[13], "m_DuckHullMax", 1048832, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[14], "m_CachedStandHullMinAttempt", 1048844, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[15], "m_CachedStandHullMaxAttempt", 1048856, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[16], "m_CachedDuckHullMinAttempt", 1048868, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[17], "m_CachedDuckHullMaxAttempt", 1048880, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[18], "m_vLocalUp", 1048892, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);
    SendPropVector(&g_SendProps_191[19], "m_vEyeOffset", 1048904, 12, 0, 4, 0.0, -121121.12, SendProxy_UnmodifiedQAngles, 0x80u);

    SendPropQAngles(&g_SendProps_191[20], "m_qQuaternionPunch", 1048916, 12, 0, 4, SendProxy_QAngles, 0x80u);

    SendPropInt(&g_SendProps_191[21], "m_PaintedPowerType", 352, 4, -1, 0, 0, 0x80u);

    SendPropDataTable(&g_SendProps_191[22], "m_PaintedPowerTimer", 356, &DT_CountdownTimer::g_SendTable, SendProxy_DataTableToDataTable, 0x80u);

    SendPropFloat(&g_SendProps_191[23], "m_flCurrentStickTime", 372, 4, 0, 4, 0.0, -121121.12, SendProxy_FloatToFloat, 0x80u);
    SendPropFloat(&g_SendProps_191[24], "m_flAirInputScale", 368, 4, 0, 4, 0.0, -121121.12, SendProxy_FloatToFloat, 0x80u);

    SendPropInt(&g_SendProps_191[25], "m_nStickCameraState", 376, 4, -1, 0, 0, 0x80u);
    SendPropInt(&g_SendProps_191[26], "m_InAirState", 380, 4, -1, 0, 0, 0x80u);

    SendPropBool(&g_SendProps_191[27], "m_bDoneStickInterp", 384, 1);
    SendPropBool(&g_SendProps_191[28], "m_bDoneCorrectPitch", 385, 1);
    SendPropBool(&g_SendProps_191[29], "m_bJumpedThisFrame", 387, 1);
    SendPropBool(&g_SendProps_191[30], "m_bBouncedThisFrame", 388, 1);
    SendPropBool(&g_SendProps_191[31], "m_bDuckedInAir", 389, 1);

    SendPropEHandle(&g_SendProps_191[32], "m_hTractorBeam", 392, 4, -1, SendProxy_EHandleToInt);

    SendPropBool(&g_SendProps_191[33], "m_bZoomedIn", 400, 1);

    SendPropFloat(&g_SendProps_191[34], "m_fBouncedTime", 404, 4, 32, 0, 0.0, -121121.12, SendProxy_FloatToFloat, 0x80u);

    SendPropBool(&g_SendProps_191[35], "m_bPreventedCrouchJumpThisFrame", 408, 1);
	*/

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

Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 ); // Used only in the constructor below
CPortalPlayerLocalData::CPortalPlayerLocalData()
{
	m_StickNormal = DEFAULT_SURFACE_NORMAL;
	m_Up = DEFAULT_SURFACE_NORMAL;

#ifdef HL2_EPISODIC
	m_flFlashBattery = 0.0f;
#endif
}
