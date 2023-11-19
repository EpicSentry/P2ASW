#ifndef PAINT_SAVE_LOGIC_H
#define PAINT_SAVE_LOGIC_H

#include "util_shared.h"
#include "paint/paint_color_manager.h"

struct PaintTraceData_t
{
	DECLARE_SIMPLE_DATADESC()
	// No need for a constructor because these will be set as soon as this struct is initiated.
	EHANDLE pBrushEntity;
	PaintPowerType power;
	Vector contactPoint;
	Vector normal;
	float flPaintRadius;
};

void AddPaintDataToMemory( PaintTraceData_t data );
void RestoreAllPaint( void );

#endif // PAINT_SAVE_LOGIC_H