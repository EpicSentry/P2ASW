//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Special handling for Portal usable ladders
//
//=============================================================================//
#include "cbase.h"
#include "multiplayer/basenetworkedplayer_gamemove.h"
#include "in_buttons.h"
#include "utlrbtree.h"
#include "movevars_shared.h"
#include "portal_shareddefs.h"
#include "portal_collideable_enumerator.h"
#include "prop_portal_shared.h"
#include "rumble_shared.h"

#if defined( CLIENT_DLL )
	#include "c_portal_player.h"
	#include "c_rumble.h"
	#include "c_basedoor.h"
#else
	#include "portal_player.h"
	#include "env_player_surface_trigger.h"
	#include "portal_gamestats.h"
	#include "physicsshadowclone.h"
	#include "recipientfilter.h"
	#include "SoundEmitterSystem/isoundemittersystembase.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_player_trace_through_portals("sv_player_trace_through_portals", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Causes player movement traces to trace through portals." );
ConVar sv_player_funnel_into_portals("sv_player_funnel_into_portals", "1", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_ARCHIVE_XBOX, "Causes the player to auto correct toward the center of floor portals." ); 

// Convars for paint powerups
ConVar sv_speed_normal("sv_speed_normal", "175.f", FCVAR_REPLICATED | FCVAR_CHEAT, "For tweaking the normal speed when off speed paint.");
ConVar sv_speed_paint_max("sv_speed_paint_max", "800.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "For tweaking the max speed for speed paint.");
ConVar sv_speed_paint_side_move_factor("sv_speed_paint_side_move_factor", "0.5f", FCVAR_REPLICATED | FCVAR_CHEAT);
ConVar speed_funnelling_enabled("speed_funnelling_enabled", "1", FCVAR_REPLICATED, "Toggle whether the player is funneled into portals while running on speed paint.");
ConVar sv_paintairacceleration("sv_paintairacceleration", "5.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "Air acceleration in Paint");

ConVar sv_portal_new_player_trace( "sv_portal_new_player_trace", "1", FCVAR_REPLICATED | FCVAR_CHEAT );

#if defined( CLIENT_DLL )
ConVar cl_vertical_elevator_fix( "cl_vertical_elevator_fix", "1" );
#endif

class CReservePlayerSpot;

#define PORTAL_FUNNEL_AMOUNT 6.0f
#define	MAX_CLIP_PLANES	5

#define CRITICAL_SLOPE 0.7f
#define PLAYER_FLING_HELPER_MIN_SPEED 200.0f

extern bool g_bMovementOptimizations;
extern bool g_bAllowForcePortalTrace;
extern bool g_bForcePortalTrace;

static inline CBaseEntity *TranslateGroundEntity( CBaseEntity *pGroundEntity )
{
#ifndef CLIENT_DLL
	CPhysicsShadowClone *pClone = dynamic_cast<CPhysicsShadowClone *>(pGroundEntity);

	if( pClone && pClone->IsUntransformedClone() )
	{
		CBaseEntity *pSource = pClone->GetClonedEntity();

		if( pSource )
			return pSource;
	}
#endif //#ifndef CLIENT_DLL

	return pGroundEntity;
}


//-----------------------------------------------------------------------------
// Purpose: Portal specific movement code
//-----------------------------------------------------------------------------
class CPortalGameMovement : public CGameMovement
{
	typedef CGameMovement BaseClass;
public:

	CPortalGameMovement();

	bool	m_bInPortalEnv;
// Overrides
	virtual void ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMove );
	virtual bool CheckJumpButton( void );

	void FunnelIntoPortal( CProp_Portal *pPortal, Vector &wishdir );
	
	virtual void AirMove( void );
	virtual void AirAccelerate( Vector& wishdir, float wishspeed, float accel );
	
	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	//virtual void	WalkMove();
	
	// The basic solid body movement clip that slides along multiple planes
	virtual int		TryPlayerMove( Vector *pFirstDest = NULL, trace_t *pFirstTrace = NULL );

	virtual void PlayerRoughLandingEffects( float fvol );

	virtual void CategorizePosition( void );

	// Traces the player bbox as it is swept from start to end
	virtual void TracePlayerBBox( const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm );

	// Tests the player position
	virtual CBaseHandle	TestPlayerPosition( const Vector& pos, int collisionGroup, trace_t& pm );

	virtual void Duck( void );				// Check for a forced duck

	virtual int CheckStuck( void );

	virtual void SetGroundEntity( trace_t *pm );

protected:
	
	// Handle MOVETYPE_WALK.

	//virtual void	FullWalkMove();

private:
	
#if defined( CLIENT_DLL )
	void ClientVerticalElevatorFixes( CBasePlayer *pPlayer, CMoveData *pMove );
#endif

	// Stick is done by changing gravity direction because it's easier to think about that way.
	// Gravity direction is always the negation of the player's stick normal.
	Vector m_vGravityDirection;	

	CPortal_Player	*GetPortalPlayer();
};

#if defined( CLIENT_DLL )
void CPortalGameMovement::ClientVerticalElevatorFixes( CBasePlayer *pPlayer, CMoveData *pMove )
{
	//find root move parent of our ground entity
	CBaseEntity *pRootMoveParent = pPlayer->GetGroundEntity();
	while( pRootMoveParent )
	{
		C_BaseEntity *pTestParent = pRootMoveParent->GetMoveParent();
		if( !pTestParent )
			break;

		pRootMoveParent = pTestParent;
	}

	//if it's a C_BaseToggle (func_movelinear / func_door) then enable prediction if it chooses to
	bool bRootMoveParentIsLinearMovingBaseToggle = false;
	bool bAdjustedRootZ = false;
	if( pRootMoveParent && !pRootMoveParent->IsWorld() )
	{
		C_BaseToggle *pPredictableGroundEntity = dynamic_cast<C_BaseToggle *>(pRootMoveParent);
		if( pPredictableGroundEntity && (pPredictableGroundEntity->m_movementType == MOVE_TOGGLE_LINEAR) )
		{
			bRootMoveParentIsLinearMovingBaseToggle = true;
			if( !pPredictableGroundEntity->GetPredictable() )
			{
				pPredictableGroundEntity->SetPredictionEligible( true );
				pPredictableGroundEntity->m_hPredictionOwner = pPlayer;
			}
			else if( cl_vertical_elevator_fix.GetBool() )
			{
				Vector vNewOrigin = pPredictableGroundEntity->PredictPosition( player->PredictedServerTime() + TICK_INTERVAL );
				if( (vNewOrigin - pPredictableGroundEntity->GetLocalOrigin()).LengthSqr() > 0.01f )
				{
					bAdjustedRootZ = (vNewOrigin.z != pPredictableGroundEntity->GetLocalOrigin().z);
					pPredictableGroundEntity->SetLocalOrigin( vNewOrigin );

					//invalidate abs transforms for upcoming traces
					C_BaseEntity *pParent = pPlayer->GetGroundEntity();
					while( pParent )
					{
						pParent->AddEFlags( EFL_DIRTY_ABSTRANSFORM );
						pParent = pParent->GetMoveParent();
					}
				}
			}
		}
	}

	//re-seat player on vertical elevators
	if( bRootMoveParentIsLinearMovingBaseToggle && 
		cl_vertical_elevator_fix.GetBool() && 
		bAdjustedRootZ )
	{
		trace_t trElevator;
		TracePlayerBBox( pMove->GetAbsOrigin(), pMove->GetAbsOrigin() - Vector( 0.0f, 0.0f, GetPlayerMaxs().z ), MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trElevator );

		if( trElevator.startsolid )
		{
			//started in solid, and we think it's an elevator. Pop up the player if at all possible

			//trace up, ignoring the ground entity hierarchy
			Ray_t playerRay;
			playerRay.Init( pMove->GetAbsOrigin(), pMove->GetAbsOrigin() + Vector( 0.0f, 0.0f, GetPlayerMaxs().z ), GetPlayerMins(), GetPlayerMaxs() );

			CTraceFilterSimpleList ignoreGroundEntityHeirarchy( COLLISION_GROUP_PLAYER_MOVEMENT );
			{
				ignoreGroundEntityHeirarchy.AddEntityToIgnore( pPlayer );
				C_BaseEntity *pParent = pPlayer->GetGroundEntity();
				while( pParent )
				{
					ignoreGroundEntityHeirarchy.AddEntityToIgnore( pParent );
					pParent = pParent->GetMoveParent();
				}
			}

			

			enginetrace->TraceRay( playerRay, MASK_PLAYERSOLID, &ignoreGroundEntityHeirarchy, &trElevator );
			if( !trElevator.startsolid ) //success
			{
				//now trace back down
				Vector vStart = trElevator.endpos;
				TracePlayerBBox( vStart, pMove->GetAbsOrigin(), MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trElevator );
				if( !trElevator.startsolid &&
					(trElevator.m_pEnt == pPlayer->GetGroundEntity()) )
				{
					//if we landed back on the ground entity, call it good
					pMove->SetAbsOrigin( trElevator.endpos );
					pPlayer->SetNetworkOrigin( trElevator.endpos ); //paint code loads from network origin after handling paint powers
				}
			}
		}
		else if( (trElevator.endpos.z < pMove->GetAbsOrigin().z) && (trElevator.m_pEnt == pPlayer->GetGroundEntity()) )
		{
			//re-seat on ground entity
			pMove->SetAbsOrigin( trElevator.endpos );
			pPlayer->SetNetworkOrigin( trElevator.endpos ); //paint code loads from network origin after handling paint powers
		}
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPortalGameMovement::CPortalGameMovement()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CPortal_Player	*CPortalGameMovement::GetPortalPlayer()
{
	return static_cast< CPortal_Player * >( player );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pMove - 
//-----------------------------------------------------------------------------
void CPortalGameMovement::ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMove )
{
	Assert( pMove && pPlayer );

	float flStoreFrametime = gpGlobals->frametime;

//	ResetGetPointContentsCache();

	// Cropping movement speed scales mv->m_fForwardSpeed etc. globally
	// Once we crop, we don't want to recursively crop again, so we set the crop
	//  flag globally here once per usercmd cycle.
	m_bSpeedCropped = false;

	player = pPlayer;
	mv = pMove;
	mv->m_flMaxSpeed = sv_maxspeed.GetFloat();
	
	// Get the paint player
	CPortal_Player *pPortalPlayer = GetPortalPlayer();

	//!!HACK HACK: Adrian - slow down all player movement by this factor.
	//!!Blame Yahn for this one.
	gpGlobals->frametime *= pPlayer->GetLaggedMovementValue();
	
#if defined( CLIENT_DLL )
	ClientVerticalElevatorFixes( pPlayer, pMove ); //fixup vertical elevator discrepancies between client and server as best we can
#endif
	
	// Figure out move direction
	const Vector& stickNormal = pPortalPlayer->GetPortalPlayerLocalData().m_StickNormal;
	Vector vForward, vRight;
	Vector vPlayerUp = pPortalPlayer->GetPortalPlayerLocalData().m_Up;
	AngleVectors( mv->m_vecViewAngles, &vForward, &vRight, NULL );  // Determine movement angles

	const Vector worldUp( 0, 0, 1 );
	bool shouldProjectInputVectorsOntoGround = pPortalPlayer->GetGroundEntity() != NULL;

	if( shouldProjectInputVectorsOntoGround )
	{
		vForward -= DotProduct( vForward, stickNormal ) * stickNormal;
		vRight -= DotProduct( vRight, stickNormal ) * stickNormal;

		vForward.NormalizeInPlace();
		vRight.NormalizeInPlace();
	}

	Vector vWishVel = vForward*mv->m_flForwardMove + vRight*mv->m_flSideMove;
	vWishVel -= vPlayerUp * DotProduct( vWishVel, vPlayerUp );
	
	// Figure out paint power
	pPortalPlayer->SetInputVector( vWishVel );
	pPortalPlayer->UpdatePaintPowers();

	m_bInPortalEnv = (((CPortal_Player *)pPlayer)->m_hPortalEnvironment != NULL);

	g_bAllowForcePortalTrace = m_bInPortalEnv;
	g_bForcePortalTrace = m_bInPortalEnv;

	// Run the command.
	PlayerMove();

	FinishMove();

	g_bAllowForcePortalTrace = false;
	g_bForcePortalTrace = false;

#ifndef CLIENT_DLL
	pPlayer->UnforceButtons( IN_DUCK );
	pPlayer->UnforceButtons( IN_JUMP );
#endif

	//This is probably not needed, but just in case.
	gpGlobals->frametime = flStoreFrametime;
}

//-----------------------------------------------------------------------------
// Purpose: Base jump behavior, plus an anim event
// Input  :  - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPortalGameMovement::CheckJumpButton()
{
	if ( BaseClass::CheckJumpButton() && GetPortalPlayer() )
	{
		GetPortalPlayer()->DoAnimationEvent( PLAYERANIMEVENT_JUMP, 0 );
		return true;
	}

	return false;
}

void CPortalGameMovement::FunnelIntoPortal( CProp_Portal *pPortal, Vector &wishdir )
{
	// Make sure there's a portal
	if ( !pPortal )
		return;

	// Get portal vectors
	Vector vPortalForward, vPortalRight, vPortalUp;
	pPortal->GetVectors( &vPortalForward, &vPortalRight, &vPortalUp );

	// Make sure it's a floor portal
	if ( vPortalForward.z < 0.8f )
		return;

	vPortalRight.z = 0.0f;
	vPortalUp.z = 0.0f;
	VectorNormalize( vPortalRight );
	VectorNormalize( vPortalUp );

	// Make sure the player is looking downward
	CPortal_Player *pPlayer = GetPortalPlayer();

	Vector vPlayerForward;
	pPlayer->EyeVectors( &vPlayerForward );

	if ( vPlayerForward.z > -0.1f )
		return;

	Vector vPlayerOrigin = pPlayer->GetAbsOrigin();
	Vector vPlayerToPortal = pPortal->GetAbsOrigin() - vPlayerOrigin;

	// Make sure the player is trying to air control, they're falling downward and they are vertically close to the portal
	if ( fabsf( wishdir[ 0 ] ) > 64.0f || fabsf( wishdir[ 1 ] ) > 64.0f || mv->m_vecVelocity[ 2 ] > -165.0f || vPlayerToPortal.z < -512.0f )
		return;

	// Make sure we're in the 2D portal rectangle
	if ( ( vPlayerToPortal.Dot( vPortalRight ) * vPortalRight ).Length() > PORTAL_HALF_WIDTH * 1.5f )
		return;
	if ( ( vPlayerToPortal.Dot( vPortalUp ) * vPortalUp ).Length() > PORTAL_HALF_HEIGHT * 1.5f )
		return;

	if ( vPlayerToPortal.z > -8.0f )
	{
		// We're too close the the portal to continue correcting, but zero the velocity so our fling velocity is nice
		mv->m_vecVelocity[ 0 ] = 0.0f;
		mv->m_vecVelocity[ 1 ] = 0.0f;
	}
	else
	{
		// Funnel toward the portal
		float fFunnelX = vPlayerToPortal.x * PORTAL_FUNNEL_AMOUNT - mv->m_vecVelocity[ 0 ];
		float fFunnelY = vPlayerToPortal.y * PORTAL_FUNNEL_AMOUNT - mv->m_vecVelocity[ 1 ];

		wishdir[ 0 ] += fFunnelX;
		wishdir[ 1 ] += fFunnelY;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : wishdir - 
//			accel - 
//-----------------------------------------------------------------------------
void CPortalGameMovement::AirAccelerate( Vector& wishdir, float wishspeed, float accel )
{
	int i;
	float addspeed, accelspeed, currentspeed;
	float wishspd;

	wishspd = wishspeed;

	if (player->pl.deadflag)
		return;

	if (player->m_flWaterJumpTime)
		return;

	// Cap speed
	if (wishspd > 60.0f)
		wishspd = 60.0f;

	// Determine veer amount
	currentspeed = mv->m_vecVelocity.Dot(wishdir);

	// See how much to add
	addspeed = wishspd - currentspeed;

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	accelspeed = accel * wishspeed * gpGlobals->frametime * player->m_surfaceFriction;

	// Cap it
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust pmove vel.
	for (i=0 ; i<3 ; i++)
	{
		mv->m_vecVelocity[i] += accelspeed * wishdir[i];
		mv->m_outWishVel[i] += accelspeed * wishdir[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPortalGameMovement::AirMove( void )
{
	int			i;
	Vector		wishvel;
	float		fmove, smove;
	Vector		wishdir;
	float		wishspeed;
	Vector forward, right, up;

	AngleVectors (mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

	// Copy movement amounts
	fmove = mv->m_flForwardMove;
	smove = mv->m_flSideMove;

	// Zero out z components of movement vectors
	forward[2] = 0;
	right[2]   = 0;
	VectorNormalize(forward);  // Normalize remainder of vectors
	VectorNormalize(right);    // 

	for (i=0 ; i<2 ; i++)       // Determine x and y parts of velocity
		wishvel[i] = forward[i]*fmove + right[i]*smove;
	wishvel[2] = 0;             // Zero out z part of velocity

	VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move

	//
	// Don't let the player screw their fling because of adjusting into a floor portal
	//
	if ( mv->m_vecVelocity[ 0 ] * mv->m_vecVelocity[ 0 ] + mv->m_vecVelocity[ 1 ] * mv->m_vecVelocity[ 1 ] > MIN_FLING_SPEED * MIN_FLING_SPEED )
	{
		if ( mv->m_vecVelocity[ 0 ] > MIN_FLING_SPEED * 0.5f && wishdir[ 0 ] < 0.0f )
			wishdir[ 0 ] = 0.0f;
		else if ( mv->m_vecVelocity[ 0 ] < -MIN_FLING_SPEED * 0.5f && wishdir[ 0 ] > 0.0f )
			wishdir[ 0 ] = 0.0f;

		if ( mv->m_vecVelocity[ 1 ] > MIN_FLING_SPEED * 0.5f && wishdir[ 1 ] < 0.0f )
			wishdir[ 1 ] = 0.0f;
		else if ( mv->m_vecVelocity[ 1 ] < -MIN_FLING_SPEED * 0.5f && wishdir[ 1 ] > 0.0f )
			wishdir[ 1 ] = 0.0f;
	}

	//
	// Try to autocorrect the player to fall into the middle of the portal
	//
	else if ( sv_player_funnel_into_portals.GetBool() )
	{
		int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
		if( iPortalCount != 0 )
		{
			CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
			for( int i = 0; i != iPortalCount; ++i )
			{
				CProp_Portal *pTempPortal = pPortals[i];
				if( pTempPortal->IsActivedAndLinked() )
				{
					FunnelIntoPortal( pTempPortal, wishdir );
				}
			}
		}
	}

	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if ( wishspeed != 0 && (wishspeed > mv->m_flMaxSpeed))
	{
		VectorScale (wishvel, mv->m_flMaxSpeed/wishspeed, wishvel);
		wishspeed = mv->m_flMaxSpeed;
	}

	AirAccelerate( wishdir, wishspeed, 15.0f );

	// Add in any base velocity to the current velocity.
	VectorAdd(mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

	TryPlayerMove();

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
}

void CPortalGameMovement::PlayerRoughLandingEffects( float fvol )
{
	BaseClass::PlayerRoughLandingEffects( fvol );

#ifndef CLIENT_DLL
	if ( fvol >= 1.0 )
	{
		// Play the future shoes sound
		CRecipientFilter filter;
		filter.AddRecipientsByPAS( player->GetAbsOrigin() );

		CSoundParameters params;
		if ( CBaseEntity::GetParametersForSound( "PortalPlayer.FallRecover", params, NULL ) )
		{
			EmitSound_t ep( params );
			ep.m_nPitch = 125.0f - player->m_Local.m_flFallVelocity * 0.03f;					// lower pitch the harder they land
			ep.m_flVolume = MIN( player->m_Local.m_flFallVelocity * 0.00075f - 0.38, 1.0f );	// louder the harder they land

			CBaseEntity::EmitSound( filter, player->entindex(), ep );
		}
	}
#endif
}

void TracePlayerBBoxForGround2( const Vector& start, const Vector& end, const Vector& minsSrc,
							   const Vector& maxsSrc, IHandleEntity *player, unsigned int fMask,
							   int collisionGroup, trace_t& pm )
{

	VPROF( "TracePlayerBBoxForGround" );

	CPortal_Player *pPortalPlayer = dynamic_cast<CPortal_Player *>(player->GetRefEHandle().Get());
	CProp_Portal *pPlayerPortal = pPortalPlayer->m_hPortalEnvironment;

#ifndef CLIENT_DLL
	if( pPlayerPortal && pPlayerPortal->m_PortalSimulator.IsReadyToSimulate() == false )
		pPlayerPortal = NULL;
#endif

	Ray_t ray;
	Vector mins, maxs;

	float fraction = pm.fraction;
	Vector endpos = pm.endpos;

	// Check the -x, -y quadrant
	mins = minsSrc;
	maxs.Init( MIN( 0, maxsSrc.x ), MIN( 0, maxsSrc.y ), maxsSrc.z );
	ray.Init( start, end, mins, maxs );

	if( pPlayerPortal )
		UTIL_Portal_TraceRay( pPlayerPortal, ray, fMask, player, collisionGroup, &pm );
	else
		UTIL_TraceRay( ray, fMask, player, collisionGroup, &pm );

	if ( pm.m_pEnt && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the +x, +y quadrant
	mins.Init( MAX( 0, minsSrc.x ), MAX( 0, minsSrc.y ), minsSrc.z );
	maxs = maxsSrc;
	ray.Init( start, end, mins, maxs );

	if( pPlayerPortal )
		UTIL_Portal_TraceRay( pPlayerPortal, ray, fMask, player, collisionGroup, &pm );
	else
		UTIL_TraceRay( ray, fMask, player, collisionGroup, &pm );

	if ( pm.m_pEnt && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the -x, +y quadrant
	mins.Init( minsSrc.x, MAX( 0, minsSrc.y ), minsSrc.z );
	maxs.Init( MIN( 0, maxsSrc.x ), maxsSrc.y, maxsSrc.z );
	ray.Init( start, end, mins, maxs );

	if( pPlayerPortal )
		UTIL_Portal_TraceRay( pPlayerPortal, ray, fMask, player, collisionGroup, &pm );
	else
		UTIL_TraceRay( ray, fMask, player, collisionGroup, &pm );

	if ( pm.m_pEnt && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the +x, -y quadrant
	mins.Init( MAX( 0, minsSrc.x ), minsSrc.y, minsSrc.z );
	maxs.Init( maxsSrc.x, MIN( 0, maxsSrc.y ), maxsSrc.z );
	ray.Init( start, end, mins, maxs );

	if( pPlayerPortal )
		UTIL_Portal_TraceRay( pPlayerPortal, ray, fMask, player, collisionGroup, &pm );
	else
		UTIL_TraceRay( ray, fMask, player, collisionGroup, &pm );

	if ( pm.m_pEnt && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	pm.fraction = fraction;
	pm.endpos = endpos;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &input - 
//-----------------------------------------------------------------------------
void CPortalGameMovement::CategorizePosition( void )
{
	Vector point;
	trace_t pm;

	// if the player hull point one unit down is solid, the player
	// is on ground

	// see if standing on something solid	

	// Doing this before we move may introduce a potential latency in water detection, but
	// doing it after can get us stuck on the bottom in water if the amount we move up
	// is less than the 1 pixel 'threshold' we're about to snap to.	Also, we'll call
	// this several times per frame, so we really need to avoid sticking to the bottom of
	// water on each call, and the converse case will correct itself if called twice.
	CheckWater();

	// observers don't have a ground entity
	if ( player->IsObserver() )
		return;

	point[0] = mv->GetAbsOrigin()[0];
	point[1] = mv->GetAbsOrigin()[1];
	point[2] = mv->GetAbsOrigin()[2] - 2;

	Vector bumpOrigin;
	bumpOrigin = mv->GetAbsOrigin();

	// Shooting up really fast.  Definitely not on ground.
	// On ladder moving up, so not on ground either
	// NOTE: 145 is a jump.
	if ( mv->m_vecVelocity[2] > 140 || 
		( mv->m_vecVelocity[2] > 0.0f && player->GetMoveType() == MOVETYPE_LADDER ) )   
	{
		SetGroundEntity( NULL );
	}
	else
	{
		// Try and move down.
		TracePlayerBBox( bumpOrigin, point, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm );

		// If we hit a steep plane, we are not on ground
		if ( pm.plane.normal[2] < 0.7)
		{
			// Test four sub-boxes, to see if any of them would have found shallower slope we could
			// actually stand on

			TracePlayerBBoxForGround2( bumpOrigin, point, GetPlayerMins(), GetPlayerMaxs(), mv->m_nPlayerHandle.Get(), MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm );
			if ( pm.plane.normal[2] < 0.7)
			{

				SetGroundEntity( NULL );	// too steep
				// probably want to add a check for a +z velocity too!
				if ( ( mv->m_vecVelocity.z > 0.0f ) && ( player->GetMoveType() != MOVETYPE_NOCLIP ) )
				{
					player->m_surfaceFriction = 0.25f;
				}
			}
			else
			{
				SetGroundEntity( &pm );  // Otherwise, point to index of ent under us.
			}
		}
		else
		{
			SetGroundEntity( &pm );  // Otherwise, point to index of ent under us.
		}

		// If we are on something...
		if (player->GetGroundEntity() != NULL)
		{
			// Then we are not in water jump sequence
			player->m_flWaterJumpTime = 0;

			// If we could make the move, drop us down that 1 pixel
			if ( player->GetWaterLevel() < WL_Waist && !pm.startsolid && !pm.allsolid )
			{
				// check distance we would like to move -- this is supposed to just keep up
				// "on the ground" surface not stap us back to earth (i.e. on move origin to
				// end position when the ground is within .5 units away) (2 units)
				if( pm.fraction )
					//				if( pm.fraction < 0.5)
				{
					mv->SetAbsOrigin( pm.endpos );
				}
			}
		}

#ifndef CLIENT_DLL

		//Adrian: vehicle code handles for us.
		if ( player->IsInAVehicle() == false )
		{
			// If our gamematerial has changed, tell any player surface triggers that are watching
			IPhysicsSurfaceProps *physprops = MoveHelper()->GetSurfaceProps();
			surfacedata_t *pSurfaceProp = physprops->GetSurfaceData( pm.surface.surfaceProps );
			char cCurrGameMaterial = pSurfaceProp->game.material;
			if ( !player->GetGroundEntity() )
			{
				cCurrGameMaterial = 0;
			}

			// Changed?
			if ( player->m_chPreviousTextureType != cCurrGameMaterial )
			{
				CEnvPlayerSurfaceTrigger::SetPlayerSurface( player, cCurrGameMaterial );
			}

			player->m_chPreviousTextureType = cCurrGameMaterial;
		}
#endif
	}
}

void CPortalGameMovement::Duck( void )
{
	return BaseClass::Duck();
}

int CPortalGameMovement::CheckStuck( void )
{
	if( BaseClass::CheckStuck() )
	{
		CPortal_Player *pPortalPlayer = GetPortalPlayer();

#ifndef CLIENT_DLL
		if( pPortalPlayer->IsAlive() )
			g_PortalGameStats.Event_PlayerStuck( pPortalPlayer );
#endif

		//try to fix it, then recheck
		Vector vIndecisive;
		if( pPortalPlayer->m_hPortalEnvironment )
		{
			pPortalPlayer->m_hPortalEnvironment->GetVectors( &vIndecisive, NULL, NULL );
		}
		else
		{
			vIndecisive.Init( 0.0f, 0.0f, 1.0f );
		}
		Vector ptOldOrigin = pPortalPlayer->GetAbsOrigin();

		if( pPortalPlayer->m_hPortalEnvironment )
		{
			if( !FindClosestPassableSpace( pPortalPlayer, vIndecisive ) )
			{
#ifndef CLIENT_DLL
				DevMsg( "Hurting the player for FindClosestPassableSpaceFailure!" );

				CTakeDamageInfo info( pPortalPlayer, pPortalPlayer, vec3_origin, vec3_origin, 1e10, DMG_CRUSH );
				pPortalPlayer->OnTakeDamage( info );
#endif
			}

			//make sure we didn't get put behind the portal >_<
			Vector ptCurrentOrigin = pPortalPlayer->GetAbsOrigin();
			if( vIndecisive.Dot( ptCurrentOrigin - ptOldOrigin ) < 0.0f )
			{
				pPortalPlayer->SetAbsOrigin( ptOldOrigin + (vIndecisive * 5.0f) ); //this is an anti-bug hack, since this would have probably popped them out of the world, we're just going to move them forward a few units
			}
		}

		mv->SetAbsOrigin( pPortalPlayer->GetAbsOrigin() );
		return BaseClass::CheckStuck();
	}
	else
	{
		return 0;
	}
}

void CPortalGameMovement::SetGroundEntity( trace_t *pm )
{
#ifndef CLIENT_DLL
	if ( !player->GetGroundEntity() && pm && pm->m_pEnt )
	{
		IGameEvent *event = gameeventmanager->CreateEvent( "portal_player_touchedground" );
		if ( event )
		{
			event->SetInt( "userid", player->GetUserID() );
			gameeventmanager->FireEvent( event );
		}
	}
#endif

	BaseClass::SetGroundEntity( pm );
}

void CPortalGameMovement::TracePlayerBBox( const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm )
{
	VPROF( "CGameMovement::TracePlayerBBox" );
	
	CPortal_Player *pPortalPlayer = (CPortal_Player *)((CBaseEntity *)mv->m_nPlayerHandle.Get());

	Ray_t ray;
	ray.Init( start, end, GetPlayerMins(), GetPlayerMaxs() );

#ifdef CLIENT_DLL
	CTraceFilterSimple traceFilter( mv->m_nPlayerHandle.Get(), collisionGroup );
#else
	CTraceFilterSimple baseFilter( mv->m_nPlayerHandle.Get(), collisionGroup );
	CTraceFilterTranslateClones traceFilter( &baseFilter );
#endif

	UTIL_Portal_TraceRay_With( pPortalPlayer->m_hPortalEnvironment, ray, fMask, &traceFilter, &pm );

	// If we're moving through a portal and failed to hit anything with the above ray trace
	// Use UTIL_Portal_TraceEntity to test this movement through a portal and override the trace with the result
	if ( pm.fraction == 1.0f && UTIL_DidTraceTouchPortals( ray, pm ) && sv_player_trace_through_portals.GetBool() )
	{
		trace_t tempTrace;
		UTIL_Portal_TraceEntity( pPortalPlayer, start, end, fMask, &traceFilter, &tempTrace );

		if ( tempTrace.DidHit() && tempTrace.fraction < pm.fraction && !tempTrace.startsolid && !tempTrace.allsolid )
		{
			pm = tempTrace;
		}
	}
}

CBaseHandle CPortalGameMovement::TestPlayerPosition( const Vector& pos, int collisionGroup, trace_t& pm )
{
	TracePlayerBBox( pos, pos, MASK_PLAYERSOLID, collisionGroup, pm ); //hook into the existing portal special trace functionality

	//Ray_t ray;
	//ray.Init( pos, pos, GetPlayerMins(), GetPlayerMaxs() );
	//UTIL_TraceRay( ray, MASK_PLAYERSOLID, mv->m_nPlayerHandle.Get(), collisionGroup, &pm );
	if( pm.startsolid && pm.m_pEnt && (pm.contents & MASK_PLAYERSOLID) )
	{
#ifdef _DEBUG
		AssertMsgOnce( false, "The player got stuck on something. Break to investigate." ); //happens enough to just leave in a perma-debugger
		//this next trace is PURELY for tracking down how the player got stuck. Nothing new is discovered over the same trace about 10 lines up
        TracePlayerBBox( pos, pos, MASK_PLAYERSOLID, collisionGroup, pm );		
#endif
		return pm.m_pEnt->GetRefEHandle();
	}
#ifndef CLIENT_DLL
	else if ( pm.startsolid && pm.m_pEnt && CPSCollisionEntity::IsPortalSimulatorCollisionEntity( pm.m_pEnt ) )
	{
		// Stuck in a portal environment object, so unstick them!
		CPortal_Player *pPortalPlayer = (CPortal_Player *)((CBaseEntity *)mv->m_nPlayerHandle.Get());
		pPortalPlayer->SetStuckOnPortalCollisionObject();

		return INVALID_EHANDLE_INDEX;
	}
#endif
	else
	{	
		return INVALID_EHANDLE_INDEX;
	}
}


// Expose our interface.
static CPortalGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );

int CPortalGameMovement::TryPlayerMove( Vector *pFirstDest, trace_t *pFirstTrace )
{
	int			bumpcount, numbumps;
	Vector		dir;
	float		d;
	int			numplanes;
	Vector		planes[MAX_CLIP_PLANES];
	Vector		primal_velocity, original_velocity;
	Vector      new_velocity;
	int			i, j;
	trace_t	pm;
	Vector		end;
	float		time_left, allFraction;
	int			blocked;		

	numbumps  = 4;           // Bump up to four times

	blocked   = 0;           // Assume not blocked
	numplanes = 0;           //  and not sliding along any planes

	VectorCopy (mv->m_vecVelocity, original_velocity);  // Store original velocity
	VectorCopy (mv->m_vecVelocity, primal_velocity);

	allFraction = 0;
	time_left = gpGlobals->frametime;   // Total time for this movement operation.

	new_velocity.Init();

	for (bumpcount=0 ; bumpcount < numbumps; bumpcount++)
	{
		if ( mv->m_vecVelocity.Length() == 0.0 )
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA( mv->GetAbsOrigin(), time_left, mv->m_vecVelocity, end );

		// See if we can make it from origin to end point.
		if ( g_bMovementOptimizations )
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if ( pFirstDest && ( end == *pFirstDest ) )
			{
				pm = *pFirstTrace;
			}
			else
			{
#if defined( PLAYER_GETTING_STUCK_TESTING )
				trace_t foo;
				TracePlayerBBox( mv->GetAbsOrigin(), mv->GetAbsOrigin(), PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, foo );
				if ( foo.startsolid || foo.fraction != 1.0f )
				{
					Msg( "bah\n" );
				}
#endif
				TracePlayerBBox( mv->GetAbsOrigin(), end, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm );
			}
		}
		else
		{
			TracePlayerBBox( mv->GetAbsOrigin(), end, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm );
		}

		allFraction += pm.fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm.allsolid)
		{	
			// entity is trapped in another solid
			VectorCopy (vec3_origin, mv->m_vecVelocity);
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if( pm.fraction > 0 )
		{	

			// NOTE: Disabled this in portal as we don't have displacement collisions and all traces are amplified greatly making each TracePlayerBBox() really expensive
#if defined(RECHECK_TERRAIN_COLLISION_BUG)
			if ( numbumps > 0 && pm.fraction == 1 )
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox( pm.endpos, pm.endpos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, stuck );
				if ( stuck.startsolid || stuck.fraction != 1.0f )
				{
					//Msg( "Player will become stuck!!!\n" );
					VectorCopy (vec3_origin, mv->m_vecVelocity);
					break;
				}
			}
#endif

#if defined( PLAYER_GETTING_STUCK_TESTING )
			trace_t foo;
			TracePlayerBBox( pm.endpos, pm.endpos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, foo );
			if ( foo.startsolid || foo.fraction != 1.0f )
			{
				Msg( "Player will become stuck!!!\n" );
			}
#endif
			// actually covered some distance
			mv->SetAbsOrigin( pm.endpos);
			VectorCopy (mv->m_vecVelocity, original_velocity);
			numplanes = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm.fraction == 1)
		{
			break;		// moved the entire distance
		}

		// Save entity that blocked us (since fraction was < 1.0)
		//  for contact
		// Add it if it's not already in the list!!!
		MoveHelper( )->AddToTouched( pm, mv->m_vecVelocity );


		if ( player->GetAbsVelocity().AsVector2D().Length() > 275.0f )
		{
			if ( pm.m_pEnt && pm.m_pEnt->GetMoveType() == MOVETYPE_VPHYSICS )
			{
				// Don't let the player auto grab something that they just ran in to
				// They end up holding it, but with less momentum which is confusing.
				CPortal_Player *pPortalPlayer = static_cast< CPortal_Player* >( player );
				pPortalPlayer->m_flAutoGrabLockOutTime = gpGlobals->curtime;

#ifdef CLIENT_DLL
				STEAMWORKS_TESTSECRET();

				if ( pm.m_pEnt == pPortalPlayer->m_hUseEntToSend.Get() )
				{
					pPortalPlayer->m_hUseEntToSend = NULL;
				}
#endif
			}
		}

		// If the plane we hit has a normal whose dot product with the gravity direction is high
		// then this is probably a floor relative to our orientation
		if ( DotProduct( pm.plane.normal, m_vGravityDirection ) > CRITICAL_SLOPE )
		{
			blocked |= 1;		// floor
		}
		// If the plane's normal dotted with the gravity direction is 0 then it's a wall relative to our orientation
		if ( DotProduct( pm.plane.normal, m_vGravityDirection ) == 0.f )
		{
			blocked |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left -= time_left * pm.fraction;

		// Did we run out of planes to clip against?
		if (numplanes >= MAX_CLIP_PLANES)
		{	
			// this shouldn't really happen
			//  Stop our movement if so.
			VectorCopy (vec3_origin, mv->m_vecVelocity);
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		VectorCopy (pm.plane.normal, planes[numplanes]);
		numplanes++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if ( numplanes == 1 &&
			player->GetMoveType() == MOVETYPE_WALK &&
			player->GetGroundEntity() == NULL )	
		{
			for ( i = 0; i < numplanes; i++ )
			{
				if ( DotProduct( planes[i], m_vGravityDirection ) > CRITICAL_SLOPE  )
				{
					// floor or slope
					ClipVelocity( original_velocity, planes[i], new_velocity, 1 );
					VectorCopy( new_velocity, original_velocity );
				}
				else
				{
					ClipVelocity( original_velocity, planes[i], new_velocity, 1.0 + sv_bounce.GetFloat() * (1 - player->m_surfaceFriction) );
				}
			}

			VectorCopy( new_velocity, mv->m_vecVelocity );
			VectorCopy( new_velocity, original_velocity );
		}
		else
		{
			for (i=0 ; i < numplanes ; i++)
			{
				ClipVelocity( original_velocity, planes[i], mv->m_vecVelocity, 1 );

				for (j=0 ; j<numplanes ; j++)
				{
					if (j != i)
					{
						// Are we now moving against this plane?
						if (mv->m_vecVelocity.Dot(planes[j]) < 0)
							break;	// not ok
					}
				}

				if (j == numplanes)  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i != numplanes)
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;  
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					VectorCopy (vec3_origin, mv->m_vecVelocity);
					break;
				}
				CrossProduct (planes[0], planes[1], dir);
				dir.NormalizeInPlace();
				d = dir.Dot(mv->m_vecVelocity);
				VectorScale (dir, d, mv->m_vecVelocity );
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d = mv->m_vecVelocity.Dot(primal_velocity);
			if (d <= 0)
			{
				//Con_DPrintf("Back\n");
				VectorCopy (vec3_origin, mv->m_vecVelocity);
				break;
			}
		}
	}

	if ( allFraction == 0 )
	{
		VectorCopy (vec3_origin, mv->m_vecVelocity);
	}

	return blocked;
}

// We'll fix this later
#if 0
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPortalGameMovement::WalkMove()
{
	Vector wishvel;
	float spd;
	float fmove, smove;
	Vector wishdir;
	float wishspeed;

	Vector dest;
	trace_t pm;
	Vector forward, right, up;

	AngleVectors (mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

	CBaseEntity *pOldGround = player->GetGroundEntity();

	// Copy movement amounts
	fmove = mv->m_flForwardMove;
	smove = mv->m_flSideMove;

	// Keep movement vectors in our plane of movement
	forward -= m_vGravityDirection * DotProduct( forward, m_vGravityDirection );
	VectorNormalize( forward );

	right -= m_vGravityDirection * DotProduct( right, m_vGravityDirection );
	VectorNormalize( right );

	// Determine velocity
	wishvel = fmove * forward + smove * right;

	// Don't let them stand on the edge of vertical bridges
	bool shouldShovePlayer = false;
	float wishVelShoveDampenFactor = 0;
	Vector shoveVector = vec3_origin;
	CProjectedWallEntity *pProjectedWall = dynamic_cast< CProjectedWallEntity* >( pOldGround );
	if ( pProjectedWall )
	{
		Vector vBridgeUp = pProjectedWall->Up();
		if ( vBridgeUp.z > -0.4f && vBridgeUp.z < 0.4f )
		{
			wishVelShoveDampenFactor = 0.25f;	// Weaken their manual control
			shoveVector = vBridgeUp * 150.f;	// Shove!
			shouldShovePlayer = true;
		}
	}

	// Don't let players stand on top of each other
	if( pOldGround && pOldGround->IsPlayer() )
	{
		wishVelShoveDampenFactor = 0.25f;
		shoveVector = player->GetAbsOrigin() - pOldGround->GetAbsOrigin();
		shoveVector.z = 0.0f;
		if( shoveVector.IsZeroFast() )
			shoveVector = player->Forward();

		shoveVector.NormalizeInPlace();
		shoveVector *= 150.0f;
		shouldShovePlayer = true;
	}

	if( shouldShovePlayer )
	{
		wishvel *= wishVelShoveDampenFactor;	// Weaken their manual control
		wishvel += shoveVector;					// Shove!
	}

	// Restrict wishvel to plane of movement
	wishvel -= m_vGravityDirection * DotProduct( wishvel, m_vGravityDirection );

	// Try funnelling
	if( sv_player_funnel_into_portals.GetBool() && speed_funnelling_enabled.GetBool() )
	{
		wishvel += PortalFunnel( wishvel );
	}

	VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move
	wishspeed = VectorNormalize(wishdir);

	//
	// Clamp to server defined max speed
	//
	if ((wishspeed != 0.0f) && (wishspeed > mv->m_flMaxSpeed))
	{
		VectorScale (wishvel, mv->m_flMaxSpeed/wishspeed, wishvel);
		wishspeed = mv->m_flMaxSpeed;
	}

	// Set pmove velocity
	Accelerate ( wishdir, wishspeed, sv_accelerate.GetFloat() );

	// Add in any base velocity to the current velocity.
	VectorAdd (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

	spd = VectorLength( mv->m_vecVelocity );

	if ( spd < 1.0f )
	{
		mv->m_vecVelocity.Init();
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
		return;
	}

	// first try just moving to the destination	
	dest = mv->GetAbsOrigin() + ( mv->m_vecVelocity * gpGlobals->frametime );

	// first try moving directly to the next spot
	TracePlayerBBox( mv->GetAbsOrigin(), dest, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm );

	// If we made it all the way, then copy trace end as new player position.
	mv->m_outWishVel += wishdir * wishspeed;

	if ( pm.fraction == 1 )
	{
		mv->SetAbsOrigin( pm.endpos );
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

		StayOnGround();
		return;
	}

	// Don't walk up stairs if not on ground.
	if ( pOldGround == NULL && player->GetWaterLevel()  == 0 )
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
		return;
	}

	// If we are jumping out of water, don't do anything more.
	if ( player->m_flWaterJumpTime )         
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
		return;
	}

	// See if we collided with a ramp
	if( DotProduct( pm.plane.normal, -m_vGravityDirection ) > CRITICAL_SLOPE )
	{
		// Compute normalized forward direction in tangent plane
		const Vector vWishDirection = mv->m_vecVelocity.Normalized();
		const Vector vTangentRight = CrossProduct( vWishDirection, pm.plane.normal );
		const Vector vNormTangentForward = CrossProduct( pm.plane.normal, vTangentRight ).Normalized();

		// Move up the ramp
		float fSpeed = mv->m_vecVelocity.Length();
		Vector vEndPos = player->GetAbsOrigin() + (mv->m_vecVelocity * pm.fraction + vNormTangentForward * (1.0 - pm.fraction) * fSpeed) * gpGlobals->frametime;
		
		//above code has the distinct possibility of placing the player inside a wall. Not quite sure why it works so well most of the time. Add some error checking
		if( sv_portal_new_player_trace.GetBool() )
		{
			trace_t rampTrace;
			TracePlayerBBox( vEndPos, vEndPos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, rampTrace );

			if( !rampTrace.startsolid )
			{
				mv->SetAbsOrigin( vEndPos );
			}
			else
			{
				StepMove( dest, pm );
			}
		}
		else
		{
			mv->SetAbsOrigin( vEndPos );
		}
	}
	else
	{
		StepMove( dest, pm );
	}

#if defined GAME_DLL
	RANDOM_CEG_TEST_SECRET();
#endif

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

	StayOnGround();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPortalGameMovement::FullWalkMove()
{
	if ( !CheckWater() ) 
	{
		StartGravity();
	}

	// If we are leaping out of the water, just update the counters.
	if (player->m_flWaterJumpTime)
	{
		WaterJump();
		TryPlayerMove( 0, 0 );
		// See if we are still in water?
		CheckWater();
		return;
	}

	// If we are swimming in the water, see if we are nudging against a place we can jump up out
	//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
	if ( player->GetWaterLevel() >= WL_Waist ) 
	{
		// don't let player jump off the water in coop
		if ( !GameRules()->IsMultiplayer() )
		{
			if ( player->GetWaterLevel() == WL_Waist )
			{
				CheckWaterJump();
			}

			// If we are falling again, then we must not trying to jump out of water any more.
			if ( DotProduct( mv->m_vecVelocity, m_vGravityDirection) > 0.f && 
				player->m_flWaterJumpTime )
			{
				player->m_flWaterJumpTime = 0;
			}

			// Was jump button pressed?
			if (mv->m_nButtons & IN_JUMP)
			{
				CheckJumpButton();
			}
			else
			{
				mv->m_nOldButtons &= ~IN_JUMP;
			}
		}
		else
		{
			CPortal_Player *pPortalPlayer = static_cast< CPortal_Player* >( player );
			if ( !pPortalPlayer->m_Shared.InCond( PORTAL_COND_DROWNING ) )
			{
				pPortalPlayer->m_Shared.AddCond( PORTAL_COND_DROWNING );
			}
		}

		// Perform regular water movement
		WaterMove();

		// Redetermine position vars
		CategorizePosition();

		// If we are on ground, no downward velocity.
		if ( player->GetGroundEntity() != NULL )
		{
			mv->m_vecVelocity.z = 0;
		}
	}
	else
		// Not fully underwater
	{
		// Was jump button pressed?
		if (mv->m_nButtons & IN_JUMP)
		{
			CheckJumpButton();
		}
		else
		{
			mv->m_nOldButtons &= ~IN_JUMP;
		}

		// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
		//  we don't slow when standing still, relative to the conveyor.
		if (player->GetGroundEntity() != NULL)
		{
			mv->m_vecVelocity -= m_vGravityDirection * DotProduct( mv->m_vecVelocity, m_vGravityDirection );
			player->m_Local.m_flFallVelocity = 0.0f;
			Friction();
		}

		// Make sure velocity is valid.
		CheckVelocity();

		CPortal_Player *pPortalPlayer = static_cast< CPortal_Player* >( player );
		if ( pPortalPlayer->m_PortalLocal.m_hTractorBeam.Get() )
		{
			TBeamMove();
		}
		else
		{
			Vector vecVel = mv->m_vecVelocity;

			if ( player->GetGroundEntity() != NULL )
			{
				WalkMove();
			}
			else
			{
#if defined CLIENT_DLL
				RANDOM_CEG_TEST_SECRET();
#endif
				AirMove();  // Take into account movement when in air.
			}

			CheckWallImpact( vecVel );
		}

		// Set final flags.
		CategorizePosition();

		// Make sure velocity is valid.
		CheckVelocity();

		// Add any remaining gravitational component.
		if ( !CheckWater() )
		{
			FinishGravity();
		}

		// If we are on ground, no downward velocity.
		if ( player->GetGroundEntity() != NULL )
		{
			mv->m_vecVelocity -= m_vGravityDirection * DotProduct( mv->m_vecVelocity, m_vGravityDirection );
		}
		CheckFalling();
	}

	if  ( ( m_nOldWaterLevel == WL_NotInWater && player->GetWaterLevel() != WL_NotInWater ) ||
		( m_nOldWaterLevel != WL_NotInWater && player->GetWaterLevel() == WL_NotInWater ) )
	{
		PlaySwimSound();
#if !defined( CLIENT_DLL )
		player->Splash();
#endif
	}
}
#endif