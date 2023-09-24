//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
// Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//=============================================================================//
#include "cbase.h"
#include "logic_timescale.h"
#include "game_timescale_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( logic_timescale, CLogicTimeScale );

BEGIN_DATADESC( CLogicTimeScale ) // Purpose: Completely restored (Line 13)
    DEFINE_KEYFIELD( m_flBlendTime, FIELD_FLOAT, "BlendTime" ),
    // DEFINE_KEYFIELD( m_flTimeScale, FIELD_FLOAT, "TimeScale" ),

    DEFINE_INPUTFUNC( FIELD_FLOAT, "SetDesiredTimescale", InputSetDesiredTimescale ),
    DEFINE_INPUTFUNC( FIELD_FLOAT, "SetTimescaleBlendTime", InputSetTimescaleBlendTime ),
END_DATADESC()

CLogicTimeScale::CLogicTimeScale() // Purpose: Completely restored (Linux)
{ // Unknown line (Probably goes here)
}

CLogicTimeScale::~CLogicTimeScale() // Purpose: Completely restored (Linux)
{ // Unknown line (Probably goes here)
}

void CLogicTimeScale::InputSetDesiredTimescale( inputdata_t &inputdata ) // Purpose: Completely restored
{ // Line 30
    GameTimescale()->SetDesiredTimescale( inputdata.value.Float(), m_flBlendTime, CGameTimescale::INTERPOLATOR_LINEAR, 0.0 ); // Line 31
} // Line 32

void CLogicTimeScale::InputSetTimescaleBlendTime( inputdata_t &inputdata ) // Purpose: Completely restored
{ // Line 35
    if ( m_flBlendTime < 0.0 ) // Line 36
        return;

    m_flBlendTime = inputdata.value.Float(); // Line 39
} // Line 40
