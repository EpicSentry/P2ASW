#include "cbase.h"
#include "ai_basenpc_physicsflyer.h"
#include "player_pickup.h"
#include "smoke_trail.h"
#include "IEffects.h"
#include "Sprite.h"
#include "ai_memory.h"
#include "ai_senses.h"
#include "collisionproperty.h"
#include "portal_base2d.h"
#include "props.h"
#include "explode.h"
#include "world.h"
#include "tier0/memdbgon.h"

#define HOVER_TURRET_GLOW_SPRITE	"sprites/light_glow03.vmt"

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

Activity ACT_HOVER_TURRET_SEARCH;
Activity ACT_HOVER_TURRET_ALERT;
Activity ACT_HOVER_TURRET_ANGRY;
Activity ACT_HOVER_TURRET_DISABLED;

class CNPC_HoverTurret : public CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>, public CDefaultPlayerPickupVPhysics
{
public:
	//DECLARE_SERVERCLASS();
	DECLARE_CLASS(CNPC_HoverTurret, CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>);

	Class_T Classify(void) { return CLASS_COMBINE; }
	void Event_Killed(const CTakeDamageInfo& info);
	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr) { return BaseClass::TraceAttack(info, vecDir, ptr); }
	void ShootLaser(Vector& vecSrc, Vector& vecDirToEnemy);
	void TalkThink();
	void FindTargetThink();
	void AimThink();
	void SetFiringState(hoverTurretAttackState_e state);
	void UpdateOnRemove();
	bool OverrideMove(float flInterval);
	void MoveToTarget(float flInterval, const Vector& vMoveTarget);
	void MoveExecute_Alive(float flInterval);
	void MoveExecute_Dead(float flInterval);

	Vector BodyTarget(const Vector& posSrc, bool bNoisy);

	float GetHeadTurnRate() { return 45.0f; }

	void TurnHeadToTarget(float flInterval, const Vector& MoveTarget);

	bool UpdateFacing();

	void UpdateMuzzleMatrix();
	Vector GetClosestVisibleEnemyPosition();
	void CreateSmokeTrail();
	void DestroySmokeTrail();
	void Precache();
	void RunTask(const Task_t* pTask);
	void Spawn();
	void StartTask(const Task_t* pTask);
	void GatherConditions() { return BaseClass::GatherConditions(); }
	void PrescheduleThink();
	void Explode();
	void StartDeathSequence();
	void VPhysicsCollision(int index, gamevcollisionevent_t* pEvent);
	void ClampMotorForces(Vector& linear, AngularImpulse& angular);
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunPickup_t reason);
	bool HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer);
	QAngle PreferredCarryAngles();
	int ObjectCaps() { return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE; }
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	CBasePlayer* HasPhysicsAttacker(float dt) { return 0; }
	float GetMaxEnginePower() { return 1.0f; }
	bool IsMovementDisabled();

	DEFINE_CUSTOM_AI;
	DECLARE_DATADESC();
private:
	void TakeDamageFromPhysicsImpact(int index, gamevcollisionevent_t* pEvent);
	bool OnBurning();
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

void CNPC_HoverTurret::ShootLaser(Vector& vecSrc, Vector& vecDirToEnemy)
{
	trace_t tr;
	Vector dir;
	UTIL_TraceLine(vecSrc + (vecDirToEnemy * 8192.0), dir = vecSrc + (vecDirToEnemy * 8192.0), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction != 1.0f)
	{
		if (tr.m_pEnt)
		{
			ClearMultiDamage();
			dir = tr.endpos - GetAbsOrigin();
			VectorNormalize(dir);

			float flDamage = (gpGlobals->curtime - GetLastThink() * 150.0f);
			CTakeDamageInfo info(this, this, flDamage, DMG_ENERGYBEAM);
		
			float flScale = 1.0;

			if (m_bCanPushPlayer)
			{
				flScale = 5.0f;
			}
			else
			{
				flScale = 0.0f;
			}

			UTIL_DecalTrace(&tr, "RedGlowFade");
		
			CalculateMeleeDamageForce(&info, dir, tr.endpos, flScale);
			tr.m_pEnt->DispatchTraceAttack(info, dir, &tr);
			ApplyMultiDamage();

			if (tr.m_pEnt->IsPlayer())
			{
				EmitSound("HL2Player.BurnPain");
			}
		}
		g_pEffects->Sparks(tr.endpos);
	}
}

void CNPC_HoverTurret::TalkThink()
{
	if (IsDissolving() || IsOnFire())
	{
		m_iDesiredState = HOVER_TURRET_DESTRUCTING;
	}

	if (m_iLastState != m_iDesiredState && (m_iDesiredState == HOVER_TURRET_TIPPED || m_iDesiredState == HOVER_TURRET_DESTRUCTING || m_iDesiredState == HOVER_TURRET_PICKUP))
		m_fNextTalk = gpGlobals->curtime - 1.0f;

	if (gpGlobals->curtime > m_fNextTalk && m_iLastState != m_iDesiredState)
	{
		const char* soundname = g_HoverTurretTalkNames[m_iDesiredState];

		switch (m_iDesiredState)
		{
			case HOVER_TURRET_TARGETING:
				EmitSound(soundname);
				m_fNextTalk = gpGlobals->curtime + 1.75f;
				break;

			case HOVER_TURRET_SEARCHING:
				EmitSound(soundname);
				m_fNextTalk = gpGlobals->curtime + 2.75f;
				break;
				
			case HOVER_TURRET_TIPPED:
				EmitSound(soundname);
				m_fNextTalk = gpGlobals->curtime + 1.15f;
				break;
				
			case HOVER_TURRET_PICKUP:
				EmitSound(soundname);
				m_fNextTalk = gpGlobals->curtime + 5.0f;
				break;
				
			case HOVER_TURRET_DESTRUCTING:
				EmitSound(soundname);
				m_fNextTalk = gpGlobals->curtime + 10.0f;
				break;

			default:
				return;
		}
	}
}

void CNPC_HoverTurret::FindTargetThink()
{
	if (m_flEngineStallTime <= gpGlobals->curtime && m_iFiringState != HOVER_TURRET_SHOT_DISABLED)
	{
		GetEnemies()->RefreshMemories();

		GetSenses()->Look( 8192 );
		SetEnemy(BestEnemy());

		if (BaseClass::HasCondition(COND_SEE_ENEMY))
		{
			if (GetEnemy() && IsAlive())
			{
				bool bEnemyInFOV = FInViewCone(GetEnemy());
				bool bEnemyVisible = FVisible(GetEnemy());
				bool v6 = false;

				CPortal_Base2D* pPortal = FInViewConeThroughPortal(GetEnemy());
			
				if (pPortal && FVisibleThroughPortal(pPortal, GetEnemy()))
					v6 = true;

				if (bEnemyInFOV && bEnemyVisible || v6)
				{
					UpdateFacing();
					return;
				}
			}
		}
		else
		{
			m_vecGoalAngles = GetAbsAngles();
			m_flFieldOfView = 0.4f;
		}
		UpdateFacing();
	}
}

void CNPC_HoverTurret::AimThink()
{
	if (((!BaseClass::HasCondition(COND_SEE_ENEMY) || m_flEngineStallTime > gpGlobals->curtime)) || (m_iFiringState == HOVER_TURRET_SHOT_DISABLED))
	{
		m_vecGoalAngles = GetAbsAngles();
		UpdateFacing();
		if (m_bAimingAtTarget)
		{
			return;
		}
	}
	else
	{
		if (!GetEnemy())
		{
			m_vecTargetPos = GetClosestVisibleEnemyPosition();
		}

		UpdateFacing();
		if (GetActivity() != ACT_HOVER_TURRET_ALERT)
		{
			SetActivity(ACT_HOVER_TURRET_ALERT);
		}

		if (gpGlobals->curtime > (m_flAimStartTime + 1.5f))
		{
			if (GetEnemy()->IsAlive())
			{
				FInViewCone(GetEnemy());
				FVisible(GetEnemy());

				CPortal_Base2D* pPortal = NULL;
				pPortal = FInViewConeThroughPortal(GetEnemy());

				Vector vecMuzzle, vecMuzzleDir;

				UpdateMuzzleMatrix();
				MatrixGetColumn(m_muzzleToWorld, 3, vecMuzzle);
				MatrixGetColumn(m_muzzleToWorld, 0, vecMuzzleDir);

				ShootLaser(vecMuzzle, vecMuzzleDir);
			}
		}
	}
}

void CNPC_HoverTurret::SetFiringState(hoverTurretAttackState_e state)
{
	if (!IsAlive() && state != HOVER_TURRET_SHOT_DISABLED)
	{
		return;
	}

	if (!m_hEyeGlow)
	{
		m_hEyeGlow = CSprite::SpriteCreate(HOVER_TURRET_GLOW_SPRITE, GetLocalOrigin(), false);
		if (!m_hEyeGlow)
			return;

		m_hEyeGlow->SetTransparency(kRenderWorldGlow, 255, 0, 0, 128, kRenderFxNoDissipation);
		m_hEyeGlow->SetAttachment(this, 1);
	}

	if (state == HOVER_TURRET_AIM_TARGET)
	{
		m_iDesiredState = HOVER_TURRET_TARGETING;
		m_bAimingAtTarget = true;
		m_vecTargetPos = GetClosestVisibleEnemyPosition();
		m_iFiringState = HOVER_TURRET_AIM_TARGET;
		m_flAimStartTime = gpGlobals->curtime;
		SetActivity(ACT_HOVER_TURRET_ALERT);
		m_nSkin = 1;
		m_hEyeGlow->SetBrightness(196,0.1f);
	}

	if (state == HOVER_TURRET_SHOT_DISABLED)
	{
		m_iFiringState = HOVER_TURRET_SHOT_DISABLED;
		m_iDesiredState = HOVER_TURRET_PICKUP;
		SetActivity(ACT_HOVER_TURRET_ANGRY);
		m_nSkin = 2;
		m_hEyeGlow->SetScale(0.1f,3.0f);
		m_hEyeGlow->SetBrightness(0,3.0f);
	}

	if (state == HOVER_TURRET_HIT_WITH_PHYSICS)
	{
		m_iFiringState = HOVER_TURRET_FIND_TARGET;
		m_iDesiredState = HOVER_TURRET_DESTRUCTING;
		SetActivity(ACT_HOVER_TURRET_ANGRY);
		m_nSkin = 2;
	}
}

void CNPC_HoverTurret::UpdateOnRemove()
{
	TalkThink();
	BaseClass::UpdateOnRemove();
}

bool CNPC_HoverTurret::OverrideMove(float flInterval)
{
	if (m_iHealth > 0)
	{
		if (!m_bInitialPositionSet)
		{
			m_vForceMoveTarget = GetAbsOrigin();
			m_vForceMoveTarget.z = m_vForceMoveTarget.z + 52.0f;
			m_bInitialPositionSet = true;
		}

		if (m_pPhysicsObject)
			m_pPhysicsObject->Wake();

		if (m_flEngineStallTime > gpGlobals->curtime || m_iFiringState == HOVER_TURRET_SHOT_DISABLED)
		{
			return false;
		}
		else
		{
			if (m_flEngineStallTime <= gpGlobals->curtime && !m_pMotionController)
			{
				m_pMotionController = physenv->CreateMotionController((IMotionEvent*)this);
				m_pMotionController->AttachObject(m_pPhysicsObject, true);
			}
			MoveToTarget(flInterval, m_vForceMoveTarget);
			MoveExecute_Alive(flInterval);
			return true;
		}
	}
	else
	{
		MoveExecute_Dead(flInterval);
		return true;
	}
}

void CNPC_HoverTurret::MoveToTarget(float flInterval, const Vector& vMoveTarget)
{
	if (flInterval > 0.0f && m_flEngineStallTime <= gpGlobals->curtime && m_iFiringState != HOVER_TURRET_SHOT_DISABLED)
	{
		if ( GetEnemy() && BaseClass::HasCondition(COND_SEE_ENEMY) )
		{
			TurnHeadToTarget(flInterval, GetClosestVisibleEnemyPosition());
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

		float newYaw = AI_ClampYaw(yawSpeedPerSec, m_fHeadYaw, (yawSpeedPerSec * flInterval) + m_fHeadYaw, gpGlobals->curtime - GetLastThink());

		if (newYaw != m_fHeadYaw)
		{
			m_fHeadYaw = newYaw;
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

Vector CNPC_HoverTurret::GetClosestVisibleEnemyPosition()
{
	CBaseEntity* pEnemy = GetEnemy();

	UpdateMuzzleMatrix();

	Vector vecMuzzlePos, vecMidEnemyTransformed;
	MatrixGetColumn(m_muzzleToWorld, 3, vecMuzzlePos);

	Vector vecMidEnemy = pEnemy->BodyTarget(vecMidEnemyTransformed, &vecMuzzlePos);

	//Calculate dir and dist to enemy
	Vector	vecDirToEnemyTransformed = vecMidEnemy * 0.65;
	float	flDistToEnemyTransformed = VectorNormalize(vecDirToEnemyTransformed);

	Vector vecCenter;
	CollisionProp()->CollisionToWorldSpace(Vector(0, 0, 52), &vecCenter);

	Vector vecMid = EyePosition();
	pEnemy->BodyTarget(vecMid);

	//Look for our current enemy
	bool bEnemyInFOV = FInViewCone(pEnemy);
	bool bEnemyVisible = FVisible(pEnemy);

	Vector	vecDirToEnemy = vecMidEnemy - vecMid;
	float	flDistToEnemy = VectorNormalize(vecDirToEnemy);

	CPortal_Base2D* pPortal = NULL;

	if (pEnemy->IsAlive())
	{
		pPortal = FInViewConeThroughPortal(pEnemy);

		if (pPortal && FVisibleThroughPortal(pPortal, pEnemy))
		{
			UTIL_Portal_PointTransform(pPortal->m_hLinkedPortal->MatrixThisToLinked(), vecMidEnemy, vecDirToEnemyTransformed);
			vecDirToEnemyTransformed = vecMidEnemyTransformed - vecMuzzlePos;
			VectorNormalize(vecDirToEnemyTransformed);
			if (!bEnemyInFOV || !bEnemyVisible || flDistToEnemy > flDistToEnemyTransformed)
			{
				vecDirToEnemy = vecDirToEnemyTransformed;
			}
		}
	}

	VectorNormalize(vecDirToEnemy);
	QAngle vecAnglesToEnemy;
	VectorAngles(vecDirToEnemy, vecAnglesToEnemy);
	m_vecGoalAngles = vecAnglesToEnemy;
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

void CNPC_HoverTurret::PrescheduleThink()
{
	if (IsOnFire())
	{
		OnBurning();
	}
	else if (m_iHealth < 0)
	{
		if (m_flDeathTime != 0.0f && gpGlobals->curtime > m_flDeathTime)
			Explode();
	}
	else
	{
		switch (m_iFiringState)
		{
		case HOVER_TURRET_AIM_TARGET:
			AimThink();
			break;
		case HOVER_TURRET_SHOT_DISABLED:
			return;
		case HOVER_TURRET_FIND_TARGET:
			FindTargetThink();
			break;
		}
	}
	TalkThink();
	BaseClass::PrescheduleThink();
}

void CNPC_HoverTurret::Explode()
{
	Vector vecVelocity;

	CBreakableProp* pEntity = (CBreakableProp*)CBaseEntity::Create("prop_glass_futbol", GetAbsOrigin(), GetAbsAngles());
	if (pEntity)
	{
		GetVelocity(&vecVelocity, NULL);
		pEntity->SetAbsVelocity(vecVelocity);
		CTakeDamageInfo info(this, this, 200.0f, DMG_BLAST);
		pEntity->Break(this, info);
	}

	string_t strTurretName = AllocPooledString("npc_hover_turret");
	string_t strPropName = AllocPooledString("prop_weighted_cube");

	Vector maxs;

	maxs.x = GetAbsOrigin().x + 128.0f;
	maxs.y = GetAbsOrigin().y + 128.0f;
	maxs.z = GetAbsOrigin().z + 128.0f;

	vecVelocity.x = GetAbsOrigin().x - 128.0f;
	vecVelocity.y = GetAbsOrigin().y - 128.0f;
	vecVelocity.z = GetAbsOrigin().z - 128.0f;

	CBaseEntity* pEnts[128];
	int nNumFound = UTIL_EntitiesInBox(pEnts, 128, vecVelocity, maxs, 0);

	for (int i = 0; i < nNumFound; i++)
	{
		if (pEnts[i] == NULL)
			continue;

		if ( ClassMatches(strTurretName) || ClassMatches(strPropName) || IsPlayer() )
		{
			trace_t tr;
			UTIL_TraceLine(GetAbsOrigin(), pEnts[i]->WorldSpaceCenter(), (MASK_WATER | MASK_SOLID_BRUSHONLY), this, COLLISION_GROUP_NONE, &tr);

			if (tr.fraction >= 1.0f)
			{
				CTakeDamageInfo info(this, this, 50.0f, DMG_ENERGYBEAM);
				info.GetDamagePosition() = GetAbsOrigin();

				vecVelocity = GetAbsOrigin() - GetAbsOrigin();
				CalculateExplosiveDamageForce(&info, vecVelocity, GetAbsOrigin());
				pEnts[i]->DispatchTraceAttack(info, vec3_origin, &tr);
				ApplyMultiDamage();
			}
		}
	}

	ExplosionCreate(WorldSpaceCenter(), vec3_angle, this, 200, 100, (SF_ENVEXPLOSION_NODAMAGE | SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS | SF_ENVEXPLOSION_NOSMOKE | SF_ENVEXPLOSION_NOFIREBALLSMOKE));
	UTIL_ScreenShake(WorldSpaceCenter(), 5.0f, 150.0f, 1.0f, 750.0f, SHAKE_START);
	
	Vector origin;
	CPVSFilter filter(origin);

	Vector gibVelocity = RandomVector(-30.0, 30.0);
	int iModelIndex = modelinfo->GetModelIndex(g_PropDataSystem.GetRandomChunkModel("MetalChunks"));

	for (int i = 0; i < 16; i++)
	{
		vecVelocity.x = 16.0f;
		vecVelocity.y = 16.0f;
		vecVelocity.z = 16.0f;
		te->BreakModel(filter, 0.0, GetAbsOrigin(), GetAbsAngles(), Vector(40, 40, 40), gibVelocity, iModelIndex, 400, 1, 2.5, BREAK_METAL);
	}

	AddEffects(EF_NODRAW);
	SetThink(SUB_Remove);
	SetNextThink(gpGlobals->curtime + 0.1f);
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

void CNPC_HoverTurret::VPhysicsCollision(int index, gamevcollisionevent_t* pEvent)
{
	if (m_iHealth > 0)
	{
		BaseClass::VPhysicsCollision(index, pEvent);
		m_flFieldOfView = -1.0f;

		CWorld* pHitEntity = (CWorld *)pEvent->pEntities[!index];

		if (pHitEntity->IsPlayer())
		{
			if (m_iFiringState != HOVER_TURRET_AIM_TARGET)
			{
				SetFiringState(HOVER_TURRET_FIND_TARGET);
				m_flEngineStallTime = gpGlobals->curtime + 0.5f;
			}
		}
		else
		{
			if (pHitEntity != GetWorldEntity())
			{
				bool bDisabled = m_iFiringState == HOVER_TURRET_SHOT_DISABLED;
				m_flEngineStallTime = gpGlobals->curtime + 4.0f;

				if (!bDisabled)
					SetFiringState(HOVER_TURRET_HIT_WITH_PHYSICS);
			}
			TakeDamageFromPhysicsImpact(index, pEvent);
		}
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

void CNPC_HoverTurret::OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunPickup_t reason)
{
	if (IsAlive())
	{
		if (m_pMotionController)
		{
			physenv->DestroyMotionController(m_pMotionController);
			m_pMotionController = NULL;
		}
		m_bCarriedByPlayer = true;
		SetFiringState(HOVER_TURRET_SHOT_DISABLED);
	}
}

bool CNPC_HoverTurret::HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer)
{
	if (IsAlive())
		return m_bUseCarryAngles;

	return false;
}

QAngle CNPC_HoverTurret::PreferredCarryAngles()
{
	static QAngle g_prefAngles;

	Vector vecUserForward;
	CBasePlayer* pPlayer = AI_GetSinglePlayer();
	pPlayer->EyeVectors(&vecUserForward);

	// If we're looking up, then face directly forward
	if (vecUserForward.z >= 0.0f)
		return vec3_angle;

	// Otherwise, stay "upright"
	g_prefAngles.Init();
	g_prefAngles.x = -pPlayer->EyeAngles().x;

	return g_prefAngles;
}

void CNPC_HoverTurret::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	if ( pPlayer )
	{
		pPlayer->PickupObject( this, false );
	}
}

bool CNPC_HoverTurret::IsMovementDisabled()
{
	if (m_flEngineStallTime <= gpGlobals->curtime)
		return m_iFiringState == HOVER_TURRET_SHOT_DISABLED;

	return true;
}

void CNPC_HoverTurret::TakeDamageFromPhysicsImpact(int index, gamevcollisionevent_t* pEvent)
{
	int damageType;
	float damage = CalculateDefaultPhysicsDamage(index, pEvent, 20.0f * m_impactEnergyScale, true, damageType);

	if (pEvent->pEntities[index == 0] && ClassMatches("prop_energy_ball"))
	{
		damage = 100.0f;
	}
	else if (damage == 0.0f)
	{
		return;
	}

	Vector damagePos;
	pEvent->pInternalData->GetContactPoint(damagePos);

	Vector damageForce = pEvent->postVelocity[index] * pEvent->pObjects[index]->GetMass();

	if (vec3_origin == damageForce)
	{
		damageForce = pEvent->postVelocity[index] * pEvent->pObjects[index]->GetMass();
	}

	if (damage > 50.0f)
	{

		CTakeDamageInfo info(pEvent->pEntities[index == 0], pEvent->pEntities[index == 0], damageForce, damagePos, damage, damageType);
		Event_Killed(info);
	}
}

bool CNPC_HoverTurret::OnBurning()
{
	if (m_iHealth <= 0)
	{
		Explode();
		return false;
	}
	return true;
}
