#ifndef PROP_FUTBOL_CATCHER_H
#define PROP_FUTBOL_CATCHER_H
#pragma once

#include "cbase.h"
#include "trace.h"
#include "prop_glass_futbol.h"

class CFutbolCatcher : public CBaseAnimating
{
public:
	DECLARE_CLASS(CFutbolCatcher, CBaseAnimating);
	DECLARE_DATADESC();

	CFutbolCatcher::CFutbolCatcher()
	{
		m_vCatcherBoxHalfDiagonal = Vector(25.0f, 25.0f, 25.0f);
		m_vecCatchBoxMins = vec3_origin;
		m_vecCatchBoxMaxs = vec3_origin;
		m_vecCatchBoxOrig = vec3_origin;
		m_bDisableRecaptureOnPlayerGrab = false;
	}
	void Spawn();

protected:
	CHandle<CBaseEntity> m_hCaughtFutbol;
	Vector m_vCatcherBoxHalfDiagonal;
	Vector m_vecCatchBoxMins;
	Vector m_vecCatchBoxMaxs;
	Vector m_vecCatchBoxOrig;
	bool m_bDisableRecaptureOnPlayerGrab;
	COutputEvent m_OnFutbolReleased;
	COutputEvent m_OnFutbolCaught;
	void CatchThink();
	void CaptureThink();
	void CaptureFutbol(CPropGlassFutbol* pFutbol);
};

class CFilterOnlyGlassFutbol : public CTraceFilterSimple
{
	DECLARE_CLASS(CFilterOnlyGlassFutbol, CTraceFilterSimple);
public:
	CFilterOnlyGlassFutbol(const IHandleEntity* passentity, int collisionGroup)
		: CTraceFilterSimple(passentity, collisionGroup)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity* pHandleEntity, int contentsMask);
};

#endif // PROP_FUTBOL_CATCHER_H
