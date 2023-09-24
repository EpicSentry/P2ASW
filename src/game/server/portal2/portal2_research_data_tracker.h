//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
// $NoKeywords: $
//===========================================================================//
#ifndef PORTAL2_RESEARCH_DATA_TRACKER_H
#define PORTAL2_RESEARCH_DATA_TRACKER_H

#if !defined( _GAMECONSOLE )
#include "igamesystem.h"
#include "matchmaking/imatchframework.h"
#include "portal_player.h"

class CPortal2ResearchDataTracker : public CAutoGameSystem, public IMatchEventsSink // Purpose: Coop stats for steps, deaths, etc
{
public:
	virtual const char* Name() { return "CPortal2ResearchDataTracker"; } // Line 18
    virtual bool Init();
    virtual void Shutdown();
    virtual void LevelInitPostEntity();
    virtual void OnEvent( KeyValues *pEvent );

    void Event_PlayerGaveUp();
    void Event_LevelCompleted();

    // Changes a player's stats
    void IncrementPortalFired( CPortal_Player *pPortalPlayer );
    void IncrementStepsTaken( CPortal_Player *pPortalPlayer );
    void IncrementDeath( CPortal_Player *pPortalPlayer );
    void SetPlayerName( CPortal_Player *pPortalPlayer );

private:
    void ResetResearchData();
    bool ShouldResetStartTime();
    int GetStatIndex( CPortal_Player *pPortalPlayer );

    // The writing logic
    void BeginRecordResearchData();
    void WriteResearchDataToFile( bool bLevelCompleted );

    struct tm m_levelStartLocalTime;

    // Initialized with 2 possible simultaneous values,
    // first is for TEAM_BLUE and second is for TEAM_RED
    int m_nPortalFired[2];
    int m_nStepsTaken[2];
    int m_nDeaths[2];
    char m_pszName[2][64]; // 64 is the max length for usernames here

    bool m_bWritten;
};

extern CPortal2ResearchDataTracker g_Portal2ResearchDataTracker;

#endif // !defined( _GAMECONSOLE )

#endif // PORTAL2_RESEARCH_DATA_TRACKER_H
