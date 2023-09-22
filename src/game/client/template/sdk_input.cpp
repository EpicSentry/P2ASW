#include "cbase.h"
#include "sdk_input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: ASW Input interface
//-----------------------------------------------------------------------------
static CSDKInput g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;

CSDKInput *SDKInput() 
{ 
	return &g_Input;
}

CSDKInput::CSDKInput()
{

}