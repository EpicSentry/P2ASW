// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$ENVMAP && ( $BASEALPHAENVMAPMASK || $ENVMAPMASK )
// !$BASETEXTURE && $BASEALPHAENVMAPMASK
// $BASEALPHAENVMAPMASK && $ENVMAPMASK
// !$BASETEXTURE && $BASEALPHAENVMAPMASK
// $SELFILLUM && $BASEALPHAENVMAPMASK
// !$BASETEXTURE && $SELFILLUM

#pragma once
#include "shaderlib/cshader.h"
class SDK_lightmappedgeneric_ps20b_Static_Index
{
	unsigned int m_nBASETEXTURE : 2;
	unsigned int m_nENVMAP : 2;
	unsigned int m_nENVMAPMASK : 2;
	unsigned int m_nSELFILLUM : 2;
	unsigned int m_nBASEALPHAENVMAPMASK : 2;
#ifdef _DEBUG
	bool m_bBASETEXTURE : 1;
	bool m_bENVMAP : 1;
	bool m_bENVMAPMASK : 1;
	bool m_bSELFILLUM : 1;
	bool m_bBASEALPHAENVMAPMASK : 1;
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

	void SetENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAP = i;
#ifdef _DEBUG
		m_bENVMAP = true;
#endif	// _DEBUG
	}

	void SetENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPMASK = i;
#ifdef _DEBUG
		m_bENVMAPMASK = true;
#endif	// _DEBUG
	}

	void SetSELFILLUM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUM = i;
#ifdef _DEBUG
		m_bSELFILLUM = true;
#endif	// _DEBUG
	}

	void SetBASEALPHAENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASEALPHAENVMAPMASK = i;
#ifdef _DEBUG
		m_bBASEALPHAENVMAPMASK = true;
#endif	// _DEBUG
	}

	SDK_lightmappedgeneric_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nBASETEXTURE = 0;
		m_nENVMAP = 0;
		m_nENVMAPMASK = 0;
		m_nSELFILLUM = 0;
		m_nBASEALPHAENVMAPMASK = 0;
#ifdef _DEBUG
		m_bBASETEXTURE = false;
		m_bENVMAP = false;
		m_bENVMAPMASK = false;
		m_bSELFILLUM = false;
		m_bBASEALPHAENVMAPMASK = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBASETEXTURE && m_bENVMAP && m_bENVMAPMASK && m_bSELFILLUM && m_bBASEALPHAENVMAPMASK );
		AssertMsg( !( !m_nENVMAP && ( m_nBASEALPHAENVMAPMASK || m_nENVMAPMASK ) ), "Invalid combo combination ( !ENVMAP && ( BASEALPHAENVMAPMASK || ENVMAPMASK ) )" );
		AssertMsg( !( !m_nBASETEXTURE && m_nBASEALPHAENVMAPMASK ), "Invalid combo combination ( !BASETEXTURE && BASEALPHAENVMAPMASK )" );
		AssertMsg( !( m_nBASEALPHAENVMAPMASK && m_nENVMAPMASK ), "Invalid combo combination ( BASEALPHAENVMAPMASK && ENVMAPMASK )" );
		AssertMsg( !( !m_nBASETEXTURE && m_nBASEALPHAENVMAPMASK ), "Invalid combo combination ( !BASETEXTURE && BASEALPHAENVMAPMASK )" );
		AssertMsg( !( m_nSELFILLUM && m_nBASEALPHAENVMAPMASK ), "Invalid combo combination ( SELFILLUM && BASEALPHAENVMAPMASK )" );
		AssertMsg( !( !m_nBASETEXTURE && m_nSELFILLUM ), "Invalid combo combination ( !BASETEXTURE && SELFILLUM )" );
		return ( 1 * m_nBASETEXTURE ) + ( 2 * m_nENVMAP ) + ( 4 * m_nENVMAPMASK ) + ( 8 * m_nSELFILLUM ) + ( 16 * m_nBASEALPHAENVMAPMASK ) + 0;
	}
};

#define shaderStaticTest_SDK_lightmappedgeneric_ps20b psh_forgot_to_set_static_BASETEXTURE + psh_forgot_to_set_static_ENVMAP + psh_forgot_to_set_static_ENVMAPMASK + psh_forgot_to_set_static_SELFILLUM + psh_forgot_to_set_static_BASEALPHAENVMAPMASK


class SDK_lightmappedgeneric_ps20b_Dynamic_Index
{
public:
	SDK_lightmappedgeneric_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_SDK_lightmappedgeneric_ps20b 1

