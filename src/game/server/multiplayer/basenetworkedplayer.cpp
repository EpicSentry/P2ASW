#include "cbase.h"
#include "basenetworkedplayer.h"
#include "predicted_viewmodel.h"
#include "ilagcompensationmanager.h"
#include "basenetworkedragdoll.h"

//* **************** CTEPlayerAnimEvent* *********************

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer* pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( (const Vector&)pPlayer->EyePosition() );
	filter.RemoveRecipient( pPlayer );

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	// Transmit nData to support PLAYERANIMEVENT_CUSTOM at the cost of 4 extra bytes per TE
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

//* *************** CBaseNetworkedPlayer *******************

#ifndef PORTAL2_DLL
LINK_ENTITY_TO_CLASS( player, CBaseNetworkedPlayer );
#endif

extern void SendProxy_Origin( const SendProp* pProp, const void* pStruct, const void* pData, DVariant* pOut, int iElement, int objectID );

BEGIN_SEND_TABLE_NOBASE( CBaseNetworkedPlayer, DT_BaseNetworkedPlayerExclusive )
	// send a hi-res origin to the local player for use in prediction
	SendPropVector	(SENDINFO(m_vecOrigin), -1,  SPROP_NOSCALE|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CBaseNetworkedPlayer, DT_BaseNetworkedPlayerNonLocalExclusive )
	SendPropVector(SENDINFO(m_vecOrigin), CELL_BASEENTITY_ORIGIN_CELL_BITS, SPROP_CELL_COORD|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, CBaseEntity::SendProxy_CellOrigin ),
	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST(CBaseNetworkedPlayer, DT_BaseNetworkedPlayer )
	// These aren't needed either because we use client-side animation, or because they are being moved to the local/non-local table.
	SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseAnimating", "m_nNewSequenceParity" ),
	SendPropExclude( "DT_BaseAnimating", "m_nResetEventsParity" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),
	
	// New props
	SendPropBool( SENDINFO( m_bSpawnInterpCounter) ),
	SendPropEHandle( SENDINFO( m_hRagdoll ) ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "netplayer_localdata", 0, &REFERENCE_SEND_TABLE(DT_BaseNetworkedPlayerExclusive), SendProxy_SendLocalDataTable ),
	// Data that gets sent to all other players
	SendPropDataTable( "netplayer_nonlocaldata", 0, &REFERENCE_SEND_TABLE(DT_BaseNetworkedPlayerNonLocalExclusive), SendProxy_SendNonLocalDataTable ),
END_NETWORK_TABLE()


//* ****************** FUNCTIONS* *********************


CBaseNetworkedPlayer::CBaseNetworkedPlayer() {
	UseClientSideAnimation();
	SetPredictionEligible(true);
	m_bSpawnInterpCounter = false;
	m_angEyeAngles.Init();
	ragdoll_ent_name = "networked_ragdoll";
	
	MakeAnimState();
}
CBaseNetworkedPlayer::~CBaseNetworkedPlayer() {
	m_PlayerAnimState->Release();	
}

void CBaseNetworkedPlayer::UpdateOnRemove()
{
	RemoveRagdollEntity();
	BaseClass::UpdateOnRemove();
}

void CBaseNetworkedPlayer::Spawn()
{
	BaseClass::Spawn();

	// used to not interp players when they spawn
	m_bSpawnInterpCounter = !m_bSpawnInterpCounter;
	
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	RemoveRagdollEntity();
}

void CBaseNetworkedPlayer::CreateViewModel( int index )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel(index) )
		return;

	CPredictedViewModel* vm = (CPredictedViewModel*)CreateEntityByName( "predicted_viewmodel" );
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

void CBaseNetworkedPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	m_PlayerAnimState->DoAnimationEvent( event, nData ); // execute on server
	TE_PlayerAnimEvent( this, event, nData );	// transmit to clients
}

void CBaseNetworkedPlayer::PostThink()
{
	BaseClass::PostThink();

	// Keep the model upright; pose params will handle pitch aiming.
	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);

	m_angEyeAngles = EyeAngles();

	m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
}

void CBaseNetworkedPlayer::FireBullets ( const FireBulletsInfo_t &info )
{
	lagcompensation->StartLagCompensation( this, LAG_COMPENSATE_HITBOXES );
		BaseClass::FireBullets(info);
	lagcompensation->FinishLagCompensation( this );
}

void CBaseNetworkedPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	// show killer in death cam mode
	if( info.GetAttacker() && info.GetAttacker()->IsPlayer() && info.GetAttacker() != (CBaseEntity*)this )
	{
		m_hObserverTarget.Set( info.GetAttacker() ); 
		SetFOV( this, 0 ); // reset
	}
	else
		m_hObserverTarget.Set( NULL );

	// Create our own ragdoll...
	BecomeRagdollOnClient();

	// ...and employ a minor hack to stop CBaseCombatCharacter creating its own
	const_cast<CTakeDamageInfo*>(&info)->AddDamageType(DMG_REMOVENORAGDOLL);

	BaseClass::Event_Killed( info );

	AddEffects(EF_NODRAW);
}

void CBaseNetworkedPlayer::RemoveRagdollEntity()
{
	if ( m_hRagdoll.Get() )
		UTIL_RemoveImmediate( m_hRagdoll.Get() );	
}

void CBaseNetworkedPlayer::BecomeRagdollOnClient()
{
	// The the clientside entity makes its own ragdoll if it can
	EntityMessageBegin(this,true);
		WRITE_BYTE( BECOME_RAGDOLL );
	MessageEnd();
			
	// A dedicated entity is also created to ensure that clients who aren't in
	// PVS when the player dies or who connect later on see a ragdoll too
	RemoveRagdollEntity();

	CBaseNetworkedRagdoll* pRagdoll = (CBaseNetworkedRagdoll*)CreateEntityByName(ragdoll_ent_name);
	m_hRagdoll = pRagdoll;
	pRagdoll->m_hPlayer = this;

	pRagdoll->m_nModelIndex = m_nModelIndex;
		
	pRagdoll->m_vecForce = GetAbsVelocity() * 20;
	pRagdoll->m_nForceBone = m_nForceBone;

	pRagdoll->SetAbsOrigin( GetAbsOrigin() );
	pRagdoll->SetAbsAngles( GetAbsAngles() );

	pRagdoll->Spawn();
}