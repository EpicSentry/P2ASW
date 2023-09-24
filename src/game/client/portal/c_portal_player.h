//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#ifndef PORTAL_PLAYER_H
#define PORTAL_PLAYER_H
#pragma once

#include "portal_playeranimstate.h"
#include "c_basehlplayer.h"
#include "portal_player_shared.h"
#include "c_prop_portal.h"
#include "weapon_portalbase.h"
#include "c_func_liquidportal.h"
#include "colorcorrectionmgr.h"
#include "paint/paint_power_user.h"
#include "paint/paintable_entity.h"
#include "c_portal_playerlocaldata.h"
#include "portal_shareddefs.h"
#include "portal_grabcontroller_shared.h"

struct PaintPowerChoiceCriteria_t;

enum PortalScreenSpaceEffect
{
	PAINT_SCREEN_SPACE_EFFECT,

	PORTAL_SCREEN_SPACE_EFFECT_COUNT
};

//=============================================================================
// >> Portal_Player
//=============================================================================
class C_Portal_Player : public PaintPowerUser< CPaintableEntity< C_BaseHLPlayer > >
{
public:
	DECLARE_CLASS( C_Portal_Player, PaintPowerUser< CPaintableEntity< C_BaseHLPlayer > > );

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();


	C_Portal_Player();
	~C_Portal_Player( void );

	void ClientThink( void );
	void FixTeleportationRoll( void );

	static inline C_Portal_Player* GetLocalPortalPlayer()
	{
		return (C_Portal_Player*)C_BasePlayer::GetLocalPlayer();
	}

	static inline C_Portal_Player* GetLocalPlayer()
	{
		return (C_Portal_Player*)C_BasePlayer::GetLocalPlayer();
	}

	virtual const QAngle& GetRenderAngles();

	virtual void UpdateClientSideAnimation();
	void DoAnimationEvent( PlayerAnimEvent_t event, int nData );
	virtual void FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options );

	virtual int DrawModel( int flags, const RenderableInstance_t& instance );
	virtual void AddEntity( void );

	QAngle GetAnimEyeAngles( void ) { return m_angEyeAngles; }
	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );

	// Used by prediction, sets the view angles for the player
	virtual void SetLocalViewAngles( const QAngle &viewAngles );
	virtual void SetViewAngles( const QAngle &ang );

	// Should this object cast shadows?
	virtual ShadowType_t	ShadowCastType( void );
	virtual C_BaseAnimating* BecomeRagdollOnClient();
	virtual bool			ShouldDraw( void );
	virtual const QAngle&	EyeAngles();
	virtual void			OnPreDataChanged( DataUpdateType_t type );
	virtual void			OnDataChanged( DataUpdateType_t type );
	bool					DetectAndHandlePortalTeleportation( void ); //detects if the player has portalled and fixes views
	virtual float			GetFOV( void );
	virtual CStudioHdr*		OnNewModel( void );
	virtual void			TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr );
	virtual void			ItemPreFrame( void );
	virtual void			ItemPostFrame( void );
	virtual float			GetMinFOV()	const { return 5.0f; }
	virtual Vector			GetAutoaimVector( float flDelta );
	virtual bool			ShouldReceiveProjectedTextures( int flags );
	virtual void			PostDataUpdate( DataUpdateType_t updateType );
	virtual void			GetStepSoundVelocities( float *velwalk, float *velrun );
	virtual void			PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual void			PreThink( void );
	virtual void			DoImpactEffect( trace_t &tr, int nDamageType );
	virtual bool			CreateMove( float flInputSampleTime, CUserCmd *pCmd );
	virtual bool			IsZoomed( void )	{ return m_PortalLocal.m_bZoomedIn; }

	virtual Vector			EyePosition();
	Vector					EyeFootPosition( const QAngle &qEyeAngles );//interpolates between eyes and feet based on view angle roll
	inline Vector			EyeFootPosition( void ) { return EyeFootPosition( EyeAngles() ); };
	void					PlayerPortalled( C_Prop_Portal *pEnteredPortal );

	virtual void	CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );
	void			CalcPortalView( Vector &eyeOrigin, QAngle &eyeAngles );
	virtual void	CalcViewModelView( const Vector& eyeOrigin, const QAngle& eyeAngles);
	
	bool			IsInvalidHandoff( CBaseEntity *pObject );
	void			PollForUseEntity( bool bBasicUse, CBaseEntity **ppUseEnt, CPortal_Base2D **ppUseThroughPortal );
	CBaseEntity*	FindUseEntity( C_Portal_Base2D **pThroughPortal );
	CBaseEntity*	FindUseEntityThroughPortal( void );

	inline bool		IsCloseToPortal( void ) //it's usually a good idea to turn on draw hacks when this is true
	{
		return ((PortalEyeInterpolation.m_bEyePositionIsInterpolating) || (m_hPortalEnvironment.Get() != NULL));	
	} 

	void	HandleSpeedChanges( void );
	void	UpdateLookAt( void );
	void	Initialize( void );
	int		GetIDTarget() const;
	void	UpdateIDTarget( void );

	void ToggleHeldObjectOnOppositeSideOfPortal( void ) { m_bHeldObjectOnOppositeSideOfPortal = !m_bHeldObjectOnOppositeSideOfPortal; }
	void SetHeldObjectOnOppositeSideOfPortal( bool p_bHeldObjectOnOppositeSideOfPortal ) { m_bHeldObjectOnOppositeSideOfPortal = p_bHeldObjectOnOppositeSideOfPortal; }
	bool IsHeldObjectOnOppositeSideOfPortal( void ) { return m_bHeldObjectOnOppositeSideOfPortal; }
	CProp_Portal *GetHeldObjectPortal( void ) { return m_hHeldObjectPortal; }

	void SetHeldObjectPortal( CProp_Portal *pPortal ) { m_hHeldObjectPortal = pPortal; }
	void SetUsingVMGrabState( bool bState ) { m_bUsingVMGrabState = bState; }
	bool IsUsingVMGrab( void );
	bool WantsVMGrab( void );
	bool IsForcingDrop( void ) { return m_bForcingDrop; }
	
	EHANDLE m_hGrabbedEntity;
	EHANDLE m_hPortalThroughWhichGrabOccured;
	bool m_bSilentDropAndPickup;
	void ForceDropOfCarriedPhysObjects( CBaseEntity *pOnlyIfHoldingThis );
	void PickupObject(CBaseEntity *pObject, bool bLimitMassAndSize );
	
	bool m_bForceFireNextPortal;
	
	void PreventCrouchJump( CUserCmd* ucmd );

	CGrabController &GetGrabController()
	{
		return m_GrabController;
	}

	Activity TranslateActivity( Activity baseAct, bool *pRequired = NULL );
	CWeaponPortalBase* GetActivePortalWeapon() const;
	
	bool	IsHoldingSomething( void ) const { return m_bIsHoldingSomething; }

	float GetMotionBlurAmount(void) { return m_flMotionBlurAmount; }

	bool				m_bPingDisabled;
	bool				m_bTauntDisabled;
	bool				m_bTauntRemoteView;
	Vector				m_vecRemoteViewOrigin;
	QAngle				m_vecRemoteViewAngles;
	float				m_fTauntCameraDistance;

	float				m_fTeamTauntStartTime;
	int					m_nOldTeamTauntState;
	int					m_nTeamTauntState;
	

	float	m_flUseKeyStartTime;	// for long duration uses, record the initial keypress start time
	int		m_nUseKeyEntFoundCommandNum;  // Kind of a hack... if we find a use ent, keep it around until it sends off to the server then clear
	int		m_nUseKeyEntClearCommandNum;
	int		m_nLastRecivedCommandNum;
	EHANDLE m_hUseEntToSend;		// if we find a use ent during the extended polling, keep the handle
	float	m_flAutoGrabLockOutTime;

	bool m_bForcingDrop;
	bool m_bUseVMGrab;
	bool m_bUsingVMGrabState;

	EHANDLE m_hAttachedObject;
	EHANDLE m_hOldAttachedObject;

	EHANDLE m_hPreDataChangedAttachedObject; // Ok, I just want to know if our attached object went null on this network update for some cleanup
											 // but the 'OldAttachedObject' above somehow got intertwined in some VM mode toggle logic I don't want to unravel.
											 // Adding yet another ehandle to the same entity so I can cleanly detect when the server has cleared our held 
											 // object irrespective of what held mode we are in.

	
	CGrabController m_GrabController;
	
	bool	IsTaunting( void ) { return false; } // FIXME!!
	bool	IsRemoteViewTaunt( void ) { return false; } // FIXME!!

private:
	
	void AvoidPlayers( CUserCmd *pCmd );

	C_Portal_Player( const C_Portal_Player & );

	void UpdatePortalEyeInterpolation( void );
	
	CPortalPlayerAnimState *m_PlayerAnimState;

	QAngle	m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	virtual IRagdoll		*GetRepresentativeRagdoll() const;
	EHANDLE	m_hRagdoll;

	int	m_headYawPoseParam;
	int	m_headPitchPoseParam;
	float m_headYawMin;
	float m_headYawMax;
	float m_headPitchMin;
	float m_headPitchMax;

	bool m_isInit;
	Vector m_vLookAtTarget;

	float m_flLastBodyYaw;
	float m_flCurrentHeadYaw;
	float m_flCurrentHeadPitch;
	float m_flStartLookTime;

	int	  m_iIDEntIndex;

	CountdownTimer m_blinkTimer;

	int	  m_iSpawnInterpCounter;
	int	  m_iSpawnInterpCounterCache;

	bool  m_bHeldObjectOnOppositeSideOfPortal;
	CProp_Portal *m_hHeldObjectPortal;

	int	m_iForceNoDrawInPortalSurface; //only valid for one frame, used to temp disable drawing of the player model in a surface because of freaky artifacts

	struct PortalEyeInterpolation_t
	{
		bool	m_bEyePositionIsInterpolating; //flagged when the eye position would have popped between two distinct positions and we're smoothing it over
		Vector	m_vEyePosition_Interpolated; //we'll be giving the interpolation a certain amount of instant movement per frame based on how much an uninterpolated eye would have moved
		Vector	m_vEyePosition_Uninterpolated; //can't have smooth movement without tracking where we just were
		//bool	m_bNeedToUpdateEyePosition;
		//int		m_iFrameLastUpdated;

		int		m_iTickLastUpdated;
		float	m_fTickInterpolationAmountLastUpdated;
		bool	m_bDisableFreeMovement; //used for one frame usually when error in free movement is likely to be high
		bool	m_bUpdatePosition_FreeMove;

		PortalEyeInterpolation_t( void ) : m_iTickLastUpdated(0), m_fTickInterpolationAmountLastUpdated(0.0f), m_bDisableFreeMovement(false), m_bUpdatePosition_FreeMove(false) { };
	} PortalEyeInterpolation;

	struct PreDataChanged_Backup_t
	{
		CHandle<C_Prop_Portal>	m_hPortalEnvironment;
		CHandle<C_Func_LiquidPortal>	m_hSurroundingLiquidPortal;
		//Vector					m_ptPlayerPosition;
		QAngle					m_qEyeAngles;
	} PreDataChanged_Backup;

	Vector	m_ptEyePosition_LastCalcView;
	QAngle	m_qEyeAngles_LastCalcView; //we've got some VERY persistent single frame errors while teleporting, this will be updated every frame in CalcView() and will serve as a central source for fixed angles
	C_Prop_Portal *m_pPortalEnvironment_LastCalcView;

	ClientCCHandle_t	m_CCDeathHandle;	// handle to death cc effect
	float				m_flDeathCCWeight;	// for fading in cc effect	

	bool	m_bPortalledMessagePending; //Player portalled. It's easier to wait until we get a OnDataChanged() event or a CalcView() before we do anything about it. Otherwise bits and pieces can get undone
	VMatrix m_PendingPortalMatrix;

	bool				m_bIsHoldingSomething;
		
	// we need to interpolate hull height to maintain the world space center
	float m_flHullHeight;
	CInterpolatedVar< float > m_iv_flHullHeight;

	//CDiscontinuousInterpolatedVar< Vector > m_iv_vEyeOffset;

	void ManageHeldObject();

public: // PAINT SPECIFIC
	static bool RenderLocalScreenSpaceEffect( PortalScreenSpaceEffect effect, IMatRenderContext *pRenderContext, int x, int y, int w, int h );
	
	virtual Vector Weapon_ShootPosition();
	Vector GetPaintGunShootPosition();
	
	EHANDLE GetAttachedObject ( void ) { return m_hAttachedObject; }
	
	bool IsPressingJumpKey() const;
	bool IsHoldingJumpKey() const;
	bool IsTryingToSuperJump( const PaintPowerInfo_t* pInfo = NULL ) const;
	void SetJumpedThisFrame( bool jumped );
	bool JumpedThisFrame() const;
	void SetBouncedThisFrame( bool bounced );
	bool BouncedThisFrame() const;
	InAirState GetInAirState() const;
	
	bool WantsToSwapGuns( void );
	void SetWantsToSwapGuns( bool bWantsToSwap );
	
	virtual PaintPowerType GetPaintPowerAtPoint( const Vector& worldContactPt ) const;
	virtual void Paint( PaintPowerType type, const Vector& worldContactPt );
	virtual void CleansePaint();
	
	const Vector& GetInputVector() const;
	void SetInputVector( const Vector& vInput );

	virtual bool RenderScreenSpaceEffect( PortalScreenSpaceEffect effect, IMatRenderContext *pRenderContext, int x, int y, int w, int h );

	bool ScreenSpacePaintEffectIsActive() const;
	void SetScreenSpacePaintEffectColors( IMaterialVar* pColor1, IMaterialVar* pColor2 ) const;

	void Reorient( QAngle& viewAngles );
	float GetReorientationProgress() const;
	bool IsDoneReorienting() const;
	
	virtual const Vector GetPlayerMins() const;
	virtual const Vector GetPlayerMaxs() const;
	const Vector& GetHullMins() const;
	const Vector& GetHullMaxs() const;
	const Vector& GetStandHullMins() const;
	const Vector& GetStandHullMaxs() const;
	const Vector& GetDuckHullMins() const;
	const Vector& GetDuckHullMaxs() const;

	float GetHullHeight() const;
	float GetHullWidth() const;
	float GetStandHullHeight() const;
	float GetStandHullWidth() const;
	float GetDuckHullHeight() const;
	float GetDuckHullWidth() const;
	
	virtual void UpdateCollisionBounds();

	// stick camera
	void RotateUpVector( Vector& vForward, Vector& vUp );
	void SnapCamera( StickCameraState nCameraState, bool bLookingInBadDirection );
	//void PostTeleportationCameraFixup( const CProp_Portal *pEnteredPortal );
	
	StickCameraState GetStickCameraState() const;
	void SetQuaternionPunch( const Quaternion& qPunch );
	void DecayQuaternionPunch();
	
	using BaseClass::AddSurfacePaintPowerInfo;
	void AddSurfacePaintPowerInfo( const BrushContact& contact, char const* context = 0 );
	void AddSurfacePaintPowerInfo( const trace_t& trace, char const* context = 0 );
	
	void SetEyeOffset( const Vector& vOldOrigin, const Vector& vNewOrigin );
	
	void SetHullHeight( float flHeight );
	
	void OnBounced( float fTimeOffset = 0.0f );
	
	virtual void ChooseActivePaintPowers( PaintPowerInfoVector& activePowers );

private: // PAINT SPECIFIC
	void DecayEyeOffset();
	
	// Find all the contacts
	void DeterminePaintContacts();
	void PredictPaintContacts( const Vector& contactBoxMin,
								const Vector& contactBoxMax,
								const Vector& traceBoxMin,
								const Vector& traceBoxMax,
								float lookAheadTime,
								char const* context );
	void ChooseBestPaintPowersInRange( PaintPowerChoiceResultArray& bestPowers,
										PaintPowerConstIter begin,
										PaintPowerConstIter end,
										const PaintPowerChoiceCriteria_t& info ) const;
	
	// Paint Power User Implementation
	virtual PaintPowerState ActivateSpeedPower( PaintPowerInfo_t& powerInfo );
	virtual PaintPowerState UseSpeedPower( PaintPowerInfo_t& powerInfo );
	virtual PaintPowerState DeactivateSpeedPower( PaintPowerInfo_t& powerInfo );

	virtual PaintPowerState ActivateBouncePower( PaintPowerInfo_t& powerInfo );
	virtual PaintPowerState UseBouncePower( PaintPowerInfo_t& powerInfo );
	virtual PaintPowerState DeactivateBouncePower( PaintPowerInfo_t& powerInfo );
	
	void PlayPaintSounds( const PaintPowerChoiceResultArray& touchedPowers );
	void UpdatePaintedPower();
	void UpdateAirInputScaleFadeIn();
	void UpdateInAirState();
	void CachePaintPowerChoiceResults( const PaintPowerChoiceResultArray& choiceInfo );
	bool LateSuperJumpIsValid() const;
	void RecomputeBoundsForOrientation();
	void TryToChangeCollisionBounds( const Vector& newStandHullMin,
		const Vector& newStandHullMax,
		const Vector& newDuckHullMin,
		const Vector& newDuckHullMax );
	
	float SpeedPaintAcceleration( float flDefaultMaxSpeed,
		float flSpeed,
		float flWishCos,
		float flWishDirSpeed ) const;
	
	bool RenderScreenSpacePaintEffect( IMatRenderContext *pRenderContext );
	void InvalidatePaintEffects();

	float m_flCachedJumpPowerTime;
	bool CheckToUseBouncePower( PaintPowerInfo_t& info );

	float m_flSpeedDecelerationTime;
	float m_flPredictedJumpTime;
	Vector m_vInputVector;
	CountdownTimer m_PaintScreenEffectCooldownTimer;

	float m_flUsePostTeleportationBoxTime;
	bool m_bJumpWasPressedWhenForced;	// The jump button was actually pressed when ForceDuckThisFrame() was called
	float m_flTimeSinceLastTouchedPower[3];

	// PAINT POWER STATE
	PaintPowerInfo_t m_CachedJumpPower;
	CUtlReference< CNewParticleEffect > m_PaintScreenSpaceEffect;
	
	// Paint power debug
	void DrawJumpHelperDebug( PaintPowerConstIter begin, PaintPowerConstIter end, float duration, bool noDepthTest, const PaintPowerInfo_t* pSelected ) const;

	bool m_bWantsToSwapGuns;

public:
	CPortalPlayerShared	m_Shared;

	const C_PortalPlayerLocalData& GetPortalPlayerLocalData() const;

	bool	m_bPitchReorientation;
	float	m_fReorientationRate;
	bool	m_bEyePositionIsTransformedByPortal; //when the eye and body positions are not on the same side of a portal

	CHandle<C_Prop_Portal>	m_hPortalEnvironment; //a portal whose environment the player is currently in, should be invalid most of the time
	
	void FixPortalEnvironmentOwnership( void ); //if we run prediction, there are multiple cases where m_hPortalEnvironment != CPortalSimulator::GetSimulatorThatOwnsEntity( this ), and that's bad

	CHandle<C_Func_LiquidPortal>	m_hSurroundingLiquidPortal; //a liquid portal whose volume the player is standing in

	QAngle m_vecCarriedObjectAngles;
	C_PlayerHeldObjectClone *m_pHeldEntityClone;
	C_PlayerHeldObjectClone *m_pHeldEntityThirdpersonClone;
	
	Vector m_vecCarriedObject_CurPosToTargetPos;
	QAngle m_vecCarriedObject_CurAngToTargetAng;

	//this is where we'll ease into the networked value over time and avoid applying newly networked data to previously predicted frames
	Vector m_vecCarriedObject_CurPosToTargetPos_Interpolated;
	QAngle m_vecCarriedObject_CurAngToTargetAng_Interpolated;

protected:
	CInterpolatedVar<Vector> m_iv_vecCarriedObject_CurPosToTargetPos_Interpolator;
	CInterpolatedVar<QAngle> m_iv_vecCarriedObject_CurAngToTargetAng_Interpolator;
	
	void PollForUseEntity( CUserCmd *pCmd );

	EHANDLE m_hUseEntThroughPortal;
	bool	m_bUseWasDown;

	C_PortalPlayerLocalData		m_PortalLocal;

	float m_flMotionBlurAmount;

};

inline C_Portal_Player *ToPortalPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return dynamic_cast<C_Portal_Player*>( pEntity );
}

inline C_Portal_Player *GetPortalPlayer( void )
{
	return static_cast<C_Portal_Player*>( C_BasePlayer::GetLocalPlayer() );
}

#endif //Portal_PLAYER_H
