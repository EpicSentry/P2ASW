#include "cbase.h"
#include "props.h"
#include "portal_player.h"
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
	void Event_Killed(const CTakeDamageInfo &info);

	//Think functions
	void ThrownThink();
	void AnimThink();

	void OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);
	void OnPhysGunPickup(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason);

	string_t m_strSpawnerName;
	futbol_holder_type_t m_Holder;
	Vector m_vecThrowDirection;
private:
	CHandle<CPortal_Player> m_hLastHeldByPlayer;
};

LINK_ENTITY_TO_CLASS(prop_glass_futbol, CPropGlassFutbol);

BEGIN_DATADESC(CPropGlassFutbol)
DEFINE_THINKFUNC(ThrownThink),
DEFINE_THINKFUNC(AnimThink),
END_DATADESC()

void CPropGlassFutbol::Precache()
{
	BaseClass::Precache();
	PrecacheModel("models/props/futbol.mdl");
}

void CPropGlassFutbol::Spawn()
{
	Precache();
	KeyValue("model", "models/props/futbol.mdl");
	
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

		if ( pPlayer )
		{
			CPortal_Base2D *pPortal = pPlayer->GetHeldObjectPortal();

			if ( pPortal )
			{
				VMatrix matThisToLinked = pPortal->MatrixThisToLinked();
				UTIL_Portal_VectorTransform(matThisToLinked, forward, forward);
			}
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
