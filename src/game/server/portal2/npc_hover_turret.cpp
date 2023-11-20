#include "cbase.h"
#include "ai_basenpc_physicsflyer.h"
#include "player_pickup.h"
#include "smoke_trail.h"
#include "tier0/memdbgon.h"

ConVar sk_hover_turret_health("sk_hover_turret_health", "150", FCVAR_NONE);

int ACT_HOVER_TURRET_SEARCH;
int ACT_HOVER_TURRET_ALERT;
int ACT_HOVER_TURRET_ANGRY;
int ACT_HOVER_TURRET_DISABLED;

class CNPC_HoverTurret : public CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS(CNPC_HoverTurret, CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>);
	//DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr) { return BaseClass::TraceAttack(info, vecDir, ptr); }
	void CreateSmokeTrail();
	void Precache();
	void Spawn();
	void StartDeathSequence();
private:
	//CNetworkVar(int, m_sLaserHaloSprite);
	bool m_bCarriedByPlayer;
	Vector m_vForceVelocity;
	Vector m_vForceMoveTarget;
	Vector m_vTargetBanking;
	float m_flDeathTime;
	CHandle<SmokeTrail> m_hSmokeTrail;
	int m_iMuzzleAttachment;
	QAngle m_vecGoalAngles;
	QAngle m_vInitialLookAngles;
};

LINK_ENTITY_TO_CLASS(npc_hover_turret, CNPC_HoverTurret);

BEGIN_DATADESC(CNPC_HoverTurret)

END_DATADESC()

//IMPLEMENT_SERVERCLASS_ST(CNPC_HoverTurret, DT_NPC_HoverTurret)

//SendPropInt(SENDINFO(m_sLaserHaloSprite)),

//END_SEND_TABLE()

void CNPC_HoverTurret::CreateSmokeTrail()
{
	if (m_hSmokeTrail)
		return;

	// Smoke trail.
	if ((m_hSmokeTrail = SmokeTrail::CreateSmokeTrail()) != NULL)
	{
		m_hSmokeTrail->m_SpawnRate = 64.0f;
		m_hSmokeTrail->m_ParticleLifetime = 0.5f;
		m_hSmokeTrail->m_StartSize = 8;
		m_hSmokeTrail->m_EndSize = 16;
		m_hSmokeTrail->m_SpawnRadius = 16;
		m_hSmokeTrail->m_MinSpeed = 60;
		m_hSmokeTrail->m_MaxSpeed = 100;
		m_hSmokeTrail->m_StartColor.Init(0.5f, 0.5f, 0.5f);
		m_hSmokeTrail->m_EndColor.Init(0.0, 0.0, 0.0);

		m_hSmokeTrail->SetLifetime(-1.0f);
		m_hSmokeTrail->FollowEntity(this, "0");
	}
}

void CNPC_HoverTurret::Precache()
{
	PrecacheModel("models/props_junk/rock001a.mdl");
	PrecacheModel("effects/redlaser1.vmt");
	PrecacheModel("models/props/futbol_Gib01.mdl");
	PrecacheModel("models/props/futbol_Gib02.mdl");
	PrecacheModel("models/props/futbol_Gib03.mdl");
	PrecacheModel("models/props/futbol_Gib04.mdl");
	PrecacheModel("models/npcs/hover_turret.mdl");
	ADD_CUSTOM_ACTIVITY(CNPC_HoverTurret, ACT_HOVER_TURRET_SEARCH);
	ADD_CUSTOM_ACTIVITY(CNPC_HoverTurret, ACT_HOVER_TURRET_ALERT);
	ADD_CUSTOM_ACTIVITY(CNPC_HoverTurret, ACT_HOVER_TURRET_ANGRY);
	ADD_CUSTOM_ACTIVITY(CNPC_HoverTurret, ACT_HOVER_TURRET_DISABLED);
	PrecacheScriptSound("NPC_RocketTurret.LockingBeep");
	PrecacheScriptSound("NPC_FloorTurret.LockedBeep");
	PrecacheScriptSound("HL2Player.BurnPain");
	PrecacheScriptSound("NPC_HoverTurret.Snap");
	UTIL_PrecacheDecal("decals/scorchfade");
	PrecacheModel("models/props_bts/rocket_sentry.mdl");
	PrecacheModel("effects/bluelaser1.vmt");
	//m_sLaserHaloSprite = PrecacheModel("sprites/redlaserglow.vmt");
	UTIL_PrecacheOther("prop_glass_futbol");
	BaseClass::Precache();
}

void CNPC_HoverTurret::Spawn()
{
	Precache();

	SetModel("models/npcs/hover_turret.mdl");
	SetHullType(HULL_TINY_CENTERED);

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);

	SetMoveType(MOVETYPE_VPHYSICS);

	m_iHealth = sk_hover_turret_health.GetFloat();
	SetNavType(NAV_FLY);
	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL);
	AddEffects(EF_NOSHADOW);
	SetBloodColor(DONT_BLEED);
	SetCurrentVelocity(vec3_origin);
	m_vForceVelocity.Init();
	m_vCurrentBanking.Init();
	m_vTargetBanking.Init();
	CapabilitiesAdd(bits_CAP_INNATE_MELEE_ATTACK1 | bits_CAP_MOVE_FLY | bits_CAP_SQUAD);
	Vector bobAmount;
	bobAmount.x = random->RandomFloat(-1.0f, 1.0f);
	bobAmount.y = random->RandomFloat(-1.0f, 1.0f);
	bobAmount.z = random->RandomFloat(-1.0f, 1.0f);
	SetNoiseMod(bobAmount);
	m_fHeadYaw = 0.0f;
	NPCInit();
	SetViewOffset(bobAmount = Vector(0,0,10));

	m_flFieldOfView = 0.4f;
	SetCollisionGroup(COLLISION_GROUP_NONE);
	int iMuzzleAttachment = LookupAttachment("eyes");
	m_iMuzzleAttachment = iMuzzleAttachment;
	m_vecGoalAngles = GetAbsAngles();
	m_vInitialLookAngles = GetAbsAngles();
	m_bCarriedByPlayer = false;
	m_flDeathTime = 0.0f;
}

void CNPC_HoverTurret::StartDeathSequence()
{
	if (m_flDeathTime == 0.0)
	{
		m_flDeathTime = gpGlobals->curtime + 2.5f;
		EmitSound("NPC_RocketTurret.LockingBeep");
		EmitSound("NPC_RocketTurret.LockingBeep", gpGlobals->curtime + 0.5f);
		EmitSound("NPC_RocketTurret.LockingBeep", gpGlobals->curtime + 1.0f);
		EmitSound("NPC_RocketTurret.LockingBeep", gpGlobals->curtime + 1.5f);
		EmitSound("NPC_FloorTurret.LockedBeep", gpGlobals->curtime + 2.0f);
	}
}
