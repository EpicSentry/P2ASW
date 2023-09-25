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

#define CFuncPortalBumper C_FuncPortalBumper

// Spawnflags
#define SF_START_INACTIVE			0x01


class C_FuncPortalBumper : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_FuncPortalBumper, C_BaseEntity );
	DECLARE_CLIENTCLASS();
	C_FuncPortalBumper();

	// Overloads from base entity
	virtual void	Spawn( void );

	// misc public methods
	bool IsActive() { return m_bActive; }	// is this area currently bumping portals

	DECLARE_DATADESC();

private:
	bool					m_bActive;			// are we currently blocking portals
};
#endif