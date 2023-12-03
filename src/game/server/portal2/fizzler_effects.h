//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef FIZZLER_EFFECTS_H
#define FIZZLER_EFFECTS_H
#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

//-----------------------------------------------------------------------------
// Purpose: Entity to apply a dynamic fizzle effect to
//-----------------------------------------------------------------------------
// idk if any of these are static or not
class FizzlerEffects : public CBaseEntity
{
public:
    DECLARE_CLASS( FizzlerEffects, CBaseEntity );
    DECLARE_DATADESC();
    DECLARE_SERVERCLASS();

    FizzlerEffects();

    static FizzlerEffects *Create( CBaseEntity *pAttachTo );

    virtual void Spawn();
    virtual int UpdateTransmitState();

    void AttachToEntity( CBaseEntity *pAttachTo );

private:
    void FizzleThink();

    float m_ElapsedAnimationTime;
    bool m_ScaleStarted;
};

#endif // FIZZLER_EFFECTS_H