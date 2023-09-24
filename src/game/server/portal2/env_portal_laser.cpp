#include "cbase.h"
#include "env_portal_laser.h"
#include "prop_portal_shared.h"
#include "portal_util_shared.h"
#include "player.h" 
#include "particle_parse.h"
#include "ieffects.h"
#include "util_shared.h"


// constants
const int CPortalLaser::FLOOR_TURRET_PORTAL_EYE_ATTACHMENT = 1;
const float CPortalLaser::FLOOR_TURRET_PORTAL_LASER_RANGE = 8192;
const char* CPortalLaser::LASER_ATTACHMENT_NAME = "laser_attachment";
const float CPortalLaser::FLOOR_TURRET_PORTAL_END_POINT_PULSE_SCALE = 4.0f;
const int CPortalLaser::FLOOR_TURRET_PORTAL_LASER_ATTACHMENT = 1;

BEGIN_DATADESC(CPortalLaser)
DEFINE_KEYFIELD(m_modelName, FIELD_STRING, "model"),
DEFINE_KEYFIELD(m_bStartOff, FIELD_BOOLEAN, "StartState"),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOn", InputTurnOn),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOff", InputTurnOff),
DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
//DEFINE_FIELD(m_bIsHittingPortal, FIELD_BOOLEAN),
END_DATADESC()
/*
IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_EnvPortalLaser)

SendPropBool(SENDINFO(m_bIsHittingPortal)),
SendPropVector(SENDINFO(v_vHitPos)),
SendPropVector(SENDINFO(vecNetOrigin)),
SendPropVector(SENDINFO(vecNetMuzzleDir)),

END_SEND_TABLE()
*/

void CPortalLaser::Think()
{
	// Schedule the next think
	SetNextThink(gpGlobals->curtime + 0.1f);

	if (m_bLaserOn)
	{
		UpdateLaser(); // Update the laser position if it's on
	}
	else
	{
		if (m_hCubeBeam)
		{
			m_hCubeBeam->AddEffects(EF_NODRAW); // Hide the cube's beam when the laser is off
		}
	}

}

void RotateVector(Vector& vec, const QAngle& angles) {
	matrix3x4_t matRotate;
	AngleMatrix(angles, matRotate);
	VectorRotate(vec, matRotate, vec);
}

float m_flLastDamageTime = 0.0f;
void CPortalLaser::UpdateLaser()
{
	Vector vecOrigin = GetAbsOrigin(); 
	QAngle angMuzzleDir;
	GetAttachment(FLOOR_TURRET_PORTAL_LASER_ATTACHMENT, vecOrigin, angMuzzleDir);

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment(FLOOR_TURRET_PORTAL_EYE_ATTACHMENT, vecEye, angEyeDir);

	Vector vecMuzzleDir;
	AngleVectors(angEyeDir, &vecMuzzleDir);

	trace_t cubeTrace;

	CTraceFilterSimpleClassnameList masterTraceFilter(this, COLLISION_GROUP_NONE);
	masterTraceFilter.AddClassnameToIgnore("info_placement_helper");
	masterTraceFilter.AddClassnameToIgnore("player");
	masterTraceFilter.AddClassnameToIgnore("Player");
	masterTraceFilter.AddClassnameToIgnore("prop_energy_ball");

	CTraceFilterSimpleClassnameList playerTraceFilter(this, COLLISION_GROUP_NONE);
	playerTraceFilter.AddClassnameToIgnore("info_placement_helper");
	playerTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	playerTraceFilter.AddClassnameToIgnore("prop_portal");

	UTIL_TraceLine(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &masterTraceFilter, &cubeTrace);

	// Check if were is the cube
	m_bIsLaserHittingCube = false;
	if (cubeTrace.m_pEnt && FClassnameIs(cubeTrace.m_pEnt, "prop_weighted_cube") && FStrEq(STRING(cubeTrace.m_pEnt->GetModelName()), "models/props/reflection_cube.mdl"))
	{
		m_bIsLaserHittingCube = true;
	}

	// Trace for the laser catcher
	trace_t catcherTrace;
	UTIL_TraceLine(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &masterTraceFilter, &catcherTrace);
	
	// Check if were hitting the catcher
	m_bIsLaserHittingCatcher = false;
	if (catcherTrace.m_pEnt && FClassnameIs(catcherTrace.m_pEnt, "prop_laser_catcher"))
	{
		m_bIsLaserHittingCatcher = true;
	}
	
	// Deal damage to the player if they are touching the laser beam
	if (gpGlobals->curtime - m_flLastDamageTime >= 2.0f) // Damage every 2 seconds
	{
		// Trace from the laser emitter to check if it hits the player
		trace_t playerTrace;
		UTIL_TraceLine(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &playerTraceFilter, &playerTrace);

		if (playerTrace.m_pEnt && FClassnameIs(playerTrace.m_pEnt, "player"))
		{
			// Player is touching the laser beam, deal damage and push them
			Vector vecPushDir = (playerTrace.m_pEnt->GetAbsOrigin() - vecOrigin);
			VectorNormalize(vecPushDir);

			Vector vecLaserDir = vecMuzzleDir;
			VectorNormalize(vecLaserDir);
			Vector vecPushSideways = CrossProduct(vecPushDir, vecLaserDir);
			VectorNormalize(vecPushSideways);

			playerTrace.m_pEnt->TakeDamage(CTakeDamageInfo(this, this, 10, DMG_ENERGYBEAM));
			playerTrace.m_pEnt->ApplyAbsVelocityImpulse(vecPushSideways * 500.0f); // Adjust the push force as needed

			m_flLastDamageTime = gpGlobals->curtime;
		}
	}

	if (!m_pBeam)
	{
		m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(10.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, vecOrigin);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);
		m_pBeam->SetEndPos(vecOrigin);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);

	trace_t trace; // Used for determining the portal hit

	// Perform the portal detection trace from the origin to the muzzle direction
	UTIL_TraceLine(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &masterTraceFilter, &trace);

	// Check if the trace hits any portals
	CProp_Portal* pLocalPortal = nullptr;
	CProp_Portal* pRemotePortal = nullptr;
	bool bHitPortal = UTIL_DidTraceTouchPortals(rayPath, trace, &pLocalPortal, &pRemotePortal);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &masterTraceFilter, &fEndFraction))
	{
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE;
		//Msg("Portal Beam End Point: (%f, %f, %f)\n", vEndPoint.x, vEndPoint.y, vEndPoint.z);
	}
	else
	{
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE * fEndFraction;
		//Msg("Main Trace End Point: (%f, %f, %f)\n", vEndPoint.x, vEndPoint.y, vEndPoint.z);
	}

	// Handle portal hit, if any
	if (bHitPortal)
	{
		if (pLocalPortal != nullptr)
		{
			// Local portal, no need to do anything here
		}

		if (pRemotePortal != nullptr)
		{
			//Msg("Remote Portal classname: %s\n", pRemotePortal->GetClassname());
			DoTraceFromPortal(pRemotePortal); // Do the rest from another void
		}
	}
	else
	{
		// No portal hit
		//Msg("No portal hit\n"); No need to do anything else from here.
	}

	//v_vHitPos = vEndPoint;
	m_pBeam->PointsInit(vEndPoint, vecOrigin);

	g_pEffects->Sparks(vEndPoint, 2, 2, &vecMuzzleDir);
}

void CPortalLaser::DoTraceFromPortal(CProp_Portal* pRemotePortal)
{
	if (!pRemotePortal)
		return;

	// Get the position and angles of the remote portal
	Vector vecRemoteOrigin = pRemotePortal->GetAbsOrigin();
	QAngle angRemoteMuzzleDir = pRemotePortal->GetAbsAngles();

	// Calculate the forward vector from the angles of the remote portal
	Vector vecRemoteMuzzleDir;
	AngleVectors(angRemoteMuzzleDir, &vecRemoteMuzzleDir);

	// Perform the trace from the remote portal's origin
	trace_t remoteTrace;
	CTraceFilterSimpleClassnameList remoteTraceFilter(this, COLLISION_GROUP_NONE);
	remoteTraceFilter.AddClassnameToIgnore("info_placement_helper");
	remoteTraceFilter.AddClassnameToIgnore("player");
	remoteTraceFilter.AddClassnameToIgnore("Player");
//	remoteTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	remoteTraceFilter.AddClassnameToIgnore("prop_portal");
	UTIL_TraceLine(vecRemoteOrigin, vecRemoteOrigin + vecRemoteMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &remoteTraceFilter, &remoteTrace);
	NDebugOverlay::Line(vecRemoteOrigin, remoteTrace.endpos, 132, 0, 255, true, 10.0f);

	// Check if the trace hits a laser catcher
	if (remoteTrace.m_pEnt)
	{
		// Print the class name and position of the hit entity
		DevMsg("Hit entity class name: %s at: %.2f %.2f %.2f\n", remoteTrace.m_pEnt->GetClassname(),
			remoteTrace.m_pEnt->GetAbsOrigin().x, remoteTrace.m_pEnt->GetAbsOrigin().y, remoteTrace.m_pEnt->GetAbsOrigin().z);

		if (FClassnameIs(remoteTrace.m_pEnt, "prop_laser_catcher"))
		{
			//Msg("Hit catcher!\n");
			m_bIsLaserHittingPortalCatcher = true;
		}
		else
		{
			//Msg("No Catcher hit\n");
			m_bIsLaserHittingPortalCatcher = false;
		}
	}
	else
	{
		// No entity was hit
		//DevMsg("No entity hit by trace.\n");
	}

	/*
	// Define a new trace and trace filter for the player
	CTraceFilterSimpleClassnameList playerTraceFilter(this, COLLISION_GROUP_NONE);
	playerTraceFilter.AddClassnameToIgnore("info_placement_helper");
	playerTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	playerTraceFilter.AddClassnameToIgnore("prop_portal");
	trace_t playerTrace;
	UTIL_TraceLine(vecRemoteOrigin, vecRemoteOrigin + vecRemoteMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &playerTraceFilter, &playerTrace);

	// Deal damage to the player if they are touching the laser beam
	if (gpGlobals->curtime - m_flLastDamageTime >= 2.0f && playerTrace.m_pEnt && FClassnameIs(playerTrace.m_pEnt, "player"))
	{
		// Player is touching the laser beam, deal damage and push them
		Vector vecPushDir = (playerTrace.m_pEnt->GetAbsOrigin() - vecRemoteOrigin);
		VectorNormalize(vecPushDir);

		Vector vecLaserDir = vecRemoteMuzzleDir;
		VectorNormalize(vecLaserDir);
		Vector vecPushSideways = CrossProduct(vecPushDir, vecLaserDir);
		VectorNormalize(vecPushSideways);

		playerTrace.m_pEnt->TakeDamage(CTakeDamageInfo(this, this, 10, DMG_ENERGYBEAM));
		playerTrace.m_pEnt->ApplyAbsVelocityImpulse(vecPushSideways * 500.0f);

		m_flLastDamageTime = gpGlobals->curtime;
	}
	*/
}


void CPortalLaser::Spawn(void)
{
	//Msg("Laser Init\n");
	Precache();
	SetSolid(SOLID_BBOX);
	SetMoveType(MOVETYPE_NONE);
	SetModel("models/props/laser_emitter_center.mdl"); // Default model if it is not defined as a keyvalue

	// Check if the entity has a keyvalue for "model" and use that value to set the model path
	if (m_modelName != NULL_STRING)
	{
		const char* pszModelName = STRING(m_modelName);
		if (pszModelName && pszModelName[0] != '\0')
		{
			SetModel(pszModelName);
		}
	}

	if (m_bStartOff)
	{
		Msg("Laser state set to OFF\n");
		LaserOff();
	}
	else
	{
		Msg("Laser state set to ON\n");
		LaserOn();
	}


	SetThink(&CPortalLaser::Think);
	SetNextThink(gpGlobals->curtime + 0.1f);

	BaseClass::Spawn();
}

void CPortalLaser::Precache(void)
{
	Msg("Laser Precache\n");
	// Precache the model using the stored model path
	PrecacheModel(STRING(m_modelName));
	PrecacheModel("models/props/laser_emitter_center.mdl");
}

void CPortalLaser::LaserOff(void)
{
	m_bLaserOn = false;
	Msg("Laser Deactivating\n");
	if (m_pBeam)
	{
		m_pBeam->AddEffects(EF_NODRAW);
	}
}
//start laseron
void CPortalLaser::LaserOn(void)
{
	m_bLaserOn = true;
	Msg("Laser Activating\n");

	Vector vecOrigin = GetAbsOrigin();
	QAngle angMuzzleDir;
	CBaseAnimating* pBaseAnimating = dynamic_cast<CBaseAnimating*>(this);
	bool bFoundAttachment = false;
	if (pBaseAnimating)
	{
		int lensBone = pBaseAnimating->LookupBone("lens");
		if (lensBone != -1)
		{
			pBaseAnimating->GetBonePosition(lensBone, vecOrigin, angMuzzleDir);
			bFoundAttachment = true;
		}
	}

	if (!bFoundAttachment)
	{
		int laserAttachmentIndex = LookupAttachment("laser_attachment");
		if (laserAttachmentIndex > 0)
		{
			GetAttachmentLocal(laserAttachmentIndex, vecOrigin, angMuzzleDir);
			bFoundAttachment = true;
		}
	}

	if (!bFoundAttachment)
	{
		Msg("Laser Attachment NOT Found: laser_attachment\n");
		LaserOff();
		return;
	}

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment(FLOOR_TURRET_PORTAL_EYE_ATTACHMENT, vecEye, angEyeDir);

	Vector vecMuzzleDir;
	AngleVectors(angEyeDir, &vecMuzzleDir);

	if (!m_pBeam)
	{
		m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		//m_pBeam->SetColor(255, 32, 32);
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(10.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, vecOrigin);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);
		m_pBeam->SetEndPos(vecOrigin);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	// Trace to find an endpoint
	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &traceFilter, &fEndFraction))
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE;
	else
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE * fEndFraction;

	m_pBeam->PointsInit(vEndPoint, vecOrigin);
}

//end laseron
//----------------------------------------------------------------------------
//Hammer entity options
//this section controls the numerous values this entity can have in hammer
//----------------------------------------------------------------------------
void CPortalLaser::InputTurnOn(inputdata_t& inputData)
{
	if (!m_bLaserOn)
		LaserOn();
}

void CPortalLaser::InputTurnOff(inputdata_t& inputData)
{
	if (m_bLaserOn)
		LaserOff();
}

void CPortalLaser::InputToggle(inputdata_t& inputData)
{
	if (m_bLaserOn)
		LaserOff();
	else
		LaserOn();
}

// Misc
float CPortalLaser::LaserEndPointSize(void)
{
	return ((MAX(0.0f, sinf(gpGlobals->curtime * M_PI + m_fPulseOffset))) * FLOOR_TURRET_PORTAL_END_POINT_PULSE_SCALE + 3.0f) * 1.5f;
}

bool CPortalLaser::IsLaserHittingCube()
{
	return m_bIsLaserHittingCube;
}

bool CPortalLaser::IsLaserHittingCatcher()
{
	return m_bIsLaserHittingCatcher;
}

bool CPortalLaser::IsLaserHittingPortalCatcher()
{
	return m_bIsLaserHittingPortalCatcher;
}

LINK_ENTITY_TO_CLASS(env_portal_laser, CPortalLaser);