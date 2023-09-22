#include "cbase.h"
#include "props.h"

#define	DOOR_MODEL "models/props/portal_door_combined.mdl"
#define	OPEN_SOUND "prop_portal_door.open"
#define	CLOSE_SOUND "prop_portal_door.close"

class CPropTestchamberDoor : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropTestchamberDoor, CDynamicProp);
	DECLARE_DATADESC();

	CPropTestchamberDoor()
	{
	}

	void UpdateAreaPortals(bool isOpen);

	void CheckSequence(int id);

	void Spawn(void);
	void Precache(void);

	void Open(void);
	void Close(void);

	void ReachedEndOfSequence(void);

	void InputOpen(inputdata_t &inputData);
	void InputClose(inputdata_t &inputData);
private:
	COutputEvent m_OnClose;
	COutputEvent m_OnFullyClosed;
	COutputEvent m_OnOpen;
	COutputEvent m_OnFullyOpen;

	int closeSequenceId;
	int idleCloseSequenceId;
	int openSequenceId;
	int idleOpenSequenceId;

	string_t areaPortalName;
};

LINK_ENTITY_TO_CLASS(prop_testchamber_door, CPropTestchamberDoor);

BEGIN_DATADESC(CPropTestchamberDoor)

DEFINE_INPUTFUNC(FIELD_VOID, "Open", InputOpen),
DEFINE_INPUTFUNC(FIELD_VOID, "Close", InputClose),

DEFINE_KEYFIELD(areaPortalName, FIELD_STRING, "AreaPortalWindow"),

DEFINE_OUTPUT(m_OnClose, "OnClose"),
DEFINE_OUTPUT(m_OnFullyClosed, "OnFullyClosed"),
DEFINE_OUTPUT(m_OnOpen, "OnOpen"),
DEFINE_OUTPUT(m_OnFullyOpen, "OnFullyOpen"),

END_DATADESC()

void CPropTestchamberDoor::CheckSequence(int id)
{
	bool sequenceStatus = PrefetchSequence(id);
	sequenceStatus;
	Assert(id != ACT_INVALID && sequenceStatus);
}

void CPropTestchamberDoor::Precache(void)
{
	PrecacheModel(DOOR_MODEL);

	PrecacheScriptSound(OPEN_SOUND);
	PrecacheScriptSound(CLOSE_SOUND);

	BaseClass::Precache();
}

void CPropTestchamberDoor::Spawn(void)
{
	Precache();
	BaseClass::Spawn();
	SetModel(DOOR_MODEL);

	closeSequenceId = LookupSequence("close");
	idleCloseSequenceId = LookupSequence("idleclose");
	openSequenceId = LookupSequence("open");
	idleOpenSequenceId = LookupSequence("idleopen");

	CheckSequence(closeSequenceId);
	CheckSequence(idleCloseSequenceId);
	CheckSequence(openSequenceId);
	CheckSequence(idleOpenSequenceId);

	//SetSolid(SOLID_VPHYSICS);

	//CreateVPhysics();
	m_iszDefaultAnim = MAKE_STRING("idleclose");
	PropSetAnim("idleclose");
	//SetSequence(idleCloseSequenceId);
	SetPlaybackRate(1.0f);
}

void CPropTestchamberDoor::Open(void)
{
	UpdateAreaPortals(true);
	m_OnOpen.FireOutput(this, this);
	PropSetAnim("open");
	//SetSequence(openSequenceId);
	EmitSound(OPEN_SOUND);
}

void CPropTestchamberDoor::Close(void)
{
	m_OnClose.FireOutput(this, this);
	//SetSequence(closeSequenceId);
	PropSetAnim("close");
	EmitSound(CLOSE_SOUND);
}

void CPropTestchamberDoor::InputOpen(inputdata_t &inputData)
{
	Open();
}

void CPropTestchamberDoor::InputClose(inputdata_t &inputData)
{
	Close();
}

void CPropTestchamberDoor::UpdateAreaPortals(bool isOpen)
{
	CBaseEntity *pPortal = NULL;
	while ((pPortal = gEntList.FindEntityByName(pPortal, areaPortalName)) != NULL)
	{
		pPortal->Use(this, this, isOpen ? USE_ON : USE_OFF, 0);
	}
}

void CPropTestchamberDoor::ReachedEndOfSequence(void)
{
	BaseClass::ReachedEndOfSequence();

	if (GetSequence() == openSequenceId)
	{
		m_OnFullyOpen.FireOutput(this, this);
		PropSetAnim("idleopen");
		m_iszDefaultAnim = MAKE_STRING("idleopen");
	}

	if (GetSequence() == closeSequenceId)
	{
		m_OnFullyClosed.FireOutput(this, this);
		PropSetAnim("idleclose");
		m_iszDefaultAnim = MAKE_STRING("idleclose");
		UpdateAreaPortals(false);
	}
}