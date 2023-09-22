#include "cbase.h"
#include "props.h"

#ifndef PROP_WEIGHTED_CUBE_H
#define PROP_WEIGHTED_CUBE_H

class CPropWeightedCube : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropWeightedCube, CDynamicProp);
	DECLARE_DATADESC();

	CPropWeightedCube()
	{
	}

	bool CreateVPhysics()
	{
		VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		return true;
	}

	void Spawn(void);
	void Precache(void);

	//Use
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	bool Dissolve(const char* materialName, float flStartTime, bool bNPCOnly, int nDissolveType, Vector vDissolverOrigin, int magnitude);

	void InputDissolve(inputdata_t &data);
	void InputSilentDissolve(inputdata_t &data);
	void InputPreDissolveJoke(inputdata_t &data);
	void InputExitDisabledState(inputdata_t &data);
	void CheckLaserHitCube();
	void Think();
	void UpdateLaser();
	void TurnLaserOff();
	void CC_Create_PortalWeightBox(void);
	bool IsCubeLaserHittingCatcher();
	bool m_bIsCubeLaserHittingCatcher;

	//Pickup
	//void Pickup(void);
	void OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason);
private:
	int	m_cubeType;
	int m_skinType;
	int m_paintPower;
	bool m_useNewSkins;
	bool m_allowFunnel;
	string_t m_uniqueID;
	//int m_oldSkin;

	CHandle<CBasePlayer> m_hPhysicsAttacker;

	COutputEvent m_OnOrangePickup;
	COutputEvent m_OnBluePickup;
	COutputEvent m_OnPlayerPickup;

	COutputEvent m_OnPainted;

	COutputEvent m_OnPhysGunDrop;

	COutputEvent m_OnFizzled;

	// Add this line to define the laser beam handle
	CHandle<CBeam> m_hCubeBeam;

};

#endif // PROP_WEIGHTED_CUBE_H