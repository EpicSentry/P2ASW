#include "cbase.h"
#include "ai_basenpc.h"
#include "baseanimating.h"
#include "sprite.h"
#include "props.h"
#include "particle_parse.h"
#include "particle_system.h"
#include "player_pickup.h"

#define	CUBE_MODEL	"models/npcs/monsters/monster_a.mdl"

class CPropMonsterBox : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropMonsterBox, CDynamicProp);
	DECLARE_DATADESC();

	CPropMonsterBox()
	{
	}

	bool CreateVPhysics()
	{
		VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
		return true;
	}

	void JumpThink(void);
	void Spawn(void);
	void Precache(void);
	void OnUse();

	//Use
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	void ReachedEndOfSequence(void);

	//Pickup
	void Pickup(void);
	void OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason);

private:
	CHandle<CBasePlayer>	m_hPhysicsAttacker;

	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunDrop;
	COutputEvent m_OnExplode;
};

LINK_ENTITY_TO_CLASS(prop_monster_box, CPropMonsterBox);

BEGIN_DATADESC(CPropMonsterBox)
// Save/load
DEFINE_USEFUNC(Use),
// Output
DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
DEFINE_OUTPUT(m_OnPhysGunDrop, "OnPhysGunDrop"),

END_DATADESC()

void CPropMonsterBox::Precache(void)
{
	PrecacheModel(CUBE_MODEL);
	BaseClass::Precache();
}

void CPropMonsterBox::Spawn(void)
{
	Precache();
	BaseClass::Spawn();

	SetModel(CUBE_MODEL);
	SetSolid(SOLID_VPHYSICS);

	// In order to pick it up, needs to be physics.
	CreateVPhysics();

	SetUse(&CPropMonsterBox::Use);

	ReachedEndOfSequence();
}

int CPropMonsterBox::ObjectCaps()
{
	int caps = BaseClass::ObjectCaps();

	caps |= FCAP_IMPULSE_USE;

	return caps;
}

void CPropMonsterBox::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer(pActivator);
	if (pPlayer)
	{
		pPlayer->PickupObject(this);
		OnUse();
	}
}

void CPropMonsterBox::OnUse()
{
}

void CPropMonsterBox::OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason)
{
	m_hPhysicsAttacker = pPhysGunUser;

	if (reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER)
	{
		m_OnPlayerPickup.FireOutput(pPhysGunUser, this);
	}
}

void CPropMonsterBox::OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason)
{
	m_OnPhysGunDrop.FireOutput(pPhysGunUser, this);
}

void CPropMonsterBox::ReachedEndOfSequence(void)
{
	/*Vector vecStartPos = GetAbsOrigin();

	Vector vecEndPos = GetAbsOrigin();
	vecEndPos.z -= 16;

	trace_t	belowTrace;
	UTIL_TraceLine(vecStartPos, vecEndPos, MASK_ALL, this, COLLISION_GROUP_PLAYER, &belowTrace);

	if (belowTrace.DidHit())
	{
		Vector vecStartPos = GetAbsOrigin();

		Vector vecEndPos = GetAbsOrigin();
		vecEndPos.y += 16;

		trace_t	frontTrace;
		UTIL_TraceLine(vecStartPos, vecEndPos, MASK_ALL, this, COLLISION_GROUP_PLAYER, &frontTrace);

		if (frontTrace.DidHit())
		{
			PropSetAnim("trapped");
		}
		else
		{
		*/
			int sequenceToPerform = RandomInt(1, 3);
			char* seqName = new char[32];
			Q_snprintf(seqName, 32, "straight0%d", sequenceToPerform);
			PropSetAnim(seqName);
			/*
		}
	}
	else
	{
		PropSetAnim("intheair");
	}*/
}