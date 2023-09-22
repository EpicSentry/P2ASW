//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: Paint gun for Paint client side.
//
//=============================================================================//
#ifndef PORTAL2
#define C_WEAPON_PAINTGUN_H
#endif

#ifndef C_WEAPON_PAINTGUN_H
#define C_WEAPON_PAINTGUN_H

#include "paint/paint_color_manager.h"
#include "weapon_portalbasecombatweapon.h"

class C_PaintStream;

class C_WeaponPaintGun : public CBasePortalCombatWeapon
{
	DECLARE_CLASS( C_WeaponPaintGun, CBasePortalCombatWeapon );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
public:
	C_WeaponPaintGun();

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void WeaponIdle();
	virtual void ClientThink();

	virtual bool Simulate();
	virtual void OnPreDataChanged( DataUpdateType_t type );
	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual bool Deploy( void );
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void Drop( const Vector &vecVelocity );
	virtual void SetSubType( int iType );
	virtual void ItemPostFrame();

	void StartHoseEffect();
	void StopHoseEffect();

	C_BaseViewModel* GetEffectViewModel();

	virtual void	AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float	CalcViewmodelBob( void );

	void SetPaintPower( PaintPowerType type );
	void CyclePaintPower( bool bForward );
	PaintPowerType GetCurrentPaint( void );
	bool HasPaintPower( PaintPowerType nIndex );
	bool HasAnyPaintPower();
	void ChangeRenderColor( bool bForce = false );

	virtual void	PrimaryAttack();
	virtual void	SecondaryAttack();

	bool			HasPaintAmmo( unsigned paintType ) const;
	void			DecrementPaintAmmo( unsigned paintType );
	void			ResetAmmo();
	int				GetPaintAmmo() const;

	DECLARE_ACTTABLE();
private:

	void SprayPaint( float flDeltaTime, int paintType );

	CUtlReference<CNewParticleEffect> m_pStreamEffect;
	float m_flBobScale;
	float m_flAccumulatedTime;
	Vector m_vecOldBlobFirePos;

	float m_flLastThinkTime;

	// Networked variables
	int m_nCurrentColor;
	bool m_bFiringPaint;
	bool m_bFiringErase;
	int m_nPaintAmmo;
	bool m_bHasPaint[PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER];
	CNetworkArray( EHANDLE, m_hPaintStream, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER );
	CNetworkArray( int, m_PaintAmmoPerType, PAINT_POWER_TYPE_COUNT );

	// data for stopping particle when throw weapon
	CBaseCombatCharacter* m_pLastOwner;
	CUtlReference<CNewParticleEffect> m_pLastEffect;

	int m_nBlobRandomSeed;
};

typedef C_WeaponPaintGun CWeaponPaintGun;

#endif // C_WEAPON_PAINTGUN_H
