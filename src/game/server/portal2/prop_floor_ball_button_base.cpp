#include "cbase.h"
#include "prop_floor_ball_button_base.h"
//#include "prop_weighted_cube.cpp"

BEGIN_DATADESC(CPropFloorBallButtonBase)
DEFINE_THINKFUNC(TouchThink),
DEFINE_OUTPUT(m_OnPressed, "OnPressed"),
DEFINE_OUTPUT(m_OnUnPressed, "OnUnPressed"),
END_DATADESC()

void CPropFloorBallButtonBase::HandleEntityTouch(CBaseEntity* entity)
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

void CPropFloorBallButtonBase::TouchThink(void)
{
	touchAmount = 0;
	CBaseEntity *pObject = NULL;
	while ((pObject = gEntList.FindEntityByClassname(pObject, "prop_weighted_cube")) != NULL)
	{
		const char* modelName = STRING(pObject->GetModelName());
		if (Q_stristr(modelName, "models/props_gameplay/mp_ball.mdl"))
		{
			HandleEntityTouch(pObject);
		}
	}

	if (touchAmount == 0 && wasTouching)
	{
		OnPress();
	}
	else if (touchAmount > 0 && !wasTouching)
	{
		OnUnpress();
	}

	SetThink(&CPropFloorBallButtonBase::TouchThink);
	SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}


void CPropFloorBallButtonBase::OnPress(void)
{
	wasTouching = false;
	m_OnUnPressed.FireOutput(this, this);
}

void CPropFloorBallButtonBase::OnUnpress(void)
{
	wasTouching = true;
	m_OnPressed.FireOutput(this, this);
}

void CPropFloorBallButtonBase::Spawn(void)
{
	Precache();
	BaseClass::Spawn();

	SetPlaybackRate(1.0f);
}