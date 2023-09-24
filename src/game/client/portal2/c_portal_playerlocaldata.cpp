#include "cbase.h"
#include "c_portal_playerlocaldata.h"

Vector DEFAULT_SURFACE_NORMAL( 0.0, 0.0, 1.0 ); // Used only in the constructor below

C_PortalPlayerLocalData::C_PortalPlayerLocalData()
{
	m_StickNormal = DEFAULT_SURFACE_NORMAL;
	m_Up = DEFAULT_SURFACE_NORMAL;
}