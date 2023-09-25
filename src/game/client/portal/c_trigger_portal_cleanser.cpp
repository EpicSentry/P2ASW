//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "c_triggers.h"
#include "c_portal_player.h"
#include "c_weapon_portalgun.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "model_types.h"
#include "rumble_shared.h"

#include "c_trigger_portal_cleanser.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#undef CTriggerPortalCleanser

IMPLEMENT_CLIENTCLASS_DT(C_TriggerPortalCleanser, DT_TriggerPortalCleanser, CTriggerPortalCleanser)
	RecvPropBool(RECVINFO(m_bDisabled))
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( trigger_portal_cleanser, C_TriggerPortalCleanser );

C_TriggerPortalCleanser::C_TriggerPortalCleanser()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TriggerPortalCleanser::Spawn( void )
{	
	BaseClass::Spawn();
}

// Creates a base entity with model/physics matching the parameter ent.
// Used to avoid higher level functions on a disolving entity, which should be inert
// and not react the way it used to (touches, etc).
// Uses simple physics entities declared in physobj.cpp
C_BaseEntity* ConvertToSimpleProp ( C_BaseEntity* pEnt )
{
	C_BaseEntity *pRetVal = NULL;
	int modelindex = pEnt->GetModelIndex();
	const model_t *model = modelinfo->GetModel( modelindex );
	if ( model && modelinfo->GetModelType(model) == mod_brush )
	{
		pRetVal = CreateEntityByName( "simple_physics_brush" );
	}
	else
	{
		pRetVal = CreateEntityByName( "simple_physics_prop" );
	}

	pRetVal->KeyValue( "model", STRING(pEnt->GetModelName()) );
	pRetVal->SetAbsOrigin( pEnt->GetAbsOrigin() );
	pRetVal->SetAbsAngles( pEnt->GetAbsAngles() );
	pRetVal->Spawn();
	pRetVal->VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
	
	return pRetVal;
}


void C_TriggerPortalCleanser::Touch( C_BaseEntity *pOther )
{
	BaseClass::Touch( pOther );
}