//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for Portal.
//
//=============================================================================//

#include "cbase.h"
#include "portal_player.h"
#include "globalstate.h"
#include "trains.h"
#include "game.h"
#include "portal_player_shared.h"
#include "predicted_viewmodel.h"
#include "in_buttons.h"
#include "portal_gamerules.h"
#include "portal_mp_gamerules.h"
#include "weapon_portalgun.h"
#include "paint/weapon_paintgun.h"
#include "player_pickup_controller.h"
#include "KeyValues.h"
#include "team.h"
#include "eventqueue.h"
#include "weapon_portalbase.h"
#include "engine/IEngineSound.h"
#include "ai_basenpc.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "prop_portal_shared.h"
#include "player_pickup.h"	// for player pickup code
#include "vphysics/player_controller.h"
#include "datacache/imdlcache.h"
#include "bone_setup.h"
#include "portal_gamestats.h"
#include "physicsshadowclone.h"
#include "physics_prop_ragdoll.h"
#include "soundenvelope.h"
#include "ai_speech.h"		// For expressors, vcd playing
#include "sceneentity.h"	// has the VCD precache function
#include "sendprop_priorities.h"
#include "prop_weightedcube.h"

// Max mass the player can lift with +use
#define PORTAL_PLAYER_MAX_LIFT_MASS 85
#define PORTAL_PLAYER_MAX_LIFT_SIZE 128

extern CBaseEntity	*g_pLastSpawn;

extern void respawn(CBaseEntity *pEdict, bool fCopyCorpse);

//static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

PRECACHE_REGISTER_BEGIN(GLOBAL, PortalPlayerModelPrecache)
PRECACHE(MODEL, "models/player/chell/player.mdl");
PRECACHE_REGISTER_END()

ConVar mp_server_player_team( "mp_server_player_team", "0", FCVAR_DEVELOPMENTONLY );
ConVar mp_wait_for_other_player_timeout( "mp_wait_for_other_player_timeout", "100", FCVAR_CHEAT, "Maximum time that we wait in the transition loading screen for the other player." );
ConVar mp_wait_for_other_player_notconnecting_timeout( "mp_wait_for_other_player_notconnecting_timeout", "10", FCVAR_CHEAT, "Maximum time that we wait in the transition loading screen after we fully loaded for partner to start loading." );
ConVar mp_dev_wait_for_other_player( "mp_dev_wait_for_other_player", "1", FCVAR_DEVELOPMENTONLY, "Force waiting for the other player." );

ConVar sv_portal_coop_ping_cooldown_time( "sv_portal_coop_ping_cooldown_time", "0.25", FCVAR_CHEAT, "Time (in seconds) between coop pings", true, 0.1f, false, 60.0f );
ConVar sv_portal_coop_ping_indicator_show_to_all_players( "sv_portal_coop_ping_indicator_show_to_all_players", "0" );
extern ConVar sv_player_funnel_gimme_dot;
ConVar sv_zoom_stop_movement_threashold("sv_zoom_stop_movement_threashold", "4.0", FCVAR_REPLICATED, "Move command amount before breaking player out of toggle zoom." );
ConVar sv_zoom_stop_time_threashold("sv_zoom_stop_time_threashold", "5.0", FCVAR_REPLICATED, "Time amount before breaking player out of toggle zoom." );
extern ConVar sv_player_funnel_into_portals;

extern void PaintPowerPickup( int colorIndex, CBasePlayer *pPlayer );

//=================================================================================
//
// Ragdoll Entity
//
class CPortalRagdoll : public CBaseAnimatingOverlay, public CDefaultPlayerPickupVPhysics
{
public:

	DECLARE_CLASS( CPortalRagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPortalRagdoll()
	{
		m_hPlayer.Set( NULL );
		m_vecRagdollOrigin.Init();
		m_vecRagdollVelocity.Init();
	}

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};

LINK_ENTITY_TO_CLASS( portal_ragdoll, CPortalRagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CPortalRagdoll, DT_PortalRagdoll )
SendPropVector( SENDINFO(m_vecRagdollOrigin), -1,  SPROP_COORD ),
SendPropEHandle( SENDINFO( m_hPlayer ) ),
SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
SendPropInt		( SENDINFO(m_nForceBone), 8, 0 ),
SendPropVector	( SENDINFO(m_vecForce), -1, SPROP_NOSCALE ),
SendPropVector( SENDINFO( m_vecRagdollVelocity ) ),
END_SEND_TABLE()


BEGIN_DATADESC( CPortalRagdoll )

	DEFINE_FIELD( m_vecRagdollOrigin, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_hPlayer, FIELD_EHANDLE ),
	DEFINE_FIELD( m_vecRagdollVelocity, FIELD_VECTOR ),

END_DATADESC()



extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );

// specific to the local player
BEGIN_SEND_TABLE_NOBASE( CPortal_Player, DT_PortalLocalPlayerExclusive )
	// send a hi-res origin and view offset to the local player for use in prediction
	SendPropVectorXY(SENDINFO(m_vecOrigin),               -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT, SendProxy_OriginXY, SENDPROP_LOCALPLAYER_ORIGINXY_PRIORITY ),
	SendPropFloat   (SENDINFO_VECTORELEM(m_vecOrigin, 2), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT, SendProxy_OriginZ, SENDPROP_LOCALPLAYER_ORIGINZ_PRIORITY ),
	SendPropVector	(SENDINFO(m_vecViewOffset), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT ),

	// FIXME: - Wonderland_War
	/*
	SendPropQAngles( SENDINFO( m_vecCarriedObjectAngles ) ),
	SendPropVector( SENDINFO( m_vecCarriedObject_CurPosToTargetPos )  ),
	SendPropQAngles( SENDINFO( m_vecCarriedObject_CurAngToTargetAng ) ),
	//a message buffer for entity teleportations that's guaranteed to be in sync with the post-teleport updates for said entities
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_EntityPortalledNetworkMessages ), CPortal_Player::MAX_ENTITY_PORTALLED_NETWORK_MESSAGES, SendPropDataTable( NULL, 0, &REFERENCE_SEND_TABLE( DT_EntityPortalledNetworkMessage ) ) ),
	SendPropInt( SENDINFO( m_iEntityPortalledNetworkMessageCount ) ),
	*/
END_SEND_TABLE()

// all players except the local player
BEGIN_SEND_TABLE_NOBASE( CPortal_Player, DT_PortalNonLocalPlayerExclusive )
	// send a lo-res origin and view offset to other players
	// send a lo-res origin to other players
	SendPropVectorXY( SENDINFO( m_vecOrigin ), 				 CELL_BASEENTITY_ORIGIN_CELL_BITS, SPROP_CELL_COORD_LOWPRECISION, 0.0f, HIGH_DEFAULT, CBaseEntity::SendProxy_CellOriginXY, SENDPROP_NONLOCALPLAYER_ORIGINXY_PRIORITY ),
	SendPropFloat   ( SENDINFO_VECTORELEM( m_vecOrigin, 2 ), CELL_BASEENTITY_ORIGIN_CELL_BITS, SPROP_CELL_COORD_LOWPRECISION, 0.0f, HIGH_DEFAULT, CBaseEntity::SendProxy_CellOriginZ, SENDPROP_NONLOCALPLAYER_ORIGINZ_PRIORITY ),
	SendPropFloat	(SENDINFO_VECTORELEM(m_vecViewOffset, 0), 10, SPROP_CHANGES_OFTEN, -128.0, 128.0f),
	SendPropFloat	(SENDINFO_VECTORELEM(m_vecViewOffset, 1), 10, SPROP_CHANGES_OFTEN, -128.0, 128.0f),
	SendPropFloat	(SENDINFO_VECTORELEM(m_vecViewOffset, 2), 10, SPROP_CHANGES_OFTEN, -128.0, 128.0f),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CPortalPlayerShared, DT_PortalPlayerShared )
	SendPropInt( SENDINFO( m_nPlayerCond ), PORTAL_COND_LAST, (SPROP_UNSIGNED|SPROP_CHANGES_OFTEN) ),
END_SEND_TABLE()


LINK_ENTITY_TO_CLASS( player, CPortal_Player );

IMPLEMENT_SERVERCLASS_ST(CPortal_Player, DT_Portal_Player)
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseAnimating", "m_nNewSequenceParity" ),
	SendPropExclude( "DT_BaseAnimating", "m_nResetEventsParity" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	SendPropExclude( "DT_BaseFlex", "m_viewtarget" ),
	SendPropExclude( "DT_BaseFlex", "m_flexWeight" ),
	SendPropExclude( "DT_BaseFlex", "m_blinktoggle" ),

	// portal_playeranimstate and clientside animation takes care of these on the client
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),


	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11, SPROP_CHANGES_OFTEN ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11, SPROP_CHANGES_OFTEN ),
	SendPropEHandle( SENDINFO( m_hRagdoll ) ),
	SendPropInt( SENDINFO( m_iSpawnInterpCounter), 4 ),
	SendPropBool( SENDINFO( m_bHeldObjectOnOppositeSideOfPortal) ),
	SendPropEHandle( SENDINFO( m_hHeldObjectPortal ) ),
	SendPropBool( SENDINFO( m_bPitchReorientation ) ),
	SendPropEHandle( SENDINFO( m_hPortalEnvironment ) ),
	SendPropEHandle( SENDINFO( m_hSurroundingLiquidPortal ) ),

	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),

	// Shared info
	SendPropDataTable( SENDINFO_DT( m_Shared ), &REFERENCE_SEND_TABLE( DT_PortalPlayerShared ) ),

	SendPropFloat( SENDINFO( m_flHullHeight ) ),
	SendPropFloat( SENDINFO( m_flMotionBlurAmount ) ),

	SendPropBool( SENDINFO( m_bUseVMGrab ) ),
	SendPropBool( SENDINFO( m_bUsingVMGrabState ) ),
	SendPropBool( SENDINFO( m_bIsHoldingSomething ) ),
	SendPropEHandle( SENDINFO( m_hAttachedObject ) ),
	SendPropEHandle( SENDINFO( m_hHeldObjectPortal ) ),

END_SEND_TABLE()

BEGIN_DATADESC( CPortal_Player )

	DEFINE_SOUNDPATCH( m_pWooshSound ),
	DEFINE_SOUNDPATCH( m_pGrabSound ),

	DEFINE_FIELD( m_bHeldObjectOnOppositeSideOfPortal, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_hHeldObjectPortal, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bIntersectingPortalPlane, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bStuckOnPortalCollisionObject, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fTimeLastHurt, FIELD_TIME ),
	DEFINE_FIELD( m_StatsThisLevel.iNumPortalsPlaced, FIELD_INTEGER ),
	DEFINE_FIELD( m_StatsThisLevel.iNumStepsTaken, FIELD_INTEGER ),
	DEFINE_FIELD( m_StatsThisLevel.fNumSecondsTaken, FIELD_FLOAT ),
	DEFINE_FIELD( m_fTimeLastNumSecondsUpdate, FIELD_TIME ),
	DEFINE_FIELD( m_iNumCamerasDetatched, FIELD_INTEGER ),
	DEFINE_FIELD( m_bPitchReorientation, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsRegenerating, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fNeuroToxinDamageTime, FIELD_TIME ),
	DEFINE_FIELD( m_hPortalEnvironment, FIELD_EHANDLE ),
	DEFINE_FIELD( m_vecTotalBulletForce, FIELD_VECTOR ),
	DEFINE_FIELD( m_bSilentDropAndPickup, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_hRagdoll, FIELD_EHANDLE ),
	DEFINE_FIELD( m_angEyeAngles, FIELD_VECTOR ),
	DEFINE_FIELD( m_qPrePortalledViewAngles, FIELD_VECTOR ),
	DEFINE_FIELD( m_bFixEyeAnglesFromPortalling, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_matLastPortalled, FIELD_VMATRIX_WORLDSPACE ),
	DEFINE_FIELD( m_vWorldSpaceCenterHolder, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_hSurroundingLiquidPortal, FIELD_EHANDLE ),
	DEFINE_FIELD( m_flHullHeight, FIELD_FLOAT ),
	DEFINE_FIELD( m_flMotionBlurAmount, FIELD_FLOAT ),
	DEFINE_FIELD( m_hGrabbedEntity, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hPortalThroughWhichGrabOccured, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bForcingDrop, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bUseVMGrab, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bUsingVMGrabState, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flUseKeyStartTime, FIELD_TIME ),
	DEFINE_FIELD( m_flAutoGrabLockOutTime, FIELD_TIME ),
	DEFINE_FIELD( m_hAttachedObject, FIELD_EHANDLE ),
	DEFINE_FIELD( m_ForcedGrabController, FIELD_INTEGER ),
	DEFINE_FIELD( m_flUseKeyCooldownTime, FIELD_TIME ),
	
	DEFINE_EMBEDDED( m_PortalLocal ),
	
END_DATADESC()

BEGIN_ENT_SCRIPTDESC( CPortal_Player, CBasePlayer , "Player" )
	//DEFINE_SCRIPTFUNC( IncWheatleyMonitorDestructionCount, "Set number of wheatley monitors destroyed by the player." )
	//DEFINE_SCRIPTFUNC( GetWheatleyMonitorDestructionCount, "Get number of wheatley monitors destroyed by the player." )
	DEFINE_SCRIPTFUNC( TurnOffPotatos, "Turns Off the Potatos material light" )
	DEFINE_SCRIPTFUNC( TurnOnPotatos, "Turns On the Potatos material light" )
END_SCRIPTDESC();

extern ConVar sv_regeneration_wait_time;

const char *g_pszChellModel = "models/player/chell/player.mdl";
const char *g_pszPlayerModel = g_pszChellModel;


#define MAX_COMBINE_MODELS 4
#define MODEL_CHANGE_INTERVAL 5.0f
#define TEAM_CHANGE_INTERVAL 5.0f

#define PORTALPLAYER_PHYSDAMAGE_SCALE 4.0f

extern ConVar sv_turbophysics;

//----------------------------------------------------
// Player Physics Shadow
//----------------------------------------------------
#define VPHYS_MAX_DISTANCE		2.0
#define VPHYS_MAX_VEL			10
#define VPHYS_MAX_DISTSQR		(VPHYS_MAX_DISTANCE*VPHYS_MAX_DISTANCE)
#define VPHYS_MAX_VELSQR		(VPHYS_MAX_VEL*VPHYS_MAX_VEL)


extern float IntervalDistance( float x, float x0, float x1 );

//disable 'this' : used in base member initializer list
#pragma warning( disable : 4355 )

CPortal_Player::CPortal_Player()
	: m_vInputVector( 0.0f, 0.0f, 0.0f ),
	m_flCachedJumpPowerTime( -FLT_MAX ),
	m_flSpeedDecelerationTime( 0.0f ),
	m_bJumpWasPressedWhenForced( false ),
	m_flPredictedJumpTime( 0.f ),
	m_bWantsToSwapGuns( false ),
	m_bSendSwapProximityFailEvent( false ),
	m_flMotionBlurAmount( -1.0f ),
	m_bIsFullyConnected( false ),
	m_pGrabSound( NULL )
{
	// Taunt code
	m_Shared.Init(this);
	m_Shared.m_flTauntRemoveTime = 0.0f;

	m_PlayerAnimState = CreatePortalPlayerAnimState( this );

	UseClientSideAnimation();

	m_angEyeAngles.Init();

	m_iLastWeaponFireUsercmd = 0;

	m_iSpawnInterpCounter = 0;

	m_bHeldObjectOnOppositeSideOfPortal = false;
	m_hHeldObjectPortal = NULL;

	m_bIntersectingPortalPlane = false;

	m_bPitchReorientation = false;

	m_bSilentDropAndPickup = false;

	m_ForcedGrabController = FORCE_GRAB_CONTROLLER_DEFAULT;
	m_hGrabbedEntity = NULL;

	m_flUseKeyCooldownTime = 0.0f;
}

CPortal_Player::~CPortal_Player( void )
{
	ClearSceneEvents( NULL, true );

	if ( m_PlayerAnimState )
		m_PlayerAnimState->Release();

	CPortalRagdoll *pRagdoll = dynamic_cast<CPortalRagdoll*>( m_hRagdoll.Get() );	
	if( pRagdoll )
	{
		UTIL_Remove( pRagdoll );
	}
}

void CPortal_Player::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}

void CPortal_Player::Precache( void )
{
	BaseClass::Precache();

	PrecacheScriptSound( "PortalPlayer.EnterPortal" );
	PrecacheScriptSound( "PortalPlayer.ExitPortal" );

	PrecacheScriptSound( "PortalPlayer.Woosh" );
	PrecacheScriptSound( "PortalPlayer.FallRecover" );
	
	PrecacheScriptSound( "PortalPlayer.ObjectUse" );
	PrecacheScriptSound( "PortalPlayer.UseDeny" );

	PrecacheScriptSound( "PortalPlayer.ObjectUseNoGun" );
	PrecacheScriptSound( "PortalPlayer.UseDenyNoGun" );

	PrecacheModel ( "sprites/glow01.vmt" );

	//Precache Citizen models
	PrecacheModel( g_pszPlayerModel );
	PrecacheModel( g_pszChellModel );
	
	// paint effect
	PrecacheParticleSystem( "boomer_vomit_screeneffect" );
	PrecacheParticleSystem( "boomer_vomit_survivor" );

	PrecacheScriptSound( "NPC_Citizen.die" );
}

void CPortal_Player::CreateSounds()
{
	if ( !m_pWooshSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CPASAttenuationFilter filter( this );

		m_pWooshSound = controller.SoundCreate( filter, entindex(), "PortalPlayer.Woosh" );
		controller.Play( m_pWooshSound, 0, 100 );
	}
}

void CPortal_Player::StopLoopingSounds()
{
	if ( m_pWooshSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pWooshSound );
		m_pWooshSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}

void CPortal_Player::GiveAllItems( void )
{
	EquipSuit();

	CBasePlayer::GiveAmmo( 255,	"Pistol");
	CBasePlayer::GiveAmmo( 32,	"357" );

	CBasePlayer::GiveAmmo( 255,	"AR2" );
	CBasePlayer::GiveAmmo( 3,	"AR2AltFire" );
	CBasePlayer::GiveAmmo( 255,	"SMG1");
	CBasePlayer::GiveAmmo( 3,	"smg1_grenade");

	CBasePlayer::GiveAmmo( 255,	"Buckshot");
	CBasePlayer::GiveAmmo( 16,	"XBowBolt" );

	CBasePlayer::GiveAmmo( 3,	"rpg_round");
	CBasePlayer::GiveAmmo( 6,	"grenade" );

	GiveNamedItem( "weapon_crowbar" );
	GiveNamedItem( "weapon_physcannon" );

	GiveNamedItem( "weapon_pistol" );
	GiveNamedItem( "weapon_357" );

	GiveNamedItem( "weapon_smg1" );
	GiveNamedItem( "weapon_ar2" );

	GiveNamedItem( "weapon_shotgun" );
	GiveNamedItem( "weapon_crossbow" );

	GiveNamedItem( "weapon_rpg" );
	GiveNamedItem( "weapon_frag" );

	GiveNamedItem( "weapon_bugbait" );

	//GiveNamedItem( "weapon_physcannon" );
	CWeaponPortalgun *pPortalGun = static_cast<CWeaponPortalgun*>( GiveNamedItem( "weapon_portalgun" ) );

	if ( !pPortalGun )
	{
		pPortalGun = static_cast<CWeaponPortalgun*>( Weapon_OwnsThisType( "weapon_portalgun" ) );
	}

	if ( pPortalGun )
	{
		pPortalGun->SetCanFirePortal1();
		pPortalGun->SetCanFirePortal2();
	}
}

void CPortal_Player::GiveDefaultItems( void )
{
	castable_string_t st( "suit_no_sprint" );
	GlobalEntity_SetState( st, GLOBAL_OFF );
}


//-----------------------------------------------------------------------------
// Purpose: Sets  specific defaults.
//-----------------------------------------------------------------------------
void CPortal_Player::Spawn(void)
{
	Precache();
	SetPlayerModel();

	BaseClass::Spawn();

	CreateSounds();

	pl.deadflag = false;
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	RemoveEffects( EF_NODRAW );
	StopObserverMode();

	GiveDefaultItems();

	m_nRenderFX = kRenderNormal;

	m_Local.m_iHideHUD = 0;

	AddFlag(FL_ONGROUND); // set the player on the ground at the start of the round.

	m_impactEnergyScale = PORTALPLAYER_PHYSDAMAGE_SCALE;

	RemoveFlag( FL_FROZEN );

	m_iSpawnInterpCounter = (m_iSpawnInterpCounter + 1) % 8;

	m_Local.m_bDucked = false;

	SetPlayerUnderwater(false);

#ifdef PORTAL_MP
	PickTeam();
#endif
}

void CPortal_Player::Activate( void )
{
	BaseClass::Activate();
	m_fTimeLastNumSecondsUpdate = gpGlobals->curtime;
}

void CPortal_Player::NotifySystemEvent(CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params )
{
	// On teleport, we send event for tracking fling achievements
	if ( eventType == NOTIFY_EVENT_TELEPORT )
	{
		CProp_Portal *pEnteredPortal = dynamic_cast<CProp_Portal*>( pNotify );
		IGameEvent *event = gameeventmanager->CreateEvent( "portal_player_portaled" );
		if ( event )
		{
			event->SetInt( "userid", GetUserID() );
			event->SetBool( "portal2", pEnteredPortal->m_bIsPortal2 );
			gameeventmanager->FireEvent( event );
		}
	}

	BaseClass::NotifySystemEvent( pNotify, eventType, params );
}

void CPortal_Player::OnRestore( void )
{
	BaseClass::OnRestore();
}

//bool CPortal_Player::StartObserverMode( int mode )
//{
//	//Do nothing.
//
//	return false;
//}

bool CPortal_Player::ValidatePlayerModel( const char *pModel )
{
	if ( !Q_stricmp( g_pszPlayerModel, pModel ) )
	{
		return true;
	}

	if ( !Q_stricmp( g_pszChellModel, pModel ) )
	{
		return true;
	}

	return false;
}

void CPortal_Player::SetPlayerModel( void )
{
	const char *szModelName = NULL;
	const char *pszCurrentModelName = modelinfo->GetModelName( GetModel());

	// TODO(Preston): Fix me!
	//szModelName = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ), "cl_playermodel" );
	szModelName = g_pszChellModel;

	if ( ValidatePlayerModel( szModelName ) == false )
	{
		char szReturnString[512];

		if ( ValidatePlayerModel( pszCurrentModelName ) == false )
		{
			pszCurrentModelName = g_pszPlayerModel;
		}

		Q_snprintf( szReturnString, sizeof (szReturnString ), "cl_playermodel %s\n", pszCurrentModelName );
		engine->ClientCommand ( edict(), szReturnString );

		szModelName = pszCurrentModelName;
	}

	int modelIndex = modelinfo->GetModelIndex( szModelName );

	if ( modelIndex == -1 )
	{
		szModelName = g_pszPlayerModel;

		char szReturnString[512];

		Q_snprintf( szReturnString, sizeof (szReturnString ), "cl_playermodel %s\n", szModelName );
		engine->ClientCommand ( edict(), szReturnString );
	}

	SetModel( szModelName );
}


bool CPortal_Player::Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex )
{
	bool bRet = BaseClass::Weapon_Switch( pWeapon, viewmodelindex );

	return bRet;
}


void CPortal_Player::PreThink( void )
{
	QAngle vOldAngles = GetLocalAngles();
	QAngle vTempAngles = GetLocalAngles();

	vTempAngles = EyeAngles();

	if ( vTempAngles[PITCH] > 180.0f )
	{
		vTempAngles[PITCH] -= 360.0f;
	}

	SetLocalAngles( vTempAngles );

	BaseClass::PreThink();

	if( (m_afButtonPressed & IN_JUMP) )
	{
		Jump();	
	}
	
	if ( m_afButtonPressed & IN_GRENADE1 )
	{
		if ( !m_PortalLocal.m_bZoomedIn )
		{
			ZoomIn();
		}
	}
	else if ( m_afButtonPressed & IN_GRENADE2 )
	{
		if ( m_PortalLocal.m_bZoomedIn )
		{
			ZoomOut();
		}
	}
	else if ( m_afButtonPressed & IN_ZOOM )
	{
		if ( !m_PortalLocal.m_bZoomedIn )
		{
			ZoomIn();
		}
		else
		{
			ZoomOut();
		}
	}

	UpdateVMGrab( m_hAttachedObject );

	//Reset bullet force accumulator, only lasts one frame
	m_vecTotalBulletForce = vec3_origin;

	SetLocalAngles( vOldAngles );
	
	// Cache the velocity before impact
	if( HASPAINTMAP )
		m_PortalLocal.m_vPreUpdateVelocity = GetAbsVelocity();
	
	// Update the painted power
	UpdatePaintedPower();

	// Fade the input scale back in if we lost some
	UpdateAirInputScaleFadeIn();

	// Attempt to resize the hull if there's a pending hull resize
	TryToChangeCollisionBounds( m_PortalLocal.m_CachedStandHullMinAttempt,
								m_PortalLocal.m_CachedStandHullMaxAttempt,
								m_PortalLocal.m_CachedDuckHullMinAttempt,
								m_PortalLocal.m_CachedDuckHullMaxAttempt );

	// reset remote view
	if ( !m_Shared.InCond( PORTAL_COND_TAUNTING ) && m_bTauntRemoteViewFOVFixup )
	{
		m_bTauntRemoteViewFOVFixup = false;
		SetFOV( this, 0, 0.0f, 0 );
	}
	
	if( m_hAttachedObject && !m_pGrabSound )
	{
		CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();
		CPASAttenuationFilter filter( this );
		char const* soundName = GetActivePortalWeapon() ? "PortalPlayer.ObjectUse" : "PortalPlayer.ObjectUseNoGun";
		m_pGrabSound = controller.SoundCreate( filter, entindex(), soundName );
		controller.Play( m_pGrabSound, VOL_NORM, PITCH_NORM );
	}

	if( !m_hAttachedObject && m_pGrabSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		controller.Shutdown( m_pGrabSound );
		controller.SoundDestroy( m_pGrabSound );
		m_pGrabSound = NULL;
	}
}

void CPortal_Player::PostThink( void )
{
	BaseClass::PostThink();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );

	// Regenerate heath after 3 seconds
	if ( IsAlive() && GetHealth() < GetMaxHealth() )
	{
		// Color to overlay on the screen while the player is taking damage
		color32 hurtScreenOverlay = {64,0,0,64};

		if ( gpGlobals->curtime > m_fTimeLastHurt + sv_regeneration_wait_time.GetFloat() )
		{
			TakeHealth( 1, DMG_GENERIC );
			m_bIsRegenerating = true;

			if ( GetHealth() >= GetMaxHealth() )
			{
				m_bIsRegenerating = false;
			}
		}
		else
		{
			m_bIsRegenerating = false;
			UTIL_ScreenFade( this, hurtScreenOverlay, 1.0f, 0.1f, FFADE_IN|FFADE_PURGE );
		}
	}

	UpdatePortalPlaneSounds();
	UpdateWooshSounds();

	m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
	UpdateSecondsTaken();

	// Try to fix the player if they're stuck
	if ( m_bStuckOnPortalCollisionObject )
	{
		Vector vForward = ((CProp_Portal*)m_hPortalEnvironment.Get())->m_vPrevForward;
		Vector vNewPos = GetAbsOrigin() + vForward * gpGlobals->frametime * -1000.0f;
		Teleport( &vNewPos, NULL, &vForward );
		m_bStuckOnPortalCollisionObject = false;
	}
	
	// Try to update our crosshair
	m_bIsHoldingSomething = IsHoldingEntity( NULL );

}

void CPortal_Player::PlayerDeathThink(void)
{
	float flForward;

	SetNextThink( gpGlobals->curtime + 0.1f );

	if (GetFlags() & FL_ONGROUND)
	{
		flForward = GetAbsVelocity().Length() - 20;
		if (flForward <= 0)
		{
			SetAbsVelocity( vec3_origin );
		}
		else
		{
			Vector vecNewVelocity = GetAbsVelocity();
			VectorNormalize( vecNewVelocity );
			vecNewVelocity *= flForward;
			SetAbsVelocity( vecNewVelocity );
		}
	}

	if ( HasWeapons() )
	{
		// we drop the guns here because weapons that have an area effect and can kill their user
		// will sometimes crash coming back from CBasePlayer::Killed() if they kill their owner because the
		// player class sometimes is freed. It's safer to manipulate the weapons once we know
		// we aren't calling into any of their code anymore through the player pointer.
		PackDeadPlayerItems();
	}

	if (GetModelIndex() && (!IsSequenceFinished()) && (m_lifeState == LIFE_DYING))
	{
		StudioFrameAdvance( );

		m_iRespawnFrames++;
		if ( m_iRespawnFrames < 60 )  // animations should be no longer than this
			return;
	}

	if (m_lifeState == LIFE_DYING)
		m_lifeState = LIFE_DEAD;

	StopAnimation();

	AddEffects( EF_NOINTERP );
	m_flPlaybackRate = 0.0;

	int fAnyButtonDown = (m_nButtons & ~IN_SCORE);

	// Strip out the duck key from this check if it's toggled
	if ( (fAnyButtonDown & IN_DUCK) && GetToggledDuckState())
	{
		fAnyButtonDown &= ~IN_DUCK;
	}

	// wait for all buttons released
	if ( m_lifeState == LIFE_DEAD )
	{
		if ( fAnyButtonDown || gpGlobals->curtime < m_flDeathTime + DEATH_ANIMATION_TIME )
			return;

		if ( g_pGameRules->FPlayerCanRespawn( this ) )
		{
			m_lifeState = LIFE_RESPAWNABLE;
		}

		return;
	}

	// if the player has been dead for one second longer than allowed by forcerespawn, 
	// forcerespawn isn't on. Send the player off to an intermission camera until they 
	// choose to respawn.
	if ( g_pGameRules->IsMultiplayer() && ( gpGlobals->curtime > (m_flDeathTime + DEATH_ANIMATION_TIME) ) && !IsObserver() )
	{
		// go to dead camera. 
		StartObserverMode( m_iObserverLastMode );
	}

	// wait for any button down,  or mp_forcerespawn is set and the respawn time is up
	if (!fAnyButtonDown 
		&& !( g_pGameRules->IsMultiplayer() && forcerespawn.GetInt() > 0 && (gpGlobals->curtime > (m_flDeathTime + 5))) )
		return;

	m_nButtons = 0;
	m_iRespawnFrames = 0;

	//Msg( "Respawn\n");

	respawn( this, !IsObserver() );// don't copy a corpse if we're in deathcam.
	SetNextThink( TICK_NEVER_THINK );
}

void CPortal_Player::UpdatePortalPlaneSounds( void )
{
	CProp_Portal *pPortal = m_hPortalEnvironment;
	if ( pPortal && pPortal->m_bActivated )
	{
		Vector vVelocity;
		GetVelocity( &vVelocity, NULL );

		if ( !vVelocity.IsZero() )
		{
			Vector vMin, vMax;
			CollisionProp()->WorldSpaceAABB( &vMin, &vMax );

			Vector vEarCenter = ( vMax + vMin ) / 2.0f;
			Vector vDiagonal = vMax - vMin;

			if ( !m_bIntersectingPortalPlane )
			{
				vDiagonal *= 0.25f;

				if ( UTIL_IsBoxIntersectingPortal( vEarCenter, vDiagonal, pPortal ) )
				{
					m_bIntersectingPortalPlane = true;

					CPASAttenuationFilter filter( this );
					CSoundParameters params;
					if ( GetParametersForSound( "PortalPlayer.EnterPortal", params, NULL ) )
					{
						EmitSound_t ep( params );
						ep.m_nPitch = 80.0f + vVelocity.Length() * 0.03f;
						ep.m_flVolume = MIN( 0.3f + vVelocity.Length() * 0.00075f, 1.0f );

						EmitSound( filter, entindex(), ep );
					}
				}
			}
			else
			{
				vDiagonal *= 0.30f;

				if ( !UTIL_IsBoxIntersectingPortal( vEarCenter, vDiagonal, pPortal ) )
				{
					m_bIntersectingPortalPlane = false;

					CPASAttenuationFilter filter( this );
					CSoundParameters params;
					if ( GetParametersForSound( "PortalPlayer.ExitPortal", params, NULL ) )
					{
						EmitSound_t ep( params );
						ep.m_nPitch = 80.0f + vVelocity.Length() * 0.03f;
						ep.m_flVolume = MIN( 0.3f + vVelocity.Length() * 0.00075f, 1.0f );

						EmitSound( filter, entindex(), ep );
					}
				}
			}
		}
	}
	else if ( m_bIntersectingPortalPlane )
	{
		m_bIntersectingPortalPlane = false;

		CPASAttenuationFilter filter( this );
		CSoundParameters params;
		if ( GetParametersForSound( "PortalPlayer.ExitPortal", params, NULL ) )
		{
			EmitSound_t ep( params );
			Vector vVelocity;
			GetVelocity( &vVelocity, NULL );
			ep.m_nPitch = 80.0f + vVelocity.Length() * 0.03f;
			ep.m_flVolume = MIN( 0.3f + vVelocity.Length() * 0.00075f, 1.0f );

			EmitSound( filter, entindex(), ep );
		}
	}
}

void CPortal_Player::UpdateWooshSounds( void )
{
	if ( m_pWooshSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		float fWooshVolume = GetAbsVelocity().Length() - MIN_FLING_SPEED;

		if ( fWooshVolume < 0.0f )
		{
			controller.SoundChangeVolume( m_pWooshSound, 0.0f, 0.1f );
			return;
		}

		fWooshVolume /= 2000.0f;
		if ( fWooshVolume > 1.0f )
			fWooshVolume = 1.0f;

		controller.SoundChangeVolume( m_pWooshSound, fWooshVolume, 0.1f );
		//		controller.SoundChangePitch( m_pWooshSound, fWooshVolume + 0.5f, 0.1f );
	}
}

void CPortal_Player::FireBullets ( const FireBulletsInfo_t &info )
{
	NoteWeaponFired();

	BaseClass::FireBullets( info );
}

void CPortal_Player::NoteWeaponFired( void )
{
	Assert( m_pCurrentCommand );
	if( m_pCurrentCommand )
	{
		m_iLastWeaponFireUsercmd = m_pCurrentCommand->command_number;
	}
}

extern ConVar sv_maxunlag;

bool CPortal_Player::WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const
{
	// No need to lag compensate at all if we're not attacking in this command and
	// we haven't attacked recently.
	if ( !( pCmd->buttons & IN_ATTACK ) && (pCmd->command_number - m_iLastWeaponFireUsercmd > 5) )
		return false;

	// If this entity hasn't been transmitted to us and acked, then don't bother lag compensating it.
	if ( pEntityTransmitBits && !pEntityTransmitBits->Get( pPlayer->entindex() ) )
		return false;

	const Vector &vMyOrigin = GetAbsOrigin();
	const Vector &vHisOrigin = pPlayer->GetAbsOrigin();

	// get max distance player could have moved within max lag compensation time, 
	// multiply by 1.5 to to avoid "dead zones"  (sqrt(2) would be the exact value)
	float maxDistance = 1.5 * pPlayer->MaxSpeed() * sv_maxunlag.GetFloat();

	// If the player is within this distance, lag compensate them in case they're running past us.
	if ( vHisOrigin.DistTo( vMyOrigin ) < maxDistance )
		return true;

	// If their origin is not within a 45 degree cone in front of us, no need to lag compensate.
	Vector vForward;
	AngleVectors( pCmd->viewangles, &vForward );

	Vector vDiff = vHisOrigin - vMyOrigin;
	VectorNormalize( vDiff );

	float flCosAngle = 0.707107f;	// 45 degree angle
	if ( vForward.Dot( vDiff ) < flCosAngle )
		return false;

	return true;
}


void CPortal_Player::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	m_PlayerAnimState->DoAnimationEvent( event, nData );
}

//-----------------------------------------------------------------------------
// Purpose: Override setup bones so that is uses the render angles from
//			the Portal animation state to setup the hitboxes.
//-----------------------------------------------------------------------------
void CPortal_Player::SetupBones( matrix3x4_t *pBoneToWorld, int boneMask )
{
	VPROF_BUDGET( "CBaseAnimating::SetupBones", VPROF_BUDGETGROUP_SERVER_ANIM );

	// Set the mdl cache semaphore.
	MDLCACHE_CRITICAL_SECTION();

	// Get the studio header.
	Assert( GetModelPtr() );
	CStudioHdr *pStudioHdr = GetModelPtr( );

	Vector pos[MAXSTUDIOBONES];
	Quaternion q[MAXSTUDIOBONES];

	// Adjust hit boxes based on IK driven offset.
	Vector adjOrigin = GetAbsOrigin() + Vector( 0, 0, m_flEstIkOffset );

	// FIXME: pass this into Studio_BuildMatrices to skip transforms
	CBoneBitList boneComputed;
	if ( m_pIk )
	{
		m_iIKCounter++;
		m_pIk->Init( pStudioHdr, GetAbsAngles(), adjOrigin, gpGlobals->curtime, m_iIKCounter, boneMask );
		GetSkeleton( pStudioHdr, pos, (QuaternionAligned*)q, boneMask );

		m_pIk->UpdateTargets( pos, q, (matrix3x4a_t*)pBoneToWorld, boneComputed );
		CalculateIKLocks( gpGlobals->curtime );
		m_pIk->SolveDependencies( pos, q, (matrix3x4a_t*)pBoneToWorld, boneComputed );
	}
	else
	{
		GetSkeleton( pStudioHdr, pos, (QuaternionAligned*)q, boneMask );
	}

	CBaseAnimating *pParent = dynamic_cast< CBaseAnimating* >( GetMoveParent() );
	if ( pParent )
	{
		// We're doing bone merging, so do special stuff here.
		CBoneCache *pParentCache = pParent->GetBoneCache();
		if ( pParentCache )
		{
			BuildMatricesWithBoneMerge( 
				pStudioHdr, 
				m_PlayerAnimState->GetRenderAngles(),
				adjOrigin, 
				pos, 
				q, 
				pBoneToWorld, 
				pParent, 
				pParentCache );

			return;
		}
	}

	Studio_BuildMatrices( 
		pStudioHdr, 
		m_PlayerAnimState->GetRenderAngles(),
		adjOrigin, 
		pos, 
		q, 
		-1,
		1.0f,
		(matrix3x4a_t*)pBoneToWorld,
		boneMask );
}


// Set the activity based on an event or current state
void CPortal_Player::SetAnimation( PLAYER_ANIM playerAnim )
{
	return;
}

//-----------------------------------------------------------------------------

extern int	gEvilImpulse101;
//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CPortal_Player::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

	// Can I have this weapon type?
	if ( !IsAllowedToPickupWeapons() )
		return false;

	if ( pOwner || !Weapon_CanUse( pWeapon ) || !g_pGameRules->CanHavePlayerItem( this, pWeapon ) )
	{
		if ( gEvilImpulse101 )
		{
			UTIL_Remove( pWeapon );
		}
		return false;
	}

	// Don't let the player fetch weapons through walls (use MASK_SOLID so that you can't pickup through windows)
	if( !pWeapon->FVisible( this, MASK_SOLID ) && !(GetFlags() & FL_NOTARGET) )
	{
		return false;
	}

	CWeaponPortalgun *pPickupPortalgun = dynamic_cast<CWeaponPortalgun*>( pWeapon );

	bool bOwnsWeaponAlready = !!Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType());

	if ( bOwnsWeaponAlready == true ) 
	{
		// If we picked up a second portal gun set the bool to alow secondary fire
		if ( pPickupPortalgun )
		{
			CWeaponPortalgun *pPortalGun = static_cast<CWeaponPortalgun*>( Weapon_OwnsThisType( pWeapon->GetClassname() ) );

			if ( pPickupPortalgun->CanFirePortal1() )
				pPortalGun->SetCanFirePortal1();

			if ( pPickupPortalgun->CanFirePortal2() )
				pPortalGun->SetCanFirePortal2();

			UTIL_Remove( pWeapon );
			return true;
		}

		//If we have room for the ammo, then "take" the weapon too.
		if ( Weapon_EquipAmmoOnly( pWeapon ) )
		{
			pWeapon->CheckRespawn();

			UTIL_Remove( pWeapon );
			return true;
		}
		else
		{
			return false;
		}
	}

	pWeapon->CheckRespawn();
	Weapon_Equip( pWeapon );

	// If we're holding and object before picking up portalgun, drop it
	if ( pPickupPortalgun )
	{
		ForceDropOfCarriedPhysObjects( GetPlayerHeldEntity( this ) );
	}

	return true;
}

void CPortal_Player::ShutdownUseEntity( void )
{
	ShutdownPickupController( m_hUseEntity );
}

const Vector& CPortal_Player::WorldSpaceCenter( ) const
{
	m_vWorldSpaceCenterHolder = GetAbsOrigin();
	m_vWorldSpaceCenterHolder.z += ( (IsDucked()) ? (VEC_DUCK_HULL_MAX.z) : (VEC_HULL_MAX.z) ) * 0.5f;
	return m_vWorldSpaceCenterHolder;
}

void CPortal_Player::Teleport( const Vector *newPosition, const QAngle *newAngles, const Vector *newVelocity )
{
	Vector oldOrigin = GetLocalOrigin();
	QAngle oldAngles = GetLocalAngles();
	BaseClass::Teleport( newPosition, newAngles, newVelocity );
	m_angEyeAngles = pl.v_angle;

	m_PlayerAnimState->Teleport( newPosition, newAngles, this );
}

void CPortal_Player::VPhysicsShadowUpdate( IPhysicsObject *pPhysics )
{
	if( m_hPortalEnvironment.Get() == NULL )
		return BaseClass::VPhysicsShadowUpdate( pPhysics );


	//below is mostly a cut/paste of existing CBasePlayer::VPhysicsShadowUpdate code with some minor tweaks to avoid getting stuck in stuff when in a portal environment
	if ( sv_turbophysics.GetBool() )
		return;

	Vector newPosition;

	bool physicsUpdated = m_pPhysicsController->GetShadowPosition( &newPosition, NULL ) > 0 ? true : false;

	// UNDONE: If the player is penetrating, but the player's game collisions are not stuck, teleport the physics shadow to the game position
	if ( pPhysics->GetGameFlags() & FVPHYSICS_PENETRATING )
	{
		CUtlVector<CBaseEntity *> list;
		PhysGetListOfPenetratingEntities( this, list );
		for ( int i = list.Count()-1; i >= 0; --i )
		{
			// filter out anything that isn't simulated by vphysics
			// UNDONE: Filter out motion disabled objects?
			if ( list[i]->GetMoveType() == MOVETYPE_VPHYSICS )
			{
				// I'm currently stuck inside a moving object, so allow vphysics to 
				// apply velocity to the player in order to separate these objects
				m_touchedPhysObject = true;
			}
		}
	}

	if ( m_pPhysicsController->IsInContact() || (m_afPhysicsFlags & PFLAG_VPHYSICS_MOTIONCONTROLLER) )
	{
		m_touchedPhysObject = true;
	}

	if ( IsFollowingPhysics() )
	{
		m_touchedPhysObject = true;
	}

	if ( GetMoveType() == MOVETYPE_NOCLIP )
	{
		m_oldOrigin = GetAbsOrigin();
		return;
	}

	if ( phys_timescale.GetFloat() == 0.0f )
	{
		physicsUpdated = false;
	}

	if ( !physicsUpdated )
		return;

	IPhysicsObject *pPhysGround = GetGroundVPhysics();

	Vector newVelocity;
	pPhysics->GetPosition( &newPosition, 0 );
	m_pPhysicsController->GetShadowVelocity( &newVelocity );



	Vector tmp = GetAbsOrigin() - newPosition;
	if ( !m_touchedPhysObject && !(GetFlags() & FL_ONGROUND) )
	{
		tmp.z *= 0.5f;	// don't care about z delta as much
	}

	float dist = tmp.LengthSqr();
	float deltaV = (newVelocity - GetAbsVelocity()).LengthSqr();

	float maxDistErrorSqr = VPHYS_MAX_DISTSQR;
	float maxVelErrorSqr = VPHYS_MAX_VELSQR;
	if ( IsRideablePhysics(pPhysGround) )
	{
		maxDistErrorSqr *= 0.25;
		maxVelErrorSqr *= 0.25;
	}

	if ( dist >= maxDistErrorSqr || deltaV >= maxVelErrorSqr || (pPhysGround && !m_touchedPhysObject) )
	{
		if ( m_touchedPhysObject || pPhysGround )
		{
			// BUGBUG: Rewrite this code using fixed timestep
			if ( deltaV >= maxVelErrorSqr )
			{
				Vector dir = GetAbsVelocity();
				float len = VectorNormalize(dir);
				float dot = DotProduct( newVelocity, dir );
				if ( dot > len )
				{
					dot = len;
				}
				else if ( dot < -len )
				{
					dot = -len;
				}

				VectorMA( newVelocity, -dot, dir, newVelocity );

				if ( m_afPhysicsFlags & PFLAG_VPHYSICS_MOTIONCONTROLLER )
				{
					float val = Lerp( 0.1f, len, dot );
					VectorMA( newVelocity, val - len, dir, newVelocity );
				}

				if ( !IsRideablePhysics(pPhysGround) )
				{
					if ( !(m_afPhysicsFlags & PFLAG_VPHYSICS_MOTIONCONTROLLER ) && IsSimulatingOnAlternateTicks() )
					{
						newVelocity *= 0.5f;
					}
					ApplyAbsVelocityImpulse( newVelocity );
				}
			}

			trace_t trace;
			UTIL_TraceEntity( this, newPosition, newPosition, MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );
			if ( !trace.allsolid && !trace.startsolid )
			{
				SetAbsOrigin( newPosition );
			}
		}
		else
		{
			trace_t trace;

			Ray_t ray;
			ray.Init( GetAbsOrigin(), GetAbsOrigin(), WorldAlignMins(), WorldAlignMaxs() );

			CTraceFilterSimple OriginalTraceFilter( this, COLLISION_GROUP_PLAYER_MOVEMENT );
			CTraceFilterTranslateClones traceFilter( &OriginalTraceFilter );
			UTIL_Portal_TraceRay_With( m_hPortalEnvironment, ray, MASK_PLAYERSOLID, &traceFilter, &trace );

			// current position is not ok, fixup
			if ( trace.allsolid || trace.startsolid )
			{
				//try again with new position
				ray.Init( newPosition, newPosition, WorldAlignMins(), WorldAlignMaxs() );
				UTIL_Portal_TraceRay_With( m_hPortalEnvironment, ray, MASK_PLAYERSOLID, &traceFilter, &trace );

				if( trace.startsolid == false )
				{
					SetAbsOrigin( newPosition );
				}
				else
				{
					if( !FindClosestPassableSpace( this, newPosition - GetAbsOrigin(), MASK_PLAYERSOLID ) )
					{
						// Try moving the player closer to the center of the portal
						CProp_Portal *pPortal = m_hPortalEnvironment.Get();
						newPosition += ( pPortal->GetAbsOrigin() - WorldSpaceCenter() ) * 0.1f;
						SetAbsOrigin( newPosition );

						DevMsg( "Hurting the player for FindClosestPassableSpaceFailure!" );

						// Deal 1 damage per frame... this will kill a player very fast, but allow for the above correction to fix some cases
						CTakeDamageInfo info( this, this, vec3_origin, vec3_origin, 1, DMG_CRUSH );
						OnTakeDamage( info );
					}
				}
			}
		}
	}
	else
	{
		if ( m_touchedPhysObject )
		{
			// check my position (physics object could have simulated into my position
			// physics is not very far away, check my position
			trace_t trace;
			UTIL_TraceEntity( this, GetAbsOrigin(), GetAbsOrigin(),
				MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );

			// is current position ok?
			if ( trace.allsolid || trace.startsolid )
			{
				// stuck????!?!?
				//Msg("Stuck on %s\n", trace.m_pEnt->GetClassname());
				SetAbsOrigin( newPosition );
				UTIL_TraceEntity( this, GetAbsOrigin(), GetAbsOrigin(),
					MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );
				if ( trace.allsolid || trace.startsolid )
				{
					//Msg("Double Stuck\n");
					SetAbsOrigin( m_oldOrigin );
				}
			}
		}
	}
	m_oldOrigin = GetAbsOrigin();
}

bool CPortal_Player::UseFoundEntity( CBaseEntity *pUseEntity, bool bAutoGrab )
{
	bool usedSomething = false;

	//!!!UNDONE: traceline here to prevent +USEing buttons through walls
	int caps = pUseEntity->ObjectCaps();
	variant_t emptyVariant;

	if ( ( (m_nButtons & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
		 ( ( (m_afButtonPressed & IN_USE) || bAutoGrab ) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
	{
		if ( caps & FCAP_CONTINUOUS_USE )
		{
			m_afPhysicsFlags |= PFLAG_USING;
		}

		pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );

		usedSomething = true;
	}
	// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
	else if ( (m_afButtonReleased & IN_USE) && (pUseEntity->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
	{
		pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );

		usedSomething = true;
	}

	return usedSomething;
}

//bool CPortal_Player::StartReplayMode( float fDelay, float fDuration, int iEntity )
//{
//	if ( !BaseClass::StartReplayMode( fDelay, fDuration, 1 ) )
//		return false;
//
//	CSingleUserRecipientFilter filter( this );
//	filter.MakeReliable();
//
//	UserMessageBegin( filter, "KillCam" );
//
//	EHANDLE hPlayer = this;
//
//	if ( m_hObserverTarget.Get() )
//	{
//		WRITE_EHANDLE( m_hObserverTarget );	// first target
//		WRITE_EHANDLE( hPlayer );	//second target
//	}
//	else
//	{
//		WRITE_EHANDLE( hPlayer );	// first target
//		WRITE_EHANDLE( 0 );			//second target
//	}
//	MessageEnd();
//
//	return true;
//}
//
//void CPortal_Player::StopReplayMode()
//{
//	BaseClass::StopReplayMode();
//
//	CSingleUserRecipientFilter filter( this );
//	filter.MakeReliable();
//
//	UserMessageBegin( filter, "KillCam" );
//	WRITE_EHANDLE( 0 );
//	WRITE_EHANDLE( 0 );
//	MessageEnd();
//}

void CPortal_Player::PlayerUse( void )
{
	CBaseEntity *pUseEnt = m_hGrabbedEntity.Get();
	CPortal_Base2D *pUseThroughPortal = (CPortal_Base2D*)m_hPortalThroughWhichGrabOccured.Get();

	if ( gpGlobals->curtime < m_flUseKeyCooldownTime )
	{
		return;
	}

	bool bUsePressed = (m_afButtonPressed & IN_USE) != 0;

	if ( bUsePressed && m_hUseEntity )
	{
		// Currently using a latched entity?
		if ( !ClearUseEntity() )
		{
			m_bPlayUseDenySound = true;
		}
		else
		{
			m_flAutoGrabLockOutTime = gpGlobals->curtime;
		}
		return;
	}

	if ( !pUseEnt )
	{
		PollForUseEntity( bUsePressed, &pUseEnt, &pUseThroughPortal );
	}

	// Was use pressed or released?
	if ( !bUsePressed && !pUseEnt )
	{
		return;
	}

	// Only run the below if use is pressed and the client sent a grab entity
	bool bUsedSomething = false;
	if ( pUseEnt && m_hAttachedObject.Get() == NULL )
	{
		// Use the found entity, and skip the button down checks if we're forcing the grabbing of a client use entity
		bUsedSomething = UseFoundEntity( pUseEnt, !bUsePressed );

		if ( bUsedSomething && pUseThroughPortal )
		{
			SetHeldObjectOnOppositeSideOfPortal( true );
			SetHeldObjectPortal( pUseThroughPortal );
		}
		else
		{
			SetHeldObjectOnOppositeSideOfPortal( false );
			SetHeldObjectPortal( NULL );
		}

		// Debounce the use key
		if ( bUsedSomething )
		{
			m_Local.m_nOldButtons |= IN_USE;
			m_afButtonPressed &= ~IN_USE;
		}
	}

	if ( bUsePressed && !bUsedSomething )
	{
		// No entity passed up with the use command, play deny sound
		m_bPlayUseDenySound = true;

		// Make the weapon "dry fire" to show we tried to +use
		CWeaponPortalgun *pWeapon = dynamic_cast<CWeaponPortalgun *>(GetActivePortalWeapon());
		if ( pWeapon )
		{
			pWeapon->UseDeny();
		}
	}
}

extern ConVar sv_enableholdrotation;
ConVar sv_holdrotationsensitivity( "sv_holdrotationsensitivity", "0.1", FCVAR_ARCHIVE );
void CPortal_Player::PlayerRunCommand(CUserCmd *ucmd, IMoveHelper *moveHelper)
{
	if ( sv_enableholdrotation.GetBool() && !IsUsingVMGrab() )
	{
		if( (ucmd->buttons & IN_ATTACK2) && (GetPlayerHeldEntity( this ) != NULL) )
		{
			VectorCopy ( pl.v_angle, ucmd->viewangles );
			float x, y;
			if( abs( ucmd->mousedx ) > abs(ucmd->mousedy) )
			{
				x = ucmd->mousedx * sv_holdrotationsensitivity.GetFloat();
				y = 0;
			}
			else
			{
				x = 0;
				y = ucmd->mousedy * sv_holdrotationsensitivity.GetFloat();
			}
			RotatePlayerHeldObject( this, x, y, true );
		}
	}

	// Can't use stuff while dead
	if ( IsDead() )
	{
		ucmd->buttons &= ~IN_USE;
	}

	m_hGrabbedEntity = ucmd->player_held_entity > 0 ? CBaseEntity::Instance ( ucmd->player_held_entity ) : NULL;
	m_hPortalThroughWhichGrabOccured = ucmd->held_entity_was_grabbed_through_portal > 0 ? CBaseEntity::Instance ( ucmd->held_entity_was_grabbed_through_portal ) : NULL;

	//============================================================================
	// Fix the eye angles after portalling. The client may have sent commands with
	// the old view angles before it knew about the teleportation.

	//sorry for crappy name, the client sent us a command, we acknowledged it, and they are now telling us the latest one they received an acknowledgement for in this brand new command
	int iLastCommandAcknowledgementReceivedOnClientForThisCommand = ucmd->command_number - ucmd->command_acknowledgements_pending;
	while( (m_PendingPortalTransforms.Count() > 0) && (iLastCommandAcknowledgementReceivedOnClientForThisCommand >= m_PendingPortalTransforms[0].command_number) )
	{
		m_PendingPortalTransforms.Remove( 0 );
	}

	// The server changed the angles, and the user command was created after the teleportation, but before the client knew they teleported. Need to fix up the angles into the new space
	if( m_PendingPortalTransforms.Count() > ucmd->predictedPortalTeleportations )
	{
		matrix3x4_t matComputeFinalTransform[2];
		int iFlip = 0;

		//most common case will be exactly 1 transform
		matComputeFinalTransform[0] = m_PendingPortalTransforms[ucmd->predictedPortalTeleportations].matTransform;

		for( int i = ucmd->predictedPortalTeleportations + 1; i < m_PendingPortalTransforms.Count(); ++i )
		{
			ConcatTransforms( m_PendingPortalTransforms[i].matTransform, matComputeFinalTransform[iFlip], matComputeFinalTransform[1-iFlip] );
			iFlip = 1 - iFlip;
		}

		//apply the final transform
		matrix3x4_t matAngleTransformIn, matAngleTransformOut;
		AngleMatrix( ucmd->viewangles, matAngleTransformIn );
		ConcatTransforms( matComputeFinalTransform[iFlip], matAngleTransformIn, matAngleTransformOut );
		MatrixAngles( matAngleTransformOut, ucmd->viewangles );
	}

	PreventCrouchJump( ucmd );

	if ( m_PortalLocal.m_bZoomedIn )
	{
		if ( IsTaunting() )
		{
			// Pop out of zoom when I'm taunting
			ZoomOut();
		}
		else
		{
			float fThreshold = sv_zoom_stop_movement_threashold.GetFloat();
			if ( gpGlobals->curtime > GetFOVTime() + sv_zoom_stop_time_threashold.GetFloat() &&
				 ( fabsf( ucmd->forwardmove ) > fThreshold ||  fabsf( ucmd->sidemove ) > fThreshold ) )
			{
				// After 5 seconds, moving while zoomed will pop you back out
				// This is to fix people who accidentally switch into zoom mode, but don't know how to get back out...
				// Should give plenty of time to players who want to move while zoomed
				ZoomOut();
			}
		}
	}

	BaseClass::PlayerRunCommand( ucmd, moveHelper );
}


bool CPortal_Player::ClientCommand( const CCommand &args )
{
	if ( FStrEq( args[0], "spectate" ) )
	{
		// do nothing.
		return true;
	}

	return BaseClass::ClientCommand( args );
}

void CPortal_Player::CheatImpulseCommands( int iImpulse )
{
	switch ( iImpulse )
	{
	case 101:
		{
			if( sv_cheats->GetBool() )
			{
				GiveAllItems();
			}
		}
		break;

	default:
		BaseClass::CheatImpulseCommands( iImpulse );
	}
}

void CPortal_Player::CreateViewModel( int index /*=0*/ )
{
	BaseClass::CreateViewModel( index );
	return;
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

bool CPortal_Player::BecomeRagdollOnClient( const Vector &force )
{
	return true;//BaseClass::BecomeRagdollOnClient( force );
}

void CPortal_Player::CreateRagdollEntity( const CTakeDamageInfo &info )
{
	if ( m_hRagdoll )
	{
		UTIL_RemoveImmediate( m_hRagdoll );
		m_hRagdoll = NULL;
	}

#if PORTAL_HIDE_PLAYER_RAGDOLL
	AddSolidFlags( FSOLID_NOT_SOLID );
	AddEffects( EF_NODRAW | EF_NOSHADOW );
	AddEFlags( EFL_NO_DISSOLVE );
#endif // PORTAL_HIDE_PLAYER_RAGDOLL
	CBaseEntity *pRagdoll = CreateServerRagdoll( this, m_nForceBone, info, COLLISION_GROUP_INTERACTIVE_DEBRIS, true );
	pRagdoll->m_takedamage = DAMAGE_NO;
	m_hRagdoll = pRagdoll;

/*
	// If we already have a ragdoll destroy it.
	CPortalRagdoll *pRagdoll = dynamic_cast<CPortalRagdoll*>( m_hRagdoll.Get() );
	if( pRagdoll )
	{
		UTIL_Remove( pRagdoll );
		pRagdoll = NULL;
	}
	Assert( pRagdoll == NULL );

	// Create a ragdoll.
	pRagdoll = dynamic_cast<CPortalRagdoll*>( CreateEntityByName( "portal_ragdoll" ) );
	if ( pRagdoll )
	{
		

		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->CopyAnimationDataFrom( this );
		pRagdoll->SetOwnerEntity( this );
		pRagdoll->m_flAnimTime = gpGlobals->curtime;
		pRagdoll->m_flPlaybackRate = 0.0;
		pRagdoll->SetCycle( 0 );
		pRagdoll->ResetSequence( 0 );

		float fSequenceDuration = SequenceDuration( GetSequence() );
		float fPreviousCycle = clamp(GetCycle()-( 0.1 * ( 1 / fSequenceDuration ) ),0.f,1.f);
		float fCurCycle = GetCycle();
		matrix3x4_t pBoneToWorld[MAXSTUDIOBONES], pBoneToWorldNext[MAXSTUDIOBONES];
		SetupBones( pBoneToWorldNext, BONE_USED_BY_ANYTHING );
		SetCycle( fPreviousCycle );
		SetupBones( pBoneToWorld, BONE_USED_BY_ANYTHING );
		SetCycle( fCurCycle );

		pRagdoll->InitRagdoll( info.GetDamageForce(), m_nForceBone, info.GetDamagePosition(), pBoneToWorld, pBoneToWorldNext, 0.1f, COLLISION_GROUP_INTERACTIVE_DEBRIS, true );
		pRagdoll->SetMoveType( MOVETYPE_VPHYSICS );
		pRagdoll->SetSolid( SOLID_VPHYSICS );
		if ( IsDissolving() )
		{
			pRagdoll->TransferDissolveFrom( this );
		}

		Vector mins, maxs;
		mins = CollisionProp()->OBBMins();
		maxs = CollisionProp()->OBBMaxs();
		pRagdoll->CollisionProp()->SetCollisionBounds( mins, maxs );
		pRagdoll->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
	}

	// Turn off the player.
	AddSolidFlags( FSOLID_NOT_SOLID );
	AddEffects( EF_NODRAW | EF_NOSHADOW );
	SetMoveType( MOVETYPE_NONE );

	// Save ragdoll handle.
	m_hRagdoll = pRagdoll;
*/
}

void CPortal_Player::Jump( void )
{
	g_PortalGameStats.Event_PlayerJump( GetAbsOrigin(), GetAbsVelocity() );
	BaseClass::Jump();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPortal_Player::ZoomIn()
{
	SetFOV( this, 45.0f, 0.15f );
	m_PortalLocal.m_bZoomedIn = true;

	IGameEvent * event = gameeventmanager->CreateEvent( "player_zoomed" );
	if ( event )
	{
		event->SetInt( "userid", GetUserID() );
		gameeventmanager->FireEvent( event );
	}
}

void CPortal_Player::ZoomOut()
{
	SetFOV( this, 0.0f, 0.15f );
	m_PortalLocal.m_bZoomedIn = false;

	IGameEvent * event = gameeventmanager->CreateEvent( "player_unzoomed" );
	if ( event )
	{
		event->SetInt( "userid", GetUserID() );
		gameeventmanager->FireEvent( event );
	}
}

bool CPortal_Player::IsZoomed( void )
{
	return m_PortalLocal.m_bZoomedIn;
}


void CPortal_Player::Event_Killed( const CTakeDamageInfo &info )
{
	//update damage info with our accumulated physics force
	CTakeDamageInfo subinfo = info;
	subinfo.SetDamageForce( m_vecTotalBulletForce );

	// show killer in death cam mode
	// chopped down version of SetObserverTarget without the team check
	//if( info.GetAttacker() )
	//{
	//	// set new target
	//	m_hObserverTarget.Set( info.GetAttacker() ); 
	//}
	//else
	//	m_hObserverTarget.Set( NULL );

	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.
	CreateRagdollEntity( info );

	BaseClass::Event_Killed( subinfo );
	
	SetUseKeyCooldownTime( 3.0f );

#if PORTAL_HIDE_PLAYER_RAGDOLL
	// Fizzle all portals so they don't see the player disappear
	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	for( int i = 0; i != iPortalCount; ++i )
	{
		CProp_Portal *pTempPortal = pPortals[i];

		if( pTempPortal && pTempPortal->m_bActivated )
		{
			pTempPortal->Fizzle();
		}
	}
#endif // PORTAL_HIDE_PLAYER_RAGDOLL

	if ( (info.GetDamageType() & DMG_DISSOLVE) && !(m_hRagdoll.Get()->GetEFlags() & EFL_NO_DISSOLVE) )
	{
		if ( m_hRagdoll )
		{
			m_hRagdoll->GetBaseAnimating()->Dissolve( NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
		}
	}

	m_lifeState = LIFE_DYING;
	//StopZooming();

	if ( GetObserverTarget() )
	{
		//StartReplayMode( 3, 3, GetObserverTarget()->entindex() );
		//StartObserverMode( OBS_MODE_DEATHCAM );
	}
}

int CPortal_Player::OnTakeDamage( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo inputInfoCopy( inputInfo );

	// If you shoot yourself, make it hurt but push you less
	if ( inputInfoCopy.GetAttacker() == this && inputInfoCopy.GetDamageType() == DMG_BULLET )
	{
		inputInfoCopy.ScaleDamage( 5.0f );
		inputInfoCopy.ScaleDamageForce( 0.05f );
	}

	CBaseEntity *pAttacker = inputInfoCopy.GetAttacker();
	CBaseEntity *pInflictor = inputInfoCopy.GetInflictor();

	bool bIsTurret = false;

	if ( pAttacker && FClassnameIs( pAttacker, "npc_portal_turret_floor" ) )
		bIsTurret = true;

	// Refuse damage from prop_glados_core.
	if ( (pAttacker && FClassnameIs( pAttacker, "prop_glados_core" )) ||
		(pInflictor && FClassnameIs( pInflictor, "prop_glados_core" ))  )
	{
		inputInfoCopy.SetDamage(0.0f);
	}

	if ( bIsTurret && ( inputInfoCopy.GetDamageType() & DMG_BULLET ) )
	{
		Vector vLateralForce = inputInfoCopy.GetDamageForce();
		vLateralForce.z = 0.0f;

		// Push if the player is moving against the force direction
		if ( GetAbsVelocity().Dot( vLateralForce ) < 0.0f )
			ApplyAbsVelocityImpulse( vLateralForce );
	}
	else if ( ( inputInfoCopy.GetDamageType() & DMG_CRUSH ) )
	{
		if ( bIsTurret )
		{
			inputInfoCopy.SetDamage( inputInfoCopy.GetDamage() * 0.5f );
		}

		if ( inputInfoCopy.GetDamage() >= 10.0f )
		{
			EmitSound( "PortalPlayer.BonkYelp" );
		}
	}
	else if ( ( inputInfoCopy.GetDamageType() & DMG_SHOCK ) || ( inputInfoCopy.GetDamageType() & DMG_BURN ) )
	{
		EmitSound( "PortalPortal.PainYelp" );
	}

	int ret = BaseClass::OnTakeDamage( inputInfoCopy );

	// Copy the multidamage damage origin over what the base class wrote, because
	// that gets translated correctly though portals.
	m_DmgOrigin = inputInfo.GetDamagePosition();

	if ( GetHealth() < 100 )
	{
		m_fTimeLastHurt = gpGlobals->curtime;
	}

	return ret;
}

int CPortal_Player::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	// set damage type sustained
	m_bitsDamageType |= info.GetDamageType();

	if ( !CBaseCombatCharacter::OnTakeDamage_Alive( info ) )
		return 0;

	CBaseEntity * attacker = info.GetAttacker();

	if ( !attacker )
		return 0;

	Vector vecDir = vec3_origin;
	if ( info.GetInflictor() )
	{
		vecDir = info.GetInflictor()->WorldSpaceCenter() - Vector ( 0, 0, 10 ) - WorldSpaceCenter();
		VectorNormalize( vecDir );
	}

	if ( info.GetInflictor() && (GetMoveType() == MOVETYPE_WALK) && 
		( !attacker->IsSolidFlagSet(FSOLID_TRIGGER)) )
	{
		Vector force = vecDir;// * -DamageForce( WorldAlignSize(), info.GetBaseDamage() );
		if ( force.z > 250.0f )
		{
			force.z = 250.0f;
		}
		ApplyAbsVelocityImpulse( force );
	}

	// fire global game event

	IGameEvent * event = gameeventmanager->CreateEvent( "player_hurt" );
	if ( event )
	{
		event->SetInt("userid", GetUserID() );
		event->SetInt("health", MAX(0, m_iHealth) );
		event->SetInt("priority", 5 );	// HLTV event priority, not transmitted

		if ( attacker->IsPlayer() )
		{
			CBasePlayer *player = ToBasePlayer( attacker );
			event->SetInt("attacker", player->GetUserID() ); // hurt by other player
		}
		else
		{
			event->SetInt("attacker", 0 ); // hurt by "world"
		}

		gameeventmanager->FireEvent( event );
	}

	// Insert a combat sound so that nearby NPCs hear battle
	if ( attacker->IsNPC() )
	{
		CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 512, 0.5, this );//<<TODO>>//magic number
	}

	return 1;
}


void CPortal_Player::ForceDuckThisFrame( void )
{
	if( m_Local.m_bDucked != true )
	{
		//m_Local.m_bDucking = false;
		m_Local.m_bDucked = true;
		ForceButtons( IN_DUCK );
		AddFlag( FL_DUCKING );
		SetVCollisionState( GetAbsOrigin(), GetAbsVelocity(), VPHYS_CROUCH );
	}
}

void CPortal_Player::UnDuck( void )
{
	if( m_Local.m_bDucked != false )
	{
		m_Local.m_bDucked = false;
		UnforceButtons( IN_DUCK );
		RemoveFlag( FL_DUCKING );
		SetVCollisionState( GetAbsOrigin(), GetAbsVelocity(), VPHYS_WALK );
	}
}


void CPortal_Player::SetForcedGrabControllerType( ForcedGrabControllerType type )
{
	m_ForcedGrabController = type;
}

//--------------------------------------------------------------------------------------------------
// Disable the use key for the specified time (in seconds)
//--------------------------------------------------------------------------------------------------
void CPortal_Player::SetUseKeyCooldownTime( float flCooldownDuration )
{
	const float flMaxCooldownDuration = 120.0f;
	Assert( flCooldownDuration < flMaxCooldownDuration );
	flCooldownDuration = clamp ( flCooldownDuration, 0.0f, flMaxCooldownDuration );
	m_flUseKeyCooldownTime = gpGlobals->curtime + flCooldownDuration;
}


void CPortal_Player::UpdateVMGrab( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return;

	switch( m_ForcedGrabController )
	{
	case FORCE_GRAB_CONTROLLER_VM:
		{
			m_bUseVMGrab = true;
		}
		break;
	case FORCE_GRAB_CONTROLLER_PHYSICS:
		{
			m_bUseVMGrab = false;
		}
		break;
	case FORCE_GRAB_CONTROLLER_DEFAULT:
	default:
		{
#if 0	// Turning this off for now... the FOV differences cause a visual pop that people are bugging
			// and we might not be getting that much for having it not collide while flinging.
			if ( GetAbsVelocity().Length() > 350.0f )
			{
				// Going fast! Use VM
				m_bUseVMGrab = true;
				return;
			}
#endif

			if ( FClassnameIs( pEntity, "npc_portal_turret_floor" ) )
			{
				// It's a turret, use physics
				m_bUseVMGrab = false;
				return;
			}

			if ( GameRules() && GameRules()->IsMultiplayer() )
			{
				// In MP our reflective cubes need to go physics when touching or near a laser
				if ( UTIL_IsReflectiveCube( pEntity ) ||  UTIL_IsSchrodinger( pEntity ) )
				{
					CPropWeightedCube *pCube = (CPropWeightedCube *)pEntity;
					if ( pCube->HasLaser() )
					{
						// VMGrabController'd laser cubes that were grabbed through portals still think they are being
						// held through portals.  Clear that out here, so when it comes time to ComputeError, we won't get
						// a bogus error and force drop the cube.
						SetHeldObjectOnOppositeSideOfPortal( false );
						SetHeldObjectPortal( NULL );

						// It's redirecting a laser, use physics
						m_bUseVMGrab = false;
						return;
					}
					else
					{
						// TODO: Add lasers and then add this code back.
#if 0
						// Check it's distance to each laser line so the depth renders properly when the laser is between the player and cube
						for ( int i = 0; i < IPortalLaserAutoList::AutoList().Count(); ++i )
						{
							CPortalLaser *pLaser = static_cast< CPortalLaser* >( IPortalLaserAutoList::AutoList()[ i ] );
							if ( pLaser )
							{
								Vector vClosest = pLaser->ClosestPointOnLineSegment( pCube->GetAbsOrigin() );
								if ( vClosest.DistToSqr( pCube->GetAbsOrigin() ) < ( 80.0f * 80.0f ) )
								{
									// VMGrabController'd laser cubes that were grabbed through portals still think they are being
									// held through portals.  Clear that out here, so when it comes time to ComputeError, we won't get
									// a bogus error and force drop the cube.
									SetHeldObjectOnOppositeSideOfPortal( false );
									SetHeldObjectPortal( NULL );

									// It's close to the laser
									m_bUseVMGrab = false;
									return;
								}
							}
						}
#endif
					}
				}

				// Multiplayer uses VM otherwise
				m_bUseVMGrab = true;
				return;
			}

			if ( FClassnameIs( pEntity, "npc_personality_core" ) )
			{
				// It's a personality core, use VM mode
				m_bUseVMGrab = true;
				return;
			}

			m_bUseVMGrab = false;
		}
		break;

	} //Switch forced grab controller
}


void CPortal_Player::IncrementPortalsPlaced( void )
{
	m_StatsThisLevel.iNumPortalsPlaced++;

	if ( m_iBonusChallenge == PORTAL_CHALLENGE_PORTALS )
		SetBonusProgress( static_cast<int>( m_StatsThisLevel.iNumPortalsPlaced ) );
}

void CPortal_Player::IncrementStepsTaken( void )
{
	m_StatsThisLevel.iNumStepsTaken++;

	if ( m_iBonusChallenge == PORTAL_CHALLENGE_STEPS )
		SetBonusProgress( static_cast<int>( m_StatsThisLevel.iNumStepsTaken ) );
}

void CPortal_Player::UpdateSecondsTaken( void )
{
	float fSecondsSinceLastUpdate = ( gpGlobals->curtime - m_fTimeLastNumSecondsUpdate );
	m_StatsThisLevel.fNumSecondsTaken += fSecondsSinceLastUpdate;
	m_fTimeLastNumSecondsUpdate = gpGlobals->curtime;

	if ( m_iBonusChallenge == PORTAL_CHALLENGE_TIME )
		SetBonusProgress( static_cast<int>( m_StatsThisLevel.fNumSecondsTaken ) );

	if ( m_fNeuroToxinDamageTime > 0.0f )
	{
		float fTimeRemaining = m_fNeuroToxinDamageTime - gpGlobals->curtime;

		if ( fTimeRemaining < 0.0f )
		{
			CTakeDamageInfo info;
			info.SetDamage( gpGlobals->frametime * 50.0f );
			info.SetDamageType( DMG_NERVEGAS );
			TakeDamage( info );
			fTimeRemaining = 0.0f;
		}

		PauseBonusProgress( false );
		SetBonusProgress( static_cast<int>( fTimeRemaining ) );
	}
}

void CPortal_Player::ResetThisLevelStats( void )
{
	m_StatsThisLevel.iNumPortalsPlaced = 0;
	m_StatsThisLevel.iNumStepsTaken = 0;
	m_StatsThisLevel.fNumSecondsTaken = 0.0f;

	if ( m_iBonusChallenge != PORTAL_CHALLENGE_NONE )
		SetBonusProgress( 0 );
}


//-----------------------------------------------------------------------------
// Purpose: Update the area bits variable which is networked down to the client to determine
//			which area portals should be closed based on visibility.
// Input  : *pvs - pvs to be used to determine visibility of the portals
//-----------------------------------------------------------------------------
void CPortal_Player::UpdatePortalViewAreaBits( unsigned char *pvs, int pvssize )
{
	Assert ( pvs );

	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount == 0 )
		return;

	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	int *portalArea = (int *)stackalloc( sizeof( int ) * iPortalCount );
	bool *bUsePortalForVis = (bool *)stackalloc( sizeof( bool ) * iPortalCount );

	unsigned char *portalTempBits = (unsigned char *)stackalloc( sizeof( unsigned char ) * 32 * iPortalCount );
	COMPILE_TIME_ASSERT( (sizeof( unsigned char ) * 32) >= sizeof( ((CPlayerLocalData*)0)->m_chAreaBits ) );

	// setup area bits for these portals
	for ( int i = 0; i < iPortalCount; ++i )
	{
		CProp_Portal* pLocalPortal = pPortals[ i ];
		// Make sure this portal is active before adding it's location to the pvs
		if ( pLocalPortal && pLocalPortal->m_bActivated )
		{
			CProp_Portal* pRemotePortal = pLocalPortal->m_hLinkedPortal.Get();

			// Make sure this portal's linked portal is in the PVS before we add what it can see
			if ( pRemotePortal && pRemotePortal->m_bActivated && pRemotePortal->NetworkProp() && 
				pRemotePortal->NetworkProp()->IsInPVS( edict(), pvs, pvssize ) )
			{
				portalArea[ i ] = engine->GetArea( pPortals[ i ]->GetAbsOrigin() );

				if ( portalArea [ i ] >= 0 )
				{
					bUsePortalForVis[ i ] = true;
				}

				engine->GetAreaBits( portalArea[ i ], &portalTempBits[ i * 32 ], sizeof( unsigned char ) * 32 );
			}
		}
	}

	// Use the union of player-view area bits and the portal-view area bits of each portal
	for ( int i = 0; i < m_Local.m_chAreaBits.Count(); i++ )
	{
		for ( int j = 0; j < iPortalCount; ++j )
		{
			// If this portal is active, in PVS and it's location is valid
			if ( bUsePortalForVis[ j ]  )
			{
				m_Local.m_chAreaBits.Set( i, m_Local.m_chAreaBits[ i ] | portalTempBits[ (j * 32) + i ] );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// AddPortalCornersToEnginePVS
// Subroutine to wrap the adding of portal corners to the PVS which is called once for the setup of each portal.
// input - pPortal: the portal we are viewing 'out of' which needs it's corners added to the PVS
//////////////////////////////////////////////////////////////////////////
void AddPortalCornersToEnginePVS( CProp_Portal* pPortal )
{
	Assert ( pPortal );

	if ( !pPortal )
		return;

	Vector vForward, vRight, vUp;
	pPortal->GetVectors( &vForward, &vRight, &vUp );

	// Center of the remote portal
	Vector ptOrigin			= pPortal->GetAbsOrigin();

	// Distance offsets to the different edges of the portal... Used in the placement checks
	Vector vToTopEdge = vUp * ( PORTAL_HALF_HEIGHT - PORTAL_BUMP_FORGIVENESS );
	Vector vToBottomEdge = -vToTopEdge;
	Vector vToRightEdge = vRight * ( PORTAL_HALF_WIDTH - PORTAL_BUMP_FORGIVENESS );
	Vector vToLeftEdge = -vToRightEdge;

	// Distance to place PVS points away from portal, to avoid being in solid
	Vector vForwardBump		= vForward * 1.0f;

	// Add center and edges to the engine PVS
	engine->AddOriginToPVS( ptOrigin + vForwardBump);
	engine->AddOriginToPVS( ptOrigin + vToTopEdge + vToLeftEdge + vForwardBump );
	engine->AddOriginToPVS( ptOrigin + vToTopEdge + vToRightEdge + vForwardBump );
	engine->AddOriginToPVS( ptOrigin + vToBottomEdge + vToLeftEdge + vForwardBump );
	engine->AddOriginToPVS( ptOrigin + vToBottomEdge + vToRightEdge + vForwardBump );
}

void PortalSetupVisibility( CBaseEntity *pPlayer, int area, unsigned char *pvs, int pvssize )
{
	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount == 0 )
		return;

	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	for( int i = 0; i != iPortalCount; ++i )
	{
		CProp_Portal *pPortal = pPortals[i];

		if ( pPortal && pPortal->m_bActivated )
		{
			if ( pPortal->NetworkProp()->IsInPVS( pPlayer->edict(), pvs, pvssize ) )
			{
				if ( engine->CheckAreasConnected( area, pPortal->NetworkProp()->AreaNum() ) )
				{
					CProp_Portal *pLinkedPortal = static_cast<CProp_Portal*>( pPortal->m_hLinkedPortal.Get() );
					if ( pLinkedPortal )
					{
						AddPortalCornersToEnginePVS ( pLinkedPortal );
					}
				}
			}
		}
	}
}

void CPortal_Player::SetupVisibility( CBaseEntity *pViewEntity, unsigned char *pvs, int pvssize )
{
	BaseClass::SetupVisibility( pViewEntity, pvs, pvssize );

	int area = pViewEntity ? pViewEntity->NetworkProp()->AreaNum() : NetworkProp()->AreaNum();

	// At this point the EyePosition has been added as a view origin, but if we are currently stuck
	// in a portal, our EyePosition may return a point in solid. Find the reflected eye position
	// and use that as a vis origin instead.
	if ( m_hPortalEnvironment )
	{
		CProp_Portal *pPortal = NULL, *pRemotePortal = NULL;
		pPortal = m_hPortalEnvironment;
		pRemotePortal = pPortal->m_hLinkedPortal;

		if ( pPortal && pRemotePortal && pPortal->m_bActivated && pRemotePortal->m_bActivated )
		{		
			Vector ptPortalCenter = pPortal->GetAbsOrigin();
			Vector vPortalForward;
			pPortal->GetVectors( &vPortalForward, NULL, NULL );

			Vector eyeOrigin = EyePosition();
			Vector vEyeToPortalCenter = ptPortalCenter - eyeOrigin;

			float fPortalDist = vPortalForward.Dot( vEyeToPortalCenter );
			if( fPortalDist > 0.0f ) //eye point is behind portal
			{
				// Move eye origin to it's transformed position on the other side of the portal
				UTIL_Portal_PointTransform( pPortal->MatrixThisToLinked(), eyeOrigin, eyeOrigin );

				// Use this as our view origin (as this is where the client will be displaying from)
				engine->AddOriginToPVS( eyeOrigin );
				if ( !pViewEntity || pViewEntity->IsPlayer() )
				{
					area = engine->GetArea( eyeOrigin );
				}	
			}
		}
	}

	PortalSetupVisibility( this, area, pvs, pvssize );
}

extern CBaseEntity *GetCoopSpawnLocation( int iTeam ); //in info_coop_spawn.cpp

CBaseEntity* CPortal_Player::EntSelectSpawnPoint( void )
{
	if ( !g_pGameRules->IsMultiplayer() )
	{
		return BaseClass::EntSelectSpawnPoint();
	}

	if( g_pGameRules->IsCoOp() )
	{
		switch( GetTeamNumber() )
		{
		case TEAM_UNASSIGNED:
		//case TEAM_SPECTATOR:
			PickTeam();
		}

		CBaseEntity *pSpawnLocation = GetCoopSpawnLocation( GetTeamNumber() );
		Msg("TEAM NUMBER: %i\n", this->GetTeamNumber());
		if( pSpawnLocation )
			return pSpawnLocation;
	}

	CBaseEntity *pSpot = NULL;
	CBaseEntity *pLastSpawnPoint = g_pLastSpawn;
	const char *pSpawnpointName = "info_player_start";

	pSpot = pLastSpawnPoint;
	// Randomize the start spot
	for ( int i = random->RandomInt(1,5); i > 0; i-- )
		pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
	if ( !pSpot )  // skip over the null point
		pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );

	CBaseEntity *pFirstSpot = pSpot;

	do
	{
		if ( pSpot )
		{
			// check if pSpot is valid
			if ( g_pGameRules->IsSpawnPointValid( pSpot, this ) )
			{
				if ( pSpot->GetLocalOrigin() == vec3_origin )
				{
					pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
					continue;
				}

				// if so, go to pSpot
				goto ReturnSpot;
			}
		}
		// increment pSpot
		pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
	} while ( pSpot != pFirstSpot ); // loop if we're not back to the start

#if 0
	// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
	edict_t		*player = edict();
	if ( pSpot )
	{
		CBaseEntity *ent = NULL;
		for ( CEntitySphereQuery sphere( pSpot->GetAbsOrigin(), 128 ); (ent = sphere.GetCurrentEntity()) != NULL; sphere.NextEntity() )
		{
			// if ent is a client, kill em (unless they are ourselves)
			if ( ent->IsPlayer() && !(ent->edict() == player) )
				ent->TakeDamage( CTakeDamageInfo( GetContainingEntity(INDEXENT(0)), GetContainingEntity(INDEXENT(0)), 300, DMG_GENERIC ) );
		}
		goto ReturnSpot;
	}
#endif // 0

	if ( !pSpot  )
	{
		pSpot = gEntList.FindEntityByClassname( pSpot, "info_player_start" );

		if ( pSpot )
			goto ReturnSpot;
	}

ReturnSpot:

	g_pLastSpawn = pSpot;

	//m_flSlamProtectTime = gpGlobals->curtime + 0.5;

	return pSpot;
}

static int s_CoopTeamAssignments[MAX_PLAYERS] = { 0 };

void CPortal_Player::PickTeam( void )
{
	int iIndex = ENTINDEX( this ) - 1;
	if( g_pGameRules->IsCoOp() && (s_CoopTeamAssignments[iIndex] != 0) )
	{
		ChangeTeam( s_CoopTeamAssignments[iIndex] );
		return;
	}

	//picks lowest or random
	CTeam *pRed = g_Teams[TEAM_RED];
	CTeam *pBlue = g_Teams[TEAM_BLUE];

	if ( pRed->GetNumPlayers() > pBlue->GetNumPlayers() )
	{
		ChangeTeam( TEAM_BLUE );
	}
	else if ( pRed->GetNumPlayers() < pBlue->GetNumPlayers() )
	{
		ChangeTeam( TEAM_RED );
	}
	else
	{
		if ( mp_server_player_team.GetInt() == 0 )
		{
			ChangeTeam( TEAM_BLUE );
		}
		else if ( mp_server_player_team.GetInt() == 1 )
		{
			ChangeTeam( TEAM_RED );
		}
		else
		{
			ChangeTeam( random->RandomInt( TEAM_RED, TEAM_BLUE ) );
		}
	}


	if( g_pGameRules->IsCoOp() )
	{
		s_CoopTeamAssignments[iIndex] = GetTeamNumber();
	}
}

void CPortal_Player::ClientDisconnected( edict_t *pPlayer )
{
	s_CoopTeamAssignments[ENTINDEX( pPlayer ) - 1] = 0;
}

CON_COMMAND( displayportalplayerstats, "Displays current level stats for portals placed, steps taken, and seconds taken." )
{
	CPortal_Player *pPlayer = (CPortal_Player *)UTIL_GetCommandClient();
	if( pPlayer == NULL )
		pPlayer = GetPortalPlayer( 1 ); //last ditch effort

	if( pPlayer )
	{
		int iMinutes = static_cast<int>( pPlayer->NumSecondsTaken() / 60.0f );
		int iSeconds = static_cast<int>( pPlayer->NumSecondsTaken() ) % 60;

		CFmtStr msg;
		NDebugOverlay::ScreenText( 0.5f, 0.5f, msg.sprintf( "Portals Placed: %d\nSteps Taken: %d\nTime: %d:%d", pPlayer->NumPortalsPlaced(), pPlayer->NumStepsTaken(), iMinutes, iSeconds ), 255, 255, 255, 150, 5.0f );
	}
}

CON_COMMAND( startneurotoxins, "Starts the nerve gas timer." )
{
	CPortal_Player *pPlayer = (CPortal_Player *)UTIL_GetCommandClient();
	if( pPlayer == NULL )
		pPlayer = GetPortalPlayer( 1 ); //last ditch effort

	float fCoundownTime = 180.0f;

	if ( args.ArgC() > 1 )
		fCoundownTime = atof( args[ 1 ] );

	if( pPlayer )
		pPlayer->SetNeuroToxinDamageTime( fCoundownTime );
}

void CPortal_Player::InitVCollision( const Vector &vecAbsOrigin, const Vector &vecAbsVelocity )
{
	/*
	// Cleanup any old vphysics stuff.
	VPhysicsDestroyObject();

	// in turbo physics players dont have a physics shadow
	if ( sv_turbophysics.GetBool() )
		return;

	CPhysCollide *pModel = PhysCreateBbox( GetStandHullMins(), GetStandHullMaxs() );
	CPhysCollide *pCrouchModel = PhysCreateBbox( GetDuckHullMins(), GetDuckHullMaxs() );

	SetupVPhysicsShadow( vecAbsOrigin, vecAbsVelocity, pModel, "player_stand", pCrouchModel, "player_crouch" );
	*/

	BaseClass::InitVCollision( vecAbsOrigin, vecAbsVelocity );
}


void CPortal_Player::GivePlayerPaintGun( bool bActivatePaintPowers, bool bSwitchTo )
{
	CBaseCombatWeapon *pWeapon = Weapon_OwnsThisType( "weapon_paintgun", 0 );
	CWeaponPaintGun *pPaintGun = NULL;

	//If the player already has a paint gun
	if( pWeapon )
	{
		pPaintGun = static_cast<CWeaponPaintGun*>( pWeapon );
	}
	else //Give the player a paint gun
	{
		pPaintGun = (CWeaponPaintGun*)CreateEntityByName( "weapon_paintgun" );
		if ( pPaintGun )
		{
			DispatchSpawn( pPaintGun );

			if ( !pPaintGun->IsMarkedForDeletion() )
			{
				Weapon_Equip( pPaintGun );
			}
		}
	}

	//Activate all the paint powers if needed
	if( pPaintGun && bActivatePaintPowers )
	{
		pPaintGun->ActivatePaint(BOUNCE_POWER);
		pPaintGun->ActivatePaint(SPEED_POWER);
		//pPaintGun->ActivatePaint(REFLECT_POWER);
		pPaintGun->ActivatePaint(PORTAL_POWER);
		PaintPowerPickup( BOUNCE_POWER, this );
		PaintPowerPickup( SPEED_POWER, this );
		//PaintPowerPickup( REFLECT_POWER, this );
		PaintPowerPickup( PORTAL_POWER, this );
	}

	//Switch to the paint gun
	if( pPaintGun && bSwitchTo )
	{
		Weapon_Switch( pPaintGun );
	}
}


void CPortal_Player::GivePlayerPortalGun( bool bUpgraded, bool bSwitchTo )
{
	CBaseCombatWeapon *pWeapon = Weapon_OwnsThisType( "weapon_portalgun", 0 );
	CWeaponPortalgun *pPortalGun = NULL;

	//If the player already has a portal gun
	if( pWeapon )
	{
		pPortalGun = static_cast<CWeaponPortalgun*>( pWeapon );
	}
	else //Give the player a portal gun
	{
		pPortalGun = (CWeaponPortalgun*)CreateEntityByName( "weapon_portalgun" );
		if ( pPortalGun )
		{
			pPortalGun->SetLocalOrigin( GetLocalOrigin() );
			pPortalGun->AddSpawnFlags( SF_NORESPAWN );
			pPortalGun->SetSubType( 0 );

			DispatchSpawn( pPortalGun );

			if ( !pPortalGun->IsMarkedForDeletion() )
			{
				pPortalGun->SetCanFirePortal1();

				//Upgrade the portal gun if needed
				if( bUpgraded )
				{
					pPortalGun->SetCanFirePortal2();
				}

				//pPortalGun->Touch( this );
				Weapon_Equip( pPortalGun );
			}
		}
	}

	//Switch to the portal gun
	if( pPortalGun && bSwitchTo )
	{
		Weapon_Switch( pPortalGun );
	}
}

CON_COMMAND(give_portalgun, "gives the command user a portalgun")
{
	CPortal_Player *pPlayer = static_cast<CPortal_Player*>( UTIL_GetCommandClient() );

	pPlayer->GivePlayerPortalGun( false, true );

}

CON_COMMAND(give_paintgun, "Equips the player with a single portal portalgun.")
{
	CPortal_Player *pPlayer = static_cast<CPortal_Player*>( UTIL_GetCommandClient() );

	pPlayer->GivePlayerPaintGun( true, true );
}