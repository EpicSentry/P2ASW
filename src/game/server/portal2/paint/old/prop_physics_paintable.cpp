//========= Copyright 1996-2010, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"
#include "props.h"
#include "cegclientwrapper.h"
#include "paint/player_pickup_paint_power_user.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
/*
#if !defined( FRONTLINEFORCE )	//Tony; in FLF, CPhysicsProp is a paintpoweruser.
class CPropPhysicsPaintable : public PlayerPickupPaintPowerUser< CPhysicsProp >
{
	DECLARE_CLASS( CPropPhysicsPaintable, PlayerPickupPaintPowerUser< CPhysicsProp > );
};


LINK_ENTITY_TO_CLASS( prop_physics_paintable, CPropPhysicsPaintable );	
#else
LINK_ENTITY_TO_CLASS( prop_physics_paintable, CPhysicsProp );	
#endif
*/
