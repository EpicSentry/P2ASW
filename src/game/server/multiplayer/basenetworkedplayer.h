#include "cbase.h"
#include "multiplayer/multiplayer_animstate.h"
#include "multiplayer/basenetworkedplayer_shared.h"
#include "basemultiplayerplayer.h"

#define BASENETWORKEDPLAYER_H

class CBaseNetworkedPlayer : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS( CBaseNetworkedPlayer, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();

	CBaseNetworkedPlayer();
	~CBaseNetworkedPlayer();

	virtual void Spawn();
	virtual void UpdateOnRemove();
	
	virtual void BecomeRagdollOnClient();
	virtual void RemoveRagdollEntity();
	virtual void Event_Killed( const CTakeDamageInfo &info );
		
	// Create a predicted viewmodel
	virtual void CreateViewModel( int index );

	// Lag compensate when firing bullets
	virtual void FireBullets ( const FireBulletsInfo_t &info );

	// Implement CMultiPlayerAnimState
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0);
	void SetAnimation( PLAYER_ANIM playerAnim ) {} // Deprecated; would screw up ambient animation if not overridden
	virtual void PostThink();

protected:
	void MakeAnimState();
	CMultiPlayerAnimState* m_PlayerAnimState;
	const char* ragdoll_ent_name;

	CNetworkQAngle( m_angEyeAngles );
	CNetworkVar( bool, m_bSpawnInterpCounter );
	CNetworkHandle( CBaseEntity, m_hRagdoll );
};

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();
	CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name ) {}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	//CNetworkVar( int, m_nData );
};