#ifndef _INCLUDED_SDK_INPUT_H
#define _INCLUDED_SDK_INPUT_H
#ifdef _WIN32
#pragma once
#endif

#include "input.h"

//-----------------------------------------------------------------------------
// Purpose: ASW Input interface
//-----------------------------------------------------------------------------
class CPortal2Input : public CInput
{
public:
	CPortal2Input();
};

extern CPortal2Input *SDKInput();

#endif // _INCLUDED_SDK_INPUT_H