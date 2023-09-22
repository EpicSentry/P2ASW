//==== Copyright © 2012, Sandern Corporation, All rights reserved. =========
//
//
//=============================================================================

#include "cbase.h"
#include "ai_baseactor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_infected_swipe_damage( "sk_infected_swipe_damage", "5" );

#define INFECTED_SKIN_COUNT 32
#define	INFECTED_MELEE1_RANGE		100.0f

ConVar	sk_infected_health( "sk_infected_health","20");

//-----------------------------------------------------------------------------
// Custom Activities
//-----------------------------------------------------------------------------
Activity ACT_TERROR_IDLE_NEUTRAL;
Activity ACT_TERROR_IDLE_ALERT;
Activity ACT_TERROR_WALK_NEUTRAL;
Activity ACT_TERROR_RUN_INTENSE;
Activity ACT_TERROR_ATTACK;
Activity ACT_TERROR_JUMP;
Activity ACT_TERROR_JUMP_OVER_GAP;
Activity ACT_TERROR_FALL;
Activity ACT_TERROR_JUMP_LANDING;
Activity ACT_TERROR_JUMP_LANDING_HARD;

//-----------------------------------------------------------------------------
// Animation events
//-----------------------------------------------------------------------------
int AE_FOOTSTEP_RIGHT;
int AE_FOOTSTEP_LEFT;
int AE_ATTACK_HIT;

class CNPC_Infected : public CAI_BaseActor
{
	DECLARE_CLASS( CNPC_Infected, CAI_BaseActor );

public:
	CNPC_Infected();
	~CNPC_Infected();

	//---------------------------------

	void			Precache();
	void			Spawn();
	void			Activate();

	void			SetZombieModel();

	Class_T			Classify();
	
	void			SetupGlobalModelData();
	Activity		NPC_TranslateActivity( Activity baseAct );
	virtual	bool	OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval );

	virtual float	GetIdealSpeed( ) const;
	virtual float	GetSequenceGroundSpeed( CStudioHdr *pStudioHdr, int iSequence );

	virtual void		DeathSound( const CTakeDamageInfo &info );
	virtual void		AlertSound( void );
	virtual void		IdleSound( void );
	virtual void		PainSound( const CTakeDamageInfo &info );
	//virtual void		FearSound( void );

	virtual void	MeleeAttack( float distance, float damage, QAngle &viewPunch, Vector &shove );
	virtual void	HandleAnimEvent( animevent_t *pEvent );

	void			StartTask( const Task_t *pTask );
	void			RunTask( const Task_t *pTask );

	void			RunAttackTask( int task );

public:
	static int gm_nMoveXPoseParam;
	static int gm_nMoveYPoseParam;
	static int gm_nLeanYawPoseParam;
	static int gm_nLeanPitchPoseParam;

private:
	DEFINE_CUSTOM_AI;

	DECLARE_DATADESC();

	int m_iAttackLayer;
};

LINK_ENTITY_TO_CLASS( npc_infected, CNPC_Infected );


BEGIN_DATADESC( CNPC_Infected )
END_DATADESC()

int CNPC_Infected::gm_nMoveXPoseParam = -1;
int CNPC_Infected::gm_nMoveYPoseParam = -1;
int CNPC_Infected::gm_nLeanYawPoseParam = -1;
int CNPC_Infected::gm_nLeanPitchPoseParam = -1;

CNPC_Infected::CNPC_Infected()
{

}

CNPC_Infected::~CNPC_Infected()
{

}

void CNPC_Infected::Precache()
{
#ifndef INFECTED_ALLMODELS
	PrecacheModel( "models/infected/common_male01.mdl" );
#else
	PrecacheModel( "models/infected/common_female_rural01.mdl");
	PrecacheModel( "models/infected/common_worker_male01.mdl");
	PrecacheModel( "models/infected/common_tsaagent_male01.mdl");
	PrecacheModel( "models/infected/common_worker_male01.mdl");
	PrecacheModel( "models/infected/common_surgeon_male01.mdl");
	PrecacheModel( "models/infected/common_police_male01.mdl");
	PrecacheModel( "models/infected/common_surgeon_male01.mdl");
	PrecacheModel( "models/infected/common_patient_male01.mdl");
	PrecacheModel( "models/infected/common_military_male01.mdl");
	PrecacheModel( "models/infected/common_male_suit.mdl");
	PrecacheModel( "models/infected/common_male_rural01.mdl");
	PrecacheModel( "models/infected/common_male_pilot.mdl");
	PrecacheModel( "models/infected/common_male_baggagehandler_01.mdl");
	PrecacheModel( "models/infected/common_female01.mdl");
	PrecacheModel( "models/infected/common_female_nurse01.mdl");

    PrecacheScriptSound("Zombie.Sleeping");
    PrecacheScriptSound("Zombie.Wander");
    PrecacheScriptSound("Zombie.BecomeAlert");
    PrecacheScriptSound("Zombie.Alert");
    PrecacheScriptSound("Zombie.BecomeEnraged");
    PrecacheScriptSound("Zombie.Rage");
    PrecacheScriptSound("Zombie.RageAtVictim");
    PrecacheScriptSound("Zombie.Shoved");
    PrecacheScriptSound("Zombie.Shot");
    PrecacheScriptSound("Zombie.Die");
    PrecacheScriptSound("Zombie.IgniteScream");
    PrecacheScriptSound("Zombie.HeadlessCough");
    PrecacheScriptSound("Zombie.AttackMiss");
    PrecacheScriptSound("Zombie.BulletImpact");
    PrecacheScriptSound("Zombie.ClawScrape");
    PrecacheScriptSound("Zombie.Punch");
    PrecacheScriptSound("MegaMobIncoming");
#endif // INFECTED_ALLMODELS

	BaseClass::Precache();
}

void CNPC_Infected::Spawn()
{
	Precache();

	SetZombieModel();
	BaseClass::Spawn();

	SetHullType( HULL_HUMAN );
	SetHullSizeNormal();
	SetDefaultEyeOffset();
	
	SetNavType( NAV_GROUND );
	m_NPCState = NPC_STATE_NONE;
	
	m_iHealth = m_iMaxHealth = sk_infected_health.GetInt();

	m_flFieldOfView		= 0.2;

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );

	SetupGlobalModelData();
	
	CapabilitiesAdd( bits_CAP_MOVE_GROUND );
	CapabilitiesAdd( bits_CAP_INNATE_MELEE_ATTACK1 );

	NPCInit();
}

void CNPC_Infected::Activate()
{
	BaseClass::Activate();

	SetupGlobalModelData();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Class_T CNPC_Infected::Classify()
{
	return CLASS_ZOMBIE;
}

//---------------------------------------------------------
//---------------------------------------------------------
void CNPC_Infected::SetZombieModel( void )
{
#ifndef INFECTED_ALLMODELS
	SetModel( "models/infected/common_male01.mdl" );
	SetHullType( HULL_HUMAN );
		
	SetBodygroup ( 0, RandomInt (0, 3 ) ); // Head
	SetBodygroup ( 1, RandomInt (0, 5 ) ); // Upper body
	//SetBodygroup ( 2, RandomInt (0, 3 ) ); // Lower body, changes the number of legs cut off
		
	m_nSkin = random->RandomInt( 0, INFECTED_SKIN_COUNT-1 );
#else
	const int numberOfModels = 16;
	int modelIndex = RandomInt(0,numberOfModels-1);

	switch (modelIndex) {
		case 0:
		{
			SetModel( "models/infected/common_male01.mdl" );
			SetHullType( HULL_HUMAN );
		
			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 5 ) );
			SetBodygroup ( 2, RandomInt (0, 3 ) );
		
			m_nSkin = random->RandomInt( 0, INFECTED_SKIN_COUNT-1 );
			break;
		}

		case 1:
		{
			SetModel( "models/infected/common_female_rural01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 2:
		{
			SetModel( "models/infected/common_worker_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}
	
		case 3:
		{
			SetModel( "models/infected/common_tsaagent_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 4:
		{
			SetModel( "models/infected/common_worker_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}
	
		case 5:
		{
			SetModel( "models/infected/common_surgeon_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 6:
		{
			SetModel( "models/infected/common_police_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 7:
		{
			SetModel( "models/infected/common_surgeon_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 8:
		{
			SetModel( "models/infected/common_patient_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 9:
		{
			SetModel( "models/infected/common_military_male01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 10:
		{
			SetModel( "models/infected/common_male_suit.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 11:
		{
			SetModel( "models/infected/common_male_rural01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 12:
		{
			SetModel( "models/infected/common_male_pilot.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 13:
		{
			SetModel( "models/infected/common_male_baggagehandler_01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 14:
		{
			SetModel( "models/infected/common_female01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}

		case 15:
		{
			SetModel( "models/infected/common_female_nurse01.mdl");
			SetHullType( HULL_HUMAN );

			SetBodygroup ( 0, RandomInt (0, 3 ) );
			SetBodygroup ( 1, RandomInt (0, 3 ) );
			SetBodygroup ( 2, RandomInt (0, 1 ) );

			m_nSkin = random->RandomInt( 0, 3 );
			break;
		}
	}
#endif // INFECTED_ALLMODELS
}


void CNPC_Infected::SetupGlobalModelData()
{
	if ( gm_nMoveXPoseParam != -1 )
		return;

	gm_nMoveXPoseParam = LookupPoseParameter( "move_x" );
	gm_nMoveYPoseParam = LookupPoseParameter( "move_y" );
	gm_nLeanYawPoseParam = LookupPoseParameter( "lean_yaw" );
	gm_nLeanPitchPoseParam = LookupPoseParameter( "lean_pitch" );
}

float CNPC_Infected::GetIdealSpeed( ) const
{
	// Ensure navigator will move
	// TODO: Could limit it to move sequences only.
	float speed = BaseClass::GetIdealSpeed();
	if( speed <= 0 ) speed = 1.0f;
	return speed;
}

float CNPC_Infected::GetSequenceGroundSpeed( CStudioHdr *pStudioHdr, int iSequence )
{
	// Ensure navigator will move
	// TODO: Could limit it to move sequences only.
	float speed = BaseClass::GetSequenceGroundSpeed( pStudioHdr, iSequence );
	if( speed <= 0 /*&& GetSequenceActivity( iSequence) == ACT_TERROR_RUN_INTENSE*/ ) speed = 1.0f;
	return speed;
}

Activity CNPC_Infected::NPC_TranslateActivity( Activity baseAct )
{

	if( baseAct == ACT_WALK )
	{
		return ACT_TERROR_WALK_NEUTRAL;
	}
	else if( baseAct == ACT_RUN )
	{
		return ACT_TERROR_RUN_INTENSE;
	}
	else if ( ( baseAct == ACT_IDLE ) )
	{
		return ACT_TERROR_IDLE_ALERT;
	}
	else if ( ( baseAct == ACT_MELEE_ATTACK1 ) )
	{
		return ACT_TERROR_ATTACK;
	}

	return baseAct;
}

bool CNPC_Infected::OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval )
{
	// required movement direction
	float flMoveYaw = UTIL_VecToYaw( move.dir );

	// FIXME: move this up to navigator so that path goals can ignore these overrides.
	Vector dir;
	float flInfluence = GetFacingDirection( dir );
	dir = move.facing * (1 - flInfluence) + dir * flInfluence;
	VectorNormalize( dir );

	// ideal facing direction
	float idealYaw = UTIL_AngleMod( UTIL_VecToYaw( dir ) );
		
	// FIXME: facing has important max velocity issues
	GetMotor()->SetIdealYawAndUpdate( idealYaw );	

	// find movement direction to compensate for not being turned far enough
	float flDiff = UTIL_AngleDiff( flMoveYaw, GetLocalAngles().y );

	// Setup the 9-way blend parameters based on our speed and direction.
	Vector2D vCurMovePose( 0, 0 );

	vCurMovePose.x = cos( DEG2RAD( flDiff ) ) * 1.0f; //flPlaybackRate;
	vCurMovePose.y = -sin( DEG2RAD( flDiff ) ) * 1.0f; //flPlaybackRate;

	SetPoseParameter( gm_nMoveXPoseParam, vCurMovePose.x );
	SetPoseParameter( gm_nMoveYPoseParam, vCurMovePose.y );

	// ==== Update Lean pose parameters
	if ( gm_nLeanYawPoseParam >= 0 )
	{
		float targetLean = GetPoseParameter( gm_nMoveYPoseParam ) * 30.0f;
		float curLean = GetPoseParameter( gm_nLeanYawPoseParam );
		if( curLean < targetLean )
			curLean += MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		else
			curLean -= MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		SetPoseParameter( gm_nLeanYawPoseParam, curLean );
	}

	if( gm_nLeanPitchPoseParam >= 0 )
	{
		float targetLean = GetPoseParameter( gm_nMoveXPoseParam ) * -30.0f;
		float curLean = GetPoseParameter( gm_nLeanPitchPoseParam );
		if( curLean < targetLean )
			curLean += MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		else
			curLean -= MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		SetPoseParameter( gm_nLeanPitchPoseParam, curLean );
	}

	return true;
}

void CNPC_Infected::MeleeAttack( float distance, float damage, QAngle &viewPunch, Vector &shove )
{
	Vector vecForceDir;

	// Always hurt bullseyes for now
	if ( ( GetEnemy() != NULL ) && ( GetEnemy()->Classify() == CLASS_BULLSEYE ) )
	{
		vecForceDir = (GetEnemy()->GetAbsOrigin() - GetAbsOrigin());
		CTakeDamageInfo info( this, this, damage, DMG_SLASH );
		CalculateMeleeDamageForce( &info, vecForceDir, GetEnemy()->GetAbsOrigin() );
		GetEnemy()->TakeDamage( info );
		return;
	}

	CBaseEntity *pHurt = CheckTraceHullAttack( distance, -Vector(16,16,32), Vector(16,16,32), damage, DMG_SLASH, 5.0f );

	if ( pHurt )
	{
		vecForceDir = ( pHurt->WorldSpaceCenter() - WorldSpaceCenter() );

		//FIXME: Until the interaction is setup, kill combine soldiers in one hit -- jdw
		if ( FClassnameIs( pHurt, "npc_combine_s" ) )
		{
			CTakeDamageInfo	dmgInfo( this, this, pHurt->m_iHealth+25, DMG_SLASH );
			CalculateMeleeDamageForce( &dmgInfo, vecForceDir, pHurt->GetAbsOrigin() );
			pHurt->TakeDamage( dmgInfo );
			return;
		}

		CBasePlayer *pPlayer = ToBasePlayer( pHurt );

		if ( pPlayer != NULL )
		{
			//Kick the player angles
			if ( !(pPlayer->GetFlags() & FL_GODMODE ) && pPlayer->GetMoveType() != MOVETYPE_NOCLIP )
			{
				pPlayer->ViewPunch( viewPunch );

				Vector	dir = pHurt->GetAbsOrigin() - GetAbsOrigin();
				VectorNormalize(dir);

				QAngle angles;
				VectorAngles( dir, angles );
				Vector forward, right;
				AngleVectors( angles, &forward, &right, NULL );

				//Push the target back
				pHurt->ApplyAbsVelocityImpulse( - right * shove[1] - forward * shove[0] );
			}
		}

		// Play a random attack hit sound
		EmitSound( "Zombie.Punch" );
	}
	else
	{
		EmitSound( "Zombie.AttackMiss" );
	}
}

void CNPC_Infected::HandleAnimEvent( animevent_t *pEvent )
{
	int nEvent = pEvent->Event();

	if ( nEvent ==  AE_FOOTSTEP_RIGHT )
	{
		EmitSound( "Zombie.FootstepLeft", pEvent->eventtime );
		return;
	}

	if ( nEvent ==  AE_FOOTSTEP_LEFT )
	{
		EmitSound( "Zombie.FootstepRight", pEvent->eventtime );
		return;
	}


	if ( pEvent->Event() == AE_ATTACK_HIT )
	{
		MeleeAttack( INFECTED_MELEE1_RANGE, sk_infected_swipe_damage.GetFloat(), QAngle( 20.0f, 0.0f, -12.0f ), Vector( -250.0f, 1.0f, 1.0f ) );
		return;

	}
	BaseClass::HandleAnimEvent( pEvent );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Infected::PainSound( const CTakeDamageInfo &info )
{
	// We're constantly taking damage when we are on fire. Don't make all those noises!
	if ( IsOnFire() )
	{
		return;
	}

	EmitSound( "Zombie.Pain" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Infected::DeathSound( const CTakeDamageInfo &info ) 
{
	EmitSound( "Zombie.Die" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Infected::AlertSound( void )
{
	EmitSound( "Zombie.Alert" );
}

//-----------------------------------------------------------------------------
// Purpose: Play a random idle sound.
//-----------------------------------------------------------------------------
void CNPC_Infected::IdleSound( void )
{
	EmitSound( "Zombie.Wander" );
}

void CNPC_Infected::StartTask( const Task_t *pTask )
{
	switch( pTask->iTask )
	{
	case TASK_MELEE_ATTACK1:
	{
		SetLastAttackTime( gpGlobals->curtime );
		m_iAttackLayer = AddGesture( ACT_TERROR_ATTACK );
		break;
	}
	default:
		BaseClass::StartTask( pTask );
		break;
	}
}

void CNPC_Infected::RunTask( const Task_t *pTask )
{
	switch( pTask->iTask )
	{
	case TASK_MELEE_ATTACK1:
	{
		RunAttackTask( pTask->iTask );
		break;
	}
	default:
		BaseClass::RunTask( pTask );
		break;
	}
}

void CNPC_Infected::RunAttackTask( int task )
{
	AutoMovement( );

	Vector vecEnemyLKP = GetEnemyLKP();

	// If our enemy was killed, but I'm not done animating, the last known position comes
	// back as the origin and makes the me face the world origin if my attack schedule
	// doesn't break when my enemy dies. (sjb)
	if( vecEnemyLKP != vec3_origin )
	{
		if ( ( task == TASK_RANGE_ATTACK1 || task == TASK_RELOAD ) && 
			 ( CapabilitiesGet() & bits_CAP_AIM_GUN ) && 
			 FInAimCone( vecEnemyLKP ) )
		{
			// Arms will aim, so leave body yaw as is
			GetMotor()->SetIdealYawAndUpdate( GetMotor()->GetIdealYaw(), AI_KEEP_YAW_SPEED );
		}
		else
		{
			GetMotor()->SetIdealYawToTargetAndUpdate( vecEnemyLKP, AI_KEEP_YAW_SPEED );
		}
	}

	CAnimationLayer *pPlayer = GetAnimOverlay( m_iAttackLayer );
	if ( pPlayer->m_bSequenceFinished )
	{
		if ( task == TASK_RELOAD && GetShotRegulator() )
		{
			GetShotRegulator()->Reset( false );
		}

		TaskComplete();
	}
}

//-------------------------------------------------------------------------------------------------
//
// Schedules
//
//-------------------------------------------------------------------------------------------------
AI_BEGIN_CUSTOM_NPC( npc_infected, CNPC_Infected )
	DECLARE_ACTIVITY( ACT_TERROR_IDLE_NEUTRAL )
	DECLARE_ACTIVITY( ACT_TERROR_IDLE_ALERT )
	DECLARE_ACTIVITY( ACT_TERROR_WALK_NEUTRAL )
	DECLARE_ACTIVITY( ACT_TERROR_RUN_INTENSE )
	DECLARE_ACTIVITY( ACT_TERROR_ATTACK )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP_OVER_GAP )
	DECLARE_ACTIVITY( ACT_TERROR_FALL )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP_LANDING )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP_LANDING_HARD )

	DECLARE_ANIMEVENT( AE_FOOTSTEP_RIGHT )
	DECLARE_ANIMEVENT( AE_FOOTSTEP_LEFT )
	DECLARE_ANIMEVENT( AE_ATTACK_HIT )

AI_END_CUSTOM_NPC()
