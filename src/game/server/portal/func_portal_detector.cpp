//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume in which no portal can be placed. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals wholly or partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "func_portal_detector.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "portal_util_shared.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CEntityClassList<CFuncPortalDetector> g_FuncPortalDetectorList;
template <> CFuncPortalDetector *CEntityClassList<CFuncPortalDetector>::m_pClassList = NULL;

CFuncPortalDetector* GetPortalDetectorList()
{
	return g_FuncPortalDetectorList.m_pClassList;
}

// Spawnflags
#define SF_START_INACTIVE			0x01


LINK_ENTITY_TO_CLASS( func_portal_detector, CFuncPortalDetector );

BEGIN_DATADESC( CFuncPortalDetector )

	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	DEFINE_KEYFIELD( m_iLinkageGroupID, FIELD_INTEGER, "LinkageGroupID" ),
	DEFINE_KEYFIELD( m_bCheckAllIDs, FIELD_BOOLEAN, "CheckAllIDs" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

	DEFINE_OUTPUT( m_OnStartTouchPortal, "OnStartTouchPortal" ),
	DEFINE_OUTPUT( m_OnStartTouchPortal1, "OnStartTouchPortal1" ),
	DEFINE_OUTPUT( m_OnStartTouchPortal2, "OnStartTouchPortal2" ),
	DEFINE_OUTPUT( m_OnStartTouchLinkedPortal, "OnStartTouchLinkedPortal" ),
	DEFINE_OUTPUT( m_OnStartTouchBothLinkedPortals, "OnStartTouchBothLinkedPortals" ),
	DEFINE_OUTPUT( m_OnEndTouchPortal, "OnEndTouchPortal" ),
	DEFINE_OUTPUT( m_OnEndTouchPortal1, "OnEndTouchPortal1" ),
	DEFINE_OUTPUT( m_OnEndTouchPortal2, "OnEndTouchPortal2" ),
	DEFINE_OUTPUT( m_OnEndTouchLinkedPortal, "OnEndTouchLinkedPortal" ),
	DEFINE_OUTPUT( m_OnEndTouchBothLinkedPortals, "OnEndTouchBothLinkedPortals" ),

	DEFINE_FUNCTION( IsActive ),

END_DATADESC()


CFuncPortalDetector::CFuncPortalDetector()
{
	m_bActive = false;
	m_iLinkageGroupID = 0;
	m_phTouchingPortals[0] = NULL;
	m_phTouchingPortals[1] = NULL;
	m_iTouchingPortalCount = 0;
	m_bCheckAllIDs = false;
	g_FuncPortalDetectorList.Insert( this );
}

CFuncPortalDetector::~CFuncPortalDetector()
{
	g_FuncPortalDetectorList.Remove( this );
}

void CFuncPortalDetector::Spawn()
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
	SetSolid( SOLID_VPHYSICS );		// we may want slanted walls, so we'll use OBB
	AddSolidFlags( FSOLID_NOT_SOLID );
}

void CFuncPortalDetector::SetActive( bool bActive )
{
	m_bActive = bActive;

	Vector vMin, vMax;
	CollisionProp()->WorldSpaceAABB( &vMin, &vMax );

	Vector vBoxCenter = ( vMin + vMax ) * 0.5f;
	Vector vBoxExtents = ( vMax - vMin ) * 0.5f;

	bool bTouchedPortal1 = false;
	bool bTouchedPortal2 = false;

	if (bActive)
	{
		m_OnStartTouchPortal = m_OnStartTouchPortal;
		m_OnStartTouchPortal2 = m_OnStartTouchPortal2;
		m_OnStartTouchPortal1 = m_OnStartTouchPortal1;
		m_OnStartTouchLinkedPortal = m_OnStartTouchLinkedPortal;
		m_OnStartTouchBothLinkedPortals = m_OnStartTouchBothLinkedPortals;
	}
	else
	{
		m_OnStartTouchPortal = m_OnEndTouchPortal;
		m_OnStartTouchPortal2 = m_OnEndTouchPortal2;
		m_OnStartTouchPortal1 = m_OnEndTouchPortal1;
		m_OnStartTouchLinkedPortal = m_OnEndTouchLinkedPortal;
		m_OnStartTouchBothLinkedPortals = m_OnEndTouchBothLinkedPortals;
	}

	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount != 0 )
	{
		CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
		for( int i = 0; i != iPortalCount; ++i )
		{
			CProp_Portal *pTempPortal = pPortals[i];

			//require that it's active and/or linked?

			if( pTempPortal->GetLinkageGroup() == m_iLinkageGroupID && UTIL_IsBoxIntersectingPortal( vBoxCenter, vBoxExtents, pTempPortal ) )
			{
				m_OnStartTouchPortal.FireOutput(pTempPortal, this);
				if( pTempPortal->IsPortal2() )
				{
					m_OnStartTouchPortal2.FireOutput( pTempPortal, this );

					if ( pTempPortal->IsActivedAndLinked() )
					{
						bTouchedPortal2 = true;
						m_OnStartTouchLinkedPortal.FireOutput( pTempPortal, this );
					}
				}
				else
				{
					m_OnStartTouchPortal1.FireOutput( pTempPortal, this );

					if ( pTempPortal->IsActivedAndLinked() )
					{
						bTouchedPortal1 = true;
						m_OnStartTouchLinkedPortal.FireOutput( pTempPortal, this );
					}
				}
			}
		}
	}

	if ( bTouchedPortal1 && bTouchedPortal2 )
	{
		m_OnStartTouchBothLinkedPortals.FireOutput( this, this );
	}
}

void CFuncPortalDetector::InputDisable( inputdata_t &inputdata )
{
	SetActive(false);
}

void CFuncPortalDetector::InputEnable( inputdata_t &inputdata )
{
	SetActive(true);
}

void CFuncPortalDetector::InputToggle( inputdata_t &inputdata )
{
	m_bActive = !m_bActive;

	SetActive(m_bActive);
}


void CFuncPortalDetector::NotifyPortalEvent( PortalEvent_t nEventType, CPortal_Base2D *pNotifier )
{
	if (nEventType == PORTALEVENT_FIZZLE)
	{
		CProp_Portal *pPropPortal = dynamic_cast<CProp_Portal*>( pNotifier );

		UpdateOnPortalMoved( pPropPortal );
	}
}

void CFuncPortalDetector::UpdateOnPortalMoved( CProp_Portal *pPortal )
{
	if ( m_bActive )
	{
		m_iLinkageGroupID = pPortal->GetLinkageGroup();

		bool bWasTouchingPortalDetector = IsPortalTouchingDetector( pPortal );

		Vector vMax, vMin;
		Vector vBoxCenter;
		Vector vBoxExtents;

		if ( GetLinkageGroupID() != pPortal->GetLinkageGroup() && !m_bCheckAllIDs )
			goto SKIP_BOX_CHECK;

		CollisionProp()->WorldSpaceAABB( &vMin, &vMax );

		vBoxCenter = ( vMax + vMin ) * 0.5;
		vBoxExtents = ( vMax - vMin ) * 0.5;
		
		bool bIsTouchingPortalDetector = true;

		if ( !UTIL_IsBoxIntersectingPortal( vBoxCenter, vBoxExtents, pPortal ) )
		{
			SKIP_BOX_CHECK:
			bIsTouchingPortalDetector = false;
		}

		if ( ( bWasTouchingPortalDetector && !pPortal->IsActive() ) 
			|| ( bWasTouchingPortalDetector && !bIsTouchingPortalDetector ) )
		{
			m_phTouchingPortals[pPortal->m_bIsPortal2] = NULL;
			
			--m_iTouchingPortalCount;
			PortalRemovedFromInsideBounds( pPortal );
		}

		if ( bIsTouchingPortalDetector && !bWasTouchingPortalDetector )
		{
			m_phTouchingPortals[pPortal->m_bIsPortal2] = pPortal;
			++m_iTouchingPortalCount;
			PortalPlacedInsideBounds( pPortal );
		}
	}
}

void CFuncPortalDetector::PortalPlacedInsideBounds( CProp_Portal *pPortal )
{
	m_OnStartTouchPortal.FireOutput( pPortal, this );
	if ( pPortal->m_bIsPortal2 )
	{
		m_OnStartTouchPortal2.FireOutput( pPortal, this );
		if ( !pPortal->IsActivedAndLinked() )
			goto ADD_LISTENER;
	}
	else
	{
		m_OnStartTouchPortal1.FireOutput( pPortal, this );
		if ( !pPortal->IsActivedAndLinked() )
			goto ADD_LISTENER;
	}
	m_OnStartTouchLinkedPortal.FireOutput( pPortal, this );
	if ( m_iTouchingPortalCount == 2 )
		m_OnStartTouchBothLinkedPortals.FireOutput( pPortal, this );
ADD_LISTENER:
	pPortal->AddPortalEventListener( this );
}

bool CFuncPortalDetector::IsPortalTouchingDetector( const CProp_Portal *pPortal )
{
	if ( !pPortal )
		return false;

	for ( int i = 0; i < 2; i++ )
	{
		if ( dynamic_cast<CProp_Portal*>( m_phTouchingPortals[i].Get() ) == pPortal )
			return true;
	}

	return false;
}

void CFuncPortalDetector::UpdateOnPortalActivated( CProp_Portal *pPortal )
{
	if ( IsPortalTouchingDetector( pPortal ) )
	{
		m_OnStartTouchLinkedPortal.FireOutput( pPortal, this );
		if ( m_iTouchingPortalCount == 2 )
			m_OnStartTouchBothLinkedPortals.FireOutput( pPortal, this );
	}
}

void CFuncPortalDetector::PortalRemovedFromInsideBounds( CProp_Portal *pPortal )
{
	EHANDLE p_hListener; // [esp+1Ch] [ebp-Ch] BYREF

	m_OnEndTouchPortal.FireOutput( pPortal, this );
	if ( pPortal->m_bIsPortal2 )
	{
		m_OnEndTouchPortal2.FireOutput( pPortal, this );
		if ( !pPortal->IsActivedAndLinked() )
			goto REMOVE_LISTENER;
	}
	else
	{
		m_OnEndTouchPortal1.FireOutput( pPortal, this );
		if ( !pPortal->IsActivedAndLinked() )
			goto REMOVE_LISTENER;
	}
	m_OnEndTouchLinkedPortal.FireOutput( pPortal, this );
	if ( !m_iTouchingPortalCount )
		m_OnEndTouchBothLinkedPortals.FireOutput( pPortal, this );
REMOVE_LISTENER:
	pPortal->RemovePortalEventListener( this );
}