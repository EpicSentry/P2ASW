#include "cbase.h"
#include "prop_floor_ball_button_base.h"
#include "props.h"

#define	BUTTON_MODEL "models/props/ball_button.mdl"
#define	PRESS_SOUND "Portal.ButtonDepress"
#define	UNPRESS_SOUND "Portal.ButtonRelease"

class CPropFloorBallButton : public CPropFloorBallButtonBase
{
public:
	DECLARE_CLASS(CPropFloorBallButton, CPropFloorBallButtonBase);
	DECLARE_DATADESC();

	CPropFloorBallButton()
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

LINK_ENTITY_TO_CLASS(prop_floor_ball_button, CPropFloorBallButton);

BEGIN_DATADESC(CPropFloorBallButton)
DEFINE_THINKFUNC(CPropFloorBallButtonBase::TouchThink),
END_DATADESC()

void CPropFloorBallButton::Precache(void)
{
	PrecacheModel(BUTTON_MODEL);

	PrecacheScriptSound(PRESS_SOUND);
	PrecacheScriptSound(UNPRESS_SOUND);

	BaseClass::Precache();
}

void CPropFloorBallButton::OnPress(void)
{
	BaseClass::OnPress();
	m_nSkin = 0;
	PropSetAnim("down");
	m_iszDefaultAnim = MAKE_STRING("idledown");
	EmitSound(UNPRESS_SOUND);
}

void CPropFloorBallButton::OnUnpress(void)
{
	BaseClass::OnUnpress();
	m_nSkin = 1;
	PropSetAnim("up");
	m_iszDefaultAnim = MAKE_STRING("BindPose");
	EmitSound(PRESS_SOUND);
}

void CPropFloorBallButton::Spawn(void)
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


	SetThink(&CPropFloorBallButtonBase::TouchThink);
	SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}