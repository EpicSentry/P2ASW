#include "cbase.h"
#include "prop_floor_button_base.h"
#include "props.h"

#define	BUTTON_MODEL "models/props/portal_button.mdl"
#define	PRESS_SOUND "Portal.ButtonDepress"
#define	UNPRESS_SOUND "Portal.ButtonRelease"

class CPropFloorButton : public CPropFloorButtonBase
{
public:
	DECLARE_CLASS(CPropFloorButton, CPropFloorButtonBase);
	DECLARE_DATADESC();

	CPropFloorButton()
	{
		wasTouching = false;
		m_differenceMap = Vector(48, 48, 32);
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

LINK_ENTITY_TO_CLASS(prop_floor_button, CPropFloorButton);

BEGIN_DATADESC(CPropFloorButton)
DEFINE_THINKFUNC(CPropFloorButtonBase::TouchThink),
END_DATADESC()

void CPropFloorButton::Precache(void)
{
	PrecacheModel(BUTTON_MODEL);

	PrecacheScriptSound(PRESS_SOUND);
	PrecacheScriptSound(UNPRESS_SOUND);

	BaseClass::Precache();
}

void CPropFloorButton::OnPress(void)
{
	BaseClass::OnPress();
	m_nSkin = 0;
	PropSetAnim("down");
	m_iszDefaultAnim = MAKE_STRING("idledown");
	EmitSound(UNPRESS_SOUND);
}

void CPropFloorButton::OnUnpress(void)
{
	BaseClass::OnUnpress();
	m_nSkin = 1;
	PropSetAnim("up");
	m_iszDefaultAnim = MAKE_STRING("BindPose");
	EmitSound(PRESS_SOUND);
}

void CPropFloorButton::Spawn(void)
{
	Precache();
	BaseClass::Spawn();
	SetModel(BUTTON_MODEL);

	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	idleSequenceId = LookupSequence("BindPose");
	downSequenceId = LookupSequence("down");
	upSequenceId = LookupSequence("up");
	idleDownSequenceId = LookupSequence("idledown");

	m_iszDefaultAnim = MAKE_STRING("BindPose");
	PropSetAnim("BindPose");


	SetThink(&CPropFloorButtonBase::TouchThink);
	SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}