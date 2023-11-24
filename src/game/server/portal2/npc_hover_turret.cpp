#include "cbase.h"
#include "ai_basenpc_physicsflyer.h"
#include "player_pickup.h"
#include "smoke_trail.h"
#include "IEffects.h"
#include "Sprite.h"
#include "tier0/memdbgon.h"

extern ConVar sv_gravity;

enum hoverTurretState_e
{
	HOVER_TURRET_TARGETING = 0,
	HOVER_TURRET_SEARCHING = 1,
	HOVER_TURRET_TIPPED = 2,
	HOVER_TURRET_DISABLED = 3,
	HOVER_TURRET_COLLIDE = 4,
	HOVER_TURRET_PICKUP = 5,
	HOVER_TURRET_SHOTAT = 6,
	HOVER_TURRET_DESTRUCTING = 7,
	HOVER_TURRET_STATE_TOTAL = 8
};

const char* g_HoverTurretTalkNames[HOVER_TURRET_STATE_TOTAL] =
{
	"NPC_HoverTurret.TalkActive",
	"NPC_HoverTurret.TalkSearch",
	"NPC_FloorTurret.TalkTipped",
	"NPC_FloorTurret.TalkDisabled",
	"NPC_FloorTurret.TalkCollide",
	"NPC_HoverTurret.TalkPickup",
	"NPC_FloorTurret.TalkShotAt",
	"NPC_HoverTurret.TalkDissolved"
};

enum HoverTurretTasks
{
	TASK_HOVER_TURRET_HOVER = LAST_SHARED_TASK
};

enum hoverTurretAttackState_e
{
	HOVER_TURRET_FIND_TARGET = 0,
	HOVER_TURRET_AIM_TARGET = 1,
	HOVER_TURRET_SHOT_DISABLED = 2,
	HOVER_TURRET_HIT_WITH_PHYSICS = 3
};

ConVar sk_hover_turret_health("sk_hover_turret_health", "150", FCVAR_NONE);

int ACT_HOVER_TURRET_SEARCH;
int ACT_HOVER_TURRET_ALERT;
int ACT_HOVER_TURRET_ANGRY;
int ACT_HOVER_TURRET_DISABLED;

class CNPC_HoverTurret : public CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>, public CDefaultPlayerPickupVPhysics
{
public:
	//DECLARE_SERVERCLASS();
	DECLARE_CLASS(CNPC_HoverTurret, CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>);
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;

	Class_T Classify(void) { return CLASS_COMBINE; }
	void Event_Killed(const CTakeDamageInfo& info);
	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr) { return BaseClass::TraceAttack(info, vecDir, ptr); }
	void UpdateOnRemove();
	void MoveToTarget(float flInterval, const Vector& vMoveTarget);
	void MoveExecute_Alive(float flInterval);
	void MoveExecute_Dead(float flInterval);

	Vector BodyTarget(const Vector& posSrc, bool bNoisy);

	float GetHeadTurnRate() { return 45.0f; }

	void TurnHeadToTarget(float flInterval, const Vector& MoveTarget);

	bool UpdateFacing();

	void UpdateMuzzleMatrix();
	void CreateSmokeTrail();
	void DestroySmokeTrail();
	void Precache();
	void RunTask(const Task_t* pTask);
	void Spawn();
	void StartTask(const Task_t* pTask);
	void StartDeathSequence();
	void ClampMotorForces(Vector& linear, AngularImpulse& angular);
private:
	//CNetworkVar(int, m_sLaserHaloSprite);
	float m_fNextTalk;

	int m_iDesiredState;
	int m_iLastState;

	bool m_bAimingAtTarget;
	bool m_bCarriedByPlayer;
	bool m_bUseCarryAngles;

	Vector m_vecTargetPos;
	float m_flAimStartTime;
	Vector m_vForceVelocity;
	Vector m_vForceMoveTarget;
	Vector m_vTargetBanking;
	
	CHandle<CBaseEntity> m_hAttachTarget;

	string_t m_strAttachName;

	float m_fSparkTime;
	float m_flDeathTime;

	CHandle<SmokeTrail> m_hSmokeTrail;
	CHandle<CBaseEntity> m_hTether;

	float m_flSentryTurnSpeed;

	bool m_bCanPushPlayer;

	float m_flEngineStallTime;

	bool m_bInitialPositionSet;

	COutputEvent m_OnPhysGunPickup;
	COutputEvent m_OnPhysGunDrop;

	hoverTurretAttackState_e m_iFiringState;
	
	CHandle<CSprite> m_hEyeGlow;

	matrix3x4_t m_muzzleToWorld;

	int m_iMuzzleAttachment;
	int m_muzzleToWorldTick;

	QAngle m_vecGoalAngles;
	QAngle m_vInitialLookAngles;
};

LINK_ENTITY_TO_CLASS(npc_hover_turret, CNPC_HoverTurret);

BEGIN_DATADESC(CNPC_HoverTurret)
	DEFINE_FIELD(m_bAimingAtTarget, FIELD_BOOLEAN),
	DEFINE_FIELD(m_vecTargetPos, FIELD_VECTOR),
	DEFINE_FIELD(m_flAimStartTime, FIELD_FLOAT),
	DEFINE_FIELD(m_iFiringState, FIELD_INTEGER),
	DEFINE_FIELD(m_vForceVelocity, FIELD_VECTOR),
	DEFINE_FIELD(m_bInitialPositionSet, FIELD_BOOLEAN),
	DEFINE_FIELD(m_vecGoalAngles, FIELD_VECTOR),
	DEFINE_FIELD(m_vInitialLookAngles, FIELD_VECTOR),
	DEFINE_FIELD(m_vForceMoveTarget, FIELD_VECTOR),
	DEFINE_FIELD(m_vTargetBanking, FIELD_VECTOR),
	DEFINE_FIELD(m_flEngineStallTime, FIELD_TIME),
	DEFINE_FIELD(m_fNextTalk, FIELD_TIME),
	DEFINE_FIELD(m_iDesiredState, FIELD_INTEGER),
	DEFINE_FIELD(m_iLastState, FIELD_INTEGER),
	DEFINE_FIELD(m_bCarriedByPlayer, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bUseCarryAngles, FIELD_BOOLEAN),
	DEFINE_FIELD(m_iMuzzleAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_muzzleToWorldTick, FIELD_INTEGER),
	DEFINE_FIELD(m_hEyeGlow, FIELD_EHANDLE),
	DEFINE_FIELD(m_flDeathTime, FIELD_FLOAT),
	DEFINE_FIELD(m_fSparkTime, FIELD_FLOAT),
	DEFINE_FIELD(m_hSmokeTrail, FIELD_EHANDLE),
	DEFINE_FIELD(m_hTether, FIELD_EHANDLE),
	DEFINE_FIELD(m_hAttachTarget, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_strAttachName, FIELD_STRING, "attachTarget"),
	DEFINE_KEYFIELD(m_flSentryTurnSpeed, FIELD_FLOAT, "sentryRotateSpeed"),
	DEFINE_KEYFIELD(m_bCanPushPlayer, FIELD_BOOLEAN, "canPushPlayer"),
	DEFINE_OUTPUT(m_OnPhysGunPickup, "OnPhysGunPickup"),
	DEFINE_OUTPUT(m_OnPhysGunDrop, "OnPhysGunDrop"),
END_DATADESC()

AI_BEGIN_CUSTOM_NPC(npc_hover_turret, CNPC_HoverTurret)
	DECLARE_TASK(TASK_HOVER_TURRET_HOVER)
AI_END_CUSTOM_NPC()

//IMPLEMENT_SERVERCLASS_ST(CNPC_HoverTurret, DT_NPC_HoverTurret)

//SendPropInt(SENDINFO(m_sLaserHaloSprite)),

//END_SEND_TABLE()

void CNPC_HoverTurret::Event_Killed(const CTakeDamageInfo& info)
{
	if (m_hSmokeTrail)
	{
		CreateSmokeTrail();
		m_fSparkTime = gpGlobals->curtime + 0.1f;
	}

	if (m_pMotionController)
	{
		physenv->DestroyMotionController(m_pMotionController);
	}

	if (!m_bCarriedByPlayer)
		StartDeathSequence();
	
	m_iHealth = 0;

	m_OnDeath.FireOutput(this, this);
}

void CNPC_HoverTurret::UpdateOnRemove()
{
	BaseClass::UpdateOnRemove();
}

void CNPC_HoverTurret::MoveToTarget(float flInterval, const Vector& vMoveTarget)
{
	if (flInterval > 0.0f && m_flEngineStallTime <= gpGlobals->curtime && m_iFiringState != HOVER_TURRET_SHOT_DISABLED)
	{
		if ( GetEnemy() && BaseClass::HasCondition(COND_SEE_ENEMY) )
		{

		}

		float yawSpeedPerSec = m_flSentryTurnSpeed;
		if (yawSpeedPerSec == 0.0f)
		{
			Vector vecMoveDir, vecLookDir;

			vecMoveDir = vMoveTarget - GetLocalOrigin();
			VectorNormalize(vecMoveDir);

			AngleVectors(m_vInitialLookAngles,&vecLookDir);

			vecLookDir* (vecLookDir * 5.0) + vecMoveDir;
		}
	}
}

void CNPC_HoverTurret::MoveExecute_Alive(float flInterval)
{
	BaseClass::PhysicsCheckWaterTransition();

	if (m_pPhysicsObject)
	{
		if (m_hTether)
		{
			m_pPhysicsObject->Wake();
		}
	}

	m_vCurrentVelocity.x = m_vForceVelocity.x;
	m_vCurrentVelocity.y = m_vForceVelocity.y;
	m_vCurrentVelocity.z = m_vForceVelocity.z - (flInterval * 0.1) * sv_gravity.GetFloat();
	m_vForceVelocity = vec3_origin;
}

void CNPC_HoverTurret::MoveExecute_Dead(float flInterval)
{
	Vector forward, up;
	Vector newVelocity;
	
	GetVectors(&forward, NULL, &up);

	if (gpGlobals->curtime > m_fSparkTime)
	{
		g_pEffects->Sparks(GetAbsOrigin());
		m_fSparkTime = random->RandomFloat(0.05f,0.1f) + gpGlobals->curtime;
	}

	m_vCurrentVelocity = m_vForceVelocity + newVelocity;
	m_vForceVelocity = vec3_origin;
}

Vector CNPC_HoverTurret::BodyTarget(const Vector& posSrc, bool bNoisy)
{
	return WorldSpaceCenter();
}

void CNPC_HoverTurret::TurnHeadToTarget(float flInterval, const Vector& MoveTarget)
{
	Vector vecDir;
	vecDir = MoveTarget - GetLocalOrigin();

	float newYaw = BaseClass::VecToYaw(vecDir);

	float time = gpGlobals->curtime - GetLastThink();
	float yawSpeedPerSec = GetHeadTurnRate() * 10.0f;
	
	float clampYaw = AI_ClampYaw(yawSpeedPerSec, m_fHeadYaw, newYaw, time);
	
	if (clampYaw != m_fHeadYaw)
		m_fHeadYaw = clampYaw;
}

bool CNPC_HoverTurret::UpdateFacing()
{
	bool  bMoved = false;
	UpdateMuzzleMatrix();

	Vector vecGoalDir;
	AngleVectors(m_vecGoalAngles, &vecGoalDir);

	Vector vecGoalLocalDir;
	VectorIRotate(vecGoalDir, m_muzzleToWorld, vecGoalLocalDir);

	QAngle vecGoalLocalAngles;
	VectorAngles(vecGoalLocalDir, vecGoalLocalAngles);

	// Update pitch
	float flDiff = AngleNormalize(UTIL_ApproachAngle(vecGoalLocalAngles.x, 0.0, 0.05f * MaxYawSpeed()));

	SetPoseParameter(m_poseAim_Pitch, GetPoseParameter(m_poseAim_Pitch) + (flDiff / 2.5f));

	if (fabs(flDiff) > 0.1f)
	{
		bMoved = true;
	}

	// Update yaw
	flDiff = AngleNormalize(UTIL_ApproachAngle(vecGoalLocalAngles.y, 0.0, 0.05f * MaxYawSpeed()));

	SetPoseParameter(m_poseAim_Yaw, GetPoseParameter(m_poseAim_Yaw) + (flDiff / 2.5f));

	if (fabs(flDiff) > 0.1f)
	{
		bMoved = true;
	}

	// You're going to make decisions based on this info.  So bump the bone cache after you calculate everything
	InvalidateBoneCache();

	return bMoved;
}

void CNPC_HoverTurret::UpdateMuzzleMatrix()
{
	if (gpGlobals->tickcount != m_muzzleToWorldTick)
	{
		m_muzzleToWorldTick = gpGlobals->tickcount;
		GetAttachment(m_iMuzzleAttachment, m_muzzleToWorld);
	}
}

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

void CNPC_HoverTurret::DestroySmokeTrail()
{
	if (m_hSmokeTrail != NULL)
		UTIL_Remove(m_hSmokeTrail);
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

void CNPC_HoverTurret::RunTask(const Task_t* pTask)
{
	switch (pTask->iTask)
	{

	case TASK_HOVER_TURRET_HOVER:
		BaseClass::RunTask(pTask);

	}
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

void CNPC_HoverTurret::StartTask(const Task_t* pTask)
{
	switch (pTask->iTask)
	{

	case TASK_HOVER_TURRET_HOVER:
		BaseClass::StartTask(pTask);

	}
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

void CNPC_HoverTurret::ClampMotorForces(Vector& linear, AngularImpulse& angular)
{
	if (m_iHealth > 0)
	{
		linear.x = 0.0f;
		linear.y = 0.0f;
		linear.z = 0.0f;
		if (!m_bAimingAtTarget)
		{
			angular.x = angular.x * 0.0099999998f;
			angular.y = angular.y * 0.0099999998f;
			if (m_flSentryTurnSpeed == 0.0f)
			{
				linear.x = random->RandomFloat(-50.0f,60.0f);
				linear.y = random->RandomFloat(-50.0f,60.0f);
				linear.z = random->RandomFloat(-150.0f,150.0f);
			}
		}
	}
	else
	{
		linear.x = 0.0f;
		linear.y = 0.0f;
		linear.z = 0.0f;
		angular.x = 0.0f;
		angular.y = 0.0f;
		angular.z = 0.0f;
	}
}
