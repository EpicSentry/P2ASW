//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "c_func_portal_bumper.h"

#undef CFuncPortalBumper

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( C_FuncPortalBumper )

	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	
END_DATADESC()

IMPLEMENT_CLIENTCLASS_DT(C_FuncPortalBumper, DT_FuncPortalBumper, CFuncPortalBumper)

	RecvPropBool(RECVINFO(m_bActive)),
	RecvPropInt(RECVINFO(m_spawnflags)),

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( func_portal_bumper, C_FuncPortalBumper );

C_FuncPortalBumper::C_FuncPortalBumper()
{
	m_bActive = true;
}

void C_FuncPortalBumper::Spawn()
{
	BaseClass::Spawn();

	if ( m_spawnflags & SF_START_INACTIVE )
	{
		m_bActive = false;
	}
	else
	{
		m_bActive = true;
	}

	// Bind to our model, cause we need the extents for bounds checking
	SetModel( STRING( GetModelName() ) );
	SetRenderMode( kRenderNone );	// Don't draw
	SetSolid( SOLID_VPHYSICS );	// we may want slanted walls, so we'll use OBB
	AddSolidFlags( FSOLID_NOT_SOLID );
	AddSolidFlags( FSOLID_TRIGGER ); // This is needed to fix the client sided bumping entities check
}
