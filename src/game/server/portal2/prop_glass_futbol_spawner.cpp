#include "cbase.h"
#include "futbol_catcher.h"
#include "tier0/memdbgon.h"

class CPropFutbolSpawner : public CFutbolCatcher
{
public:
	DECLARE_CLASS(CPropFutbolSpawner, CFutbolCatcher);
	DECLARE_DATADESC();

	CPropFutbolSpawner::CPropFutbolSpawner()
	{
		m_bHasFutbol = false;
		m_vCatcherBoxHalfDiagonal.x = 7.0f;
		m_vCatcherBoxHalfDiagonal.y = 7.0f;
		m_vCatcherBoxHalfDiagonal.z = 7.0f;
	}

	void Precache();
	void Spawn();
	void FutbolDestroyed();
	void FutbolGrabbed();
private:
	bool m_bHasFutbol;
	COutputEvent m_OnFutbolSpawned;
	COutputEvent m_OnFutbolGrabbed;
	void InputForceSpawn(inputdata_t& data);
	void SpawnFutbol();
};

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
