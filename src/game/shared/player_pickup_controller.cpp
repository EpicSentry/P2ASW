//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Physics cannon
//
//=============================================================================//

#include "cbase.h"
#include "prop_portal_shared.h"
#include "portal_util_shared.h"
#include "gamerules.h"
#include "soundenvelope.h"
#include "engine/IEngineSound.h"
#include "physics.h"
#include "in_buttons.h"
#include "IEffects.h"
#include "shake.h"
#include "beam_shared.h"
#include "sprite.h"
//#include "portal/weapon_physcannon.h"
#include "player_pickup_controller.h"
#include "physics_saverestore.h"
#include "player_pickup.h"
#include "movevars_shared.h"
#include "weapon_portalbasecombatweapon.h"
#include "vphysics/friction.h"
#include "saverestore_utlvector.h"
#include "hl2_gamerules.h"
#include "citadel_effects_shared.h"
#include "model_types.h"
#include "rumble_shared.h"
#include "gamestats.h"

#ifdef GAME_DLL
#include "player.h"
#include "weapon_portalgun.h"
#include "physics_prop_ragdoll.h"
#include "globalstate.h"
#include "props.h"
#include "ai_interactions.h"
#include "eventqueue.h"
#include "prop_combine_ball.h"
#include "physobj.h"
#include "te_effect_dispatch.h"
#include "ai_basenpc.h"
#include "portal_player.h"
#include "hl2_player.h"
#include "util.h"
#include "ndebugoverlay.h"
#include "soundent.h"
#else
#include "c_baseplayer.h"
#include "c_weapon_portalgun.h"
#include "c_props.h"
#include "c_physicsprop.h"
#include "c_prop_combine_ball.h"
#include "c_physbox.h"
#include "c_te_effect_dispatch.h"
#include "c_ai_basenpc.h"
#include "c_portal_player.h"
#include "c_basehlplayer.h"
#include "cdll_util.h"
#endif

#ifdef CLIENT_DLL
#define CPhysBox C_PhysBox
#define CPhysicsProp C_PhysicsProp
//#define CRagdollProp C_ServerRagdoll
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static const char *s_pWaitForUpgradeContext = "WaitForUpgrade";

ConVar	g_debug_physcannon("g_debug_physcannon", "0");

ConVar physcannon_minforce("physcannon_minforce", "700");
ConVar physcannon_maxforce("physcannon_maxforce", "1500");
ConVar physcannon_maxmass("physcannon_maxmass", "250");
ConVar physcannon_tracelength("physcannon_tracelength", "250");
ConVar physcannon_mega_tracelength("physcannon_mega_tracelength", "850");
ConVar physcannon_chargetime("physcannon_chargetime", "2");
ConVar physcannon_pullforce("physcannon_pullforce", "4000");
ConVar physcannon_mega_pullforce("physcannon_mega_pullforce", "8000");
ConVar physcannon_cone("physcannon_cone", "0.97");
ConVar physcannon_ball_cone("physcannon_ball_cone", "0.997");
ConVar physcannon_punt_cone("physcannon_punt_cone", "0.997");
ConVar player_throwforce("player_throwforce", "1000");
ConVar physcannon_dmg_glass("physcannon_dmg_glass", "15");

extern ConVar player_walkspeed;

// We want to test against brushes alone
class CTraceFilterOnlyBrushes : public CTraceFilterSimple
{
public:
	DECLARE_CLASS(CTraceFilterOnlyBrushes, CTraceFilterSimple);
	CTraceFilterOnlyBrushes(int collisionGroup) : CTraceFilterSimple(NULL, collisionGroup) {}
	virtual TraceType_t	GetTraceType() const { return TRACE_WORLD_ONLY; }
};

//-----------------------------------------------------------------------------
// this will hit skip the pass entity, but not anything it owns 
// (lets player grab own grenades)
class CTraceFilterNoOwnerTest : public CTraceFilterSimple
{
public:
	DECLARE_CLASS(CTraceFilterNoOwnerTest, CTraceFilterSimple);

	CTraceFilterNoOwnerTest(const IHandleEntity *passentity, int collisionGroup)
		: CTraceFilterSimple(NULL, collisionGroup), m_pPassNotOwner(passentity)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask)
	{
		if (pHandleEntity != m_pPassNotOwner)
			return BaseClass::ShouldHitEntity(pHandleEntity, contentsMask);

		return false;
	}

protected:
	const IHandleEntity *m_pPassNotOwner;
};

static void MatrixOrthogonalize(matrix3x4_t &matrix, int column)
{
	Vector columns[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		MatrixGetColumn(matrix, i, columns[i]);
	}

	int index0 = column;
	int index1 = (column + 1) % 3;
	int index2 = (column + 2) % 3;

	columns[index2] = CrossProduct(columns[index0], columns[index1]);
	columns[index1] = CrossProduct(columns[index2], columns[index0]);
	VectorNormalize(columns[index2]);
	VectorNormalize(columns[index1]);
	MatrixSetColumn(columns[index1], index1, matrix);
	MatrixSetColumn(columns[index2], index2, matrix);
}

#define SIGN(x) ( (x) < 0 ? -1 : 1 )

static QAngle AlignAngles(const QAngle &angles, float cosineAlignAngle)
{
	matrix3x4_t alignMatrix;
	AngleMatrix(angles, alignMatrix);

	// NOTE: Must align z first
	for (int j = 3; --j >= 0; )
	{
		Vector vec;
		MatrixGetColumn(alignMatrix, j, vec);
		for (int i = 0; i < 3; i++)
		{
			if (fabs(vec[i]) > cosineAlignAngle)
			{
				vec[i] = SIGN(vec[i]);
				vec[(i + 1) % 3] = 0;
				vec[(i + 2) % 3] = 0;
				MatrixSetColumn(vec, j, alignMatrix);
				MatrixOrthogonalize(alignMatrix, j);
				break;
			}
		}
	}

	QAngle out;
	MatrixAngles(alignMatrix, out);
	return out;
}


static void TraceCollideAgainstBBox(const CPhysCollide *pCollide, const Vector &start, const Vector &end, const QAngle &angles, const Vector &boxOrigin, const Vector &mins, const Vector &maxs, trace_t *ptr)
{
	physcollision->TraceBox(boxOrigin, boxOrigin + (start - end), mins, maxs, pCollide, start, angles, ptr);

	if (ptr->DidHit())
	{
		ptr->endpos = start * (1 - ptr->fraction) + end * ptr->fraction;
		ptr->startpos = start;
		ptr->plane.dist = -ptr->plane.dist;
		ptr->plane.normal *= -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Finds the nearest ragdoll sub-piece to a location and returns it
// Input  : *pTarget - entity that is the potential ragdoll
//			&position - position we're testing against
// Output : IPhysicsObject - sub-object (if any)
//-----------------------------------------------------------------------------
IPhysicsObject *GetRagdollChildAtPosition(CBaseEntity *pTarget, const Vector &position)
{
#ifdef GAME_DLL
	// Check for a ragdoll
	if (dynamic_cast<CRagdollProp*>(pTarget) == NULL)
		return NULL;

	// Get the root
	IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pTarget->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));

	IPhysicsObject *pBestChild = NULL;
	float			flBestDist = 99999999.0f;
	float			flDist;
	Vector			vPos;

	// Find the nearest child to where we're looking
	for (int i = 0; i < count; i++)
	{
		pList[i]->GetPosition(&vPos, NULL);

		flDist = (position - vPos).LengthSqr();

		if (flDist < flBestDist)
		{
			pBestChild = pList[i];
			flBestDist = flDist;
		}
	}

	// Make this our base now
	pTarget->VPhysicsSwapObject(pBestChild);

	return pTarget->VPhysicsGetObject();
#else
	return NULL;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Computes a local matrix for the player clamped to valid carry ranges
//-----------------------------------------------------------------------------
// when looking level, hold bottom of object 8 inches below eye level
#define PLAYER_HOLD_LEVEL_EYES	-8

// when looking down, hold bottom of object 0 inches from feet
#define PLAYER_HOLD_DOWN_FEET	2

// when looking up, hold bottom of object 24 inches above eye level
#define PLAYER_HOLD_UP_EYES		24

// use a +/-30 degree range for the entire range of motion of pitch
#define PLAYER_LOOK_PITCH_RANGE	30

// player can reach down 2ft below his feet (otherwise he'll hold the object above the bottom)
#define PLAYER_REACH_DOWN_DISTANCE	24

static void ComputePlayerMatrix(CBasePlayer *pPlayer, matrix3x4_t &out)
{
	if (!pPlayer)
		return;

	QAngle angles = pPlayer->EyeAngles();
	Vector origin = pPlayer->EyePosition();

	// 0-360 / -180-180
	//angles.x = init ? 0 : AngleDistance( angles.x, 0 );
	//angles.x = clamp( angles.x, -PLAYER_LOOK_PITCH_RANGE, PLAYER_LOOK_PITCH_RANGE );
	angles.x = 0;

	float feet = pPlayer->GetAbsOrigin().z + pPlayer->WorldAlignMins().z;
	float eyes = origin.z;
	float zoffset = 0;
	// moving up (negative pitch is up)
	if (angles.x < 0)
	{
		zoffset = RemapVal(angles.x, 0, -PLAYER_LOOK_PITCH_RANGE, PLAYER_HOLD_LEVEL_EYES, PLAYER_HOLD_UP_EYES);
	}
	else
	{
		zoffset = RemapVal(angles.x, 0, PLAYER_LOOK_PITCH_RANGE, PLAYER_HOLD_LEVEL_EYES, PLAYER_HOLD_DOWN_FEET + (feet - eyes));
	}
	origin.z += zoffset;
	angles.x = 0;
	AngleMatrix(angles, origin, out);
}


BEGIN_SIMPLE_DATADESC(game_shadowcontrol_params_t)

DEFINE_FIELD(targetPosition, FIELD_POSITION_VECTOR),
DEFINE_FIELD(targetRotation, FIELD_VECTOR),
DEFINE_FIELD(maxAngular, FIELD_FLOAT),
DEFINE_FIELD(maxDampAngular, FIELD_FLOAT),
DEFINE_FIELD(maxSpeed, FIELD_FLOAT),
DEFINE_FIELD(maxDampSpeed, FIELD_FLOAT),
DEFINE_FIELD(dampFactor, FIELD_FLOAT),
DEFINE_FIELD(teleportDistance, FIELD_FLOAT),

END_DATADESC()

BEGIN_SIMPLE_DATADESC(CGrabController)

DEFINE_EMBEDDED(m_shadow),

DEFINE_FIELD(m_timeToArrive, FIELD_FLOAT),
DEFINE_FIELD(m_errorTime, FIELD_FLOAT),
DEFINE_FIELD(m_error, FIELD_FLOAT),
DEFINE_FIELD(m_contactAmount, FIELD_FLOAT),
DEFINE_AUTO_ARRAY(m_savedRotDamping, FIELD_FLOAT),
DEFINE_AUTO_ARRAY(m_savedMass, FIELD_FLOAT),
DEFINE_FIELD(m_flLoadWeight, FIELD_FLOAT),
DEFINE_FIELD(m_bCarriedEntityBlocksLOS, FIELD_BOOLEAN),
DEFINE_FIELD(m_bIgnoreRelativePitch, FIELD_BOOLEAN),
DEFINE_FIELD(m_attachedEntity, FIELD_EHANDLE),
DEFINE_FIELD(m_angleAlignment, FIELD_FLOAT),
DEFINE_FIELD(m_vecPreferredCarryAngles, FIELD_VECTOR),
DEFINE_FIELD(m_bHasPreferredCarryAngles, FIELD_BOOLEAN),
DEFINE_FIELD(m_flDistanceOffset, FIELD_FLOAT),
DEFINE_FIELD(m_attachedAnglesPlayerSpace, FIELD_VECTOR),
DEFINE_FIELD(m_attachedPositionObjectSpace, FIELD_VECTOR),
DEFINE_FIELD(m_bAllowObjectOverhead, FIELD_BOOLEAN),

// Physptrs can't be inside embedded classes
// DEFINE_PHYSPTR( m_controller ),

END_DATADESC()

const float DEFAULT_MAX_ANGULAR = 360.0f * 10.0f;
const float REDUCED_CARRY_MASS = 1.0f;

CGrabController::CGrabController(void)
{
	m_shadow.dampFactor = 1.0;
	m_shadow.teleportDistance = 0;
	m_errorTime = 0;
	m_error = 0;
	// make this controller really stiff!
	m_shadow.maxSpeed = 1000;
	m_shadow.maxAngular = DEFAULT_MAX_ANGULAR;
	m_shadow.maxDampSpeed = m_shadow.maxSpeed * 2;
	m_shadow.maxDampAngular = m_shadow.maxAngular;
	m_attachedEntity = NULL;
	m_vecPreferredCarryAngles = vec3_angle;
	m_bHasPreferredCarryAngles = false;
	m_flDistanceOffset = 0;
}

CGrabController::~CGrabController(void)
{
	DetachEntity(false);
}

void CGrabController::OnRestore()
{
	if (m_controller)
	{
		m_controller->SetEventHandler(this);
	}
}

void CGrabController::SetTargetPosition(const Vector &target, const QAngle &targetOrientation)
{
	m_shadow.targetPosition = target;
	m_shadow.targetRotation = targetOrientation;

	m_timeToArrive = gpGlobals->frametime;

	CBaseEntity *pAttached = GetAttached();
	if (pAttached)
	{
		IPhysicsObject *pObj = pAttached->VPhysicsGetObject();

		if (pObj != NULL)
		{
			pObj->Wake();
		}
		else
		{
			DetachEntity(false);
		}
	}
}

void CGrabController::GetTargetPosition(Vector *target, QAngle *targetOrientation)
{
	if (target)
		*target = m_shadow.targetPosition;

	if (targetOrientation)
		*targetOrientation = m_shadow.targetRotation;
}
//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CGrabController::ComputeError()
{
	if (m_errorTime <= 0)
		return 0;

	CBaseEntity *pAttached = GetAttached();
	if (pAttached)
	{
		Vector pos;
		IPhysicsObject *pObj = pAttached->VPhysicsGetObject();

		if (pObj)
		{
			pObj->GetShadowPosition(&pos, NULL);

			float error = (m_shadow.targetPosition - pos).Length();
			if (m_errorTime > 0)
			{
				if (m_errorTime > 1)
				{
					m_errorTime = 1;
				}
				float speed = error / m_errorTime;
				if (speed > m_shadow.maxSpeed)
				{
					error *= 0.5;
				}
				m_error = (1 - m_errorTime) * m_error + error * m_errorTime;
			}
		}
		else
		{
			DevMsg("Object attached to Physcannon has no physics object\n");
			DetachEntity(false);
			return 9999; // force detach
		}
	}

	if (pAttached->IsEFlagSet(EFL_IS_BEING_LIFTED_BY_BARNACLE))
	{
		m_error *= 3.0f;
	}

	// If held across a portal but not looking at the portal multiply error
	CPortal_Player *pPortalPlayer = (CPortal_Player *)GetPlayerHoldingEntity(pAttached);
	Assert(pPortalPlayer);
	if (pPortalPlayer->IsHeldObjectOnOppositeSideOfPortal())
	{
		Vector forward, right, up;
		QAngle playerAngles = pPortalPlayer->EyeAngles();

		float pitch = AngleDistance(playerAngles.x, 0);
		playerAngles.x = clamp(pitch, -75, 75);
		AngleVectors(playerAngles, &forward, &right, &up);

		Vector start = pPortalPlayer->Weapon_ShootPosition();

		// If the player is upside down then we need to hold the box closer to their feet.
		if (up.z < 0.0f)
			start += pPortalPlayer->GetViewOffset() * up.z;
		if (right.z < 0.0f)
			start += pPortalPlayer->GetViewOffset() * right.z;

		Ray_t rayPortalTest;
		rayPortalTest.Init(start, start + forward * 256.0f);

		if (UTIL_IntersectRayWithPortal(rayPortalTest, pPortalPlayer->GetHeldObjectPortal()) < 0.0f)
		{
			m_error *= 2.5f;
		}
	}

	m_errorTime = 0;

	return m_error;
}


#define MASS_SPEED_SCALE	60
#define MAX_MASS			40

void CGrabController::ComputeMaxSpeed(CBaseEntity *pEntity, IPhysicsObject *pPhysics)
{
#ifdef GAME_DLL
	m_shadow.maxSpeed = 1000;
	m_shadow.maxAngular = DEFAULT_MAX_ANGULAR;

	// Compute total mass...
	float flMass = PhysGetEntityMass(pEntity);
	float flMaxMass = physcannon_maxmass.GetFloat();
	if (flMass <= flMaxMass)
		return;

	float flLerpFactor = clamp(flMass, flMaxMass, 500.0f);
	flLerpFactor = SimpleSplineRemapVal(flLerpFactor, flMaxMass, 500.0f, 0.0f, 1.0f);

	float invMass = pPhysics->GetInvMass();
	float invInertia = pPhysics->GetInvInertia().Length();

	float invMaxMass = 1.0f / MAX_MASS;
	float ratio = invMaxMass / invMass;
	invMass = invMaxMass;
	invInertia *= ratio;

	float maxSpeed = invMass * MASS_SPEED_SCALE * 200;
	float maxAngular = invInertia * MASS_SPEED_SCALE * 360;

	m_shadow.maxSpeed = Lerp(flLerpFactor, m_shadow.maxSpeed, maxSpeed);
	m_shadow.maxAngular = Lerp(flLerpFactor, m_shadow.maxAngular, maxAngular);
#endif
}


QAngle CGrabController::TransformAnglesToPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer)
{
	if (m_bIgnoreRelativePitch)
	{
		matrix3x4_t test;
		QAngle angleTest = pPlayer->EyeAngles();
		angleTest.x = 0;
		AngleMatrix(angleTest, test);
		return TransformAnglesToLocalSpace(anglesIn, test);
	}
	return TransformAnglesToLocalSpace(anglesIn, pPlayer->EntityToWorldTransform());
}

QAngle CGrabController::TransformAnglesFromPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer)
{
	if (m_bIgnoreRelativePitch)
	{
		matrix3x4_t test;
		QAngle angleTest = pPlayer->EyeAngles();
		angleTest.x = 0;
		AngleMatrix(angleTest, test);
		return TransformAnglesToWorldSpace(anglesIn, test);
	}
	return TransformAnglesToWorldSpace(anglesIn, pPlayer->EntityToWorldTransform());
}


void CGrabController::AttachEntity(CBasePlayer *pPlayer, CBaseEntity *pEntity, IPhysicsObject *pPhys, bool bIsMegaPhysCannon, const Vector &vGrabPosition, bool bUseGrabPosition)
{
	CPortal_Player *pPortalPlayer = ToPortalPlayer(pPlayer);
	// play the impact sound of the object hitting the player
	// used as feedback to let the player know he picked up the object
#ifdef GAME_DLL
	if (!pPortalPlayer->m_bSilentDropAndPickup)
	{
		int hitMaterial = pPhys->GetMaterialIndex();
		int playerMaterial = pPlayer->VPhysicsGetObject() ? pPlayer->VPhysicsGetObject()->GetMaterialIndex() : hitMaterial;
		PhysicsImpactSound(pPlayer, pPhys, CHAN_STATIC, hitMaterial, playerMaterial, 1.0, 64);
	}
#endif
	Vector position;
	QAngle angles;
	pPhys->GetPosition(&position, &angles);
	// If it has a preferred orientation, use that instead.
	Pickup_GetPreferredCarryAngles(pEntity, pPlayer, pPlayer->EntityToWorldTransform(), angles);

	//Fix attachment orientation weirdness
	if (pPortalPlayer->IsHeldObjectOnOppositeSideOfPortal())
	{
		Vector vPlayerForward;
		pPlayer->EyeVectors(&vPlayerForward);

		Vector radial = physcollision->CollideGetExtent(pPhys->GetCollide(), vec3_origin, pEntity->GetAbsAngles(), -vPlayerForward);
		Vector player2d = pPlayer->CollisionProp()->OBBMaxs();
		float playerRadius = player2d.Length2D();
		float flDot = DotProduct(vPlayerForward, radial);

		float radius = playerRadius + fabs(flDot);

		float distance = 24 + (radius * 2.0f);

		//find out which portal the object is on the other side of....
		Vector start = pPlayer->Weapon_ShootPosition();
		Vector end = start + (vPlayerForward * distance);

		CProp_Portal *pObjectPortal = NULL;
		pObjectPortal = pPortalPlayer->GetHeldObjectPortal();

		// If our end point hasn't gone into the portal yet we at least need to know what portal is in front of us
		if (!pObjectPortal)
		{
			Ray_t rayPortalTest;
			rayPortalTest.Init(start, start + vPlayerForward * 1024.0f);

			int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
			if (iPortalCount != 0)
			{
				CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
				float fMinDist = 2.0f;
				for (int i = 0; i != iPortalCount; ++i)
				{
					CProp_Portal *pTempPortal = pPortals[i];
					if (pTempPortal->m_bActivated &&
						(pTempPortal->m_hLinkedPortal.Get() != NULL))
					{
						float fDist = UTIL_IntersectRayWithPortal(rayPortalTest, pTempPortal);
						if ((fDist >= 0.0f) && (fDist < fMinDist))
						{
							fMinDist = fDist;
							pObjectPortal = pTempPortal;
						}
					}
				}
			}
		}

		if (pObjectPortal)
		{
			UTIL_Portal_AngleTransform(pObjectPortal->m_hLinkedPortal->MatrixThisToLinked(), angles, angles);
		}
	}

	VectorITransform(pEntity->WorldSpaceCenter(), pEntity->EntityToWorldTransform(), m_attachedPositionObjectSpace);
	//	ComputeMaxSpeed( pEntity, pPhys );

	// If we haven't been killed by a grab, we allow the gun to grab the nearest part of a ragdoll
	if (bUseGrabPosition)
	{
		IPhysicsObject *pChild = GetRagdollChildAtPosition(pEntity, vGrabPosition);

		if (pChild)
		{
			pPhys = pChild;
		}
	}

	// Carried entities can never block LOS
	m_bCarriedEntityBlocksLOS = pEntity->BlocksLOS();
	pEntity->SetBlocksLOS(false);
	m_controller = physenv->CreateMotionController(this);
	m_controller->AttachObject(pPhys, true);
	// Don't do this, it's causing trouble with constraint solvers.
	//m_controller->SetPriority( IPhysicsMotionController::HIGH_PRIORITY );

	pPhys->Wake();
	PhysSetGameFlags(pPhys, FVPHYSICS_PLAYER_HELD);
	SetTargetPosition(position, angles);
	m_attachedEntity = pEntity;
	IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pEntity->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
	m_flLoadWeight = 0;
	float damping = 10;
	float flFactor = count / 7.5f;
	if (flFactor < 1.0f)
	{
		flFactor = 1.0f;
	}
	for (int i = 0; i < count; i++)
	{
		float mass = pList[i]->GetMass();
		pList[i]->GetDamping(NULL, &m_savedRotDamping[i]);
		m_flLoadWeight += mass;
		m_savedMass[i] = mass;

		// reduce the mass to prevent the player from adding crazy amounts of energy to the system
		pList[i]->SetMass(REDUCED_CARRY_MASS / flFactor);
		pList[i]->SetDamping(NULL, &damping);
	}

	// Give extra mass to the phys object we're actually picking up
	pPhys->SetMass(REDUCED_CARRY_MASS);
	pPhys->EnableDrag(false);

	m_errorTime = bIsMegaPhysCannon ? -1.5f : -1.0f; // 1 seconds until error starts accumulating
	m_error = 0;
	m_contactAmount = 0;

	m_attachedAnglesPlayerSpace = TransformAnglesToPlayerSpace(angles, pPlayer);
	if (m_angleAlignment != 0)
	{
		m_attachedAnglesPlayerSpace = AlignAngles(m_attachedAnglesPlayerSpace, m_angleAlignment);
	}

	// Ragdolls don't offset this way
#ifdef GAME_DLL
	if (dynamic_cast<CRagdollProp*>(pEntity))
	{
		m_attachedPositionObjectSpace.Init();
	}
	else
#endif
	{
		VectorITransform(pEntity->WorldSpaceCenter(), pEntity->EntityToWorldTransform(), m_attachedPositionObjectSpace);
	}

	// If it's a prop, see if it has desired carry angles
	CPhysicsProp *pProp = dynamic_cast<CPhysicsProp *>(pEntity);
	if (pProp)
	{
#ifdef CLIENT_DLL
		// We need to update all this
		m_bHasPreferredCarryAngles = pProp->HasPreferredCarryAnglesForPlayer( pPlayer );
		m_flDistanceOffset = 0;
#else
		m_bHasPreferredCarryAngles = pProp->GetPropDataAngles("preferred_carryangles", m_vecPreferredCarryAngles);
		m_flDistanceOffset = pProp->GetCarryDistanceOffset();
#endif
	}
	else
	{
		m_bHasPreferredCarryAngles = false;
		m_flDistanceOffset = 0;
	}

	m_bAllowObjectOverhead = IsObjectAllowedOverhead(pEntity);
}

static void ClampPhysicsVelocity(IPhysicsObject *pPhys, float linearLimit, float angularLimit)
{
	Vector vel;
	AngularImpulse angVel;
	pPhys->GetVelocity(&vel, &angVel);
	float speed = VectorNormalize(vel) - linearLimit;
	float angSpeed = VectorNormalize(angVel) - angularLimit;
	speed = speed < 0 ? 0 : -speed;
	angSpeed = angSpeed < 0 ? 0 : -angSpeed;
	vel *= speed;
	angVel *= angSpeed;
	pPhys->AddVelocity(&vel, &angVel);
}

void CGrabController::DetachEntity(bool bClearVelocity)
{
#ifdef GAME_DLL
	Assert(!PhysIsInCallback());
#endif
	CBaseEntity *pEntity = GetAttached();
	if (pEntity)
	{
		// Restore the LS blocking state
		pEntity->SetBlocksLOS(m_bCarriedEntityBlocksLOS);
		IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
		int count = pEntity->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
		for (int i = 0; i < count; i++)
		{
			IPhysicsObject *pPhys = pList[i];
			if (!pPhys)
				continue;

			// on the odd chance that it's gone to sleep while under anti-gravity
			pPhys->EnableDrag(true);
			pPhys->Wake();
			pPhys->SetMass(m_savedMass[i]);
			pPhys->SetDamping(NULL, &m_savedRotDamping[i]);
			PhysClearGameFlags(pPhys, FVPHYSICS_PLAYER_HELD);
			if (bClearVelocity)
			{
				PhysForceClearVelocity(pPhys);
			}
			else
			{
				ClampPhysicsVelocity(pPhys, player_walkspeed.GetFloat() * 1.5f, 2.0f * 360.0f);
			}

		}
	}

	m_attachedEntity = NULL;
	physenv->DestroyMotionController(m_controller);
	m_controller = NULL;
}

static bool InContactWithHeavyObject(IPhysicsObject *pObject, float heavyMass)
{
	bool contact = false;
	IPhysicsFrictionSnapshot *pSnapshot = pObject->CreateFrictionSnapshot();
	while (pSnapshot->IsValid())
	{
		IPhysicsObject *pOther = pSnapshot->GetObject(1);
		if (!pOther->IsMoveable() || pOther->GetMass() > heavyMass)
		{
			contact = true;
			break;
		}
		pSnapshot->NextFrictionData();
	}
	pObject->DestroyFrictionSnapshot(pSnapshot);
	return contact;
}

IMotionEvent::simresult_e CGrabController::Simulate(IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular)
{
	game_shadowcontrol_params_t shadowParams = m_shadow;
	if (InContactWithHeavyObject(pObject, GetLoadWeight()))
	{
		m_contactAmount = Approach(0.1f, m_contactAmount, deltaTime*2.0f);
	}
	else
	{
		m_contactAmount = Approach(1.0f, m_contactAmount, deltaTime*2.0f);
	}
	shadowParams.maxAngular = m_shadow.maxAngular * m_contactAmount * m_contactAmount * m_contactAmount;
	m_timeToArrive = pObject->ComputeShadowControl(shadowParams, m_timeToArrive, deltaTime);

	// Slide along the current contact points to fix bouncing problems
	Vector velocity;
	AngularImpulse angVel;
	pObject->GetVelocity(&velocity, &angVel);
	PhysComputeSlideDirection(pObject, velocity, angVel, &velocity, &angVel, GetLoadWeight());
	pObject->SetVelocityInstantaneous(&velocity, NULL);

	linear.Init();
	angular.Init();
	m_errorTime += deltaTime;

	return SIM_LOCAL_ACCELERATION;
}

float CGrabController::GetSavedMass(IPhysicsObject *pObject)
{
	CBaseEntity *pHeld = m_attachedEntity;
	if (pHeld)
	{
		if (pObject->GetGameData() == (void*)pHeld)
		{
			IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
			int count = pHeld->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
			for (int i = 0; i < count; i++)
			{
				if (pList[i] == pObject)
					return m_savedMass[i];
			}
		}
	}
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Is this an object that the player is allowed to lift to a position 
// directly overhead? The default behavior prevents lifting objects directly
// overhead, but there are exceptions for gameplay purposes.
//-----------------------------------------------------------------------------
bool CGrabController::IsObjectAllowedOverhead(CBaseEntity *pEntity)
{
#ifdef GAME_DLL
	// Allow combine balls overhead 
	if (UTIL_IsCombineBallDefinite(pEntity))
		return true;

	// Allow props that are specifically flagged as such
	CPhysicsProp *pPhysProp = dynamic_cast<CPhysicsProp *>(pEntity);
	if (pPhysProp != NULL && pPhysProp->HasInteraction(PROPINTER_PHYSGUN_ALLOW_OVERHEAD))
		return true;

	// String checks are fine here, we only run this code one time- when the object is picked up.
	if (pEntity->ClassMatches("grenade_helicopter"))
		return true;

	if (pEntity->ClassMatches("weapon_striderbuster"))
		return true;
#endif

	return false;
}




void CGrabController::SetPortalPenetratingEntity(CBaseEntity *pPenetrated)
{
	m_PenetratedEntity = pPenetrated;
}

//-----------------------------------------------------------------------------
// Player pickup controller
//-----------------------------------------------------------------------------
class CPlayerPickupController : public CBaseEntity
{
	DECLARE_DATADESC();
	DECLARE_CLASS(CPlayerPickupController, CBaseEntity);
public:
	void Init(CBasePlayer *pPlayer, CBaseEntity *pObject);
	void Shutdown(bool bThrown = false);
	bool OnControls(CBaseEntity *pControls) { return true; }
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void OnRestore()
	{
		m_grabController.OnRestore();
	}
	void VPhysicsUpdate(IPhysicsObject *pPhysics) {}
	void VPhysicsShadowUpdate(IPhysicsObject *pPhysics) {}

	bool IsHoldingEntity(CBaseEntity *pEnt);
	CGrabController &GetGrabController() { return m_grabController; }

private:
	CGrabController		m_grabController;
	CBasePlayer			*m_pPlayer;
};

LINK_ENTITY_TO_CLASS(player_pickup, CPlayerPickupController);

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CPlayerPickupController)

DEFINE_EMBEDDED(m_grabController),

// Physptrs can't be inside embedded classes
DEFINE_PHYSPTR(m_grabController.m_controller),

DEFINE_FIELD(m_pPlayer, FIELD_CLASSPTR),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*pObject - 
//-----------------------------------------------------------------------------
void CPlayerPickupController::Init(CBasePlayer *pPlayer, CBaseEntity *pObject)
{
#ifdef GAME_DLL
	// Holster player's weapon
	if (pPlayer->GetActiveWeapon())
	{
		// Don't holster the portalgun
		if (FClassnameIs(pPlayer->GetActiveWeapon(), "weapon_portalgun"))
		{
			CWeaponPortalgun *pPortalGun = (CWeaponPortalgun*)(pPlayer->GetActiveWeapon());
			pPortalGun->OpenProngs(true);
		}
		else
		{
			if (!pPlayer->GetActiveWeapon()->Holster())
			{
				Shutdown();
				return;
			}
		}
	}
#endif

	// If the target is debris, convert it to non-debris
	if (pObject->GetCollisionGroup() == COLLISION_GROUP_DEBRIS)
	{
		// Interactive debris converts back to debris when it comes to rest
		pObject->SetCollisionGroup(COLLISION_GROUP_INTERACTIVE_DEBRIS);
	}

	// done so I'll go across level transitions with the player
	SetParent(pPlayer);
	m_grabController.SetIgnorePitch(true);
#ifdef GAME_DLL
	m_grabController.SetAngleAlignment(DOT_30DEGREE);
#else
	m_grabController.SetAngleAlignment(0.866025403784);
	
#endif
	m_pPlayer = pPlayer;
	IPhysicsObject *pPhysics = pObject->VPhysicsGetObject();

	Pickup_OnPhysGunPickup(pObject, m_pPlayer, PICKED_UP_BY_PLAYER);

	m_grabController.AttachEntity(pPlayer, pObject, pPhysics, false, vec3_origin, false);

	m_pPlayer->m_Local.m_iHideHUD |= HIDEHUD_WEAPONSELECTION;
#ifdef GAME_DLL
	m_pPlayer->SetUseEntity(this);
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void CPlayerPickupController::Shutdown(bool bThrown)
{
	CBaseEntity *pObject = m_grabController.GetAttached();

	bool bClearVelocity = false;
	if (!bThrown && pObject && pObject->VPhysicsGetObject() && pObject->VPhysicsGetObject()->GetContactPoint(NULL, NULL))
	{
		bClearVelocity = true;
	}

	m_grabController.DetachEntity(bClearVelocity);

#ifdef GAME_DLL
	if (pObject != NULL)
	{
		if (!ToPortalPlayer(m_pPlayer)->m_bSilentDropAndPickup)
		{
			Pickup_OnPhysGunDrop(pObject, m_pPlayer, bThrown ? THROWN_BY_PLAYER : DROPPED_BY_PLAYER);
		}
	}
	if (m_pPlayer)
	{
		m_pPlayer->SetUseEntity(NULL);
			if (m_pPlayer->GetActiveWeapon())
			{
				if (FClassnameIs(m_pPlayer->GetActiveWeapon(), "weapon_portalgun"))
				{
					m_pPlayer->SetNextAttack(gpGlobals->curtime + 0.5f);
					CWeaponPortalgun *pPortalGun = (CWeaponPortalgun*)(m_pPlayer->GetActiveWeapon());
					pPortalGun->DelayAttack(0.5f);
					pPortalGun->OpenProngs(false);
				}
				else
				{
					if (!m_pPlayer->GetActiveWeapon()->Deploy())
					{
						// We tried to restore the player's weapon, but we couldn't.
						// This usually happens when they're holding an empty weapon that doesn't
						// autoswitch away when out of ammo. Switch to next best weapon.
						m_pPlayer->SwitchToNextBestWeapon(NULL);
					}
				}
			}

			m_pPlayer->m_Local.m_iHideHUD &= ~HIDEHUD_WEAPONSELECTION;
	}
#endif
	Remove();
}


void CPlayerPickupController::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (ToBasePlayer(pActivator) == m_pPlayer)
	{
		CBaseEntity *pAttached = m_grabController.GetAttached();

		// UNDONE: Use vphysics stress to decide to drop objects
		// UNDONE: Must fix case of forcing objects into the ground you're standing on (causes stress) before that will work
		if (!pAttached || useType == USE_OFF || (m_pPlayer->m_nButtons & IN_ATTACK2) || m_grabController.ComputeError() > 12)
		{
			Shutdown();
			return;
		}

		//Adrian: Oops, our object became motion disabled, let go!
		IPhysicsObject *pPhys = pAttached->VPhysicsGetObject();
		if (pPhys && pPhys->IsMoveable() == false)
		{
			Shutdown();
			return;
		}

#if STRESS_TEST
		vphysics_objectstress_t stress;
		CalculateObjectStress(pPhys, pAttached, &stress);
		if (stress.exertedStress > 250)
		{
			Shutdown();
			return;
		}
#endif
		// +ATTACK will throw phys objects
		if (m_pPlayer->m_nButtons & IN_ATTACK)
		{
			Shutdown(true);
			Vector vecLaunch;
			m_pPlayer->EyeVectors(&vecLaunch);
			// If throwing from the opposite side of a portal, reorient the direction
			if (((CPortal_Player *)m_pPlayer)->IsHeldObjectOnOppositeSideOfPortal())
			{
				CProp_Portal *pHeldPortal = ((CPortal_Player *)m_pPlayer)->GetHeldObjectPortal();
				UTIL_Portal_VectorTransform(pHeldPortal->MatrixThisToLinked(), vecLaunch, vecLaunch);
			}

			((CPortal_Player *)m_pPlayer)->SetHeldObjectOnOppositeSideOfPortal(false);
			// JAY: Scale this with mass because some small objects really go flying
			float massFactor = clamp(pPhys->GetMass(), 0.5, 15);
			massFactor = RemapVal(massFactor, 0.5, 15, 0.5, 4);
			vecLaunch *= player_throwforce.GetFloat() * massFactor;

			pPhys->ApplyForceCenter(vecLaunch);
			AngularImpulse aVel = RandomAngularImpulse(-10, 10) * massFactor;
			pPhys->ApplyTorqueCenter(aVel);
			return;
		}

		if (useType == USE_SET)
		{
			// update position
			m_grabController.UpdateObject(m_pPlayer, 12);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEnt - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPlayerPickupController::IsHoldingEntity(CBaseEntity *pEnt)
{
	if (pEnt)
	{
		return (m_grabController.GetAttached() == pEnt);
	}

	return (m_grabController.GetAttached() != 0);
}

void PlayerPickupObject(CBasePlayer *pPlayer, CBaseEntity *pObject)
{
#ifdef GAME_DLL
	//Don't pick up if we don't have a phys object.
	if (pObject->VPhysicsGetObject() == NULL)
		return;

	if (pObject->GetBaseAnimating() && pObject->GetBaseAnimating()->IsDissolving())
		return;

	CPlayerPickupController *pController = (CPlayerPickupController *)CBaseEntity::Create("player_pickup", pObject->GetAbsOrigin(), vec3_angle, pPlayer);

	if (!pController)
		return;

	pController->Init(pPlayer, pObject);
#endif
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Physcannon
//-----------------------------------------------------------------------------

#define	NUM_BEAMS	4
#define	NUM_SPRITES	6

struct thrown_objects_t
{
	float				fTimeThrown;
	EHANDLE				hEntity;

	DECLARE_SIMPLE_DATADESC();
};

BEGIN_SIMPLE_DATADESC(thrown_objects_t)
DEFINE_FIELD(fTimeThrown, FIELD_TIME),
DEFINE_FIELD(hEntity, FIELD_EHANDLE),
END_DATADESC()



enum
{
	ELEMENT_STATE_NONE = -1,
	ELEMENT_STATE_OPEN,
	ELEMENT_STATE_CLOSED,
};

enum
{
	EFFECT_NONE,
	EFFECT_CLOSED,
	EFFECT_READY,
	EFFECT_HOLDING,
	EFFECT_LAUNCH,
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGrabController::UpdateObject(CBasePlayer *pPlayer, float flError)
{
	CBaseEntity *pPenetratedEntity = m_PenetratedEntity.Get();
	if (pPenetratedEntity)
	{
		//FindClosestPassableSpace( pPenetratedEntity, Vector( 0.0f, 0.0f, 1.0f ) );
		IPhysicsObject *pPhysObject = pPenetratedEntity->VPhysicsGetObject();
		if (pPhysObject)
			pPhysObject->Wake();

		m_PenetratedEntity = NULL; //assume we won
	}

	CBaseEntity *pEntity = GetAttached();
	if (!pEntity || ComputeError() > flError || pPlayer->GetGroundEntity() == pEntity || !pEntity->VPhysicsGetObject())
	{
		return false;
	}

	//Adrian: Oops, our object became motion disabled, let go!
	IPhysicsObject *pPhys = pEntity->VPhysicsGetObject();
	if (pPhys && pPhys->IsMoveable() == false)
	{
		return false;
	}

	Vector forward, right, up;
	QAngle playerAngles = pPlayer->EyeAngles();
	float pitch = AngleDistance(playerAngles.x, 0);
	if (!m_bAllowObjectOverhead)
	{
		playerAngles.x = clamp(pitch, -75, 75);
	}
	else
	{
		playerAngles.x = clamp(pitch, -90, 75);
	}
	AngleVectors(playerAngles, &forward, &right, &up);

	Vector start = pPlayer->Weapon_ShootPosition();

	// If the player is upside down then we need to hold the box closer to their feet.
	if (up.z < 0.0f)
		start += pPlayer->GetViewOffset() * up.z;
	if (right.z < 0.0f)
		start += pPlayer->GetViewOffset() * right.z;

	CPortal_Player *pPortalPlayer = ToPortalPlayer(pPlayer);

	// Find out if it's being held across a portal
	bool bLookingAtHeldPortal = true;
	CProp_Portal *pPortal = pPortalPlayer->GetHeldObjectPortal();

	if (!pPortal)
	{
		// If the portal is invalid make sure we don't try to hold it across the portal
		pPortalPlayer->SetHeldObjectOnOppositeSideOfPortal(false);
	}

	if (pPortalPlayer->IsHeldObjectOnOppositeSideOfPortal())
	{
		Ray_t rayPortalTest;
		rayPortalTest.Init(start, start + forward * 1024.0f);

		// Check if we're looking at the portal we're holding through
		if (pPortal)
		{
			if (UTIL_IntersectRayWithPortal(rayPortalTest, pPortal) < 0.0f)
			{
				bLookingAtHeldPortal = false;
			}
		}
		// If our end point hasn't gone into the portal yet we at least need to know what portal is in front of us
		else
		{
			int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
			if (iPortalCount != 0)
			{
				CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
				float fMinDist = 2.0f;
				for (int i = 0; i != iPortalCount; ++i)
				{
					CProp_Portal *pTempPortal = pPortals[i];
					if (pTempPortal->m_bActivated &&
						(pTempPortal->m_hLinkedPortal.Get() != NULL))
					{
						float fDist = UTIL_IntersectRayWithPortal(rayPortalTest, pTempPortal);
						if ((fDist >= 0.0f) && (fDist < fMinDist))
						{
							fMinDist = fDist;
							pPortal = pTempPortal;
						}
					}
				}
			}
		}
	}
	else
	{
		pPortal = NULL;
	}

	QAngle qEntityAngles = pEntity->GetAbsAngles();
#ifdef GAME_DLL
	if (pPortal)
	{

		// If the portal isn't linked we need to drop the object
		if (!pPortal->m_hLinkedPortal.Get())
		{
			pPlayer->ForceDropOfCarriedPhysObjects();
			return false;
		}

		UTIL_Portal_AngleTransform(pPortal->m_hLinkedPortal->MatrixThisToLinked(), qEntityAngles, qEntityAngles);
	}
#endif
	// Now clamp a sphere of object radius at end to the player's bbox
	Vector radial = physcollision->CollideGetExtent(pPhys->GetCollide(), vec3_origin, qEntityAngles, -forward);
	Vector player2d = pPlayer->CollisionProp()->OBBMaxs();
	float playerRadius = player2d.Length2D();

	float radius = playerRadius + radial.Length();

	float distance = 24 + (radius * 2.0f);

	// Add the prop's distance offset
	distance += m_flDistanceOffset;

	Vector end = start + (forward * distance);

	trace_t	tr;
	CTraceFilterSkipTwoEntities traceFilter(pPlayer, pEntity, COLLISION_GROUP_NONE);
	Ray_t ray;
	ray.Init(start, end);
	//enginetrace->TraceRay( ray, MASK_SOLID_BRUSHONLY, &traceFilter, &tr );
	UTIL_Portal_TraceRay(ray, MASK_SOLID_BRUSHONLY, &traceFilter, &tr);

	if (tr.fraction < 0.5)
	{
		end = start + forward * (radius*0.5f);
	}
	else if (tr.fraction <= 1.0f)
	{
		end = start + forward * (distance - radius);
	}
	Vector playerMins, playerMaxs, nearest;
	pPlayer->CollisionProp()->WorldSpaceAABB(&playerMins, &playerMaxs);
	Vector playerLine = pPlayer->CollisionProp()->WorldSpaceCenter();
	CalcClosestPointOnLine(end, playerLine + Vector(0, 0, playerMins.z), playerLine + Vector(0, 0, playerMaxs.z), nearest, NULL);

	if (!m_bAllowObjectOverhead)
	{
		Vector delta = end - nearest;
		float len = VectorNormalize(delta);
		if (len < radius)
		{
			end = nearest + radius * delta;
		}
	}

	QAngle angles = TransformAnglesFromPlayerSpace(m_attachedAnglesPlayerSpace, pPlayer);

	//Show overlays of radius
	if (g_debug_physcannon.GetBool())
	{
		NDebugOverlay::Box(end, -Vector(2, 2, 2), Vector(2, 2, 2), 0, 255, 0, true, 0);

		NDebugOverlay::Box(GetAttached()->WorldSpaceCenter(),
			-Vector(radius, radius, radius),
			Vector(radius, radius, radius),
			255, 0, 0,
			true,
			0.0f);
	}


	// If it has a preferred orientation, update to ensure we're still oriented correctly.
	Pickup_GetPreferredCarryAngles(pEntity, pPlayer, pPlayer->EntityToWorldTransform(), angles);

	// We may be holding a prop that has preferred carry angles
	if (m_bHasPreferredCarryAngles)
	{
		matrix3x4_t tmp;
		ComputePlayerMatrix(pPlayer, tmp);
		angles = TransformAnglesToWorldSpace(m_vecPreferredCarryAngles, tmp);
	}

	matrix3x4_t attachedToWorld;
	Vector offset;
	AngleMatrix(angles, attachedToWorld);
	VectorRotate(m_attachedPositionObjectSpace, attachedToWorld, offset);

	// Translate hold position and angles across portal
	if (pPortalPlayer->IsHeldObjectOnOppositeSideOfPortal())
	{
		CProp_Portal *pPortalLinked = pPortal->m_hLinkedPortal;
		if (pPortal && pPortal->m_bActivated && pPortalLinked != NULL)
		{
			Vector vTeleportedPosition;
			QAngle qTeleportedAngles;

			if (!bLookingAtHeldPortal && (start - pPortal->GetAbsOrigin()).Length() > distance - radius)
			{
				// Pull the object through the portal
				Vector vPortalLinkedForward;
				pPortalLinked->GetVectors(&vPortalLinkedForward, NULL, NULL);
				vTeleportedPosition = pPortalLinked->GetAbsOrigin() - vPortalLinkedForward * (1.0f + offset.Length());
				qTeleportedAngles = pPortalLinked->GetAbsAngles();
			}
			else
			{
				// Translate hold position and angles across the portal
				VMatrix matThisToLinked = pPortal->MatrixThisToLinked();
				UTIL_Portal_PointTransform(matThisToLinked, end - offset, vTeleportedPosition);
				UTIL_Portal_AngleTransform(matThisToLinked, angles, qTeleportedAngles);
			}

			SetTargetPosition(vTeleportedPosition, qTeleportedAngles);
#ifdef GAME_DLL
			pPortalPlayer->SetHeldObjectPortal(pPortal);
		}
		else
		{
			pPlayer->ForceDropOfCarriedPhysObjects();
#endif
		}
	}
	else
	{
		SetTargetPosition(end - offset, angles);
#ifdef GAME_DLL
		pPortalPlayer->SetHeldObjectPortal(NULL);
#endif
	}

	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define PHYSCANNON_DANGER_SOUND_RADIUS 128

#define	PHYSCANNON_MAX_MASS		500



//-----------------------------------------------------------------------------
// Purpose: Go through the objects in the thrown objects list and discard any
//			objects that have gone 'stale'. (Were thrown several seconds ago), or
//			have been destroyed or removed.
//
//-----------------------------------------------------------------------------
#define PHYSCANNON_THROWN_LIST_TIMEOUT	10.0f


bool PlayerPickupControllerIsHoldingEntity(CBaseEntity *pPickupControllerEntity, CBaseEntity *pHeldEntity)
{
	CPlayerPickupController *pController = dynamic_cast<CPlayerPickupController *>(pPickupControllerEntity);

	return pController ? pController->IsHoldingEntity(pHeldEntity) : false;
}

void ShutdownPickupController(CBaseEntity *pPickupControllerEntity)
{
	CPlayerPickupController *pController = dynamic_cast<CPlayerPickupController *>(pPickupControllerEntity);

	pController->Shutdown(false);
}


CBaseEntity *GetPlayerHeldEntity(CBasePlayer *pPlayer)
{
	CBaseEntity *pObject = NULL;
	CPlayerPickupController *pPlayerPickupController = (CPlayerPickupController *)(pPlayer->GetUseEntity());

	if (pPlayerPickupController)
	{
		pObject = pPlayerPickupController->GetGrabController().GetAttached();
	}

	return pObject;
}

CBasePlayer *GetPlayerHoldingEntity(CBaseEntity *pEntity)
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer)
		{
			if (GetPlayerHeldEntity(pPlayer) == pEntity)
				return pPlayer;
		}
	}
	return NULL;
}

CGrabController *GetGrabControllerForPlayer(CBasePlayer *pPlayer)
{
	CPlayerPickupController *pPlayerPickupController = (CPlayerPickupController *)(pPlayer->GetUseEntity());
	if (pPlayerPickupController)
		return &(pPlayerPickupController->GetGrabController());

	return NULL;
}

void GetSavedParamsForCarriedPhysObject(CGrabController *pGrabController, IPhysicsObject *pObject, float *pSavedMassOut, float *pSavedRotationalDampingOut)
{
	CBaseEntity *pHeld = pGrabController->m_attachedEntity;
	if (pHeld)
	{
		if (pObject->GetGameData() == (void*)pHeld)
		{
			IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
			int count = pHeld->VPhysicsGetObjectList(pList, ARRAYSIZE(pList));
			for (int i = 0; i < count; i++)
			{
				if (pList[i] == pObject)
				{
					if (pSavedMassOut)
						*pSavedMassOut = pGrabController->m_savedMass[i];

					if (pSavedRotationalDampingOut)
						*pSavedRotationalDampingOut = pGrabController->m_savedRotDamping[i];

					return;
				}
			}
		}
	}

	if (pSavedMassOut)
		*pSavedMassOut = 0.0f;

	if (pSavedRotationalDampingOut)
		*pSavedRotationalDampingOut = 0.0f;

	return;
}

void UpdateGrabControllerTargetPosition(CBasePlayer *pPlayer, Vector *vPosition, QAngle *qAngles)
{
	CGrabController *pGrabController = GetGrabControllerForPlayer(pPlayer);

	if (!pGrabController)
		return;

	pGrabController->UpdateObject(pPlayer, 12);
	pGrabController->GetTargetPosition(vPosition, qAngles);
}


float PlayerPickupGetHeldObjectMass(CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject)
{
	float mass = 0.0f;
	CPlayerPickupController *pController = dynamic_cast<CPlayerPickupController *>(pPickupControllerEntity);
	if (pController)
	{
		CGrabController &grab = pController->GetGrabController();
		mass = grab.GetSavedMass(pHeldObject);
	}
	return mass;
}


void GrabController_SetPortalPenetratingEntity(CGrabController *pController, CBaseEntity *pPenetrated)
{
	pController->SetPortalPenetratingEntity(pPenetrated);
}
