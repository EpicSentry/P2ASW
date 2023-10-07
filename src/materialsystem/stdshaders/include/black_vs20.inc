// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $DOWATERFOG == 0 ) && ( $DOPIXELFOG && $HARDWAREFOGBLEND )
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class black_vs20_Static_Index
{
	unsigned int m_nDOPIXELFOG : 1;
	unsigned int m_nHARDWAREFOGBLEND : 2;
public:
	void SetDOPIXELFOG( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nDOPIXELFOG = i;
	}

	void SetHARDWAREFOGBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHARDWAREFOGBLEND = i;
	}

	black_vs20_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nDOPIXELFOG = 0;
		m_nHARDWAREFOGBLEND = !g_pHardwareConfig->SupportsPixelShaders_2_b() ;
	}

	int GetIndex() const
	{
		return ( 2 * m_nDOPIXELFOG ) + ( 2 * m_nHARDWAREFOGBLEND ) + 0;
	}
};

#define shaderStaticTest_black_vs20 1


class black_vs20_Dynamic_Index
{
	unsigned int m_nDOWATERFOG : 1;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetDOWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nDOWATERFOG = i;
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	black_vs20_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nDOWATERFOG = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSKINNING );
		return ( 1 * m_nDOWATERFOG ) + ( 1 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_black_vs20 vsh_forgot_to_set_dynamic_SKINNING

