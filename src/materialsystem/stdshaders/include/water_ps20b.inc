// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// $MULTITEXTURE && $BASETEXTURE
// $SIMPLEOVERLAY && $BASETEXTURE
// $SIMPLEOVERLAY && !$FLOWMAP
// $FLOWMAP && $MULTITEXTURE
// ( $FLASHLIGHT == 0 ) && ( $FLASHLIGHTSHADOWS == 1 )
// ( $FLASHLIGHT || $LIGHTMAPWATERFOG ) && ( ( $BASETEXTURE && !$FLOWMAP ) || $MULTITEXTURE )
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
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class water_ps20b_Static_Index
{
	unsigned int m_nBASETEXTURE : 2;
	unsigned int m_nMULTITEXTURE : 2;
	unsigned int m_nREFLECT : 2;
	unsigned int m_nREFRACT : 2;
	unsigned int m_nABOVEWATER : 2;
	unsigned int m_nFLOWMAP : 2;
	unsigned int m_nFLOW_DEBUG : 2;
	unsigned int m_nFLASHLIGHT : 2;
	unsigned int m_nLIGHTMAPWATERFOG : 2;
	unsigned int m_nFORCEFRESNEL : 2;
	unsigned int m_nSIMPLEOVERLAY : 2;
#ifdef _DEBUG
	bool m_bBASETEXTURE : 1;
	bool m_bMULTITEXTURE : 1;
	bool m_bREFLECT : 1;
	bool m_bREFRACT : 1;
	bool m_bABOVEWATER : 1;
	bool m_bFLOWMAP : 1;
	bool m_bFLOW_DEBUG : 1;
	bool m_bFLASHLIGHT : 1;
	bool m_bLIGHTMAPWATERFOG : 1;
	bool m_bFORCEFRESNEL : 1;
	bool m_bSIMPLEOVERLAY : 1;
#endif	// _DEBUG
public:
	void SetBASETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURE = i;
#ifdef _DEBUG
		m_bBASETEXTURE = true;
#endif	// _DEBUG
	}

	void SetMULTITEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMULTITEXTURE = i;
#ifdef _DEBUG
		m_bMULTITEXTURE = true;
#endif	// _DEBUG
	}

	void SetREFLECT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFLECT = i;
#ifdef _DEBUG
		m_bREFLECT = true;
#endif	// _DEBUG
	}

	void SetREFRACT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFRACT = i;
#ifdef _DEBUG
		m_bREFRACT = true;
#endif	// _DEBUG
	}

	void SetABOVEWATER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nABOVEWATER = i;
#ifdef _DEBUG
		m_bABOVEWATER = true;
#endif	// _DEBUG
	}

	void SetFLOWMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLOWMAP = i;
#ifdef _DEBUG
		m_bFLOWMAP = true;
#endif	// _DEBUG
	}

	void SetFLOW_DEBUG( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nFLOW_DEBUG = i;
#ifdef _DEBUG
		m_bFLOW_DEBUG = true;
#endif	// _DEBUG
	}

	void SetFLASHLIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHT = i;
#ifdef _DEBUG
		m_bFLASHLIGHT = true;
#endif	// _DEBUG
	}

	void SetLIGHTMAPWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPWATERFOG = i;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = true;
#endif	// _DEBUG
	}

	void SetFORCEFRESNEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFORCEFRESNEL = i;
#ifdef _DEBUG
		m_bFORCEFRESNEL = true;
#endif	// _DEBUG
	}

	void SetSIMPLEOVERLAY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSIMPLEOVERLAY = i;
#ifdef _DEBUG
		m_bSIMPLEOVERLAY = true;
#endif	// _DEBUG
	}

	water_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nBASETEXTURE = 0;
		m_nMULTITEXTURE = 0;
		m_nREFLECT = 0;
		m_nREFRACT = 0;
		m_nABOVEWATER = 0;
		m_nFLOWMAP = 0;
		m_nFLOW_DEBUG = 0;
		m_nFLASHLIGHT = 0;
		m_nLIGHTMAPWATERFOG = 0;
		m_nFORCEFRESNEL = 0;
		m_nSIMPLEOVERLAY = 0;
#ifdef _DEBUG
		m_bBASETEXTURE = false;
		m_bMULTITEXTURE = false;
		m_bREFLECT = false;
		m_bREFRACT = false;
		m_bABOVEWATER = false;
		m_bFLOWMAP = false;
		m_bFLOW_DEBUG = false;
		m_bFLASHLIGHT = false;
		m_bLIGHTMAPWATERFOG = false;
		m_bFORCEFRESNEL = false;
		m_bSIMPLEOVERLAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBASETEXTURE && m_bMULTITEXTURE && m_bREFLECT && m_bREFRACT && m_bABOVEWATER && m_bFLOWMAP && m_bFLOW_DEBUG && m_bFLASHLIGHT && m_bLIGHTMAPWATERFOG && m_bFORCEFRESNEL && m_bSIMPLEOVERLAY );
		AssertMsg( !( m_nMULTITEXTURE && m_nBASETEXTURE ), "Invalid combo combination ( MULTITEXTURE && BASETEXTURE )" );
		AssertMsg( !( m_nSIMPLEOVERLAY && m_nBASETEXTURE ), "Invalid combo combination ( SIMPLEOVERLAY && BASETEXTURE )" );
		AssertMsg( !( m_nSIMPLEOVERLAY && !m_nFLOWMAP ), "Invalid combo combination ( SIMPLEOVERLAY && !FLOWMAP )" );
		AssertMsg( !( m_nFLOWMAP && m_nMULTITEXTURE ), "Invalid combo combination ( FLOWMAP && MULTITEXTURE )" );
		AssertMsg( !( ( m_nFLASHLIGHT || m_nLIGHTMAPWATERFOG ) && ( ( m_nBASETEXTURE && !m_nFLOWMAP ) || m_nMULTITEXTURE ) ), "Invalid combo combination ( ( FLASHLIGHT || LIGHTMAPWATERFOG ) && ( ( BASETEXTURE && !FLOWMAP ) || MULTITEXTURE ) )" );
		return ( 8 * m_nBASETEXTURE ) + ( 16 * m_nMULTITEXTURE ) + ( 32 * m_nREFLECT ) + ( 64 * m_nREFRACT ) + ( 128 * m_nABOVEWATER ) + ( 256 * m_nFLOWMAP ) + ( 512 * m_nFLOW_DEBUG ) + ( 1536 * m_nFLASHLIGHT ) + ( 3072 * m_nLIGHTMAPWATERFOG ) + ( 6144 * m_nFORCEFRESNEL ) + ( 12288 * m_nSIMPLEOVERLAY ) + 0;
	}
};

#define shaderStaticTest_water_ps20b psh_forgot_to_set_static_BASETEXTURE + psh_forgot_to_set_static_MULTITEXTURE + psh_forgot_to_set_static_REFLECT + psh_forgot_to_set_static_REFRACT + psh_forgot_to_set_static_ABOVEWATER + psh_forgot_to_set_static_FLOWMAP + psh_forgot_to_set_static_FLOW_DEBUG + psh_forgot_to_set_static_FLASHLIGHT + psh_forgot_to_set_static_LIGHTMAPWATERFOG + psh_forgot_to_set_static_FORCEFRESNEL + psh_forgot_to_set_static_SIMPLEOVERLAY


class water_ps20b_Dynamic_Index
{
	unsigned int m_nFLASHLIGHTSHADOWS : 2;
	unsigned int m_nBUILDWORLDIMPOSTER : 2;
	unsigned int m_nPIXELFOGTYPE : 2;
#ifdef _DEBUG
	bool m_bFLASHLIGHTSHADOWS : 1;
#endif	// _DEBUG
public:
	void SetFLASHLIGHTSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHTSHADOWS = i;
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = true;
#endif	// _DEBUG
	}

	void SetBUILDWORLDIMPOSTER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUILDWORLDIMPOSTER = i;
	}

	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
	}

	water_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nFLASHLIGHTSHADOWS = 0;
		m_nBUILDWORLDIMPOSTER = r_buildingmapforworld.GetBool() ? 1 : 0 ;
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFLASHLIGHTSHADOWS );
		return ( 1 * m_nFLASHLIGHTSHADOWS ) + ( 2 * m_nBUILDWORLDIMPOSTER ) + ( 4 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_water_ps20b psh_forgot_to_set_dynamic_FLASHLIGHTSHADOWS

