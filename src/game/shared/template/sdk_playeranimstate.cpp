//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "base_playeranimstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"

#include "sdk_playeranimstate.h"
#include "base_playeranimstate.h"
#include "datacache/imdlcache.h"

#ifdef CLIENT_DLL
#include "c_sdk_player.h"
#else
#include "sdk_player.h"
#endif
#include "weapon_sdkbase.h"
#include "sdk_fx_shared.h"

#define SDK_RUN_SPEED				320.0f
#define SDK_WALK_SPEED				75.0f
#define SDK_CROUCHWALK_SPEED		110.0f


#define FIRESEQUENCE_LAYER		(AIMSEQUENCE_LAYER+NUM_AIMSEQUENCE_LAYERS)
#define RELOADSEQUENCE_LAYER	(FIRESEQUENCE_LAYER + 1)
#define NUM_LAYERS_WANTED		(RELOADSEQUENCE_LAYER + 1)

ConVar mp_slammoveyaw( "mp_slammoveyaw", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, "Force movement yaw along an animation path." );
ConVar sdk_facefronttime( "sdk_facefronttime", "0.0", FCVAR_REPLICATED, "How many seconds before marine faces front when standing still." );
ConVar sdk_max_body_yaw( "sdk_max_body_yaw", "60", FCVAR_REPLICATED, "Max angle body yaw can turn either side before feet snap." );
ConVar sdk_feetyawrate("sdk_feetyawrate", "800",  FCVAR_REPLICATED, "How many degrees per second that we can turn our feet or upper body." );

float SnapYawTo( float flValue )
{
	float flSign = 1.0f;
	if ( flValue < 0.0f )
	{
		flSign = -1.0f;
		flValue = -flValue;
	}

	if ( flValue < 23.0f )
	{
		flValue = 0.0f;
	}
	else if ( flValue < 67.0f )
	{
		flValue = 45.0f;
	}
	else if ( flValue < 113.0f )
	{
		flValue = 90.0f;
	}
	else if ( flValue < 157 )
	{
		flValue = 135.0f;
	}
	else
	{
		flValue = 180.0f;
	}

	return ( flValue * flSign );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
// Output : CMultiPlayerAnimState*
//-----------------------------------------------------------------------------
CSDKPlayerAnimState* CreateSDKPlayerAnimState( CSDK_Player *pPlayer )
{
	MDLCACHE_CRITICAL_SECTION();

	// Setup the movement data.
	CModAnimConfig movementData;
	movementData.m_LegAnimType = LEGANIM_9WAY;
	movementData.m_flMaxBodyYawDegrees = sdk_max_body_yaw.GetFloat();
	movementData.m_bUseAimSequences = false;

	// Create animation state for this player.
	CSDKPlayerAnimState *pRet = new CSDKPlayerAnimState();
	pRet->Init( pPlayer, movementData );

	// Specific SDK player initialization.
	pRet->InitSDKAnimState( pPlayer );

	return pRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CSDKPlayerAnimState::CSDKPlayerAnimState()
{
	m_pSDKPlayer = NULL;
	m_bReloading = false;
	m_bPlayingEmoteGesture = false;
	m_bWeaponSwitching = false;
	m_fReloadPlaybackRate = 1.0f;
	m_flReloadBlendOut = 0.1f;
	m_flReloadBlendIn = 0.1f;

	// Don't initialize SDK specific variables here. Init them in InitSDKAnimState()
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CSDKPlayerAnimState::~CSDKPlayerAnimState()
{
}

//-----------------------------------------------------------------------------
// Purpose: Initialize Team Fortress specific animation state.
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CSDKPlayerAnimState::InitSDKAnimState( CSDK_Player *pPlayer )
{
	m_pSDKPlayer = pPlayer;
}

void CSDKPlayerAnimState::ComputeSequences( CStudioHdr *pStudioHdr )
{
	BaseClass::ComputeSequences(pStudioHdr);

	ComputeFireSequence();
	//ComputeMiscSequence();
	ComputeReloadSequence();
	ComputeWeaponSwitchSequence();	
}

void CSDKPlayerAnimState::ClearAnimationLayers()
{
	if ( !m_pOuter )
		return;

	m_pOuter->SetNumAnimOverlays( NUM_LAYERS_WANTED );
	for ( int i=0; i < m_pOuter->GetNumAnimOverlays(); i++ )
	{
		m_pOuter->GetAnimOverlay( i )->SetOrder( CBaseAnimatingOverlay::MAX_OVERLAYS );
#ifndef CLIENT_DLL
		m_pOuter->GetAnimOverlay( i )->m_fFlags = 0;
#endif
	}
}

void CSDKPlayerAnimState::ClearAnimationState()
{
	m_bJumping = false;
	m_bFiring = false;
	m_bReloading = false;
	m_bPlayingEmoteGesture = false;
	m_bWeaponSwitching = false;
	BaseClass::ClearAnimationState();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSDKPlayerAnimState::DoAnimationEvent( PlayerAnimEvent_t event )
{
	if( event == PLAYERANIMEVENT_ATTACK_PRIMARY )
	{
		// Regardless of what we're doing in the fire layer, restart it.
		m_flFireCycle = 0;
		m_iFireSequence = CalcFireLayerSequence( event );
		m_bFiring = m_iFireSequence != -1;

		if( !m_pSDKPlayer->GetActiveSDKWeapon() )
			return;
		FX_FireBullets(
				m_pSDKPlayer->entindex(),
				m_pSDKPlayer->Weapon_ShootPosition(),
				m_pSDKPlayer->EyeAngles() + m_pSDKPlayer->GetPunchAngle(),
				&(m_pSDKPlayer->GetActiveSDKWeapon()->GetSDKWpnData()),
				0,
				CBaseEntity::GetPredictionRandomSeed() & 255,
				m_pSDKPlayer->GetActiveSDKWeapon()->GetWeaponSpread()
		);
	}
	else if ( event == PLAYERANIMEVENT_JUMP )
	{
		// Play the jump animation.
		if (!m_bJumping)
		{
			m_bJumping = true;
			m_bFirstJumpFrame = true;
			m_flJumpStartTime = gpGlobals->curtime;
		}
	}
	else if ( event == PLAYERANIMEVENT_RELOAD )
	{
		m_iReloadSequence = CalcReloadLayerSequence();
		if ( m_iReloadSequence != -1 )
		{
			// clear other events that might be playing in our layer
			m_bWeaponSwitching = false;
			m_bPlayingEmoteGesture = false;
			bool bShortReloadAnim = false;

#if 0
#ifdef CLIENT_DLL
			C_ASW_Marine* pMarine = dynamic_cast<C_ASW_Marine*>(GetOuter());
#else
			CASW_Marine* pMarine = dynamic_cast<CASW_Marine*>(GetOuter());
#endif			
			float fReloadTime = 2.2f;
			CASW_Weapon* pActiveWeapon = pMarine->GetActiveASWWeapon();
			if (pActiveWeapon && pActiveWeapon->GetWeaponInfo())
			{
				fReloadTime = pActiveWeapon->GetWeaponInfo()->flReloadTime;

				if (pActiveWeapon->ASW_SelectWeaponActivity(ACT_RELOAD) == ACT_RELOAD_PISTOL)
					bShortReloadAnim = true;
			}
			if (pMarine)
				fReloadTime *= MarineSkills()->GetSkillBasedValueByMarine(pMarine, ASW_MARINE_SKILL_RELOADING);
#else
			float fReloadTime = 2.2f; // TODO: Add to weapon info
#endif // 0
						
			// calc playback rate needed to play the whole anim in this time
			// normal weapon reload anim is 48fps and 105 frames = 2.1875 seconds
			// normal weapon reload time in script is 2.2 seconds
			// pistol weapon reload anim is 38fps and 38 frames = 1 second
			// pistol weapon reload time in script is 1.0 seconds
			if (bShortReloadAnim)
				m_fReloadPlaybackRate = 1.0f / fReloadTime; //38.0f / (fReloadTime * 38.0f);
			else
				m_fReloadPlaybackRate = 105.6f / (fReloadTime * 48.0f);

			m_bReloading = true;			
			m_flReloadCycle = 0;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Activity CSDKPlayerAnimState::CalcMainActivity()
{
	if ( HandleJumping() )
	{
		return ACT_MP_JUMP;
	}
	else
	{
		Activity idealActivity = ACT_MP_STAND_IDLE;

		float flOuterSpeed = GetOuterXYSpeed();

		if ( m_pSDKPlayer->GetFlags() & FL_DUCKING )
		{
			if ( flOuterSpeed < MOVING_MINIMUM_SPEED )
			{
				idealActivity = ACT_MP_CROUCH_IDLE;		
			}
			else
			{
				idealActivity = ACT_MP_CROUCHWALK;		
			}
		}
		else
		{
			if ( flOuterSpeed > 0.1f )
			{
				idealActivity = ACT_MP_RUN;
			}
		}

		return idealActivity;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : actDesired - 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CSDKPlayerAnimState::TranslateActivity( Activity actDesired )
{
	Activity translateActivity = BaseClass::TranslateActivity( actDesired );

	if ( GetSDKPlayer()->GetActiveWeapon() )
	{
		translateActivity = GetSDKPlayer()->GetActiveWeapon()->ActivityOverride( translateActivity, false );
	}

	return translateActivity;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CSDKPlayerAnimState::ShouldResetMainSequence( int iCurrentSequence, int iNewSequence )
{
	return iCurrentSequence != iNewSequence;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CSDKPlayerAnimState::GetCurrentMaxGroundSpeed()
{
	// find the speed of currently playing directional sequences when full blended in
	CStudioHdr *pStudioHdr = GetOuter()->GetModelPtr();

	if ( pStudioHdr == NULL )
		return 1.0f;

	int iMoveX = GetOuter()->LookupPoseParameter( pStudioHdr, "move_x" );
	int iMoveY = GetOuter()->LookupPoseParameter( pStudioHdr, "move_y" );
	if ( iMoveX < 0 || iMoveY < 0 )
			return 1.0f;

	float prevX = GetOuter()->GetPoseParameter( iMoveX );
	float prevY = GetOuter()->GetPoseParameter( iMoveY );

	float d = sqrt( prevX * prevX + prevY * prevY );
	float newX, newY;
	if ( d == 0.0 )
	{ 
		newX = 1.0;
		newY = 0.0;
	}
	else
	{
		newX = prevX / d;
		newY = prevY / d;
	}	

	GetOuter()->SetPoseParameter( pStudioHdr, iMoveX, newX );
	GetOuter()->SetPoseParameter( pStudioHdr, iMoveY, newY );

	float flSequenceSpeed = GetOuter()->GetSequenceGroundSpeed( GetOuter()->GetSequence() );

	GetOuter()->SetPoseParameter( pStudioHdr, iMoveX, prevX );		// restore pose params
	GetOuter()->SetPoseParameter( pStudioHdr, iMoveY, prevY );

	return flSequenceSpeed;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CSDKPlayerAnimState::HandleJumping()
{
	if ( m_bJumping )
	{
		if ( m_bFirstJumpFrame )
		{
			m_bFirstJumpFrame = false;
			RestartMainSequence();	// Reset the animation.
		}

		// Don't check if he's on the ground for a sec.. sometimes the client still has the
		// on-ground flag set right when the message comes in.
		if (m_flJumpStartTime > gpGlobals->curtime)
			m_flJumpStartTime = gpGlobals->curtime;
		if ( gpGlobals->curtime - m_flJumpStartTime > 0.2f)
		{
			if ( m_pOuter->GetFlags() & FL_ONGROUND || GetOuter()->GetGroundEntity() != NULL)
			{
				m_bJumping = false;
				RestartMainSequence();	// Reset the animation.				
			}
		}
	}

	// Are we still jumping? If so, keep playing the jump animation.
	return m_bJumping;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CSDKPlayerAnimState::ShouldResetGroundSpeed( Activity oldActivity, Activity idealActivity )
{
	// If we went from idle to walk, reset the interpolation history.
	return ( (oldActivity == ACT_CROUCHIDLE || oldActivity == ACT_IDLE || oldActivity == ACT_IDLE_RELAXED
		|| oldActivity == ACT_MP_CROUCHWALK_ITEM1 || oldActivity == ACT_MP_STAND_ITEM1) && 
		(idealActivity == ACT_WALK || idealActivity == ACT_RUN_CROUCH || idealActivity == ACT_WALK_AIM_RIFLE
		|| idealActivity == ACT_WALK_AIM_PISTOL || idealActivity == ACT_RUN || idealActivity == ACT_RUN_AIM_RIFLE
		|| idealActivity == ACT_RUN_AIM_PISTOL || idealActivity == ACT_RUN_RELAXED || idealActivity == ACT_WALK_RELAXED
		|| idealActivity == ACT_MP_RUN_ITEM1 || idealActivity == ACT_MP_WALK_ITEM1) );
}

int CSDKPlayerAnimState::CalcFireLayerSequence(PlayerAnimEvent_t event)
{
	return SelectWeightedSequence( TranslateActivity(ACT_MP_ATTACK_STAND_PRIMARYFIRE) );
}

int CSDKPlayerAnimState::CalcWeaponSwitchLayerSequence()
{
	// single weapon switch anim
	int iWeaponSwitchSequence = m_pOuter->LookupSequence( "WeaponSwitch_smg1" );
	return iWeaponSwitchSequence;
}


int CSDKPlayerAnimState::CalcReloadLayerSequence()
{	
#ifdef ASW_TRANSLATE_ACTIVITIES_BY_ACTMOD
	return GetOuter()->SelectWeightedSequenceFromModifiers( ACT_MP_RELOAD_STAND, m_ActivityModifiers.Base(), m_ActivityModifiers.Count() );
#else
	return SelectWeightedSequence( TranslateActivity( ACT_MP_RELOAD_STAND ) );
#endif
}

void CSDKPlayerAnimState::ComputeWeaponSwitchSequence()
{
#ifdef CLIENT_DLL
	UpdateLayerSequenceGeneric( RELOADSEQUENCE_LAYER, m_bWeaponSwitching, m_flWeaponSwitchCycle, m_iWeaponSwitchSequence, false, 0, 0.5f );
#else
	// Server doesn't bother with different fire sequences.
#endif
}

int CSDKPlayerAnimState::CalcReloadSucceedLayerSequence()
{	
	return GetOuter()->SelectWeightedSequenceFromModifiers( ACT_RELOAD_SUCCEED, m_ActivityModifiers.Base(), m_ActivityModifiers.Count() );
}

int CSDKPlayerAnimState::CalcReloadFailLayerSequence()
{	
	return GetOuter()->SelectWeightedSequenceFromModifiers( ACT_RELOAD_FAIL, m_ActivityModifiers.Base(), m_ActivityModifiers.Count() );
}

void CSDKPlayerAnimState::UpdateLayerSequenceGeneric( int iLayer, bool &bEnabled,
					float &flCurCycle, int &iSequence, bool bWaitAtEnd,
					float fBlendIn, float fBlendOut, bool bMoveBlend, float fPlaybackRate, bool bUpdateCycle /* = true */ )
{
	if ( !bEnabled )
		return;

	CStudioHdr *hdr = GetOuter()->GetModelPtr();
	if ( !hdr )
		return;

	if ( iSequence < 0 || iSequence >= hdr->GetNumSeq() )
		return;

	// Increment the fire sequence's cycle.
	if ( bUpdateCycle )
	{
		flCurCycle += m_pOuter->GetSequenceCycleRate( hdr, iSequence ) * gpGlobals->frametime * fPlaybackRate;
	}

	// temp: if the sequence is looping, don't override it - we need better handling of looping anims, 
	//  especially in misc layer from melee (right now the same melee attack is looped manually in asw_melee_system.cpp)
	bool bLooping = m_pOuter->IsSequenceLooping( hdr, iSequence );

	if ( flCurCycle > 1 && !bLooping )
	{
		if ( iLayer == RELOADSEQUENCE_LAYER )
		{
			m_bPlayingEmoteGesture = false;
			m_bReloading = false;
		}
		if ( bWaitAtEnd )
		{
			flCurCycle = 1;
		}
		else
		{
			// Not firing anymore.
			bEnabled = false;
			iSequence = 0;
			return;
		}
	}

	// if this animation should blend out as we move, then check for dropping it completely since we're moving too fast
	float speed = 0;
	if (bMoveBlend)
	{
		Vector vel;
		GetOuterAbsVelocity( vel );

		float speed = vel.Length2D();

		if (speed > 50)
		{
			bEnabled = false;
			iSequence = 0;
			return;
		}
	}

	// Now dump the state into its animation layer.
	CAnimationLayer *pLayer = m_pOuter->GetAnimOverlay( iLayer );

	pLayer->SetCycle( flCurCycle );
	pLayer->SetSequence( iSequence );

	pLayer->SetPlaybackRate( fPlaybackRate );
	pLayer->SetWeight( 1.0f );
	if (iLayer == RELOADSEQUENCE_LAYER)
	{
		// blend this layer in and out for smooth reloading
		if (flCurCycle < fBlendIn && fBlendIn>0)
		{
			pLayer->SetWeight( clamp<float>(flCurCycle / fBlendIn,
				0.001f, 1.0f) );
		}
		else if (flCurCycle >= (1.0f - fBlendOut) && fBlendOut>0)
		{
			pLayer->SetWeight( clamp<float>((1.0f - flCurCycle) / fBlendOut,
				0.001f, 1.0f) );
		}
		else
		{
			pLayer->SetWeight( 1.0f );
		}
	}
	else
	{
		pLayer->SetWeight( 1.0f );			
	}
	if (bMoveBlend)		
	{
		// blend the animation out as we move faster
		if (speed <= 50)			
			pLayer->SetWeight( pLayer->GetWeight() * (50.0f - speed) / 50.0f );
	}

#ifndef CLIENT_DLL
	pLayer->m_fFlags |= ANIM_LAYER_ACTIVE;
#endif
	pLayer->SetOrder( iLayer );
}

void CSDKPlayerAnimState::ComputeFireSequence()
{
#ifdef CLIENT_DLL
	UpdateLayerSequenceGeneric( FIRESEQUENCE_LAYER, m_bFiring, m_flFireCycle, m_iFireSequence, false );
#else
	// Server doesn't bother with different fire sequences.
#endif
}

void CSDKPlayerAnimState::ComputeReloadSequence()
{
#ifdef CLIENT_DLL
	UpdateLayerSequenceGeneric( RELOADSEQUENCE_LAYER, m_bReloading, m_flReloadCycle, m_iReloadSequence, false, m_flReloadBlendIn, m_flReloadBlendOut, false, m_fReloadPlaybackRate );
#else
	// Server doesn't bother with different fire sequences.
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CSDKPlayerAnimState::SetOuterBodyYaw( float flValue )
{
	int body_yaw = GetOuter()->LookupPoseParameter( "body_yaw" );
	if ( body_yaw < 0 )
	{
		return 0;
	}

	SetOuterPoseParameter( body_yaw, flValue );
	return flValue;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : dt - 
//-----------------------------------------------------------------------------
void CSDKPlayerAnimState::EstimateYaw()
{
	Vector est_velocity;
	GetOuterAbsVelocity( est_velocity );

	float flLength = est_velocity.Length2D();
	if ( flLength > MOVING_MINIMUM_SPEED )
	{
		m_flGaitYaw = atan2( est_velocity[1], est_velocity[0] );
		m_flGaitYaw = RAD2DEG( m_flGaitYaw );
		m_flGaitYaw = AngleNormalize( m_flGaitYaw );
	}
}

extern ConVar mp_slammoveyaw;
void CSDKPlayerAnimState::ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr )
{
	int iMoveX = GetOuter()->LookupPoseParameter( pStudioHdr, "move_x" );
	int iMoveY = GetOuter()->LookupPoseParameter( pStudioHdr, "move_y" );
	if ( iMoveX < 0 || iMoveY < 0 )
		return;

	// Get the estimated movement yaw.
	EstimateYaw();

	// Get the view yaw.
	float flAngle = AngleNormalize( m_flEyeYaw );

	// Calc side to side turning - the view vs. movement yaw.
	float flYaw = flAngle - m_flGaitYaw;
	flYaw = AngleNormalize( -flYaw );

	// Get the current speed the character is running.
	bool bIsMoving;
	float flPlaybackRate = CalcMovementPlaybackRate( &bIsMoving );

	// Setup the 9-way blend parameters based on our speed and direction.
	Vector2D vecCurrentMoveYaw( 0.0f, 0.0f );
	if ( bIsMoving )
	{
		if ( mp_slammoveyaw.GetBool() )
		{
			flYaw = SnapYawTo( flYaw );
		}
		vecCurrentMoveYaw.x = cos( DEG2RAD( flYaw ) ) * flPlaybackRate;
		vecCurrentMoveYaw.y = -sin( DEG2RAD( flYaw ) ) * flPlaybackRate;
	}

	// Set the 9-way blend movement pose parameters.
	m_pOuter->SetPoseParameter( pStudioHdr, iMoveX, vecCurrentMoveYaw.x );
	m_pOuter->SetPoseParameter( pStudioHdr, iMoveY, -vecCurrentMoveYaw.y ); //Tony; flip it

	//m_DebugAnimData.m_vecMoveYaw = vecCurrentMoveYaw;
}


void CSDKPlayerAnimState::ComputePoseParam_BodyYaw()
{
	VPROF( "CBasePlayerAnimState::ComputePoseParam_BodyYaw" );

	// Find out which way he's running (m_flEyeYaw is the way he's looking).
	Vector vel;
	GetOuterAbsVelocity( vel );
	bool bIsMoving = vel.Length2D() > MOVING_MINIMUM_SPEED;
	float flFeetYawRate = GetFeetYawRate();
	float flMaxGap = m_AnimConfig.m_flMaxBodyYawDegrees;

#if 0
	if ( m_pMarine && m_pMarine->GetCurrentMeleeAttack() )		// don't allow upper body turning when melee attacking
	{
		flMaxGap = 0.0f;
	}
#endif // 0

	// If we just initialized this guy (maybe he just came into the PVS), then immediately
	// set his feet in the right direction, otherwise they'll spin around from 0 to the 
	// right direction every time someone switches spectator targets.
	if ( !m_bCurrentFeetYawInitialized )
	{
		m_bCurrentFeetYawInitialized = true;
		m_flGoalFeetYaw = m_flCurrentFeetYaw = m_flEyeYaw;
		m_flLastTurnTime = 0.0f;
	}
	else if ( bIsMoving )
	{
		// player is moving, feet yaw = aiming yaw
		if ( m_AnimConfig.m_LegAnimType == LEGANIM_9WAY || m_AnimConfig.m_LegAnimType == LEGANIM_8WAY )
		{
			// His feet point in the direction his eyes are, but they can run in any direction.
			m_flGoalFeetYaw = m_flEyeYaw;
		}
		else
		{
			m_flGoalFeetYaw = RAD2DEG( atan2( vel.y, vel.x ) );

			// If he's running backwards, flip his feet backwards.
			Vector vEyeYaw( cos( DEG2RAD( m_flEyeYaw ) ), sin( DEG2RAD( m_flEyeYaw ) ), 0 );
			Vector vFeetYaw( cos( DEG2RAD( m_flGoalFeetYaw ) ), sin( DEG2RAD( m_flGoalFeetYaw ) ), 0 );
			if ( vEyeYaw.Dot( vFeetYaw ) < -0.01 )
			{
				m_flGoalFeetYaw += 180;
			}
		}

	}
	else if ( (gpGlobals->curtime - m_flLastTurnTime) > sdk_facefronttime.GetFloat() )
	{
		// player didn't move & turn for quite some time
		m_flGoalFeetYaw = m_flEyeYaw;
	}
	else
	{
		// If he's rotated his view further than the model can turn, make him face forward.
		float flDiff = AngleNormalize(  m_flGoalFeetYaw - m_flEyeYaw );

		if ( fabs(flDiff) > flMaxGap )
		{
			m_flGoalFeetYaw = m_flEyeYaw;
			//flMaxGap = 180.0f / gpGlobals->frametime;
		}
	}

	m_flGoalFeetYaw = AngleNormalize( m_flGoalFeetYaw );

	if ( m_flCurrentFeetYaw != m_flGoalFeetYaw )
	{
		ConvergeAngles( m_flGoalFeetYaw, flFeetYawRate, flMaxGap,
			gpGlobals->frametime, m_flCurrentFeetYaw );

		m_flLastTurnTime = gpGlobals->curtime;
	}

	float flCurrentTorsoYaw = AngleNormalize( m_flEyeYaw - m_flCurrentFeetYaw );

	// Rotate entire body into position
	m_angRender[YAW] = m_flCurrentFeetYaw;
	m_angRender[PITCH] = m_angRender[ROLL] = 0;

	SetOuterBodyYaw( flCurrentTorsoYaw );
	g_flLastBodyYaw = flCurrentTorsoYaw;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSDKPlayerAnimState::ComputePoseParam_BodyPitch(CStudioHdr *pStudioHdr)
{
	// Get pitch from v_angle
	float flPitch = m_flEyePitch;
	if ( flPitch > 180.0f )
	{
		flPitch -= 360.0f;
	}
	flPitch = clamp( flPitch, -90, 90 );

	// See if we have a blender for pitch
	int pitch = GetOuter()->LookupPoseParameter( "body_pitch" );
	if ( pitch < 0 )
	{
		return;
	}

	SetOuterPoseParameter( pitch, -flPitch ); // NOTE: DOD model uses inverse pitch (in contrast with the alien swarm marine!)
}

float CSDKPlayerAnimState::GetFeetYawRate( void ) 
{ 
	return sdk_feetyawrate.GetFloat();
}