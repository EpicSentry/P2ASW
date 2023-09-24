//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//============================================================================//

#include "cbase.h"
#include "baseentity.h"
#include "triggers.h"
#include "portal_mp_gamerules.h"
#include "./../../../public/mathlib/vector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Brush entity that fires outputs from ping tool based on whether a player is on TEAM_BLUE or TEAM_RED
// TODO: This class MUST be defined elsewhere oddly since there is literally no room given the line defs...
//-----------------------------------------------------------------------------
// idk if any of these are static, however, they are all correctly placed
class CTriggerPingDetector : public CBaseTrigger, public CGameEventListener
{
public:
    DECLARE_CLASS( CTriggerPingDetector, CBaseTrigger );
    DECLARE_DATADESC();

    virtual void Spawn();
    virtual void FireGameEvent( IGameEvent *event ); // virtual ?

private:
    COutputEvent m_OnOrangePlayerPinged;
    COutputEvent m_OnBluePlayerPinged;
};

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CTriggerPingDetector::Spawn()
{ // Line 20
    ClearSpawnFlags(); // Line 21
    BaseClass::Spawn(); // Line 22
    InitTrigger(); // Line 23
    ListenForGameEvent( "portal_player_ping" ); // Line 24
}


void CTriggerPingDetector::FireGameEvent( IGameEvent *event ) // Purpose: Completely restored
{ // Line 29
    const char *name = event->GetName();
    if ( !Q_strcmp( name, "portal_player_ping" ) ) // Line 31
    {
        // Get player ID
        int playerId = event->GetInt( "userid" ); // Line 34
        int nPlayerTeam = -1;
        Vector vecPingPos;
        vecPingPos.x = event->GetFloat( "ping_x" ); // Line 37
        vecPingPos.y = event->GetFloat( "ping_y" ); // Line 38
        vecPingPos.z = event->GetFloat( "ping_z" ); // Line 39

        for ( int i = 1; i <= MAX_PLAYERS; i++ ) // Line 41
        {
            CBasePlayer *pPlayer = UTIL_PlayerByIndex( i ); // Line 43
            if ( pPlayer && playerId == pPlayer->GetUserID() )
            {
                nPlayerTeam = pPlayer->GetTeamNumber(); // Line 46
                break;
            }
        }

        Ray_t shotRay;
        shotRay.Init( vecPingPos, vecPingPos ); // Line 52
        trace_t tr;
        enginetrace->ClipRayToEntity( shotRay, MASK_ALL, this, &tr ); // Line 54

        if ( tr.DidHitNonWorldEntity() && tr.m_pEnt == this ) // If the trace hit THIS entity (Line 56)
        {
            if ( nPlayerTeam == TEAM_RED )
            {
                m_OnOrangePlayerPinged.FireOutput( NULL, NULL ); // Line 60
            }
            else
            {
                m_OnBluePlayerPinged.FireOutput( NULL, NULL ); // Line 64
            }
        }
    }
} // Line 68






BEGIN_DATADESC( CTriggerPingDetector ) // Line 75
    DEFINE_OUTPUT( m_OnOrangePlayerPinged, "OnOrangePlayerPinged" ), // (L76 BP)
    DEFINE_OUTPUT( m_OnBluePlayerPinged, "OnBluePlayerPinged" ), // (L77 BP)
END_DATADESC() // (L78 BP)

LINK_ENTITY_TO_CLASS( trigger_ping_detector, CTriggerPingDetector ); // (L80 BP)
