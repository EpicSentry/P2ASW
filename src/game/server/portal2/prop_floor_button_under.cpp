#include "cbase.h"
#include "prop_floor_button_base.h"
#include "props.h"

#define	BUTTON_MODEL "models/props_underground/underground_floor_button.mdl"
#define	PRESS_SOUND "Portal.OGButtonDepress"
#define	UNPRESS_SOUND "Portal.OGButtonRelease"

class CPropFloorButtonUnder : public CPropFloorButtonBase
{
public:
	DECLARE_CLASS(CPropFloorButtonUnder, CPropFloorButtonBase);
	DECLARE_DATADESC();

	CPropFloorButtonUnder()
	{
		wasTouching = false;
		m_differenceMap = Vector(64, 64, 32);
	}

	void Spawn(void);
	void Precache(void);
	void OnPress(void);
	void OnUnpress(void);

private:
	int idleSequenceId;
	int downSequenceId;
	int upSequenceId;
	int idleDownSequenceId;
};

LINK_ENTITY_TO_CLASS(prop_under_floor_button, CPropFloorButtonUnder);

BEGIN_DATADESC(CPropFloorButtonUnder)
DEFINE_THINKFUNC(CPropFloorButtonBase::TouchThink),
END_DATADESC()

void CPropFloorButtonUnder::Precache(void)
{
	PrecacheModel(BUTTON_MODEL);

	PrecacheScriptSound(PRESS_SOUND);
	PrecacheScriptSound(UNPRESS_SOUND);

	BaseClass::Precache();
}

void CPropFloorButtonUnder::OnPress(void)
{
	BaseClass::OnPress();
	PropSetAnim("press");
	m_iszDefaultAnim = MAKE_STRING("press_idle");
	EmitSound(UNPRESS_SOUND);
}

void CPropFloorButtonUnder::OnUnpress(void)
{
	BaseClass::OnUnpress();
	PropSetAnim("release");
	m_iszDefaultAnim = MAKE_STRING("release_idle");
	EmitSound(PRESS_SOUND);
}

void CPropFloorButtonUnder::Spawn(void)
{
	Precache();
	SetModel(BUTTON_MODEL);
	BaseClass::Spawn();

	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	idleSequenceId = LookupSequence("release_idle");
	downSequenceId = LookupSequence("press");
	upSequenceId = LookupSequence("release");
	idleDownSequenceId = LookupSequence("press_idle");

	m_iszDefaultAnim = MAKE_STRING("release_idle");
	PropSetAnim("release_idle");

	SetThink(&CPropFloorButtonBase::TouchThink);
	SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}