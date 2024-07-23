#include "cbase.h"
#include "prop_monster_box.h"
#include "portal_grabcontroller_shared.h"
#include "trigger_portal_cleanser.h"

const char *s_pAnimateThinkContext = "Animate";


ConVar sv_monster_turret_velocity( "sv_monster_turret_velocity", "100.0f", FCVAR_REPLICATED, "The amount of velocity the monster turret tries to move with." );


BEGIN_DATADESC( CPropMonsterBox )

	DEFINE_FIELD( m_bHeld, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsABox, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsFlying, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsShortcircuit, FIELD_BOOLEAN ),
	
	DEFINE_KEYFIELD( m_bForcedAsBox, FIELD_BOOLEAN, "StartAsBox" ),
	DEFINE_KEYFIELD( m_flBoxSwitchSpeed, FIELD_FLOAT, "BoxSwitchSpeed" ),

	DEFINE_FIELD( m_flPushStrength, FIELD_FLOAT ),
	DEFINE_FIELD( m_nBodyGroups, FIELD_INTEGER ),
	DEFINE_FIELD( m_nDeferredTransform, FIELD_INTEGER ),

	DEFINE_KEYFIELD(m_bAllowSilentDissolve, FIELD_BOOLEAN, "AllowSilentDissolve"),

	DEFINE_INPUTFUNC( FIELD_VOID, "BecomeBox", InputBecomeBox ),
	DEFINE_INPUTFUNC( FIELD_VOID, "BecomeMonster", InputBecomeMonster ),
	DEFINE_INPUTFUNC( FIELD_VOID, "BecomeShortcircuit", InputBecomeShortcircuit ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SilentDissolve", InputSilentDissolve ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Dissolve", InputDissolve ),
	
	DEFINE_OUTPUT( m_OnFizzled, "OnFizzled" ),

	DEFINE_THINKFUNC( AnimateThink ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( prop_monster_box, CPropMonsterBox )

CPropMonsterBox::CPropMonsterBox()
{
	m_nDeferredTransform = 0;
	m_flPushStrength = 1.0;
	m_flBoxSwitchSpeed = 400.0;
	m_bIsShortcircuit = false;
	m_bHeld = false;
	m_bAllowSilentDissolve = false;
}

CPropMonsterBox::~CPropMonsterBox()
{

}

void CPropMonsterBox::Spawn( void )
{
	Precache();

	const char *pszModelName = "models/npcs/monsters/monster_A_box.mdl";
	if (!m_bForcedAsBox)
		pszModelName = "models/npcs/monsters/monster_a.mdl";
	
	SetModel( pszModelName );
	
	CollisionProp()->SetSolid( SOLID_BBOX );
	AddEffects( EF_NOFLASHLIGHT );
	//g_PortalGameStats.Event_CubeSpawn();
	BaseClass::Spawn();
	if ( m_bForcedAsBox )
	{
		ResetSequence( LookupSequence("hermit_idle") );
		SetCycle( 0.0 );
		m_bIsABox = true;
	}
	else
	{
		ResetSequence( LookupSequence("straight01") );
		SetCycle( 0.0 );
		m_bIsABox = false;
	}

	m_nBodyGroups = RandomInt( 0, 15 );
	m_nBodyGroups |= RandomInt( 0, 15 );
	m_nBodyGroups |= RandomInt( 0, 15 );
	

	SetBodygroup( 1, m_nBodyGroups & 1 );
	SetBodygroup( 2, ((m_nBodyGroups >> 1) & 1) != 0 );
	SetBodygroup( 3, ((m_nBodyGroups >> 2) & 1) != 0 );
	SetBodygroup( 4, ((m_nBodyGroups >> 3) & 1) != 0 );
	
	SetMoveType( MOVETYPE_VPHYSICS );
	
	SetContextThink( &CPropMonsterBox::AnimateThink, gpGlobals->curtime + 0.1, s_pAnimateThinkContext );

	if ( VPhysicsGetObject() )
		VPhysicsGetObject()->SetMass( 40.0 );
	
	UpdateObjectCapsCache();
	SetFadeDistance( -1.0, 0.0 );
	SetGlobalFadeScale( 0.0 );
}

void CPropMonsterBox::Precache( void )
{
	BaseClass::Precache();
	PrecacheModel("models/npcs/monsters/monster_a.mdl");
	PrecacheModel("models/npcs/monsters/monster_A_box.mdl");
	PrecacheScriptSound("DoSparkSmaller");
}

void CPropMonsterBox::BecomeBox( bool bForce )
{
	if ( m_bIsShortcircuit )
		return;

	//m_bForcedAsBox |= bForce;
	m_bForcedAsBox = bForce;
	if (m_bIsABox)
	{
		m_nDeferredTransform = 0;
	}
	else if (m_bHeld)
	{
		m_nDeferredTransform = 1;
	}
	else
	{
		m_bIsABox = true;
		if (VPhysicsGetObject())
		{
			Vector vecVelocity;
			Vector vecAngVelocity;

			VPhysicsGetObject()->GetVelocity( &vecVelocity, &vecAngVelocity );
			SetModel("models/npcs/monsters/monster_A_box.mdl");
			VPhysicsDestroyObject();
			CreateVPhysics();

			VPhysicsGetObject()->SetVelocity( &vecVelocity, &vecAngVelocity );
				
			ResetSequence( LookupSequence("hermit_in") );
		}
	}
}

void CPropMonsterBox::BecomeMonster( bool bForce )
{
	if ( m_bIsShortcircuit )
		return;
	
	if ( bForce )
	{
		m_bForcedAsBox = false;
	}
	else if ( m_bForcedAsBox )
	{
		return;
	}

	if ( m_bIsABox )
	{
		if ( m_bHeld )
		{
			m_nDeferredTransform = 2;
		}
		else
		{
			m_bIsABox = false;
			if ( VPhysicsGetObject() )
			{
				Vector vecVelocity;
				Vector vecAngVelocity;
				VPhysicsGetObject()->GetVelocity( &vecVelocity, &vecAngVelocity );
				
				SetModel("models/npcs/monsters/monster_a.mdl");
				VPhysicsDestroyObject();
				CreateVPhysics();

				VPhysicsGetObject()->SetVelocity( &vecVelocity, &vecAngVelocity );
				
				CBaseAnimating::ResetSequence( LookupSequence("hermit_out") );
			}
		}
	}
	else
	{
		m_nDeferredTransform = 0;
	}
}

void CPropMonsterBox::BecomeShortcircuit( void )
{
	if ( VPhysicsGetObject() )
	{
		if ( !m_bIsABox )
		{
			Vector vecVelocity;
			Vector vecAngVelocity;
			VPhysicsGetObject()->GetVelocity( &vecVelocity, &vecAngVelocity );
			SetModel("models/npcs/monsters/monster_A_box.mdl");
			VPhysicsDestroyObject();
			CreateVPhysics();
			
			VPhysicsGetObject()->SetVelocity( &vecVelocity, &vecAngVelocity );
		}

		// NOTE: This was UTIL_GetLocalPlayer, but this won't work well in multiplayer and will force the player
		// to drop what they're grabbing even if they aren't holding this! Making a deviation here - Wonderland_War
		
		CBasePlayer *pPlayer = GetPlayerHoldingEntity( this );
		if ( pPlayer )
			pPlayer->ForceDropOfCarriedPhysObjects( this );
		
		ResetSequence( LookupSequence( "shortcircuit" ) );
		EmitSound( "DoSparkSmaller" );
		m_bIsABox = true;
		m_bIsShortcircuit = true;
	}
}

void CPropMonsterBox::OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason )
{
	m_bHeld = false;
	BaseClass::OnPhysGunDrop( pPhysGunUser, reason );
	if ( !m_bIsShortcircuit )
	{
		if ( m_nDeferredTransform )
		{
			if ( m_nDeferredTransform == 1 )
			{
				BecomeBox( true );
			}
			else if ( m_nDeferredTransform == 2 )
			{				
				BecomeMonster( true );
			}
			m_nDeferredTransform = 0;
		}
		if ( !m_bForcedAsBox )
		{
			ResetSequence( LookupSequence( "hermit_out" ) );
		}
	}
}


void CPropMonsterBox::HandleAnimEvent( animevent_t *pEvent )
{
	int event_type;

	if ((pEvent->type & AE_TYPE_NEWEVENTSYSTEM) != 0)
		event_type = pEvent->_event_lowword;
	else
		event_type = pEvent->_event_highword;

	if ( event_type != 1100 || m_bHeld )
	{
		BaseClass::HandleAnimEvent(pEvent);
	}
	else
	{
		if ( VPhysicsGetObject() )
		{
			Vector vecForward;
			Vector vecUp;
			GetVectors( &vecForward, NULL, &vecUp );
			if ( vecUp.z >= 0.1 )
			{
				Vector vecAbsEnd;
				vecAbsEnd.x = GetAbsOrigin().x - ( vecUp.x * 32.0 );
				vecAbsEnd.y = GetAbsOrigin().y - ( vecUp.y * 32.0 );		

				trace_t tr;
				UTIL_TraceLine( GetAbsOrigin(), vecAbsEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);
				if (tr.fraction != 1.0)
				{
					Vector angImpulse = RandomAngularImpulse( -0.5, 0.5 );
					
					vecAbsEnd.z = RandomFloat(0.7, 1.0);

					Vector vecVelocity;

					//vecVelocity.x = vecForward.x + flRandom;
					//vecVelocity.y = vecForward.y + flRandom;
					//vecVelocity.z = vecForward.z + vecAbsEnd.z;

					float flVel = sv_monster_turret_velocity.GetFloat();
					
					vecVelocity.x = ( (vecForward.x + RandomFloat(-0.2, 0.2)) * flVel ) * m_flPushStrength;
					vecVelocity.y = ( (vecForward.y + RandomFloat(-0.2, 0.2)) * flVel ) * m_flPushStrength;
					vecVelocity.z = ( (vecForward.z + vecAbsEnd.z) * flVel ) * m_flPushStrength;
					SetGroundEntity( NULL );
					VPhysicsGetObject()->Wake();

					VPhysicsGetObject()->SetVelocityInstantaneous( &vec3_origin, &angImpulse);
					VPhysicsGetObject()->ApplyForceCenter( vecVelocity );
					ApplyAbsVelocityImpulse( vecVelocity );
				}
			}
		}
	}
}

void CPropMonsterBox::AnimateThink( void )
{
	StudioFrameAdvance();
	DispatchAnimEvents( this );

	SetContextThink( &CPropMonsterBox::AnimateThink, gpGlobals->curtime + 0.1, s_pAnimateThinkContext );

	if ( m_bIsShortcircuit )
	{
		if ( m_bSequenceFinished )
		{
			CBaseAnimating::ResetSequence( LookupSequence( "shortcircuit" ) );
		}

		return;
	}

	Vector vecVelocity = vec3_origin;
	if (VPhysicsGetObject())
		VPhysicsGetObject()->GetVelocity( &vecVelocity, NULL );

	if ( vecVelocity.LengthSqr() <= Sqr( m_flBoxSwitchSpeed ) )
	{
		if ( !m_bHeld && m_bIsFlying && !m_bForcedAsBox )
		{
			trace_t tr;
			UTIL_TraceHull( GetAbsOrigin(), GetAbsOrigin(), CollisionProp()->OBBMins(), CollisionProp()->OBBMaxs(), MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
			if ( tr.startsolid || tr.allsolid )
			{
				Vector vecUp;
				GetVectors( NULL, NULL, &vecUp );
				Vector angImpulse = RandomAngularImpulse( -0.5, 0.5 );

				//vecImpulse.x = vecUp.x + v32;
				//vecImpulse.y = vecUp.y + v32;
				//vecImpulse.z = vecUp.z + RandomFloat( 1.7, 2.0 );

				Vector vecImpulse;
				vecImpulse.x = (vecUp.x + RandomFloat( -0.2, 0.2 ) ) * sv_monster_turret_velocity.GetFloat();
				vecImpulse.y = (vecUp.y + RandomFloat( -0.2, 0.2 ) ) * sv_monster_turret_velocity.GetFloat();
				vecImpulse.z = (vecUp.z + RandomFloat( 1.7, 2.0 ) ) * sv_monster_turret_velocity.GetFloat();
					
				SetGroundEntity( NULL );
					
				VPhysicsGetObject()->Wake();
				VPhysicsGetObject()->SetVelocityInstantaneous( &vec3_origin, &angImpulse );
				VPhysicsGetObject()->ApplyForceCenter( vecImpulse );
					
				ApplyAbsVelocityImpulse( vecImpulse );
			}
			else
			{
				m_bIsFlying = true;
				BecomeMonster( false );
			}
		}
	}
	else if ( !m_bHeld && !m_bIsFlying )
	{
		m_bIsFlying = true;
		BecomeBox( false );
	}
	if ( m_bSequenceFinished )
	{
		if ( m_bHeld || m_bForcedAsBox )
		{
			ResetSequence( LookupSequence( "hermit_idle" ) );
		}
		else
		{
			Vector vecUp;
			GetVectors( NULL, NULL, &vecUp );
			if ( vecUp.z >= 0.6 )
			{
				Vector vecImpulse = GetAbsOrigin() - ( vecUp * 32.0 );
				trace_t tr;
				UTIL_TraceLine( GetAbsOrigin(), vecImpulse, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
				if ( tr.fraction == 1.0 )
				{
					ResetSequence( LookupSequence( "intheair" ) );
				}
				else
				{
					int iRandom1 = RandomInt( 0, 2 );
					if ( iRandom1 )
					{
						int iRandom2 = iRandom1 - 1;
						if ( iRandom2 )
						{
							if ( iRandom2 == 1 )
							{
								CBaseAnimating::ResetSequence( LookupSequence( "straight03" ) );
								m_flPushStrength = 1.1;
							}
						}
						else
						{
							CBaseAnimating::ResetSequence( LookupSequence( "straight02" ) );
							m_flPushStrength = 0.7;
						}
					}
					else
					{
						CBaseAnimating::ResetSequence( LookupSequence( "straight01" ) );
						m_flPushStrength = 1.0;
					}
				}
			}
			else
			{
				CBaseAnimating::ResetSequence( LookupSequence( "fallover_idle" ) );
			}
		}
	}
}

void CPropMonsterBox::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Linux has this function but windows doesn't, use BaseClass code for now.
#if 0
	if (pActivator)
	{
		if ((*((unsigned __int8(__cdecl **)(CBaseEntity *))pActivator->_vptr_IHandleEntity + 86))(pActivator))
			(*((void(__cdecl **)(CBaseEntity *, CPropMonsterBox *const, int))pActivator->_vptr_IHandleEntity + 443))(
			pActivator,
			this,
			1);
	}
#else
	BaseClass::Use( pActivator, pCaller, useType, value );
#endif
}

void CPropMonsterBox::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	if ( !m_bHeld )
	{
		m_bHeld = true;
		if (!m_bIsShortcircuit)
		{
			ResetSequence( LookupSequence( "hermit_in" ) );
		}
		BaseClass::OnPhysGunPickup( pPhysGunUser, reason );
	}
}

int CPropMonsterBox::ObjectCaps( void )
{
	return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS;
}

void CPropMonsterBox::OnFizzled( void )
{
	m_OnFizzled.FireOutput( this, this );
}

void CPropMonsterBox::InputBecomeBox( inputdata_t &inputdata )
{
	BecomeBox( true );
}

void CPropMonsterBox::InputBecomeMonster( inputdata_t &inputdata )
{
	BecomeMonster( true );
}

void CPropMonsterBox::InputBecomeShortcircuit( inputdata_t &inputdata )
{
	BecomeShortcircuit();
}

void CPropMonsterBox::InputDissolve( inputdata_t &inputdata )
{
	CTriggerPortalCleanser::FizzleBaseAnimating( NULL, this );
}

void CPropMonsterBox::InputSilentDissolve( inputdata_t &inputdata )
{
	if ( m_bAllowSilentDissolve )
	{
		OnFizzled();
		UTIL_Remove(this);
	}
}
