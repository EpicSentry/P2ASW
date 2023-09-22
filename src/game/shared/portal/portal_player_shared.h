//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef PORTAL_PLAYER_SHARED_H
#define PORTAL_PLAYER_SHARED_H
#pragma once

#define PORTAL_PUSHAWAY_THINK_INTERVAL		(1.0f / 20.0f)
#include "studio.h"
#include "paint/paint_color_manager.h"
#include "cegclientwrapper.h"
#include "paint/paint_power_info.h"

//=============================================================================
// Paint Power Choice
//=============================================================================
struct PaintPowerChoiceCriteria_t
{
	Vector vNormInputDir;
	Vector vNormVelocity;
	bool bInPortal;
};

struct PaintPowerChoiceResult_t
{
	const PaintPowerInfo_t* pPaintPower;
	float flInputCos;
	float flVelocityCos;
	bool bWasIgnored;

	inline void Initialize()
	{
		pPaintPower = NULL;
		flInputCos = 1.0f;
		flVelocityCos = 2.0f;
		bWasIgnored = false;
	}
};

typedef CUtlVectorFixed< PaintPowerChoiceResult_t, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER > PaintPowerChoiceResultArray;

void ExpandAABB(Vector& boxMin, Vector& boxMax, const Vector& sweepVector);

enum
{
	PLAYER_SOUNDS_CITIZEN = 0,
	PLAYER_SOUNDS_COMBINESOLDIER,
	PLAYER_SOUNDS_METROPOLICE,
	PLAYER_SOUNDS_MAX,
};

enum 
{
	CONCEPT_CHELL_IDLE,
	CONCEPT_CHELL_DEAD,
};

struct CachedPaintPowerChoiceResult
{
	Vector surfaceNormal;
	CBaseHandle surfaceEntity;
	bool wasValid;
	bool wasIgnored;

	inline void Initialize()
	{
		surfaceNormal = Vector(0, 0, 0);
		surfaceEntity = NULL;
		wasValid = false;
		wasIgnored = false;
	}
};

enum JumpButtonPress
{
	JUMP_ON_TOUCH = 0,
	PRESS_JUMP_TO_BOUNCE,
	HOLD_JUMP_TO_BOUNCE,
	TRAMPOLINE_BOUNCE
};

enum InAirState
{
	ON_GROUND,
	IN_AIR_JUMPED,
	IN_AIR_BOUNCED,
	IN_AIR_FELL
};

enum PaintSurfaceType
{
	FLOOR_SURFACE = 0,
	WALL_SURFACE,
	CEILING_SURFACE
};

enum StickCameraState
{
	STICK_CAMERA_SURFACE_TRANSITION = 0,
	STICK_CAMERA_ROLL_CORRECT,
	STICK_CAMERA_PORTAL,
	STICK_CAMERA_WALL_STICK_DEACTIVATE_TRANSITION,
	STICK_CAMERA_SWITCH_TO_ABS_UP_MODE,
	STICK_CAMERA_ABS_UP_MODE,
	STICK_CAMERA_SWITCH_TO_LOCAL_UP,
	STICK_CAMERA_SWITCH_TO_LOCAL_UP_LOOKING_UP,
	STICK_CAMERA_LOCAL_UP_LOOKING_UP,
	STICK_CAMERA_UPRIGHT
};

enum StickCameraCorrectionMethod
{
	QUATERNION_CORRECT = 0,
	ROTATE_UP,
	SNAP_UP,
	DO_NOTHING
};

extern const char *g_pszChellConcepts[];
int GetChellConceptIndexFromString( const char *pszConcept );

#if defined( CLIENT_DLL )
#define CPortal_Player C_Portal_Player
#endif


#endif //PORTAL_PLAYER_SHARED_h
