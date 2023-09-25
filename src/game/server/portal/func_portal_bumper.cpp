//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "func_portal_bumper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CFuncPortalBumper )

	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Deactivate",  InputDeactivate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle",  InputToggle ),

	DEFINE_FUNCTION( IsActive ),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST(CFuncPortalBumper, DT_FuncPortalBumper)

	SendPropBool(SENDINFO(m_bActive)),
	SendPropInt(SENDINFO(m_spawnflags)),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(func_portal_bumper, CFuncPortalBumper);


CFuncPortalBumper::CFuncPortalBumper()
{
	m_bActive = true;
}

void CFuncPortalBumper::Spawn()
{
	SetTransmitState(FL_EDICT_PVSCHECK);
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

void CFuncPortalBumper::InputActivate( inputdata_t &inputdata )
{
	m_bActive = true;
}

void CFuncPortalBumper::InputDeactivate( inputdata_t &inputdata )
{
	m_bActive = false;
}

void CFuncPortalBumper::InputToggle( inputdata_t &inputdata )
{
	m_bActive = !m_bActive;
}

