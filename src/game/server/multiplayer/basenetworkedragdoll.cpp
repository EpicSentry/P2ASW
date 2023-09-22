#include "cbase.h"
#include "basenetworkedragdoll.h"

LINK_ENTITY_TO_CLASS( networked_ragdoll, CBaseNetworkedRagdoll );

IMPLEMENT_SERVERCLASS_ST( CBaseNetworkedRagdoll, DT_BaseNetworkedRagdoll )
	SendPropVector( SENDINFO( m_vecForce ) ),
	SendPropInt ( SENDINFO( m_nForceBone) ),
END_SEND_TABLE()

void CBaseNetworkedRagdoll::Spawn()
{
	BaseClass::Spawn();

	// Don't broadcast to players who see the death; the player ent handles things for them
	CPlayerBitVec PvsPlayers;
	engine->Message_DetermineMulticastRecipients( false, GetAbsOrigin(), PvsPlayers );
	
	for ( int i=0; i < PvsPlayers.GetNumBits(); i++ )
		if (PvsPlayers.IsBitSet(i))
			SuppressedClients.AddToTail( dynamic_cast<CBasePlayer*>(INDEXENT(i+1)->GetUnknown()) ); // +1 for player index to ent index
}

int CBaseNetworkedRagdoll::ShouldTransmit(const CCheckTransmitInfo* pInfo)
{
	for (int i=0; i < SuppressedClients.Count(); i++)
		if ( SuppressedClients[i] == pInfo->m_pClientEnt->GetUnknown() ) // Can't use Find() on pointers
			return FL_EDICT_DONTSEND;
	
	return FL_EDICT_ALWAYS;
}