//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "cam_thirdperson.h"
#include "gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static Vector CAM_HULL_MIN(-CAM_HULL_OFFSET,-CAM_HULL_OFFSET,-CAM_HULL_OFFSET);
static Vector CAM_HULL_MAX( CAM_HULL_OFFSET, CAM_HULL_OFFSET, CAM_HULL_OFFSET);

#ifdef CLIENT_DLL

#include "input.h"


extern const ConVar *sv_cheats;

extern ConVar cam_idealdist;
extern ConVar cam_idealdistright;
extern ConVar cam_idealdistup;

void CAM_ToThirdPerson(void);
void CAM_ToFirstPerson(void);

void ToggleThirdPerson( bool bValue )
{
	if ( bValue == true )
	{
		CAM_ToThirdPerson();
	}
	else
	{
		CAM_ToFirstPerson();
	}
}

void ThirdPersonChange( IConVar *pConVar, const char *pOldValue, float flOldValue )
{
	ConVarRef var( pConVar );

	ToggleThirdPerson( var.GetBool() );
}

ConVar cl_thirdperson( "cl_thirdperson", "0", FCVAR_NOT_CONNECTED | FCVAR_ARCHIVE | FCVAR_DEVELOPMENTONLY, "Enables/Disables third person", ThirdPersonChange  );

#endif

CThirdPersonManager::CThirdPersonManager( void )
{
}

void CThirdPersonManager::Init( void )
{
	m_bOverrideThirdPerson = false;
	m_bForced = false;
	m_flUpFraction = 0.0f;
	m_flFraction = 1.0f;

	m_flUpLerpTime = 0.0f;
	m_flLerpTime = 0.0f;

	m_flUpOffset = CAMERA_UP_OFFSET;

	if ( input )
	{
		input->CAM_SetCameraThirdData( NULL, vec3_angle );
	}
}

void CThirdPersonManager::Update( void )
{

#ifdef CLIENT_DLL
	if ( !sv_cheats )
	{
		sv_cheats = cvar->FindVar( "sv_cheats" );
	}

	bool bIsThirdPerson = input->CAM_IsThirdPerson() != 0;

	// If cheats have been disabled, pull us back out of third-person view.
	if ( sv_cheats && !sv_cheats->GetBool() && GameRules() && GameRules()->AllowThirdPersonCamera() == false )
	{
		if ( bIsThirdPerson )
		{
			input->CAM_ToFirstPerson();
		}
		return;
	}

	if ( IsOverridingThirdPerson() == false )
	{
		
		if ( bIsThirdPerson != ( cl_thirdperson.GetBool() || m_bForced ) && GameRules() && GameRules()->AllowThirdPersonCamera() == true )
		{
			ToggleThirdPerson( m_bForced || cl_thirdperson.GetBool() );
		}
	}
#endif

}

Vector CThirdPersonManager::GetDesiredCameraOffset( void )
{ 
	if ( IsOverridingThirdPerson() == true )
	{
		return Vector( cam_idealdist.GetFloat(), cam_idealdistright.GetFloat(), cam_idealdistup.GetFloat() );
	}

	return m_vecDesiredCameraOffset; 
}

Vector CThirdPersonManager::GetFinalCameraOffset( void )
{
	Vector vDesired = GetDesiredCameraOffset();

	if ( m_flUpFraction != 1.0f )
	{
		vDesired.z += m_flUpOffset;
	}

	return vDesired;

}

Vector CThirdPersonManager::GetDistanceFraction( void )
{
	if ( IsOverridingThirdPerson() == true )
	{
		return Vector( m_flTargetFraction, m_flTargetFraction, m_flTargetFraction );
	}

	float flFraction = m_flFraction;
	float flUpFraction = m_flUpFraction;

	float flFrac = RemapValClamped( gpGlobals->curtime - m_flLerpTime, 0, CAMERA_OFFSET_LERP_TIME, 0, 1 );

	flFraction = Lerp( flFrac, m_flFraction, m_flTargetFraction );

	if ( flFrac == 1.0f )
	{
		m_flFraction = m_flTargetFraction;
	}

	flFrac = RemapValClamped( gpGlobals->curtime - m_flUpLerpTime, 0, CAMERA_UP_OFFSET_LERP_TIME, 0, 1 );

	flUpFraction = 1.0f - Lerp( flFrac, m_flUpFraction, m_flTargetUpFraction );

	if ( flFrac == 1.0f )
	{
		m_flUpFraction = m_flTargetUpFraction;
	}

	return Vector( flFraction, flFraction, flUpFraction );
}

void CThirdPersonManager::PositionCamera( CBasePlayer *pPlayer, QAngle angles )
{
	if ( pPlayer )
	{
		trace_t trace;

		Vector camForward, camRight, camUp;

		// find our player's origin, and from there, the eye position
		Vector origin = pPlayer->GetLocalOrigin();
		origin += pPlayer->GetViewOffset();

		AngleVectors( angles, &camForward, &camRight, &camUp );

		Vector endPos = origin;

		Vector vecCamOffset = endPos + (camForward * - GetDesiredCameraOffset()[DIST_FORWARD]) + (camRight * GetDesiredCameraOffset()[ DIST_RIGHT ]) + (camUp * GetDesiredCameraOffset()[ DIST_UP ] );

		// use our previously #defined hull to collision trace
		CTraceFilterSimple traceFilter( pPlayer, COLLISION_GROUP_NONE );
		UTIL_TraceHull( endPos, vecCamOffset, CAM_HULL_MIN, CAM_HULL_MAX, MASK_SOLID & ~CONTENTS_MONSTER, &traceFilter, &trace );

		if ( trace.fraction != m_flTargetFraction )
		{
			m_flLerpTime = gpGlobals->curtime;
		}

		m_flTargetFraction = trace.fraction;
		m_flTargetUpFraction = 1.0f;

		//If we're getting closer to a wall snap the fraction right away.
		if ( m_flTargetFraction < m_flFraction )
		{
			m_flFraction = m_flTargetFraction;
			m_flLerpTime = gpGlobals->curtime;
		}


		// move the camera closer if it hit something
		if( trace.fraction < 1.0  )
		{
			m_vecCameraOffset[ DIST ] *= trace.fraction;

			UTIL_TraceHull( endPos, endPos + (camForward * - GetDesiredCameraOffset()[DIST_FORWARD]), CAM_HULL_MIN, CAM_HULL_MAX, MASK_SOLID & ~CONTENTS_MONSTER, &traceFilter, &trace );

			if ( trace.fraction != 1.0f )
			{
				if ( trace.fraction != m_flTargetUpFraction )
				{
					m_flUpLerpTime = gpGlobals->curtime;
				}

				m_flTargetUpFraction = trace.fraction;

				if ( m_flTargetUpFraction < m_flUpFraction )
				{
					m_flUpFraction = trace.fraction;
					m_flUpLerpTime = gpGlobals->curtime;
				}
			}
		}
	}
}

bool CThirdPersonManager::WantToUseGameThirdPerson( void )
{
	return cl_thirdperson.GetBool() && GameRules() && GameRules()->AllowThirdPersonCamera() && IsOverridingThirdPerson() == false;
}


CThirdPersonManager g_ThirdPersonManager;