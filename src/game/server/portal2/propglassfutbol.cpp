#include "cbase.h"
#include "props.h"
#include "portal_player.h"
#include "trace.h"
#include "tier0/memdbgon.h"

ConVar sv_futbol_fake_force("sv_futbol_fake_force", "500", FCVAR_NONE);
ConVar sv_futbol_force_players_to_catch("sv_futbol_force_players_to_catch", "1", FCVAR_NONE);
ConVar sv_futbol_use_cooldown_time("sv_futbol_use_cooldown_time", "0.7", FCVAR_NONE);
ConVar sv_futbol_use_steals_from_holding_player("sv_futbol_use_steals_from_holding_player", "1", FCVAR_NONE);

static const char* g_szFutbolAnimThinkContext = "FutbolAnimateThinkContext";
static const char* g_szFutbolThrownThinkContext = "FutbolThrownThinkContext";

enum futbol_holder_type_t
{
	FUTBOL_HELD_BY_NONE = 0,
	FUTBOL_HELD_BY_PLAYER = 1,
	FUTBOL_HELD_BY_SPAWNER = 2,
	FUTBOL_HELD_BY_CATCHER = 3,
	FUTBOL_HELD_BY_COUNT = 4
};

class CFutbolCatcher : public CBaseAnimating
{
public:
	DECLARE_CLASS(CFutbolCatcher, CBaseAnimating);
	DECLARE_DATADESC();

	CFutbolCatcher::CFutbolCatcher()
	{
		m_vCatcherBoxHalfDiagonal = Vector(25.0f, 25.0f, 25.0f);
		m_vecCatchBoxMins = vec3_origin;
		m_vecCatchBoxMaxs = vec3_origin;
		m_vecCatchBoxOrig = vec3_origin;
		m_bDisableRecaptureOnPlayerGrab = false;
	}
	void Spawn();

protected:
	CHandle<CBaseEntity> m_hCaughtFutbol;
	Vector m_vCatcherBoxHalfDiagonal;
	Vector m_vecCatchBoxMins;
	Vector m_vecCatchBoxMaxs;
	Vector m_vecCatchBoxOrig;
	bool m_bDisableRecaptureOnPlayerGrab;
	COutputEvent m_OnFutbolReleased;
	COutputEvent m_OnFutbolCaught;
	void CatchThink();
	void CaptureThink();
	void CaptureFutbol(class CPropGlassFutbol* pFutbol);
};

class CFilterOnlyGlassFutbol : public CTraceFilterSimple
{
	DECLARE_CLASS(CFilterOnlyGlassFutbol, CTraceFilterSimple);
public:
	CFilterOnlyGlassFutbol(const IHandleEntity* passentity, int collisionGroup)
		: CTraceFilterSimple(passentity, collisionGroup)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity* pHandleEntity, int contentsMask);
};

class CPropFutbolSocket : public CFutbolCatcher
{
public:
	DECLARE_CLASS(CPropFutbolSocket, CFutbolCatcher);

	void Precache();
	void Spawn();
};

class CPropFutbolSpawner : public CFutbolCatcher
{
public:
	DECLARE_CLASS(CPropFutbolSpawner, CFutbolCatcher);
	DECLARE_DATADESC();

	CPropFutbolSpawner::CPropFutbolSpawner()
	{
		m_bHasFutbol = false;
		m_vCatcherBoxHalfDiagonal = Vector(7.0f, 7.0f, 7.0f);
	}

	void Precache();
	void Spawn();
	void FutbolDestroyed();
	void FutbolGrabbed();
private:
	bool m_bHasFutbol;
	COutputEvent m_OnFutbolSpawned;
	COutputEvent m_OnFutbolGrabbed;
	void InputForceSpawn(inputdata_t& data);
	void SpawnFutbol();
};

class CPropGlassFutbol : public CPhysicsProp
{
public:
	DECLARE_CLASS(CPropGlassFutbol, CPhysicsProp);
	DECLARE_DATADESC();


	CPropGlassFutbol::CPropGlassFutbol()
	{
		m_Holder = FUTBOL_HELD_BY_NONE;
		m_vecThrowDirection = vec3_origin;
	}

	QAngle PreferredCarryAngles() { return QAngle(180, -90, 180); }
	bool HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	void Precache();
	void Spawn();
	void Event_Killed(const CTakeDamageInfo& info);

	//Think functions
	void ThrownThink();
	void AnimThink();

	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void SetSpawner(CPropFutbolSpawner* pMySpawner);

	futbol_holder_type_t GetHolder { m_Holder };
	void SetHolder(futbol_holder_type_t type) { GetHolder = type; }
	CPortal_Player* GetLastPlayerToHold() { return m_hLastHeldByPlayer; }
private:
	CHandle<CPropFutbolSpawner> m_hSpawner;
	string_t m_strSpawnerName;
	futbol_holder_type_t m_Holder;
	Vector m_vecThrowDirection;
	CHandle<CPortal_Player> m_hLastHeldByPlayer;
};

LINK_ENTITY_TO_CLASS(futbol_catcher, CFutbolCatcher);

BEGIN_DATADESC(CFutbolCatcher)
	DEFINE_FIELD(m_hCaughtFutbol, FIELD_EHANDLE),
	DEFINE_FIELD(m_vecCatchBoxMins, FIELD_VECTOR),
	DEFINE_FIELD(m_vecCatchBoxMaxs, FIELD_VECTOR),
	DEFINE_FIELD(m_vecCatchBoxOrig, FIELD_VECTOR),
	DEFINE_FIELD(m_bDisableRecaptureOnPlayerGrab, FIELD_BOOLEAN),
	DEFINE_OUTPUT(m_OnFutbolReleased, "OnFutbolReleased"),
	DEFINE_OUTPUT(m_OnFutbolCaught, "OnFutbolCaught"),
	DEFINE_THINKFUNC(CatchThink),
	DEFINE_THINKFUNC(CaptureThink),
END_DATADESC()

LINK_ENTITY_TO_CLASS(prop_glass_futbol_spawner, CPropFutbolSpawner);

BEGIN_DATADESC(CPropFutbolSpawner)
	DEFINE_KEYFIELD(m_bHasFutbol, FIELD_BOOLEAN, "StartWithFutbol"),
	DEFINE_OUTPUT(m_OnFutbolSpawned, "OnFutbolSpawned"),
	DEFINE_OUTPUT(m_OnFutbolGrabbed, "OnFutbolGrabbed"),
	DEFINE_INPUTFUNC(FIELD_VOID, "ForceSpawn", InputForceSpawn),
END_DATADESC()

LINK_ENTITY_TO_CLASS(prop_glass_futbol, CPropGlassFutbol);

BEGIN_DATADESC(CPropGlassFutbol)
	DEFINE_KEYFIELD(m_strSpawnerName, FIELD_STRING, "SpawnerName"),
	DEFINE_FIELD(m_hSpawner, FIELD_EHANDLE),
	DEFINE_FIELD(m_Holder, FIELD_INTEGER),
	DEFINE_FIELD(m_vecThrowDirection, FIELD_VECTOR),
	DEFINE_FIELD(m_hLastHeldByPlayer, FIELD_EHANDLE),
	DEFINE_THINKFUNC(ThrownThink),
	DEFINE_THINKFUNC(AnimThink),
END_DATADESC()

void CFutbolCatcher::Spawn()
{
	BaseClass::Spawn();
	GetAttachment("ball", m_vecCatchBoxOrig);

	m_vecCatchBoxMaxs = m_vCatcherBoxHalfDiagonal;
	SetThink(&CFutbolCatcher::CatchThink);
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CatchThink()
{
	Ray_t ray;
	trace_t tr;

	CFilterOnlyGlassFutbol filter(this, GetCollisionGroup());

	ray.Init(m_vecCatchBoxOrig, m_vecCatchBoxOrig, m_vecCatchBoxMins, m_vecCatchBoxMaxs);
	enginetrace->TraceRay(ray, MASK_SOLID, &filter, &tr);

	if (r_visualizetraces.GetBool())
		DebugDrawLine(tr.startpos, tr.endpos, 0xff, 0, 0, true, -1.0f);

	if (!m_bDisableRecaptureOnPlayerGrab)
	{
		CPropGlassFutbol* pGlassFutbol = dynamic_cast<CPropGlassFutbol*>(tr.m_pEnt);
		if (pGlassFutbol)
		{
			if (FUTBOL_HELD_BY_NONE || FUTBOL_HELD_BY_PLAYER)
			{
				CaptureFutbol(pGlassFutbol);
			}
		}
	}
	m_bDisableRecaptureOnPlayerGrab = false;
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CaptureThink()
{
	if (m_hCaughtFutbol)
	{
		SetThink(&CFutbolCatcher::CatchThink);
		m_OnFutbolReleased.FireOutput(this, this);
		m_bDisableRecaptureOnPlayerGrab = true;
	}
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime);
}

void CFutbolCatcher::CaptureFutbol(CPropGlassFutbol* pFutbol)
{
	IPhysicsObject* pPhysicsObject = pFutbol->VPhysicsGetObject();

	if (pFutbol)
	{
		if (pPhysicsObject)
			pPhysicsObject->EnableMotion(false);
		pFutbol->Teleport(&m_vecCatchBoxOrig, &vec3_angle, &vec3_origin, true);
		SetThink(&CFutbolCatcher::CaptureThink);
		pFutbol->SetHolder ( FUTBOL_HELD_BY_CATCHER );
		m_OnFutbolCaught.FireOutput(pFutbol, pFutbol->GetLastPlayerToHold());
	}
}

bool CFilterOnlyGlassFutbol::ShouldHitEntity(IHandleEntity* pServerEntity, int contentsMask)
{
	CBaseEntity* pEnt = static_cast<IServerUnknown*>(pServerEntity)->GetBaseEntity();
	if (pEnt)
	{
		if (FClassnameIs(pEnt, "prop_glass_futbol"))
			return true;
	}
	return false;
}

void CPropFutbolSpawner::Precache()
{
	PrecacheModel("models/props/futbol_dispenser.mdl");
}

void CPropFutbolSpawner::Spawn()
{
	Precache();
	SetModel("models/props/futbol_dispenser.mdl");
	SetSolid(SOLID_VPHYSICS);
	BaseClass::Spawn();
	if (m_bHasFutbol)
		SpawnFutbol();
}

void CPropFutbolSpawner::FutbolDestroyed()
{
	SpawnFutbol();
}

void CPropFutbolSpawner::FutbolGrabbed()
{
	if (m_bHasFutbol)
		m_OnFutbolGrabbed.FireOutput(this, this);
	m_bHasFutbol = false;
}

void CPropFutbolSpawner::InputForceSpawn(inputdata_t& data)
{
	SpawnFutbol();
}

void CPropFutbolSpawner::SpawnFutbol()
{
	CPropGlassFutbol* pFutbol = (CPropGlassFutbol*)CreateEntityByName("prop_glass_futbol");
	if (pFutbol)
	{
		m_OnFutbolSpawned.FireOutput( this, this );

		Vector vecBallSpawnPoint;
		GetAttachment( "ball", vecBallSpawnPoint );
		pFutbol->SetAbsOrigin( vecBallSpawnPoint );
		pFutbol->SetSpawner( this );
		m_bHasFutbol = true;
		DispatchSpawn( pFutbol );
		CaptureFutbol( pFutbol );
	}
}

void CPropGlassFutbol::Precache()
{
	BaseClass::Precache();
	PrecacheModel("models/props/futbol.mdl");
}

void CPropGlassFutbol::Spawn()
{
	Precache();
	KeyValue("model", "models/props/futbol.mdl");

	if (!m_hSpawner)
	{
		CPropFutbolSpawner* pSpawner = dynamic_cast<CPropFutbolSpawner*>( gEntList.FindEntityByName(NULL, m_strSpawnerName) );
		if (pSpawner)
		{
			SetSpawner( pSpawner );
		}
	}
	
	BaseClass::Spawn();
	ResetSequence(LookupSequence("rot"));
	SetContextThink( &CPropGlassFutbol::AnimThink, gpGlobals->curtime + 0.1f, g_szFutbolAnimThinkContext);
}

void CPropGlassFutbol::Event_Killed(const CTakeDamageInfo& info)
{
	if (ClassMatches("trigger_portal_cleanser"))
	{
		CBaseEntity::Event_Killed(info);
	}
	else
	{
		BaseClass::Event_Killed(info);
	}
}

void CPropGlassFutbol::ThrownThink()
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
	
	Vector vecForce = m_vecThrowDirection * sv_futbol_fake_force.GetFloat();

	if (pPhysicsObject)
	{
		pPhysicsObject->SetVelocityInstantaneous(&vec3_origin, NULL);
		pPhysicsObject->ApplyForceCenter(vecForce);
	}
	BaseClass::ApplyAbsVelocityImpulse(vecForce);
	SetContextThink(NULL, -1.0f, g_szFutbolThrownThinkContext);
}

void CPropGlassFutbol::AnimThink()
{
	StudioFrameAdvance();
	SetNextThink(gpGlobals->interval_per_tick + gpGlobals->curtime, g_szFutbolAnimThinkContext);
}

void CPropGlassFutbol::OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason)
{
	Vector forward;

	if (pPhysGunUser)
	{
		BaseClass::OnPhysGunDrop(pPhysGunUser, reason);
		pPhysGunUser->EyeVectors(&forward);		

		CPortal_Player *pPlayer = static_cast<CPortal_Player*>( pPhysGunUser );

		CPortal_Base2D *pPortal = pPlayer->GetHeldObjectPortal();

		if ( pPortal )
		{
			VMatrix matThisToLinked = pPortal->MatrixThisToLinked();
			UTIL_Portal_VectorTransform(matThisToLinked, forward, forward);
		}
				
		m_vecThrowDirection = forward;
		SetContextThink(&CPropGlassFutbol::ThrownThink, gpGlobals->interval_per_tick + gpGlobals->curtime, g_szFutbolThrownThinkContext);
	}
}

void CPropGlassFutbol::OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason)
{
	CPortal_Player* pPortalPlayer = dynamic_cast<CPortal_Player*>(pPhysGunUser);

	if (reason == PICKED_UP_BY_PLAYER && pPortalPlayer && sv_futbol_use_steals_from_holding_player.GetInt())
		pPortalPlayer->SetUseKeyCooldownTime(sv_futbol_use_cooldown_time.GetFloat());
}

void CPropGlassFutbol::SetSpawner(CPropFutbolSpawner* pMySpawner)
{
	pMySpawner = m_hSpawner;
}

void CPropFutbolSocket::Precache()
{
	PrecacheModel("models/props/futbol_socket.mdl");
}

void CPropFutbolSocket::Spawn()
{
	Precache();
	SetModel("models/props/futbol_socket.mdl");
	SetSolid(SOLID_VPHYSICS);
	SetSequence(LookupSequence("hold"));
	BaseClass::Spawn();
}
