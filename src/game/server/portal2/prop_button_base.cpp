#include "cbase.h"
#include "prop_button_base.h"

//LINK_ENTITY_TO_CLASS(prop_button, CPropButton);

BEGIN_DATADESC(CPropButtonBase)

DEFINE_USEFUNC(Use),
//DEFINE_KEYFIELD(areaPortalName, FIELD_STRING, "AreaPortalWindow"),
DEFINE_INPUTFUNC(FIELD_VOID, "Press", InputPress),

DEFINE_OUTPUT(m_OnPressed, "OnPressed"),
DEFINE_OUTPUT(m_OnButtonReset, "OnButtonReset"),
DEFINE_OUTPUT(m_OnPressedBlue, "OnPressedBlue"),
DEFINE_OUTPUT(m_OnPressedOrange, "OnPressedOrange"),

END_DATADESC()

void CPropButtonBase::CheckSequence(int id)
{
	bool sequenceStatus = PrefetchSequence(id);
	sequenceStatus;
	Assert(id != ACT_INVALID && sequenceStatus);
}

int CPropButtonBase::ObjectCaps()
{
	int caps = BaseClass::ObjectCaps();

	caps |= FCAP_IMPULSE_USE;

	return caps;
}

void CPropButtonBase::InputPress(inputdata_t& data)
{
	Press();
}

void CPropButtonBase::Press()
{
	if (GetSequence() != idleSequenceId)
		return;
}

void CPropButtonBase::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	Press();
}

void CPropButtonBase::Spawn(void)
{
	BaseClass::Spawn();

	SetSolid(SOLID_VPHYSICS);

	SetUse(&CPropButtonBase::Use);

	CreateVPhysics();
	SetPlaybackRate(1.0f);
}

void CPropButtonBase::Unpress(void)
{
	m_OnPressed.FireOutput(this, this);
}

void CPropButtonBase::Reset(void)
{
	m_OnButtonReset.FireOutput(this, this);
}

void CPropButtonBase::ReachedEndOfSequence(void)
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