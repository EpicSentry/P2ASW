//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_WEAPON_PORTALGUN_H
#define C_WEAPON_PORTALGUN_H

#ifdef _WIN32
#pragma once
#endif

#define CWeaponPortalgun C_WeaponPortalgun

#include "weapon_portalgun_shared.h"
#include "weapon_portalbasecombatweapon.h"

#include "c_prop_portal.h"
#include "fx_interpvalue.h"
#include "beamdraw.h"
#include "iviewrender_beams.h"


class C_WeaponPortalgun : public CBasePortalCombatWeapon
{

public:
	DECLARE_CLASS( C_WeaponPortalgun, CBasePortalCombatWeapon );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

private:
	CNetworkVar( bool,	m_bCanFirePortal1 );	// Is able to use primary fire
	CNetworkVar( bool,	m_bCanFirePortal2 );	// Is able to use secondary fire
	CNetworkVar( int,	m_iLastFiredPortal );	// Which portal was placed last
	CNetworkVar( bool,	m_bOpenProngs );		// Which portal was placed last

	CNetworkVar( float,	m_fEffectsMaxSize1 );
	CNetworkVar( float,	m_fEffectsMaxSize2 );

	unsigned char m_iPortalLinkageGroupID;

public:
	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_10DEGREES;
		return cone;
	}
	
	void Precache ( void );

	virtual void OnRestore( void );
	virtual void UpdateOnRemove( void );
	void Spawn( void );
	void DoEffectBlast( CBaseEntity *pOwner, bool bPortal2, int iPlacedBy, const Vector &ptStart, const Vector &ptFinalPos, const QAngle &qStartAngles, float fDelay );
	void DoEffectCreate( Vector &vDir, Vector &ptStart, Vector &ptEnd, bool bPortal1, bool bPlayer );

	virtual bool ShouldDrawCrosshair( void );
	void SetLastFiredPortal( int iLastFiredPortal ) { m_iLastFiredPortal = iLastFiredPortal; }
	int GetLastFiredPortal( void ) { return m_iLastFiredPortal; }
	bool IsHoldingObject( void ) { return m_bOpenProngs; }
	
	void FirePortal1( void );
	void FirePortal2( void );

	bool Reload( void );
	void FillClip( void );
	void CheckHolsterReload( void );
	void ItemHolsterFrame( void );
	bool Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	bool Deploy( void );

	void SetCanFirePortal1( bool bCanFire = true );
	void SetCanFirePortal2( bool bCanFire = true );
	bool CanFirePortal1( void ) const;
	bool CanFirePortal2( void ) const;
	
	Activity GetPrimaryAttackActivity( void );
	
	void ResetRefireTime( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );

	void DelayAttack( float fDelay );

	void DryFire( void );
	virtual float GetFireRate( void ) { return 0.7; };
	void WeaponIdle( void );
	
	void PostAttack( void );
	

	bool TraceFirePortal( const Vector &vTraceStart, const Vector &vDirection, bool bPortal2, PortalPlacedBy_t ePlacedBy, TracePortalPlacementInfo_t &placementInfo );
	PortalPlacementResult_t FirePortal( bool bPortal2, Vector *pVector = 0 );
	
	bool  PortalTraceClippedByBlockers( ComplexPortalTrace_t *pTraceResults, int nNumResultSegments, const Vector &vecDirection, bool bIsSecondPortal, TracePortalPlacementInfo_t &placementInfo );
	bool AttemptStealCoopPortal( TracePortalPlacementInfo_t &placementInfo );
	bool AttemptSnapToPlacementHelper( CProp_Portal *pPortal, ComplexPortalTrace_t *pTraceResults, int nNumResultSegments, PortalPlacedBy_t ePlacedBy, TracePortalPlacementInfo_t &placementInfo );
	CProp_Portal *GetAssociatedPortal( bool bPortal2 );

	Vector m_vecOrangePortalPos;
	Vector m_vecBluePortalPos;
	
	void UseDeny( void );
    void DoCleanseEffect( bool bPortal1Active, bool bPortal2Active );

	int GetLinkageGroupID() { return m_iPortalLinkageGroupID; }

protected:
	
	void	StartEffects( void );	// Initialize all sprites and beams
	void	StopEffects( bool stopSound = true );	// Hide all effects temporarily
	void	DestroyEffects( void );	// Destroy all sprites and beams

	// Portalgun effects
	void	DoEffect( int effectType, Vector *pos = NULL );

	void	DoEffectClosed( void );
	void	DoEffectReady( void );
	void	DoEffectHolding( void );
	void	DoEffectNone( void );
	

	CHandle<CProp_Portal> m_hPrimaryPortal;
	CHandle<CProp_Portal> m_hSecondaryPortal;

    CUtlReference<CNewParticleEffect> m_hPortalGunEffectFP;
    CUtlReference<CNewParticleEffect> m_hPortalGunEffectTP;
    CUtlReference<CNewParticleEffect> m_hPortalGunEffectHoldingFP;
    CUtlReference<CNewParticleEffect> m_hPortalGunEffectHoldingTP;
#if 0
	enum EffectType_t
	{
		PORTALGUN_GRAVLIGHT = 0,
		PORTALGUN_GRAVLIGHT_WORLD,
		PORTALGUN_PORTAL1LIGHT,
		PORTALGUN_PORTAL1LIGHT_WORLD,
		PORTALGUN_PORTAL2LIGHT,
		PORTALGUN_PORTAL2LIGHT_WORLD,

		PORTALGUN_GLOW1,	// Must be in order!
		PORTALGUN_GLOW2,
		PORTALGUN_GLOW3,
		PORTALGUN_GLOW4,
		PORTALGUN_GLOW5,
		PORTALGUN_GLOW6,

		PORTALGUN_GLOW1_WORLD,
		PORTALGUN_GLOW2_WORLD,
		PORTALGUN_GLOW3_WORLD,
		PORTALGUN_GLOW4_WORLD,
		PORTALGUN_GLOW5_WORLD,
		PORTALGUN_GLOW6_WORLD,

		PORTALGUN_ENDCAP1,	// Must be in order!
		PORTALGUN_ENDCAP2,
		PORTALGUN_ENDCAP3,

		PORTALGUN_ENDCAP1_WORLD,
		PORTALGUN_ENDCAP2_WORLD,
		PORTALGUN_ENDCAP3_WORLD,

		PORTALGUN_MUZZLE_GLOW,

		PORTALGUN_MUZZLE_GLOW_WORLD,

		PORTALGUN_TUBE_BEAM1,
		PORTALGUN_TUBE_BEAM2,
		PORTALGUN_TUBE_BEAM3,
		PORTALGUN_TUBE_BEAM4,
		PORTALGUN_TUBE_BEAM5,

		PORTALGUN_TUBE_BEAM1_WORLD,
		PORTALGUN_TUBE_BEAM2_WORLD,
		PORTALGUN_TUBE_BEAM3_WORLD,
		PORTALGUN_TUBE_BEAM4_WORLD,
		PORTALGUN_TUBE_BEAM5_WORLD,

		NUM_PORTALGUN_PARAMETERS	// Must be last!
	};
#endif
	#define	NUM_GLOW_SPRITES ((C_WeaponPortalgun::PORTALGUN_GLOW6-C_WeaponPortalgun::PORTALGUN_GLOW1)+1)
	#define	NUM_GLOW_SPRITES_WORLD ((C_WeaponPortalgun::PORTALGUN_GLOW6_WORLD-C_WeaponPortalgun::PORTALGUN_GLOW1_WORLD)+1)
	#define NUM_ENDCAP_SPRITES ((C_WeaponPortalgun::PORTALGUN_ENDCAP3-C_WeaponPortalgun::PORTALGUN_ENDCAP1)+1)
	#define NUM_ENDCAP_SPRITES_WORLD ((C_WeaponPortalgun::PORTALGUN_ENDCAP3_WORLD-C_WeaponPortalgun::PORTALGUN_ENDCAP1_WORLD)+1)
	#define NUM_TUBE_BEAM_SPRITES ((C_WeaponPortalgun::PORTALGUN_TUBE_BEAM5-C_WeaponPortalgun::PORTALGUN_TUBE_BEAM1)+1)
	#define NUM_TUBE_BEAM_SPRITES_WORLD ((C_WeaponPortalgun::PORTALGUN_TUBE_BEAM5_WORLD-C_WeaponPortalgun::PORTALGUN_TUBE_BEAM1_WORLD)+1)

	#define	NUM_PORTALGUN_BEAMS	6

	Vector			GetEffectColor( int iPalletIndex );

	int				m_nOldEffectState;	// Used for parity checks
	bool			m_bOldCanFirePortal1;
	bool			m_bOldCanFirePortal2;

	bool			m_bPulseUp;
	float			m_fPulse;

	CNetworkVar( int,	m_EffectState );		// Current state of the effects on the gun

public:

	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	ClientThink( void );
	void DoEffectIdle( void );

public:

	DECLARE_ACTTABLE();

	C_WeaponPortalgun(void);
	~C_WeaponPortalgun(void);

private:
	C_WeaponPortalgun( const C_WeaponPortalgun & );

};


#endif // C_WEAPON_PORTALGUN_H
