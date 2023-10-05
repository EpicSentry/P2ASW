// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( ( $DETAIL1 == 0 ) && ( $DETAIL2 != 0 ) )
// ( $TANGENTTOPACITY && $TANGENTSOPACITY )
// ( $FLOWMAP && ( $DETAIL1 || $DETAIL2 ) )
// ( ( $FLOWMAP == 0 ) && ( $VORTEX1 || $VORTEX2 ) )
// ( $DOWATERFOG == 0 ) && ( $DOPIXELFOG && $HARDWAREFOGBLEND )
// ( $DOWATERFOG == 0 ) && ( $HARDWAREFOGBLEND == 0 ) && ( $DOPIXELFOG == 0 )
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class solidenergy_vs20_Static_Index
{
	unsigned int m_nDETAIL1 : 2;
	unsigned int m_nDETAIL2 : 2;
	unsigned int m_nTANGENTTOPACITY : 2;
	unsigned int m_nTANGENTSOPACITY : 2;
	unsigned int m_nFRESNELOPACITY : 2;
	unsigned int m_nVERTEXCOLOR : 2;
	unsigned int m_nFLOWMAP : 2;
	unsigned int m_nMODELFORMAT : 2;
	unsigned int m_nDOPIXELFOG : 2;
	unsigned int m_nHARDWAREFOGBLEND : 2;
#ifdef _DEBUG
	bool m_bDETAIL1 : 1;
	bool m_bDETAIL2 : 1;
	bool m_bTANGENTTOPACITY : 1;
	bool m_bTANGENTSOPACITY : 1;
	bool m_bFRESNELOPACITY : 1;
	bool m_bVERTEXCOLOR : 1;
	bool m_bFLOWMAP : 1;
	bool m_bMODELFORMAT : 1;
#endif	// _DEBUG
public:
	void SetDETAIL1( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAIL1 = i;
#ifdef _DEBUG
		m_bDETAIL1 = true;
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

	void SetMODELFORMAT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMODELFORMAT = i;
#ifdef _DEBUG
		m_bMODELFORMAT = true;
#endif	// _DEBUG
	}

	void SetDOPIXELFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDOPIXELFOG = i;
	}

	void SetHARDWAREFOGBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHARDWAREFOGBLEND = i;
	}

	solidenergy_vs20_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nDETAIL1 = 0;
		m_nDETAIL2 = 0;
		m_nTANGENTTOPACITY = 0;
		m_nTANGENTSOPACITY = 0;
		m_nFRESNELOPACITY = 0;
		m_nVERTEXCOLOR = 0;
		m_nFLOWMAP = 0;
		m_nMODELFORMAT = 0;
		m_nDOPIXELFOG = g_pHardwareConfig->SupportsPixelShaders_2_b() ;
		m_nHARDWAREFOGBLEND = !g_pHardwareConfig->SupportsPixelShaders_2_b() ;
#ifdef _DEBUG
		m_bDETAIL1 = false;
		m_bDETAIL2 = false;
		m_bTANGENTTOPACITY = false;
		m_bTANGENTSOPACITY = false;
		m_bFRESNELOPACITY = false;
		m_bVERTEXCOLOR = false;
		m_bFLOWMAP = false;
		m_bMODELFORMAT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDETAIL1 && m_bDETAIL2 && m_bTANGENTTOPACITY && m_bTANGENTSOPACITY && m_bFRESNELOPACITY && m_bVERTEXCOLOR && m_bFLOWMAP && m_bMODELFORMAT );
		AssertMsg( !( ( m_nDETAIL1 == 0 ) && ( m_nDETAIL2 != 0 ) ), "Invalid combo combination ( ( DETAIL1 == 0 ) && ( DETAIL2 != 0 ) )" );
		AssertMsg( !( m_nTANGENTTOPACITY && m_nTANGENTSOPACITY ), "Invalid combo combination ( TANGENTTOPACITY && TANGENTSOPACITY )" );
		AssertMsg( !( m_nFLOWMAP && ( m_nDETAIL1 || m_nDETAIL2 ) ), "Invalid combo combination ( FLOWMAP && ( DETAIL1 || DETAIL2 ) )" );
		return ( 32 * m_nDETAIL1 ) + ( 64 * m_nDETAIL2 ) + ( 128 * m_nTANGENTTOPACITY ) + ( 256 * m_nTANGENTSOPACITY ) + ( 512 * m_nFRESNELOPACITY ) + ( 1024 * m_nVERTEXCOLOR ) + ( 2048 * m_nFLOWMAP ) + ( 4096 * m_nMODELFORMAT ) + ( 8192 * m_nDOPIXELFOG ) + ( 16384 * m_nHARDWAREFOGBLEND ) + 0;
	}
};

#define shaderStaticTest_solidenergy_vs20 vsh_forgot_to_set_static_DETAIL1 + vsh_forgot_to_set_static_DETAIL2 + vsh_forgot_to_set_static_TANGENTTOPACITY + vsh_forgot_to_set_static_TANGENTSOPACITY + vsh_forgot_to_set_static_FRESNELOPACITY + vsh_forgot_to_set_static_VERTEXCOLOR + vsh_forgot_to_set_static_FLOWMAP + vsh_forgot_to_set_static_MODELFORMAT


class solidenergy_vs20_Dynamic_Index
{
	unsigned int m_nCOMPRESSED_VERTS : 2;
	unsigned int m_nSKINNING : 2;
	unsigned int m_nVORTEX1 : 2;
	unsigned int m_nVORTEX2 : 2;
	unsigned int m_nDOWATERFOG : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSED_VERTS : 1;
	bool m_bSKINNING : 1;
	bool m_bVORTEX1 : 1;
	bool m_bVORTEX2 : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSED_VERTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSED_VERTS = i;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
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

	void SetDOWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDOWATERFOG = i;
	}

	solidenergy_vs20_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCOMPRESSED_VERTS = 0;
		m_nSKINNING = 0;
		m_nVORTEX1 = 0;
		m_nVORTEX2 = 0;
		m_nDOWATERFOG = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = false;
		m_bSKINNING = false;
		m_bVORTEX1 = false;
		m_bVORTEX2 = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSED_VERTS && m_bSKINNING && m_bVORTEX1 && m_bVORTEX2 );
		return ( 1 * m_nCOMPRESSED_VERTS ) + ( 2 * m_nSKINNING ) + ( 4 * m_nVORTEX1 ) + ( 8 * m_nVORTEX2 ) + ( 16 * m_nDOWATERFOG ) + 0;
	}
};

#define shaderDynamicTest_solidenergy_vs20 vsh_forgot_to_set_dynamic_COMPRESSED_VERTS + vsh_forgot_to_set_dynamic_SKINNING + vsh_forgot_to_set_dynamic_VORTEX1 + vsh_forgot_to_set_dynamic_VORTEX2

