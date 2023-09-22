//============== Copyright © Valve Corporation, All rights reserved. =================//
//
// Entities which paint brushes that intersect with specified sphere
//
//=====================================================================================//
#ifndef PAINT_SPHERE_H
#define PAINT_SPHERE_H

#ifdef _WIN32
#pragma once
#endif

#include "paint/paint_color_manager.h"

class CPaintSphere : public CPointEntity
{
public:
	DECLARE_CLASS( CPaintSphere, CPointEntity );
	DECLARE_DATADESC();

	CPaintSphere();

	void Paint( inputdata_t &inputdata );
private:
	PaintPowerType m_nPaintType;
	float m_flSphereRadius;
	float m_flAlphaPercent;
};

#endif
