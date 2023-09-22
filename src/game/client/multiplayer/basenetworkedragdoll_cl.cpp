#include "cbase.h"
#include "basenetworkedragdoll_cl.h"

IMPLEMENT_CLIENTCLASS_DT( C_BaseNetworkedRagdoll, DT_BaseNetworkedRagdoll, CBaseNetworkedRagdoll )
	RecvPropVector( RECVINFO( m_vecForce ) ),
	RecvPropInt ( RECVINFO( m_nForceBone) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( networked_ragdoll, C_BaseNetworkedRagdoll );

C_BaseNetworkedRagdoll::~C_BaseNetworkedRagdoll()
{
	PhysCleanupFrictionSounds( this );
}

void C_BaseNetworkedRagdoll::ImpactTrace( trace_t* pTrace, int iDamageType, char* pCustomImpactName )
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if ( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght
				
		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;  
	
		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );	
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}

void C_BaseNetworkedRagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SelectWeightedSequence( ACT_MP_RUN );
		// m_vecForce is applied succesfully yet the ragdoll doesn't budge.
		// This may be of benefit however, as it means that it is likely to
		// settle where the player was actually killed.
		BecomeRagdollOnClient();
	}
}

void C_BaseNetworkedRagdoll::UpdateOnRemove()
{
	assert(m_pClientsideRagdoll);
	if(m_pClientsideRagdoll)
		UTIL_Remove(m_pClientsideRagdoll);

	BaseClass::UpdateOnRemove();
}