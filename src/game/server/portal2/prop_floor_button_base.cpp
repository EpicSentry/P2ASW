#include "cbase.h"
#include "prop_floor_button_base.h"

BEGIN_DATADESC(CPropFloorButtonBase)
DEFINE_THINKFUNC(TouchThink),
DEFINE_OUTPUT(m_OnPressed, "OnPressed"),
DEFINE_OUTPUT(m_OnUnPressed, "OnUnPressed"),
END_DATADESC()

void CPropFloorButtonBase::HandleEntityTouch(CBaseEntity* entity)
{
	Vector ourPos = GetAbsOrigin();
	Vector theirPos = entity->GetAbsOrigin();
	Vector difference = ourPos - theirPos;

	if (difference.x > m_differenceMap.x || difference.x < -m_differenceMap.x)
		return;
	if (difference.y > m_differenceMap.y || difference.y < -m_differenceMap.y)
		return;
	if (difference.z > m_differenceMap.z || difference.z < -m_differenceMap.z)
		return;

	touchAmount++;
}

void CPropFloorButtonBase::TouchThink(void)
{
	touchAmount = 0;
	CBaseEntity *pObject = NULL;
	while ((pObject = gEntList.FindEntityByClassname(pObject, "prop_weighted_cube")) != NULL)
	{
		HandleEntityTouch(pObject);
	}
	while ((pObject = gEntList.FindEntityByClassname(pObject, "prop_monster_box")) != NULL)
	{
		HandleEntityTouch(pObject);
	}
	while ((pObject = gEntList.FindEntityByClassname(pObject, "player")) != NULL)
	{
		HandleEntityTouch(pObject);
	}

	if (touchAmount == 0 && wasTouching)
	{
		OnPress();
	}
	else if (touchAmount > 0 && !wasTouching)
	{
		OnUnpress();
	}

	SetThink(&CPropFloorButtonBase::TouchThink);
	SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}

void CPropFloorButtonBase::OnPress(void)
{
	wasTouching = false;
	m_OnUnPressed.FireOutput(this, this);
}

void CPropFloorButtonBase::OnUnpress(void)
{
	wasTouching = true;
	m_OnPressed.FireOutput(this, this);
}

void CPropFloorButtonBase::Spawn(void)
{
	Precache();
	BaseClass::Spawn();

	SetPlaybackRate(1.0f);
}