//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Core of the GlaDOS computer.
//
//=====================================================================================//

#include "cbase.h"
#include "baseentity.h"
#include "te_effect_dispatch.h"	// Sprite effect
#include "props.h"				// CPhysicsProp base class
#include "saverestore_utlvector.h"
#include "tier1/fmtstr.h"

#define GLADOS_CORE_MODEL_NAME "models/npcs/personality_sphere/personality_sphere.mdl" 

static const char *s_pAnimateThinkContext = "Animate";

#define DEFAULT_LOOK_ANINAME			"personality_sphere_testanim"
#define RICHARD_VOICE_NAME				"sphere01"
#define AQUARIUM_VOICE_NAME				"sphere02"

#define DEFAULT_SKIN					0
#define CURIOUS_SKIN					1
#define AGGRESSIVE_SKIN					2
#define	CRAZY_SKIN						3
#define RICHARD_SKIN					1
#define AQUARIUM_SKIN					2

enum SphereEvent_e
{
	SPHERE_EVENT_INFO_TARGET,
	SPHERE_EVENT_WAITING,
	SPHERE_EVENT_HELD,
	SPHERE_EVENT_HELD_PANIC,
	SPHERE_EVENT_DROP,
	SPHERE_EVENT_NOTHELD,
	SPHERE_EVENT_NOTHELD_RELIEVED,
	SPHERE_EVENT_FIRST_PICKUP,
	SPHERE_EVENT_FRUSTRATION,
	SPHERE_EVENT_HURT,
	SPHERE_EVENT_HURT_REPEATEDLY,
	SPHERE_EVENT_VELOCITY_UP_HIGH,
	SPHERE_EVENT_VELOCITY_DOWN_HIGH,
	SPHERE_EVENT_VELOCITY_LATERAL_LOW,
	SPHERE_EVENT_VELOCITY_LATERAL_HIGH,
	SPHERE_EVENT_TOTAL
};

const char *g_LineNames[] =
{
	"InfoTarget",
	"MUTTER",
	"HELD",
	"HELDPANIC",
	"DROP",
	"NOTHELD",
	"UNHELDRELIEF",
	"PICKUPFIRST",
	"FRUSTRATION",
	"PAIN",
	"BASHING",
	"WHOOP",
	"WHOOP",
	"WHOOP",
	"WHOOP",
};

class CPropPersonalitySphere : public CPhysicsProp
{
public:
	DECLARE_CLASS( CPropPersonalitySphere, CPhysicsProp );
	DECLARE_DATADESC();

	CPropPersonalitySphere();
	~CPropPersonalitySphere();

	typedef enum 
	{
		CORETYPE_CURIOUS,
		CORETYPE_AGGRESSIVE,
		CORETYPE_CRAZY,
		CORETYPE_RICHARD,
		CORETYPE_AQUARIUM,
		CORETYPE_NONE,
		CORETYPE_TOTAL,

	} CORETYPE;

	virtual void Spawn( void );
	virtual void Precache( void );

	void RegisterSoundEvent( SphereEvent_e ,int );

	virtual QAngle	PreferredCarryAngles( void ) { return QAngle( 180, -90, 180 ); }
	virtual bool	HasPreferredCarryAnglesForPlayer( CBasePlayer *pPlayer ) { return true; }

	void	InputPanic( inputdata_t &inputdata );
	void	InputStartTalking( inputdata_t &inputdata );

	void	StartTalking ( float flDelay );

	void	CheckForInfoTargets( void );

	void	UpdatePositions( void );

	void	TalkingThink ( void );
	void	AnimateThink ( void );

	void	SetupVOList( void );
	
	void	TrySpeakLine( int line, int priority );

	void	OnPhysGunPickup( CBasePlayer* pPhysGunUser, PhysGunPickup_t reason );
	void	OnPhysGunDrop( CBasePlayer* pPhysGunUser, PhysGunPickup_t reason );

	//void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );
	void	NotifySystemEvent( CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params );
	
	int	ObjectCaps() { return (BaseClass::ObjectCaps() | FCAP_IMPULSE_USE); }

private:
	bool m_bHasEvent[SPHERE_EVENT_TOTAL];
	int m_iEyeballAttachment;
	float m_flBetweenVOPadding;		// Spacing (in seconds) between VOs
	bool m_bFirstPickup;
	bool m_bHeld;
	bool m_bPanicked;

	float m_flFirstNoSkipTime;
	float m_flLineCompletionTime;
	float m_flInfoTargetTime;
	float m_flIdleWaitTime;
	float m_flLastHeldTime;
	float m_flAbuseLevel;

	// Names of sound scripts for this core's personality
	CUtlVector<Vector> m_pastPositions;
	CUtlVector<CUtlVector<int>> m_iLineOrder;

	int m_iCurrentLine;
	int m_iCurrentPriority;
	int m_iPendingLine;
	int m_iPendingPriority;

	string_t	m_iszInfoTargetScriptName;
	string_t	m_iszLastLinePlayed;
	string_t	m_iszDeathSoundScriptName;
	string_t	m_iszLookAnimationName;		// Different animations for each personality
	string_t	m_iszVoiceName;

	CORETYPE m_iCoreType;
};

LINK_ENTITY_TO_CLASS( prop_personality_sphere, CPropPersonalitySphere );

//-----------------------------------------------------------------------------
// Save/load 
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CPropPersonalitySphere )

	DEFINE_FIELD( m_iEyeballAttachment,						FIELD_INTEGER ),
	DEFINE_FIELD( m_iszDeathSoundScriptName,				FIELD_STRING ),
	DEFINE_FIELD( m_iszLookAnimationName,					FIELD_STRING ),
	DEFINE_FIELD( m_bFirstPickup,							FIELD_BOOLEAN ),

	DEFINE_KEYFIELD( m_iCoreType,			FIELD_INTEGER, "CoreType" ),
	DEFINE_KEYFIELD( m_flBetweenVOPadding,  FIELD_FLOAT, "DelayBetweenLines" ),

	DEFINE_INPUTFUNC( FIELD_VOID,			"Panic", InputPanic ),
	DEFINE_INPUTFUNC( FIELD_VOID,			"StartTalking", InputStartTalking ),

	DEFINE_THINKFUNC( TalkingThink ),
	DEFINE_THINKFUNC( AnimateThink ),
	
END_DATADESC()

CPropPersonalitySphere::CPropPersonalitySphere()
{
	m_iszLookAnimationName = m_iszDeathSoundScriptName = NULL_STRING;
	m_flBetweenVOPadding = 2.5f;
	m_bFirstPickup = true;
}

CPropPersonalitySphere::~CPropPersonalitySphere()
{
}

void CPropPersonalitySphere::Spawn( void )
{
	SetupVOList();

	Precache();
	KeyValue( "model", GLADOS_CORE_MODEL_NAME );
	BaseClass::Spawn();

	//Default to 'dropped' animation
	ResetSequence(LookupSequence("drop"));
	SetCycle( 1.0f );

	DisableAutoFade();
	m_iEyeballAttachment = LookupAttachment( "eyeball" );

	SetContextThink( &CPropPersonalitySphere::AnimateThink, gpGlobals->curtime + 0.1f, s_pAnimateThinkContext );
}

void CPropPersonalitySphere::Precache( void )
{
	BaseClass::Precache();

	PrecacheModel( GLADOS_CORE_MODEL_NAME );
}

//-----------------------------------------------------------------------------
// Purpose: Switch to panic think, play panic vo and animations
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::InputPanic( inputdata_t &inputdata )
{
	m_bPanicked = true;
}

//-----------------------------------------------------------------------------
// Purpose: Start playing personality VO list
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::InputStartTalking ( inputdata_t &inputdata )
{
	StartTalking( 0.0f );
}

void CPropPersonalitySphere::StartTalking( float flDelay )
{
	SetThink( &CPropPersonalitySphere::TalkingThink );
	SetNextThink( gpGlobals->curtime + m_flBetweenVOPadding + flDelay );
}

//-----------------------------------------------------------------------------
// Purpose: Start playing personality VO list
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::TalkingThink( void )
{
	// Undone
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::AnimateThink()
{
	StudioFrameAdvance();
	SetContextThink( &CPropPersonalitySphere::AnimateThink, gpGlobals->curtime + 0.1f, s_pAnimateThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: Setup list of lines based on core personality
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::SetupVOList( void )
{
	switch ( m_iCoreType )
	{
	case CORETYPE_CURIOUS:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_nSkin = CURIOUS_SKIN;
			
		}
		break;
	case CORETYPE_AGGRESSIVE:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_nSkin = AGGRESSIVE_SKIN;
		}
		break;
	case CORETYPE_CRAZY:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_nSkin = CRAZY_SKIN;
		}
		break;
	case CORETYPE_RICHARD:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_iszVoiceName = AllocPooledString( RICHARD_VOICE_NAME );
			m_nSkin = RICHARD_SKIN;
			m_flBetweenVOPadding = 0.5;
			m_flIdleWaitTime = -0.5;
		}
	case CORETYPE_AQUARIUM:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_iszVoiceName = AllocPooledString( AQUARIUM_VOICE_NAME );
			m_nSkin = AQUARIUM_SKIN;
			m_flBetweenVOPadding = 0.1;
			m_flIdleWaitTime = -0.1;
		}
		break;
	default:
		{
			m_iszLookAnimationName = AllocPooledString( DEFAULT_LOOK_ANINAME );
			m_nSkin = DEFAULT_SKIN;
		}
		break;
	};

	m_iszDeathSoundScriptName =  AllocPooledString( "Portal.Glados_core.Death" );
}

//-----------------------------------------------------------------------------
// Purpose: Attempt to say something
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::TrySpeakLine( int line, int priority )
{
	if ( !m_bHasEvent[line] )
		return;

	float flTimeSinceNoSkip = m_flFirstNoSkipTime - gpGlobals->curtime;
	if ( flTimeSinceNoSkip > 0.0 && flTimeSinceNoSkip < 0.5 )
	{
		if ( line != SPHERE_EVENT_HURT && priority >= m_iPendingPriority )
		{
			m_iPendingPriority = priority;
			m_iPendingLine = line;
		}
	}

	if ( priority > m_iCurrentPriority || line == SPHERE_EVENT_INFO_TARGET )
	{
		m_flLineCompletionTime = gpGlobals->curtime;
	}

	if ( m_flLineCompletionTime > gpGlobals->curtime )
		return;

	if ( m_flAbuseLevel <= 1.0 || priority > 3 )
	{
		float flAdditionalPadding = 1.5;

		if ( line == SPHERE_EVENT_HURT )
		{
			if ( m_bHasEvent[SPHERE_EVENT_HURT_REPEATEDLY] )
			{
				m_flAbuseLevel += 1.0;
				if ( m_flAbuseLevel > 2.0 )
				{
					line = SPHERE_EVENT_HURT_REPEATEDLY;
				}
				else
				{
					flAdditionalPadding = 0.0;
				}
			}
			else
			{
				flAdditionalPadding = 0.0;
			}
		}
		else if ( line == SPHERE_EVENT_HELD )
		{
			flAdditionalPadding = 0.5;
		}

		if ( m_iCoreType != CORETYPE_AQUARIUM || flAdditionalPadding == 0.5 )
		{
			StopSound( m_iszLastLinePlayed.ToCStr() );
			if ( m_iCoreType == CORETYPE_RICHARD )
			{
				if ( line != SPHERE_EVENT_INFO_TARGET )
				{
					int iIndex = m_iLineOrder[line][0];
					m_iLineOrder[line].FindAndRemove( 0 ); // p2asw: Hard to tell what this was since it's inlined					
					m_iLineOrder[line].AddToTail( m_iLineOrder[line][0] ); // TODO: This makes no sense

					
					CFmtStrN<256> sound( "%s.%s%02d", m_iszVoiceName.ToCStr(), g_LineNames[line], iIndex );
					m_iszLastLinePlayed = AllocPooledString( sound.Access() );
				}
			}
			else
			{
				if ( line != SPHERE_EVENT_INFO_TARGET )
				{
					CFmtStrN<256> sound( "%s.%s", m_iszVoiceName.ToCStr(), g_LineNames[line] );
					m_iszLastLinePlayed = AllocPooledString( sound.Access() );
				}
			}

			EmitSound( m_iszLastLinePlayed.ToCStr() );
			m_iCurrentLine = line;
			m_iCurrentPriority = priority;
			m_iPendingPriority = -1;
			m_iPendingLine = -1;
			
			float flCurDuration = GetSoundDuration( m_iszLastLinePlayed.ToCStr(), GLADOS_CORE_MODEL_NAME );
			m_flFirstNoSkipTime = gpGlobals->curtime + flCurDuration;
			m_flLineCompletionTime = flCurDuration = m_flBetweenVOPadding + gpGlobals->curtime + flAdditionalPadding;

			if ( line == SPHERE_EVENT_HURT || line == SPHERE_EVENT_HURT_REPEATEDLY )
			{
				m_flLineCompletionTime = m_flFirstNoSkipTime;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Cores play a special animation when picked up and dropped
// Input  : pPhysGunUser - player picking up object
//			reason - type of pickup
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::OnPhysGunDrop( CBasePlayer* pPhysGunUser, PhysGunPickup_t reason )
{
	m_bHeld = false;
	TrySpeakLine( SPHERE_EVENT_DROP, 4 );
	m_flLastHeldTime = gpGlobals->curtime;
	BaseClass::OnPhysGunPickup ( pPhysGunUser, reason );
}

//-----------------------------------------------------------------------------
// Purpose: Cores play a special animation when picked up and dropped
// Input  : pPhysGunUser - player picking up object
//			reason - type of pickup
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::OnPhysGunPickup( CBasePlayer* pPhysGunUser, PhysGunPickup_t reason )
{
	if ( m_bFirstPickup )
	{
		float flTalkingDelay = (CORETYPE_CURIOUS == m_iCoreType) ? (2.0f) : (0.0f);
		StartTalking ( flTalkingDelay );
	}

	m_bFirstPickup = false;
	ResetSequence(LookupSequence("turn"));

	// +use always enables motion on these props
	EnableMotion();

	BaseClass::OnPhysGunPickup ( pPhysGunUser, reason );
}

//-----------------------------------------------------------------------------
// Purpose: Purge past positions
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::NotifySystemEvent( CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params )
{
	if ( eventType == NOTIFY_EVENT_TELEPORT )
	{
		m_pastPositions.Purge();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Purge past positions
//-----------------------------------------------------------------------------
void CPropPersonalitySphere::RegisterSoundEvent( SphereEvent_e eventName, int count )
{
	m_bHasEvent[eventName] = true;

	if ( eventName != SPHERE_EVENT_INFO_TARGET )
		return;

	if ( m_iCoreType == CORETYPE_RICHARD )
	{
		for ( int i = 1; i > count; ++i )
		{
			CFmtStrN<256> sound( "%s.%s%02d", m_iszVoiceName.ToCStr(), g_LineNames[eventName], i );
			PrecacheScriptSound( sound.Access() );
			m_iLineOrder[eventName].AddToTail( i );

			int iLine = random->RandomInt( 0, i-1 );

			m_iLineOrder[eventName][i-1] = m_iLineOrder[eventName][iLine];
			m_iLineOrder[eventName][iLine] = i;
		}
	}
	else
	{
		CFmtStrN<256> sound( "%s.%s", m_iszVoiceName.ToCStr(), g_LineNames[eventName] );
		PrecacheScriptSound( sound.Access() );
	}
}