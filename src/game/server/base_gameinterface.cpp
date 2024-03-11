//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gameinterface.h"
#include "mapentities.h"
#include "fmtstr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sv_force_transmit_ents;

#ifdef PORTAL2
extern ConVar sv_portal_players;
extern ConVar cm_is_current_community_map_coop;

ConVar mp_dev_gamemode("mp_dev_gamemode", "", FCVAR_DEVELOPMENTONLY);
#endif

void CServerGameClients::GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers ) const
{
#ifdef PORTAL2
	minplayers = defaultMaxPlayers = 1;
	maxplayers = CommandLine()->FindParm( "-allowspectators" ) ? 3 : 2;
#else
	minplayers = defaultMaxPlayers = 1; 
	maxplayers = MAX_PLAYERS;
#endif
}


// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameDLL implementation.
// -------------------------------------------------------------------------------------------- //

void CServerGameDLL::LevelInit_ParseAllEntities( const char *pMapEntities )
{
}

//-----------------------------------------------------------------------------
// Purpose: Called to apply lobby settings to a dedicated server
//-----------------------------------------------------------------------------
void CServerGameDLL::ApplyGameSettings( KeyValues *pKV )
{
	if ( !pKV )
		return;

	DevMsg("\nApplyGameSettings() called.\nSession settings:\n\n");
	KeyValuesDumpAsDevMsg( pKV );
	DevMsg( "\n" );
	
	// NOTE: I am assuming all code not present in Swarm SDK was originally under a #ifdef PORTAL2
#ifdef PORTAL2
	char modDir[MAX_PATH];
	UTIL_GetModDir(modDir, MAX_PATH);
#endif

	// Fix the game settings request when a generic request for
	// map launch comes in (it might be nested under reservation
	// processing)
	bool bAlreadyLoadingMap = false;
	char const *szBspName = NULL;
	if ( !Q_stricmp( pKV->GetName(), "::ExecGameTypeCfg" ) )
	{
		if ( !engine )
			return;

		char const *szNewMap = pKV->GetString( "map/mapname", "" );
		if ( !szNewMap || !*szNewMap )
			return;

		KeyValues *pLaunchOptions = engine->GetLaunchOptions();
		if ( !pLaunchOptions )
			return;

		if ( FindLaunchOptionByValue( pLaunchOptions, "changelevel" ) ||
			FindLaunchOptionByValue( pLaunchOptions, "changelevel2" ) )
			return;

		if ( FindLaunchOptionByValue( pLaunchOptions, "map_background" ) )
		{
#ifdef PORTAL2
			// Seems like this is supposed to set max players to 1 for background maps..?
			// Decompilers (both IDA and Ghidra) showed CCollisionProperty::TestCollision()
			// being called here, but that's completely nonsensical in this context and seems wrong
			sv_portal_players.SetValue(1);

			ConVarRef coop("coop");
			if (coop.IsValid())
			{
				coop.SetValue(0);
			}

			pKV->SetInt("members/numSlots", 1);
#endif
			return;
		}

		bAlreadyLoadingMap = true;

		if ( FindLaunchOptionByValue( pLaunchOptions, "reserved" ) )
		{
			// We are already reserved, but we still need to let the engine
			// baseserver know how many human slots to allocate
			int numSlots = 1;

#ifdef PORTAL2
			numSlots = sv_portal_players.GetInt();
#endif

			pKV->SetInt( "members/numSlots", numSlots );
			return;
		}

#ifdef PORTAL2
		int numSlotsPortal = 1;

		if (FindLaunchOptionByValue( pLaunchOptions, "*mp" )
			 || !FindLaunchOptionByValue( pLaunchOptions, "*sp" )
			 && (V_stristr( szNewMap, "mp_coop_" ) || cm_is_current_community_map_coop.GetBool()))
		{
			numSlotsPortal = CommandLine()->FindParm("-allowspectators") ? 3 : 2;
		}
		else
		{
			numSlotsPortal = 1;
		}

		pKV->SetInt( "members/numSlots", numSlotsPortal );
		DevMsg("  Adjusting game players %d.\n", numSlotsPortal);
		pKV->SetName(modDir);
#endif
	}

#ifdef PORTAL2
	if ( V_stricmp( pKV->GetName(), modDir ) || bAlreadyLoadingMap )
		return;

	int numSlots = pKV->GetInt("members/numSlots", 0);
	if (numSlots)
	{
		sv_portal_players.SetValue(numSlots);
		ConVarRef coop_ref("coop");
		if (coop_ref.IsValid())
		{
			coop_ref.SetValue(numSlots >= 2 ? 1 : 0);
		}
		DevMsg("            game players = %d\n", numSlots);
	}

	// Promo items
	int promoSkins = pKV->GetInt("gameextras/skins", 0);
	if (promoSkins)
	{
		const char* szStatus = (promoSkins <= 0 ? "disabled" : "enabled");
		DevMsg("            extras skins = %s\n", szStatus);

		if ( promoSkins <= 0 )
			g_nPortal2PromoFlags &= ~1u;
		else
			g_nPortal2PromoFlags |= 1u;
	}
	int promoHelmets = pKV->GetInt("gameextras/helmets", 0);
	if (promoHelmets)
	{
		const char* szStatus = (promoHelmets <= 0 ? "disabled" : "enabled");
		DevMsg("            extras helmets = %s\n", szStatus);

		if ( promoHelmets <= 0 )
			g_nPortal2PromoFlags &= ~2u;
		else
			g_nPortal2PromoFlags |= 2u;
	}
	int promoAntenna = pKV->GetInt("gameextras/antenna", 0);
	if (promoAntenna)
	{
		const char* szStatus = (promoAntenna <= 0 ? "disabled" : "enabled");
		DevMsg("            extras antenna = %s\n", szStatus);

		if ( promoAntenna <= 0 )
			g_nPortal2PromoFlags &= ~4u;
		else
			g_nPortal2PromoFlags |= 4u;
	}

	// Challenge mode handling
	ConVarRef sv_bonus_challenge("sv_bonus_challenge");
	if (sv_bonus_challenge.IsValid())
	{
		char const* szGameMode = pKV->GetString("game/mode", "");
		if (!szGameMode || !*szGameMode)
		{
			szGameMode = mp_dev_gamemode.GetString();
		}

		bool bIsChallenge = false;
		if (!V_stricmp( szGameMode, "sp" ))
		{
			const char* playType = pKV->GetString("options/play", "");
			bIsChallenge = !V_stricmp( playType, "challenge" );
		}
		else
		{
			bIsChallenge = !V_stricmp( szGameMode, "coop_challenge" );
		}
		sv_bonus_challenge.SetValue(bIsChallenge);
	}

	// Has to be "game/mission" in Swarm - "game/map" is not passed through to this code
	szBspName = pKV->GetString("game/mission", "");
	
#endif


	//
	// Change map as the last command
	//
	if ( szBspName && szBspName[0] && !bAlreadyLoadingMap )
	{
		// Vitaliy: Disable cheats as part of reservation in case they were enabled (unless we are on Steam Beta)
		if ( sv_force_transmit_ents.IsFlagSet( FCVAR_DEVELOPMENTONLY ) &&	// any convar with FCVAR_DEVELOPMENTONLY flag will be sufficient here
			sv_cheats && sv_cheats->GetBool() )
		{
			sv_cheats->SetValue( 0 );
		}

#ifdef PORTAL2
		ConVarRef map_wants_save_disable("map_wants_save_disable");
		map_wants_save_disable.SetValue(0);

		// handle loading savegames
		// "game/save" is not passed through either, so "game/campaign" is used instead
		const char* szSaveName = pKV->GetString("game/campaign", 0);

		// Ignore if the save name is "jacob", this is the default value set by Swarm matchmaking.dll if none is specified
		// (if the user actually has a save with this name, it will be passed in as "jacob.sav")
		if (szSaveName && V_stricmp(szSaveName, "jacob"))
		{
			DevMsg("            loading savegame: %s %s (map %s)...\n", "load", szSaveName, szBspName);
			engine->ServerCommand("maxplayers 1\n");
			engine->ServerCommand( CFmtStr("%s %s\n", "load", szSaveName) );
			engine->ServerExecute();
		}
		else
		{
#endif
			char const *szMapCommand = pKV->GetString( "map/mapcommand", "map" );
			DevMsg( "            starting: %s %s...\n", szMapCommand, szBspName );
			engine->ServerCommand( CFmtStr( "%s %s reserved\n",
				szMapCommand,
				szBspName ) );
#ifdef PORTAL2
		}
#endif
	}
}