//====== Copyright © 1996-2003, Valve Corporation, All rights reserved. =======
//
// Purpose: Game rules for Scratch
//
//=============================================================================

#ifndef SDK_GAMERULES_H
#define SDK_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "gamerules.h"
#include "singleplay_gamerules.h"

#ifdef CLIENT_DLL
	#define CSDKGameRules C_SDKGameRules
	#define CSDKProxy C_SDKProxy
#endif // CLIENT_DLL

class CSDKProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CSDKProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
	DECLARE_DATADESC();
};

class CSDKGameRules : public CSingleplayRules
{
public:
	DECLARE_CLASS( CSDKGameRules, CSingleplayRules );

	virtual void LevelInitPostEntity() {}

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	virtual const char *GetGameDescription( void ) { return "SDK"; }

	virtual void PlayerThink( CBasePlayer *pPlayer ) {}

	virtual void PlayerSpawn( CBasePlayer *pPlayer );

	virtual void			InitDefaultAIRelationships( void );
#endif // CLIENT_DLL

	// misc
	virtual void CreateStandardEntities( void );	
};

//-----------------------------------------------------------------------------
// Gets us at the SDK game rules
//-----------------------------------------------------------------------------
inline CSDKGameRules* SDKGameRules()
{
	return static_cast<CSDKGameRules*>(g_pGameRules);
}



#endif // SDK_GAMERULES_H
