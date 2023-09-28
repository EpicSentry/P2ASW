//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef C_FIZZLER_EFFECTS_H
#define C_FIZZLER_EFFECTS_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"

//-----------------------------------------------------------------------------
// Purpose: Placement helper for a prop_portal
//-----------------------------------------------------------------------------
class C_InfoPlacementHelper : public C_BaseEntity
{
public:
    DECLARE_CLASS( C_InfoPlacementHelper, C_BaseEntity );
    DECLARE_CLIENTCLASS();

    C_InfoPlacementHelper();

    void Spawn();
    void UpdateOnRemove();

    float GetTargetRadius() { return m_flRadius; } // Optimized; Retrieved based on prediction and previous encounter in server class
    C_BaseEntity *GetTargetOverride(); // Optimized
    Vector &GetTargetOrigin(); // Optimized
    const QAngle &GetTargetAngles()
    { // Unknown line; Optimized, but reimplemented based on the compiled logic
        if ( m_strTargetProxy )
        {
            return vec3_angle;
        }
        else
        {
            return GetAbsAngles();
        }
    }

    bool ShouldUseHelperAngles() { return m_bSnapToHelperAngles; } // Unknown line
    bool ShouldForcePlacement() { return m_bForcePlacement; } // Unknown line (Optimized, but obvious)

    void Disable(float ); // Optimized
    bool IsEnabled();
    void BindToPortal( C_Portal_Base2D *pPortal );
    bool ShouldRegenerateOriginFromCellBits() { return false; } // Completely restored. Unknown line

private:
    const char *m_strTargetProxy;
    const char *m_strTargetEntity;
    float m_flRadius;
    bool m_bSnapToHelperAngles;
    bool m_bForcePlacement;
    bool m_bDisabled;
    float m_flDisableTime;
    bool m_bDeferringToPortal;
};

extern C_InfoPlacementHelper *UTIL_FindPlacementHelper( const Vector &vecEndPoint, C_BasePlayer *pPlayer );

#endif // C_FIZZLER_EFFECTS_H