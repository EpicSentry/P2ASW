#include "cbase.h"
#include "trace.h"
#include "prop_glass_futbol.h"
#include "tier0/memdbgon.h"

class CFutbolCatcher : public CBaseAnimating
{
public:
	DECLARE_CLASS(CFutbolCatcher, CBaseAnimating);
	DECLARE_DATADESC();

	CFutbolCatcher::CFutbolCatcher()
	{
		m_vCatcherBoxHalfDiagonal.x = 25.0f;
		m_vCatcherBoxHalfDiagonal.y = 25.0f;
		m_vCatcherBoxHalfDiagonal.z = 25.0f;
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
	void CaptureFutbol(CPropGlassFutbol *pFutbol);
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

LINK_ENTITY_TO_CLASS(futbol_catcher, CFutbolCatcher);

BEGIN_DATADESC(CFutbolCatcher)
DEFINE_THINKFUNC(CatchThink),
END_DATADESC()

void CFutbolCatcher::Spawn()
{
	BaseClass::Spawn();
	GetAttachment("ball", m_vecCatchBoxOrig);

	m_vecCatchBoxMaxs = m_vCatcherBoxHalfDiagonal;
	SetThink(&CFutbolCatcher::CatchThink);
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CatchThink()
{
	Ray_t ray;
	trace_t tr;

	CFilterOnlyGlassFutbol filter(this, GetCollisionGroup());

	ray.Init( m_vecCatchBoxOrig, m_vecCatchBoxOrig, m_vecCatchBoxMins, m_vecCatchBoxMaxs);
	enginetrace->TraceRay(ray, MASK_SOLID, &filter, &tr);

	if (r_visualizetraces.GetBool())
		DebugDrawLine(tr.startpos, tr.endpos, 0xff, 0, 0, true, -1.0f);

	if (!m_bDisableRecaptureOnPlayerGrab)
	{
		CPropGlassFutbol* pGlassFutbol = dynamic_cast<CPropGlassFutbol*>(tr.m_pEnt);
		if (pGlassFutbol)
		{
			if (FUTBOL_HELD_BY_NONE || FUTBOL_HELD_BY_PLAYER)
			{
				CaptureFutbol(pGlassFutbol);
			}
		}
	}
	m_bDisableRecaptureOnPlayerGrab = false;
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CaptureThink()
{
	if (m_hCaughtFutbol)
	{
		SetThink(&CFutbolCatcher::CatchThink);
		m_OnFutbolReleased.FireOutput(this, this);
		m_bDisableRecaptureOnPlayerGrab = true;
	}
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CaptureFutbol(CPropGlassFutbol* pFutbol)
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();

	if (pFutbol)
	{
		if (pPhysicsObject)
			pPhysicsObject->EnableMotion(false);
		pFutbol->Teleport(&m_vecCatchBoxOrig, &vec3_angle, &vec3_origin, true);
		SetThink(&CFutbolCatcher::CaptureThink);
		pFutbol->GetHolder = FUTBOL_HELD_BY_CATCHER;
		m_OnFutbolCaught.FireOutput(pFutbol, pFutbol->GetLastPlayerToHold());
	}
}

bool CFilterOnlyGlassFutbol::ShouldHitEntity(IHandleEntity* pHandleEntity, int contentsMask)
{
	return true;
}
