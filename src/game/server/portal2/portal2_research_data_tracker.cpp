//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//===========================================================================//
#include "cbase.h"
#include "portal2_research_data_tracker.h"
#include "portal_mp_gamerules.h"
#include "filesystem.h"
#include "keyvalues.h"
#include "../../../public/tier0/platform.h" // TODO: Need to fix Plat_timegm definition

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define RESEARCH_FILE "portal2_research_data.csv" // Custom define
#if !defined( _GAMECONSOLE )
ConVar record_research_data( "record_research_data", "0", FCVAR_NONE, "In challenge and editor maps, output research data to " RESEARCH_FILE " if enabled" );

CPortal2ResearchDataTracker g_Portal2ResearchDataTracker; // This global is used in other files to write the statistics

bool CPortal2ResearchDataTracker::Init() // Purpose: Completely restored
{ // Line 23
    if ( g_pMatchFramework && g_pMatchFramework->GetEventsSubscription() )
    {
        g_pMatchFramework->GetEventsSubscription()->Subscribe( this );
    }

    ResetResearchData();

    return true;
}


void CPortal2ResearchDataTracker::Shutdown() // Purpose: Completely restored
{ // Line 36
    if ( g_pMatchFramework && g_pMatchFramework->GetEventsSubscription() ) // Line 37
    {
        g_pMatchFramework->GetEventsSubscription()->Unsubscribe( this ); // Line 39
    }
}


void CPortal2ResearchDataTracker::LevelInitPostEntity() // Purpose: Completely restored
{
    BeginRecordResearchData(); // Line 46
}


void CPortal2ResearchDataTracker::OnEvent( KeyValues *pEvent ) // Purpose: Completely restored (Hook to tell whether or not a player quit the map)
{ // Line 51
    const char* szEvent = pEvent->GetName(); // Line 52
    const char* pzState = pEvent->GetString( "state", "" );
    if ( !Q_stricmp( "OnMatchSessionUpdate", szEvent ) ) // Line 54
    {

        if ( !Q_stricmp( "closed", pzState ) )
        {

            Event_PlayerGaveUp();
        }
    }
} // Line 63


// Called through CPortal2ResearchDataTracker::OnEvent and CPortal_Player::ClientCommand "restart_level"
void CPortal2ResearchDataTracker::Event_PlayerGaveUp() // Purpose: Completely restored
{ // Line 68
    WriteResearchDataToFile( false ); // Line 69
    ResetResearchData(); // Line 70
} // Line 71


// Called through RequestMapRating and CPortal_UI_Controller::OnLevelEnd
void CPortal2ResearchDataTracker::Event_LevelCompleted() // Purpose: Completely restored
{ // Line 76
    WriteResearchDataToFile( true ); // Line 77
    ResetResearchData(); // Line 78
} // Line 79

// Called through CPortal_Player::IncrementPortalsPlaced
void CPortal2ResearchDataTracker::IncrementPortalFired( CPortal_Player *pPortalPlayer ) // Purpose: Completely restored
{ // Line 83
    int nStatIndex = GetStatIndex( pPortalPlayer );
    m_nPortalFired[ nStatIndex ]++;
}

// Called through CPortal_Player::IncrementStepsTaken
void CPortal2ResearchDataTracker::IncrementStepsTaken( CPortal_Player *pPortalPlayer ) // Purpose: Completely restored
{ // Line 90
    int nStatIndex = GetStatIndex( pPortalPlayer );
    m_nStepsTaken[ nStatIndex ]++;
}

// Called through CPortal_Player::Event_Killed
void CPortal2ResearchDataTracker::IncrementDeath( CPortal_Player *pPortalPlayer ) // Purpose: Completely restored
{ // Line 97
    int nStatIndex = GetStatIndex( pPortalPlayer );
    m_nDeaths[ nStatIndex ]++;
}

// Called through CPortal_Player::Spawn
void CPortal2ResearchDataTracker::SetPlayerName( CPortal_Player *pPortalPlayer ) // Purpose: Completely restored
{ // Line 104
    int nStatIndex = GetStatIndex( pPortalPlayer );
    Q_snprintf( m_pszName[ nStatIndex ], sizeof( m_pszName[ nStatIndex ] ), pPortalPlayer->GetPlayerName() );
}


//-----------------------------------------------------------------------------
// Research data logic...
//-----------------------------------------------------------------------------
void CPortal2ResearchDataTracker::ResetResearchData() // Purpose: Completely restored (Linux)
{ // Unknown line placement
    Q_memset( m_nPortalFired, 0, sizeof( m_nPortalFired ) );
    Q_memset( m_nStepsTaken, 0, sizeof( m_nStepsTaken ) );
    Q_memset( m_nDeaths, 0, sizeof( m_nDeaths ) );
    Q_memset( m_pszName, 0, sizeof( m_pszName ) );
    Q_memset( m_pszName[ 1 ], 0, sizeof( m_pszName[ 1 ] ) ); // separate change for red name? maybe strings work a bit differently with this func
}



bool CPortal2ResearchDataTracker::ShouldResetStartTime() // Purpose: Completely restored (Linux)
{ // Unknown line placement
    if ( g_pGameRules->IsMultiplayer() )
    {
        if ( !m_nDeaths[ 0 ] && !m_nDeaths[ 1 ] )
        {
            return true; // We should reset the start time
        }
    }
    else
    {
        if ( !m_nDeaths[ 0 ] ) // SP mode
        {
            return true;
        }
    }

    return false;
}


// A helper func that gets the index number for the member var arrays
// depending on the player's team number for the portal player methods
int CPortal2ResearchDataTracker::GetStatIndex( CPortal_Player *pPortalPlayer ) // Purpose: Completely restored (Linux)
{ // Unknown line placement
    if ( g_pGameRules->IsMultiplayer() )
    {
        return ( pPortalPlayer->GetTeamNumber() != TEAM_BLUE ); // Red player index
    }

    return 0; // Blue player index
}



void CPortal2ResearchDataTracker::BeginRecordResearchData() // Purpose: Completely restored
{ // Line 160
    if ( ShouldResetStartTime() ) // If nobody died yet
    {
        Plat_GetLocalTime( &m_levelStartLocalTime ); // Set m_levelStartLocalTime
    }

    m_bWritten = false; // Line 166
} // Line 167



void CPortal2ResearchDataTracker::WriteResearchDataToFile( bool bLevelCompleted ) // Purpose: Completely restored
{ // Line 172
    if ( !record_research_data.GetBool() || m_bWritten ) // Line 173
    {
        return;
    }

    // Check if it already exists
    bool bFileExists = g_pFullFileSystem->FileExists( RESEARCH_FILE ); // Line 179

    // Get us at the file
    FileHandle_t handle = g_pFullFileSystem->Open( RESEARCH_FILE, "a" ); // Line 182
    if ( !g_pFullFileSystem->IsOk( handle ) ) // Line 183
    {
        Warning( "Failed to write to survey file '%s'\n", RESEARCH_FILE ); // Was written in a way that implied a #define (Line 185)
        return;
    }

    if ( !bFileExists ) // Line 189
    {
        g_pFullFileSystem->FPrintf( handle, "Name,Date,Level,Completed Level,Start Time,End Time, Elapsed Time(seconds), Portals Fired, Steps Taken, Deaths\n" ); // Line 191
    }

    char pszMap[ 64 ];
    Q_FileBase( STRING( gpGlobals->mapname ), pszMap, sizeof( pszMap ) ); // Line 195

    char pszDate[ 32 ];
    Q_snprintf( pszDate, sizeof( pszDate ), "%02i-%02i-%i", m_levelStartLocalTime.tm_mon + 1, m_levelStartLocalTime.tm_mday, m_levelStartLocalTime.tm_year + 1900 ); // Line 198

    char pszStartTime[ 32 ];
    Q_snprintf( pszStartTime, sizeof( pszStartTime ), "%02i:%02i:%02i", m_levelStartLocalTime.tm_hour, m_levelStartLocalTime.tm_min, m_levelStartLocalTime.tm_sec ); // Line 201

    struct tm levelEndLocalTime;
    Plat_GetLocalTime( &levelEndLocalTime ); // Line 204
    char pszEndTime[ 32 ];
    Q_snprintf( pszEndTime, sizeof( pszEndTime ), "%02i:%02i:%02i", levelEndLocalTime.tm_hour, levelEndLocalTime.tm_min, levelEndLocalTime.tm_sec ); // Line 206

    // Get the total time spent
	int nElapsedTime = 1; //Plat_timegm( &levelEndLocalTime ) - Plat_timegm( &m_levelStartLocalTime ); // Line 209 // (TODO: Fix platform.h defs to make this work!!!)

    for ( int i = 1; i <= gpGlobals->maxClients; ++i ) // Line 211
    {
        g_pFullFileSystem->FPrintf( handle, "%s,%s,%s,%i,%s,%s,%i,%i,%i,%i\n",
                                            m_pszName,
                                            pszDate,
                                            pszMap, bLevelCompleted,
                                            pszStartTime, pszEndTime, nElapsedTime,
                                            m_nPortalFired,
                                            m_nStepsTaken,
                                            m_nDeaths ); // Line 220
    }

    g_pFullFileSystem->Close( handle ); // Line 223

    m_bWritten = true; // Line 225
} // Line 226

#endif // !defined( _GAMECONSOLE )
