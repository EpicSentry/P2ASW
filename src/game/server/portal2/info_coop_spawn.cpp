//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//  Everything has been reversed except for StringCompare, which is not possible,
//  as there appears to be no string comparison of any type in this file...
//
//============================================================================//

#include "cbase.h"
#include "baseanimating.h"
#include "baseentity.h"
#include "portal_mp_gamerules.h"    // To check Is2GunsCoOp() and TEAM_BLUE definition
#include "debugoverlay_shared.h"    // For drawing debug lines

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Atlas & P-Body spawn nodes for Portal MP GameRules
//-----------------------------------------------------------------------------
class CInfo_Coop_Spawn : public CBaseAnimating
{
public:
    DECLARE_CLASS( CInfo_Coop_Spawn, CBaseAnimating );
    DECLARE_DATADESC();

    virtual void Activate();

    void InputEnable( inputdata_t &inputdata );
    void InputDisable( inputdata_t &inputdata );
    void InputSetAsActiveSpawn( inputdata_t &inputdata );

    virtual void UpdateOnRemove();
    bool StringCompare( const char *, const char * ); // not showing up at ALL in vtable and is optimized...
    virtual int DrawDebugTextOverlays();

    bool m_bEnabled;            // Starts enabled as a possible spawning location
    int m_iStartingTeam;        // Restricts the spawning of all players to this specific team
    bool m_bForceGunOnSpawn;    // If set and portal_mp_gamerules says this Is2GunsCoOp(), force Blue team to get the PORTAL gun even if they were originally going to have the PAINT gun instead
};

BEGIN_DATADESC( CInfo_Coop_Spawn ) // Line 43
    DEFINE_KEYFIELD( m_bEnabled, FIELD_BOOLEAN, "Enabled" ),
    DEFINE_KEYFIELD( m_iStartingTeam, FIELD_INTEGER, "StartingTeam" ),
    DEFINE_KEYFIELD( m_bForceGunOnSpawn, FIELD_BOOLEAN, "ForceGunOnSpawn" ),

    // Input functions
    DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
    DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
    DEFINE_INPUTFUNC( FIELD_VOID, "SetAsActiveSpawn", InputSetAsActiveSpawn ),
END_DATADESC() // (L52 BP)

LINK_ENTITY_TO_CLASS( info_coop_spawn, CInfo_Coop_Spawn ); // (L54 BP)

static CUtlVector<CInfo_Coop_Spawn *> s_SpawnLocations; // Stores the number of info_coop_spawn's currently existing (L56 BP)
extern int g_iPortalGunPlayerTeam; // in portal_player.cpp

void CInfo_Coop_Spawn::Activate()
{ // Line 60
    s_SpawnLocations.AddToTail( this );
    ChangeTeam( m_iStartingTeam ); // This entity's team KeyValue is independent of "StartingTeam" only after this part is ran
    BaseClass::Activate();
}

void CInfo_Coop_Spawn::InputEnable( inputdata_t& inputdata )
{ // Line 67
    m_bEnabled = true;
}

void CInfo_Coop_Spawn::InputDisable( inputdata_t& inputdata )
{ // Line 72
    m_bEnabled = false;
}

//-----------------------------------------------------------------------------
// Purpose: Enables the spawn location AND turns off any others that match the same spawning criteria
//-----------------------------------------------------------------------------
void CInfo_Coop_Spawn::InputSetAsActiveSpawn( inputdata_t &inputdata ) // ( CPhysicsShadowClone::NotifyDestroy helps to reverse this )
{ // Line 80
    CInfo_Coop_Spawn **pKnownLocations = s_SpawnLocations.Base(); // Line 81
    int iKnownLocations = s_SpawnLocations.Count(); // Line 82

    // Iterate over the list of all coop spawn points
    // Turn off ALL other possible spawn locations except THIS
    for ( int i = 0; i < iKnownLocations; i++ ) // Line 86
    {
        if ( !pKnownLocations[i] ) // Line 88
        {
            Warning( "info_coop_spawn was removed but is still in known location list." ); // was removed from s_SpawnLocations (Should never see this)
        }
        else
        {
            if ( pKnownLocations[i] != this && pKnownLocations[i]->m_iStartingTeam == m_iStartingTeam ) // Line 94
            {
                pKnownLocations[i]->m_bEnabled = false; // Line 96
                if ( m_debugOverlays ) // Line 97
                {
                    NDebugOverlay::Cross3D( pKnownLocations[i]->GetAbsOrigin(), 4.0f, 255, 0, 0, true, 2.0f ); // red: Indicates the newly DISABLED spawnpoint (Line 99)
                }
            }
        }
    }
    if ( m_debugOverlays ) // Line 104
    {
        NDebugOverlay::Cross3D( GetAbsOrigin(), 4.0f, 0, 255, 0, true, 2.0f ); // green: Indicates the newly ENABLED spawnpoint (Line 106)
    }
    m_bEnabled = true; // Line 108 (THIS entity will be the new spawnpoint)
} // Line 109

void CInfo_Coop_Spawn::UpdateOnRemove() // ( CPVS_Extender::~CPVS_Extender() confirms structure )
{
    s_SpawnLocations.FindAndFastRemove( this ); // Line 113
    BaseClass::UpdateOnRemove(); // Line 114
}

CBaseEntity *GetCoopSpawnLocation( int iTeam ) // Purpose: Completely restored. ( CVGuiSystemModuleLoader::InitializeAllModules helps to reverse this )
{ // Line 118
    int iValidLocations = 0; // Line 119 (Note: Is this line and line 122 switched?)

    CInfo_Coop_Spawn **pKnownLocations = s_SpawnLocations.Base(); // Line 121
    CInfo_Coop_Spawn **pValidLocations = (CInfo_Coop_Spawn **)_alloca( sizeof(CInfo_Coop_Spawn*) * s_SpawnLocations.Count() ); // Create a table of all the coop spawnpoints that we have stored in s_SpawnLocations (Line 122)
    for ( int i = 0; i < s_SpawnLocations.Count(); i++ ) // Line 123
    {
        if ( pKnownLocations[i]->m_bEnabled ) // Line 125
        {
            if ( pKnownLocations[i]->GetTeamNumber() == iTeam || pKnownLocations[i]->GetTeamNumber() <= TEAM_UNASSIGNED ) // Line 127
            {
                pValidLocations[iValidLocations] = pKnownLocations[i]; // Temporarily store the valid locations from the s_SpawnLocations list that meet the criteria (Line 129)
                iValidLocations++; // Temporarily store the total number of valid locations (Line 130)
            }
        }
    }

    // If we didn't get a single location, we are done
    if ( !iValidLocations ) // Line 136
       return NULL; // Line 137

    CInfo_Coop_Spawn *pValidSpawn = pValidLocations[ RandomInt( 0, iValidLocations - 1 ) ]; // Choose a random spot to spawn at!!! (Line 139)

    // portal_player.cpp hack continuation
    if ( pValidSpawn->m_bForceGunOnSpawn && ((CPortalMPGameRules *)g_pGameRules)->Is2GunsCoOp() ) // Line 142
    {
        g_iPortalGunPlayerTeam = TEAM_BLUE; // Ensures that blue will get the portal gun if the KeyValue is set when on paintgun maps (Line 144)
    }

    return (CBaseEntity *)pValidSpawn; // Line 147
} // Line 148

bool CInfo_Coop_Spawn::StringCompare( const char *, const char * ) // Purpose: Supposedly compares two strings and returns true on match
{ // Unknown line placement (probably goes here since it fits...)
    return false; // Unused entirely
}

int CInfo_Coop_Spawn::DrawDebugTextOverlays() // Purpose: Draws debug text overlays
{ // Line 156
    int text_offset = BaseClass::DrawDebugTextOverlays();
    char tempstr[255];


    Q_snprintf( tempstr, sizeof(tempstr), "%s", m_bEnabled ? "Enabled" : "Disabled" ); // (L161 BP)
    EntityText( text_offset, tempstr, 0 ); // (L162 BP)
    text_offset++; // (L163 BP)

    return text_offset;
} // (L166 BP)
