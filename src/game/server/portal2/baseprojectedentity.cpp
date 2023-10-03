#include "cbase.h"
#include "info_placement_helper.h"
#include "baseprojectedentity_shared.h"
#include "prop_weightedcube.h"

BEGIN_DATADESC( CBaseProjectedEntity )
END_DATADESC()


IMPLEMENT_SERVERCLASS_ST( CBaseProjectedEntity, DT_BaseProjectedEntity )

	SendPropEHandle( SENDINFO( m_hHitPortal ) ),
	SendPropEHandle( SENDINFO( m_hSourcePortal ) ),
	SendPropEHandle( SENDINFO( m_hChildSegment ) ),
	SendPropEHandle( SENDINFO( m_hPlacementHelper ) ),

	SendPropVector( SENDINFO( m_vecSourcePortalCenter ) ),
	SendPropVector( SENDINFO( m_vecSourcePortalRemoteCenter ) ),
	SendPropQAngles( SENDINFO( m_vecSourcePortalAngle ) ),
	SendPropQAngles( SENDINFO( m_vecSourcePortalRemoteAngle ) ),
	
	SendPropVector( SENDINFO( m_vecStartPoint ) ),
	SendPropVector( SENDINFO( m_vecEndPoint ) ),
	
	SendPropBool( SENDINFO( m_bCreatePlacementHelper ) ),
	SendPropInt( SENDINFO( m_iMaxRemainingRecursions ) ),


END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( projected_entity, CBaseProjectedEntity )

CBaseProjectedEntity::CBaseProjectedEntity( void )
{

}

CBaseProjectedEntity::~CBaseProjectedEntity( void )
{

}

void CBaseProjectedEntity::Spawn( void )
{
	BaseClass::Spawn();
	FindProjectedEndpoints();
}

void CBaseProjectedEntity::OnRestore( void )
{
	BaseClass::OnRestore();
	if ( m_bCreatePlacementHelper )
	{
		bool bCreateHelper = false;

		if ( !m_hPlacementHelper )
		{
			bCreateHelper = true;
			m_hPlacementHelper = (CInfoPlacementHelper*)CreateEntityByName( "info_placement_helper" );
		}

		PlacePlacementHelper( m_hPlacementHelper );
		
		if ( bCreateHelper )
		{
			DispatchSpawn( m_hPlacementHelper );
		}
	}
}

void CBaseProjectedEntity::UpdateOnRemove( void )
{
	if ( m_hChildSegment )
		UTIL_Remove( m_hChildSegment );
	
	if ( m_hPlacementHelper )
		UTIL_Remove( m_hPlacementHelper );

	BaseClass::UpdateOnRemove();
}

void CBaseProjectedEntity::PlacePlacementHelper( CInfoPlacementHelper *pHelper )
{
	if (pHelper)
	{
		Vector vForward;
		GetVectors( &vForward, NULL, NULL );
		pHelper->SetAbsOrigin( GetEndPoint() = vForward );
		pHelper->KeyValue( "radius", "52" );
		pHelper->KeyValue( "force_placement", "1" );
		pHelper->SetOwnerEntity( this );
	}
}

void CBaseProjectedEntity::OnPreProjected( void )
{
	// FixMe?
}

void CBaseProjectedEntity::SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways )
{
	BaseClass::SetTransmit( pInfo, bAlways );
	if ( m_hChildSegment )
		m_hChildSegment->SetTransmit( pInfo, bAlways );
}

void CBaseProjectedEntity::CheckForSettledReflectorCubes( void )
{
	if (vec3_origin == m_vecStartPoint || vec3_origin == m_vecEndPoint )
		return;
	
	for ( int i = 0; i < IPropWeightedCubeAutoList::AutoList().Count(); ++i)
	{
		CPropWeightedCube *pCube = assert_cast<CPropWeightedCube*>( IPropWeightedCubeAutoList::AutoList()[i]->GetEntity() );
		if ( !pCube )
			continue;

		if ( !pCube->IsMovementDisabled() )
			continue;

		pCube->ExitDisabledState();
	}	
}

CBaseProjectedEntity *CBaseProjectedEntity::CreateNewProjectedEntity(void)
{
	return (CBaseProjectedEntity*)CreateEntityByName("projected_entity");
}

