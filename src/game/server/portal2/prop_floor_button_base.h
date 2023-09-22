#include "cbase.h"
#include "props.h"

class CPropFloorButtonBase : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropFloorButtonBase, CDynamicProp);
	DECLARE_DATADESC();

	CPropFloorButtonBase()
	{
		wasTouching = false;
	}

	virtual void OnPress(void);
	virtual void OnUnpress(void);

	virtual void Spawn(void);

	void TouchThink(void);
	void HandleEntityTouch(CBaseEntity* entity);
protected:
	Vector m_differenceMap;

	COutputEvent m_OnPressed;
	COutputEvent m_OnUnPressed;

	int touchAmount;
	bool wasTouching;
};