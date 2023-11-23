// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class portal_refract_vs20_Static_Index
{
	unsigned int m_nSTAGE : 2;
	unsigned int m_nUSEONSTATICPROP : 2;
#ifdef _DEBUG
	bool m_bSTAGE : 1;
	bool m_bUSEONSTATICPROP : 1;
#endif	// _DEBUG
public:
	void SetSTAGE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSTAGE = i;
#ifdef _DEBUG
		m_bSTAGE = true;
#endif	// _DEBUG
	}

	void SetUSEONSTATICPROP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUSEONSTATICPROP = i;
#ifdef _DEBUG
		m_bUSEONSTATICPROP = true;
#endif	// _DEBUG
	}

	portal_refract_vs20_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nSTAGE = 0;
		m_nUSEONSTATICPROP = 0;
#ifdef _DEBUG
		m_bSTAGE = false;
		m_bUSEONSTATICPROP = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSTAGE && m_bUSEONSTATICPROP );
		return ( 2 * m_nSTAGE ) + ( 6 * m_nUSEONSTATICPROP ) + 0;
	}
};

#define shaderStaticTest_portal_refract_vs20 vsh_forgot_to_set_static_STAGE + vsh_forgot_to_set_static_USEONSTATICPROP


class portal_refract_vs20_Dynamic_Index
{
	unsigned int m_nCOMPRESSED_VERTS : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSED_VERTS : 1;
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

	portal_refract_vs20_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCOMPRESSED_VERTS = 0;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSED_VERTS );
		return ( 1 * m_nCOMPRESSED_VERTS ) + 0;
	}
};

#define shaderDynamicTest_portal_refract_vs20 vsh_forgot_to_set_dynamic_COMPRESSED_VERTS

