// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $STAGE != 2 ) && ( $TINTED == 1 )
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class portal_refract_ps20b_Static_Index
{
	unsigned int m_nSTAGE : 2;
	unsigned int m_nTINTED : 2;
	unsigned int m_nSHADER_SRGB_READ : 2;
	unsigned int m_nSHADER_SRGB_READ : 1;
	unsigned int m_nDOPIXELFOG : 2;
#ifdef _DEBUG
	bool m_bSTAGE : 1;
	bool m_bTINTED : 1;
	bool m_bSHADER_SRGB_READ : 1;
	bool m_bSHADER_SRGB_READ : 1;
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

	void SetTINTED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTINTED = i;
#ifdef _DEBUG
		m_bTINTED = true;
#endif	// _DEBUG
	}

	void SetSHADER_SRGB_READ( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSHADER_SRGB_READ = i;
#ifdef _DEBUG
		m_bSHADER_SRGB_READ = true;
#endif	// _DEBUG
	}

	void SetSHADER_SRGB_READ( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nSHADER_SRGB_READ = i;
#ifdef _DEBUG
		m_bSHADER_SRGB_READ = true;
#endif	// _DEBUG
	}

	void SetDOPIXELFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDOPIXELFOG = i;
	}

	portal_refract_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nSTAGE = 0;
		m_nTINTED = 0;
		m_nSHADER_SRGB_READ = 0;
		m_nSHADER_SRGB_READ = 0;
		m_nDOPIXELFOG = !IS_FLAG_SET( MATERIAL_VAR_VERTEXFOG ) ? 1 : 0 ;
#ifdef _DEBUG
		m_bSTAGE = false;
		m_bTINTED = false;
		m_bSHADER_SRGB_READ = false;
		m_bSHADER_SRGB_READ = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSTAGE && m_bTINTED && m_bSHADER_SRGB_READ && m_bSHADER_SRGB_READ );
		AssertMsg( !( ( m_nSTAGE != 2 ) && ( m_nTINTED == 1 ) ), "Invalid combo combination ( ( STAGE != 2 ) && ( TINTED == 1 ) )" );
		return ( 4 * m_nSTAGE ) + ( 12 * m_nTINTED ) + ( 24 * m_nSHADER_SRGB_READ ) + ( 48 * m_nSHADER_SRGB_READ ) + ( 48 * m_nDOPIXELFOG ) + 0;
	}
};

#define shaderStaticTest_portal_refract_ps20b psh_forgot_to_set_static_STAGE + psh_forgot_to_set_static_TINTED + psh_forgot_to_set_static_SHADER_SRGB_READ + psh_forgot_to_set_static_SHADER_SRGB_READ


class portal_refract_ps20b_Dynamic_Index
{
	unsigned int m_nD_NVIDIA_STEREO : 2;
	unsigned int m_nD_NVIDIA_STEREO : 1;
	unsigned int m_nPIXELFOGTYPE : 2;
#ifdef _DEBUG
	bool m_bD_NVIDIA_STEREO : 1;
	bool m_bD_NVIDIA_STEREO : 1;
#endif	// _DEBUG
public:
	void SetD_NVIDIA_STEREO( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nD_NVIDIA_STEREO = i;
#ifdef _DEBUG
		m_bD_NVIDIA_STEREO = true;
#endif	// _DEBUG
	}

	void SetD_NVIDIA_STEREO( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nD_NVIDIA_STEREO = i;
#ifdef _DEBUG
		m_bD_NVIDIA_STEREO = true;
#endif	// _DEBUG
	}

	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
	}

	portal_refract_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nD_NVIDIA_STEREO = 0;
		m_nD_NVIDIA_STEREO = 0;
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
#ifdef _DEBUG
		m_bD_NVIDIA_STEREO = false;
		m_bD_NVIDIA_STEREO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bD_NVIDIA_STEREO && m_bD_NVIDIA_STEREO );
		return ( 1 * m_nD_NVIDIA_STEREO ) + ( 2 * m_nD_NVIDIA_STEREO ) + ( 2 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_portal_refract_ps20b psh_forgot_to_set_dynamic_D_NVIDIA_STEREO + psh_forgot_to_set_dynamic_D_NVIDIA_STEREO

