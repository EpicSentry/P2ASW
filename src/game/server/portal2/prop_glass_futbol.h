#ifndef PROP_GLASS_FUTBOL_H
#define PROP_GLASS_FUTBOL_H
#pragma once

#include "cbase.h"
#include "props.h"
#include "portal_player.h"

enum futbol_holder_type_t
{
	FUTBOL_HELD_BY_NONE = 0,
	FUTBOL_HELD_BY_PLAYER = 1,
	FUTBOL_HELD_BY_SPAWNER = 2,
	FUTBOL_HELD_BY_CATCHER = 3,
	FUTBOL_HELD_BY_COUNT = 4
};

class CPropGlassFutbol : public CPhysicsProp
{
public:
	DECLARE_CLASS(CPropGlassFutbol, CPhysicsProp);
	DECLARE_DATADESC();


	CPropGlassFutbol::CPropGlassFutbol()
	{
		m_Holder = FUTBOL_HELD_BY_NONE;
		m_vecThrowDirection = vec3_origin;
	}

	QAngle PreferredCarryAngles() { return QAngle(180, -90, 180); }
	bool HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	void Precache();
	void Spawn();
	void Event_Killed(const CTakeDamageInfo& info);

	//Think functions
	void ThrownThink();
	void AnimThink();

	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	enum futbol_holder_type_t GetHolder { m_Holder };
	void SetHolder(futbol_holder_type_t type) { GetHolder = type; }
	CPortal_Player* GetLastPlayerToHold() { return m_hLastHeldByPlayer; }
private:
	string_t m_strSpawnerName;
	futbol_holder_type_t m_Holder;
	Vector m_vecThrowDirection;
	CHandle<CPortal_Player> m_hLastHeldByPlayer;
};

#endif // PROP_GLASS_FUTBOL_H
