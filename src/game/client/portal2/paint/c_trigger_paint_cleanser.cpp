//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"

#include "c_trigger_paint_cleanser.h"
#include "paint/paint_cleanser_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_TriggerPaintCleanser, DT_TriggerPaintCleanser, CTriggerPaintCleanser )
	RecvPropBool( RECVINFO(m_bDisabled) )
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( trigger_paint_cleanser, C_TriggerPaintCleanser );

C_TriggerPaintCleanser::C_TriggerPaintCleanser()
{
}


C_TriggerPaintCleanser::~C_TriggerPaintCleanser()
{
}


void C_TriggerPaintCleanser::Spawn( void )
{
	BaseClass::Spawn();

	PaintCleanserManager.AddPaintCleanser( this );
}


void C_TriggerPaintCleanser::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		// need to add the entity to partition tree so we can trace against this on the client
		Vector vWorldMins, vWorldMaxs;
		CollisionProp()->WorldSpaceSurroundingBounds( &vWorldMins, &vWorldMaxs );

		partition->ElementMoved( CollisionProp()->GetPartitionHandle(), vWorldMins, vWorldMaxs );
	}
}


void C_TriggerPaintCleanser::UpdateOnRemove( void )
{
	PaintCleanserManager.RemovePaintCleanser( this );

	BaseClass::UpdateOnRemove();
}