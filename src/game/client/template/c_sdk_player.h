#ifndef C_SDK_PLAYER_H
#define C_SDK_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseplayer.h"
#include "baseparticleentity.h"
#include "steam/steam_api.h"
#include <vgui_controls/PHandle.h>
#include "sdk_playeranimstate.h"
#include "weapon_sdkbase.h"

namespace vgui
{
	class Frame;
};

class C_SDK_Player : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_SDK_Player, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_SDK_Player();
	virtual ~C_SDK_Player();

	static C_SDK_Player* GetLocalSDKPlayer( int nSlot = -1 );

	virtual bool ShouldRegenerateOriginFromCellBits() const;

	virtual const QAngle& EyeAngles( void );
	virtual const QAngle& GetRenderAngles();
	virtual void UpdateClientSideAnimation();
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

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

// Called by shared code.
public:
	void DoAnimationEvent( PlayerAnimEvent_t event, bool bPredicted = false );

	CWeaponSDKBase *GetActiveSDKWeapon() const;

	CSDKPlayerAnimState *m_PlayerAnimState;

public:
	QAngle	m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

private:
	C_SDK_Player( const C_SDK_Player & );
};

inline C_SDK_Player* ToSDK_Player( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return NULL;
	Assert( dynamic_cast< C_SDK_Player* >( pEntity ) != NULL );
	return static_cast< C_SDK_Player* >( pEntity );
}


#endif // C_SDK_PLAYER_H
