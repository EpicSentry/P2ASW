#ifndef SDK_PLAYER_H
#define SDK_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "player.h"
#include "server_class.h"
#include "basemultiplayerplayer.h"
#include "sdk_playeranimstate.h"

class CWeaponSDKBase;

//=============================================================================
// >> Swarm player
//=============================================================================
class CSDK_Player : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS( CSDK_Player, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CSDK_Player();
	virtual ~CSDK_Player();

	virtual int UpdateTransmitState();

	static CSDK_Player *CreatePlayer( const char *className, edict_t *ed );
	static CSDK_Player* Instance( int iEnt );

	virtual void Precache();
	virtual void Spawn();

	// This passes the event to the client's and server's CPlayerAnimState.
	void DoAnimationEvent( PlayerAnimEvent_t event, bool bPredicted = false );

	virtual void GiveDefaultItems();

	virtual void PostThink();

	// Animstate handles this.
	void SetAnimation( PLAYER_ANIM playerAnim ) { return; }

	CWeaponSDKBase *GetActiveSDKWeapon() const;
	virtual void	CreateViewModel( int viewmodelindex = 0 );

// In shared code.
public:
	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y );

public:
	CNetworkQAngle( m_angEyeAngles );	// Copied from EyeAngles() so we can send it to the client.

private:
	CSDKPlayerAnimState *m_PlayerAnimState;
};

inline CSDK_Player *ToSDK_Player( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CSDK_Player*>( pEntity ) != 0 );
#endif
	return static_cast< CSDK_Player* >( pEntity );
}


#endif	// SDK_PLAYER_H
