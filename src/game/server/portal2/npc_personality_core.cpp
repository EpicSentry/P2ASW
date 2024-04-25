//========= Copyright © Valve Corporation, All rights reserved. ===============//
//
// Purpose: Recreation of the Personality Core entity from Portal 2
// Recreated by Jordon (JordyPorgie)
//	
//=============================================================================//

//TODO: A fix is needed for whenever the core attempts to clear it's parent.

#include "cbase.h"
#include "ai_playerally.h"
#include "props.h"
#include "sceneentity.h"
#include "inforemarkable.h"
#include "choreoevent.h"
#include "choreoscene.h"
#include "choreoactor.h"
#include "env_projectedtexture.h"
#include "explode.h"
#include "portal_grabcontroller_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_personality_core_pca_pitch( "sv_personality_core_pca_pitch", "180", FCVAR_NONE, "Pitch value for personality core perferred carry angles." );
ConVar sv_personality_core_pca_yaw( "sv_personality_core_pca_yaw", "-90", FCVAR_NONE, "Yaw value for personality core perferred carry angles." );
ConVar sv_personality_core_pca_roll( "sv_personality_core_pca_roll", "195", FCVAR_NONE, "Roll value for personality core perferred carry angles." );

#define	CORE_MODEL "models/npcs/personality_sphere/personality_sphere.mdl"
#define	CORE_SKINS_MODEL "models/npcs/personality_sphere/personality_sphere_skins.mdl"

class CNPC_PersonalityCore : public CAI_PlayerAlly, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS(CNPC_PersonalityCore, CAI_PlayerAlly);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void Precache(void);
	void Spawn(void);
	void StartTask(const Task_t* pTask) { return; }
	void RunTask(const Task_t* pTask) { return; }
	bool ShouldSavePhysics() { return true; }
	void EnableMotion();

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
	void InputExplode(inputdata_t& inputdata);
	void InputClearParent(inputdata_t& inputdata);

	int	OnTakeDamage_Alive(const CTakeDamageInfo& inputInfo);
	int SelectSchedule() { return BaseClass::SelectSchedule(); }
	bool ShouldIgnite(const CTakeDamageInfo& info) { return false; }
	bool ShouldPlayIdleSound();
	void IdleSound();
	void HandleAnimEvent(animevent_t* pEvent) { BaseClass::HandleAnimEvent(pEvent); }
	int	TranslateSchedule(int scheduleType);
	void GatherConditions();
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunPickup_t reason);
	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnFizzled();

	bool CreateVPhysics();
	
	int ObjectCaps();
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void VPhysicsCollision(int index, gamevcollisionevent_t* pEvent);
	QAngle PreferredCarryAngles();
	bool HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	void ModifyOrAppendCriteria(AI_CriteriaSet& set);
	CAI_Expresser* CreateExpresser();
	void NotifySystemEvent(CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params);
	bool IsOkToSpeakInResponseToPlayer(void);
	bool StartSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* event, CChoreoActor* actor, CBaseEntity* pTarget);
	void PrescheduleThink(void);
protected:
	bool TestRemarkingUpon(CInfoRemarkable* pRemarkable);
	bool IsBeingHeldByPlayer();
	float GetPlayerSpeed();
	CBasePlayer* GetFirstPlayer();
	const char* GetPlayerHeldEntityName();
	float m_flNextIdleSoundTime;
	float m_flLastPhysicsImpactTime;
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
	DEFINE_CUSTOM_AI;
};

LINK_ENTITY_TO_CLASS(npc_personality_core, CNPC_PersonalityCore);

BEGIN_DATADESC(CNPC_PersonalityCore)
	DEFINE_FIELD(m_flNextIdleSoundTime, FIELD_TIME),
	DEFINE_FIELD(m_flLastPhysicsImpactTime, FIELD_FLOAT),
	DEFINE_FIELD(m_flAnimResetTime, FIELD_FLOAT),
	DEFINE_FIELD(m_bHasBeenPickedUp, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bPickupEnabled, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bAttached, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bFlashlightEnabled, FIELD_BOOLEAN),
	DEFINE_FIELD(m_iIdleOverrideSequence, FIELD_INTEGER),
	DEFINE_FIELD(m_hProjectedTexture, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_bUseAltModel, FIELD_BOOLEAN, "altmodel"),
	DEFINE_OUTPUT(m_OnPlayerPickup, "OnPlayerPickup"),
	DEFINE_OUTPUT(m_OnPlayerDrop, "OnPlayerDrop"),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnableMotion", InputEnableMotion),
	DEFINE_INPUTFUNC(FIELD_VOID, "DisableMotion", InputDisableMotion),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnableFlashlight", InputEnableFlashlight),
	DEFINE_INPUTFUNC(FIELD_VOID, "DisableFlashlight", InputDisableFlashlight),
	DEFINE_INPUTFUNC(FIELD_VOID, "ForcePickup", InputForcePickup),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnablePickup", InputEnablePickup),
	DEFINE_INPUTFUNC(FIELD_VOID, "DisablePickup", InputDisablePickup),
	DEFINE_INPUTFUNC(FIELD_VOID, "PlayLock", InputPlayLock),
	DEFINE_INPUTFUNC(FIELD_VOID, "PlayAttach", InputPlayAttach),
	DEFINE_INPUTFUNC(FIELD_VOID, "PlayDetach", InputPlayDetach),
	DEFINE_INPUTFUNC(FIELD_STRING, "SetIdleSequence", InputSetIdleSequence),
	DEFINE_INPUTFUNC(FIELD_VOID, "ClearIdleSequence", InputClearIdleSequence),
	DEFINE_INPUTFUNC(FIELD_VOID, "Explode", InputExplode),
	DEFINE_INPUTFUNC(FIELD_VOID, "ClearParent", InputClearParent),
	DEFINE_USEFUNC(Use),
END_DATADESC()

AI_BEGIN_CUSTOM_NPC(npc_personality_core, CNPC_PersonalityCore)

AI_END_CUSTOM_NPC()

IMPLEMENT_SERVERCLASS_ST(CNPC_PersonalityCore, DT_NPC_Personality_Core)
	SendPropBool( SENDINFO (m_bFlashlightEnabled) ),
END_SEND_TABLE()

static const char *pDefaultHeldItemName = "None";

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
	Precache();
	CapabilitiesClear();
	if (m_bUseAltModel == false)
		SetModel(CORE_MODEL);
	else
		SetModel(CORE_SKINS_MODEL);
	SetHullType(HULL_SMALL_CENTERED);
	SetHullSizeNormal();
	SetDefaultEyeOffset();
	SetSolid(SOLID_BBOX);
	SetSolidFlags(FSOLID_NOT_STANDABLE);
	SetCollisionGroup(COLLISION_GROUP_NONE);
	SetMoveType(MOVETYPE_VPHYSICS);
	AddFlag(FL_UNBLOCKABLE_BY_PLAYER);
	CapabilitiesAdd(bits_CAP_ANIMATEDFACE);
	m_NPCState = NPC_STATE_NONE;
	m_takedamage = DAMAGE_EVENTS_ONLY;
	SetBloodColor(DONT_BLEED);
	NPCInit();
	m_flFieldOfView = -1.0f;
	m_bRemarkablePolling = true;

	BaseClass::Spawn();
	SetBlocksLOS(false);

	m_flLastPhysicsImpactTime = 0.0f;
	m_flAnimResetTime = 0.0f;
	m_bHasBeenPickedUp = false;
	m_bPickupEnabled = true;
	m_bAttached = false;
	m_iIdleOverrideSequence = -1;
	m_bFlashlightEnabled = false;

	if ( !m_bUseAltModel )
	{
		int iAttachmentIndex = LookupAttachment("eyes");
		Vector vecAttachOrigin;
		QAngle vecAttachAngles;
		GetAttachment(iAttachmentIndex, vecAttachOrigin, vecAttachAngles);

		m_hProjectedTexture = CreateEntityByName("env_projectedtexture");
		m_hProjectedTexture->KeyValue("lightfov", "85.0f");
		m_hProjectedTexture->KeyValue("enableshadows", "1");
		m_hProjectedTexture->KeyValue("texturename", "effects/flashlight001_improved");
		DispatchSpawn(m_hProjectedTexture);
		m_hProjectedTexture->AddSpawnFlags(ENV_PROJECTEDTEXTURE_ALWAYSUPDATE);
		m_hProjectedTexture->SetAbsAngles(vecAttachAngles);
		m_hProjectedTexture->SetAbsOrigin(vecAttachOrigin);
		m_hProjectedTexture->SetParent(this, iAttachmentIndex);

		variant_t emptyVariant;
		m_hProjectedTexture->AcceptInput("TurnOff", NULL, NULL, emptyVariant, 0);
	}
	if (m_pParent)
		AddSolidFlags(FSOLID_NOT_SOLID);

}

void CNPC_PersonalityCore::EnableMotion()
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();

	if (pPhysicsObject)
	{
		pPhysicsObject->EnableMotion(true);
		pPhysicsObject->Wake();
	}
}

void CNPC_PersonalityCore::InputEnableMotion(inputdata_t& inputdata)
{
	EnableMotion();
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
		SetParent(NULL);
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

void CNPC_PersonalityCore::InputExplode(inputdata_t& inputdata)
{
	ExplosionCreate(WorldSpaceCenter(), GetAbsAngles(), this, 100, 500, (SF_ENVEXPLOSION_NODAMAGE | SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS | SF_ENVEXPLOSION_NOSMOKE | SF_ENVEXPLOSION_NOFIREBALLSMOKE), false);
	UTIL_ScreenShake(WorldSpaceCenter(), 10.0f, 150.0f, 1.0f, 750.0f, SHAKE_START);

	CPVSFilter filter(WorldSpaceCenter());
	for (int i = 0; i < 4; i++)
	{
		Vector gibVelocity = RandomVector(-100, 100);
		int iModelIndex = modelinfo->GetModelIndex(g_PropDataSystem.GetRandomChunkModel("MetalChunks"));
		te->BreakModel(filter, 0.0, GetAbsOrigin(), GetAbsAngles(), Vector(40, 40, 40), gibVelocity, iModelIndex, 400, 1, 2.5, BREAK_METAL);
	}
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CNPC_PersonalityCore::InputClearParent(inputdata_t& inputdata)
{
	BaseClass::InputClearParent(inputdata);
	RemoveSolidFlags(FSOLID_NOT_SOLID);
}

int CNPC_PersonalityCore::OnTakeDamage_Alive(const CTakeDamageInfo& inputInfo)
{
	CTakeDamageInfo info = inputInfo;

	if (inputInfo.GetDamageType() & DMG_BURN)
	{
		Speak("TLK_BURNED");
	}
	return 1;
}

bool CNPC_PersonalityCore::ShouldPlayIdleSound(void)
{
	if ((m_NPCState == NPC_STATE_IDLE || m_NPCState == NPC_STATE_ALERT) && gpGlobals->curtime > m_flNextIdleSoundTime && !HasSpawnFlags(SF_NPC_GAG))
	{
		return true;
	}

	return false;
}

void CNPC_PersonalityCore::IdleSound()
{
	m_flNextIdleSoundTime = gpGlobals->curtime + 0.1;
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
	Speak("TLK_FIZZLED");
	BaseClass::OnFizzled();
}

bool CNPC_PersonalityCore::CreateVPhysics()
{
	VPhysicsDestroyObject();
	RemoveSolidFlags(FSOLID_NOT_SOLID);
	SetSolid(SOLID_BBOX);
	IPhysicsObject* pPhysicsObject = VPhysicsInitNormal(SOLID_VPHYSICS, GetSolidFlags(), false);
	if (!pPhysicsObject)
		return false;

	pPhysicsObject->SetMass(75.0f);
	SetMoveType(MOVETYPE_VPHYSICS);
	return true;
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
		SetParent(NULL);
		CreateVPhysics();
		pPlayer->PickupObject(this, true);
	}
}

void CNPC_PersonalityCore::VPhysicsCollision(int index, gamevcollisionevent_t* pEvent)
{
	BaseClass::VPhysicsCollision(index, pEvent);

	if ((gpGlobals->curtime - m_flLastPhysicsImpactTime) >= 0.1f)
	{
		if (pEvent->pEntities[!index]->IsWorld())
		{
			if (&CNPC_PersonalityCore::IsBeingHeldByPlayer)
			{
				Speak("TLK_HELD_PHYSICS_IMPACT");
				m_flLastPhysicsImpactTime = gpGlobals->curtime;
			}
			Vector vecVelocity = pEvent->preVelocity[index];
			if ((vecVelocity.y * vecVelocity.y) + (vecVelocity.x * vecVelocity.x) + (vecVelocity.z * vecVelocity.z) <= 10000.0f)
				return;
		}
		else if (pEvent->pEntities[!index]->IsPlayer())
		{
			return;
		}
		Speak("TLK_PHYSICS_IMPACT");
	}
}

QAngle CNPC_PersonalityCore::PreferredCarryAngles()
{
	return QAngle(sv_personality_core_pca_pitch.GetFloat(), sv_personality_core_pca_yaw.GetFloat(), sv_personality_core_pca_roll.GetFloat());
}

void CNPC_PersonalityCore::ModifyOrAppendCriteria(AI_CriteriaSet& set)
{
	const char* coreheldvalue;
	const char* pickupvalue;

	BaseClass::ModifyOrAppendCriteria(set);

	bool heldbyplayer = IsBeingHeldByPlayer() == false;
	coreheldvalue = "true";

	if (heldbyplayer)
		coreheldvalue = "false";

	set.AppendCriteria("core_held", coreheldvalue);

	pickupvalue = "true";
	if (!m_bHasBeenPickedUp)
		pickupvalue = "false";

	set.AppendCriteria("core_picked_up", pickupvalue);

	set.AppendCriteria("player_speed", GetPlayerSpeed());
}

CAI_Expresser* CNPC_PersonalityCore::CreateExpresser()
{
	return BaseClass::CreateExpresser();
}

void CNPC_PersonalityCore::NotifySystemEvent(CBaseEntity* pNotify, notify_system_event_t eventType, const notify_system_event_params_t& params)
{
	if (eventType == NOTIFY_EVENT_TELEPORT)
	{
		Speak("TLK_PORTALED");
	}
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
	info->m_nSequence = ISequence;

	if (ISequence < 0)
		return false;

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
		return true;
	}
	return result;
}

void CNPC_PersonalityCore::PrescheduleThink()
{
	float flAnimResetTime = this->m_flAnimResetTime;
	if (gpGlobals->curtime > flAnimResetTime && flAnimResetTime != 0.0f)
	{
		int iIdleOverrideSequence = this->m_iIdleOverrideSequence;
		if (iIdleOverrideSequence <= -1)
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
			SetIdealSequence(iIdleOverrideSequence);
		}
		m_flCycle = 0.0f;
		m_flAnimResetTime = 0.0f;
	}
	BaseClass::PrescheduleThink();
}

bool CNPC_PersonalityCore::TestRemarkingUpon(CInfoRemarkable* pRemarkable)
{
	return IsLineOfSightClear( pRemarkable, IGNORE_ACTORS );
}

bool CNPC_PersonalityCore::IsBeingHeldByPlayer()
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer)
		{
			if ( GetPlayerHeldEntity(pPlayer) == this )
				break;
		}
	}
	return true;
}

float CNPC_PersonalityCore::GetPlayerSpeed()
{
	CBasePlayer* pPlayer = GetFirstPlayer();

	if (pPlayer)
	{
		return sqrt(pPlayer->GetAbsVelocity().Length());
	}
	return 0.0f;
}

CBasePlayer* CNPC_PersonalityCore::GetFirstPlayer()
{
	CBasePlayer* result;
	int v2;

	result = 0;
	if (gpGlobals->maxClients > 0)
	{
		v2 = 1;
		do
		{
			result = UTIL_PlayerByIndex(v2);
			if (result)
				break;
			++v2;
		}
		while (gpGlobals->maxClients >= v2);
	}
	return result;
}

const char* CNPC_PersonalityCore::GetPlayerHeldEntityName()
{
	CBasePlayer* pPlayer = GetFirstPlayer();

	if (pPlayer)
	{
		CBaseEntity* pHeld = GetPlayerHeldEntity(pPlayer);

		if (pHeld)
		{
			pDefaultHeldItemName = pHeld->GetClassname();
		}
	}

	return pDefaultHeldItemName;
}

