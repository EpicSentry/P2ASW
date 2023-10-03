#include "prop_glass_futbol.h"
#include "tier0/memdbgon.h"

ConVar sv_futbol_fake_force("sv_futbol_fake_force", "500", FCVAR_NONE);
ConVar sv_futbol_force_players_to_catch("sv_futbol_force_players_to_catch", "1", FCVAR_NONE);
ConVar sv_futbol_use_cooldown_time("sv_futbol_use_cooldown_time", "0.7", FCVAR_NONE);
ConVar sv_futbol_use_steals_from_holding_player("sv_futbol_use_steals_from_holding_player", "1", FCVAR_NONE);

static const char* g_szFutbolAnimThinkContext = "FutbolAnimateThinkContext";
static const char* g_szFutbolThrownThinkContext = "FutbolThrownThinkContext";

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
