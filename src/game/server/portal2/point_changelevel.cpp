#include "cbase.h"
#include "mapentities.h" // Include this for the engine function

// This file is mostly useless, its just here so the vscripts valve uses think this entity is valid and will use it for the changelevel rather than the 5 second longer normal transition it falls back to.

class CPointChangeLevel : public CPointEntity
{
public:
	DECLARE_CLASS(CPointChangeLevel, CPointEntity);
	DECLARE_DATADESC();

	void Spawn() override;
	void Think() override;
	void InputChangeLevel(inputdata_t &inputData);

private:
	string_t m_szNextMapName; // Store the next map's name
	bool m_bStopChecking;

	void TeleportPlayerToLandmark(CBaseEntity* pLandmark); // Function to teleport player to the landmark
};

LINK_ENTITY_TO_CLASS(point_changelevel, CPointChangeLevel);

BEGIN_DATADESC(CPointChangeLevel)
// Input function mapping
DEFINE_INPUTFUNC(FIELD_STRING, "ChangeLevel", InputChangeLevel),
END_DATADESC()

void CPointChangeLevel::Spawn()
{
	m_bStopChecking = false;
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPointChangeLevel::Think()
{
	// This code was commented as when the player died this would accidentally teleport them to the start box rather than the elevator after the map was reloaded

	// Check if we need to teleport and if we should stop checking
	if (!m_bStopChecking)
	{
		// Search for the info_landmark_entry entity
		CBaseEntity* pLandmark = gEntList.FindEntityByClassname(nullptr, "info_landmark_entry");

		if (pLandmark)
		{
			// Found the landmark, set flags
			m_bStopChecking = true;
			//Msg("Teleporting to entry landmark pos\n");

			// Call the teleport function and pass the landmark entity
			//TeleportPlayerToLandmark(pLandmark);
		}
		else
		{
			// Landmark not found or multiple found, print message and stop checking
			//Warning("Could not find info_landmark_entry or multiple found. Stopping checks.\n");
			m_bStopChecking = true;
		}
	}
	BaseClass::Think();
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CPointChangeLevel::InputChangeLevel(inputdata_t &inputData)
{
	// Retrieve the next map's name from the input
	const char* nextMapName = inputData.value.String();

	// Convert to string_t
	m_szNextMapName = AllocPooledString(nextMapName);

	// Print the next map name to the console
	//Warning("Initiating transition to map %s\n", STRING(m_szNextMapName));

	// Trigger a changelevel to the specified map
	engine->ChangeLevel(STRING(m_szNextMapName), nullptr);
}

void CPointChangeLevel::TeleportPlayerToLandmark(CBaseEntity* pLandmark)
{
	// Check if the landmark entity is valid
	if (!pLandmark)
	{
		Warning("Invalid landmark entity. Aborting\n");
		return;
	}

	// Loop through all players and find the one you want to teleport
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);

		// Check if this player is connected and valid
		if (pPlayer && pPlayer->IsConnected())
		{
			// Teleport the player to the landmark's position
			pPlayer->SetAbsOrigin(pLandmark->GetAbsOrigin());
			//Msg("point_changelevel tasks done\n");
		}
	}
}
