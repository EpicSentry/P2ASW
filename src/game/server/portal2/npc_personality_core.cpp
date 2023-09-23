#include "cbase.h"
#include "ai_baseactor.h"
#include "ai_basenpc.h"
#include "ai_playerally.h"
#include "props.h"
#include <sceneentity.h>
#include "inforemarkable.h"
#include "choreoevent.h"
#include "choreoscene.h"
#include "choreoactor.h"

#define	CORE_MODEL "models/npcs/personality_sphere/personality_sphere.mdl"
#define	CORE_SKINS_MODEL "models/npcs/personality_sphere/personality_sphere_skins.mdl"

class CNPCPersonalityCore : public CAI_PlayerAlly, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS(CNPCPersonalityCore, CAI_PlayerAlly);
	DECLARE_DATADESC();

	CNPCPersonalityCore()
	{
	}

	bool CreateVPhysics(void)
	{
		VPhysicsDestroyObject();
		RemoveSolidFlags(FSOLID_NOT_SOLID);
		SetSolid(SOLID_BBOX);
		IPhysicsObject *pPhysicsObject = VPhysicsInitNormal(SOLID_VPHYSICS, GetSolidFlags(), false);
		if (pPhysicsObject == NULL)
			return false;

		pPhysicsObject->SetMass(75.0f);
		SetMoveType(MOVETYPE_VPHYSICS, MOVECOLLIDE_DEFAULT);
		return true;
	}

	void Spawn(void);
	void Precache(void);

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps();

	void OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason);

	bool IsOkToSpeakInResponseToPlayer(void);
	bool TestRemarkingUpon(CInfoRemarkable* pRemarkable);
	void OnFizzled();
	void HandleAnimEvent(animevent_t* pEvent);
	void PrescheduleThink(void);
	int	TranslateSchedule(int scheduleType);

	void InputEnablePickup(inputdata_t& inputdata);
	void InputDisablePickup(inputdata_t& inputdata);
	void InputForcePickup(inputdata_t& inputdata);
	void InputSetIdleSequence(inputdata_t& inputdata);
	void InputPlayAttach(inputdata_t& inputdata);
	void InputPlayDetach(inputdata_t& inputdata);
	void InputPlayLock(inputdata_t& inputdata);
	void InputEnableMotion(inputdata_t& inputdata);
	void InputDisableMotion(inputdata_t& inputdata);
	bool StartSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* event, CChoreoActor* actor, CBaseEntity* pTarget);
	void GatherConditions();

	bool m_bUseAltModel;
	bool m_bPickupEnabled;
	int m_iIdleOverrideSequence;
	float m_flAnimResetTime;
	bool m_bAttached;
	int m_nIdealSequence;
	bool m_bHasBeenPickedUp;

private:
	CHandle<CBasePlayer>	m_hPhysicsAttacker;
	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunDrop;
};

LINK_ENTITY_TO_CLASS(npc_personality_core, CNPCPersonalityCore);

BEGIN_DATADESC(CNPCPersonalityCore)
DEFINE_KEYFIELD(m_bUseAltModel, FIELD_BOOLEAN, "altmodel"),
DEFINE_INPUTFUNC(FIELD_VOID, "EnablePickup", InputEnablePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "DisablePickup", InputDisablePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "ForcePickup", InputForcePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "SetIdleSequence", InputSetIdleSequence),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayAttach", InputPlayAttach),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayDetach", InputPlayDetach),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayLock", InputPlayLock),
DEFINE_INPUTFUNC(FIELD_VOID, "EnableMotion", InputEnableMotion),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableMotion", InputDisableMotion),
DEFINE_USEFUNC(Use),
DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
DEFINE_OUTPUT(m_OnPhysGunDrop, "OnPhysGunDrop"),

END_DATADESC()

void CNPCPersonalityCore::Precache(void)
{
	if (m_bUseAltModel == false) {
		PrecacheModel(CORE_MODEL);
	}
	else {
		PrecacheModel(CORE_SKINS_MODEL);
	}
	PrecacheInstancedScene("scenes/npc/sp_proto_sphere/sphere_plug_attach.vcd");
	PrecacheParticleSystem("flashlight_thirdperson");
	BaseClass::Precache();
}

void CNPCPersonalityCore::Spawn(void)
{
	Precache();
	CapabilitiesClear();
	if (m_bUseAltModel == false) {
		SetModel(CORE_MODEL);
	}
	else {
		SetModel(CORE_SKINS_MODEL);
	}
	SetHullType(HULL_SMALL_CENTERED);
	SetHullSizeNormal();
	SetDefaultEyeOffset();
	SetSolid(SOLID_VPHYSICS);
	SetSolidFlags(FSOLID_NOT_STANDABLE);
	SetCollisionGroup(COLLISION_GROUP_NONE);
	SetMoveType(MOVETYPE_VPHYSICS,MOVECOLLIDE_DEFAULT);
	AddFlag(FL_UNBLOCKABLE_BY_PLAYER);
	CapabilitiesAdd(bits_CAP_ANIMATEDFACE);
	m_NPCState = NPC_STATE_NONE;
	m_takedamage = DAMAGE_EVENTS_ONLY;
	SetBloodColor(DONT_BLEED);
	NPCInit();
	m_flFieldOfView = -1.0f;
	m_bRemarkablePolling = true;

	CBaseAnimating::Spawn();
	SetBlocksLOS(false);

	m_flAnimResetTime = 0.0f;
	m_bHasBeenPickedUp = false;
	m_bPickupEnabled = true;
	m_bAttached = false;
	m_iIdleOverrideSequence = -1;

	if (m_bUseAltModel == false)
	{
		int iEyes = LookupAttachment("eyes");
		matrix3x4_t eyesToWorld;
		GetAttachment(iEyes, eyesToWorld);

		CBaseEntity* pProjectedTexture = CreateEntityByName("env_projectedtexture");
		pProjectedTexture->KeyValue("lightfov", "85.0f");
		pProjectedTexture->KeyValue("enableshadows", "1");
		pProjectedTexture->KeyValue("texturename", "effects/flashlight001_improved");
	
		variant_t emptyVariant;
		AcceptInput("TurnOff", NULL, NULL, emptyVariant, 0);
	}
	if (m_pParent != NULL)
		AddSolidFlags(FSOLID_NOT_SOLID);

	return;
}

int CNPCPersonalityCore::ObjectCaps()
{
	return UsableNPCObjectCaps(BaseClass::ObjectCaps());
}

void CNPCPersonalityCore::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer* pPlayer = ToBasePlayer(pActivator);

	if (((m_bPickupEnabled != false) && (pActivator != false)) && (pPlayer != false)) {
		SetParent(0,-1);
		CreateVPhysics();
		pPlayer->PickupObject(this, true);
	}
	return;
}

void CNPCPersonalityCore::OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason)
{
	m_hPhysicsAttacker = pPhysGunUser;

	if (reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER)
	{
		m_OnPlayerPickup.FireOutput(pPhysGunUser, this);
	}
}

void CNPCPersonalityCore::OnPhysGunDrop(CBasePlayer *pPhysGunUser, PhysGunDrop_t reason)
{
	m_OnPhysGunDrop.FireOutput(pPhysGunUser, this);
}

void CNPCPersonalityCore::InputEnablePickup(inputdata_t& inputdata)
{
	m_bPickupEnabled = true;
}

void CNPCPersonalityCore::InputDisablePickup(inputdata_t& inputdata)
{
	m_bPickupEnabled = false;
}

void CNPCPersonalityCore::InputForcePickup(inputdata_t& inputdata)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	if (pPlayer != false) {
		SetParent(0, -1);
		CreateVPhysics();
		pPlayer->PickupObject(this, true);
	}
}

void CNPCPersonalityCore::InputSetIdleSequence(inputdata_t& inputdata)
{
	m_iIdleOverrideSequence = LookupSequence(STRING(inputdata.value.StringID()));
	m_flAnimResetTime = -1.0f;
}

bool CNPCPersonalityCore::IsOkToSpeakInResponseToPlayer(void)
{
	return IsOkToSpeak(SPEECH_PRIORITY,true);
}

bool CNPCPersonalityCore::TestRemarkingUpon(CInfoRemarkable* pRemarkable)
{
	return IsLineOfSightClear( pRemarkable, IGNORE_ACTORS );
}

void CNPCPersonalityCore::OnFizzled()
{
	ResponseRules::CRR_Concept::CRR_Concept("TLK_FIZZLED");
	BaseClass::OnFizzled();
}

void CNPCPersonalityCore::HandleAnimEvent(animevent_t* pEvent)
{
	if (m_pPrimaryBehavior != false) {
		m_pPrimaryBehavior->HandleAnimEvent(pEvent);
	return;
	}
	BaseClass::HandleAnimEvent(pEvent);
	return;
}

void CNPCPersonalityCore::PrescheduleThink()
{
	{
	if ((m_flAnimResetTime < (gpGlobals->curtime) && (m_flAnimResetTime != 0.0f))) {
		if (m_iIdleOverrideSequence < 0) {
			if (m_bAttached == false) {
				SetIdealActivity(ACT_IDLE);
			}
			else {
				m_nIdealSequence = LookupSequence("sphere_plug_idle_neutral");
				SetIdealActivity(ACT_SPECIFIC_SEQUENCE);
			}
		}
		else {
			SetIdealActivity(ACT_SPECIFIC_SEQUENCE);
		}
		m_flCycle = 0.0f;
	}
	m_flAnimResetTime = 0.0f;
	}
	BaseClass::PrescheduleThink();
	return;
}

void CNPCPersonalityCore::InputPlayAttach(inputdata_t& inputdata)
{
	InstancedScriptedScene(this,"scenes/npc/sp_proto_sphere/sphere_plug_attach.vcd");
	m_bAttached = true;
	return;
}

void CNPCPersonalityCore::InputPlayDetach(inputdata_t& inputdata)
{
	m_bAttached = false;
	m_flAnimResetTime = -1.0f;
	RemoveSolidFlags(FSOLID_NOT_SOLID);
	return;
}

void CNPCPersonalityCore::InputPlayLock(inputdata_t& inputdata)
{
	InstancedScriptedScene(this, "scenes/npc/sp_proto_sphere/sphere_plug_lock.vcd");
	SetSolidFlags(FSOLID_NOT_SOLID);
}

int CNPCPersonalityCore::TranslateSchedule(int schedule)
{
	return BaseClass::TranslateSchedule( schedule );
}

void CNPCPersonalityCore::InputEnableMotion(inputdata_t& inputdata)
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject != NULL ) {
		pPhysicsObject->EnableMotion(true);
		pPhysicsObject->Wake();
	}
	CreateVPhysics();
	return;
}

void CNPCPersonalityCore::InputDisableMotion(inputdata_t& inputdata)
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject != NULL) {
		pPhysicsObject->EnableMotion(false);
	}
	return;
}

bool CNPCPersonalityCore::StartSceneEvent( CSceneEventInfo *info, CChoreoScene *scene, CChoreoEvent *event, CChoreoActor *actor, CBaseEntity *pTarget )
{
	switch (info->m_pEvent->GetType() )
	case CChoreoEvent::SEQUENCE: 
	{
		return BaseClass::StartSceneEvent(info, scene, event, actor, pTarget);
	}

	info->m_nSequence = LookupSequence(event->GetParameters());

	if (info->m_nSequence < 0)
		return false;

	ResetIdealActivity(ACT_SPECIFIC_SEQUENCE);
	m_nIdealSequence = m_nSequence;
	float flResetTime = m_flAnimResetTime;

	float flDuration = (event->GetEndTime() - scene->GetTime());
	if ((flDuration) + (gpGlobals->curtime < flResetTime)) {
		return true;
	}

	m_flAnimResetTime = (flDuration) + (gpGlobals->curtime);
	return true;
}

void CNPCPersonalityCore::GatherConditions()
{
	BaseClass::GatherConditions();
}
