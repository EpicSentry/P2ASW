#ifndef POINT_LASER_TARGET_H
#define POINT_LASER_TARGET_H

#include "cbase.h"
#include "env_portal_laser.h"
#include "particle_parse.h"
#include "prop_weighted_cube.h"

class CLaserCatcher : public CBaseAnimating
{
	DECLARE_CLASS(CLaserCatcher, CBaseAnimating);
public:
	DECLARE_DATADESC();

	// Constructor
	CLaserCatcher();

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

#endif // POINT_LASER_TARGET_H