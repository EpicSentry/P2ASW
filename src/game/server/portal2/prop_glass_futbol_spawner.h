#ifndef PROP_FUTBOL_SPAWNER_H
#define PROP_FUTBOL_SPAWNER_H
#pragma once

#include "cbase.h"
#include "futbol_catcher.h"

class CFutbolCatcher;
class CPropFutbolSpawner : public CFutbolCatcher
{
public:
	DECLARE_CLASS(CPropFutbolSpawner, CFutbolCatcher);
	DECLARE_DATADESC();

	CPropFutbolSpawner::CPropFutbolSpawner()
	{
		m_bHasFutbol = false;
		m_vCatcherBoxHalfDiagonal = Vector(7.0f, 7.0f, 7.0f);
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

#endif
