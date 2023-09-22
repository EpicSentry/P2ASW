//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#if !defined( PAINT_INPUT_H )
#define PAINT_INPUT_H
#ifdef _WIN32
#pragma once
#endif

#include "input.h"
#include "mathlib/vector.h"
#include "kbutton.h"
#include "ehandle.h"
#include "inputsystem/analogcode.h"
#include "shareddefs.h"


class C_Paint_Input : public CInput
{
// Interface
public:
	C_Paint_Input( void ) {};
	~C_Paint_Input( void ) {};

protected:
	virtual void ApplyMouse( int nSlot, QAngle& viewangles, CUserCmd *cmd, float mouse_x, float mouse_y );
	virtual void JoyStickTurn( CUserCmd *cmd, float &yaw, float &pitch, float frametime, bool bAbsoluteYaw, bool bAbsolutePitch );
	//ClampAngles
};



#endif // PAINT_INPUT_H
	
