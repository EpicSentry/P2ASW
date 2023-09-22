#include "cbase.h"
#include "c_baseanimating.h"
#include "beam_shared.h"
#include "prop_portal_shared.h"
#include "c_prop_portal.h"
#include "portal_util_shared.h"
#include "c_te_effect_dispatch.h"
#include "fx_sparks.h"
#include "ieffects.h"

class C_PortalLaser_Client : public C_BaseAnimating
{
public:
	DECLARE_CLASS(C_PortalLaser_Client, C_BaseAnimating);
	DECLARE_CLIENTCLASS();

	void Spawn();
	void ClientThink();
	void DoLaserOnPortal();
	void RemoveLaser();
	void TraceFromRemotePortal(C_BaseEntity* pRemotePortal);
	bool    m_bIsHittingPortal;
	Vector v_vHitPos;
	Vector vecNetOrigin;
	Vector vecNetMuzzleDir;

	// Add any member variables or functions here as needed
};
IMPLEMENT_CLIENTCLASS_DT(C_PortalLaser_Client, DT_EnvPortalLaser, CEnvPortalLaser)

RecvPropBool(RECVINFO(m_bIsHittingPortal)),
RecvPropVector(RECVINFO(v_vHitPos)),
RecvPropVector(RECVINFO(vecNetOrigin)),
RecvPropVector(RECVINFO(vecNetMuzzleDir)),

END_RECV_TABLE()

void C_PortalLaser_Client::Spawn(void)
{
	//Set think to check if the bool is true or not
	SetThink(&C_PortalLaser_Client::ClientThink);
	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

void C_PortalLaser_Client::ClientThink(void)
{
	// TODO: Need to get the cube laser version of the check done sooner or later

	// Check the bool
	SetNextThink(gpGlobals->curtime + 0.1f);

	// Print messages based on the value of m_bIsHittingPortal
	if (m_bIsHittingPortal)
	{
		//Msg("Client: I'm hitting a portal\n");
		DoLaserOnPortal();
	}
	else
	{
		//Msg("Client: I don't see anything\n");
	}
}

void C_PortalLaser_Client::DoLaserOnPortal(void)
{
	//Comments = Debugging statements for later

	// Use the stored hit position as the endpoint for the laser beam
	Vector vecPortalHit = v_vHitPos;
	//Msg("HitPos: (%f, %f, %f)\n", v_vHitPos.x, v_vHitPos.y, v_vHitPos.z);

	// Calculate the direction of the laser beam (normalized)
	//Msg("MuzzleDir: (%f, %f, %f)\n", vecNetMuzzleDir.x, vecNetMuzzleDir.y, vecNetMuzzleDir.z);

	// Trace from the adjusted hit position to find the portal entity
	trace_t trace;
	const float FLOOR_TURRET_PORTAL_LASER_RANGE = 8192;
	Vector vecEnd = vecNetOrigin + vecNetMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE;
	Ray_t ray;
	ray.Init(vecNetOrigin, vecEnd);

	UTIL_TraceRay(ray, MASK_SHOT, this, COLLISION_GROUP_NONE, &trace);

	// Check if the trace hits any portals
	CProp_Portal* pLocalPortal = nullptr;
	CProp_Portal* pRemotePortal = nullptr;
	bool bHitPortal = UTIL_DidTraceTouchPortals(ray, trace, &pLocalPortal, &pRemotePortal);

	// Print the hit entity and position regardless of whether it's a portal or nothing
	if (trace.m_pEnt)
	{
		//Msg("Hit Entity: %s\n", trace.m_pEnt->GetClassname());
		//Msg("Hit Position: (%f, %f, %f)\n", trace.endpos.x, trace.endpos.y, trace.endpos.z);

		if (bHitPortal)
		{
			if (pLocalPortal != nullptr)
			{
				// Trace entrypoint portal
				//Msg("Local Portal classname: %s\n", pLocalPortal->GetClassname());
				//g_pEffects->Sparks(pLocalPortal->GetAbsOrigin());
				// Do something with the local portal
			}

			if (pRemotePortal != nullptr)
			{
				// The linked portal
				Msg("Remote Portal classname: %s\n", pRemotePortal->GetClassname());
				// Do something with the remote portal
				TraceFromRemotePortal(pRemotePortal);
			}
		}
		else
		{
			// Handle if there's no portal hit, the server will do the rest
			Msg("BUG: No portal hit\n");
		}
	}
	else
	{
		// Handle if there's no entity hit, print a message and the server does the rest
		Msg("BUG: Nothing hit, trace failed\n");
	}

	// Update the laser beam's endpoint
	// Don't need to atm
}

void C_PortalLaser_Client::TraceFromRemotePortal(C_BaseEntity* pRemotePortal)
{
	// Perform the trace from the remote portal
	trace_t trace;
	Vector vecOrigin = pRemotePortal->GetAbsOrigin();
	Vector vecForward;
	AngleVectors(pRemotePortal->GetAbsAngles(), &vecForward); // Get the forward vector of the remote portal

	// Define the length of the trace (adjust as needed)
	const float TRACE_DISTANCE = 8124.0f;
	Vector vecEnd = vecOrigin + vecForward * TRACE_DISTANCE;

	// Perform the trace
	UTIL_TraceLine(vecOrigin, vecEnd, MASK_SHOT, pRemotePortal, COLLISION_GROUP_NONE, &trace);

	// Check if the trace hits any entity
	if (trace.m_pEnt)
	{
		// Check if the trace hits a prop_laser_catcher entity
		if (FClassnameIs(trace.m_pEnt, "prop_laser_catcher"))
		{
			// Do something specific for the prop_laser_catcher entity
			Msg("Trace from Remote Portal - Hit a laser catcher\n");
			// Add your code to handle the prop_laser_catcher entity here
		}
		else
		{
			// Handle other types of entities that are hit by the trace
			Msg("Trace from Remote Portal - Hit Entity: %s\n", trace.m_pEnt->GetClassname);
		}

		// Print the hit position
		//Msg("Trace from Remote Portal - Hit Position: (%f, %f, %f)\n", trace.endpos.x, trace.endpos.y, trace.endpos.z);
	}
	else
	{
		Msg("Trace from Remote Portal - Nothing hit\n");
	}
}

void C_PortalLaser_Client::RemoveLaser(void)
{
	// TODO: need to do this after dolaseronportal is finished later
}
