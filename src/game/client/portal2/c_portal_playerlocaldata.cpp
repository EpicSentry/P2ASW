#include "cbase.h"
#include "c_portal_playerlocaldata.h"


BEGIN_RECV_TABLE_NOBASE( CPortalPlayerLocalData, DT_PortalLocal ) // Purpose: Not done (Line 16)

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

END_RECV_TABLE()


BEGIN_PREDICTION_DATA_NO_BASE( C_PortalPlayerLocalData )

END_PREDICTION_DATA()

Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 ); // Used only in the constructor below

C_PortalPlayerLocalData::C_PortalPlayerLocalData()
{
	m_StickNormal = DEFAULT_SURFACE_NORMAL;
	m_Up = DEFAULT_SURFACE_NORMAL;
}