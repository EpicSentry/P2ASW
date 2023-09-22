#include "cbase.h"

class CBaseNetworkedRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CBaseNetworkedRagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	virtual void Spawn();

	// players who saw the death don't need this entity
	virtual int UpdateTransmitState() { return SetTransmitState( FL_EDICT_FULLCHECK ); }
	virtual int ShouldTransmit(const CCheckTransmitInfo* pInfo);
	CUtlVector<CBasePlayer*> SuppressedClients;
	
	CNetworkHandle( CBaseEntity, m_hPlayer );
};