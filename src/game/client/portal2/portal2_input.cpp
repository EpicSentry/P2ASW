#include "cbase.h"
#include "portal2_input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: ASW Input interface
//-----------------------------------------------------------------------------
static CPortal2Input g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;

CPortal2Input *Portal2Input()
{ 
	return &g_Input;
}

CPortal2Input::CPortal2Input()
{

}