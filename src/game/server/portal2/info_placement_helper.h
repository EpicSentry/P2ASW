//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef INFO_PLACEMENT_HELPER_H
#define INFO_PLACEMENT_HELPER_H
#ifdef _WIN32
#pragma once
#endif
#include "baseentity.h"
#include "portal_base2d.h"
#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Purpose: Entity to help correct portal placement shots
//-----------------------------------------------------------------------------
class CInfoPlacementHelper : public CPointEntity
{
public:
    DECLARE_CLASS( CInfoPlacementHelper, CPointEntity );
    DECLARE_SERVERCLASS();
    DECLARE_DATADESC();

    CInfoPlacementHelper();

    virtual void Spawn();
    virtual void UpdateOnRemove();
    virtual void DrawDebugGeometryOverlays();
    virtual void NotifyPortalEvent( PortalEvent_t nEventType, CPortal_Base2D *pNotifier );
    virtual void OnRestore();
    virtual int UpdateTransmitState(); // Linux
    virtual int ObjectCaps() // Purpose: Completely restored
    { // Line 36
        return BaseClass::ObjectCaps() | (m_hOwnerEntity.Get() != NULL ? FCAP_DONT_SAVE : 0); // Line 37
    } // Line 38
    float GetTargetRadius() { return m_flRadius; } // Optimized, however, confirmed to be 100% accurate since it is used in CWeaponPortalgun::AttemptSnapToPlacementHelper
    void OnObjectPlaced( CBaseEntity * ); // ??

    CBaseEntity *GetTargetOverride() // Purpose: Provide an alternative entity's information instead of this one
    { // Note: This is completely optimized, but restored based on repeated logic in GetTargetOrigin() and GetTargetAngles()
        CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_strTargetProxy ); // pEntity is a custom var name
        if ( pEntity )
        {
            return pEntity;
        }
        else
        {
            Warning( "Placement proxy entity %s not found!\n", STRING( m_strTargetProxy.Get() ) );
        }
        return NULL;
    }

	const Vector &GetTargetOrigin() // Purpose: Completely restored. Rewritten using the GetTargetOverride optimized function
    { // Line 57

		if ( m_strTargetProxy.Get() != NULL_STRING )
		{
			CBaseEntity* pProxy = GetTargetOverride();

			if ( pProxy )
			{
				return pProxy->GetAbsOrigin();
			}
		}
		return GetAbsOrigin();
    } // Line 69

    const QAngle &GetTargetAngles() // Purpose: Completely restored. Rewritten using the GetTargetOverride optimized function
    { // Line 72

		if ( m_strTargetProxy.Get() != NULL_STRING )
		{

			CBaseEntity* pProxy = GetTargetOverride();

			if ( pProxy )
			{

				return pProxy->GetAbsAngles();
			}
		}
		return GetAbsAngles();
    } // Line 86

    // Entity KeyValue checks
    bool ShouldUseHelperAngles();                   // Linux. Used only in CWeaponPortalgun::AttemptSnapToPlacementHelper
    bool ShouldForcePlacement();                    // Optimized (Reimplemented based on an obvious prediction)

    // Entity status
    void Enable();
    void Disable(float );                           // ?? Optimized
    void Disable();
    bool IsEnabled();                               // Linux

    void BindToPortal( CPortal_Base2D *pPortal );

    // Entity inputs
    void InputEnable( inputdata_t &inputdata );
    void InputDisable( inputdata_t &inputdata );

    // Entity outputs
    COutputEvent m_OnObjectPlaced;
    COutputFloat m_ObjectPlacedSize;

    // Proxy entity compatibility
    CNetworkVar( string_t, m_strTargetProxy );
    CNetworkVar( string_t, m_strTargetEntity );
    CNetworkVar( float, m_flRadius );

    CNetworkVar( bool, m_bSnapToHelperAngles );
    CNetworkVar( bool, m_bForcePlacement );

    CNetworkVar( bool, m_bDisabled );
    CNetworkVar( float, m_flDisableTime );

private:
    CNetworkVar( bool, m_bDeferringToPortal );
};

//-----------------------------------------------------------------------------
// Purpose: Internal class to help allocate and manage existing helpers
//          Has a similar structure to CRemarkableEntityList
//-----------------------------------------------------------------------------
// idk if these are even correct or static apart from their names
// However, they are all correctly placed
// TODO: What is InfoPlacementHandle_t ?
class CInfoPlacementManager : public CAutoGameSystem
{
public:
    virtual void LevelShutdownPostEntity();															// Linux

    void Purge();																					// Optimized out of all builds; reimplmented based on obvious predictions...
    void AddPlacementHelper( CInfoPlacementHelper *pHelper );										// Windows
    void RemovePlacementHelper( CInfoPlacementHelper *pHelper );									// need to confirm pHelper variable name!
    CInfoPlacementHelper *FindPlacementHelper( const Vector &vecOrigin, CBasePlayer *pPlayer );		// Windows
private:
    CUtlVector<CHandle<CInfoPlacementHelper>> m_PlacementHelpers;									// list of placement helpers in the current level
};

extern CInfoPlacementManager g_PlacementManager;

extern CInfoPlacementHelper *UTIL_FindPlacementHelper( const Vector &vecEndPoint, CBasePlayer *pPlayer );

extern ConVar sv_show_placement_help_in_preview; // TODO: Oddly not used in this file; What is it actually used for? Junk ConVar?

#endif // INFO_PLACEMENT_HELPER_H