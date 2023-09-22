//============== Copyright ?Valve Corporation, All rights reserved. =================//
//
// Entities which paint brushes that intersect with specified sphere
//
//=====================================================================================//

#include "cbase.h"

#include "paint_sphere.h"
#ifdef PORTAL2
#include "portal_util_shared.h"
#endif
#include "paint_database.h"
#include "world.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CPaintSphere )
	DEFINE_KEYFIELD( m_nPaintType, FIELD_INTEGER, "paint_type" ),
	DEFINE_KEYFIELD( m_flSphereRadius, FIELD_FLOAT, "radius" ),
	DEFINE_KEYFIELD( m_flAlphaPercent, FIELD_FLOAT, "alpha_percent" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Paint", Paint ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( paint_sphere, CPaintSphere );

// default values must match values in portal2.fgd
CPaintSphere::CPaintSphere() : m_nPaintType( BOUNCE_POWER ), m_flSphereRadius( 60.f ), m_flAlphaPercent( 1.f )
{
}


void CPaintSphere::Paint( inputdata_t &inputdata )
{
	if ( HASPAINTMAP )
	{
		const Vector& vContact = GetAbsOrigin();
		PaintDatabase.AddPaint( GetWorldEntity(), vContact, vec3_origin, m_nPaintType, m_flSphereRadius, m_flAlphaPercent );
	}
}
