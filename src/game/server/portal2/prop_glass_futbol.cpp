#include <cbase.h>
#include <props.h>
#include <portal_player.h>

static const char* s_FutbolAnimateThinkContext = "FutbolAnimateThinkContext";

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
		m_vecThrowDirection.x = vec3_origin.x;
		m_vecThrowDirection.y = vec3_origin.y;
		m_vecThrowDirection.z = vec3_origin.z;
	}

	void Precache();
	void Spawn();

	//Think functions
	void AnimThink();

	string_t m_strSpawnerName;
	futbol_holder_type_t m_Holder;
	Vector m_vecThrowDirection;
private:
	CHandle< CPortal_Player > m_hLastHeldByPlayer;
};

LINK_ENTITY_TO_CLASS(prop_glass_futbol, CPropGlassFutbol);

BEGIN_DATADESC(CPropGlassFutbol)
DEFINE_THINKFUNC(AnimThink),
END_DATADESC()

void CPropGlassFutbol::Precache()
{
	BaseClass::Precache();
	PrecacheModel("models/props/futbol.mdl");
}

void CPropGlassFutbol::Spawn()
{
	Precache();
	KeyValue("model", "models/props/futbol.mdl");
	
	BaseClass::Spawn();
	ResetSequence(LookupSequence("rot"));
	SetContextThink( &CPropGlassFutbol::AnimThink, gpGlobals->curtime + 0.1f, s_FutbolAnimateThinkContext);
}

void CPropGlassFutbol::AnimThink()
{
	StudioFrameAdvance();
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime, s_FutbolAnimateThinkContext);
}
