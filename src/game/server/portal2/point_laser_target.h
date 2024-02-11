//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef POINT_LASER_TARGET_H
#define POINT_LASER_TARGET_H
#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

enum TargetPowerState_e
{
    UNPOWERED,
    POWERING_DOWN,
    POWERING_UP,
    POWERED,
};

//-----------------------------------------------------------------------------
// Purpose: Interface for a catcher...
//-----------------------------------------------------------------------------
abstract_class ICatcher
{
public:
    virtual void OnPowered() = 0;
    virtual void OnUnPowered() = 0;
};

// Purpose: Base catcher class implementation
class CCatcher : public ICatcher
{
public:
    virtual void OnPowered() { ; };     // Does absolutely nothing according to decompiler
    virtual void OnUnPowered() { ; };   // Does absolutely nothing according to decompiler
};

//-----------------------------------------------------------------------------
// Purpose: (point_laser_target)
//          Helper entity for the below classes
//-----------------------------------------------------------------------------
class CPortalLaserTarget : public CBaseEntity
{
public:
    DECLARE_CLASS( CPortalLaserTarget, CBaseEntity );

    virtual void Spawn();

    virtual int OnTakeDamage( const CTakeDamageInfo &info );

    bool IsTerminalPoint();					// TODO: Virtual?
    void SetCatcher( CCatcher *pCatcher );  // TODO: Virtual?
    bool IsPowered();						// TODO: Virtual?

protected:
    void DisableThink();

    COutputEvent m_OnPowered;
    COutputEvent m_OnUnpowered;

    bool m_bPowered;
    bool m_bTerminalPoint;

    CCatcher *m_pCatcher;

    string_t m_ModelName;

    DECLARE_DATADESC();
};

//-----------------------------------------------------------------------------
// Purpose: (prop_laser_catcher)
//          Implements the catcher prop as an entity
//-----------------------------------------------------------------------------
class CLaserCatcher : public CBaseAnimating, public CCatcher
{
public:
    DECLARE_CLASS( CLaserCatcher, CBaseAnimating );
    DECLARE_DATADESC();

    CLaserCatcher(); // TODO: Virtual?

    virtual void Precache();
    virtual void Spawn();
    virtual bool CreateVPhysics();
    virtual void Activate();
    virtual void AnimateThink();
    virtual void UpdateOnRemove();

    void OnPowered();
    void OnUnPowered();

    virtual bool PoweringDown();
    virtual bool PoweringUp();

    virtual void CreateHelperEntities();
    virtual const char *GetCatcherModelName();

    virtual bool IsTerminalPoint(); // TODO: Virtual?

    virtual float SpinUp( float flSpinRate );
    virtual float SpinDown( float flSpinRate );

    virtual void OnRestore();

protected:
    CPortalLaserTarget *m_pCatcherLaserTarget;

    COutputEvent m_OnPowered;
    COutputEvent m_OnUnpowered;

    int m_iPowerState;

private:
    int m_IdleSequence;
    int m_PowerOnSequence;
    int m_iTargetAttachment;
    int m_iOffSkin;
    int m_iOnSkin;

    bool m_bRusted;
};

//-----------------------------------------------------------------------------
// Purpose: (prop_laser_relay)
//          Same thing as a CLaserCatcher, just a different model and no end point
//-----------------------------------------------------------------------------
class CLaserRelay : public CLaserCatcher
{
    DECLARE_CLASS( CLaserRelay, CLaserCatcher );
    DECLARE_DATADESC();

    virtual const char *GetCatcherModelName();

    virtual bool IsTerminalPoint();
};

#endif // POINT_LASER_TARGET_H