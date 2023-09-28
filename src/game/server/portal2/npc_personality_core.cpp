#include "cbase.h"
#include "ai_baseactor.h"
#include "ai_basenpc.h"
#include "ai_playerally.h"
#include "props.h"
#include "sceneentity.h"
#include "inforemarkable.h"
#include "choreoevent.h"
#include "choreoscene.h"
#include "choreoactor.h"
#include "tier0/memdbgon.h"

ConVar sv_personality_core_pca_pitch( "sv_personality_core_pca_pitch", "180", 0, "Pitch value for personality core perferred carry angles." );
ConVar sv_personality_core_pca_yaw( "sv_personality_core_pca_yaw", "-90", 0, "Yaw value for personality core perferred carry angles." );
ConVar sv_personality_core_pca_roll( "sv_personality_core_pca_roll", "195", 0, "Roll value for personality core perferred carry angles." );

#define	CORE_MODEL "models/npcs/personality_sphere/personality_sphere.mdl"
#define	CORE_SKINS_MODEL "models/npcs/personality_sphere/personality_sphere_skins.mdl"

class CNPC_PersonalityCore : public CAI_PlayerAlly, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS(CNPC_PersonalityCore, CAI_PlayerAlly);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CNPC_PersonalityCore()
	{
	}

	void Precache(void);
	void Spawn(void);

	void InputEnableMotion(inputdata_t& inputdata);
	void InputDisableMotion(inputdata_t& inputdata);
	void InputEnableFlashlight(inputdata_t& inputdata);
	void InputDisableFlashlight(inputdata_t& inputdata);
	void InputEnablePickup(inputdata_t& inputdata);
	void InputDisablePickup(inputdata_t& inputdata);
	void InputForcePickup(inputdata_t& inputdata);
	void InputPlayLock(inputdata_t& inputdata);
	void InputPlayAttach(inputdata_t& inputdata);
	void InputPlayDetach(inputdata_t& inputdata);
	void InputSetIdleSequence(inputdata_t& inputdata);
	void InputClearIdleSequence(inputdata_t& inputdata);
	void InputClearParent(inputdata_t& inputdata);

	int	TranslateSchedule(int scheduleType);
	void GatherConditions();
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnFizzled();

	bool CreateVPhysics(void)
	{
		VPhysicsDestroyObject();
		RemoveSolidFlags(FSOLID_NOT_SOLID);
		SetSolid(SOLID_BBOX);
		IPhysicsObject* pPhysicsObject = VPhysicsInitNormal(SOLID_VPHYSICS, GetSolidFlags(), false);
		if (pPhysicsObject == NULL)
			return false;

		pPhysicsObject->SetMass(75.0f);
		SetMoveType(MOVETYPE_VPHYSICS, MOVECOLLIDE_DEFAULT);
		return true;
	}
	
	int ObjectCaps();
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	QAngle PreferredCarryAngles();
	bool HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	CAI_Expresser* CreateExpresser();
	bool IsOkToSpeakInResponseToPlayer(void);
	bool StartSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* event, CChoreoActor* actor, CBaseEntity* pTarget);
	void PrescheduleThink(void);
protected:
	bool TestRemarkingUpon(CInfoRemarkable* pRemarkable);
	bool m_bHasBeenPickedUp;
	bool m_bPickupEnabled;
	bool m_bAttached;
	bool m_bUseAltModel;
	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPlayerDrop;
	float m_flAnimResetTime;
	int m_iIdleOverrideSequence;
	CNetworkVar(bool, m_bFlashlightEnabled);
	CHandle<CBaseEntity> m_hProjectedTexture;
private:
	CAI_Expresser* m_pExpresser;
};

LINK_ENTITY_TO_CLASS(npc_personality_core, CNPC_PersonalityCore);

BEGIN_DATADESC(CNPC_PersonalityCore)
DEFINE_INPUTFUNC(FIELD_VOID, "EnableMotion", InputEnableMotion),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableMotion", InputDisableMotion),
DEFINE_INPUTFUNC(FIELD_VOID, "EnableFlashlight", InputEnableFlashlight),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableFlashlight", InputDisableFlashlight),
DEFINE_INPUTFUNC(FIELD_VOID, "EnablePickup", InputEnablePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "DisablePickup", InputDisablePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "ForcePickup", InputForcePickup),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayLock", InputPlayLock),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayAttach", InputPlayAttach),
DEFINE_INPUTFUNC(FIELD_VOID, "PlayDetach", InputPlayDetach),
DEFINE_INPUTFUNC(FIELD_VOID, "SetIdleSequence", InputSetIdleSequence),
DEFINE_INPUTFUNC(FIELD_VOID, "ClearIdleSequence", InputClearIdleSequence),
DEFINE_INPUTFUNC(FIELD_VOID, "ClearParent", InputClearParent),
DEFINE_USEFUNC(Use),
DEFINE_KEYFIELD(m_bUseAltModel, FIELD_BOOLEAN, "altmodel"),
DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
DEFINE_OUTPUT(m_OnPlayerDrop, "OnPlayerDrop"),
DEFINE_FIELD(m_bFlashlightEnabled, FIELD_BOOLEAN),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CNPC_PersonalityCore, DT_NPC_Personality_Core)
SendPropBool( SENDINFO (m_bFlashlightEnabled) ),
END_SEND_TABLE()

void CNPC_PersonalityCore::Precache(void)
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

void CNPC_PersonalityCore::Spawn(void)
{
	Vector vecAttachOrigin;
	QAngle vecAttachAngles;

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

	if ( !m_bUseAltModel )
	{
		int iAttachmentIndex = LookupAttachment("eyes");
		GetAttachment(iAttachmentIndex, vecAttachOrigin, vecAttachAngles);

		m_hProjectedTexture = CreateEntityByName("env_projectedtexture");
		m_hProjectedTexture->KeyValue("lightfov", "85.0f");
		m_hProjectedTexture->KeyValue("enableshadows", "1");
		m_hProjectedTexture->KeyValue("texturename", "effects/flashlight001_improved");
		DispatchSpawn(m_hProjectedTexture);
		m_hProjectedTexture->SetAbsAngles(vecAttachAngles);
		m_hProjectedTexture->SetAbsOrigin(vecAttachOrigin);

		variant_t emptyVariant;
		m_hProjectedTexture->AcceptInput("TurnOff", NULL, NULL, emptyVariant, 0);
	}
	if (m_pParent != NULL)
		AddSolidFlags(FSOLID_NOT_SOLID);

}

void CNPC_PersonalityCore::InputEnableMotion(inputdata_t& inputdata)
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();

	if (pPhysicsObject)
	{
		pPhysicsObject->EnableMotion(true);
		pPhysicsObject->Wake();
	}
	CreateVPhysics();
}

void CNPC_PersonalityCore::InputDisableMotion(inputdata_t& inputdata)
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject != NULL) {
		pPhysicsObject->EnableMotion(false);
	}
}

void CNPC_PersonalityCore::InputEnableFlashlight(inputdata_t& inputdata)
{
	variant_t emptyVariant;
	if (m_hProjectedTexture)
		m_hProjectedTexture->AcceptInput("TurnOn", NULL, NULL, emptyVariant, 0);
	m_bFlashlightEnabled = true;
}

void CNPC_PersonalityCore::InputDisableFlashlight(inputdata_t& inputdata)
{
	variant_t emptyVariant;
	if (m_hProjectedTexture)
		m_hProjectedTexture->AcceptInput("TurnOff", NULL, NULL, emptyVariant, 0);
	m_bFlashlightEnabled = false;
}

void CNPC_PersonalityCore::InputEnablePickup(inputdata_t& inputdata)
{
	m_bPickupEnabled = true;
}

void CNPC_PersonalityCore::InputDisablePickup(inputdata_t& inputdata)
{
	m_bPickupEnabled = false;
}

void CNPC_PersonalityCore::InputForcePickup(inputdata_t& inputdata)
{
	CBasePlayer* LocalPlayer = UTIL_GetLocalPlayer();

	if (LocalPlayer)
	{
		SetParent(0, -1);
		CreateVPhysics();
		LocalPlayer->PickupObject(this, true);
	}
}

void CNPC_PersonalityCore::InputPlayLock(inputdata_t& inputdata)
{
	InstancedScriptedScene(this, "scenes/npc/sp_proto_sphere/sphere_plug_lock.vcd");
	SetSolidFlags(FSOLID_NOT_SOLID);
}

void CNPC_PersonalityCore::InputPlayAttach(inputdata_t& inputdata)
{
	InstancedScriptedScene(this, "scenes/npc/sp_proto_sphere/sphere_plug_attach.vcd");
	m_bAttached = true;
}

void CNPC_PersonalityCore::InputPlayDetach(inputdata_t& inputdata)
{
	m_bAttached = false;
	m_flAnimResetTime = -1.0f;
	RemoveSolidFlags(FSOLID_NOT_SOLID);
}

void CNPC_PersonalityCore::InputSetIdleSequence(inputdata_t& inputdata)
{
	m_iIdleOverrideSequence = LookupSequence(STRING(inputdata.value.StringID()));
	m_flAnimResetTime = -1.0f;
}

void CNPC_PersonalityCore::InputClearIdleSequence(inputdata_t& inputdata)
{
	m_iIdleOverrideSequence = -1;
	m_flAnimResetTime = -1.0f;
}

void CNPC_PersonalityCore::InputClearParent(inputdata_t& inputdata)
{
	BaseClass::InputClearParent(inputdata);
	RemoveSolidFlags(FSOLID_NOT_SOLID);
}

int CNPC_PersonalityCore::TranslateSchedule(int schedule)
{
	return BaseClass::TranslateSchedule(schedule);
}

void CNPC_PersonalityCore::GatherConditions()
{
	BaseClass::GatherConditions();
}

void CNPC_PersonalityCore::OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunPickup_t reason)
{
	if (reason == PICKED_UP_BY_PLAYER)
	{
		m_OnPlayerPickup.FireOutput(pPhysGunUser, this);
	}
}

void CNPC_PersonalityCore::OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason)
{
	m_OnPlayerDrop.FireOutput(pPhysGunUser, this);
}

void CNPC_PersonalityCore::OnFizzled()
{
	ResponseRules::CRR_Concept::CRR_Concept("TLK_FIZZLED");
	BaseClass::OnFizzled();
}

int CNPC_PersonalityCore::ObjectCaps()
{
	return UsableNPCObjectCaps(BaseClass::ObjectCaps());
}

void CNPC_PersonalityCore::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer* pPlayer = ToBasePlayer(pActivator);

	if (m_bPickupEnabled && pActivator && pPlayer )
	{
		SetParent(0,-1);
		CreateVPhysics();
		pPlayer->PickupObject(this, true);
	}
}

QAngle CNPC_PersonalityCore::PreferredCarryAngles()
{
	return QAngle(sv_personality_core_pca_pitch.GetFloat(), sv_personality_core_pca_yaw.GetFloat(), sv_personality_core_pca_roll.GetFloat());
}

CAI_Expresser* CNPC_PersonalityCore::CreateExpresser()
{
	m_pExpresser = new CAI_ExpresserWithFollowup(this);
	if (!m_pExpresser)
		return NULL;

	m_pExpresser->Connect(this);
	return m_pExpresser;
}

bool CNPC_PersonalityCore::IsOkToSpeakInResponseToPlayer(void)
{
	return IsOkToSpeak(SPEECH_PRIORITY,true);
}

bool CNPC_PersonalityCore::StartSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* event, CChoreoActor* actor, CBaseEntity* pTarget)
{
	bool result;
	double GetTime;
	float infoa;
	float infob;
	float infoc;
	float eventa;

	if (event->GetType() != CChoreoEvent::SEQUENCE)
		return BaseClass::StartSceneEvent(info, scene, event, actor, pTarget);

	const char* Parameters = event->GetParameters();
	int ISequence = LookupSequence(Parameters);

	if (ISequence < 0)
		return 0;

	ResetIdealActivity(ACT_SPECIFIC_SEQUENCE);
	infoa = m_flAnimResetTime;
	SetIdealSequence(ISequence);
	eventa = event->GetEndTime();
	if (infoa <= eventa - scene->GetTime() + gpGlobals->curtime)
	{
		infob = event->GetEndTime();
		GetTime = scene->GetTime();
		result = true;
		infoc = infob - GetTime + gpGlobals->curtime;
		m_flAnimResetTime = infoc;
	}
	else
	{
		this->m_flAnimResetTime = infoa;
		return 1;
	}
	return result;
}

void CNPC_PersonalityCore::PrescheduleThink()
{
	float m_flAnimResetTime = this->m_flAnimResetTime;
	if (gpGlobals->curtime > m_flAnimResetTime && m_flAnimResetTime != 0.0f)
	{
		m_iIdleOverrideSequence = this->m_iIdleOverrideSequence;
		if (m_iIdleOverrideSequence <= -1)
		{
			if (m_bAttached)
			{
				int iPlugidle = LookupSequence("sphere_plug_idle_neutral");
				SetIdealActivity(ACT_SPECIFIC_SEQUENCE);
				SetIdealSequence(iPlugidle);
			}
			else
			{
				SetIdealActivity(ACT_IDLE);
			}
		}
		else
		{
			SetIdealActivity(ACT_SPECIFIC_SEQUENCE);
		}
		m_flAnimResetTime = 0.0f;
	}
	BaseClass::PrescheduleThink();
}

bool CNPC_PersonalityCore::TestRemarkingUpon(CInfoRemarkable* pRemarkable)
{
	return IsLineOfSightClear( pRemarkable, IGNORE_ACTORS );
}

