#include "cbase.h"
#include "prop_portal_shared.h" // For UTIL_Portal_TraceRay_Beam
#include "beam_shared.h"
#include "prop_weighted_cube.h"
#include "ai_basenpc.h"
#include "baseanimating.h"
#include "sprite.h"
#include "particle_parse.h"
#include "props.h"
#include "particle_system.h"
#include "player_pickup.h"
#include "env_portal_laser.h"
#include "ieffects.h"

#define	CUBE_MODEL			"models/props/metal_box.mdl"
#define	REFLECTION_MODEL	"models/props/reflection_cube.mdl"
#define	SPHERE_MODEL		"models/props_gameplay/mp_ball.mdl"
#define	ANTIQUE_MODEL		"models/props_underground/underground_weighted_cube.mdl"

#define FIZZLE_SOUND		"Prop.Fizzled"


enum CubeType
{
	Standard = 0,
	Companion = 1,
	Reflective,
	Sphere,
	Antique,
};

enum SkinOld
{
	OLDStandard = 0,
	OLDCompanion = 1,
	OLDStandardActivated,
	OLDReflective,
	OLDSphere,
	OLDAntique
};

enum SkinType
{
	Clean = 0,
	Rusted = 1,
};

enum PaintPower
{
	Bounce = 0,
	Stick = 1,
	Speed,
	Portal,
	None,
};


LINK_ENTITY_TO_CLASS(prop_weighted_cube, CPropWeightedCube);

BEGIN_DATADESC(CPropWeightedCube)

// Save/load
DEFINE_USEFUNC(Use),

//DEFINE_KEYFIELD(m_oldSkin, FIELD_INTEGER, "skin"),
DEFINE_KEYFIELD(m_cubeType, FIELD_INTEGER, "CubeType"),
DEFINE_KEYFIELD(m_skinType, FIELD_INTEGER, "SkinType"),
DEFINE_KEYFIELD(m_paintPower, FIELD_INTEGER, "PaintPower"),
DEFINE_KEYFIELD(m_useNewSkins, FIELD_BOOLEAN, "NewSkins"),
DEFINE_KEYFIELD(m_allowFunnel, FIELD_BOOLEAN, "allowfunnel"),

DEFINE_INPUTFUNC(FIELD_VOID, "Dissolve", InputDissolve),
DEFINE_INPUTFUNC(FIELD_VOID, "SilentDissolve", InputSilentDissolve),
DEFINE_INPUTFUNC(FIELD_VOID, "PreDissolveJoke", InputPreDissolveJoke),
DEFINE_INPUTFUNC(FIELD_VOID, "ExitDisabledState", InputExitDisabledState),

// Output
DEFINE_OUTPUT(m_OnPainted, "OnPainted"),
DEFINE_OUTPUT(m_OnFizzled, "OnFizzled"),
DEFINE_OUTPUT(m_OnOrangePickup, "OnOrangePickup"),
DEFINE_OUTPUT(m_OnBluePickup, "OnBluePickup"),
DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
DEFINE_OUTPUT(m_OnPhysGunDrop, "OnPhysGunDrop"),

END_DATADESC()

void CPropWeightedCube::Precache(void)
{
	PrecacheModel(CUBE_MODEL);
	PrecacheModel(REFLECTION_MODEL);
	PrecacheModel(SPHERE_MODEL);
	PrecacheModel(ANTIQUE_MODEL);
	PrecacheScriptSound(FIZZLE_SOUND);
	BaseClass::Precache();
}

void CPropWeightedCube::InputPreDissolveJoke(inputdata_t &data)
{
	// Sets some VO to do before fizzling.
}

void CPropWeightedCube::InputExitDisabledState(inputdata_t &data)
{
	// Exits the disabled state of a reflector cube.
}


void CPropWeightedCube::InputDissolve(inputdata_t &data)
{
	Dissolve(NULL, gpGlobals->curtime, false, 0, GetAbsOrigin(), 1);
	EmitSound(FIZZLE_SOUND);
}

void CPropWeightedCube::InputSilentDissolve(inputdata_t &data)
{
	Dissolve(NULL, gpGlobals->curtime, false, 0, GetAbsOrigin(), 1);
}


void CPropWeightedCube::Spawn(void)
{
	Precache();
	BaseClass::Spawn();
	m_bIsCubeLaserHittingCatcher = false;
	if (m_useNewSkins)
	{
		m_nSkin = 0;
		switch (m_cubeType) 
		{
		case Standard:
			SetModel(CUBE_MODEL);
			if (m_skinType == Rusted)
				m_nSkin = 3;
			if (m_paintPower == Stick)
				m_nSkin = 8;
			if (m_paintPower == Speed)
				m_nSkin = 9;
			break;
		case Companion:
			SetModel(CUBE_MODEL);
			m_nSkin = 1;
			if (m_paintPower == Stick)
				m_nSkin = 10;
			if (m_paintPower == Speed)
				m_nSkin = 11;
			break;
		case Reflective:
			SetModel(REFLECTION_MODEL);
			if (m_skinType == Rusted)
				m_nSkin = 1;
			if (m_paintPower == Stick)
				m_nSkin = 2;
			if (m_paintPower == Speed)
				m_nSkin = 3;
			break;
		case Sphere:
			SetModel(SPHERE_MODEL);
			break;
		case Antique:
			SetModel(ANTIQUE_MODEL);
			if (m_paintPower == Stick)
				m_nSkin = 1;
			if (m_paintPower == Speed)
				m_nSkin = 2;
			break;
		}
	}
	else
	{
		switch (m_nSkin)
		{
		case OLDStandard:
			SetModel(CUBE_MODEL);
			if (m_skinType == Rusted)
				m_nSkin = 3;
			if (m_paintPower == Stick)
				m_nSkin = 8;
			if (m_paintPower == Speed)
				m_nSkin = 9;
			break;
		case OLDCompanion:
			SetModel(CUBE_MODEL);
			m_nSkin = 1;
			if (m_paintPower == Stick)
				m_nSkin = 10;
			if (m_paintPower == Speed)
				m_nSkin = 11;
			break;
		case OLDReflective:
			SetModel(REFLECTION_MODEL);
			if (m_skinType == Rusted)
				m_nSkin = 1;
			if (m_paintPower == Stick)
				m_nSkin = 2;
			if (m_paintPower == Speed)
				m_nSkin = 3;
			break;
		case OLDSphere:
			SetModel(SPHERE_MODEL);
			break;
		case OLDAntique:
			SetModel(ANTIQUE_MODEL);
			if (m_paintPower == Stick)
				m_nSkin = 1;
			if (m_paintPower == Speed)
				m_nSkin = 2;
			break;
		}
	}
	SetSolid(SOLID_VPHYSICS);
	SetCollisionGroup(COLLISION_GROUP_WEIGHTED_CUBE);

	// In order to pick it up, needs to be physics.
	CreateVPhysics();

	//Begin checking for the laser
	SetThink(&CPropWeightedCube::Think);
	SetNextThink(gpGlobals->curtime + 0.1f);

	SetUse(&CPropWeightedCube::Use);
}

int CPropWeightedCube::ObjectCaps()
{
	int caps = BaseClass::ObjectCaps();

	caps |= FCAP_IMPULSE_USE;

	return caps;
}

bool CPropWeightedCube::Dissolve(const char* materialName, float flStartTime, bool bNPCOnly, int nDissolveType, Vector vDissolverOrigin, int magnitude)
{
	m_OnFizzled.FireOutput(this, this);
	return BaseClass::Dissolve(materialName, flStartTime, bNPCOnly, nDissolveType, vDissolverOrigin, magnitude);
}

void CPropWeightedCube::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer(pActivator);
	if (pPlayer)
	{
		pPlayer->PickupObject(this);
	}
}

void CPropWeightedCube::OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason)
{
	m_hPhysicsAttacker = pPhysGunUser;

	if (reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER)
	{
		m_OnPlayerPickup.FireOutput(pPhysGunUser, this);
	}
}

void CPropWeightedCube::OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason)
{
	m_OnPhysGunDrop.FireOutput(pPhysGunUser, this);
}

void CPropWeightedCube::Think()
{
	// Call the function to check if the main beam is hitting the cube.
	CheckLaserHitCube();

	// Set the next think time, you can adjust the interval as needed (0.1 seconds in this example).
	SetNextThink(gpGlobals->curtime + 0.1f);
}


void CPropWeightedCube::CheckLaserHitCube()
{
	// Find the first env_portal_laser entity in the map
	CEnvPortalLaser* pLaser = nullptr;
	pLaser = dynamic_cast<CEnvPortalLaser*>(gEntList.FindEntityByClassname(pLaser, "env_portal_laser")); // Separate the assignment from the condition

	while (pLaser)
	{
		if (pLaser->IsLaserHittingCube())
		{
			// If the main beam of any laser is hitting the cube, print a message to the console.
			//Msg("The main beam of a laser is hitting the cube!\n");
			UpdateLaser();
			return; // Return if any laser is hitting the cube to avoid checking the rest.
		}

		// Get the next laser entity in the map
		pLaser = dynamic_cast<CEnvPortalLaser*>(gEntList.FindEntityByClassname(pLaser, "env_portal_laser"));
	}

	// If no laser is hitting the cube, print a different message.
	//Msg("No laser is hitting the cube.\n");
	TurnLaserOff(); // Turn it off as nothing is hitting it
}

const float FLOOR_TURRET_PORTAL_LASER_RANGE = 8192;
void CPropWeightedCube::UpdateLaser()
{
	// Get the cube's position and angles
	Vector vecOrigin = GetAbsOrigin();
	QAngle angAngles = GetAbsAngles();

	// Calculate the laser direction based on the cube's angles
	Vector vecMuzzleDir;
	AngleVectors(angAngles, &vecMuzzleDir);

	trace_t cubeTrace;
	UTIL_TraceLine(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, this, COLLISION_GROUP_NONE, &cubeTrace);

	if (!m_hCubeBeam)
	{
		// Create the laser beam entity if it doesn't exist
		m_hCubeBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		m_hCubeBeam->SetBrightness(92);
		m_hCubeBeam->SetNoise(0);
		m_hCubeBeam->SetWidth(10.0f);
		m_hCubeBeam->SetEndWidth(0);
		m_hCubeBeam->SetScrollRate(0);
		m_hCubeBeam->SetFadeLength(0);
		m_hCubeBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_hCubeBeam->PointsInit(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, vecOrigin);
		m_hCubeBeam->SetBeamFlag(FBEAM_REVERSED);
		m_hCubeBeam->SetStartEntity(this);
	}
	else
	{
		m_hCubeBeam->SetStartPos(vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);
		m_hCubeBeam->SetEndPos(vecOrigin);
		m_hCubeBeam->RemoveEffects(EF_NODRAW);
	}

	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecOrigin, vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE);

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &traceFilter, &fEndFraction))
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE;
	else
		vEndPoint = vecOrigin + vecMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE * fEndFraction;

	m_hCubeBeam->PointsInit(vEndPoint, vecOrigin);

	g_pEffects->Sparks(vEndPoint, 2, 2, &vecMuzzleDir);
}



void CPropWeightedCube::TurnLaserOff()
{
	if (m_hCubeBeam && !m_hCubeBeam->IsMarkedForDeletion())
	{
		m_hCubeBeam->Remove();
		m_hCubeBeam = nullptr; // Reset the handle to null after removing the beam
	}
}

bool CPropWeightedCube::IsCubeLaserHittingCatcher()
{
	return m_bIsCubeLaserHittingCatcher;
}


void CC_Create_PortalReflectorBox(void)
{
	// Try to create entity
	CPropWeightedCube *pCube = dynamic_cast<CPropWeightedCube*>(CreateEntityByName("prop_weighted_cube"));
	if (pCube)
	{
		// Set the CubeType keyfield to Standard
		pCube->KeyValue("CubeType", "Reflective");

		// Set the model of the cube
		pCube->Precache();
		pCube->SetModel(REFLECTION_MODEL);
		pCube->SetName(MAKE_STRING("reflection_box"));

		// Enable physics for the cube
		pCube->SetSolid(SOLID_VPHYSICS); // Enable vphysics
		pCube->AddSolidFlags(FSOLID_NOT_SOLID); // Make sure it starts as nonsolid
		pCube->VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		pCube->SetMoveType(MOVETYPE_VPHYSICS);

		// Now attempt to drop into the world
		CBasePlayer* pPlayer = UTIL_GetCommandClient();
		trace_t tr;
		Vector forward;
		pPlayer->EyeVectors(&forward);
		UTIL_TraceLine(pPlayer->EyePosition(),
			pPlayer->EyePosition() + forward * MAX_TRACE_LENGTH, MASK_SOLID,
			pPlayer, COLLISION_GROUP_DEBRIS, &tr); // Set the collision group here
		if (tr.fraction != 1.0)
		{
			tr.endpos.z += 12;
			pCube->Teleport(&tr.endpos, NULL, NULL);
			UTIL_DropToFloor(pCube, MASK_SOLID);
		}
	}
}
static ConCommand ent_create_portal_reflector_cube("ent_create_portal_reflector_cube", CC_Create_PortalReflectorBox, "Creates a reflection cube at the location the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);

void CC_Create_PortalWeightBox(void)
{
	// Try to create entity
	CPropWeightedCube *pCube = dynamic_cast<CPropWeightedCube*>(CreateEntityByName("prop_weighted_cube"));
	if (pCube)
	{
		// Set the CubeType keyfield to Standard
		pCube->KeyValue("CubeType", "Standard");

		// Set the model of the cube
		pCube->Precache();
		pCube->SetModel(CUBE_MODEL);
		pCube->SetName(MAKE_STRING("box"));

		// Enable physics for the cube
		pCube->SetSolid(SOLID_VPHYSICS); // Enable vphysics
		pCube->AddSolidFlags(FSOLID_NOT_SOLID); // Make sure it starts as nonsolid
		pCube->VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		pCube->SetMoveType(MOVETYPE_VPHYSICS);
		//pCube->SetCollisionGroup(COLLISION_GROUP_DEBRIS); // Use COLLISION_GROUP_DEBRIS here

		// Now attempt to drop into the world
		CBasePlayer* pPlayer = UTIL_GetCommandClient();
		trace_t tr;
		Vector forward;
		pPlayer->EyeVectors(&forward);
		UTIL_TraceLine(pPlayer->EyePosition(),
			pPlayer->EyePosition() + forward * MAX_TRACE_LENGTH, MASK_SOLID,
			pPlayer, COLLISION_GROUP_DEBRIS, &tr); // Set the collision group here
		if (tr.fraction != 1.0)
		{
			tr.endpos.z += 12;
			pCube->Teleport(&tr.endpos, NULL, NULL);
			UTIL_DropToFloor(pCube, MASK_SOLID);
		}
	}
}
static ConCommand ent_create_portal_weighted_cube("ent_create_portal_weighted_cube", CC_Create_PortalWeightBox, "Creates a standard cube at the location the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);

