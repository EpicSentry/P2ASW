#include "cbase.h"
#include "env_portal_laser.h"
#include "particle_parse.h"

class CPropLaserRelay : public CBaseAnimating
{
	DECLARE_CLASS(CPropLaserRelay, CBaseAnimating);
public:
	DECLARE_DATADESC();

	// Constructor
	CPropLaserRelay();

	// Precache the resources used by the entity
	void Precache() override;

	// Initialize the entity
	void Spawn() override;

	void CheckLaserHitCatcher();

	void Think();

	void OnPowered();

	void OnUnpowered();

	// Add more member functions and variables as needed.

private:
	// Member variable to store the model path
	string_t m_iszModel;

	// Member variable to store the skin type
	int m_iSkinType;

	// Outputs
	COutputEvent m_OnPowered;
	COutputEvent m_OnUnpowered;
	bool m_bHasFiredPowered;
	bool m_bHasFiredUnpowered;

	bool m_bIsActive;

	CSoundPatch *m_pPowerOnSound;
	CSoundPatch *m_pPowerLoopSound;
	CSoundPatch *m_pPowerOffSound;
};

// Link the entity to the class name in Hammer
LINK_ENTITY_TO_CLASS(prop_laser_relay, CPropLaserRelay);

// Start of the data description for the entity
BEGIN_DATADESC(CPropLaserRelay)


// Define the "model" keyvalue property
DEFINE_KEYFIELD(m_iszModel, FIELD_STRING, "model"),

// Define the "SkinType" keyvalue property with default value 0 (clean on)
DEFINE_KEYFIELD(m_iSkinType, FIELD_INTEGER, "SkinType"),
DEFINE_OUTPUT(m_OnPowered, "OnPowered"),
DEFINE_OUTPUT(m_OnUnpowered, "OnUnpowered"),
END_DATADESC()

CPropLaserRelay::CPropLaserRelay()
{
	// Set the default model path if the "model" property is not set
	if (m_iszModel == NULL_STRING)
	{
		m_iszModel = MAKE_STRING("models/props/laser_relay.mdl");
	}

	// Set the default skin type if the "SkinType" property is not set
	if (m_iSkinType != 1)
	{
		m_iSkinType = 0; // Default to clean on
	}
}

void CPropLaserRelay::Precache()
{
	// Precache the model resource
	//PrecacheModel("models/props/laser_catcher_center.mdl"); // Add the custom model here
	PrecacheModel(STRING(m_iszModel));

	PrecacheScriptSound("world/laser_node_power_on.wav");
	PrecacheScriptSound("world/laser_node_power_loop.wav");
	PrecacheScriptSound("world/laser_node_power_off.wav");
	PrecacheScriptSound("world/laser_node_lp_01.wav");
	PrecacheParticleSystem("laser_relay_powered");
}

void CPropLaserRelay::Spawn()
{
	// Add entity spawning code here, such as model setting, physics properties, etc.
	Precache();
	m_bIsActive = false;

	// Set the model based on the "model" property
	SetModel(STRING(m_iszModel));

	// Enable physics for the entity
	SetSolid(SOLID_VPHYSICS);
	AddSolidFlags(FSOLID_NOT_SOLID); // Make the entity initially not solid to avoid immediate collisions

	// Set the skin based on the "SkinType" property using bodygroups (assuming 4 skins in the model)
	int iSkinIndex = (m_iSkinType == 1) ? 3 : 1;
	SetBodygroup(0, iSkinIndex);

	// Enable collisions and set the model's physics properties
	if (GetModelPtr())
	{
		// Make the entity solid now that the model is set
		RemoveSolidFlags(FSOLID_NOT_SOLID);

		// Set the model's physics properties
		SetCollisionGroup(COLLISION_GROUP_NONE);
		VPhysicsInitStatic();
	}

	//Begin checking for the laser
	SetThink(&CPropLaserRelay::Think);
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPropLaserRelay::Think()
{
	// Call the function to check if the main beam is hitting the cube.
	CheckLaserHitCatcher();

	// Check if the laser catcher is active
	if (m_bIsActive)
	{
		SetSequence(1);
	}

	// Set the next think time, you can adjust the interval as needed (0.1 seconds in this example).
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPropLaserRelay::CheckLaserHitCatcher()
{
	// Find the first env_portal_laser entity in the map
	CEnvPortalLaser* pLaser = nullptr;
	pLaser = dynamic_cast<CEnvPortalLaser*>(gEntList.FindEntityByClassname(pLaser, "env_portal_laser")); // Separate the assignment from the condition

	while (pLaser)
	{
		if (pLaser->IsLaserHittingCatcher() || pLaser->IsLaserHittingPortalCatcher())
		{
			// If the main beam or the through-portal beam of any laser is hitting the catcher, trigger the appropriate output.
			OnPowered();
			return; // Return if any laser is hitting the catcher to avoid checking the rest.
		}

		// Get the next laser entity in the map
		pLaser = dynamic_cast<CEnvPortalLaser*>(gEntList.FindEntityByClassname(pLaser, "env_portal_laser"));
	}

	// If no laser is hitting the catcher, trigger the appropriate output.
	OnUnpowered();
}


void CPropLaserRelay::OnPowered()
{
	//Msg("OnPowered called\n");
	// Check if the m_bHasFiredPowered is false, if so run the below code
	if (!m_bHasFiredPowered)
	{
		// Fire the OnPowered output
		m_OnPowered.FireOutput(this, this);
		Msg("Fired Onpowered output\n");
		m_bHasFiredPowered = true;
		m_bHasFiredUnpowered = false;
		// Set the skin of the catcher to be enabled
		m_nSkin = 1;
		// Start looping the active animation
		m_bIsActive = true;
		// Play the sounds
		StopSound("prop_laser_catcher.poweroff");
		EmitSound("prop_laser_catcher.poweron");
		EmitSound("prop_laser_catcher.powerloop");
		// Dispatch particle effect
		DispatchParticleEffect("laser_relay_powered", PATTACH_POINT_FOLLOW, this, "particle_emitter", false, -1, nullptr);
	}
}

void CPropLaserRelay::OnUnpowered()
{
	//Msg("OnUnpowered called\n");
	// Check if m_bHasFiredUnpowered is false, if so run the below code
	if (!m_bHasFiredUnpowered)
	{
		// Fire the OnUnpowered output
		m_OnUnpowered.FireOutput(this, this);
		Msg("Fired Unpowered output\n");
		m_bHasFiredPowered = false;
		m_bHasFiredUnpowered = true;
		// Set the skin of the catcher to be disabled
		m_nSkin = 2;
		// Stop any animations
		ResetSequence(2);
		m_bIsActive = false;
		StopSound("prop_laser_catcher.poweron");
		StopSound("prop_laser_catcher.powerloop");
		EmitSound("prop_laser_catcher.poweroff");
		// Stop all particles
		StopParticleEffect(this, "laser_relay_powered");
	}
}
