//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//============================================================================//

#include "cbase.h"
#include "baseentity.h"
#include "eventqueue.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PORTAL2_LANDMARK_NAME "__p2_landmark" // Custom define name

//-----------------------------------------------------------------------------
// Purpose: Sets the exact location in the world for transitioning maps and is referenced via targetname
//-----------------------------------------------------------------------------
class CInfoLandmark : public CPointEntity
{
public:
    DECLARE_CLASS( CInfoLandmark, CPointEntity );

    virtual void Activate();
};

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoLandmark::Activate()
{
    BaseClass::Activate();
    SetName( AllocPooledString( PORTAL2_LANDMARK_NAME ) ); // Line 33
}

//-----------------------------------------------------------------------------
// Purpose: A simple class to prepare maps to change to new levels in Portal 2
//-----------------------------------------------------------------------------
class CPointChangelevel : public CPointEntity
{
public:
    DECLARE_CLASS( CPointChangelevel, CPointEntity );
    DECLARE_DATADESC();
private:
    void InputChangeLevel( inputdata_t &inputdata );
    void InputChangeLevelPostFade( inputdata_t &inputdata );

    COutputEvent m_OnChangeLevel;
};

static char st_szOriginMap[ 32 ]; // cchMapNameMost?
static char st_szDestinationMap[ 32 ]; // cchMapNameMost?

ConVar sv_transition_fade_time( "sv_transition_fade_time", "0.5", FCVAR_DEVELOPMENTONLY );

LINK_ENTITY_TO_CLASS( info_landmark_entry, CInfoLandmark ); // Valve goes by classname to differentiate between the two possible landmark types
LINK_ENTITY_TO_CLASS( info_landmark_exit, CInfoLandmark );  // See CChangeLevel::BuildChangeLevelList for the implementation

BEGIN_DATADESC( CPointChangelevel ) // Line 59

    DEFINE_INPUTFUNC( FIELD_STRING, "ChangeLevel", InputChangeLevel ),
    DEFINE_INPUTFUNC( FIELD_STRING, "ChangeLevelPostFade", InputChangeLevelPostFade ),

    DEFINE_OUTPUT( m_OnChangeLevel, "OnChangeLevel" )

END_DATADESC()

LINK_ENTITY_TO_CLASS( point_changelevel, CPointChangelevel );

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CPointChangelevel::InputChangeLevel( inputdata_t &inputdata )
{ // Line 74


    float flFadeTime = sv_transition_fade_time.GetFloat(); // Line 77

    CReliableBroadcastRecipientFilter allplayers;

    UserMessageBegin( allplayers, "TransitionFade" ); // Line 81
        WRITE_FLOAT( flFadeTime );
    MessageEnd(); // Line 83

    // Add the event to queue
    g_EventQueue.AddEvent( this, "ChangeLevelPostFade", inputdata.value, flFadeTime, inputdata.pActivator, inputdata.pCaller, inputdata.nOutputID ); // Line 86
}


//-----------------------------------------------------------------------------
// Purpose: Completely restored
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CPointChangelevel::InputChangeLevelPostFade( inputdata_t &inputdata )
{ // Line 98
    // Fire entity output
    m_OnChangeLevel.FireOutput( inputdata.pActivator, this ); // Line 100

    // Update our map info
    Q_strncpy( st_szOriginMap, STRING( gpGlobals->mapname ), sizeof( st_szOriginMap ) ); // Line 103
    Q_strncpy( st_szDestinationMap, inputdata.value.String(), sizeof( st_szDestinationMap ) );

    // Fire the base game logic now
    GameRules()->OnBeginChangeLevel( st_szDestinationMap, NULL ); // Line 107
    engine->ChangeLevel( st_szDestinationMap, PORTAL2_LANDMARK_NAME ); // Line 108
}


//-----------------------------------------------------------------------------
// Purpose: Funcs for triggers.cpp
//-----------------------------------------------------------------------------
const char *ChangeLevel_DestinationMapName( void ) // Purpose: Completely restored
{ // Line 116
    return st_szDestinationMap; // Line 117
}

const char *ChangeLevel_OriginMapName( void ) // Purpose: Completely restored
{ // Line 121
    return st_szOriginMap; // Line 122
}

const char *ChangeLevel_GetLandmarkName( void ) // Purpose: Completely restored
{ // Line 126
    return PORTAL2_LANDMARK_NAME; // Line 127
}
