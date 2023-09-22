#include "cbase.h"
#include "props.h"
#include "prop_button_base.h"

#define	BUTTON_MODEL "models/props/switch001.mdl"
#define	PRESS_SOUND "Portal.button_down"
#define	UNPRESS_SOUND "Portal.button_up"

class CPropButton : public CPropButtonBase
{
public:
	DECLARE_CLASS(CPropButton, CPropButtonBase);

	CPropButton()
	{
	}
	void Spawn(void);
	void Precache(void);

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void Press(void);
	void Unpress(void);
	void ReachedEndOfSequence(void);
};

LINK_ENTITY_TO_CLASS(prop_button, CPropButton);

void CPropButton::Precache(void)
{
	PrecacheModel(BUTTON_MODEL);

	PrecacheScriptSound(PRESS_SOUND);
	PrecacheScriptSound(UNPRESS_SOUND);

	BaseClass::Precache();
}

void CPropButton::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	Press();
}

void CPropButton::ReachedEndOfSequence(void)
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

void CPropButton::Press(void)
{
	PropSetAnim("down");
	m_iszDefaultAnim = MAKE_STRING("idle_down");
	EmitSound(PRESS_SOUND);
}

void CPropButton::Unpress(void)
{
	PropSetAnim("up");
	m_iszDefaultAnim = MAKE_STRING("idle");
}

void CPropButton::Spawn(void)
{
	Precache();
	BaseClass::Spawn();
	SetModel(BUTTON_MODEL);

	idleSequenceId = LookupSequence("idle");
	downSequenceId = LookupSequence("down");
	upSequenceId = LookupSequence("up");
	idleDownSequenceId = LookupSequence("idle_down");

	CheckSequence(idleSequenceId);
	CheckSequence(downSequenceId);
	CheckSequence(upSequenceId);
	CheckSequence(idleDownSequenceId);

	SetUse(&CPropButton::Use);

	CreateVPhysics();
	m_iszDefaultAnim = MAKE_STRING("idle");
	PropSetAnim("idle");
	//SetSequence(idleCloseSequenceId);
}