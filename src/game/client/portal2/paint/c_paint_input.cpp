//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Mouse input routines
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include "hud.h"
#include "cdll_int.h"
#include "kbutton.h"
#include "basehandle.h"
#include "usercmd.h"
#include "c_paint_input.h"
#include "iviewrender.h"
#include "iclientmode.h"
#include "tier0/icommandline.h"
#include "vgui/isurface.h"
#include "vgui_controls/controls.h"
#include "vgui/cursor.h"
#include "cdll_client_int.h"
#include "cdll_util.h"
#include "tier1/convar_serverbounded.h"
#include "debugoverlay_shared.h"
#include "cam_thirdperson.h"

#ifdef PORTAL2
#include "c_portal_player.h"
#include "portal_util_shared.h"
#endif

#if defined( _X360 )
#include "xbox/xbox_win32stubs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// up / down
#define	PITCH	0
// left / right
#define	YAW		1

extern ConVar lookstrafe;
extern ConVar thirdperson_platformer;
extern ConVar cam_idealyaw;
extern ConVar cam_idealpitch;
extern ConVar_ServerBounded* m_pitch;
extern ConVar cl_pitchdown;
extern ConVar cl_pitchup;

extern ConVar m_yaw( "m_yaw","0.022", FCVAR_ARCHIVE, "Mouse yaw factor." );
extern ConVar m_side( "m_side","0.8", FCVAR_ARCHIVE, "Mouse side factor." );
static ConVar m_forward( "m_forward","1", FCVAR_ARCHIVE, "Mouse forward factor." );


ConVar debug_mouse("debug_mouse", "20.f");

ConVar debug_pitch_limit("debug_pitch_limit", "0.f");
//-----------------------------------------------------------------------------
// Purpose: ApplyMouse -- applies mouse deltas to CUserCmd
// Input  : viewangles - 
//			*cmd - 
//			mouse_x - 
//			mouse_y - 
//-----------------------------------------------------------------------------
#define stick_cam_input_suppression_threshhold	0.8f	//ConVar stick_cam_input_suppression_threshhold("stick_cam_input_suppression_threshhold", "0.8f", FCVAR_DEVELOPMENTONLY);
void C_Paint_Input::ApplyMouse( int nSlot, QAngle& viewangles, CUserCmd *cmd, float mouse_x, float mouse_y )
{
	//PerUserInput_t &user = GetPerUser( nSlot );

	// setup orientation vectors of player based on data from server
	CPortal_Player *pPlayer = C_Portal_Player::GetLocalPortalPlayer();

	if( !pPlayer )
		return;

	Vector vForward, vRight, vUp;
	Vector vPlayerUp = pPlayer->GetPortalPlayerLocalData().m_Up;
	Vector vStickNormal = pPlayer->GetPortalPlayerLocalData().m_StickNormal;
	AngleVectors( viewangles, &vForward, &vRight, &vUp );

	//scale mouse_y input while reorienting camera so players don't over correct themselves
	bool bShouldDampInput = !pPlayer->IsDoneReorienting();
	if ( bShouldDampInput && pPlayer->GetPortalPlayerLocalData().m_nStickCameraState == STICK_CAMERA_SURFACE_TRANSITION )
	{
		float flProgress = pPlayer->GetReorientationProgress();
		if( flProgress < stick_cam_input_suppression_threshhold )
			flProgress = 0.f;
		else if( flProgress < 1.f )
		{
			flProgress -= stick_cam_input_suppression_threshhold;
			flProgress /= ( 1.f - stick_cam_input_suppression_threshhold );
		}
		mouse_y *= flProgress;
	}

	if ( !((in_strafe.GetPerUser( nSlot ).state & 1) || lookstrafe.GetInt()) )
	{
		
		if ( CAM_IsThirdPerson() && pPlayer->IsTaunting() )
		{
			if ( mouse_x && !pPlayer->IsInterpolatingTauntAngles() )
			{
				// use the mouse to orbit the camera around the player, and update the idealAngle
				Vector vAngle = g_ThirdPersonManager.GetCameraOffsetAngles();
				vAngle[YAW] -= m_yaw.GetFloat() * mouse_x;
				g_ThirdPersonManager.SetCameraOffsetAngles( vAngle );

				if ( fabsf( vAngle[YAW] - pPlayer->GetTauntCamTargetYaw() ) > 30.0f )
				{
					pPlayer->SetFaceTauntCameraEndAngles( true );
				}
			}
		}
		else
		{
			// Otherwize, use mouse to spin around vertical axis
			{
				VMatrix rotMatrix;
				float angle = -m_yaw.GetFloat() * mouse_x;
				MatrixBuildRotationAboutAxis( rotMatrix, vPlayerUp, angle );
				vRight = rotMatrix * vRight;
			}
		}
	}
	else
	{
		// If holding strafe key or mlooking and have lookstrafe set to true, then apply
		//  horizontal mouse movement to sidemove.
		cmd->sidemove += m_side.GetFloat() * mouse_x;
	}

	// If mouselooking and not holding strafe key, then use vertical mouse
	//  to adjust view pitch.
	if (!(in_strafe.GetPerUser( nSlot ).state & 1))
	{
		
		if ( CAM_IsThirdPerson() && pPlayer->IsTaunting() )
		{
			if ( mouse_y && !pPlayer->IsInterpolatingTauntAngles() )
			{
				// use the mouse to orbit the camera around the player, and update the idealAngle
				Vector vAngle = g_ThirdPersonManager.GetCameraOffsetAngles();
				float flPitchLimitDown = -15.0f;
				float flPitchLimitUp = 55.0f;
				if ( pPlayer->m_Shared.InCond( PORTAL_COND_DROWNING ) )
				{
					flPitchLimitDown = 5.0f;
				}

				vAngle[PITCH] = clamp( vAngle[PITCH] + m_pitch->GetFloat() * mouse_y, flPitchLimitDown, flPitchLimitUp );

				g_ThirdPersonManager.SetCameraOffsetAngles( vAngle );
			}
		}
		else
		{
			float pitch =  90.0f - RAD2DEG( acos( clamp( DotProduct( vForward, vPlayerUp ), -1.f, 1.f ) ) );
			pitch += -m_pitch->GetFloat() * mouse_y;
			pitch = clamp( pitch, -cl_pitchup.GetFloat() + debug_pitch_limit.GetFloat(), cl_pitchdown.GetFloat() - debug_pitch_limit.GetFloat());

			VMatrix rotMatrix;
			MatrixBuildRotationAboutAxis( rotMatrix, vRight, pitch );

			vForward = CrossProduct( vPlayerUp, vRight );
			vForward = rotMatrix * vForward;
		}		
	}
	else
	{
		// Otherwise if holding strafe key and noclipping, then move upward
/*		if ((in_strafe.state & 1) && IsNoClipping() )
		{
			cmd->upmove -= m_forward.GetFloat() * mouse_y;
		} 
		else */
		{
			// Default is to apply vertical mouse movement as a forward key press.
			cmd->forwardmove -= m_forward.GetFloat() * mouse_y;
		}
	}

	//update viewangles
	QAngle newAngles, viewOffset( 0, 0, 0 );
	VectorAngles( vForward, vPlayerUp, newAngles );
	UTIL_NormalizedAngleDiff( viewangles, newAngles, &viewOffset );
	
	viewangles += viewOffset;

	// Finally, add mouse state to usercmd.
	// NOTE:  Does rounding to int cause any issues?  ywb 1/17/04
	cmd->mousedx = (int)mouse_x;
	cmd->mousedy = (int)mouse_y;
}

void C_Paint_Input::JoyStickTurn( CUserCmd *cmd, float &yaw, float &pitch, float frametime, bool bAbsoluteYaw, bool bAbsolutePitch )
{
	CInput::JoyStickTurn( cmd, yaw, pitch, frametime, bAbsoluteYaw, bAbsolutePitch );

	//PerUserInput_t &user = GetPerUser( nSlot );

	// setup orientation vectors of player based on data from server
	CPortal_Player *pPlayer = C_Portal_Player::GetLocalPortalPlayer();
	if ( !pPlayer )
		return;
	
	if ( CAM_IsThirdPerson() && pPlayer->IsTaunting() )
	{
		if ( yaw != 0.0f && !pPlayer->IsInterpolatingTauntAngles() )
		{
			Vector vAngle = g_ThirdPersonManager.GetCameraOffsetAngles();
			vAngle[YAW] -= yaw;
			g_ThirdPersonManager.SetCameraOffsetAngles( vAngle );

			if ( fabsf( vAngle[YAW] - pPlayer->GetTauntCamTargetYaw() ) > 30.0f )
			{
				pPlayer->SetFaceTauntCameraEndAngles( true );
			}
		}

		if ( pitch != 0.0f && !pPlayer->IsInterpolatingTauntAngles() )
		{
			Vector vAngle = g_ThirdPersonManager.GetCameraOffsetAngles();
			float flPitchLimitDown = -15.0f;
			float flPitchLimitUp = 55.0f;

			if ( pPlayer->m_Shared.InCond( PORTAL_COND_DROWNING ) )
			{
				flPitchLimitDown = 5.0f;
			}

			vAngle[PITCH] = clamp( vAngle[PITCH] + pitch, flPitchLimitDown, flPitchLimitUp );

			g_ThirdPersonManager.SetCameraOffsetAngles( vAngle );
		}
	}
	
}