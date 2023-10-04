#include "cbase.h"
#include "c_baseprojectedentity.h"
#include "prop_portal_shared.h"

#undef CBaseProjectedEntity

IMPLEMENT_CLIENTCLASS_DT( C_BaseProjectedEntity, DT_BaseProjectedEntity, CBaseProjectedEntity )

	RecvPropEHandle( RECVINFO( m_hHitPortal ) ),
	RecvPropEHandle( RECVINFO( m_hSourcePortal ) ),
	RecvPropEHandle( RECVINFO( m_hChildSegment ) ),
	RecvPropEHandle( RECVINFO( m_hPlacementHelper ) ),

	RecvPropVector( RECVINFO( m_vecSourcePortalCenter ) ),
	RecvPropVector( RECVINFO( m_vecSourcePortalRemoteCenter ) ),
	RecvPropQAngles( RECVINFO( m_vecSourcePortalAngle ) ),
	RecvPropQAngles( RECVINFO( m_vecSourcePortalRemoteAngle ) ),
	
	RecvPropVector( RECVINFO( m_vecStartPoint ) ),
	RecvPropVector( RECVINFO( m_vecEndPoint ) ),
	
	RecvPropBool( RECVINFO( m_bCreatePlacementHelper ) ),
	RecvPropInt( RECVINFO( m_iMaxRemainingRecursions ) ),

END_RECV_TABLE()


BEGIN_PREDICTION_DATA( C_BaseProjectedEntity )

	//DEFINE_PRED_FIELD( m_hHitPortal, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE ),
	//DEFINE_PRED_FIELD( m_hSourcePortal, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE ),
	
	DEFINE_PRED_FIELD( m_vecSourcePortalCenter, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vecSourcePortalRemoteCenter, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vecSourcePortalAngle, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vecSourcePortalRemoteAngle, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	
	DEFINE_PRED_FIELD( m_vecStartPoint, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vecEndPoint, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),

	DEFINE_PRED_FIELD( m_iMaxRemainingRecursions, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),

END_PREDICTION_DATA()

CUtlVector<C_BaseProjectedEntity*> s_AllProjectedEntities;

C_BaseProjectedEntity::C_BaseProjectedEntity()
{
	s_AllProjectedEntities.AddToTail(this);
}

C_BaseProjectedEntity::~C_BaseProjectedEntity()
{
	s_AllProjectedEntities.FindAndRemove(this);
}

void C_BaseProjectedEntity::TestAllForProjectionChanges( void )
{
	for ( int i = 0; i < s_AllProjectedEntities.Count(); ++i)
		s_AllProjectedEntities[i]->TestForProjectionChanges();
}

// FIXME: Some of this may be inaccurate
void C_BaseProjectedEntity::OnPreDataChanged( DataUpdateType_t updateType )
{
	PreDataChanged.vStartPoint = m_vecStartPoint;
	PreDataChanged.vEndPoint = m_vecEndPoint;
	PreDataChanged.qAngles = GetNetworkAngles();
}

// FIXME: Some of this may be inaccurate
void C_BaseProjectedEntity::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	HandleDataChange();
	
	if ( !m_hSourcePortal || m_hSourcePortal->GetSimulatingPlayer() != C_BasePlayer::GetLocalPlayer() )
	{
		SetPredictionEligible( false );
		UnsetPlayerSimulated();
		return;
	}
	
	SetPredictionEligible( true );


	if ( m_hSourcePortal && m_hSourcePortal->GetSimulatingPlayer() )
		SetPlayerSimulated( m_hSourcePortal->GetSimulatingPlayer() );
}

void C_BaseProjectedEntity::HandleDataChange( void )
{
	if ( PreDataChanged.vStartPoint != m_vecStartPoint ||
		PreDataChanged.vEndPoint != m_vecEndPoint ||
		PreDataChanged.qAngles != GetNetworkAngles() )
	{
		OnProjected();
	}
}

void C_BaseProjectedEntity::HandlePredictionError( bool bErrorInThisEntity )
{
	BaseClass::HandlePredictionError( bErrorInThisEntity );
	if ( bErrorInThisEntity )
		HandleDataChange();
}

// NOTE: This function was completely rewritten by me
// The decompiled code is just outright unreadable, and I've covered most valid prediction cases here.

C_BasePlayer *C_BaseProjectedEntity::GetPredictionOwner( void )
{
	if ( !m_hSourcePortal )
		return NULL;
	
	
	C_Prop_Portal *pPropPortal = dynamic_cast<C_Prop_Portal*>( m_hSourcePortal.Get() );
	
	// FiredByPlayer is the most important!

	if ( pPropPortal && pPropPortal->GetFiredByPlayer() == CBasePlayer::GetLocalPlayer() )
		return pPropPortal->GetFiredByPlayer();
	
	if ( m_hSourcePortal->GetPredictionOwner() == CBasePlayer::GetLocalPlayer() )
		return m_hSourcePortal->GetPredictionOwner();
	
	if ( m_hSourcePortal->GetSimulatingPlayer() == CBasePlayer::GetLocalPlayer() )
		return m_hSourcePortal->GetSimulatingPlayer();	

	return NULL;
}

// NOTE: Once again, the decompiled code is unreadable, rewriting the function...
bool C_BaseProjectedEntity::ShouldPredict( void )
{	
	if ( GetPredictionOwner() && GetPredictionOwner()->IsLocalPlayer() )
		return true;

	return false;
}

Vector C_BaseProjectedEntity::GetLengthVector( void )
{
	return m_vecEndPoint - m_vecStartPoint;
}