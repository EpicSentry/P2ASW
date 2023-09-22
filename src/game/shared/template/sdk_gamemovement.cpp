//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Special handling for hl2 usable ladders
//
//=============================================================================//
#include "cbase.h"
#include "coordsize.h"
#include "movevars_shared.h"
#include "in_buttons.h"
#include "utlrbtree.h"
#include "gamemovement.h"

#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
#else
	#include "sdk_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CSDKGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS( CSDKGameMovement, CGameMovement );

	virtual bool CheckJumpButton( void );
};

// Expose our interface.
static CSDKGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CSDKGameMovement::CheckJumpButton( void )
{
	if (!player->IsAlive())
	{
		mv->m_nOldButtons |= IN_JUMP ;	// don't jump again until released
		return false;
	}

	// See if we are waterjumping.  If so, decrement count and return.
	float flWaterJumpTime = player->GetWaterJumpTime();

	if ( flWaterJumpTime > 0 )
	{
		flWaterJumpTime -= gpGlobals->frametime;
		if (flWaterJumpTime < 0)
			flWaterJumpTime = 0;

		player->SetWaterJumpTime( flWaterJumpTime );
		
		return false;
	}

	// If we are in the water most of the way...
	if ( player->GetWaterLevel() >= 2 )
	{	
		// swimming, not jumping
		SetGroundEntity( NULL );

		if(player->GetWaterType() == CONTENTS_WATER)    // We move up a certain amount
			mv->m_vecVelocity[2] = 100;
		else if (player->GetWaterType() == CONTENTS_SLIME)
			mv->m_vecVelocity[2] = 80;
		
		// play swiming sound
		if ( player->GetSwimSoundTime() <= 0 )
		{
			// Don't play sound again for 1 second
			player->SetSwimSoundTime( 1000 );
			PlaySwimSound();
		}

		return false;
	}

	// No more effect
 	if (player->GetGroundEntity() == NULL)
	{
		mv->m_nOldButtons |= IN_JUMP;
		return false;		// in air, so no effect
	}

	if ( mv->m_nOldButtons & IN_JUMP )
		return false;		// don't pogo stick

	// In the air now.
	SetGroundEntity( NULL );
	
	//player->PlayStepSound( (Vector &)mv->GetAbsOrigin(), player->GetSurfaceData(), 1.0, true );
	dynamic_cast<CSDK_Player *>(player)->DoAnimationEvent( PLAYERANIMEVENT_JUMP );

//Tony; liek the landing sound, leaving this here if as an example for playing a jump sound.
//	// make the jump sound
//	CPASFilter filter( m_pSDKPlayer->GetAbsOrigin() );
//	filter.UsePredictionRules();
//	player->EmitSound( filter, player->entindex(), "Player.Jump" );

	float flGroundFactor = 1.0f;
	/*if ( player->GetSurfaceData() )
	{
		flGroundFactor = player->GetSurfaceData()->game.jumpFactor; 
	}*/	


	Assert( sv_gravity.GetFloat() == 800.0f );

	float flJumpHeight = 268.3281572999747f;
	
	// Accelerate upward
	// If we are ducking...
	float startz = mv->m_vecVelocity[2];
	if ( (  player->m_Local.m_bDucking ) || (  player->GetFlags() & FL_DUCKING ) )
	{
		// d = 0.5 * g * t^2		- distance traveled with linear accel
		// t = sqrt(2.0 * 45 / g)	- how long to fall 45 units
		// v = g * t				- velocity at the end (just invert it to jump up that high)
		// v = g * sqrt(2.0 * 45 / g )
		// v^2 = g * g * 2.0 * 45 / g
		// v = sqrt( g * 2.0 * 45 )
							
		mv->m_vecVelocity[2] = flGroundFactor * flJumpHeight;		// flJumpSpeed of 45
	}
	else
	{
		mv->m_vecVelocity[2] += flGroundFactor * flJumpHeight;	// flJumpSpeed of 45
	}
	
	FinishGravity();

	mv->m_outWishVel.z += mv->m_vecVelocity[2] - startz;
	mv->m_outStepHeight += 0.1f;

	// Flag that we jumped.
	mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released

	//player->m_Shared.SetJumping( true );

	return true;
}
