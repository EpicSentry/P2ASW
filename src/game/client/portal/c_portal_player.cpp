//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:		Player for .
//
//===========================================================================//

#include "cbase.h"
#include "vcollide_parse.h"
#include "c_portal_player.h"
#include "view.h"
#include "c_basetempentity.h"
#include "takedamageinfo.h"
#include "in_buttons.h"
#include "iviewrender_beams.h"
#include "r_efx.h"
#include "dlight.h"
#include "portalrender.h"
#include "toolframework/itoolframework.h"
#include "toolframework_client.h"
#include "tier1/keyvalues.h"
#include "ScreenSpaceEffects.h"
#include "portal_shareddefs.h"
#include "ivieweffects.h"		// for screenshake
#include "prop_portal_shared.h"
#include "materialsystem/imaterialvar.h"
#include "cdll_bounded_cvars.h"
#include "prediction.h"
#include "in_main.h"
#include "collisionutils.h"
#include "input.h"
#include "portal_util_shared.h"

#include "c_weapon_portalgun.h"
#include "dt_utlvector_recv.h"

// Don't alias here
#if defined( CPortal_Player )
#undef CPortal_Player	
#endif

ConVar cl_portal_camera_orientation_max_speed("cl_portal_camera_orientation_max_speed", "375.0f" );
ConVar cl_portal_camera_orientation_rate("cl_portal_camera_orientation_rate", "480.0f" );
ConVar cl_portal_camera_orientation_rate_base("cl_portal_camera_orientation_rate_base", "45.0f" );
ConVar cl_portal_camera_orientation_acceleration_rate("cl_portal_camera_orientation_acceleration_rate", "1000.0f" );
ConVar cl_skip_player_render_in_main_view("cl_skip_player_render_in_main_view", "1", FCVAR_ARCHIVE );
ConVar cl_auto_taunt_pip( "cl_auto_taunt_pip", "1", FCVAR_ARCHIVE );

extern ConVar sv_use_trace_duration;

#define C_Portal_Base2D C_Prop_Portal


#define REORIENTATION_RATE 120.0f
#define REORIENTATION_ACCELERATION_RATE 400.0f

#define ENABLE_PORTAL_EYE_INTERPOLATION_CODE


#define DEATH_CC_LOOKUP_FILENAME "materials/correction/cc_death.raw"
#define DEATH_CC_FADE_SPEED 0.05f


ConVar sv_zoom_stop_movement_threashold("sv_zoom_stop_movement_threashold", "4.0", FCVAR_REPLICATED, "Move command amount before breaking player out of toggle zoom." );
ConVar sv_zoom_stop_time_threashold("sv_zoom_stop_time_threashold", "5.0", FCVAR_REPLICATED, "Time amount before breaking player out of toggle zoom." );
ConVar cl_taunt_finish_rotate_cam("cl_taunt_finish_rotate_cam", "1", FCVAR_CHEAT);
ConVar cl_taunt_finish_speed("cl_taunt_finish_speed", "0.8f", FCVAR_CHEAT);

ConVar cl_reorient_in_air("cl_reorient_in_air", "1", FCVAR_ARCHIVE, "Allows the player to only reorient from being upside down while in the air." ); 


//=================================================================================
//
// Ragdoll Entity
//
class C_PortalRagdoll : public C_BaseFlex
{
public:

	DECLARE_CLASS( C_PortalRagdoll, C_BaseFlex );
	DECLARE_CLIENTCLASS();

	C_PortalRagdoll();
	~C_PortalRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

	virtual void SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights );

private:

	C_PortalRagdoll( const C_PortalRagdoll & ) {}

	void Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity );
	void CreatePortalRagdoll();

private:

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );

};

IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_PortalRagdoll, DT_PortalRagdoll, CPortalRagdoll )
RecvPropVector( RECVINFO(m_vecRagdollOrigin) ),
RecvPropEHandle( RECVINFO( m_hPlayer ) ),
RecvPropInt( RECVINFO( m_nModelIndex ) ),
RecvPropInt( RECVINFO(m_nForceBone) ),
RecvPropVector( RECVINFO(m_vecForce) ),
RecvPropVector( RECVINFO( m_vecRagdollVelocity ) ),
END_RECV_TABLE()


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
C_PortalRagdoll::C_PortalRagdoll()
{
	m_hPlayer = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
C_PortalRagdoll::~C_PortalRagdoll()
{
	( this );
}



//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSourceEntity - 
//-----------------------------------------------------------------------------
void C_PortalRagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
	if ( !pSourceEntity )
		return;

	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
		for ( int j=0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pDestEntry->watcher->GetDebugName() ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Setup vertex weights for drawing
//-----------------------------------------------------------------------------
void C_PortalRagdoll::SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights )
{
	// While we're dying, we want to mimic the facial animation of the player. Once they're dead, we just stay as we are.
	if ( (m_hPlayer && m_hPlayer->IsAlive()) || !m_hPlayer )
	{
		BaseClass::SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );
	}
	else if ( m_hPlayer )
	{
		m_hPlayer->SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
void C_PortalRagdoll::CreatePortalRagdoll()
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_Portal_Player *pPlayer = dynamic_cast<C_Portal_Player*>( m_hPlayer.Get() );

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		// Move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t *varMap = GetVarMapping();

		// This is the local player, so set them in a default
		// pose and slam their velocity, angles and origin
		SetAbsOrigin( /* m_vecRagdollOrigin : */ pPlayer->GetRenderOrigin() );			
		SetAbsAngles( pPlayer->GetRenderAngles() );
		SetAbsVelocity( m_vecRagdollVelocity );

		// Hack! Find a neutral standing pose or use the idle.
		int iSeq = LookupSequence( "ragdoll" );
		if ( iSeq == -1 )
		{
			Assert( false );
			iSeq = 0;
		}			
		SetSequence( iSeq );
		SetCycle( 0.0 );

		Interp_Reset( varMap );

		m_nBody = pPlayer->GetBody();
		SetModelIndex( m_nModelIndex );	
		// Make us a ragdoll..
		// TODO(Preston): Fix me!
//		m_nRenderFX = kRenderFxRagdoll;

		matrix3x4_t boneDelta0[MAXSTUDIOBONES];
		matrix3x4_t boneDelta1[MAXSTUDIOBONES];
		matrix3x4_t currentBones[MAXSTUDIOBONES];
		const float boneDt = 0.05f;

		pPlayer->GetRagdollInitBoneArrays( (matrix3x4a_t*)boneDelta0, (matrix3x4a_t*)boneDelta1, (matrix3x4a_t*)currentBones, boneDt );

		InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
// Output : IRagdoll*
//-----------------------------------------------------------------------------
IRagdoll* C_PortalRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : type - 
//-----------------------------------------------------------------------------
void C_PortalRagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		CreatePortalRagdoll();
	}
}


// specific to the local player
BEGIN_RECV_TABLE_NOBASE( C_Portal_Player, DT_PortalLocalPlayerExclusive )
	RecvPropVectorXY( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ), 0, C_BasePlayer::RecvProxy_LocalOriginXY ),
	RecvPropFloat( RECVINFO_NAME( m_vecNetworkOrigin[2], m_vecOrigin[2] ), 0, C_BasePlayer::RecvProxy_LocalOriginZ ),
	RecvPropVector( RECVINFO( m_vecViewOffset ) ),

	// FIXME: - Wonderland_War
	/*
	RecvPropQAngles( RECVINFO( m_vecCarriedObjectAngles ) ),
	RecvPropVector( RECVINFO( m_vecCarriedObject_CurPosToTargetPos )  ),
	RecvPropQAngles( RECVINFO( m_vecCarriedObject_CurAngToTargetAng ) ),

	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_EntityPortalledNetworkMessages ), C_Portal_Player::MAX_ENTITY_PORTALLED_NETWORK_MESSAGES, RecvPropDataTable(NULL, 0, 0, &REFERENCE_RECV_TABLE( DT_EntityPortalledNetworkMessage ) ) ),
	RecvPropInt( RECVINFO( m_iEntityPortalledNetworkMessageCount ) ),
	*/
END_RECV_TABLE()

// all players except the local player
BEGIN_RECV_TABLE_NOBASE( C_Portal_Player, DT_PortalNonLocalPlayerExclusive )
	RecvPropVectorXY( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ), 0, C_BasePlayer::RecvProxy_NonLocalCellOriginXY ),
	RecvPropFloat( RECVINFO_NAME( m_vecNetworkOrigin[2], m_vecOrigin[2] ), 0, C_BasePlayer::RecvProxy_NonLocalCellOriginZ ),
	RecvPropFloat( RECVINFO( m_vecViewOffset[0] ) ),
	RecvPropFloat( RECVINFO( m_vecViewOffset[1] ) ),
	RecvPropFloat( RECVINFO( m_vecViewOffset[2] ) ),
END_RECV_TABLE()


BEGIN_RECV_TABLE_NOBASE( CPortalPlayerShared, DT_PortalPlayerShared )
	RecvPropInt( RECVINFO( m_nPlayerCond ) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( player, C_Portal_Player );

IMPLEMENT_CLIENTCLASS_DT(C_Portal_Player, DT_Portal_Player, CPortal_Player)
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
	RecvPropInt( RECVINFO( m_iSpawnInterpCounter ) ),
	RecvPropBool( RECVINFO( m_bHeldObjectOnOppositeSideOfPortal ) ),
	RecvPropEHandle( RECVINFO( m_hHeldObjectPortal ) ),
	RecvPropBool( RECVINFO( m_bPitchReorientation ) ),
	RecvPropEHandle( RECVINFO( m_hPortalEnvironment ) ),
	RecvPropEHandle( RECVINFO( m_hSurroundingLiquidPortal ) ),
	RecvPropBool( RECVINFO( m_bIsHoldingSomething ) ),
	RecvPropDataTable( "portallocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_PortalLocalPlayerExclusive) ),
	RecvPropDataTable( "portalnonlocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_PortalNonLocalPlayerExclusive) ),

	RecvPropDataTable( RECVINFO_DT( m_Shared ), 0, &REFERENCE_RECV_TABLE( DT_PortalPlayerShared ) ),

	RecvPropFloat( RECVINFO( m_flMotionBlurAmount ) ),

	RecvPropFloat( RECVINFO( m_flHullHeight ) ),
	
	RecvPropBool( RECVINFO( m_bUseVMGrab ) ),
	RecvPropBool( RECVINFO( m_bUsingVMGrabState ) ),
	RecvPropEHandle( RECVINFO( m_hAttachedObject ) ),
	RecvPropEHandle( RECVINFO( m_hHeldObjectPortal ) ),
	RecvPropFloat( RECVINFO( m_flMotionBlurAmount ) ),

	RecvPropBool( RECVINFO( m_bWantsToSwapGuns ) ),

END_RECV_TABLE()


BEGIN_PREDICTION_DATA( C_Portal_Player )
END_PREDICTION_DATA()

static ConVar cl_playermodel( "cl_playermodel", "none", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "Default Player Model");

extern bool g_bUpsideDown;

//EHANDLE g_eKillTarget1;
//EHANDLE g_eKillTarget2;

void SpawnBlood (Vector vecSpot, const Vector &vecDir, int bloodColor, float flDamage);

C_Portal_Player::C_Portal_Player()
: m_iv_angEyeAngles( "C_Portal_Player::m_iv_angEyeAngles" ),
	m_iv_flHullHeight( "C_Portal_Player::m_iv_flHullHeight" ),
	m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator( "C_BaseEntity::m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator" ),
	m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator( "C_BaseEntity::m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator" ),
	//m_iv_vEyeOffset( "C_Portal_Player::m_iv_vEyeOffset" ),

	m_vInputVector( 0.0f, 0.0f, 0.0f ),
	m_flCachedJumpPowerTime( -FLT_MAX ),
	m_flSpeedDecelerationTime( 0.0f ),
	m_bJumpWasPressedWhenForced( false ),
	m_flPredictedJumpTime( 0.f ),
	m_bWantsToSwapGuns( false ),
	m_flMotionBlurAmount( -1.0f )
{
	m_PlayerAnimState = CreatePortalPlayerAnimState( this );

	m_iIDEntIndex = 0;
	m_iSpawnInterpCounterCache = 0;
	m_flDeathCCWeight = 0.0f;

	m_hRagdoll.Set( NULL );
	m_flStartLookTime = 0.0f;

	m_bHeldObjectOnOppositeSideOfPortal = false;
	m_hHeldObjectPortal = NULL;

	m_bPitchReorientation = false;
	m_fReorientationRate = 0.0f;

	m_angEyeAngles.Init();

	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );
	
	m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator.Setup( &m_vecCarriedObject_CurPosToTargetPos_Interpolated, INTERPOLATE_LINEAR_ONLY );
	m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator.Setup( &m_vecCarriedObject_CurAngToTargetAng_Interpolated, INTERPOLATE_LINEAR_ONLY );

	m_EntClientFlags |= ENTCLIENTFLAG_DONTUSEIK;
	m_blinkTimer.Invalidate();

	m_CCDeathHandle = INVALID_CLIENT_CCHANDLE;

	m_flUseKeyStartTime = -sv_use_trace_duration.GetFloat() - 1.0f;
	m_nUseKeyEntFoundCommandNum = -1;
	m_nUseKeyEntClearCommandNum = -1;
	m_nLastRecivedCommandNum = -1;
	m_hUseEntToSend = NULL;
	m_hUseEntThroughPortal = NULL;
	m_flAutoGrabLockOutTime = 0.0f;

	m_bForcingDrop = false;
	m_bUseVMGrab = false;
	m_bUsingVMGrabState = false;

	m_bUseWasDown = false;

	m_bForceFireNextPortal = false;

}

C_Portal_Player::~C_Portal_Player( void )
{
	if ( m_PlayerAnimState )
	{
		m_PlayerAnimState->Release();
	}

	g_pColorCorrectionMgr->RemoveColorCorrection( m_CCDeathHandle );
}

int C_Portal_Player::GetIDTarget() const
{
	return m_iIDEntIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Update this client's target entity
//-----------------------------------------------------------------------------
void C_Portal_Player::UpdateIDTarget()
{
	if ( !IsLocalPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show IDs in chase spec mode
	if ( GetObserverMode() == OBS_MODE_CHASE || 
		GetObserverMode() == OBS_MODE_DEATHCAM )
		return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(0), 1500, MainViewForward(0), vecEnd );
	VectorMA( MainViewOrigin(0), 10,   MainViewForward(0), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && (pEntity != this) )
		{
			m_iIDEntIndex = pEntity->entindex();
		}
	}
}

void C_Portal_Player::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr )
{
	Vector vecOrigin = ptr->endpos - vecDir * 4;

	float flDistance = 0.0f;

	if ( info.GetAttacker() )
	{
		flDistance = (ptr->endpos - info.GetAttacker()->GetAbsOrigin()).Length();
	}

	if ( m_takedamage )
	{
		AddMultiDamage( info, this );

		int blood = BloodColor();

		if ( blood != DONT_BLEED )
		{
			SpawnBlood( vecOrigin, vecDir, blood, flDistance );// a little surface blood.
			TraceBleed( flDistance, vecDir, ptr, info.GetDamageType() );
		}
	}
}

void C_Portal_Player::Initialize( void )
{
	m_headYawPoseParam = LookupPoseParameter(  "head_yaw" );
	GetPoseParameterRange( m_headYawPoseParam, m_headYawMin, m_headYawMax );

	m_headPitchPoseParam = LookupPoseParameter( "head_pitch" );
	GetPoseParameterRange( m_headPitchPoseParam, m_headPitchMin, m_headPitchMax );

	CStudioHdr *hdr = GetModelPtr();
	for ( int i = 0; i < hdr->GetNumPoseParameters() ; i++ )
	{
		SetPoseParameter( hdr, i, 0.0 );
	}
}

CStudioHdr *C_Portal_Player::OnNewModel( void )
{
	CStudioHdr *hdr = BaseClass::OnNewModel();

	Initialize( );

	return hdr;
}

//-----------------------------------------------------------------------------
/**
* Orient head and eyes towards m_lookAt.
*/
void C_Portal_Player::UpdateLookAt( void )
{
	// head yaw
	if (m_headYawPoseParam < 0 || m_headPitchPoseParam < 0)
		return;

	// This is buggy with dt 0, just skip since there is no work to do.
	if ( gpGlobals->frametime <= 0.0f )
		return;

	// Player looks at themselves through portals. Pick the portal we're turned towards.
	const int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	float *fPortalDot = (float *)stackalloc( sizeof( float ) * iPortalCount );
	float flLowDot = 1.0f;
	int iUsePortal = -1;

	// defaults if no portals are around
	Vector vPlayerForward;
	GetVectors( &vPlayerForward, NULL, NULL );
	Vector vCurLookTarget = EyePosition();

	if ( !IsAlive() )
	{
		m_viewtarget = EyePosition() + vPlayerForward*10.0f;
		return;
	}

	bool bNewTarget = false;
	if ( UTIL_IntersectEntityExtentsWithPortal( this ) != NULL )
	{
		// player is in a portal
		vCurLookTarget = EyePosition() + vPlayerForward*10.0f;
	}
	else if ( pPortals && pPortals[0] )
	{
		// Test through any active portals: This may be a shorter distance to the target
		for( int i = 0; i != iPortalCount; ++i )
		{
			CProp_Portal *pTempPortal = pPortals[i];

			if( pTempPortal && pTempPortal->m_bActivated && pTempPortal->m_hLinkedPortal.Get() )
			{
				Vector vEyeForward, vPortalForward;
				EyeVectors( &vEyeForward );
				pTempPortal->GetVectors( &vPortalForward, NULL, NULL );
				fPortalDot[i] = vEyeForward.Dot( vPortalForward );
				if ( fPortalDot[i] < flLowDot )
				{
					flLowDot = fPortalDot[i];
					iUsePortal = i;
				}
			}
		}

		if ( iUsePortal >= 0 )
		{
			C_Prop_Portal* pPortal = pPortals[iUsePortal];
			if ( pPortal )
			{
				vCurLookTarget = pPortal->MatrixThisToLinked()*vCurLookTarget;
				if ( vCurLookTarget != m_vLookAtTarget )
				{
					bNewTarget = true;
				}
			}
		}
	}
	else
	{
		// No other look targets, look straight ahead
		vCurLookTarget += vPlayerForward*10.0f;
	}

	// Figure out where we want to look in world space.
	QAngle desiredAngles;
	Vector to = vCurLookTarget - EyePosition();
	VectorAngles( to, desiredAngles );
	QAngle aheadAngles;
	VectorAngles( vCurLookTarget, aheadAngles );

	// Figure out where our body is facing in world space.
	QAngle bodyAngles( 0, 0, 0 );
	bodyAngles[YAW] = GetLocalAngles()[YAW];

	m_flLastBodyYaw = bodyAngles[YAW];

	// Set the head's yaw.
	float desiredYaw = AngleNormalize( desiredAngles[YAW] - bodyAngles[YAW] );
	desiredYaw = clamp( desiredYaw, m_headYawMin, m_headYawMax );

	float desiredPitch = AngleNormalize( desiredAngles[PITCH] );
	desiredPitch = clamp( desiredPitch, m_headPitchMin, m_headPitchMax );

	if ( bNewTarget )
	{
		m_flStartLookTime = gpGlobals->curtime;
	}

	float dt = (gpGlobals->frametime);
	float flSpeed	= 1.0f - ExponentialDecay( 0.7f, 0.033f, dt );

	m_flCurrentHeadYaw = m_flCurrentHeadYaw + flSpeed * ( desiredYaw - m_flCurrentHeadYaw );
	m_flCurrentHeadYaw	= AngleNormalize( m_flCurrentHeadYaw );
	SetPoseParameter( m_headYawPoseParam, m_flCurrentHeadYaw );	

	m_flCurrentHeadPitch = m_flCurrentHeadPitch + flSpeed * ( desiredPitch - m_flCurrentHeadPitch );
	m_flCurrentHeadPitch = AngleNormalize( m_flCurrentHeadPitch );
	SetPoseParameter( m_headPitchPoseParam, m_flCurrentHeadPitch );

	// This orients the eyes
	m_viewtarget = m_vLookAtTarget = vCurLookTarget;
}

extern ConVar cl_enable_remote_splitscreen;
extern ConVar sv_enableholdrotation;

// When set to true, VGui_OnSplitScreenStateChanged will NOT change the current system level.
extern bool g_bSuppressConfigSystemLevelDueToPIPTransitions;
static ConVar cl_suppress_config_system_level_changes_on_pip_transitions( "cl_suppress_config_system_level_changes_on_pip_transitions", "1", FCVAR_DEVELOPMENTONLY );

ConVar portal_use_player_avoidance( "portal_use_player_avoidance", "0", FCVAR_REPLICATED );
ConVar portal_max_separation_force ( "portal_max_separation_force", "256", FCVAR_DEVELOPMENTONLY );

extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

void C_Portal_Player::AvoidPlayers( CUserCmd *pCmd )
{
	// Don't test if the player doesn't exist or is dead.
	if ( IsAlive() == false )
		return;

	// Up vector.
	static Vector vecUp( 0.0f, 0.0f, 1.0f );

	Vector vecPlayerCenter	= GetAbsOrigin();
	Vector vecPlayerMin		= GetPlayerMins();
	Vector vecPlayerMax		= GetPlayerMaxs();
	float flZHeight = vecPlayerMax.z - vecPlayerMin.z;
	vecPlayerCenter.z += 0.5f * flZHeight;
	VectorAdd( vecPlayerMin, vecPlayerCenter, vecPlayerMin );
	VectorAdd( vecPlayerMax, vecPlayerCenter, vecPlayerMax );

	// Find an intersecting player or object.
	int nAvoidPlayerCount = 0;
	C_Portal_Player *pAvoidPlayerList[MAX_PLAYERS];

	C_Portal_Player *pIntersectPlayer = NULL;
	float flAvoidRadius = 0.0f;

	Vector vecAvoidCenter, vecAvoidMin, vecAvoidMax;
	// for ( int i = 0; i < pTeam->GetNumPlayers(); ++i )
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		// C_TFPlayer *pAvoidPlayer = static_cast< C_TFPlayer * >( pTeam->GetPlayer( i ) );
		C_Portal_Player *pAvoidPlayer = static_cast< C_Portal_Player * >( UTIL_PlayerByIndex(i) );
		if ( pAvoidPlayer == NULL )
			continue;
		
		// Is the avoid player me?
		if ( pAvoidPlayer == this )
			continue;

		// Save as list to check against for objects.
		pAvoidPlayerList[nAvoidPlayerCount] = pAvoidPlayer;
		++nAvoidPlayerCount;

		// Check to see if the avoid player is dormant.
		if ( pAvoidPlayer->IsDormant() )
			continue;

		// Is the avoid player solid?
		if ( pAvoidPlayer->IsSolidFlagSet( FSOLID_NOT_SOLID ) )
			continue;

		Vector t1, t2;

		vecAvoidCenter = pAvoidPlayer->GetAbsOrigin();
		vecAvoidMin = pAvoidPlayer->GetPlayerMins();
		vecAvoidMax = pAvoidPlayer->GetPlayerMaxs();
		flZHeight = vecAvoidMax.z - vecAvoidMin.z;
		vecAvoidCenter.z += 0.5f * flZHeight;
		VectorAdd( vecAvoidMin, vecAvoidCenter, vecAvoidMin );
		VectorAdd( vecAvoidMax, vecAvoidCenter, vecAvoidMax );

		if ( IsBoxIntersectingBox( vecPlayerMin, vecPlayerMax, vecAvoidMin, vecAvoidMax ) )
		{
			// Need to avoid this player.
			if ( !pIntersectPlayer )
			{
				pIntersectPlayer = pAvoidPlayer;
				break;
			}
		}
	}

	// Anything to avoid?
	if ( pIntersectPlayer == NULL )
	{
		// m_Shared.SetSeparation( false );
		// m_Shared.SetSeparationVelocity( vec3_origin );
		return;
	}

	// Calculate the push strength and direction.
	Vector vecDelta;

	// Avoid a player - they have precedence.
	VectorSubtract( pIntersectPlayer->WorldSpaceCenter(), vecPlayerCenter, vecDelta );

	Vector vRad = pIntersectPlayer->WorldAlignMaxs() - pIntersectPlayer->WorldAlignMins();
	vRad.z = 0;

	flAvoidRadius = vRad.Length();

	float flPushStrength = RemapValClamped( vecDelta.Length(), flAvoidRadius, 0, 0, portal_max_separation_force.GetInt() );

	// Check to see if we have enough push strength to make a difference.
	if ( flPushStrength < 0.01f )
		return;

	Vector vecPush;
	if ( GetAbsVelocity().Length2DSqr() > 0.1f )
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z = 0.0f;
		CrossProduct( vecUp, vecVelocity, vecPush );
		VectorNormalize( vecPush );
	}
	else
	{
		// We are not moving, but we're still intersecting.
		QAngle angView = pCmd->viewangles;
		angView.x = 0.0f;
		AngleVectors( angView, NULL, &vecPush, NULL );
	}

	// Move away from the other player/object.
	Vector vecSeparationVelocity;
	if ( vecDelta.Dot( vecPush ) < 0 )
	{
		vecSeparationVelocity = vecPush * flPushStrength;
	}
	else
	{
		vecSeparationVelocity = vecPush * -flPushStrength;
	}

	// Don't allow the max push speed to be greater than the max player speed.
	float flMaxPlayerSpeed = MaxSpeed();
	float flCropFraction = 1.33333333f;

	if ( ( GetFlags() & FL_DUCKING ) && ( GetGroundEntity() != NULL ) )
	{	
		flMaxPlayerSpeed *= flCropFraction;
	}	

	float flMaxPlayerSpeedSqr = flMaxPlayerSpeed * flMaxPlayerSpeed;

	if ( vecSeparationVelocity.LengthSqr() > flMaxPlayerSpeedSqr )
	{
		vecSeparationVelocity.NormalizeInPlace();
		VectorScale( vecSeparationVelocity, flMaxPlayerSpeed, vecSeparationVelocity );
	}

	QAngle vAngles = pCmd->viewangles;
	vAngles.x = 0;
	Vector currentdir;
	Vector rightdir;

	AngleVectors( vAngles, &currentdir, &rightdir, NULL );

	Vector vDirection = vecSeparationVelocity;

	VectorNormalize( vDirection );

	float fwd = currentdir.Dot( vDirection );
	float rt = rightdir.Dot( vDirection );

	float forward = fwd * flPushStrength;
	float side = rt * flPushStrength;

	//Msg( "fwd: %f - rt: %f - forward: %f - side: %f\n", fwd, rt, forward, side );

	// m_Shared.SetSeparation( true );
	// m_Shared.SetSeparationVelocity( vecSeparationVelocity );

	pCmd->forwardmove	+= forward;
	pCmd->sidemove		+= side;

	// Clamp the move to within legal limits, preserving direction. This is a little
	// complicated because we have different limits for forward, back, and side

	//Msg( "PRECLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );

	float flForwardScale = 1.0f;
	if ( pCmd->forwardmove > fabs( cl_forwardspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_forwardspeed.GetFloat() ) / pCmd->forwardmove;
	}
	else if ( pCmd->forwardmove < -fabs( cl_backspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_backspeed.GetFloat() ) / fabs( pCmd->forwardmove );
	}

	float flSideScale = 1.0f;
	if ( fabs( pCmd->sidemove ) > fabs( cl_sidespeed.GetFloat() ) )
	{
		flSideScale = fabs( cl_sidespeed.GetFloat() ) / fabs( pCmd->sidemove );
	}

	float flScale = MIN( flForwardScale, flSideScale );
	pCmd->forwardmove *= flScale;
	pCmd->sidemove *= flScale;

	//Msg( "Pforwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );
}

bool C_Portal_Player::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{
	if ( sv_enableholdrotation.GetBool() && !IsUsingVMGrab() )
	{
		if( m_bIsHoldingSomething && (pCmd->buttons & IN_ATTACK2) )
		{
			//stomp view angle changes. When holding right click and holding something, we remap mouse movement to rotate the object instead of our view
			pCmd->viewangles = m_vecOldViewAngles;
			engine->SetViewAngles( m_vecOldViewAngles );
		}
	}

	// if we are in coop and not split screen, do PIP if the remote_view button is pressed
	if ( GameRules()->IsMultiplayer() && !( IsSplitScreenPlayer() || GetSplitScreenPlayers().Count() > 0 ) )
	{
		bool bOtherPlayerIsTaunting = false;
		bool bIsOtherPlayerRemoteViewTaunt = false;

		if ( cl_auto_taunt_pip.GetBool() )
		{
			C_Portal_Player *pOtherPlayer = ToPortalPlayer( UTIL_OtherPlayer( this ) );
			if ( pOtherPlayer )
			{
				bOtherPlayerIsTaunting = pOtherPlayer->m_Shared.InCond( PORTAL_COND_TAUNTING );
				bIsOtherPlayerRemoteViewTaunt = pOtherPlayer->IsRemoteViewTaunt();
			}
		}

		bool bRemoteViewPressed = ( pCmd->buttons & IN_REMOTE_VIEW ) != 0;

		bool bUsingPIP = m_nTeamTauntState < TEAM_TAUNT_HAS_PARTNER && 
						 ( ( bOtherPlayerIsTaunting && !bIsOtherPlayerRemoteViewTaunt ) || ( !bOtherPlayerIsTaunting && bRemoteViewPressed ) );
		
		// Hack: Suppress changes of the current system level during this transition (see vgui_int.cpp, VGui_OnSplitScreenStateChanged()).
		g_bSuppressConfigSystemLevelDueToPIPTransitions = cl_suppress_config_system_level_changes_on_pip_transitions.GetBool();
				
		cl_enable_remote_splitscreen.SetValue( bUsingPIP );

		g_bSuppressConfigSystemLevelDueToPIPTransitions = false;
	}

	static QAngle angMoveAngle( 0.0f, 0.0f, 0.0f );

	bool bNoTaunt = true;
	if ( IsTaunting() )
	{
		pCmd->forwardmove = 0.0f;
		pCmd->sidemove = 0.0f;
		pCmd->upmove = 0.0f;
		pCmd->buttons = 0;
		pCmd->weaponselect = 0;

		VectorCopy( angMoveAngle, pCmd->viewangles );

		bNoTaunt = false;
	}
	else
	{
		VectorCopy( pCmd->viewangles, angMoveAngle );
	}

	if ( GetFOV() != GetDefaultFOV() )
	{
		if ( IsTaunting() )
		{
			// Pop out of zoom when I'm taunting
			pCmd->buttons &= ~IN_ZOOM;
			KeyUp( &in_zoom, NULL );
		}
		else if ( GetVehicle() != NULL )
		{
			pCmd->buttons &= ~IN_ZOOM;
			KeyUp( &in_zoom, NULL );
		}
		else
		{
			float fThreshold = sv_zoom_stop_movement_threashold.GetFloat();
			if ( gpGlobals->curtime > GetFOVTime() + sv_zoom_stop_time_threashold.GetFloat() && 
				 ( fabsf( pCmd->forwardmove ) > fThreshold ||  fabsf( pCmd->sidemove ) > fThreshold ) )
			{
				// Pop out of the zoom if we're moving
				pCmd->buttons &= ~IN_ZOOM;
				KeyUp( &in_zoom, NULL );
			}
		}
	}

	// Bump away from other players
	AvoidPlayers( pCmd );
	
	PollForUseEntity( pCmd );
	m_bUseWasDown = (pCmd->buttons & IN_USE) != 0;

	pCmd->player_held_entity = ( m_hUseEntToSend ) ? ( m_hUseEntToSend->entindex() ) : ( 0 );
	pCmd->held_entity_was_grabbed_through_portal = ( m_hUseEntThroughPortal ) ? ( m_hUseEntThroughPortal->entindex() ) : ( 0 );
	
	//pCmd->command_acknowledgements_pending = pCmd->command_number - engine->GetLastAcknowledgedCommand(); // Engine function doesn't exist in Alien Swarm :(
	pCmd->predictedPortalTeleportations = 0;
	// FIXME!!!
#if 0
	for( int i = 0; i != m_PredictedPortalTeleportations.Count(); ++i )
	{
		if( m_PredictedPortalTeleportations[i].iCommandNumber > pCmd->command_number )
			break;

		++pCmd->predictedPortalTeleportations;
	}
#endif
	if ( m_bForceFireNextPortal )
	{
		C_WeaponPortalgun *pPortalGun = dynamic_cast< C_WeaponPortalgun* >( GetActiveWeapon() );
		if ( pPortalGun )
		{
			if ( pPortalGun->GetLastFiredPortal() == 1 )
			{
				pCmd->buttons |= IN_ATTACK2;
			}
			else
			{
				pCmd->buttons |= IN_ATTACK;
			}
		}

		if ( pCmd->command_number != 0 )
		{
			m_bForceFireNextPortal = false;
		}
	}

	BaseClass::CreateMove( flInputSampleTime, pCmd );
	
	return bNoTaunt;
}

// Calls FindUseEntity every tick for a period of time
// I'm REALLY sorry about this. This will clean up quite a bit
// once grab controllers are on the client.
void C_Portal_Player::PollForUseEntity( CUserCmd *pCmd )
{
	// Record the last received non-zero command number. 
	// Non-zero is a sloppy way of telling which CreateMoves came from ExtraMouseSample :/
	if ( pCmd->command_number != 0 )
	{
		m_nLastRecivedCommandNum = pCmd->command_number;
	}
	else if ( m_nLastRecivedCommandNum == -1 )
	{
		// If we just constructed, don't run anything below during extra mouse samples
		// until we've set m_nLastRecivedCommandNum to the correct command number from the engine
		return;
	}

	// 82077: Polling for the use entity causes the vehicle view to cache at a time
	// when the attachments are incorrect... We dont need use in a vehicle
	// so this hack prevents that caching from happening and keeps the view smooth
	// The only time we have a vehicle is during the ending sequence so this shouldn't be a big issue.
	if ( IsX360() && GetVehicle() )
		return;

	// Get rid of the use ent if we've already sent the last one up to the server
	if ( m_nLastRecivedCommandNum == m_nUseKeyEntClearCommandNum )	
	{
		m_hUseEntToSend = NULL;
		m_hUseEntThroughPortal = NULL;
		m_nUseKeyEntFoundCommandNum = -1;
		m_nUseKeyEntClearCommandNum = -1;
	}

	bool bBasicUse = ( ( pCmd->buttons & IN_USE ) != 0 && m_bUseWasDown == false );

	C_BaseEntity *pUseEnt = NULL;
	C_Portal_Base2D *pUseThroughPortal = NULL;
	PollForUseEntity( bBasicUse, &pUseEnt, &pUseThroughPortal );	// Call the shared poll logic in portal_player_shared

	if ( pUseEnt )
	{
		m_hUseEntToSend = pUseEnt;
		m_hUseEntThroughPortal = pUseThroughPortal;

		m_flUseKeyStartTime = -sv_use_trace_duration.GetFloat() - 1.0f;
		// Record the tick we found this, so we can invalidate the handle after it is sent
		m_nUseKeyEntFoundCommandNum = m_nLastRecivedCommandNum;
		// If we caught this during 'extra mouse sample' calls, clear on the cmd after next, otherwise clear next command.
		m_nUseKeyEntClearCommandNum = m_nUseKeyEntFoundCommandNum + ((pCmd->command_number == 0) ? ( 2 ) : ( 1 ));
	}
	
	// If we already found an ent this tick, fake a use key down so the server
	// will treat this as a normal +use 
	if ( m_nLastRecivedCommandNum < m_nUseKeyEntClearCommandNum && m_hUseEntToSend.Get() && 
		 // HACK: don't do this behaviour through a portal for now (54969)
		 // This will be really awkward to fix for real with grab controllers living on the server. 
		 m_hUseEntThroughPortal.Get() == NULL )
	{
		pCmd->buttons |= IN_USE;
	}
}

void C_Portal_Player::ClientThink( void )
{
	//PortalEyeInterpolation.m_bNeedToUpdateEyePosition = true;

	Vector vForward;
	AngleVectors( GetLocalAngles(), &vForward );

	// Allow sprinting
	HandleSpeedChanges();

	FixTeleportationRoll();
	

	// If dead, fade in death CC lookup
	if ( m_CCDeathHandle != INVALID_CLIENT_CCHANDLE )
	{
		if ( m_lifeState != LIFE_ALIVE )
		{
			if ( m_flDeathCCWeight < 1.0f )
			{
				m_flDeathCCWeight += DEATH_CC_FADE_SPEED;
				clamp( m_flDeathCCWeight, 0.0f, 1.0f );
			}
		}
		else 
		{
			m_flDeathCCWeight = 0.0f;
		}
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCDeathHandle, m_flDeathCCWeight );
	}
	
	if( !cl_predict->GetInt() )
	{
		UpdatePaintedPower();
	}

	UpdateIDTarget();
}

void C_Portal_Player::FixTeleportationRoll( void )
{
	if( IsInAVehicle() ) //HL2 compatibility fix. do absolutely nothing to the view in vehicles
		return;

	if( !IsLocalPlayer() )
		return;

	// Normalize roll from odd portal transitions
	QAngle vAbsAngles = EyeAngles();


	Vector vCurrentForward, vCurrentRight, vCurrentUp;
	AngleVectors( vAbsAngles, &vCurrentForward, &vCurrentRight, &vCurrentUp );

	if ( vAbsAngles[ROLL] == 0.0f )
	{
		m_fReorientationRate = 0.0f;
		g_bUpsideDown = ( vCurrentUp.z < 0.0f );
		return;
	}

	bool bForcePitchReorient = ( vAbsAngles[ROLL] > 175.0f && vCurrentForward.z > 0.99f );
	bool bOnGround = ( GetGroundEntity() != NULL );

	if ( bForcePitchReorient )
	{
		m_fReorientationRate = REORIENTATION_RATE * ( ( bOnGround ) ? ( 2.0f ) : ( 1.0f ) );
	}
	else
	{
		// Don't reorient in air if they don't want to
		if ( !cl_reorient_in_air.GetBool() && !bOnGround )
		{
			g_bUpsideDown = ( vCurrentUp.z < 0.0f );
			return;
		}
	}

	if ( vCurrentUp.z < 0.75f )
	{
		m_fReorientationRate += gpGlobals->frametime * REORIENTATION_ACCELERATION_RATE;

		// Upright faster if on the ground
		float fMaxReorientationRate = REORIENTATION_RATE * ( ( bOnGround ) ? ( 2.0f ) : ( 1.0f ) );
		if ( m_fReorientationRate > fMaxReorientationRate )
			m_fReorientationRate = fMaxReorientationRate;
	}
	else
	{
		if ( m_fReorientationRate > REORIENTATION_RATE * 0.5f )
		{
			m_fReorientationRate -= gpGlobals->frametime * REORIENTATION_ACCELERATION_RATE;
			if ( m_fReorientationRate < REORIENTATION_RATE * 0.5f )
				m_fReorientationRate = REORIENTATION_RATE * 0.5f;
		}
		else if ( m_fReorientationRate < REORIENTATION_RATE * 0.5f )
		{
			m_fReorientationRate += gpGlobals->frametime * REORIENTATION_ACCELERATION_RATE;
			if ( m_fReorientationRate > REORIENTATION_RATE * 0.5f )
				m_fReorientationRate = REORIENTATION_RATE * 0.5f;
		}
	}

	if ( !m_bPitchReorientation && !bForcePitchReorient )
	{
		// Randomize which way we roll if we're completely upside down
		if ( vAbsAngles[ROLL] == 180.0f && RandomInt( 0, 1 ) == 1 )
		{
			vAbsAngles[ROLL] = -180.0f;
		}

		if ( vAbsAngles[ROLL] < 0.0f )
		{
			vAbsAngles[ROLL] += gpGlobals->frametime * m_fReorientationRate;
			if ( vAbsAngles[ROLL] > 0.0f )
				vAbsAngles[ROLL] = 0.0f;
			engine->SetViewAngles( vAbsAngles );
		}
		else if ( vAbsAngles[ROLL] > 0.0f )
		{
			vAbsAngles[ROLL] -= gpGlobals->frametime * m_fReorientationRate;
			if ( vAbsAngles[ROLL] < 0.0f )
				vAbsAngles[ROLL] = 0.0f;
			engine->SetViewAngles( vAbsAngles );
			m_angEyeAngles = vAbsAngles;
			m_iv_angEyeAngles.Reset(gpGlobals->curtime);
		}
	}
	else
	{
		if ( vAbsAngles[ROLL] != 0.0f )
		{
			if ( vCurrentUp.z < 0.2f )
			{
				float fDegrees = gpGlobals->frametime * m_fReorientationRate;
				if ( vCurrentForward.z > 0.0f )
				{
					fDegrees = -fDegrees;
				}

				// Rotate around the right axis
				VMatrix mAxisAngleRot = SetupMatrixAxisRot( vCurrentRight, fDegrees );

				vCurrentUp = mAxisAngleRot.VMul3x3( vCurrentUp );
				vCurrentForward = mAxisAngleRot.VMul3x3( vCurrentForward );

				VectorAngles( vCurrentForward, vCurrentUp, vAbsAngles );

				engine->SetViewAngles( vAbsAngles );
				m_angEyeAngles = vAbsAngles;
				m_iv_angEyeAngles.Reset(gpGlobals->curtime);
			}
			else
			{
				if ( vAbsAngles[ROLL] < 0.0f )
				{
					vAbsAngles[ROLL] += gpGlobals->frametime * m_fReorientationRate;
					if ( vAbsAngles[ROLL] > 0.0f )
						vAbsAngles[ROLL] = 0.0f;
					engine->SetViewAngles( vAbsAngles );
					m_angEyeAngles = vAbsAngles;
					m_iv_angEyeAngles.Reset(gpGlobals->curtime);
				}
				else if ( vAbsAngles[ROLL] > 0.0f )
				{
					vAbsAngles[ROLL] -= gpGlobals->frametime * m_fReorientationRate;
					if ( vAbsAngles[ROLL] < 0.0f )
						vAbsAngles[ROLL] = 0.0f;
					engine->SetViewAngles( vAbsAngles );
					m_angEyeAngles = vAbsAngles;
					m_iv_angEyeAngles.Reset(gpGlobals->curtime);
				}
			}
		}
	}

	// Keep track of if we're upside down for look control
	vAbsAngles = EyeAngles();
	AngleVectors( vAbsAngles, NULL, NULL, &vCurrentUp );

	if ( bForcePitchReorient )
		g_bUpsideDown = ( vCurrentUp.z < 0.0f );
	else
		g_bUpsideDown = false;
}

const QAngle& C_Portal_Player::GetRenderAngles()
{
	if ( IsRagdoll() )
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState->GetRenderAngles();
	}
}

void C_Portal_Player::UpdateClientSideAnimation( void )
{
	UpdateLookAt();

	// Update the animation data. It does the local check here so this works when using
	// a third-person camera (and we don't have valid player angles).
	if ( this == C_Portal_Player::GetLocalPortalPlayer() )
		m_PlayerAnimState->Update( EyeAngles()[YAW], m_angEyeAngles[PITCH] );
	else
		m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

	BaseClass::UpdateClientSideAnimation();
}

void C_Portal_Player::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if ( GetPredictable() && IsLocalPlayer( this ) )
	{
		if ( !prediction->IsFirstTimePredicted() )
			return;
	}

	MDLCACHE_CRITICAL_SECTION();
	m_PlayerAnimState->DoAnimationEvent( event, nData );
}

void C_Portal_Player::FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options )
{
	switch( event )
	{
	case AE_WPN_PRIMARYATTACK:
		{
			if ( IsLocalPlayer() )
			{
				m_bForceFireNextPortal = true;
			}
			break;
		}

	default:
		BaseClass::FireEvent( origin, angles,event, options );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_Portal_Player::DrawModel( int flags, const RenderableInstance_t& instance )
{
	if ( !m_bReadyToDraw )
		return 0;

	if( IsLocalPlayer() )
	{
		if ( !C_BasePlayer::ShouldDrawLocalPlayer() )
		{
			if ( !g_pPortalRender->IsRenderingPortal() )
				return 0;

			if( (g_pPortalRender->GetViewRecursionLevel() == 1) && (m_iForceNoDrawInPortalSurface != -1) ) //CPortalRender::s_iRenderingPortalView )
				return 0;
		}
	}

	return BaseClass::DrawModel(flags, instance);
}



//-----------------------------------------------------------------------------
// Should this object receive shadows?
//-----------------------------------------------------------------------------
bool C_Portal_Player::ShouldReceiveProjectedTextures( int flags )
{
	Assert( flags & SHADOW_FLAGS_PROJECTED_TEXTURE_TYPE_MASK );

	if ( IsEffectActive( EF_NODRAW ) )
		return false;

	if( flags & SHADOW_FLAGS_FLASHLIGHT )
	{
		return true;
	}

	return BaseClass::ShouldReceiveProjectedTextures( flags );
}

void C_Portal_Player::DoImpactEffect( trace_t &tr, int nDamageType )
{
	if ( GetActiveWeapon() )
	{
		GetActiveWeapon()->DoImpactEffect( tr, nDamageType );
		return;
	}

	BaseClass::DoImpactEffect( tr, nDamageType );
}

void C_Portal_Player::PreThink( void )
{
	QAngle vTempAngles = GetLocalAngles();

	if ( IsLocalPlayer() )
	{
		vTempAngles[PITCH] = EyeAngles()[PITCH];
	}
	else
	{
		vTempAngles[PITCH] = m_angEyeAngles[PITCH];
	}

	if ( vTempAngles[YAW] < 0.0f )
	{
		vTempAngles[YAW] += 360.0f;
	}

	SetLocalAngles( vTempAngles );

	BaseClass::PreThink();
	
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


	HandleSpeedChanges();

	FixPortalEnvironmentOwnership();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_Portal_Player::AddEntity( void )
{
//	BaseClass::AddEntity();

	QAngle vTempAngles = GetLocalAngles();
	vTempAngles[PITCH] = m_angEyeAngles[PITCH];

	SetLocalAngles( vTempAngles );

	// Zero out model pitch, blending takes care of all of it.
	SetLocalAnglesDim( X_INDEX, 0 );

	if( this != C_BasePlayer::GetLocalPlayer() )
	{
		if ( IsEffectActive( EF_DIMLIGHT ) )
		{
			int iAttachment = LookupAttachment( "anim_attachment_RH" );

			if ( iAttachment < 0 )
				return;

			Vector vecOrigin;
			QAngle eyeAngles = m_angEyeAngles;

			GetAttachment( iAttachment, vecOrigin, eyeAngles );

			Vector vForward;
			AngleVectors( eyeAngles, &vForward );

			trace_t tr;
			UTIL_TraceLine( vecOrigin, vecOrigin + (vForward * 200), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
		}
	}
}

ShadowType_t C_Portal_Player::ShadowCastType( void ) 
{
	// Drawing player shadows looks bad in first person when they get close to walls
	// It doesn't make sense to have shadows in the portal view, but not in the main view
	// So no shadows for the player
	return SHADOWS_NONE;
}

bool C_Portal_Player::ShouldDraw( void )
{
	if ( !IsAlive() )
		return false;

	//return true;

	//	if( GetTeamNumber() == TEAM_SPECTATOR )
	//		return false;

	if( IsLocalPlayer() && IsRagdoll() )
		return true;

	if ( IsRagdoll() )
		return false;

	return true;

	return BaseClass::ShouldDraw();
}

const QAngle& C_Portal_Player::EyeAngles()
{
	if ( IsLocalPlayer() && g_nKillCamMode == OBS_MODE_NONE )
	{
		return BaseClass::EyeAngles();
	}
	else
	{
		//C_BaseEntity *pEntity1 = g_eKillTarget1.Get();
		//C_BaseEntity *pEntity2 = g_eKillTarget2.Get();

		//Vector vLook = Vector( 0.0f, 0.0f, 0.0f );

		//if ( pEntity2 )
		//{
		//	vLook = pEntity1->GetAbsOrigin() - pEntity2->GetAbsOrigin();
		//	VectorNormalize( vLook );
		//}
		//else if ( pEntity1 )
		//{
		//	return BaseClass::EyeAngles();
		//	//vLook =  - pEntity1->GetAbsOrigin();
		//}

		//if ( vLook != Vector( 0.0f, 0.0f, 0.0f ) )
		//{
		//	VectorAngles( vLook, m_angEyeAngles );
		//}

		return m_angEyeAngles;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
// Output : IRagdoll*
//-----------------------------------------------------------------------------
IRagdoll* C_Portal_Player::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		C_PortalRagdoll *pRagdoll = static_cast<C_PortalRagdoll*>( m_hRagdoll.Get() );
		if ( !pRagdoll )
			return NULL;

		return pRagdoll->GetIRagdoll();
	}
	else
	{
		return NULL;
	}
}


void C_Portal_Player::PlayerPortalled( C_Prop_Portal *pEnteredPortal )
{
	if( pEnteredPortal )
	{
		m_bPortalledMessagePending = true;
		m_PendingPortalMatrix = pEnteredPortal->MatrixThisToLinked();

		if( IsLocalPlayer() )
			g_pPortalRender->EnteredPortal( pEnteredPortal );
	}
}

void C_Portal_Player::OnPreDataChanged( DataUpdateType_t type )
{
	Assert( m_pPortalEnvironment_LastCalcView == m_hPortalEnvironment.Get() );
	PreDataChanged_Backup.m_hPortalEnvironment = m_hPortalEnvironment;
	PreDataChanged_Backup.m_hSurroundingLiquidPortal = m_hSurroundingLiquidPortal;
	PreDataChanged_Backup.m_qEyeAngles = m_iv_angEyeAngles.GetCurrent();

	BaseClass::OnPreDataChanged( type );
}

void C_Portal_Player::FixPortalEnvironmentOwnership( void )
{
	CPortalSimulator *pExistingSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity( this );
	CProp_Portal *pPortalEnvironment = m_hPortalEnvironment;
	CPortalSimulator *pNewSimulator = pPortalEnvironment ? &pPortalEnvironment->m_PortalSimulator : NULL;
	if( pExistingSimulator != pNewSimulator )
	{
		if( pExistingSimulator )
		{
			pExistingSimulator->ReleaseOwnershipOfEntity( this );
		}

		if( pNewSimulator )
		{
			pNewSimulator->TakeOwnershipOfEntity( this );
		}
	}
}


void C_Portal_Player::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if( m_hSurroundingLiquidPortal != PreDataChanged_Backup.m_hSurroundingLiquidPortal )
	{
		CLiquidPortal_InnerLiquidEffect *pLiquidEffect = (CLiquidPortal_InnerLiquidEffect *)g_pScreenSpaceEffects->GetScreenSpaceEffect( "LiquidPortal_InnerLiquid" );
		if( pLiquidEffect )
		{
			C_Func_LiquidPortal *pSurroundingPortal = m_hSurroundingLiquidPortal.Get();
			if( pSurroundingPortal != NULL )
			{
				C_Func_LiquidPortal *pOldSurroundingPortal = PreDataChanged_Backup.m_hSurroundingLiquidPortal.Get();
				if( pOldSurroundingPortal != pSurroundingPortal->m_hLinkedPortal.Get() )
				{
					pLiquidEffect->m_pImmersionPortal = pSurroundingPortal;
					pLiquidEffect->m_bFadeBackToReality = false;
				}
				else
				{
					pLiquidEffect->m_bFadeBackToReality = true;
					pLiquidEffect->m_fFadeBackTimeLeft = pLiquidEffect->s_fFadeBackEffectTime;
				}
			}
			else
			{
				pLiquidEffect->m_pImmersionPortal = NULL;
				pLiquidEffect->m_bFadeBackToReality = false;
			}
		}		
	}

	DetectAndHandlePortalTeleportation();

	if ( type == DATA_UPDATE_CREATED )
	{
		// Load color correction lookup for the death effect
		m_CCDeathHandle = g_pColorCorrectionMgr->AddColorCorrection( DEATH_CC_LOOKUP_FILENAME );
		if ( m_CCDeathHandle != INVALID_CLIENT_CCHANDLE )
		{
			g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCDeathHandle, 0.0f );
		}

		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

	UpdateVisibility();
	
	// Set held objects to draw in the view model
	if ( IsUsingVMGrab() && m_hAttachedObject.Get() && m_hOldAttachedObject == NULL )
	{
		m_hOldAttachedObject = m_hAttachedObject;
		m_hAttachedObject.Get()->UpdateVisibility();

		if ( GameRules()->IsMultiplayer() == false )
		{
			m_hAttachedObject.Get()->RenderWithViewModels( true );
		}
	}
	else if ( ( !IsUsingVMGrab() || m_hAttachedObject.Get() == NULL ) && m_hOldAttachedObject.Get() )
	{
		if ( GameRules()->IsMultiplayer() == false )
		{
			m_hOldAttachedObject.Get()->RenderWithViewModels( false );
		}
		m_hOldAttachedObject.Get()->UpdateVisibility();
		m_hOldAttachedObject = NULL;
		m_flAutoGrabLockOutTime = gpGlobals->curtime;
	}
}

//CalcView() gets called between OnPreDataChanged() and OnDataChanged(), and these changes need to be known about in both before CalcView() gets called, and if CalcView() doesn't get called
bool C_Portal_Player::DetectAndHandlePortalTeleportation( void )
{
	if( m_bPortalledMessagePending )
	{
		m_bPortalledMessagePending = false;

		//C_Prop_Portal *pOldPortal = PreDataChanged_Backup.m_hPortalEnvironment.Get();
		//Assert( pOldPortal );
		//if( pOldPortal )
		{
			Vector ptNewPosition = GetNetworkOrigin();

			UTIL_Portal_PointTransform( m_PendingPortalMatrix, PortalEyeInterpolation.m_vEyePosition_Interpolated, PortalEyeInterpolation.m_vEyePosition_Interpolated );
			UTIL_Portal_PointTransform( m_PendingPortalMatrix, PortalEyeInterpolation.m_vEyePosition_Uninterpolated, PortalEyeInterpolation.m_vEyePosition_Uninterpolated );

			PortalEyeInterpolation.m_bEyePositionIsInterpolating = true;

			UTIL_Portal_AngleTransform( m_PendingPortalMatrix, m_qEyeAngles_LastCalcView, m_angEyeAngles );
			m_angEyeAngles.x = AngleNormalize( m_angEyeAngles.x );
			m_angEyeAngles.y = AngleNormalize( m_angEyeAngles.y );
			m_angEyeAngles.z = AngleNormalize( m_angEyeAngles.z );
			m_iv_angEyeAngles.Reset(gpGlobals->curtime); //copies from m_angEyeAngles

			if( engine->IsPlayingDemo() )
			{				
				pl.v_angle = m_angEyeAngles;		
				engine->SetViewAngles( pl.v_angle );
			}

			engine->ResetDemoInterpolation();
			if( IsLocalPlayer() ) 
			{
				//DevMsg( "FPT: %.2f %.2f %.2f\n", m_angEyeAngles.x, m_angEyeAngles.y, m_angEyeAngles.z );
				SetLocalAngles( m_angEyeAngles );
			}

			m_PlayerAnimState->Teleport ( &ptNewPosition, &GetNetworkAngles(), this );

			// Reorient last facing direction to fix pops in view model lag
			for ( int i = 0; i < MAX_VIEWMODELS; i++ )
			{
				CBaseViewModel *vm = GetViewModel( i );
				if ( !vm )
					continue;

				UTIL_Portal_VectorTransform( m_PendingPortalMatrix, vm->m_vecLastFacing, vm->m_vecLastFacing );
			}
		}
		m_bPortalledMessagePending = false;
	}

	return false;
}

/*bool C_Portal_Player::ShouldInterpolate( void )
{
if( !IsInterpolationEnabled() )
return false;

return BaseClass::ShouldInterpolate();
}*/


void C_Portal_Player::PostDataUpdate( DataUpdateType_t updateType )
{
	// C_BaseEntity assumes we're networking the entity's angles, so pretend that it
	// networked the same value we already have.
	SetNetworkAngles( GetLocalAngles() );

	if ( m_iSpawnInterpCounter != m_iSpawnInterpCounterCache )
	{
		MoveToLastReceivedPosition( true );
		ResetLatched();
		m_iSpawnInterpCounterCache = m_iSpawnInterpCounter;
	}

	BaseClass::PostDataUpdate( updateType );
}

float C_Portal_Player::GetFOV( void )
{
	//Find our FOV with offset zoom value
	float flFOVOffset = C_BasePlayer::GetFOV(); //+ GetZoom();

	// Clamp FOV in MP
	int min_fov = GetMinFOV();

	// Don't let it go too low
	flFOVOffset = MAX( min_fov, flFOVOffset );

	return flFOVOffset;
}

//=========================================================
// Autoaim
// set crosshair position to point to enemey
//=========================================================
Vector C_Portal_Player::GetAutoaimVector( float flDelta )
{
	// Never autoaim a predicted weapon (for now)
	Vector	forward;
	AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
	return	forward;
}


void C_Portal_Player::HandleSpeedChanges( void )
{
}

C_BaseAnimating *C_Portal_Player::BecomeRagdollOnClient()
{
	// Let the C_CSRagdoll entity do this.
	// m_builtRagdoll = true;
	return NULL;
}

void C_Portal_Player::UpdatePortalEyeInterpolation( void )
{
#ifdef ENABLE_PORTAL_EYE_INTERPOLATION_CODE
	//PortalEyeInterpolation.m_bEyePositionIsInterpolating = false;
	if( PortalEyeInterpolation.m_bUpdatePosition_FreeMove )
	{
		PortalEyeInterpolation.m_bUpdatePosition_FreeMove = false;

		C_Prop_Portal *pOldPortal = PreDataChanged_Backup.m_hPortalEnvironment.Get();
		if( pOldPortal )
		{
			UTIL_Portal_PointTransform( pOldPortal->MatrixThisToLinked(), PortalEyeInterpolation.m_vEyePosition_Interpolated, PortalEyeInterpolation.m_vEyePosition_Interpolated );
			//PortalEyeInterpolation.m_vEyePosition_Interpolated = pOldPortal->m_matrixThisToLinked * PortalEyeInterpolation.m_vEyePosition_Interpolated;

			//Vector vForward;
			//m_hPortalEnvironment.Get()->GetVectors( &vForward, NULL, NULL );

			PortalEyeInterpolation.m_vEyePosition_Interpolated = EyeFootPosition();

			PortalEyeInterpolation.m_bEyePositionIsInterpolating = true;
		}
	}

	if( IsInAVehicle() )
		PortalEyeInterpolation.m_bEyePositionIsInterpolating = false;

	if( !PortalEyeInterpolation.m_bEyePositionIsInterpolating )
	{
		PortalEyeInterpolation.m_vEyePosition_Uninterpolated = EyeFootPosition();
		PortalEyeInterpolation.m_vEyePosition_Interpolated = PortalEyeInterpolation.m_vEyePosition_Uninterpolated;
		return;
	}

	Vector vThisFrameUninterpolatedPosition = EyeFootPosition();

	//find offset between this and last frame's uninterpolated movement, and apply this as freebie movement to the interpolated position
	PortalEyeInterpolation.m_vEyePosition_Interpolated += (vThisFrameUninterpolatedPosition - PortalEyeInterpolation.m_vEyePosition_Uninterpolated);
	PortalEyeInterpolation.m_vEyePosition_Uninterpolated = vThisFrameUninterpolatedPosition;

	Vector vDiff = vThisFrameUninterpolatedPosition - PortalEyeInterpolation.m_vEyePosition_Interpolated;
	float fLength = vDiff.Length();
	float fFollowSpeed = gpGlobals->frametime * 100.0f;
	const float fMaxDiff = 150.0f;
	if( fLength > fMaxDiff )
	{
		//camera lagging too far behind, give it a speed boost to bring it within maximum range
		fFollowSpeed = fLength - fMaxDiff;
	}
	else if( fLength < fFollowSpeed )
	{
		//final move
		PortalEyeInterpolation.m_bEyePositionIsInterpolating = false;
		PortalEyeInterpolation.m_vEyePosition_Interpolated = vThisFrameUninterpolatedPosition;
		return;
	}

	if ( fLength > 0.001f )
	{
		vDiff *= (fFollowSpeed/fLength);
		PortalEyeInterpolation.m_vEyePosition_Interpolated += vDiff;
	}
	else
	{
		PortalEyeInterpolation.m_vEyePosition_Interpolated = vThisFrameUninterpolatedPosition;
	}



#else
	PortalEyeInterpolation.m_vEyePosition_Interpolated = BaseClass::EyePosition();
#endif
}

Vector C_Portal_Player::EyePosition()
{
	return PortalEyeInterpolation.m_vEyePosition_Interpolated;  
}

Vector C_Portal_Player::EyeFootPosition( const QAngle &qEyeAngles )
{
#if 0
	static int iPrintCounter = 0;
	++iPrintCounter;
	if( iPrintCounter == 50 )
	{
		QAngle vAbsAngles = qEyeAngles;
		DevMsg( "Eye Angles: %f %f %f\n", vAbsAngles.x, vAbsAngles.y, vAbsAngles.z );
		iPrintCounter = 0;
	}
#endif

	//interpolate between feet and normal eye position based on view roll (gets us wall/ceiling & ceiling/ceiling teleportations without an eye position pop)
	float fFootInterp = fabs(qEyeAngles[ROLL]) * ((1.0f/180.0f) * 0.75f); //0 when facing straight up, 0.75 when facing straight down
	return (BaseClass::EyePosition() - (fFootInterp * m_vecViewOffset)); //TODO: Find a good Up vector for this rolled player and interpolate along actual eye/foot axis
}

void C_Portal_Player::CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov )
{
	DetectAndHandlePortalTeleportation();
	//if( DetectAndHandlePortalTeleportation() )
	//	DevMsg( "Teleported within OnDataChanged\n" );

	m_iForceNoDrawInPortalSurface = -1;
	bool bEyeTransform_Backup = m_bEyePositionIsTransformedByPortal;
	m_bEyePositionIsTransformedByPortal = false; //assume it's not transformed until it provably is
	UpdatePortalEyeInterpolation();

	QAngle qEyeAngleBackup = EyeAngles();
	Vector ptEyePositionBackup = EyePosition();
	C_Prop_Portal *pPortalBackup = m_hPortalEnvironment.Get();

	if ( m_lifeState != LIFE_ALIVE )
	{
		if ( g_nKillCamMode != 0 )
		{
			return;
		}

		Vector origin = EyePosition();

		C_BaseEntity* pRagdoll = m_hRagdoll.Get();

		if ( pRagdoll )
		{
			origin = pRagdoll->GetAbsOrigin();
#if !PORTAL_HIDE_PLAYER_RAGDOLL
			origin.z += VEC_DEAD_VIEWHEIGHT.z; // look over ragdoll, not through
#endif //PORTAL_HIDE_PLAYER_RAGDOLL
		}

		BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );

		eyeOrigin = origin;

		Vector vForward; 
		AngleVectors( eyeAngles, &vForward );

		VectorNormalize( vForward );
#if !PORTAL_HIDE_PLAYER_RAGDOLL
		VectorMA( origin, -CHASE_CAM_DISTANCE, vForward, eyeOrigin );
#endif //PORTAL_HIDE_PLAYER_RAGDOLL

		Vector WALL_MIN( -WALL_OFFSET, -WALL_OFFSET, -WALL_OFFSET );
		Vector WALL_MAX( WALL_OFFSET, WALL_OFFSET, WALL_OFFSET );

		trace_t trace; // clip against world
		C_BaseEntity::PushEnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
		UTIL_TraceHull( origin, eyeOrigin, WALL_MIN, WALL_MAX, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trace );
		C_BaseEntity::PopEnableAbsRecomputations();

		if (trace.fraction < 1.0)
		{
			eyeOrigin = trace.endpos;
		}
		
		// in multiplayer we want to make sure we get the screenshakes and stuff
		if ( GameRules()->IsMultiplayer() )
			CalcPortalView( eyeOrigin, eyeAngles );

	}
	else
	{
		IClientVehicle *pVehicle; 
		pVehicle = GetVehicle();

		if ( !pVehicle )
		{
			if ( IsObserver() )
			{
				CalcObserverView( eyeOrigin, eyeAngles, fov );
			}
			else
			{
				CalcPlayerView( eyeOrigin, eyeAngles, fov );
				if( m_hPortalEnvironment.Get() != NULL )
				{
					//time for hax
					m_bEyePositionIsTransformedByPortal = bEyeTransform_Backup;
					CalcPortalView( eyeOrigin, eyeAngles );
				}
			}
		}
		else
		{
			CalcVehicleView( pVehicle, eyeOrigin, eyeAngles, zNear, zFar, fov );
		}
	}

	m_qEyeAngles_LastCalcView = qEyeAngleBackup;
	m_ptEyePosition_LastCalcView = ptEyePositionBackup;
	m_pPortalEnvironment_LastCalcView = pPortalBackup;
}

void C_Portal_Player::SetLocalViewAngles( const QAngle &viewAngles )
{
	// Nothing
	if ( engine->IsPlayingDemo() )
		return;
	BaseClass::SetLocalViewAngles( viewAngles );
}

void C_Portal_Player::SetViewAngles( const QAngle& ang )
{
	BaseClass::SetViewAngles( ang );

	if ( engine->IsPlayingDemo() )
	{
		pl.v_angle = ang;
	}
}

void C_Portal_Player::CalcPortalView( Vector &eyeOrigin, QAngle &eyeAngles )
{
	if ( !prediction->InPrediction() )
	{
		// FIXME: Move into prediction
		view->DriftPitch();
	}

	//although we already ran CalcPlayerView which already did these copies, they also fudge these numbers in ways we don't like, so recopy
	VectorCopy( EyePosition(), eyeOrigin );
	VectorCopy( EyeAngles(), eyeAngles );
	
	VectorAdd( eyeAngles, m_Local.m_vecPunchAngle, eyeAngles );

	if ( !prediction->InPrediction() )
	{
		GetViewEffects()->CalcShake();
		GetViewEffects()->ApplyShake( eyeOrigin, eyeAngles, 1.0 );
	}

	if( !prediction->InPrediction() )
	{
		SmoothViewOnStairs( eyeOrigin );
	}

	C_Prop_Portal *pPortal = m_hPortalEnvironment.Get();
	assert( pPortal );

	C_Prop_Portal *pRemotePortal = pPortal->m_hLinkedPortal;
	if( !pRemotePortal )
	{
		return; //no hacks possible/necessary
	}

	Vector ptPortalCenter;
	Vector vPortalForward;

	ptPortalCenter = pPortal->GetNetworkOrigin();
	pPortal->GetVectors( &vPortalForward, NULL, NULL );
	float fPortalPlaneDist = vPortalForward.Dot( ptPortalCenter );

	bool bOverrideSpecialEffects = false; //sometimes to get the best effect we need to kill other effects that are simply for cleanliness

	float fEyeDist = vPortalForward.Dot( eyeOrigin ) - fPortalPlaneDist;
	bool bTransformEye = false;
	if( fEyeDist < 0.0f ) //eye behind portal
	{
		if( pPortal->m_PortalSimulator.EntityIsInPortalHole( this ) ) //player standing in portal
		{
			bTransformEye = true;
		}
		else if( vPortalForward.z < -0.01f ) //there's a weird case where the player is ducking below a ceiling portal. As they unduck their eye moves beyond the portal before the code detects that they're in the portal hole.
		{
			Vector ptPlayerOrigin = GetAbsOrigin();
			float fOriginDist = vPortalForward.Dot( ptPlayerOrigin ) - fPortalPlaneDist;

			if( fOriginDist > 0.0f )
			{
				float fInvTotalDist = 1.0f / (fOriginDist - fEyeDist); //fEyeDist is negative
				Vector ptPlaneIntersection = (eyeOrigin * fOriginDist * fInvTotalDist) - (ptPlayerOrigin * fEyeDist * fInvTotalDist);
				Assert( fabs( vPortalForward.Dot( ptPlaneIntersection ) - fPortalPlaneDist ) < 0.01f );

				Vector vIntersectionTest = ptPlaneIntersection - ptPortalCenter;

				Vector vPortalRight, vPortalUp;
				pPortal->GetVectors( NULL, &vPortalRight, &vPortalUp );

				if( (vIntersectionTest.Dot( vPortalRight ) <= PORTAL_HALF_WIDTH) &&
					(vIntersectionTest.Dot( vPortalUp ) <= PORTAL_HALF_HEIGHT) )
				{
					bTransformEye = true;
				}
			}
		}		
	}

	if( bTransformEye )
	{
		m_bEyePositionIsTransformedByPortal = true;

		//DevMsg( 2, "transforming portal view from <%f %f %f> <%f %f %f>\n", eyeOrigin.x, eyeOrigin.y, eyeOrigin.z, eyeAngles.x, eyeAngles.y, eyeAngles.z );

		VMatrix matThisToLinked = pPortal->MatrixThisToLinked();
		UTIL_Portal_PointTransform( matThisToLinked, eyeOrigin, eyeOrigin );
		UTIL_Portal_AngleTransform( matThisToLinked, eyeAngles, eyeAngles );

		//DevMsg( 2, "transforming portal view to   <%f %f %f> <%f %f %f>\n", eyeOrigin.x, eyeOrigin.y, eyeOrigin.z, eyeAngles.x, eyeAngles.y, eyeAngles.z );

		if ( IsToolRecording() )
		{
			static EntityTeleportedRecordingState_t state;

			KeyValues *msg = new KeyValues( "entity_teleported" );
			msg->SetPtr( "state", &state );
			state.m_bTeleported = false;
			state.m_bViewOverride = true;
			state.m_vecTo = eyeOrigin;
			state.m_qaTo = eyeAngles;
			MatrixInvert( matThisToLinked.As3x4(), state.m_teleportMatrix );

			// Post a message back to all IToolSystems
			Assert( (int)GetToolHandle() != 0 );
			ToolFramework_PostToolMessage( GetToolHandle(), msg );

			msg->deleteThis();
		}

		bOverrideSpecialEffects = true;
	}
	else
	{
		m_bEyePositionIsTransformedByPortal = false;
	}

	if( bOverrideSpecialEffects )
	{		
		m_iForceNoDrawInPortalSurface = ((pRemotePortal->m_bIsPortal2)?(2):(1));
		pRemotePortal->m_fStaticAmount = 0.0f;
	}
}

extern float g_fMaxViewModelLag;
void C_Portal_Player::CalcViewModelView( const Vector& eyeOrigin, const QAngle& eyeAngles)
{
	/*
	if ( UTIL_IntersectEntityExtentsWithPortal( this ) )
		g_fMaxViewModelLag = 0.0f;
	else
		g_fMaxViewModelLag = 1.5f;
		*/
	for ( int i = 0; i < MAX_VIEWMODELS; i++ )
	{
		CBaseViewModel *vm = GetViewModel( i );
		if ( !vm )
			continue;
		
		vm->CalcViewModelView( this, eyeOrigin, eyeAngles );
	}
}

bool LocalPlayerIsCloseToPortal( void )
{
	return C_Portal_Player::GetLocalPlayer()->IsCloseToPortal();
}


bool C_Portal_Player::RenderLocalScreenSpaceEffect( PortalScreenSpaceEffect effect, IMatRenderContext *pRenderContext, int x, int y, int w, int h )
{
	C_Portal_Player *pLocalPlayer = C_Portal_Player::GetLocalPlayer();
	if( pLocalPlayer )
	{
		return pLocalPlayer->RenderScreenSpaceEffect( effect, pRenderContext, x, y, w, h );
	}

	return false;
}

static void SetRenderTargetAndViewPort(ITexture *rt)
{
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->SetRenderTarget(rt);
	if ( rt )
	{
		pRenderContext->Viewport(0,0,rt->GetActualWidth(),rt->GetActualHeight());
	}
}

bool C_Portal_Player::ScreenSpacePaintEffectIsActive() const
{
	// The reference is valid and the referenced particle system is also valid
	return m_PaintScreenSpaceEffect.IsValid() && m_PaintScreenSpaceEffect->IsValid();
}

void C_Portal_Player::SetScreenSpacePaintEffectColors( IMaterialVar* pColor1, IMaterialVar* pColor2 ) const
{
	const Color visualColor = MapPowerToVisualColor( m_PortalLocal.m_PaintedPowerType );
	Vector vColor1( visualColor.r(), visualColor.g(), visualColor.b() );
	Vector vColor2 = vColor1;
	for( unsigned i = 0; i < 3; ++i )
	{
		vColor2[i] = clamp( vColor2[i] - 15.0f, 0, 255 );
	}

	vColor1.NormalizeInPlace();
	vColor2.NormalizeInPlace();

	pColor1->SetVecValue( vColor1.x, vColor1.y, vColor1.z );
	pColor2->SetVecValue( vColor2.x, vColor2.y, vColor2.z );
}


bool C_Portal_Player::RenderScreenSpacePaintEffect( IMatRenderContext *pRenderContext )
{
	if( ScreenSpacePaintEffectIsActive() )
	{
		pRenderContext->PushRenderTargetAndViewport();
		ITexture* pDestRenderTarget = materials->FindTexture( "_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET );
		SetRenderTargetAndViewPort( pDestRenderTarget );
		pRenderContext->ClearColor4ub( 128, 128, 0, 0 );
		pRenderContext->ClearBuffers( true, false, false );
		RenderableInstance_t instance;
		instance.m_nAlpha = 255;
		m_PaintScreenSpaceEffect->DrawModel( 1, instance );

		/*
		if( IsGameConsole() )
		{
			pRenderContext->CopyRenderTargetToTextureEx( pDestRenderTarget, 0, NULL, NULL );
		}
		*/

		pRenderContext->PopRenderTargetAndViewport();

		return true;
	}

	return false;
}

void C_Portal_Player::InvalidatePaintEffects()
{
	// Remove paint screen space effect
	if( m_PaintScreenSpaceEffect.IsValid() )
	{
		m_PaintScreenSpaceEffect->StopEmission();
		STEAMWORKS_TESTSECRETALWAYS();
		m_PaintScreenSpaceEffect = NULL;
	}

	// commenting out the 3rd person drop effect
	/*
	// Remove paint drip effect
	if( m_PaintDripEffect.IsValid() )
	{
		m_PaintDripEffect->StopEmission();
		m_PaintDripEffect = NULL;
	}
	*/
}


bool C_Portal_Player::RenderScreenSpaceEffect( PortalScreenSpaceEffect effect, IMatRenderContext *pRenderContext, int x, int y, int w, int h )
{
	bool result = false;
	switch( effect )
	{
	case PAINT_SCREEN_SPACE_EFFECT:
		result = RenderScreenSpacePaintEffect( pRenderContext );
		break;
	}

	return result;
}

void C_Portal_Player::ItemPreFrame( void )
{
	if ( GetFlags() & FL_FROZEN )
		return;

	BaseClass::ItemPreFrame();
	ManageHeldObject();
}

// Only runs in MP. Creates a fake held object and uses a clientside grab controller
// to move it so the held object looks responsive under lag conditions.
void C_Portal_Player::ManageHeldObject()
{
	Assert ( GameRules()->IsMultiplayer() );


	CBaseEntity *pPlayerAttached = m_hAttachedObject.Get();

	//cleanup invalid clones.
	{
		if( (m_pHeldEntityClone != NULL) && 
			((pPlayerAttached == NULL) || !IsUsingVMGrab() || (m_pHeldEntityClone->m_hOriginal != pPlayerAttached)) )
		{
			//cloning wrong entity or don't want a clone
			if( GetGrabController().GetAttached() == m_pHeldEntityClone )
			{
				bool bOldForce = m_bForcingDrop;
				m_bForcingDrop = true;
				GetGrabController().DetachEntity( false );
				m_bForcingDrop = bOldForce;
			}
			UTIL_Remove( m_pHeldEntityClone );
			m_pHeldEntityClone = NULL;
		}

		if( (m_pHeldEntityThirdpersonClone != NULL) && 
			((pPlayerAttached == NULL) || !IsUsingVMGrab() || (m_pHeldEntityThirdpersonClone->m_hOriginal != pPlayerAttached)) )
		{
			//cloning wrong entity or don't want a clone
			if( GetGrabController().GetAttached() == m_pHeldEntityThirdpersonClone )
			{
				bool bOldForce = m_bForcingDrop;
				m_bForcingDrop = true;
				GetGrabController().DetachEntity( false );
				m_bForcingDrop = bOldForce;
			}
			UTIL_Remove( m_pHeldEntityThirdpersonClone );
			m_pHeldEntityThirdpersonClone = NULL;
		}
	}

	//create clones if necessary
	if( pPlayerAttached && IsUsingVMGrab() )
	{
		if( m_pHeldEntityClone == NULL )
		{
			m_pHeldEntityClone = new C_PlayerHeldObjectClone;
			if ( m_pHeldEntityClone )
			{
				if( !m_pHeldEntityClone->InitClone( pPlayerAttached, this ) )
				{
					UTIL_Remove( m_pHeldEntityClone );
					m_pHeldEntityClone = NULL;
				}
			}
		}

		if( m_pHeldEntityThirdpersonClone == NULL )
		{
			m_pHeldEntityThirdpersonClone = new C_PlayerHeldObjectClone;
			if ( m_pHeldEntityThirdpersonClone )
			{
				if( !m_pHeldEntityThirdpersonClone->InitClone( pPlayerAttached, this, false, m_pHeldEntityClone ) )
				{
					UTIL_Remove( m_pHeldEntityThirdpersonClone );
					m_pHeldEntityThirdpersonClone = NULL;
				}
			}
		}
	}


	//ensure correct entity is attached
	{
		C_BaseEntity *pShouldBeAttached = NULL;

		//figure out exactly what should be attached
		if( pPlayerAttached )
		{
			pPlayerAttached->SetPredictionEligible( true ); //open the floodgates of possible predictability. 
															//Not sure if there's a completely sane way to invert this operation for all possible cross sections of predictables and things you can pick up, so I simply wont.

			if( IsUsingVMGrab() && m_pHeldEntityClone )
			{
				pShouldBeAttached = m_pHeldEntityClone;
			}
			else
			{
				pShouldBeAttached = pPlayerAttached;
			}
		}

		//swap it in if necessary
		if( GetGrabController().GetAttached() != pShouldBeAttached )
		{
			//clear out offset history
			m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator.ClearHistory();
			m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator.ClearHistory();

			//release whatever the grab controller is attached to
			if( GetGrabController().GetAttached() )
			{
				if( (GetGrabController().GetAttached() != pPlayerAttached) && (GetGrabController().GetAttached() != m_pHeldEntityClone) )
				{
					GetGrabController().DetachUnknownEntity(); //the entity it has is not something we gave to it (or our player attached object invalidated under our feet)
				}
				else
				{
					bool bOldForce = m_bForcingDrop;
					m_bForcingDrop = true;
					GetGrabController().DetachEntity( false ); //we know the entity it's holding on to is valid, detach normally
					m_bForcingDrop = bOldForce;
				}
			}

			//if something should be attached, attach it now
			if( pShouldBeAttached )
			{
				GetGrabController().SetIgnorePitch( true );
				GetGrabController().SetAngleAlignment( 0.866025403784 );
				GetGrabController().AttachEntity( this, pShouldBeAttached, pShouldBeAttached->VPhysicsGetObject(), false, vec3_origin, false );
			}
		}
	}

	//only adding these on first-predicted frames to keep old results consistent, and ease into new changes
	if( pPlayerAttached && (!prediction->InPrediction() || prediction->IsFirstTimePredicted()) )
	{
		m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator.AddToHead( gpGlobals->curtime, &m_vecCarriedObject_CurPosToTargetPos, true );
		m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator.AddToHead( gpGlobals->curtime, &m_vecCarriedObject_CurAngToTargetAng, true );
		
		//need to interpolate these so they clear out old data. No direct access to clearing functions
		m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator.Interpolate( gpGlobals->curtime );
		m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator.Interpolate( gpGlobals->curtime );
	}

	//update the grab controller
	if ( GetGrabController().GetAttached() )
	{
		m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator.Interpolate( gpGlobals->curtime );
		m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator.Interpolate( gpGlobals->curtime );
		GetGrabController().m_attachedAnglesPlayerSpace = m_vecCarriedObjectAngles;
		GetGrabController().UpdateObject( this, 12 );
		GetGrabController().ClientApproachTarget( this );
	}
}
