#ifndef FUNC_PORTAL_BUMPER_H
#define FUNC_PORTAL_BUMPER_H
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// Spawnflags
#define SF_START_INACTIVE			0x01


class CFuncPortalBumper : public CBaseEntity
{
public:
	DECLARE_CLASS( CFuncPortalBumper, CBaseEntity );
	DECLARE_SERVERCLASS();
	CFuncPortalBumper();

	// Overloads from base entity
	virtual void	Spawn( void );

	// Inputs to flip functionality on and off
	void InputActivate( inputdata_t &inputdata );
	void InputDeactivate( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	virtual int UpdateTransmitState(void)
	{
		return SetTransmitState(FL_EDICT_PVSCHECK);
	}

	// misc public methods
	bool IsActive() { return m_bActive; }	// is this area currently bumping portals

	DECLARE_DATADESC();

private:
	CNetworkVar (bool, m_bActive);			// are we currently blocking portals


};

#endif