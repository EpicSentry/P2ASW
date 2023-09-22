//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_SDKBASE_H
#define WEAPON_SDKBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "sdk_playeranimstate.h"
#include "sdk_weapon_parse.h"
#include "sdk_shareddefs.h"

#if defined( CLIENT_DLL )
	#define CWeaponSDKBase C_WeaponSDKBase
#endif

class CSDK_Player;

// These are the names of the ammo types that the weapon script files reference.
class CWeaponSDKBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponSDKBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponSDKBase();

	#ifdef GAME_DLL
		DECLARE_DATADESC();
	#endif
	#ifdef CLIENT_DLL
       virtual bool ShouldPredict();
	#endif
	// All predicted weapons need to implement and return true
	virtual bool	IsPredicted() const { return true; }

	// Get SDK weapon specific weapon data.
	CSDK_WeaponInfo const	&GetSDKWpnData() const;

	// Get a pointer to the player that owns this weapon
	CSDK_Player* GetPlayerOwner() const;

	// Default weapon spread, pretty accurate - accuracy systems would need to modify this
	virtual float GetWeaponSpread() { return 0.01f; }

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	//Tony; these five functions return the sequences the view model uses for a particular action. -- You can override any of these in a particular weapon if you want them to do
	//something different, ie: when a pistol is out of ammo, it would show a different sequence.
	virtual Activity	GetPrimaryAttackActivity( void )	{	return	ACT_VM_PRIMARYATTACK;	}
	virtual Activity	GetIdleActivity( void ) { return ACT_VM_IDLE; }
	virtual Activity	GetDeployActivity( void ) { return ACT_VM_DRAW; }
	virtual Activity	GetReloadActivity( void ) { return ACT_VM_RELOAD; }
	virtual Activity	GetHolsterActivity( void ) { return ACT_VM_HOLSTER; }

	virtual bool			Reload( void );
	virtual void			SendReloadEvents();

	float GetWeaponFOV()
	{
		return GetSDKWpnData().m_flWeaponFOV;
	}

	virtual float GetFireRate( void ) { return GetSDKWpnData().m_flCycleTime; };

private:
	CWeaponSDKBase( const CWeaponSDKBase & );
};


#endif // WEAPON_SDKBASE_H
