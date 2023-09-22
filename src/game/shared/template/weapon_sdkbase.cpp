//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "weapon_sdkbase.h"
#include "ammodef.h"
#include "datacache/imdlcache.h"

#if defined( CLIENT_DLL )

	#include "c_sdk_player.h"

#else

	#include "sdk_player.h"

#endif

#include "sdk_fx_shared.h"

// ----------------------------------------------------------------------------- //
// CWeaponSDKBase tables.
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSDKBase, DT_WeaponSDKBase )

BEGIN_NETWORK_TABLE( CWeaponSDKBase, DT_WeaponSDKBase )
#ifdef CLIENT_DLL
#else
	SendPropExclude( "DT_BaseAnimating", "m_nNewSequenceParity" ),
	SendPropExclude( "DT_BaseAnimating", "m_nResetEventsParity" ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponSDKBase )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_sdk_base, CWeaponSDKBase );


#ifdef GAME_DLL

	BEGIN_DATADESC( CWeaponSDKBase )

		// New weapon Think and Touch Functions go here..

	END_DATADESC()

#endif

#ifdef CLIENT_DLL
bool CWeaponSDKBase::ShouldPredict()
{
       if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer())
               return true;

       return BaseClass::ShouldPredict();
}
#endif
// ----------------------------------------------------------------------------- //
// CWeaponCSBase implementation. 
// ----------------------------------------------------------------------------- //
CWeaponSDKBase::CWeaponSDKBase()
{
	SetPredictionEligible( true );

	AddSolidFlags( FSOLID_TRIGGER ); // Nothing collides with these but it gets touches.
}

const CSDK_WeaponInfo &CWeaponSDKBase::GetSDKWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CSDK_WeaponInfo *pSDKInfo;

	#ifdef _DEBUG
		pSDKInfo = dynamic_cast< const CSDK_WeaponInfo* >( pWeaponInfo );
		Assert( pSDKInfo );
	#else
		pSDKInfo = static_cast< const CSDK_WeaponInfo* >( pWeaponInfo );
	#endif

	return *pSDKInfo;
}

CSDK_Player* CWeaponSDKBase::GetPlayerOwner() const
{
	return dynamic_cast< CSDK_Player* >( GetOwner() );
}

void CWeaponSDKBase::PrimaryAttack( void )
{
	// If my clip is empty (and I use clips) start reload
	if ( UsesClipsForAmmo1() && !m_iClip1 ) 
	{
		Reload();
		return;
	}

	CSDK_Player *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	pPlayer->DoMuzzleFlash();

	SendWeaponAnim( GetPrimaryAttackActivity() );

	// Make sure we don't fire more than the amount in the clip
	if ( UsesClipsForAmmo1() )
		m_iClip1 --;
	else
		pPlayer->RemoveAmmo(1, m_iPrimaryAmmoType );

	//float flSpread = GetWeaponSpread();

	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY, true );

	// NOTE: FX_FireBullets is tied into the animation event (saves sending some events due the effect dispatching)
	/*FX_FireBullets(
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + pPlayer->GetPunchAngle(),
		&GetSDKWpnData(),
		0, //Tony; fire mode - this is unused at the moment, left over from CSS when SDK* was created in the first place.
		CBaseEntity::GetPredictionRandomSeed() & 255,
		flSpread
		);
		*/

	//Add our view kick in
	AddViewKick();

	//Tony; update our weapon idle time
	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
}

void CWeaponSDKBase::SecondaryAttack()
{
}

bool CWeaponSDKBase::Reload( void )
{
	bool fRet;
	float fCacheTime = m_flNextSecondaryAttack;

	fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), GetReloadActivity() );
	if ( fRet )
	{

		SendReloadEvents();

		// Undo whatever the reload process has done to our secondary
		// attack timer. We allow you to interrupt reloading to fire
		// a grenade.
		m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;
		
		WeaponSound( RELOAD );
	}

	return fRet;
}
void CWeaponSDKBase::SendReloadEvents()
{
	CSDK_Player *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

#if 0
#ifdef GAME_DLL
	// Send a message to any clients that have this entity to play the reload.
	CPASFilter filter( pPlayer->GetAbsOrigin() );
	filter.RemoveRecipient( pPlayer );

	UserMessageBegin( filter, "ReloadEffect" );
	WRITE_SHORT( pPlayer->entindex() );
	MessageEnd();
#endif
#endif // 0

	// Make the player play his reload animation.
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
}