#include "cbase.h"
#include "baseprojectedentity_shared.h"
#include "baseprojector.h"

BEGIN_DATADESC( CBaseProjector )

	DEFINE_FIELD( m_hFirstChild, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bDisablePlacementHelper, FIELD_BOOLEAN ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_THINKFUNC( TestProjectionChangesThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CBaseProjector, DT_BaseProjector )
	
	SendPropEHandle( SENDINFO( m_hFirstChild ) ),
	SendPropBool( SENDINFO( m_bEnabled ) ),

END_SEND_TABLE()


CUtlVector<CBaseProjector*> s_AllProjectors;

CBaseProjector::CBaseProjector( void )
{
	m_bEnabled = true;
	m_hFirstChild = NULL;
	s_AllProjectors.AddToTail( this );
}

CBaseProjector::~CBaseProjector( void )
{
	Cleanup();
}

void CBaseProjector::Spawn( void )
{
	BaseClass::Spawn();
	SetContextThink( &CBaseProjector::TestProjectionChangesThink, gpGlobals->curtime + 0.05, "TestProjectionChanges" );
}

void CBaseProjector::Activate( void )
{
	BaseClass::Activate();

	if ( m_bEnabled )
		Project();
}

void CBaseProjector::OnRestore( void )
{
	BaseClass::OnRestore();

	if ( m_bEnabled )
		Project();
}

void CBaseProjector::UpdateOnRemove( void )
{
	s_AllProjectors.FindAndRemove( this );
	Cleanup();
	BaseClass::UpdateOnRemove();
}

void CBaseProjector::Cleanup( void )
{
	if ( m_hFirstChild )
	{
		UTIL_Remove( m_hFirstChild.Get() );
		m_hFirstChild = NULL;
	}
}

void CBaseProjector::Shutdown( void )
{

}

int CBaseProjector::DrawDebugTextOverlays( void )
{
	return BaseClass::DrawDebugTextOverlays();
}

void CBaseProjector::EnableProjection( bool bEnable )
{
	m_bEnabled = bEnable;

	if ( m_bEnabled )
	{
		Project();
	}
	else
	{
		Shutdown();
		Cleanup();
	}
}

void CBaseProjector::TestAllForProjectionChanges( void )
{
	for ( int i = 0; i < s_AllProjectors.Count(); ++i )
	{		
		Assert( s_AllProjectors[i] );
		CBaseProjectedEntity *pProjectedEnt = s_AllProjectors[i]->m_hFirstChild;

		if ( !pProjectedEnt )
			continue;
	
		pProjectedEnt->TestForProjectionChanges();	
	}
}

void CBaseProjector::TestProjectionChangesThink( void )
{
	SetContextThink( &CBaseProjector::TestProjectionChangesThink, gpGlobals->curtime + 0.05, "TestProjectionChanges" );
	TestAllForProjectionChanges();
}

void CBaseProjector::Project( void )
{
	bool bCreateNew = false;

	if ( !m_hFirstChild )
	{
		bCreateNew = true;
		m_hFirstChild = CreateNewProjectedEntity();
	}

	Assert( m_hFirstChild );

	m_hFirstChild->RecursiveProjection( bCreateNew, this, NULL, GetAbsOrigin(), GetAbsAngles(), 16, m_bDisablePlacementHelper );
}

void CBaseProjector::SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways )
{
	BaseClass::SetTransmit( pInfo, bAlways );
	
	if ( m_hFirstChild )
		m_hFirstChild->SetTransmit( pInfo, bAlways );
}

void CBaseProjector::InputEnable( inputdata_t &inputdata )
{
	EnableProjection( true );
}

void CBaseProjector::InputDisable( inputdata_t &inputdata )
{
	EnableProjection( false );
}