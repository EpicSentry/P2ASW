#include "cbase.h"
#include "ai_baseactor.h"
#include "ai_basenpc.h"
#include "props.h"

#define	CORE_MODEL "models/npcs/personality_sphere/personality_sphere.mdl"

class CNPCPersonalityCore : public CAI_BaseNPC, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS(CNPCPersonalityCore, CAI_BaseNPC);
	DECLARE_DATADESC();

	CNPCPersonalityCore()
	{
	}

	bool CreateVPhysics(void)
	{
		VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		return true;
	}

	void Spawn(void);
	void Precache(void);

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	void Pickup(void);
	void OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason);
private:
	CHandle<CBasePlayer>	m_hPhysicsAttacker;
	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunDrop;
};

LINK_ENTITY_TO_CLASS(npc_personality_core, CNPCPersonalityCore);

BEGIN_DATADESC(CNPCPersonalityCore)
DEFINE_USEFUNC(Use),
DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
DEFINE_OUTPUT(m_OnPhysGunDrop, "OnPhysGunDrop"),

END_DATADESC()

void CNPCPersonalityCore::Precache(void)
{
	PrecacheModel(CORE_MODEL);

	BaseClass::Precache();
}

void CNPCPersonalityCore::Spawn(void)
{
	Precache();
	SetModel(CORE_MODEL);

	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	SetUse(&CNPCPersonalityCore::Use);
	BaseClass::Spawn();
	NPCInit();
}

int CNPCPersonalityCore::ObjectCaps()
{
	return UsableNPCObjectCaps(BaseClass::ObjectCaps());
}

void CNPCPersonalityCore::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer(pActivator);
	if (pPlayer)
	{
		pPlayer->PickupObject(this);
	}
}

void CNPCPersonalityCore::OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason)
{
	m_hPhysicsAttacker = pPhysGunUser;

	if (reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER)
	{
		m_OnPlayerPickup.FireOutput(pPhysGunUser, this);
	}
}

void CNPCPersonalityCore::OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason)
{
	m_OnPhysGunDrop.FireOutput(pPhysGunUser, this);
}