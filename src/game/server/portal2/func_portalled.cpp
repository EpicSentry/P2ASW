#include "cbase.h"
#include "func_portalled.h"
#include "prop_portal.h"

BEGIN_DATADESC( CFunc_Portalled )

	DEFINE_OUTPUT( m_OnEntityPostPortalled, "OnEntityPostPortalled" ),
	DEFINE_OUTPUT( m_OnEntityPrePortalled, "OnEntityPrePortalled" ),

	DEFINE_KEYFIELD( m_bFireOnDeparture, FIELD_BOOLEAN, "FireOnDeparture" ),
	DEFINE_KEYFIELD( m_bFireOnArrival, FIELD_BOOLEAN, "FireOnArrival" ),
	DEFINE_KEYFIELD( m_bFireOnPlayer, FIELD_BOOLEAN, "FireOnPlayer" ),
	
	DEFINE_OUTPUT( m_OnEntityPrePortalled, "OnEntityPrePortalled" ),
	DEFINE_OUTPUT( m_OnEntityPostPortalled, "OnEntityPostPortalled" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( func_portalled, CFunc_Portalled )

CFunc_Portalled::CFunc_Portalled()
{

}

CFunc_Portalled::~CFunc_Portalled()
{

}

void CFunc_Portalled::OnPrePortalled( CBaseEntity *pOther, bool bDeparting )
{
	bool bFireOutput;

	if ( bDeparting )
		bFireOutput = !m_bFireOnDeparture;
	else
		bFireOutput = !m_bFireOnArrival;

	if ( !bFireOutput && m_bFireOnPlayer && pOther->IsPlayer() )
		m_OnEntityPrePortalled.FireOutput( pOther, this );
}

void CFunc_Portalled::OnPostPortalled( CBaseEntity *pOther, bool bDeparting )
{
	bool bFireOutput;

	if ( bDeparting )
		bFireOutput = !m_bFireOnDeparture;
	else
		bFireOutput = !m_bFireOnArrival;

	if ( !bFireOutput && m_bFireOnPlayer && pOther->IsPlayer() )
		m_OnEntityPostPortalled.FireOutput( pOther, this );
}

void CFunc_Portalled::PortalPlacedInsideBounds( CProp_Portal *pPortal )
{
	pPortal->SetFuncPortalled( this );
}