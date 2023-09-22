//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//======================================================================================//

#include "cbase.h"
#include "trigger_paint_cleanser.h"
#ifdef PORTAL2
#include "portal_player.h"
#include "weapon_paintgun.h"
#endif
#include "paint_database.h"
#include "paint/paint_cleanser_manager.h"
#include "prop_paint_bomb.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CTriggerPaintCleanser )
END_DATADESC()


IMPLEMENT_SERVERCLASS_ST( CTriggerPaintCleanser, DT_TriggerPaintCleanser )
	SendPropBool( SENDINFO(m_bDisabled) )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( trigger_paint_cleanser, CTriggerPaintCleanser );

CTriggerPaintCleanser::CTriggerPaintCleanser( void )
{
}


void CTriggerPaintCleanser::Spawn( void )
{
	BaseClass::Spawn();
	InitTrigger();

}


void CTriggerPaintCleanser::Activate( void )
{
	BaseClass::Activate();

	PaintCleanserManager.AddPaintCleanser( this );
}


void CTriggerPaintCleanser::UpdateOnRemove( void )
{
	PaintCleanserManager.RemovePaintCleanser( this );

	BaseClass::UpdateOnRemove();
}


void CTriggerPaintCleanser::Touch( CBaseEntity *pOther )
{
#ifdef PORTAL2
	// check if it's paintgun
	CWeaponPaintGun *pPaintGun = NULL;
	if ( pOther->IsPlayer() )
	{
		CPortal_Player *pPlayer = ToPortalPlayer( pOther );
		if ( pPlayer )
		{
			pPaintGun = dynamic_cast< CWeaponPaintGun* >( pPlayer->Weapon_OwnsThisType("weapon_paintgun") );
			pPlayer->CleansePaint();

			IGameEvent * event = gameeventmanager->CreateEvent( "player_touch_paint_cleanser" );
			if ( event )
			{
				event->SetInt( "userid", pPlayer->GetUserID() );
				event->SetInt( "subject", this->entindex() );

				gameeventmanager->FireEvent( event );
			}
		}
	}
	else
	{
		pPaintGun = dynamic_cast< CWeaponPaintGun* >( pOther );
	}

	if( pPaintGun )
	{
		pPaintGun->CleansePaint();
	}
	else 
#endif //PORTAL2
	if( FClassnameIs( pOther, "paintgun_blob" ) )
	{
		UTIL_Remove( pOther );
		return;
	}
	else if( FClassnameIs( pOther, "prop_weighted_cube" ) )
	{
		PaintDatabase.RemovePaintedEntity( pOther );
	}
	else if( FClassnameIs( pOther, "prop_paint_bomb" ) )
	{
		//Explode the paint bomb by giving it damage
		CTakeDamageInfo info( this, this, pOther->GetHealth(), DMG_GENERIC );
		info.SetDamagePosition( pOther->GetAbsOrigin() );

		pOther->TakeDamage( info );
	}
}


void CTriggerPaintCleanser::Enable( void )
{
	BaseClass::Enable();
}


int CTriggerPaintCleanser::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}
