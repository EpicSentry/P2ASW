#include "cbase.h"
#include "props.h"

class CPropButtonBase : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropButtonBase, CDynamicProp);
	DECLARE_DATADESC();

	CPropButtonBase()
	{
	}

	bool CreateVPhysics()
	{
		VPhysicsInitStatic();
		return true;
	}

	void InputPress(inputdata_t& data);

	void Press(void);
	void Unpress(void);
	void Reset(void);

	void CheckSequence(int id);

	void Spawn(void);

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	void ReachedEndOfSequence(void);
protected:
	int idleSequenceId;
	int downSequenceId;
	int upSequenceId;
	int idleDownSequenceId;

	COutputEvent m_OnPressed;
	COutputEvent m_OnButtonReset;
	COutputEvent m_OnPressedBlue;
	COutputEvent m_OnPressedOrange;
};