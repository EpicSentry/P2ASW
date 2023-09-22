//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:		Player for SDK
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "sdk_player.h"
#include "sdk_shareddefs.h"
#include "predicted_viewmodel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

					CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name )
					{
					}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
};

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, bool bPredicted )
{
	CPVSFilter filter( (const Vector&) pPlayer->EyePosition() );
	if( bPredicted )
		filter.UsePredictionRules();
	
	// The player himself doesn't need to be sent his animation events 
	// unless cs_showanimstate wants to show them.
	//if ( asw_showanimstate.GetInt() == pPlayer->entindex() )
	//{
		//filter.RemoveRecipient( pPlayer );
	//}

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}


// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //

LINK_ENTITY_TO_CLASS( player, CSDK_Player );
PRECACHE_REGISTER(player);

extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );

BEGIN_SEND_TABLE_NOBASE( CSDK_Player, DT_SDKLocalPlayerExclusive )
	// send a hi-res origin to the local player for use in prediction
	SendPropVector	(SENDINFO(m_vecOrigin), -1,  SPROP_NOSCALE|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),

	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
//	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN ),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CSDK_Player, DT_SDKNonLocalPlayerExclusive )
	// send a lo-res origin to other players
	SendPropVector	(SENDINFO(m_vecOrigin), -1,  SPROP_COORD_MP_LOWPRECISION|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),

	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CSDK_Player, DT_SDK_Player )
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),	
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	SendPropExclude( "DT_BaseAnimating", "m_nNewSequenceParity" ),
	SendPropExclude( "DT_BaseAnimating", "m_nResetEventsParity" ),
	SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),
	
	// playeranimstate and clientside animation takes care of these on the client
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "sdklocaldata", 0, &REFERENCE_SEND_TABLE(DT_SDKLocalPlayerExclusive), SendProxy_SendLocalDataTable ),
	// Data that gets sent to all other players
	SendPropDataTable( "sdknonlocaldata", 0, &REFERENCE_SEND_TABLE(DT_SDKNonLocalPlayerExclusive), SendProxy_SendNonLocalDataTable ),
END_SEND_TABLE()

BEGIN_DATADESC( CSDK_Player )
END_DATADESC()

CSDK_Player::CSDK_Player()
{
	m_PlayerAnimState = CreateSDKPlayerAnimState( this );

	UseClientSideAnimation();

	m_angEyeAngles.Init();
}

CSDK_Player::~CSDK_Player()
{
	m_PlayerAnimState->Release();
}

//------------------------------------------------------------------------------
// Purpose : Send even though we don't have a model
//------------------------------------------------------------------------------
int CSDK_Player::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}


CSDK_Player *CSDK_Player::CreatePlayer( const char *className, edict_t *ed )
{
	CSDK_Player::s_PlayerEdict = ed;
	return (CSDK_Player*)CreateEntityByName( className );
}

void CSDK_Player::Precache()
{

	PrecacheModel( SDK_PLAYER_MODEL );

	BaseClass::Precache();
}

void CSDK_Player::Spawn()
{
	SetModel( SDK_PLAYER_MODEL );
	
	SetBloodColor( BLOOD_COLOR_RED );
	
	BaseClass::Spawn();

	EquipSuit( false ); // Ensure hud works

	SetMaxSpeed( SDK_DEFAULT_PLAYER_RUNSPEED );
	SetMoveType( MOVETYPE_WALK );

	SetSolid( SOLID_BBOX );
	RemoveSolidFlags( FSOLID_NOT_SOLID );
	AddSolidFlags( FSOLID_NOT_STANDABLE );

	SetHealth(150);

	pl.deadflag = false;
}

void CSDK_Player::DoAnimationEvent( PlayerAnimEvent_t event, bool bPredicted )
{
	TE_PlayerAnimEvent( this, event, bPredicted );	// Send to any clients who can see this guy.
	m_PlayerAnimState->DoAnimationEvent( event );
}

void CSDK_Player::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );

	m_PlayerAnimState->Update( EyeAngles()[YAW], EyeAngles()[PITCH] );

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();
}

void CSDK_Player::GiveDefaultItems()
{
	CBasePlayer::GiveAmmo( 30,	"pistol");
	CBasePlayer::GiveAmmo( 30,	"mp5");

	GiveNamedItem( "weapon_pistol" );
	GiveNamedItem( "weapon_mp5" );
}

void CSDK_Player::CreateViewModel( int index /*=0*/ )
{
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