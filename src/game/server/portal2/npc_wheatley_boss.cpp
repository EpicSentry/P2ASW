#include "cbase.h"
#include "ai_baseactor.h"
#include "ai_basenpc.h"
#include "props.h"

#define	BOSS_MODEL "models/npcs/glados/glados_wheatley_boss.mdl"

class CNPCWheatleyBoss : public CAI_BaseNPC
{
public:
	DECLARE_CLASS(CNPCWheatleyBoss, CAI_BaseNPC);
	DECLARE_DATADESC();

	CNPCWheatleyBoss()
	{
	}

	bool CreateVPhysics(void)
	{
		VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		return true;
	}

	void Spawn(void);
	void Precache(void);
private:
	CHandle<CBasePlayer>	m_hPhysicsAttacker;
	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunDrop;
};

LINK_ENTITY_TO_CLASS(npc_wheatley_boss, CNPCWheatleyBoss);

BEGIN_DATADESC(CNPCWheatleyBoss)
END_DATADESC()

void CNPCWheatleyBoss::Precache(void)
{
	PrecacheModel(BOSS_MODEL);

	BaseClass::Precache();
}

void CNPCWheatleyBoss::Spawn(void)
{
	Precache();
	SetModel(BOSS_MODEL);

	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	BaseClass::Spawn();
}