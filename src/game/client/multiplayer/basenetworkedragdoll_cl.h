#include "cbase.h"

class C_BaseNetworkedRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_BaseNetworkedRagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();
	
	C_BaseNetworkedRagdoll() {}
	~C_BaseNetworkedRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	C_BaseEntity* GetOwner() { return m_hPlayer.Get(); }

	void ImpactTrace( trace_t* pTrace, int iDamageType, char* pCustomImpactName );
	void UpdateOnRemove( void );

	bool ShouldDraw() { return false; }
	
private:	
	C_BaseNetworkedRagdoll( const C_BaseNetworkedRagdoll & ) {}

private:
	EHANDLE	m_hPlayer;
};