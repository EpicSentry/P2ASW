//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume in which no portal can be placed. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals wholly or partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "c_func_noportal_volume.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "portal_util_shared.h"
#include "collisionutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Spawnflags
#define SF_START_INACTIVE			0x01

C_EntityClassList<C_FuncNoPortalVolume> g_FuncNoPortalVolumeList;
template <> C_FuncNoPortalVolume *C_EntityClassList<C_FuncNoPortalVolume>::m_pClassList = NULL;

C_FuncNoPortalVolume* GetNoPortalVolumeList()
{
	return g_FuncNoPortalVolumeList.m_pClassList;
}

BEGIN_DATADESC( C_FuncNoPortalVolume )

	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_iListIndex, FIELD_INTEGER ),
	// No need to save this, its rebuilt on construct
	//DEFINE_FIELD( m_pNext, FIELD_CLASSPTR ),
	
END_DATADESC()

#undef CFuncNoPortalVolume

IMPLEMENT_CLIENTCLASS_DT( C_FuncNoPortalVolume, DT_FuncNoPortalVolume, CFuncNoPortalVolume)
	RecvPropInt(RECVINFO(m_iListIndex)),
	RecvPropInt(RECVINFO(m_spawnflags)),
	RecvPropBool(RECVINFO(m_bActive)),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( func_noportal_volume, C_FuncNoPortalVolume );



C_FuncNoPortalVolume::C_FuncNoPortalVolume()
{
	// Add me to the global list
	g_FuncNoPortalVolumeList.Insert( this );
}

C_FuncNoPortalVolume::~C_FuncNoPortalVolume()
{
	g_FuncNoPortalVolumeList.Remove( this );
}


void C_FuncNoPortalVolume::Spawn()
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

void C_FuncNoPortalVolume::OnActivate( void )
{
	// We need to get predicted portalling to work first
#if 1
	if ( !GetCollideable() )
		return;

	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount != 0 )
	{
		CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
		for( int i = 0; i != iPortalCount; ++i )
		{
			CProp_Portal *pTempPortal = pPortals[i];
			if( pTempPortal->IsActive() && 
				IsOBBIntersectingOBB( pTempPortal->GetAbsOrigin(), pTempPortal->GetAbsAngles(), pTempPortal->GetLocalMins(), pTempPortal->GetLocalMaxs(), 
									  GetAbsOrigin(), GetCollideable()->GetCollisionAngles(), GetCollideable()->OBBMins(), GetCollideable()->OBBMaxs() ) )
			{
				pTempPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, false );
				pTempPortal->Fizzle();
			}
		}
	}
#endif
}
