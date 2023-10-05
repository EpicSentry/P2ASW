// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( ( $DETAIL1 == 0 ) && ( $DETAIL2 != 0 ) )
// ( ( $DETAIL1 == 0 ) && ( $DETAIL1BLENDMODE != 0 ) )
// ( ( $DETAIL2 == 0 ) && ( $DETAIL2BLENDMODE != 0 ) )
// ( $TANGENTTOPACITY && $TANGENTSOPACITY )
// ( $FRESNELOPACITY ) && ( $TANGENTTOPACITY || $TANGENTSOPACITY )
// ( $FLOWMAP && ( $DETAIL1 || $DETAIL2 ) )
// ( $FLOW_CHEAP && !$FLOWMAP )
// ( ( $FLOWMAP == 0 ) && ( $VORTEX1 || $VORTEX2 || $POWERUP ) )
// ( ( $ACTIVE == 0 ) && ( $VORTEX1 || $VORTEX2 || $POWERUP ) )
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class solidenergy_ps20b_Static_Index
{
	unsigned int m_nADDITIVE : 2;
	unsigned int m_nDETAIL1 : 2;
	unsigned int m_nDETAIL1BLENDMODE : 2;
	unsigned int m_nDETAIL2 : 2;
	unsigned int m_nDETAIL2BLENDMODE : 2;
	unsigned int m_nTANGENTTOPACITY : 2;
	unsigned int m_nTANGENTSOPACITY : 2;
	unsigned int m_nFRESNELOPACITY : 2;
	unsigned int m_nDEPTHBLEND : 2;
	unsigned int m_nDEPTHBLEND : 1;
	unsigned int m_nVERTEXCOLOR : 2;
	unsigned int m_nFLOWMAP : 2;
	unsigned int m_nFLOW_CHEAP : 2;
	unsigned int m_nFLOW_CHEAP : 1;
#ifdef _DEBUG
	bool m_bADDITIVE : 1;
	bool m_bDETAIL1 : 1;
	bool m_bDETAIL1BLENDMODE : 1;
	bool m_bDETAIL2 : 1;
	bool m_bDETAIL2BLENDMODE : 1;
	bool m_bTANGENTTOPACITY : 1;
	bool m_bTANGENTSOPACITY : 1;
	bool m_bFRESNELOPACITY : 1;
	bool m_bDEPTHBLEND : 1;
	bool m_bDEPTHBLEND : 1;
	bool m_bVERTEXCOLOR : 1;
	bool m_bFLOWMAP : 1;
	bool m_bFLOW_CHEAP : 1;
	bool m_bFLOW_CHEAP : 1;
#endif	// _DEBUG
public:
	void SetADDITIVE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nADDITIVE = i;
#ifdef _DEBUG
		m_bADDITIVE = true;
#endif	// _DEBUG
	}

	void SetDETAIL1( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAIL1 = i;
#ifdef _DEBUG
		m_bDETAIL1 = true;
#endif	// _DEBUG
	}

	void SetDETAIL1BLENDMODE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAIL1BLENDMODE = i;
#ifdef _DEBUG
		m_bDETAIL1BLENDMODE = true;
#endif	// _DEBUG
	}

	void SetDETAIL2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAIL2 = i;
#ifdef _DEBUG
		m_bDETAIL2 = true;
#endif	// _DEBUG
	}

	void SetDETAIL2BLENDMODE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAIL2BLENDMODE = i;
#ifdef _DEBUG
		m_bDETAIL2BLENDMODE = true;
#endif	// _DEBUG
	}

	void SetTANGENTTOPACITY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTTOPACITY = i;
#ifdef _DEBUG
		m_bTANGENTTOPACITY = true;
#endif	// _DEBUG
	}

	void SetTANGENTSOPACITY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTSOPACITY = i;
#ifdef _DEBUG
		m_bTANGENTSOPACITY = true;
#endif	// _DEBUG
	}

	void SetFRESNELOPACITY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFRESNELOPACITY = i;
#ifdef _DEBUG
		m_bFRESNELOPACITY = true;
#endif	// _DEBUG
	}

	void SetDEPTHBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDEPTHBLEND = i;
#ifdef _DEBUG
		m_bDEPTHBLEND = true;
#endif	// _DEBUG
	}

	void SetDEPTHBLEND( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nDEPTHBLEND = i;
#ifdef _DEBUG
		m_bDEPTHBLEND = true;
#endif	// _DEBUG
	}

	void SetVERTEXCOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLOR = i;
#ifdef _DEBUG
		m_bVERTEXCOLOR = true;
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

	void SetFLOW_CHEAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLOW_CHEAP = i;
#ifdef _DEBUG
		m_bFLOW_CHEAP = true;
#endif	// _DEBUG
	}

	void SetFLOW_CHEAP( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nFLOW_CHEAP = i;
#ifdef _DEBUG
		m_bFLOW_CHEAP = true;
#endif	// _DEBUG
	}

	solidenergy_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nADDITIVE = 0;
		m_nDETAIL1 = 0;
		m_nDETAIL1BLENDMODE = 0;
		m_nDETAIL2 = 0;
		m_nDETAIL2BLENDMODE = 0;
		m_nTANGENTTOPACITY = 0;
		m_nTANGENTSOPACITY = 0;
		m_nFRESNELOPACITY = 0;
		m_nDEPTHBLEND = 0;
		m_nDEPTHBLEND = 0;
		m_nVERTEXCOLOR = 0;
		m_nFLOWMAP = 0;
		m_nFLOW_CHEAP = 0;
		m_nFLOW_CHEAP = 0;
#ifdef _DEBUG
		m_bADDITIVE = false;
		m_bDETAIL1 = false;
		m_bDETAIL1BLENDMODE = false;
		m_bDETAIL2 = false;
		m_bDETAIL2BLENDMODE = false;
		m_bTANGENTTOPACITY = false;
		m_bTANGENTSOPACITY = false;
		m_bFRESNELOPACITY = false;
		m_bDEPTHBLEND = false;
		m_bDEPTHBLEND = false;
		m_bVERTEXCOLOR = false;
		m_bFLOWMAP = false;
		m_bFLOW_CHEAP = false;
		m_bFLOW_CHEAP = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bADDITIVE && m_bDETAIL1 && m_bDETAIL1BLENDMODE && m_bDETAIL2 && m_bDETAIL2BLENDMODE && m_bTANGENTTOPACITY && m_bTANGENTSOPACITY && m_bFRESNELOPACITY && m_bDEPTHBLEND && m_bDEPTHBLEND && m_bVERTEXCOLOR && m_bFLOWMAP && m_bFLOW_CHEAP && m_bFLOW_CHEAP );
		AssertMsg( !( ( m_nDETAIL1 == 0 ) && ( m_nDETAIL2 != 0 ) ), "Invalid combo combination ( ( DETAIL1 == 0 ) && ( DETAIL2 != 0 ) )" );
		AssertMsg( !( ( m_nDETAIL1 == 0 ) && ( m_nDETAIL1BLENDMODE != 0 ) ), "Invalid combo combination ( ( DETAIL1 == 0 ) && ( DETAIL1BLENDMODE != 0 ) )" );
		AssertMsg( !( ( m_nDETAIL2 == 0 ) && ( m_nDETAIL2BLENDMODE != 0 ) ), "Invalid combo combination ( ( DETAIL2 == 0 ) && ( DETAIL2BLENDMODE != 0 ) )" );
		AssertMsg( !( m_nTANGENTTOPACITY && m_nTANGENTSOPACITY ), "Invalid combo combination ( TANGENTTOPACITY && TANGENTSOPACITY )" );
		AssertMsg( !( m_nFRESNELOPACITY && ( m_nTANGENTTOPACITY || m_nTANGENTSOPACITY ) ), "Invalid combo combination ( FRESNELOPACITY && ( TANGENTTOPACITY || TANGENTSOPACITY ) )" );
		AssertMsg( !( m_nFLOWMAP && ( m_nDETAIL1 || m_nDETAIL2 ) ), "Invalid combo combination ( FLOWMAP && ( DETAIL1 || DETAIL2 ) )" );
		AssertMsg( !( m_nFLOW_CHEAP && !m_nFLOWMAP ), "Invalid combo combination ( FLOW_CHEAP && !FLOWMAP )" );
		return ( 32 * m_nADDITIVE ) + ( 64 * m_nDETAIL1 ) + ( 128 * m_nDETAIL1BLENDMODE ) + ( 256 * m_nDETAIL2 ) + ( 512 * m_nDETAIL2BLENDMODE ) + ( 1024 * m_nTANGENTTOPACITY ) + ( 2048 * m_nTANGENTSOPACITY ) + ( 4096 * m_nFRESNELOPACITY ) + ( 8192 * m_nDEPTHBLEND ) + ( 16384 * m_nDEPTHBLEND ) + ( 16384 * m_nVERTEXCOLOR ) + ( 32768 * m_nFLOWMAP ) + ( 65536 * m_nFLOW_CHEAP ) + ( 131072 * m_nFLOW_CHEAP ) + 0;
	}
};

#define shaderStaticTest_solidenergy_ps20b psh_forgot_to_set_static_ADDITIVE + psh_forgot_to_set_static_DETAIL1 + psh_forgot_to_set_static_DETAIL1BLENDMODE + psh_forgot_to_set_static_DETAIL2 + psh_forgot_to_set_static_DETAIL2BLENDMODE + psh_forgot_to_set_static_TANGENTTOPACITY + psh_forgot_to_set_static_TANGENTSOPACITY + psh_forgot_to_set_static_FRESNELOPACITY + psh_forgot_to_set_static_DEPTHBLEND + psh_forgot_to_set_static_DEPTHBLEND + psh_forgot_to_set_static_VERTEXCOLOR + psh_forgot_to_set_static_FLOWMAP + psh_forgot_to_set_static_FLOW_CHEAP + psh_forgot_to_set_static_FLOW_CHEAP


class solidenergy_ps20b_Dynamic_Index
{
	unsigned int m_nACTIVE : 2;
	unsigned int m_nPOWERUP : 2;
	unsigned int m_nVORTEX1 : 2;
	unsigned int m_nVORTEX2 : 2;
	unsigned int m_nPIXELFOGTYPE : 2;
#ifdef _DEBUG
	bool m_bACTIVE : 1;
	bool m_bPOWERUP : 1;
	bool m_bVORTEX1 : 1;
	bool m_bVORTEX2 : 1;
#endif	// _DEBUG
public:
	void SetACTIVE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nACTIVE = i;
#ifdef _DEBUG
		m_bACTIVE = true;
#endif	// _DEBUG
	}

	void SetPOWERUP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPOWERUP = i;
#ifdef _DEBUG
		m_bPOWERUP = true;
#endif	// _DEBUG
	}

	void SetVORTEX1( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVORTEX1 = i;
#ifdef _DEBUG
		m_bVORTEX1 = true;
#endif	// _DEBUG
	}

	void SetVORTEX2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVORTEX2 = i;
#ifdef _DEBUG
		m_bVORTEX2 = true;
#endif	// _DEBUG
	}

	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
	}

	solidenergy_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nACTIVE = 0;
		m_nPOWERUP = 0;
		m_nVORTEX1 = 0;
		m_nVORTEX2 = 0;
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
#ifdef _DEBUG
		m_bACTIVE = false;
		m_bPOWERUP = false;
		m_bVORTEX1 = false;
		m_bVORTEX2 = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bACTIVE && m_bPOWERUP && m_bVORTEX1 && m_bVORTEX2 );
		AssertMsg( !( ( m_nACTIVE == 0 ) && ( m_nVORTEX1 || ( m_nVORTEX2 || m_nPOWERUP ) ) ), "Invalid combo combination ( ( ACTIVE == 0 ) && ( VORTEX1 || ( VORTEX2 || POWERUP ) ) )" );
		return ( 1 * m_nACTIVE ) + ( 2 * m_nPOWERUP ) + ( 4 * m_nVORTEX1 ) + ( 8 * m_nVORTEX2 ) + ( 16 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_solidenergy_ps20b psh_forgot_to_set_dynamic_ACTIVE + psh_forgot_to_set_dynamic_POWERUP + psh_forgot_to_set_dynamic_VORTEX1 + psh_forgot_to_set_dynamic_VORTEX2

