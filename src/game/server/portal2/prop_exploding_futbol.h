#ifndef PROP_EXPLODING_FUTBOL_H
#define PROP_EXPLODING_FUTBOL_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "props.h"

static const char* g_szExplodingFutbolAnimThinkContext = "ExplodingFutbolAnimateThinkContext";
static const char* g_szExplodingFutbolKillThinkContext = "ExplodingFutbolKillThinkContext";
static const char* g_szExplodingFutbolTimerThinkContext = "ExplodingFutbolTimerThinkContext";

enum ExplodingFutbolHolderType_t
{
	EXPLODING_FUTBOL_HELD_BY_NONE = 0,
	EXPLODING_FUTBOL_HELD_BY_PLAYER = 1,
	EXPLODING_FUTBOL_HELD_BY_SPAWNER = 2,
	EXPLODING_FUTBOL_HELD_BY_CATCHER = 3,
	EXPLODING_FUTBOL_HELD_BY_COUNT = 4
};

class CPropExplodingFutbol : public CPhysicsProp
{
public:
	DECLARE_CLASS(CPropExplodingFutbol, CPhysicsProp);
	DECLARE_DATADESC();

	CPropExplodingFutbol();
	void Precache();
	void Spawn();
	void Event_Killed(const CTakeDamageInfo& info);
	ExplodingFutbolHolderType_t GetHolder{ m_Holder };
	void SetHolder(ExplodingFutbolHolderType_t type) { GetHolder = type; }
	class CPortal_Player* GetLastPlayerToHold() { return m_hLastHeldByPlayer; }
	void KillThink();
	void AnimThink();
	void TimerThink();
	void ActivateFutbolTimer(float flTimer);
	void StopFutbolTimer();
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	int OnTakeDamage(const CTakeDamageInfo& info);
	void OnFizzled();
	void VPhysicsCollision(int index, gamevcollisionevent_t* pEvent);
	void VPhysicsUpdate(IPhysicsObject* pPhysics);
private:
	void KillFutbol();
	void DestroyFutbol(bool bExplode);
	void ExplodeFutbol();
	void DoExplosion(Vector& vecCenter, float flExplosionRadius, class CPortal_Base2D* pPortal);
	void DamageEntity(CBaseEntity* pEntity, Vector& vecCenter, Vector& vecForward);
	void InputExplode(inputdata_t& data);
	void InputDissolve(inputdata_t& data);
	ExplodingFutbolHolderType_t m_Holder;
	CHandle<CPortal_Player> m_hLastHeldByPlayer;
	CUtlVector<CBaseEntity*, CUtlMemory<CBaseEntity*, int> > m_ExplodedEntities;
	string_t m_strSpawnerName;
	bool m_bShouldRespawn;
	bool m_bExplodeOnTouch;
	bool m_bTimerActive;
	float m_flExplosionTimer;
	float m_flTotalTimer;
	float m_flLastTickTime;
	float m_flLastTimerSoundTime;
	float m_flLastFlashTime;
};
#endif //#ifndef PROP_EXPLODING_FUTBOL_H
