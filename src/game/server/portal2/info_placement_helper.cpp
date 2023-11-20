//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//===========================================================================//

#include "cbase.h"

#include "info_placement_helper.h"
#include "debugoverlay_shared.h"    // For drawing debug lines
#include "portal_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
//
// Placement helper entity class
//
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( info_placement_helper, CInfoPlacementHelper );

BEGIN_DATADESC( CInfoPlacementHelper ) // Completely restored (Line 23)
    DEFINE_KEYFIELD( m_strTargetProxy, FIELD_STRING, "proxy_name" ),
    DEFINE_KEYFIELD( m_strTargetEntity, FIELD_STRING, "attach_target_name" ), // TODO: What is this used for?
    DEFINE_KEYFIELD( m_flRadius, FIELD_FLOAT, "radius" ),
    DEFINE_KEYFIELD( m_bSnapToHelperAngles, FIELD_BOOLEAN, "snap_to_helper_angles" ),
    DEFINE_KEYFIELD( m_bForcePlacement, FIELD_BOOLEAN, "force_placement" ),
    DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

    DEFINE_FIELD( m_flDisableTime, FIELD_TIME ),
    DEFINE_FIELD( m_bDeferringToPortal, FIELD_BOOLEAN ),

    DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
    DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

    DEFINE_OUTPUT( m_OnObjectPlaced, "OnObjectPlaced" ), // TODO: Used maybe with OnObjectPlaced( CBaseEntity * ) (L37 BP)
    DEFINE_OUTPUT( m_ObjectPlacedSize, "OnObjectPlacedSize" ) // TODO: Used maybe with OnObjectPlaced( CBaseEntity * ) (L38 BP)
END_DATADESC() // (L39 BP)


IMPLEMENT_SERVERCLASS_ST( CInfoPlacementHelper, DT_InfoPlacementHelper ) // Completely restored (Line 42)
    // Valve's hacky solution to overriding the positioning vectors...
    SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),
    SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
    SendPropVector( SENDINFO( m_vecOrigin ) ),
    SendPropVector( SENDINFO( m_angRotation ) ), // (L47 BP)

    SendPropString( SENDINFO( m_strTargetProxy ) ), // (L49 BP)
    SendPropString( SENDINFO( m_strTargetEntity ) ),
    SendPropFloat( SENDINFO( m_flRadius ) ),
    SendPropBool( SENDINFO( m_bSnapToHelperAngles ) ),
    SendPropBool( SENDINFO( m_bForcePlacement ) ),
    SendPropBool( SENDINFO( m_bDisabled ) ),
    SendPropFloat( SENDINFO( m_flDisableTime ) ),
    SendPropBool( SENDINFO( m_bDeferringToPortal ) ),
END_SEND_TABLE() // (L57 BP)

// TODO: Oddly not used in this file; What is it actually used for? Junk ConVar?
ConVar sv_show_placement_help_in_preview( "sv_show_placement_help_in_preview", "0", FCVAR_NONE, "Forces the placement preview to show any help in placement given from info_placement_helper entities.\n" );
extern ConVar sv_portal_placement_debug;
//-----------------------------------------------------------------------------
// Purpose: Placement helper manager implementation
//-----------------------------------------------------------------------------
CInfoPlacementManager g_PlacementManager; // Global for the class

void CInfoPlacementManager::LevelShutdownPostEntity() // Purpose: Taken from Linux. Originally optimized to just call m_PlacementHelpers.Purge() from here;
{ // Unknown line
    Purge();
}

void CInfoPlacementManager::Purge() // Purpose: Optimized out of all builds. m_PlacementHelpers.Purge() was in LevelShutdownPostEntity but it's likely that it was just pointing to this function
{ // Unknown line
    m_PlacementHelpers.Purge();
}

void CInfoPlacementManager::AddPlacementHelper( CInfoPlacementHelper *pHelper ) // Purpose: Completely restored. Taken from Windows
{ // Line 78
    // Attempt to add it to our list
    if ( m_PlacementHelpers.Find( pHelper ) != m_PlacementHelpers.InvalidIndex() ) // Line 80
        return;

    m_PlacementHelpers.AddToTail( pHelper ); // Line 83
} // Line 84

void CInfoPlacementManager::RemovePlacementHelper( CInfoPlacementHelper *pHelper ) // Purpose: Completely restored. Optimized, but reimplemented based on the logic
{ // Unknown line (probably goes here...)
    // Attempt to remove it from our list
    if ( m_PlacementHelpers.Find( pHelper ) == m_PlacementHelpers.InvalidIndex() )
        return;

    m_PlacementHelpers.FindAndFastRemove( pHelper );
}

CInfoPlacementHelper *CInfoPlacementManager::FindPlacementHelper( const Vector &vecOrigin, CBasePlayer *pPlayer ) // Purpose: Functionally completely restored. Need to replace an optimized var name
{ // Line 96
    // We need a player
    if ( !pPlayer ) // Line 98
        return NULL; // Line 99

    Vector vecEyeDir = pPlayer->EyeDirection3D(); // Line 101
    Vector vecEyePos = pPlayer->EyePosition(); // Line 102

    // Try to find the best helper according to the positional data
    CInfoPlacementHelper *pBestHelper = NULL; // Line 105
    float flBestDist = 9.9999998e17; // 10677D30: using guessed type const float FLOAT_9_9999998e17; prob a max value for vector tracing in a #define idk (Line 106)

    for ( int i = 0; i < g_PlacementManager.m_PlacementHelpers.Count(); i++ ) // Line 108
    {
        // Iterate over all placement helpers and get the one that is closest to the player's EyePosition, since thats where the gun shoots
        CInfoPlacementHelper *pHelper = m_PlacementHelpers[i].Get(); // Line 111

        if ( !pHelper ) // Line 113
            continue;

        // See if it is in a useable state
        if ( !pHelper->IsEnabled() ) // Line 117
            continue;

        Vector vecTargetDir = pHelper->GetAbsOrigin() - vecEyePos; // Line 120
        float flTargetDist = VectorNormalize( vecTargetDir ); // Line 121

        float BLAH1 = DotProduct( vecEyeDir, vecTargetDir ); // Line 123 (optimized var name)
        BLAH1 = acos( BLAH1 ); // __libm_sse2_acosf runs on line 124 (This means acos() runs)

        // Get the angle using the radius and see if this helper has better distancing than the previous
        if ( BLAH1 <= atan2( pHelper->GetTargetRadius(), flTargetDist ) && flTargetDist <= flBestDist ) // __libm_sse2_atan2 runs on line 127 (This means atan2() runs)
        {

            // Narrow down the best helper
            pBestHelper = pHelper; // Line 131


            // Narrow down the best distance
            flBestDist = flTargetDist; // Located somewhere in between here


            if ( sv_portal_placement_debug.GetBool() ) // Line 138
            {
                Msg("PortalPlacement: Considering placement helper (%s) at %f, %f, %f. Radius %f\n", STRING( pHelper->GetEntityName() ), XYZ( pHelper->GetAbsOrigin() ), pHelper->GetTargetRadius() ); // Line 140
            }
        }
    }
    return pBestHelper; // Line 144
} // Line 145

//-----------------------------------------------------------------------------
// Purpose: Placement helper implementation
//-----------------------------------------------------------------------------



// ?? Line 153

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Queries through our list to find an optimal placement helper
//-----------------------------------------------------------------------------
CInfoPlacementHelper *UTIL_FindPlacementHelper( const Vector &vecEndPoint, CBasePlayer *pPlayer )
{ // Line 159
    return g_PlacementManager.FindPlacementHelper( vecEndPoint, pPlayer ); // Line 160
} // Line 161

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
CInfoPlacementHelper::CInfoPlacementHelper()
                    : m_bSnapToHelperAngles( false ),
                      m_bForcePlacement( false ),
                      m_flDisableTime( 0.0 ),
                      m_bDeferringToPortal( false )
{ // Line 171
} // Line 172

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::Spawn()
{ // Line 178
    BaseClass::Spawn(); // Line 179

    g_PlacementManager.AddPlacementHelper( this ); // Line 181
} // Line 182

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::OnRestore()
{ // Line 188
    // Add the placement helper again to the list
    g_PlacementManager.AddPlacementHelper( this ); // Line 190
    BaseClass::OnRestore(); // Line 191
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::UpdateOnRemove()
{ // Line 198
    BaseClass::UpdateOnRemove(); // Line 199

    g_PlacementManager.RemovePlacementHelper( this ); // Line 201
} // Line 202

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
bool CInfoPlacementHelper::ShouldUseHelperAngles()
{
    return m_bSnapToHelperAngles; // Line 209
} // Line 210

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::InputEnable( inputdata_t &inputdata )
{
    Enable(); // Replaced with optimized function "Enable" (Line 217)
} // Line 218

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::InputDisable( inputdata_t &inputdata )
{
    Disable(); // Replaced with optimized function "Disable" (Line 225)
} // Line 226

//-----------------------------------------------------------------------------
// Purpose: Very close, just need to figure out the green color if IsEnabled()
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::DrawDebugGeometryOverlays()
{ // Line 232
    NDebugOverlay::Axis( GetAbsOrigin(), GetAbsAngles(), 8.0, false, 0.1 ); // Line 233

    // NDebugOverlay::Sphere( GetAbsOrigin(), GetAbsAngles(), GetTargetRadius(), 255, ( (unsigned __int8) - IsEnabled() ), 0, 0, false, 0.1 ); // TODO
    BaseClass::DrawDebugGeometryOverlays();

} // Line 238

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Optimized, but reimplemented based on the logic
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::Enable()
{ // Unknown line (probably either goes here, or exchanged with the below func...)
    if ( m_bDisabled )
        m_bDisabled = false;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Optimized, but reimplemented based on the logic
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::Disable()
{ // Unknown line (probably either goes here, or exchanged with the above func...)
    if ( !m_bDisabled )
        m_bDisabled = true;
}

//-----------------------------------------------------------------------------
// Purpose: Functionally completely restored (TODO: Find the correct inline for line 269)
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::NotifyPortalEvent( PortalEvent_t nEventType, CPortal_Base2D *pNotifier )
{ // Line 262
	if ( !nEventType ) // Line 263
        return;

    if ( nEventType == PORTALEVENT_MOVED ) // Line 266
    {
        Vector &vecDifference = pNotifier->GetAbsOrigin() - GetTargetOrigin(); // vecDifference is a custom var name (Written like this since these funcs are only called once each)
        if ( DotProduct( vecDifference, vecDifference ) < 4.0 ) // GetTargetOrigin() and vector logic called on line 269
            return;
    }

    // We are done, unhook us from the system
    pNotifier->RemovePortalEventListener( this ); // Line 274
    m_bDeferringToPortal = false; // Line 275

    // Set up the reset timing
    float flResetDelay = ( nEventType == PORTALEVENT_FIZZLE ) ? 0.0 : 3.0; // Line 278
    m_flDisableTime = gpGlobals->curtime + flResetDelay; // Line 279 (Could this be Disable(float ) ?)
} // Line 280

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::BindToPortal( CPortal_Base2D *pPortal )
{ // Line 286
    if ( !pPortal ) // Line 287
        return;

    // Don't need to do anything if we force placement
    if ( ShouldForcePlacement() ) // Replaced with optimized function "ShouldForcePlacement" (Line 291)
        return;

    // Hook us to the system (Enables CInfoPlacementHelper::NotifyPortalEvent to be called)
    pPortal->AddPortalEventListener( this ); // Line 295

    m_bDeferringToPortal = true; // Line 297
} // Line 298











// These have no proper placement...


//-----------------------------------------------------------------------------
// Purpose: Completely restored (Linux)
//-----------------------------------------------------------------------------
int CInfoPlacementHelper::UpdateTransmitState()
{ // Unknown line (probably goes in header)
    return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored (Linux)
//-----------------------------------------------------------------------------
bool CInfoPlacementHelper::IsEnabled()
{ // Unknown line
    if ( !m_bDisabled && !m_bDeferringToPortal )
    {
        return ShouldForcePlacement() ? true : ( gpGlobals->curtime > m_flDisableTime ); // Replaced with optimized function "ShouldForcePlacement"
    }
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
bool CInfoPlacementHelper::ShouldForcePlacement()
{ // Unknown line
    return m_bForcePlacement;
}

//-----------------------------------------------------------------------------
// Purpose: ??
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::Disable(float )
{ // Unknown line
    // Impossible to tell I would assume. Or just unused entirely
}

//-----------------------------------------------------------------------------
// Purpose: ??
//-----------------------------------------------------------------------------
void CInfoPlacementHelper::OnObjectPlaced( CBaseEntity * )
{ // Unknown line
    // More than likely does stuff with the m_OnObjectPlaced and m_ObjectPlacedSize outputs; Look into that
}
