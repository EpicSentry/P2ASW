//========= Copyright © 1996-2014, Valve Corporation, All rights reserved.=============//
//
//
//
//=====================================================================================//

// ENTITY STATUS = UNFINISHED.

#include "cbase.h"
#include "prop_portal.h"
#include "info_placement_helper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// List of all info camera links
//-----------------------------------------------------------------------------
//CUtlFixedLinkedList<CInfoPlacementHelper *> g_InfoCameraLinkList;

//sv_show_placement_help_in_preview
//Forces the placement preview to show any help in placement given
// m_flDisableTime
// m_bDeferringToPortal

//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
BEGIN_DATADESC(CInfoPlacementHelper)

	DEFINE_KEYFIELD(m_flRadius, FIELD_FLOAT, "radius"), //	Radius in which to influence placement.

	DEFINE_KEYFIELD(m_strTargetProxy, FIELD_STRING, "proxy_name"), // Name of the entity we want to use for our real placement position.
	DEFINE_KEYFIELD(m_strTargetEntity, FIELD_STRING, "attach_target_name"), // Name of the entity we want force our attachment to. Used to combine objects.
	
	DEFINE_KEYFIELD(m_bSnapToHelperAngles, FIELD_BOOLEAN, "snap_to_helper_angles"), // Placed objects will snap to the angles of the placement helper.
	DEFINE_KEYFIELD(m_bForcePlacement, FIELD_BOOLEAN, "force_placement"), // Avoids placement rules when placing at this helper.

	//DEFINE_FIELD(m_hCamera, FIELD_EHANDLE),
	DEFINE_FIELD(m_hTargetEntity, FIELD_EHANDLE),

	// Outputs
	DEFINE_OUTPUT(m_OnObjectPlaced, "OnObjectPlaced"),
	DEFINE_OUTPUT(m_ObjectPlacedSize, "OnObjectPlacedSize"),


END_DATADESC()


LINK_ENTITY_TO_CLASS(info_placement_helper, CInfoPlacementHelper);

CInfoPlacementHelper::CInfoPlacementHelper()
{

}


void CInfoPlacementHelper::Spawn(void)
{
	///////////////////
	// I implemented it in CProp_Portal::NewLocation
	// Most likely should be in CWeaponPortalgun instead

	/////////////
	// and NOW I've implemented it in TraceFirePortal in the Weapon_portalgun function
	// near the end
	// But it can't go there, as m_OnObjectPlaced wont work. It'll keep firing them
	// shoul dprorably implement in FirePortal after float flplacementsuccess

	// CProp_Portal::PlacePortal. Look in there. bruh.
	//UTIL_ScreenTilt( GetAbsOrigin(), TiltAngle(), Duration(), Radius(), TiltTime(), command, (GetSpawnFlags() & SF_TILT_EASE_INOUT) != 0 );
	//void UTIL_ScreenTilt(const Vector &center, const QAngle &tiltAngle, float duration, float radius, float tiltTime, ShakeCommand_t eCommand, bool bEaseInOut)

	//if (radius != 0.0f && pPlayer->WorldSpaceCenter().DistTo(center) > radius)
	//	continue;

	// Iterate through entities using this
	//CBaseEntity* pResult = gEntList.FindEntityByClassname(NULL,"npc_*");
	//while (pResult)
	//{
	//	CAI_BaseNPC* pNPC = dynamic_cast<CAI_BaseNPC*>(pResult);
	//	if (pNPC)
	//		pNPC->SetState(NPC_STATE_IDLE);

	//	pResult = gEntList.FindEntityByClassname(pResult, "npc_*");
	//}

}

void CInfoPlacementHelper::Activate(void)
{
	//Called after all spawn functions are done
}