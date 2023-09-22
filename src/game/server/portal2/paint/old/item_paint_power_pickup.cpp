//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"
#ifdef PORTAL2
#include "portal_player_shared.h"
#endif
#include "paint/paint_color_manager.h"
#include "items.h"
#include "weapon_paintgun.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PAINT_POWER_PICKUP_MODEL "models/props/water_bottle/water_bottle.mdl"

void PaintPowerPickup( int colorIndex, CBasePlayer *pPlayer )
{
	if ( !pPlayer || !pPlayer->GetActiveWeapon() )
		return;

	CBaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();

	CBroadcastRecipientFilter filter;
	filter.MakeReliable();

	UserMessageBegin( filter, "ChangePaintColor" );
	WRITE_EHANDLE( pWeapon );
	WRITE_BYTE( colorIndex );
	MessageEnd();
}


class CPaintPowerPickup : public CItem
{
public:
	DECLARE_CLASS( CPaintPowerPickup, CItem );
	DECLARE_DATADESC();

	CPaintPowerPickup();

	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void Touch( CBaseEntity *pOther );

private:
	int m_PaintPowerType;
};

BEGIN_DATADESC( CPaintPowerPickup )
	DEFINE_KEYFIELD( m_PaintPowerType, FIELD_INTEGER, "PaintType" )
END_DATADESC()

//LINK_ENTITY_TO_CLASS( item_paint_power_pickup, CPaintPowerPickup );
//PRECACHE_REGISTER( item_paint_power_pickup );


CPaintPowerPickup::CPaintPowerPickup()
			: m_PaintPowerType( NO_POWER )
{}


void CPaintPowerPickup::Spawn( void )
{
	Precache();
	SetModel( PAINT_POWER_PICKUP_MODEL );
	Color renderColor = MapPowerToVisualColor( m_PaintPowerType );
	SetRenderColor( renderColor.r(), renderColor.g(), renderColor.b() );
	BaseClass::Spawn();

	//Paint pickups are unpaintable
	AddFlag( FL_UNPAINTABLE );

	// Destroy VPhysics so players can't move this pickup.  We want them stationary
	VPhysicsDestroyObject();
}


void CPaintPowerPickup::Precache( void )
{
	PrecacheModel( PAINT_POWER_PICKUP_MODEL );
}


void CPaintPowerPickup::Touch( CBaseEntity* pOther )
{
	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
		return;

	CBasePlayer* pPlayer = dynamic_cast<CBasePlayer*>( pOther );

	CBaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
	if( pWeapon && FClassnameIs( pWeapon, "weapon_paintgun" ) )
	{
		CWeaponPaintGun *pPaintGun = dynamic_cast<CWeaponPaintGun*>( pWeapon );
		if( pPaintGun && pPaintGun->HasPaintPower( (PaintPowerType)m_PaintPowerType ) == false )
		{
			pPaintGun->ActivatePaint( (PaintPowerType)m_PaintPowerType );
			pPaintGun->EmitSound( "BaseCombatWeapon.WeaponMaterialize" );

			PaintPowerPickup( m_PaintPowerType, pPlayer );
		}
	}
}


