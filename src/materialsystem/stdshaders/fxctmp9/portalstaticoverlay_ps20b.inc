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
#include "shaderlib/cshader.h"
class portalstaticoverlay_ps20b_Static_Index
{
private:
	int m_nHASALPHAMASK;
#ifdef _DEBUG
	bool m_bHASALPHAMASK;
#endif
public:
	void SetHASALPHAMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHASALPHAMASK = i;
#ifdef _DEBUG
		m_bHASALPHAMASK = true;
#endif
	}
	void SetHASALPHAMASK( bool i )
	{
		Assert( ( i ? 1 : 0 ) >= 0 && ( i ? 1 : 0 ) <= 1 );
		m_nHASALPHAMASK = i ? 1 : 0;
#ifdef _DEBUG
		m_bHASALPHAMASK = true;
#endif
	}
private:
	int m_nHASSTATICTEXTURE;
#ifdef _DEBUG
	bool m_bHASSTATICTEXTURE;
#endif
public:
	void SetHASSTATICTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHASSTATICTEXTURE = i;
#ifdef _DEBUG
		m_bHASSTATICTEXTURE = true;
#endif
	}
	void SetHASSTATICTEXTURE( bool i )
	{
		Assert( ( i ? 1 : 0 ) >= 0 && ( i ? 1 : 0 ) <= 1 );
		m_nHASSTATICTEXTURE = i ? 1 : 0;
#ifdef _DEBUG
		m_bHASSTATICTEXTURE = true;
#endif
	}
private:
	int m_nPORTALGHOSTOVERLAY;
#ifdef _DEBUG
	bool m_bPORTALGHOSTOVERLAY;
#endif
public:
	void SetPORTALGHOSTOVERLAY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPORTALGHOSTOVERLAY = i;
#ifdef _DEBUG
		m_bPORTALGHOSTOVERLAY = true;
#endif
	}
	void SetPORTALGHOSTOVERLAY( bool i )
	{
		Assert( ( i ? 1 : 0 ) >= 0 && ( i ? 1 : 0 ) <= 1 );
		m_nPORTALGHOSTOVERLAY = i ? 1 : 0;
#ifdef _DEBUG
		m_bPORTALGHOSTOVERLAY = true;
#endif
	}
public:
	// CONSTRUCTOR
	portalstaticoverlay_ps20b_Static_Index( IShaderShadow *pShaderShadow, IMaterialVar **params )
	{
#ifdef _DEBUG
		m_bHASALPHAMASK = false;
#endif // _DEBUG
		m_nHASALPHAMASK = 0;
#ifdef _DEBUG
		m_bHASSTATICTEXTURE = false;
#endif // _DEBUG
		m_nHASSTATICTEXTURE = 0;
#ifdef _DEBUG
		m_bPORTALGHOSTOVERLAY = false;
#endif // _DEBUG
		m_nPORTALGHOSTOVERLAY = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllStaticVarsDefined = m_bHASALPHAMASK && m_bHASSTATICTEXTURE && m_bPORTALGHOSTOVERLAY;
		Assert( bAllStaticVarsDefined );
#endif // _DEBUG
		return ( 2 * m_nHASALPHAMASK ) + ( 4 * m_nHASSTATICTEXTURE ) + ( 8 * m_nPORTALGHOSTOVERLAY ) + 0;
	}
};
#define shaderStaticTest_portalstaticoverlay_ps20b psh_forgot_to_set_static_HASALPHAMASK + psh_forgot_to_set_static_HASSTATICTEXTURE + psh_forgot_to_set_static_PORTALGHOSTOVERLAY + 0
class portalstaticoverlay_ps20b_Dynamic_Index
{
private:
	int m_nPIXELFOGTYPE;
#ifdef _DEBUG
	bool m_bPIXELFOGTYPE;
#endif
public:
	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif
	}
	void SetPIXELFOGTYPE( bool i )
	{
		Assert( ( i ? 1 : 0 ) >= 0 && ( i ? 1 : 0 ) <= 1 );
		m_nPIXELFOGTYPE = i ? 1 : 0;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif
	}
public:
	// CONSTRUCTOR
	portalstaticoverlay_ps20b_Dynamic_Index( IShaderDynamicAPI *pShaderAPI )
	{
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif // _DEBUG
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bPIXELFOGTYPE;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nPIXELFOGTYPE ) + 0;
	}
};
#define shaderDynamicTest_portalstaticoverlay_ps20b 0