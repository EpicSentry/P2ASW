// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class portalstaticoverlay_ps20b_Static_Index
{
	unsigned int m_nHASALPHAMASK : 2;
	unsigned int m_nHASSTATICTEXTURE : 2;
	unsigned int m_nPORTALGHOSTOVERLAY : 2;
#ifdef _DEBUG
	bool m_bHASALPHAMASK : 1;
	bool m_bHASSTATICTEXTURE : 1;
	bool m_bPORTALGHOSTOVERLAY : 1;
#endif	// _DEBUG
public:
	void SetHASALPHAMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHASALPHAMASK = i;
#ifdef _DEBUG
		m_bHASALPHAMASK = true;
#endif	// _DEBUG
	}

	void SetHASSTATICTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHASSTATICTEXTURE = i;
#ifdef _DEBUG
		m_bHASSTATICTEXTURE = true;
#endif	// _DEBUG
	}

	void SetPORTALGHOSTOVERLAY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPORTALGHOSTOVERLAY = i;
#ifdef _DEBUG
		m_bPORTALGHOSTOVERLAY = true;
#endif	// _DEBUG
	}

	portalstaticoverlay_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nHASALPHAMASK = 0;
		m_nHASSTATICTEXTURE = 0;
		m_nPORTALGHOSTOVERLAY = 0;
#ifdef _DEBUG
		m_bHASALPHAMASK = false;
		m_bHASSTATICTEXTURE = false;
		m_bPORTALGHOSTOVERLAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bHASALPHAMASK && m_bHASSTATICTEXTURE && m_bPORTALGHOSTOVERLAY );
		return ( 2 * m_nHASALPHAMASK ) + ( 4 * m_nHASSTATICTEXTURE ) + ( 8 * m_nPORTALGHOSTOVERLAY ) + 0;
	}
};

#define shaderStaticTest_portalstaticoverlay_ps20b psh_forgot_to_set_static_HASALPHAMASK + psh_forgot_to_set_static_HASSTATICTEXTURE + psh_forgot_to_set_static_PORTALGHOSTOVERLAY


class portalstaticoverlay_ps20b_Dynamic_Index
{
	unsigned int m_nPIXELFOGTYPE : 2;
public:
	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
	}

	portalstaticoverlay_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
	}

	int GetIndex() const
	{
		return ( 1 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_portalstaticoverlay_ps20b 1

