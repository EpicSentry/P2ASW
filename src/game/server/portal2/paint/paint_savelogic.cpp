#include "cbase.h"
#include "util_shared.h"
#include "paint_savelogic.h"

BEGIN_SIMPLE_DATADESC( PaintTraceData_t )

DEFINE_FIELD( pBrushEntity, FIELD_EHANDLE ),

END_DATADESC()

CUtlVector<PaintTraceData_t> g_AllPaintTraces;

// The main problem is that data.pBrushEntity isn't being saved, figure this out later...
#define PAINTSAVE_TODO

void AddPaintDataToMemory( PaintTraceData_t data )
{
#ifndef PAINTSAVE_TODO
	Assert( HASPAINTMAP );

	PaintPowerType power = UTIL_Paint_TracePower( data.pBrushEntity, data.contactPoint, data.normal );

	if ( power != data.power )
	{
		g_AllPaintTraces.AddToTail( data );
		DevMsg("Add paint power: %i\n", data.power);
	}
#endif
}

void RestoreAllPaint( void )
{
#ifndef PAINTSAVE_TODO
	Assert( HASPAINTMAP );

	for ( int i = 0; i < g_AllPaintTraces.Count(); ++i )
	{
		PaintTraceData_t data = g_AllPaintTraces[i];

		Assert( data.pBrushEntity );

		UTIL_PaintBrushEntity( data.pBrushEntity, data.contactPoint, data.power, data.flPaintRadius, 0.0, NULL );		
		DevMsg("Restore paint power: %i\n", data.power);
	}

	g_AllPaintTraces.Purge();
#endif
}