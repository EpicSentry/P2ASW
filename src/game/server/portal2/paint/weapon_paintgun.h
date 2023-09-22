//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: Paint gun for Paint server side.
//
//=============================================================================//
#ifndef PORTAL2
#define WEAPON_PAINTGUN_H
#endif

#ifndef WEAPON_PAINTGUN_H
#define WEAPON_PAINTGUN_H

#include "sprite.h"
#include "weapon_portalbasecombatweapon.h"
#include "paint/paint_color_manager.h"
#include "paint/prop_paint_power_user.h"


class CPaintStream;
//-----------------------------------------------------------------------------
//Paint gun
//-----------------------------------------------------------------------------
class CWeaponPaintGun : public CBasePortalCombatWeapon
{
	DECLARE_DATADESC();
	DECLARE_CLASS( CWeaponPaintGun, CBasePortalCombatWeapon );
public:

	CWeaponPaintGun();
	~CWeaponPaintGun();

	virtual void	Activate();
	virtual void	Spawn();
	virtual void	Precache();
	virtual void	PrimaryAttack(); // paint
	virtual void	SecondaryAttack(); // eraser
	virtual bool	Deploy();
	virtual void	Drop( const Vector &vecVelocity );
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	virtual bool	SendWeaponAnim( int iActivity );
	virtual void	SetSubType( int iType );
	virtual void	WeaponIdle();
	void			PaintGunThink();
	virtual void	ItemPostFrame( void );
	virtual void	UpdateOnRemove();

	void			SetCurrentPaint( PaintPowerType nIndex );
	PaintPowerType	GetCurrentPaint();
	int				GetPaintCount( void );

	bool			ShouldDisplayHUDHint() { return true; }

	void			ActivatePaint( PaintPowerType nIndex );
	void			ResetPaint();
	void			DeactivatePaint( PaintPowerType nIndex );

	void			CleansePaint();	// Going through a paint cleanser

	bool			HasPaintPower( PaintPowerType nIndex );
	bool			HasAnyPaintPower();

	bool			HasPaintAmmo( unsigned paintType ) const;
	void			DecrementPaintAmmo( unsigned paintType );
	void			ResetAmmo();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

private:
	bool HasCurrentColor();
	void SprayPaint( float flDeltaTime, int paintType );
	void StartShootingSound();
	void StopShootingSound();
	void CreatePaintStreams();

	Vector m_vecOldBlobFirePos;

	CNetworkArray( bool, m_bHasPaint, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER );
	CNetworkArray( EHANDLE, m_hPaintStream, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER );
	CNetworkArray( int, m_PaintAmmoPerType, PAINT_POWER_TYPE_COUNT );

	float m_flAccumulatedTime;
	CNetworkVar( int, m_nCurrentColor );
	CNetworkVar( int, m_nPaintAmmo );
	CNetworkVar( bool, m_bFiringPaint );
	CNetworkVar( bool, m_bFiringErase );

	CSoundPatch *m_pLiquidLoop;	// Looping liquid shooting sound
	CSoundPatch *m_pLiquidStart;	// Liquid start shooting sound

	int m_nBlobRandomSeed;
};

#endif // WEAPON_PAINTGUN_H
