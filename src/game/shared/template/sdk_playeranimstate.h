//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_PLAYERANIMSTATE_H
#define SDK_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"
#include "base_playeranimstate.h"

#if defined( CLIENT_DLL )
class C_SDK_Player;
#define CSDK_Player C_SDK_Player
#else
class CSDK_Player;
#endif

enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_ATTACK_PRIMARY,
	PLAYERANIMEVENT_ATTACK_SECONDARY,
	PLAYERANIMEVENT_ATTACK_GRENADE,
	PLAYERANIMEVENT_RELOAD,
	PLAYERANIMEVENT_RELOAD_LOOP,
	PLAYERANIMEVENT_RELOAD_END,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_SWIM,
	PLAYERANIMEVENT_DIE,
	PLAYERANIMEVENT_FLINCH_CHEST,
	PLAYERANIMEVENT_FLINCH_HEAD,
	PLAYERANIMEVENT_FLINCH_LEFTARM,
	PLAYERANIMEVENT_FLINCH_RIGHTARM,
	PLAYERANIMEVENT_FLINCH_LEFTLEG,
	PLAYERANIMEVENT_FLINCH_RIGHTLEG,
	PLAYERANIMEVENT_DOUBLEJUMP,

	// Cancel.
	PLAYERANIMEVENT_CANCEL,
	PLAYERANIMEVENT_SPAWN,

	// Snap to current yaw exactly
	PLAYERANIMEVENT_SNAP_YAW,

	PLAYERANIMEVENT_CUSTOM,				// Used to play specific activities
	PLAYERANIMEVENT_CUSTOM_GESTURE,
	PLAYERANIMEVENT_CUSTOM_SEQUENCE,	// Used to play specific sequences
	PLAYERANIMEVENT_CUSTOM_GESTURE_SEQUENCE,

	// TF Specific. Here until there's a derived game solution to this.
	PLAYERANIMEVENT_ATTACK_PRE,
	PLAYERANIMEVENT_ATTACK_POST,
	PLAYERANIMEVENT_GRENADE1_DRAW,
	PLAYERANIMEVENT_GRENADE2_DRAW,
	PLAYERANIMEVENT_GRENADE1_THROW,
	PLAYERANIMEVENT_GRENADE2_THROW,
	PLAYERANIMEVENT_VOICE_COMMAND_GESTURE,

	// Tony; some SDK ones now too.
	PLAYERANIMEVENT_STAND_TO_PRONE,
	PLAYERANIMEVENT_CROUCH_TO_PRONE,
	PLAYERANIMEVENT_PRONE_TO_STAND,
	PLAYERANIMEVENT_PRONE_TO_CROUCH,

	PLAYERANIMEVENT_COUNT
};

// Gesture Slots.
enum
{
	GESTURE_SLOT_ATTACK_AND_RELOAD,
	GESTURE_SLOT_GRENADE,
	GESTURE_SLOT_JUMP,
	GESTURE_SLOT_SWIM,
	GESTURE_SLOT_FLINCH,
	GESTURE_SLOT_VCD,
	GESTURE_SLOT_CUSTOM,

	GESTURE_SLOT_COUNT,
};

#define GESTURE_SLOT_INVALID	-1

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CSDKPlayerAnimState : public CBasePlayerAnimState
{
public:
	
	DECLARE_CLASS( CSDKPlayerAnimState, CBasePlayerAnimState );

	CSDKPlayerAnimState();
	CSDKPlayerAnimState( CBasePlayer *pPlayer, CModAnimConfig &movementData );
	~CSDKPlayerAnimState();

	void InitSDKAnimState( CSDK_Player *pPlayer );
	CSDK_Player *GetSDKPlayer( void )							{ return m_pSDKPlayer; }

	virtual void DoAnimationEvent( PlayerAnimEvent_t event );
	virtual void ComputeSequences( CStudioHdr *pStudioHdr );
	virtual void ClearAnimationLayers();
	virtual void ClearAnimationState();

	virtual bool ShouldResetMainSequence( int iCurrentSequence, int iNewSequence );
	virtual Activity TranslateActivity( Activity actDesired );

	virtual Activity CalcMainActivity();

	virtual int CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle ) { return 0; }
	virtual float GetCurrentMaxGroundSpeed();
	virtual bool ShouldResetGroundSpeed( Activity oldActivity, Activity idealActivity );

	// For the body left/right rotation, some models use a pose parameter and some use a bone controller.
	virtual float SetOuterBodyYaw( float flValue );

	virtual	float GetFeetYawRate( void );

protected:
	virtual void		ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
	virtual void		ComputePoseParam_BodyYaw();
	virtual void		ComputePoseParam_BodyPitch( CStudioHdr *pStudioHdr );

	int CalcFireLayerSequence(PlayerAnimEvent_t event);
	void ComputeFireSequence();

	void ComputeReloadSequence();
	int CalcReloadLayerSequence();
	int CalcReloadSucceedLayerSequence();
	int CalcReloadFailLayerSequence();

	void ComputeWeaponSwitchSequence();
	int CalcWeaponSwitchLayerSequence();

	bool HandleDucking( Activity &idealActivity );
	bool HandleJumping();

	void UpdateLayerSequenceGeneric( int iLayer, bool &bEnabled, float &flCurCycle,
									int &iSequence, bool bWaitAtEnd,
									float fBlendIn=0.15f, float fBlendOut=0.15f, bool bMoveBlend = false, 
									float fPlaybackRate=1.0f, bool bUpdateCycle = true );

protected:
	CUtlVector<CUtlSymbol> m_ActivityModifiers;

private:
	void				EstimateYaw();

private:
	CSDK_Player   *m_pSDKPlayer;

	float				m_flGaitYaw;

	// Current state variables.
	bool m_bJumping;			// Set on a jump event.
	float m_flJumpStartTime;
	bool m_bFirstJumpFrame;

	// Aim sequence plays reload while this is on.
	bool m_bReloading;
	float m_flReloadCycle;
	int m_iReloadSequence;
	float m_flReloadBlendOut, m_flReloadBlendIn;
	float m_fReloadPlaybackRate;

	bool m_bWeaponSwitching;
	float m_flWeaponSwitchCycle;
	int m_iWeaponSwitchSequence;

	// This is set to true if ANY animation is being played in the fire layer.
	bool m_bFiring;						// If this is on, then it'll continue the fire animation in the fire layer
										// until it completes.
	int m_iFireSequence;				// (For any sequences in the fire layer, including grenade throw).
	float m_flFireCycle;
	bool m_bPlayingEmoteGesture;
};

CSDKPlayerAnimState *CreateSDKPlayerAnimState( CSDK_Player *pPlayer );



#endif // SDK_PLAYERANIMSTATE_H
