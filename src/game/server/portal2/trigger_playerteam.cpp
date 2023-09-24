//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//===========================================================================//

#include "cbase.h"
#include "baseentity.h"

#include "triggers.h"
#include "portal_mp_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Small trigger entity for firing outputs based on team number
// TODO: This class MUST be defined elsewhere oddly since there is literally no room given the line defs...
//-----------------------------------------------------------------------------
// idk if any of these are static, however, they are all correctly placed
class CTriggerPlayerTeam : public CBaseTrigger
{
public:
    DECLARE_CLASS( CTriggerPlayerTeam, CBaseTrigger );
    DECLARE_DATADESC();

    virtual void Spawn();
    virtual void StartTouch( CBaseEntity *pOther );
    virtual void EndTouch( CBaseEntity *pOther );

private:
    // Key fields
    int m_nTargetTeam;
    bool m_bTriggerOnce;

    // Entity outputs
    COutputEvent m_OnStartTouchOrangePlayer;
    COutputEvent m_OnEndTouchOrangePlayer;
    COutputEvent m_OnStartTouchBluePlayer;
    COutputEvent m_OnEndTouchBluePlayer;
};




void CTriggerPlayerTeam::Spawn() // Purpose: Completely restored
{ // Line 22
    ClearSpawnFlags(); // Line 23
    AddSpawnFlags( SF_TRIGGER_ALLOW_CLIENTS ); // Line 24
    BaseClass::Spawn(); // Line 25

    InitTrigger(); // Line 27
}

void CTriggerPlayerTeam::StartTouch( CBaseEntity *pOther ) // Purpose: Completely restored
{ // Line 31
    // This is a player trigger
    if ( !pOther || !pOther->IsPlayer() ) // Line 33
        return;

    // Look for coop bots only
    if ( m_nTargetTeam > 0 && pOther->GetTeamNumber() != m_nTargetTeam ) // Line 37
        return;

    BaseClass::StartTouch( pOther ); // Line 40

    if ( pOther->GetTeamNumber() == TEAM_RED ) // Line 42
    {
        m_OnStartTouchOrangePlayer.FireOutput( pOther, this ); // Line 44
    }
    else if ( pOther->GetTeamNumber() == TEAM_BLUE ) // Line 46
    {
        m_OnStartTouchBluePlayer.FireOutput( pOther, this ); // Line 48
    }

    // Check if it fires only once
    if ( m_bTriggerOnce ) // Line 52
    {
        UTIL_Remove( this ); // Line 54
    }
} // Line 56

void CTriggerPlayerTeam::EndTouch( CBaseEntity *pOther ) // Purpose: Completely restored
{ // Line 31
    // This is a player trigger
    if ( !pOther || !pOther->IsPlayer() ) // Line 61
        return;

    // Look for coop bots only
    if ( m_nTargetTeam > 0 && pOther->GetTeamNumber() != m_nTargetTeam ) // Line 65
        return;

    BaseClass::EndTouch( pOther ); // Line 68

    if ( pOther->GetTeamNumber() == TEAM_RED ) // Line 70
    {
        m_OnEndTouchOrangePlayer.FireOutput( pOther, this ); // Line 72
    }
    else if ( pOther->GetTeamNumber() == TEAM_BLUE ) // Line 74
    {
        m_OnEndTouchBluePlayer.FireOutput( pOther, this ); // Line 76
    }
} // Line 78











BEGIN_DATADESC( CTriggerPlayerTeam ) // Line 90

    DEFINE_KEYFIELD( m_nTargetTeam, FIELD_INTEGER, "target_team" ),

    DEFINE_KEYFIELD( m_bTriggerOnce, FIELD_BOOLEAN, "trigger_once" ),

    DEFINE_OUTPUT( m_OnStartTouchOrangePlayer, "OnStartTouchOrangePlayer" ), // (L96 BP)
    DEFINE_OUTPUT( m_OnEndTouchOrangePlayer, "OnEndTouchOrangePlayer" ), // (L97 BP)

    DEFINE_OUTPUT( m_OnStartTouchBluePlayer, "OnStartTouchBluePlayer" ), // (L99 BP)
    DEFINE_OUTPUT( m_OnEndTouchBluePlayer, "OnEndTouchBluePlayer" ), // (L100 BP)

END_DATADESC() // (L102 BP)

// Link it
LINK_ENTITY_TO_CLASS( trigger_playerteam, CTriggerPlayerTeam ); // (L105 BP)
