#ifndef _INCLUDED_SDK_INPUT_H
#define _INCLUDED_SDK_INPUT_H
#ifdef _WIN32
#pragma once
#endif

#include "input.h"

//-----------------------------------------------------------------------------
// Purpose: ASW Input interface
//-----------------------------------------------------------------------------
class CSDKInput : public CInput
{
public:
	CSDKInput();
};

extern CSDKInput *SDKInput();

#endif // _INCLUDED_SDK_INPUT_H