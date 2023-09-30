//========= Copyright � 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//=====================================================================================//

#include "cbase.h"
#include "utlvector.h"

#include "c_info_placement_helper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_InfoPlacementHelper, DT_InfoPlacementHelper, CInfoPlacementHelper )
    RecvPropVector( RECVINFO( m_vecOrigin ) ),
    RecvPropVector( RECVINFO( m_angRotation ) ),

    RecvPropString( RECVINFO( m_strTargetProxy ) ),
    RecvPropString( RECVINFO( m_strTargetEntity ) ),
    RecvPropFloat( RECVINFO( m_flRadius ) ),
    RecvPropBool( RECVINFO( m_bSnapToHelperAngles ) ),
    RecvPropBool( RECVINFO( m_bForcePlacement ) ),
    RecvPropBool( RECVINFO( m_bDisabled ) ),
    RecvPropFloat( RECVINFO( m_flDisableTime ) ),
    RecvPropBool( RECVINFO( m_bDeferringToPortal ) ),
END_RECV_TABLE()

CUtlVector<C_InfoPlacementHelper *> s_PlacementHelpers; // TODO: Find line placements for the below functions

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
C_InfoPlacementHelper::C_InfoPlacementHelper()
{
    m_bSnapToHelperAngles = false;
    m_bForcePlacement = false;
    m_flDisableTime = 0.0;
    m_bDeferringToPortal = false;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored (Linux had better decompile)
//-----------------------------------------------------------------------------
void C_InfoPlacementHelper::Spawn()
{
    BaseClass::Spawn();
    s_PlacementHelpers.AddToTail( this );
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void C_InfoPlacementHelper::UpdateOnRemove()
{
    BaseClass::UpdateOnRemove();
    s_PlacementHelpers.FindAndFastRemove( this );
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored (Linux)
//-----------------------------------------------------------------------------
void C_InfoPlacementHelper::BindToPortal( C_Portal_Base2D *pPortal )
{
    if ( pPortal )
    {
        if ( !ShouldForcePlacement() ) // Replaced with optimized function "ShouldForcePlacement"
            m_bDeferringToPortal = true;
    }
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored (Linux)
//-----------------------------------------------------------------------------
bool C_InfoPlacementHelper::IsEnabled()
{ // Unknown line
    if ( !m_bDisabled && !m_bDeferringToPortal )
    {
        return ShouldForcePlacement() ? true : ( gpGlobals->curtime > m_flDisableTime ); // Replaced with optimized function "ShouldForcePlacement"
    }
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Not sure if this is right; Will revisit soonTM
//-----------------------------------------------------------------------------
C_InfoPlacementHelper *UTIL_FindPlacementHelper( const Vector &vecEndPoint, C_BasePlayer *pPlayer )
{
	// We need a player
    if ( !pPlayer )
        return NULL;

	Vector vecEyeDir, vecEyePos;
	pPlayer->EyePositionAndVectors( &vecEyePos, &vecEyeDir, NULL, NULL );

    // Try to find the best helper according to the positional data
    C_InfoPlacementHelper *pBestHelper = NULL;
    float flBestDist = 9.9999998e17; // 10677D30: using guessed type const float FLOAT_9_9999998e17; prob a max value for vector tracing in a #define idk

    for ( int i = 0; i < s_PlacementHelpers.Count(); i++ )
    {
        // Iterate over all placement helpers and get the one that is closest to the player's EyePosition, since thats where the gun shoots
        C_InfoPlacementHelper *pHelper = s_PlacementHelpers[i];

        if ( !pHelper )
            continue;

        // See if it is in a useable state
        if ( !pHelper->IsEnabled() )
            continue;

        Vector vecTargetDir = pHelper->GetAbsOrigin() - vecEyePos;
        float flTargetDist = VectorNormalize( vecTargetDir );

        float BLAH1 = DotProduct( vecEyeDir, vecTargetDir );
        BLAH1 = acos( BLAH1 );

        // Get the angle using the radius and see if this helper has better distancing than the previous
        if ( BLAH1 <= atan2( pHelper->GetTargetRadius(), flTargetDist ) && flTargetDist <= flBestDist )
        {

            // Narrow down the best helper
            pBestHelper = pHelper;


            // Narrow down the best distance
            flBestDist = flTargetDist;
        }
    }
    return pBestHelper;
}