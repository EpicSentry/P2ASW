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
class portal_ps20b_Static_Index
{
	unsigned int m_nHASALPHAMASK : 2;
	unsigned int m_nHASSTATICTEXTURE : 2;
#ifdef _DEBUG
	bool m_bHASALPHAMASK : 1;
	bool m_bHASSTATICTEXTURE : 1;
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

	portal_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nHASALPHAMASK = 0;
		m_nHASSTATICTEXTURE = 0;
#ifdef _DEBUG
		m_bHASALPHAMASK = false;
		m_bHASSTATICTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bHASALPHAMASK && m_bHASSTATICTEXTURE );
		return ( 8 * m_nHASALPHAMASK ) + ( 16 * m_nHASSTATICTEXTURE ) + 0;
	}
};

#define shaderStaticTest_portal_ps20b psh_forgot_to_set_static_HASALPHAMASK + psh_forgot_to_set_static_HASSTATICTEXTURE


class portal_ps20b_Dynamic_Index
{
	unsigned int m_nADDSTATIC : 2;
	unsigned int m_nD_NVIDIA_STEREO : 2;
	unsigned int m_nD_NVIDIA_STEREO : 1;
	unsigned int m_nPIXELFOGTYPE : 2;
#ifdef _DEBUG
	bool m_bADDSTATIC : 1;
	bool m_bD_NVIDIA_STEREO : 1;
	bool m_bD_NVIDIA_STEREO : 1;
#endif	// _DEBUG
public:
	void SetADDSTATIC( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nADDSTATIC = i;
#ifdef _DEBUG
		m_bADDSTATIC = true;
#endif	// _DEBUG
	}

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

	portal_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nADDSTATIC = 0;
		m_nD_NVIDIA_STEREO = 0;
		m_nD_NVIDIA_STEREO = 0;
		m_nPIXELFOGTYPE = ( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ;
#ifdef _DEBUG
		m_bADDSTATIC = false;
		m_bD_NVIDIA_STEREO = false;
		m_bD_NVIDIA_STEREO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bADDSTATIC && m_bD_NVIDIA_STEREO && m_bD_NVIDIA_STEREO );
		return ( 1 * m_nADDSTATIC ) + ( 2 * m_nD_NVIDIA_STEREO ) + ( 4 * m_nD_NVIDIA_STEREO ) + ( 4 * m_nPIXELFOGTYPE ) + 0;
	}
};

#define shaderDynamicTest_portal_ps20b psh_forgot_to_set_dynamic_ADDSTATIC + psh_forgot_to_set_dynamic_D_NVIDIA_STEREO + psh_forgot_to_set_dynamic_D_NVIDIA_STEREO

