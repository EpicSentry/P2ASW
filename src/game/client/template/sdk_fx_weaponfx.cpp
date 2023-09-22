//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Game-specific impact effect hooks
//
//=============================================================================//
#include "cbase.h"
#include "fx_impact.h"
#include "tempent.h"
#include "c_te_effect_dispatch.h"
#include "c_te_legacytempents.h"


//-----------------------------------------------------------------------------
// Purpose: Handle weapon effect callbacks
//-----------------------------------------------------------------------------
void SDK_EjectBrass( int shell, const CEffectData &data )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if( !pPlayer )
		return;

	model_t *pModel = NULL;
	switch( shell )
	{
		case CS_SHELL_9MM:
			pModel = (model_t *) engine->LoadModel( "models/weapons/shell.mdl" );
			break;
		case CS_SHELL_57:
			pModel = (model_t *) engine->LoadModel( "models/weapons/shell.mdl" );
			break;
		case CS_SHELL_12GAUGE:
			pModel = (model_t *) engine->LoadModel( "models/weapons/shell.mdl" );
			break;
	}
	if( !pModel )
		return;

	Vector	dir;
	AngleVectors( data.m_vAngles, &dir );
	dir *= random->RandomFloat( 150.0f, 200.0f );

	Vector velocity(dir[0] + random->RandomFloat(-64,64),
						dir[1] + random->RandomFloat(-64,64),
						dir[2] + random->RandomFloat(  0,64) );

	C_LocalTempEntity *pBrass = tempents->SpawnTempModel( pModel, data.m_vOrigin, data.m_vAngles, velocity,
		1.0f + random->RandomFloat( 0.0f, 1.0f ), FTENT_COLLIDEWORLD | FTENT_FADEOUT | FTENT_GRAVITY | FTENT_ROTATE);

	//Keep track of shell type
	if ( shell == 2 )
	{
		pBrass->hitSound = BOUNCE_SHOTSHELL;
	}
	else
	{
		pBrass->hitSound = BOUNCE_SHELL;
	}
}

void SDK_FX_EjectBrass_9mm_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_9MM, data );
}

void SDK_FX_EjectBrass_12Gauge_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_12GAUGE, data );
}

void SDK_FX_EjectBrass_57_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_57, data );
}

void SDK_FX_EjectBrass_556_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_556, data );
}

void SDK_FX_EjectBrass_762Nato_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_762NATO, data );
}

void SDK_FX_EjectBrass_338Mag_Callback( const CEffectData &data )
{
	SDK_EjectBrass( CS_SHELL_338MAG, data );
}


DECLARE_CLIENT_EFFECT( EjectBrass_9mm,		SDK_FX_EjectBrass_9mm_Callback );
DECLARE_CLIENT_EFFECT( EjectBrass_12Gauge,	SDK_FX_EjectBrass_12Gauge_Callback );
DECLARE_CLIENT_EFFECT( EjectBrass_57,			SDK_FX_EjectBrass_57_Callback );
DECLARE_CLIENT_EFFECT( EjectBrass_556,		SDK_FX_EjectBrass_556_Callback );
DECLARE_CLIENT_EFFECT( EjectBrass_762Nato,	SDK_FX_EjectBrass_762Nato_Callback );
DECLARE_CLIENT_EFFECT( EjectBrass_338Mag,		SDK_FX_EjectBrass_338Mag_Callback );
