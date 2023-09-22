#include "cbase.h"
#include "props.h"
#include "prop_button_base.h"

#define	BUTTON_MODEL "models/props_underground/underground_testchamber_button.mdl"
#define	PRESS_SOUND "Portal.OGSwitchPress"
#define	UNPRESS_SOUND "Portal.OGSwitchRelease"

class CPropUnderButton : public CPropButtonBase
{
public:
	DECLARE_CLASS(CPropUnderButton, CPropButtonBase);

	CPropUnderButton()
	{
	}
	void Spawn(void);
	void Precache(void);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void Press(void);
	void Unpress(void);
	void ReachedEndOfSequence(void);
};

LINK_ENTITY_TO_CLASS(prop_under_button, CPropUnderButton);

void CPropUnderButton::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	Press();
}

void CPropUnderButton::ReachedEndOfSequence(void)
{
	BaseClass::ReachedEndOfSequence();

	if (GetSequence() == downSequenceId)
	{
		Unpress();
	}

	if (GetSequence() == upSequenceId)
	{
		Reset();
	}
}

void CPropUnderButton::Precache(void)
{
	PrecacheModel(BUTTON_MODEL);

	PrecacheScriptSound(PRESS_SOUND);
	PrecacheScriptSound(UNPRESS_SOUND);

	BaseClass::Precache();
}

void CPropUnderButton::Press(void)
{
	PropSetAnim("press");
	m_iszDefaultAnim = MAKE_STRING("press_idle");
	EmitSound(PRESS_SOUND);
}

void CPropUnderButton::Unpress(void)
{
	PropSetAnim("release");
	m_iszDefaultAnim = MAKE_STRING("release_idle");
}

void CPropUnderButton::Spawn(void)
{
	Precache();
	BaseClass::Spawn();
	SetModel(BUTTON_MODEL);

	idleSequenceId = LookupSequence("release_idle");
	downSequenceId = LookupSequence("press");
	upSequenceId = LookupSequence("release");
	idleDownSequenceId = LookupSequence("press_idle");

	CheckSequence(idleSequenceId);
	CheckSequence(downSequenceId);
	CheckSequence(upSequenceId);
	CheckSequence(idleDownSequenceId);

	SetUse(&CPropUnderButton::Use);

	CreateVPhysics();
	m_iszDefaultAnim = MAKE_STRING("release_idle");
	PropSetAnim("release_idle");
	//SetSequence(idleCloseSequenceId);
}