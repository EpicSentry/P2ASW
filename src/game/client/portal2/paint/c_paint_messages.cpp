//===== Copyright 1996-2009, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "igamesystem.h"
#include "hud_macros.h" //HOOK_MESSAGE
#include "paint/paint_color_manager.h"
#include "c_world.h"
#include "paint/paint_sprayer_shared.h"
#include "paint/paintable_entity.h"
#include "cdll_int.h"
#ifdef PORTAL2
#include "c_weapon_paintgun.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void __MsgFunc_PaintWorld( bf_read &msg )
{
	// if client is local to active server, don't do paint in client
	// because the paintmap is shared in the same engine
	if( !engine->IsClientLocalToActiveServer() )
	{
		// Get the color index and number of paint locations
		PaintPowerType power = static_cast< PaintPowerType >( msg.ReadByte() );
		C_BaseEntity *pBrushEntity = UTIL_EntityFromUserMessageEHandle( msg.ReadLong() );
		float flPaintRadius = msg.ReadFloat();
		float flAlphaPercent = msg.ReadFloat();

		int nPaintCount = msg.ReadByte();

		// Get the center point
		Vector vCenter;
		vCenter.x = msg.ReadFloat();
		vCenter.y = msg.ReadFloat();
		vCenter.z = msg.ReadFloat();

		// For each offset
		Vector vContactPoint;
		for( int i = 0; i < nPaintCount; ++i )
		{
			// Compute the position
			vContactPoint.x = vCenter.x + msg.ReadShort();
			vContactPoint.y = vCenter.y + msg.ReadShort();
			vContactPoint.z = vCenter.z + msg.ReadShort();

			UTIL_PaintBrushEntity( pBrushEntity, vContactPoint, power, flPaintRadius, flAlphaPercent );
		}
	}
}


void __MsgFunc_PaintEntity( bf_read &msg )
{
	IPaintableEntity* pPaintableEnt = dynamic_cast< IPaintableEntity* >( UTIL_EntityFromUserMessageEHandle( msg.ReadLong() ) );
	const PaintPowerType power = static_cast< PaintPowerType >( msg.ReadByte() );
	Vector pos;
	pos.x = msg.ReadFloat();
	pos.y = msg.ReadFloat();
	pos.z = msg.ReadFloat();

	pPaintableEnt->Paint( power, pos );
}


void __MsgFunc_ChangePaintColor( bf_read &msg )
{
#ifdef PORTAL2
	// get entity
	C_BaseEntity *pEntity = UTIL_EntityFromUserMessageEHandle( msg.ReadLong() );
	if ( pEntity == NULL )
	{
		DevMsg("Failed to change paint color");
		return;
	}

	C_WeaponPaintGun *pPaintGun = dynamic_cast< C_WeaponPaintGun* >( pEntity );
	if ( pPaintGun )
	{
		int power = msg.ReadByte();

		pPaintGun->SetSubType( power );
		pPaintGun->ChangeRenderColor( true );
	}
#endif
}


void __MsgFunc_RemoveAllPaint( bf_read &msg )
{
	//if( !engine->IsClientLocalToActiveServer() )
	{
		//engine->RemoveAllPaint();
	}
}


void __MsgFunc_PaintAllSurfaces( bf_read &msg )
{
	/*
	if( !engine->IsClientLocalToActiveServer() )
	{
		BYTE power = msg.ReadByte();
		engine->PaintAllSurfaces( power );
	}
	*/
}


void __MsgFunc_RemovePaint( bf_read &msg )
{
	/*
	if( engine->IsClientLocalToActiveServer() )
		return;

	C_BaseEntity *pEntity = UTIL_EntityFromUserMessageEHandle( msg.ReadLong() );
	if ( pEntity && pEntity->IsBSPModel() )
	{
		engine->RemovePaint( pEntity->GetModel() );
	}
	*/
}


// This class is to hook message for all usermessages in Paint
class C_PaintInitHelper : public CAutoGameSystem
{
	virtual bool Init()
	{
		for ( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
			HOOK_MESSAGE( PaintWorld );
			HOOK_MESSAGE( PaintEntity );
			HOOK_MESSAGE( ChangePaintColor );
			HOOK_MESSAGE( RemoveAllPaint );
			HOOK_MESSAGE( PaintAllSurfaces );
			HOOK_MESSAGE( RemovePaint );
		}

		return true;
	}
};
static C_PaintInitHelper s_PaintInitHelper;
