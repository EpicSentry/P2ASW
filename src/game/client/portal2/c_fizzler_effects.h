//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef C_FIZZLER_EFFECTS_H
#define C_FIZZLER_EFFECTS_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"

//-----------------------------------------------------------------------------
// Purpose: Entity to apply a dynamic fizzle effect to
//-----------------------------------------------------------------------------
class C_FizzlerEffects : public C_BaseEntity
{
public:
    DECLARE_CLASS( C_FizzlerEffects, C_BaseEntity );
    DECLARE_CLIENTCLASS();
};

#endif // C_FIZZLER_EFFECTS_H