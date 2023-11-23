// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class portalstaticoverlay_vs20_Static_Index
{
	unsigned int m_nMODEL : 2;
	unsigned int m_nPORTALGHOSTOVERLAY : 2;
#ifdef _DEBUG
	bool m_bMODEL : 1;
	bool m_bPORTALGHOSTOVERLAY : 1;
#endif	// _DEBUG
public:
	void SetMODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMODEL = i;
#ifdef _DEBUG
		m_bMODEL = true;
#endif	// _DEBUG
	}

	void SetPORTALGHOSTOVERLAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nPORTALGHOSTOVERLAY = i;
#ifdef _DEBUG
		m_bPORTALGHOSTOVERLAY = true;
#endif	// _DEBUG
	}

	portalstaticoverlay_vs20_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nMODEL = 0;
		m_nPORTALGHOSTOVERLAY = 0;
#ifdef _DEBUG
		m_bMODEL = false;
		m_bPORTALGHOSTOVERLAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMODEL && m_bPORTALGHOSTOVERLAY );
		return ( 2 * m_nMODEL ) + ( 4 * m_nPORTALGHOSTOVERLAY ) + 0;
	}
};

#define shaderStaticTest_portalstaticoverlay_vs20 vsh_forgot_to_set_static_MODEL + vsh_forgot_to_set_static_PORTALGHOSTOVERLAY


class portalstaticoverlay_vs20_Dynamic_Index
{
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	portalstaticoverlay_vs20_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSKINNING );
		return ( 1 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_portalstaticoverlay_vs20 vsh_forgot_to_set_dynamic_SKINNING

