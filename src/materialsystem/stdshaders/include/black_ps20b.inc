// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class black_ps20b_Static_Index
{
	unsigned int m_nDOPIXELFOG : 1;
public:
	void SetDOPIXELFOG( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nDOPIXELFOG = i;
	}

	black_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nDOPIXELFOG = 0 ;
	}

	int GetIndex() const
	{
		return ( 1 * m_nDOPIXELFOG ) + 0;
	}
};

#define shaderStaticTest_black_ps20b 1


class black_ps20b_Dynamic_Index
{
	unsigned int m_nPIXELFOGTYPE : 1;
public:
	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nPIXELFOGTYPE = i;
	}

	black_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nPIXELFOGTYPE = 0;
	}

	int GetIndex() const
	{
		return ( 1 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_black_ps20b 1

