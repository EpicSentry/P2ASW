#include "cbase.h"
#include "prop_exploding_futbol.h"
#include "particle_parse.h"
#include "takedamageinfo.h"
#include "explode.h"
#include "physobj.h"
#include "tier0/memdbgon.h"

ConVar exploding_futbol_explosion_debug("exploding_futbol_explosion_debug", "0", FCVAR_CHEAT, "Debug the explosion of the exploding futbol.");
ConVar exploding_futbol_use_cooldown_time("exploding_futbol_use_cooldown_time", "0.7f", FCVAR_CHEAT, "The cooldown time for the use key after the player picks up the futbol.");
ConVar exploding_futbol_explosion_radius("exploding_futbol_explosion_radius", "200", FCVAR_CHEAT, "The radius of the explosion for the exploding futbol.");
ConVar exploding_futbol_explosion_magnitude("exploding_futbol_explosion_magnitude", "0", FCVAR_CHEAT, "The magnitude of the explosion for the exploding futbol.");
ConVar exploding_futbol_explosion_damage("exploding_futbol_explosion_damage", "25.0f", FCVAR_CHEAT, "The damage of the explosion for the exploding futbol.");
ConVar exploding_futbol_explosion_damage_falloff("exploding_futbol_explosion_damage_falloff", "0.75f", FCVAR_CHEAT, "The percentage of damage taken at the edge of the explosion.");
ConVar exploding_futbol_start_color("exploding_futbol_start_color", "255 255 255 255", FCVAR_CHEAT, "The starting color of the exploding futbol.");
ConVar exploding_futbol_end_color("exploding_futbol_end_color", "255 106 0 255", FCVAR_CHEAT, "The ending color of the exploding futbol, before it starts the final explode sequence.");
ConVar exploding_futbol_flash_start_color("exploding_futbol_flash_start_color", "255 255 0 255", FCVAR_CHEAT, "The start color for the futbol flashing before it explodes.");
ConVar exploding_futbol_flash_end_color("exploding_futbol_flash_end_color", "255 0 0 255", FCVAR_CHEAT, "The final color of the exploding futbol, right before it explodes.");
ConVar exploding_futbol_flash_start_time("exploding_futbol_flash_start_time", "3.0f", FCVAR_CHEAT, "The time before the futbol explodes when it start to flash.");
ConVar exploding_futbol_flash_duration("exploding_futbol_flash_duration", "1.0f", FCVAR_CHEAT, "The flash duration of the exploding futbol, right before it explodes.");
ConVar exploding_futbol_hit_breakables("exploding_futbol_hit_breakables", "1", FCVAR_CHEAT, "If the exploding futbol should hit breakable entities.");
ConVar exploding_futbol_explode_on_fizzle("exploding_futbol_explode_on_fizzle", "0", FCVAR_CHEAT, "If the exploding futbol should explode when it fizzles.");
ConVar exploding_futbol_physics_punt_player("exploding_futbol_physics_punt_player", "1", FCVAR_CHEAT, "Physically perturb the player when the explosion hits them");
ConVar exploding_futbol_phys_mag("exploding_futbol_phys_mag", "100", FCVAR_CHEAT, "Magnitude of physics force for an exploding futbol");
ConVar exploding_futbol_phys_rad("exploding_futbol_phys_rad", "45", FCVAR_CHEAT, "Magnitude of physics force for an exploding futbol");
ConVar sv_futbol_funnel_max_correct("sv_futbol_funnel_max_correct", "128.f", FCVAR_DEVELOPMENTONLY, "Max distance to move our hit-target if there\'s a portal nearby it");

LINK_ENTITY_TO_CLASS(prop_exploding_futbol, CPropExplodingFutbol);

BEGIN_DATADESC(CPropExplodingFutbol)
	DEFINE_KEYFIELD(m_strSpawnerName, FIELD_STRING, "SpawnerName"),
	DEFINE_KEYFIELD(m_bShouldRespawn, FIELD_BOOLEAN, "ShouldRespawn"),
	DEFINE_KEYFIELD(m_bExplodeOnTouch, FIELD_BOOLEAN, "ExplodeOnTouch"),
	
	DEFINE_FIELD(m_Holder, FIELD_INTEGER),
	DEFINE_FIELD(m_hLastHeldByPlayer, FIELD_EHANDLE),
	DEFINE_FIELD(m_bTimerActive, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flExplosionTimer, FIELD_FLOAT),
	DEFINE_FIELD(m_flTotalTimer, FIELD_FLOAT),
	DEFINE_FIELD(m_flLastTickTime, FIELD_TIME),
	DEFINE_FIELD(m_flLastTimerSoundTime, FIELD_TIME),
	DEFINE_FIELD(m_flLastFlashTime, FIELD_TIME),

	DEFINE_THINKFUNC(AnimThink),
	DEFINE_THINKFUNC(TimerThink),
	DEFINE_THINKFUNC(KillThink),

END_DATADESC()

CPropExplodingFutbol::CPropExplodingFutbol()
{
	m_Holder = EXPLODING_FUTBOL_HELD_BY_NONE;
	m_bExplodeOnTouch = true;
	m_bTimerActive = false;
	m_flExplosionTimer = 0.0f;
	m_flTotalTimer = 0.0f;
	m_flLastTickTime = 0.0f;
	m_flLastTimerSoundTime = 0.0f;
	m_flLastTimerSoundTime = 0.0f;
}

void CPropExplodingFutbol::Precache()
{
	BaseClass::Precache();
	PrecacheModel("models/npcs/personality_sphere_angry.mdl");
	
	PrecacheScriptSound("NPC_FloorTurret.DeployingKlaxon");
	PrecacheScriptSound("Portal.room1_TickTock");
	PrecacheScriptSound("EnergyBall.Explosion");
	
	PrecacheParticleSystem("bomb_trail");
}

void CPropExplodingFutbol::Spawn()
{
	SetModelName(AllocPooledString("models/npcs/personality_sphere_angry.mdl"));
	ResetSequence(LookupSequence("rot"));
	SetContextThink(&CPropExplodingFutbol::AnimThink, gpGlobals->curtime + 0.1f, g_szExplodingFutbolAnimThinkContext);
	CPhysicsProp::Spawn();
	DispatchParticleEffect("bomb_trail", PATTACH_ABSORIGIN_FOLLOW);
}

void CPropExplodingFutbol::Event_Killed(const CTakeDamageInfo& info)
{
	BaseClass::Event_Killed(info);
}

void CPropExplodingFutbol::KillThink()
{
	KillFutbol();
}

void CPropExplodingFutbol::AnimThink()
{
	StudioFrameAdvance();
	SetNextThink( gpGlobals->curtime + gpGlobals->interval_per_tick, g_szExplodingFutbolAnimThinkContext);
}

void CPropExplodingFutbol::TimerThink()
{
	if (m_bTimerActive)
	{
		if (m_flExplosionTimer <= exploding_futbol_flash_start_time.GetFloat())
		{
			EmitSound("NPC_FloorTurret.DeployingKlaxon");
			if ((gpGlobals->curtime - m_flLastFlashTime) > exploding_futbol_flash_duration.GetFloat())
				m_flLastFlashTime = gpGlobals->curtime;
		}
		
		if ((gpGlobals->curtime - m_flLastTimerSoundTime) > exploding_futbol_flash_duration.GetFloat())
		{
			EmitSound("Portal.room1_TickTock");
			m_flLastTimerSoundTime = gpGlobals->curtime;
		}

		if (m_flExplosionTimer <= 0.0f)
		{
			EmitSound("EnergyBall.Explosion");
			KillFutbol();
			StopFutbolTimer();
		}
		if (m_bTimerActive)
		{
			SetNextThink(gpGlobals->curtime + gpGlobals->interval_per_tick, g_szExplodingFutbolTimerThinkContext);
		}
	}
}

void CPropExplodingFutbol::ActivateFutbolTimer(float flTimer)
{
	m_flTotalTimer = flTimer;
	m_flExplosionTimer = flTimer;

	if (flTimer > 0.0f)
	{
		m_bTimerActive = true;
		m_flLastTickTime = gpGlobals->curtime;
		m_flLastTimerSoundTime = gpGlobals->curtime - 1.0f;
		m_flLastFlashTime = gpGlobals->curtime;

		SetRenderColor(exploding_futbol_start_color.GetBool(), exploding_futbol_start_color.GetBool(), exploding_futbol_start_color.GetBool());

		SetContextThink(&CPropExplodingFutbol::TimerThink, gpGlobals->curtime + gpGlobals->interval_per_tick, g_szExplodingFutbolTimerThinkContext);
	}
}

void CPropExplodingFutbol::StopFutbolTimer()
{
	m_bTimerActive = false;
	SetRenderColor(exploding_futbol_start_color.GetBool(), exploding_futbol_start_color.GetBool(), exploding_futbol_start_color.GetBool());
}

void CPropExplodingFutbol::OnPhysGunDrop(CBasePlayer* pPhysGunUser, PhysGunDrop_t reason)
{
	if (pPhysGunUser)
	{
		BaseClass::OnPhysGunDrop(pPhysGunUser, reason);
	}
}

int CPropExplodingFutbol::OnTakeDamage(const CTakeDamageInfo& info)
{
	if (!m_bExplodeOnTouch)
		return 0;
	if (m_Holder == EXPLODING_FUTBOL_HELD_BY_SPAWNER || EXPLODING_FUTBOL_HELD_BY_CATCHER)
		return 0;
	else
		return BaseClass::OnTakeDamage(info);
}

void CPropExplodingFutbol::OnFizzled()
{
	BaseClass::OnFizzled();
	if (exploding_futbol_explode_on_fizzle.GetBool())
		ExplodeFutbol();
}

void CPropExplodingFutbol::VPhysicsCollision(int index, gamevcollisionevent_t* pEvent)
{
	if (m_bExplodeOnTouch && m_Holder == EXPLODING_FUTBOL_HELD_BY_NONE)
		SetContextThink(&CPropExplodingFutbol::KillThink, gpGlobals->interval_per_tick + gpGlobals->curtime, g_szExplodingFutbolKillThinkContext);
}

void CPropExplodingFutbol::VPhysicsUpdate(IPhysicsObject* pPhysics)
{
	BaseClass::VPhysicsUpdate(pPhysics);
}

void CPropExplodingFutbol::KillFutbol()
{
	CTakeDamageInfo info(this, this, GetHealth(), DMG_BLAST);

	info.SetDamagePosition(GetAbsOrigin());
	info.SetDamageForce(Vector (500, 500, 500));

	CPropExplodingFutbol::Event_Killed(info);
}

void CPropExplodingFutbol::DestroyFutbol(bool bExplode)
{
	if (bExplode)
		ExplodeFutbol();
}

void CPropExplodingFutbol::ExplodeFutbol()
{
	ExplosionCreate(GetAbsOrigin(), GetAbsAngles(), GetOwnerEntity(), exploding_futbol_explosion_magnitude.GetInt(), exploding_futbol_explosion_radius.GetFloat(), SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS | SF_ENVEXPLOSION_NOSMOKE, exploding_futbol_explosion_damage.GetFloat(), this);

	if (exploding_futbol_physics_punt_player.GetBool())
	{
		CreatePhysExplosion(GetAbsOrigin(), exploding_futbol_phys_mag.GetFloat(), exploding_futbol_phys_rad.GetFloat(), NULL_STRING, 25.0f, 31);
	}
}
