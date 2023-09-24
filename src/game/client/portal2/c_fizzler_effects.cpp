//========= Copyright � 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//=====================================================================================//

#include "cbase.h"

#include "c_fizzler_effects.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_FizzlerEffects, DT_FizzlerEffects, FizzlerEffects ) // Line 14
END_RECV_TABLE() // (L15 BP)

LINK_ENTITY_TO_CLASS( fizzler_effects, C_FizzlerEffects ); // (L17 BP)
