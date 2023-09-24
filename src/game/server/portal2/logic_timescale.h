//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef LOGIC_TIMESCALE_H
#define LOGIC_TIMESCALE_H
#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

//-----------------------------------------------------------------------------
// Purpose: Entity to manipulate time!
//-----------------------------------------------------------------------------
class CLogicTimeScale : public CLogicalEntity
{
    DECLARE_CLASS( CLogicTimeScale, CLogicalEntity );
    DECLARE_DATADESC();

public:
    CLogicTimeScale();
    virtual ~CLogicTimeScale();

    void InputSetDesiredTimescale( inputdata_t &inputdata );
    void InputSetTimescaleBlendTime( inputdata_t &inputdata );

private:
    float m_flBlendTime;
    float m_flTimeScale;
};

#endif // LOGIC_TIMESCALE_H