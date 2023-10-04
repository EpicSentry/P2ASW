#include "cbase.h"
#include "prop_glass_futbol_spawner.h"
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(prop_glass_futbol_spawner, CPropFutbolSpawner);

BEGIN_DATADESC(CPropFutbolSpawner)
	DEFINE_INPUTFUNC(FIELD_VOID, "ForceSpawn", InputForceSpawn),
END_DATADESC()

void CPropFutbolSpawner::Precache()
{
	PrecacheModel("models/props/futbol_dispenser.mdl");
}

void CPropFutbolSpawner::Spawn()
{
	Precache();
	SetModel("models/props/futbol_dispenser.mdl");
	SetSolid(SOLID_VPHYSICS);
	BaseClass::Spawn();
	if (m_bHasFutbol)
		SpawnFutbol();
}

void CPropFutbolSpawner::FutbolDestroyed()
{
	SpawnFutbol();
}

void CPropFutbolSpawner::FutbolGrabbed()
{
	if (m_bHasFutbol)
		m_OnFutbolGrabbed.FireOutput(this, this);
	m_bHasFutbol = false;
}

void CPropFutbolSpawner::InputForceSpawn(inputdata_t& data)
{
	SpawnFutbol();
}

void CPropFutbolSpawner::SpawnFutbol()
{
	Vector vecBallSpawnPoint;

	CBaseEntity* pFutbol = CreateEntityByName("prop_glass_futbol");
	if (pFutbol)
	{
		m_OnFutbolSpawned.FireOutput(this, this);
		CBaseAnimating::GetAttachment("ball", vecBallSpawnPoint);
		SetAbsOrigin(vecBallSpawnPoint);
		m_bHasFutbol = true;
		DispatchSpawn(pFutbol, true);
		CaptureFutbol((CPropGlassFutbol*)pFutbol);
	}
}
