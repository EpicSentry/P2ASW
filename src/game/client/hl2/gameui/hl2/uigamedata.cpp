//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "basemodpanel.h"
#include "basemodframe.h"
#include "UIGameData.h"

#include <ctype.h>


#include "./GameUI/IGameUI.h"
#include "ienginevgui.h"
#include "icommandline.h"
#include "vgui/ISurface.h"
#include "EngineInterface.h"
#include "tier0/dbg.h"
#include "ixboxsystem.h"
#include "GameUI_Interface.h"
#include "game/client/IGameClientExports.h"
#include "fmtstr.h"
#include "vstdlib/random.h"
#include "utlbuffer.h"
#include "filesystem/IXboxInstaller.h"
#include "tier1/tokenset.h"
#include "FileSystem.h"
#include "filesystem/IXboxInstaller.h"

#include <time.h>

// BaseModUI High-level windows

#include "VGenericConfirmation.h"
#include "VGenericWaitScreen.h"
#include "VInGameMainMenu.h"
#include "VMainMenu.h"
#include "VFooterPanel.h"
#include "VAttractScreen.h"
// vgui controls
#include "vgui/ILocalize.h"



#ifndef _X360
#include "steam/steam_api.h"
#endif

#include "gameui_util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


using namespace BaseModUI;
using namespace vgui;

//setup in GameUI_Interface.cpp
extern const char *COM_GetModDirectory( void );

ConVar x360_audio_english("x360_audio_english", "0", 0, "Keeps track of whether we're forcing english in a localized language." );

ConVar demo_ui_enable( "demo_ui_enable", "", FCVAR_DEVELOPMENTONLY, "Suffix for the demo UI" );
ConVar demo_connect_string( "demo_connect_string", "", FCVAR_DEVELOPMENTONLY, "Connect string for demo UI" );

///Asyncronous Operations

ConVar mm_ping_max_green( "ping_max_green", "70" );
ConVar mm_ping_max_yellow( "ping_max_yellow", "140" );
ConVar mm_ping_max_red( "ping_max_red", "250" );

//=============================================================================

const tokenset_t< const char * > BaseModUI::s_characterPortraits[] =
{
	{ "",			"select_Random" },
	{ "random",		"select_Random" },
	{ "infected",	"s_panel_hand" },

	{ NULL, "" }
};

//=============================================================================
// Xbox 360 Marketplace entry point
//=============================================================================
struct X360MarketPlaceEntryPoint
{
	DWORD dwEntryPoint;
	uint64 uiOfferID;
};
static X360MarketPlaceEntryPoint g_MarketplaceEntryPoint;


static void GoToMarketplaceForOffer()
{
}

static void ShowMarketplaceUiForOffer()
{
}

//=============================================================================
//
//=============================================================================
CUIGameData* CUIGameData::m_Instance = 0;
bool CUIGameData::m_bModuleShutDown = false;

//=============================================================================
CUIGameData::CUIGameData() :
#if !defined( _X360 ) && !defined( NO_STEAM )
	m_CallbackPersonaStateChanged( this, &CUIGameData::Steam_OnPersonaStateChanged ),
#endif
	m_CGameUIPostInit( false )
{
	m_LookSensitivity = 1.0f;

	m_flShowConnectionProblemTimer = 0.0f;
	m_flTimeLastFrame = Plat_FloatTime();
	m_bShowConnectionProblemActive = false;

	m_bXUIOpen = false;

	m_bWaitingForStorageDeviceHandle = false;
	m_iStorageID = XBX_INVALID_STORAGE_ID;
	m_pAsyncJob = NULL;

	m_pSelectStorageClient = NULL;

	SetDefLessFunc( m_mapUserXuidToAvatar );
	SetDefLessFunc( m_mapUserXuidToName );
}

//=============================================================================
CUIGameData::~CUIGameData()
{
}

//=============================================================================
CUIGameData* CUIGameData::Get()
{
	if ( !m_Instance && !m_bModuleShutDown )
	{
		m_Instance = new CUIGameData();
	}

	return m_Instance;
}

void CUIGameData::Shutdown()
{
	if ( !m_bModuleShutDown )
	{
		m_bModuleShutDown = true;
		delete m_Instance;
		m_Instance = NULL;
	}
}
//=============================================================================
void CUIGameData::RunFrame()
{
	RunFrame_Storage();

	if ( m_flShowConnectionProblemTimer > 0.0f )
	{
		float flCurrentTime = Plat_FloatTime();
		float flTimeElapsed = ( flCurrentTime - m_flTimeLastFrame );

		m_flTimeLastFrame = flCurrentTime;

		if ( flTimeElapsed > 0.0f )
		{
			m_flShowConnectionProblemTimer -= flTimeElapsed;
		}

		if ( m_flShowConnectionProblemTimer > 0.0f )
		{
			if ( !m_bShowConnectionProblemActive &&
				 !CBaseModPanel::GetSingleton().IsVisible() )
			{
				GameUI().ActivateGameUI();
				OpenWaitScreen( "#GameUI_RetryingConnectionToServer", 0.0f );
				m_bShowConnectionProblemActive = true;
			}
		}
		else
		{
			if ( m_bShowConnectionProblemActive )
			{
				// Before closing this particular waitscreen we need to establish
				// a correct navback, otherwise it will not close - Vitaliy (bugbait #51272)
				if ( CBaseModFrame *pWaitScreen = CBaseModPanel::GetSingleton().GetWindow( WT_GENERICWAITSCREEN ) )
				{
					if ( !pWaitScreen->GetNavBack() )
					{
						if ( CBaseModFrame *pIngameMenu = CBaseModPanel::GetSingleton().GetWindow( WT_INGAMEMAINMENU ) )
							pWaitScreen->SetNavBack( pIngameMenu );
					}

					if ( !pWaitScreen->GetNavBack() )
					{
						// This waitscreen will fail to close, force the close!
						pWaitScreen->Close();
					}
				}

				CloseWaitScreen( NULL, "Connection Problems" );
				GameUI().HideGameUI();
				m_bShowConnectionProblemActive = false;
			}
		}
	}
}

void CUIGameData::OnSetStorageDeviceId( int iController, uint nDeviceId )
{
	// Check to see if there is enough room on this storage device
	if ( nDeviceId == XBX_STORAGE_DECLINED || nDeviceId == XBX_INVALID_STORAGE_ID )
	{
		CloseWaitScreen( NULL, "ReportNoDeviceSelected" );
		m_pSelectStorageClient->OnDeviceFail( ISelectStorageDeviceClient::FAIL_NOT_SELECTED );
		m_pSelectStorageClient = NULL;
	}
	else if ( xboxsystem->DeviceCapacityAdequate( iController, nDeviceId, COM_GetModDirectory() ) == false )
	{
		CloseWaitScreen( NULL, "ReportDeviceFull" );
		m_pSelectStorageClient->OnDeviceFail( ISelectStorageDeviceClient::FAIL_FULL );
		m_pSelectStorageClient = NULL;
	}
	else
	{
		// Set the storage device
		XBX_SetStorageDeviceId( iController, nDeviceId );
		OnDeviceAttached();
		m_pSelectStorageClient->OnDeviceSelected();
	}
}

//=============================================================================
void CUIGameData::OnGameUIPostInit()
{
	m_CGameUIPostInit = true;
}

//=============================================================================
bool CUIGameData::CanPlayer2Join()
{
	return false;
}

void CUIGameData::ExecuteOverlayCommand( char const *szCommand )
{
#if !defined( _X360 ) && !defined( NO_STEAM )
	if ( steamapicontext && steamapicontext->SteamFriends() &&
		 steamapicontext->SteamUtils() && steamapicontext->SteamUtils()->IsOverlayEnabled() )
	{
		steamapicontext->SteamFriends()->ActivateGameOverlay( szCommand );
	}
	else
	{
		DisplayOkOnlyMsgBox( NULL, "#L4D360UI_SteamOverlay_Title", "#L4D360UI_SteamOverlay_Text" );
	}
#else
	ExecuteNTimes( 5, DevWarning( "ExecuteOverlayCommand( %s ) is unsupported\n", szCommand ) );
	Assert( !"ExecuteOverlayCommand" );
#endif
}

//=============================================================================
bool CUIGameData::SignedInToLive()
{
	return true;
}

bool CUIGameData::AnyUserSignedInToLiveWithMultiplayerDisabled()
{
	return false;
}

bool CUIGameData::CheckAndDisplayErrorIfOffline( CBaseModFrame *pCallerFrame, char const *szMsg )
{
	return false;
}

bool CUIGameData::CheckAndDisplayErrorIfNotSignedInToLive( CBaseModFrame *pCallerFrame )
{
	if ( !IsX360() )
		return false;

	if ( SignedInToLive() )
		return false;

	char const *szMsg = "";

	if ( AnyUserSignedInToLiveWithMultiplayerDisabled() )
	{
		szMsg = "#L4D360UI_MsgBx_NeedLiveNonGoldMsg";
	}
	else
	{
		szMsg = "#L4D360UI_MsgBx_NeedLiveSinglescreenMsg";
	}

	DisplayOkOnlyMsgBox( pCallerFrame, "#L4D360UI_XboxLive", szMsg );

	return true;
}

void CUIGameData::DisplayOkOnlyMsgBox( CBaseModFrame *pCallerFrame, const char *szTitle, const char *szMsg )
{
	GenericConfirmation* confirmation = 
		static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, pCallerFrame, false ) );
	GenericConfirmation::Data_t data;
	data.pWindowTitle = szTitle;
	data.pMessageText = szMsg;
	data.bOkButtonEnabled = true;
	confirmation->SetUsageData(data);
}

const char *CUIGameData::GetLocalPlayerName( int iController )
{
	static CGameUIConVarRef cl_names_debug( "cl_names_debug" );
	if ( cl_names_debug.GetInt() )
		return "WWWWWWWWWWWWWWW";

	IPlayer *player = g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( iController );
	if ( !player )
	{
		return "";
	}

	return player->GetName();
}


//////////////////////////////////////////////////////////////////////////


//=============================================================================
void CUIGameData::SetLookSensitivity(float sensitivity)
{
	m_LookSensitivity = sensitivity;

	static CGameUIConVarRef joy_yawsensitivity("joy_yawsensitivity");
	if(joy_yawsensitivity.IsValid())
	{
		float defaultValue = atof(joy_yawsensitivity.GetDefault());
		joy_yawsensitivity.SetValue(defaultValue * sensitivity);
	}

	static CGameUIConVarRef joy_pitchsensitivity("joy_pitchsensitivity");
	if(joy_pitchsensitivity.IsValid())
	{
		float defaultValue = atof(joy_pitchsensitivity.GetDefault());
		joy_pitchsensitivity.SetValue(defaultValue * sensitivity);
	}
}

//=============================================================================
float CUIGameData::GetLookSensitivity()
{
	return m_LookSensitivity;
}

bool CUIGameData::IsXUIOpen()
{
	return m_bXUIOpen;
}

void CUIGameData::OpenWaitScreen( const char * messageText, float minDisplayTime, KeyValues *pSettings )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] OpenWaitScreen( %s )\n", messageText );
	}

	WINDOW_TYPE wtActive = CBaseModPanel::GetSingleton().GetActiveWindowType();
	CBaseModFrame * backFrame = CBaseModPanel::GetSingleton().GetWindow( wtActive );
	if ( wtActive == WT_GENERICWAITSCREEN && backFrame )
	{
		backFrame = backFrame->GetNavBack();
		DevMsg( "CUIGameData::OpenWaitScreen - setting navback to %s instead of waitscreen\n", backFrame ? backFrame->GetName() : "NULL" );
	}
	if ( wtActive == WT_GENERICCONFIRMATION && backFrame )
	{
		DevWarning( "Cannot display waitscreen! Active window of higher priority: %s\n", backFrame->GetName() );
		return;
	}

	GenericWaitScreen* waitScreen = static_cast<GenericWaitScreen*>( 
		CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICWAITSCREEN, backFrame, false, pSettings ) );
	if( waitScreen )
	{
		waitScreen->SetNavBack( backFrame );
		waitScreen->ClearData();
		waitScreen->AddMessageText( messageText, minDisplayTime );
	}
}

void CUIGameData::UpdateWaitPanel( const char * messageText, float minDisplayTime )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] UpdateWaitPanel( %s )\n", messageText );
	}

	GenericWaitScreen* waitScreen = static_cast<GenericWaitScreen*>( 
		CBaseModPanel::GetSingleton().GetWindow( WT_GENERICWAITSCREEN ) );
	if( waitScreen )
	{
		waitScreen->AddMessageText( messageText, minDisplayTime );
	}
}

void CUIGameData::UpdateWaitPanel( const wchar_t * messageText, float minDisplayTime )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] UpdateWaitPanel( %S )\n", messageText );
	}

	GenericWaitScreen* waitScreen = static_cast<GenericWaitScreen*>( 
		CBaseModPanel::GetSingleton().GetWindow( WT_GENERICWAITSCREEN ) );
	if( waitScreen )
	{
		waitScreen->AddMessageText( messageText, minDisplayTime );
	}
}

void CUIGameData::CloseWaitScreen( vgui::Panel * callbackPanel, const char * message )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CloseWaitScreen( %s )\n", message );
	}

	GenericWaitScreen* waitScreen = static_cast<GenericWaitScreen*>( 
		CBaseModPanel::GetSingleton().GetWindow( WT_GENERICWAITSCREEN ) );
	if( waitScreen )
	{
		waitScreen->SetCloseCallback( callbackPanel, message );
	}
}

void CUIGameData::NeedConnectionProblemWaitScreen ( void )
{
	m_flShowConnectionProblemTimer = 1.0f;
}

static void PasswordEntered()
{
	CUIGameData::Get()->FinishPasswordUI( true );
}

static void PasswordNotEntered()
{
	CUIGameData::Get()->FinishPasswordUI( false );
}

void CUIGameData::ShowPasswordUI( char const *pchCurrentPW )
{
}

void CUIGameData::FinishPasswordUI( bool bOk )
{
}

IImage *CUIGameData::GetAvatarImage( XUID playerID )
{
	if ( !playerID )
		return NULL;

	// do we already have this image cached?
	CGameUiAvatarImage *pImage = NULL;
	int iIndex = m_mapUserXuidToAvatar.Find( playerID );
	
	if ( iIndex == m_mapUserXuidToAvatar.InvalidIndex() )
	{
		// cache a new image
		pImage = new CGameUiAvatarImage();

		// We may fail to set the steam ID - if the player is not our friend and we are not in a lobby or game, eg
		if ( !pImage->SetAvatarSteamID( playerID ) )
		{
			delete pImage;
			return NULL;
		}

		iIndex = m_mapUserXuidToAvatar.Insert( playerID, pImage );
	}
	else
	{
		pImage = m_mapUserXuidToAvatar.Element( iIndex );
	}

	return pImage;
}

char const * CUIGameData::GetPlayerName( XUID playerID, char const *szPlayerNameSpeculative )
{
	static CGameUIConVarRef cl_names_debug( "cl_names_debug" );
	if ( cl_names_debug.GetInt() )
		return "WWWWWWWWWWWWWWW";

#if !defined( _X360 ) && !defined( NO_STEAM )
	if ( steamapicontext && steamapicontext->SteamUtils() &&
		steamapicontext->SteamFriends() && steamapicontext->SteamUser() )
	{
		int iIndex = m_mapUserXuidToName.Find( playerID );
		if ( iIndex == m_mapUserXuidToName.InvalidIndex() )
		{
			char const *szName = steamapicontext->SteamFriends()->GetFriendPersonaName( playerID );
			if ( szName && *szName )
			{
				iIndex = m_mapUserXuidToName.Insert( playerID, szName );
			}
		}

		if ( iIndex != m_mapUserXuidToName.InvalidIndex() )
			return m_mapUserXuidToName.Element( iIndex ).Get();
	}
#endif

	return szPlayerNameSpeculative;
}

#if !defined( _X360 ) && !defined( NO_STEAM )
void CUIGameData::Steam_OnPersonaStateChanged( PersonaStateChange_t *pParam )
{
	if ( !pParam->m_ulSteamID )
		return;

	if ( pParam->m_nChangeFlags & k_EPersonaChangeName )
	{
		int iIndex = m_mapUserXuidToName.Find( pParam->m_ulSteamID );
		if ( iIndex != m_mapUserXuidToName.InvalidIndex() )
		{
			CUtlString utlName = m_mapUserXuidToName.Element( iIndex );
			m_mapUserXuidToName.RemoveAt( iIndex );
			GetPlayerName( pParam->m_ulSteamID, utlName.Get() );
		}
	}

	if ( pParam->m_nChangeFlags & k_EPersonaChangeAvatar )
	{
		CGameUiAvatarImage *pImage = NULL;
		int iIndex = m_mapUserXuidToAvatar.Find( pParam->m_ulSteamID );
		if ( iIndex != m_mapUserXuidToAvatar.InvalidIndex() )
		{
			pImage = m_mapUserXuidToAvatar.Element( iIndex );
		}

		// Re-fetch the image if we have it cached
		if ( pImage )
		{
			pImage->SetAvatarSteamID( pParam->m_ulSteamID );
		}
	}
}
#endif

CON_COMMAND_F( ui_reloadscheme, "Reloads the resource files for the active UI window", 0 )
{
//	g_pFullFileSystem->SyncDvdDevCache();
	CUIGameData::Get()->ReloadScheme();
}

void CUIGameData::ReloadScheme()
{
	CBaseModPanel::GetSingleton().ReloadScheme();
	CBaseModFrame *window = CBaseModPanel::GetSingleton().GetWindow( CBaseModPanel::GetSingleton().GetActiveWindowType() );
	if( window )
	{
		window->ReloadSettings();
	}
	CBaseModFooterPanel *footer = CBaseModPanel::GetSingleton().GetFooterPanel();
	if( footer )
	{
		footer->ReloadSettings();
	}
}

CBaseModFrame * CUIGameData::GetParentWindowForSystemMessageBox()
{
	WINDOW_TYPE wtActive = CBaseModPanel::GetSingleton().GetActiveWindowType();
	WINDOW_PRIORITY wPriority = CBaseModPanel::GetSingleton().GetActiveWindowPriority();

	CBaseModFrame *pCandidate = CBaseModPanel::GetSingleton().GetWindow( wtActive );

	if ( pCandidate )
	{
		if ( wPriority >= WPRI_WAITSCREEN && wPriority <= WPRI_MESSAGE )
		{
			if ( UI_IsDebug() )
			{
				DevMsg( "GetParentWindowForSystemMessageBox: using navback of %s\n", pCandidate->GetName() );
			}

			// Message would not be able to nav back to waitscreen or another message
			pCandidate = pCandidate->GetNavBack();
		}
		else if ( wPriority > WPRI_MESSAGE )
		{
			if ( UI_IsDebug() )
			{
				DevMsg( "GetParentWindowForSystemMessageBox: using NULL since a higher priority window is open %s\n", pCandidate->GetName() );
			}

			// Message would not be able to nav back to a higher level priority window
			pCandidate = NULL;
		}
	}

	return pCandidate;
}

bool CUIGameData::IsActiveSplitScreenPlayerSpectating( void )
{
// 	int iLocalPlayerTeam;
// 	if ( GameClientExports()->GetPlayerTeamIdByUserId( -1, iLocalPlayerTeam ) )
// 	{
// 		if ( iLocalPlayerTeam != GameClientExports()->GetTeamId_Survivor() &&
// 			 iLocalPlayerTeam != GameClientExports()->GetTeamId_Infected() )
// 			return true;
// 	}

	return false;
}

void CUIGameData::OnEvent( KeyValues *pEvent )
{
	char const *szEvent = pEvent->GetName();

	if ( !Q_stricmp( "OnSysXUIEvent", szEvent ) )
	{
		m_bXUIOpen = !Q_stricmp( "opening", pEvent->GetString( "action", "" ) );
	}
	else if ( !Q_stricmp( "OnProfileUnavailable", szEvent ) )
	{
#if defined( _DEMO ) && defined( _X360 )
		return;
#endif
		// Activate game ui to see the dialog
		if ( !CBaseModPanel::GetSingleton().IsVisible() )
		{
			engine->ExecuteClientCmd( "gameui_activate" );
		}

		// Pop a message dialog if their storage device was changed
		GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION,
			GetParentWindowForSystemMessageBox(), false ) );
		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_MsgBx_AchievementNotWrittenTitle";
		data.pMessageText = "#L4D360UI_MsgBx_AchievementNotWritten"; 
		data.bOkButtonEnabled = true;

		confirmation->SetUsageData( data );
	}
	else if ( !Q_stricmp( "OnSysStorageDevicesChanged", szEvent ) )
	{
#if defined( _DEMO ) && defined( _X360 )
		return;
#endif

		// If a storage device change is in progress, the simply ignore
		// the notification callback, but pop the dialog
		if ( m_pSelectStorageClient )
		{
			DevWarning( "Ignored OnSysStorageDevicesChanged while the storage selection was in progress...\n" );
		}

		// Activate game ui to see the dialog
		if ( !CBaseModPanel::GetSingleton().IsVisible() )
		{
			engine->ExecuteClientCmd( "gameui_activate" );
		}

		// Pop a message dialog if their storage device was changed
		GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION,
			GetParentWindowForSystemMessageBox(), false ) );
		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#GameUI_Console_StorageRemovedTitle";
		data.pMessageText = "#L4D360UI_MsgBx_StorageDeviceRemoved"; 
		data.bOkButtonEnabled = true;
		
		extern void OnStorageDevicesChangedSelectNewDevice();
		data.pfnOkCallback = m_pSelectStorageClient ? NULL : &OnStorageDevicesChangedSelectNewDevice;	// No callback if already in the middle of selecting a storage device

		confirmation->SetUsageData( data );
	}
	else if ( !Q_stricmp( "OnSysInputDevicesChanged", szEvent ) )
	{
		unsigned int nInactivePlayers = 0;  // Number of users on the spectating team (ie. idle), or disconnected in this call
		int iOldSlot = engine->GetActiveSplitScreenPlayerSlot();
		int nDisconnectedDevices = pEvent->GetInt( "mask" );
		for ( unsigned int nSlot = 0; nSlot < XBX_GetNumGameUsers(); ++nSlot, nDisconnectedDevices >>= 1 )
		{
			engine->SetActiveSplitScreenPlayerSlot( nSlot );
			
			// See if this player is spectating (ie. idle)
			bool bSpectator = IsActiveSplitScreenPlayerSpectating();
			if ( bSpectator )
			{
				nInactivePlayers++;
			}
		
			if ( nDisconnectedDevices & 0x1 )
			{
				// Only count disconnections if that player wasn't idle
				if ( !bSpectator )
				{
					nInactivePlayers++;
				}

				engine->ClientCmd( "go_away_from_keyboard" );
			}
		}
		engine->SetActiveSplitScreenPlayerSlot( iOldSlot );

		// If all the spectators and all the disconnections account for all possible users, we need to pop a message
		// Also, if the GameUI is up, always show the disconnection message
		if ( CBaseModPanel::GetSingleton().IsVisible() || nInactivePlayers == XBX_GetNumGameUsers() )
		{
			if ( !CBaseModPanel::GetSingleton().IsVisible() )
			{
				engine->ExecuteClientCmd( "gameui_activate" );
			}

			// Pop a message if a valid controller was removed!
			GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION,
				GetParentWindowForSystemMessageBox(), false ) );
			
			GenericConfirmation::Data_t data;
			data.pWindowTitle = "#L4D360UI_MsgBx_ControllerUnpluggedTitle";
			data.pMessageText = "#L4D360UI_MsgBx_ControllerUnplugged";
			data.bOkButtonEnabled = true;

			confirmation->SetUsageData(data);
		}
	}
	else if ( !Q_stricmp( "OnMatchPlayerMgrReset", szEvent ) )
	{
		char const *szReason = pEvent->GetString( "reason", "" );
		bool bShowDisconnectedMsgBox = true;
		if ( !Q_stricmp( szReason, "GuestSignedIn" ) )
		{
			char const *szDestroyedSessionState = pEvent->GetString( "settings/game/state", "lobby" );
			if ( !Q_stricmp( "lobby", szDestroyedSessionState ) )
				bShowDisconnectedMsgBox = false;
		}

		engine->HideLoadingPlaque(); // This may not go away unless we force it to hide

		// Go to the attract screen
		CBaseModPanel::GetSingleton().CloseAllWindows( CBaseModPanel::CLOSE_POLICY_EVEN_MSGS );

		// Show the message box
		GenericConfirmation* confirmation = bShowDisconnectedMsgBox ? static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, NULL, false ) ) : NULL;
		CAttractScreen::SetAttractMode( CAttractScreen::ATTRACT_GAMESTART );
		CBaseModPanel::GetSingleton().OpenWindow( WT_ATTRACTSCREEN, NULL );

		if ( confirmation )
		{
			GenericConfirmation::Data_t data;

			data.pWindowTitle = "#L4D360UI_MsgBx_SignInChangeC";
			data.pMessageText = "#L4D360UI_MsgBx_SignInChange";
			data.bOkButtonEnabled = true;

			if ( !Q_stricmp( szReason, "GuestSignedIn" ) )
			{
				data.pWindowTitle = "#L4D360UI_MsgBx_DisconnectedFromSession";	// "Disconnect"
				data.pMessageText = "#L4D360UI_MsgBx_SignInChange";				// "Sign-in change has occured."
			}

			confirmation->SetUsageData(data);

#ifdef _X360
			// When a confirmation shows up it prevents attract screen from opening, so reset user slots here:
			XBX_ResetUserIdSlots();
			XBX_SetPrimaryUserId( XBX_INVALID_USER_ID );
			XBX_SetPrimaryUserIsGuest( 0 );	
			XBX_SetNumGameUsers( 0 ); // users not selected yet
#endif
		}
	}
	else if ( !Q_stricmp( "OnEngineDisconnectReason", szEvent ) )
	{
		char const *szReason = pEvent->GetString( "reason", "" );

		if ( char const *szDisconnectHdlr = pEvent->GetString( "disconnecthdlr", NULL ) )
		{
			// If a disconnect handler was set during the event, then we don't interfere with
			// the dialog explaining disconnection, just let the disconnect handler do everything.
			return;
		}

		RemapText_t arrText[] = {
			{ "", "#DisconnectReason_Unknown", RemapText_t::MATCH_FULL },
			{ "Lost connection to LIVE", "#DisconnectReason_LostConnectionToLIVE", RemapText_t::MATCH_FULL },
			{ "Player removed from host session", "#DisconnectReason_PlayerRemovedFromSession", RemapText_t::MATCH_SUBSTR },
			{ "Connection to server timed out", "#L4D360UI_MsgBx_DisconnectedFromServer", RemapText_t::MATCH_SUBSTR },
			{ "Added to banned list", "#SessionError_Kicked", RemapText_t::MATCH_SUBSTR },
			{ "Kicked and banned", "#SessionError_Kicked", RemapText_t::MATCH_SUBSTR },
			{ "You have been voted off", "#SessionError_Kicked", RemapText_t::MATCH_SUBSTR },
			{ "All players idle", "#L4D_ServerShutdownIdle", RemapText_t::MATCH_SUBSTR },
#ifdef _X360
			{ "", "#DisconnectReason_Unknown", RemapText_t::MATCH_START },	// Catch all cases for X360
#endif
			{ NULL, NULL, RemapText_t::MATCH_FULL }
		};

		szReason = RemapText_t::RemapRawText( arrText, szReason );

		//
		// Go back to main menu and display the disconnection reason
		//
		engine->HideLoadingPlaque(); // This may not go away unless we force it to hide

		// Go to the main menu
		CBaseModPanel::GetSingleton().CloseAllWindows( CBaseModPanel::CLOSE_POLICY_EVEN_MSGS );

		// Show the message box
		GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, NULL, false ) );
		CBaseModPanel::GetSingleton().OpenWindow( WT_MAINMENU, NULL );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_MsgBx_DisconnectedFromSession";	// "Disconnect"
		data.pMessageText = szReason;
		data.bOkButtonEnabled = true;

		confirmation->SetUsageData(data);
	}
	else if ( !Q_stricmp( "OnEngineEndGame", szEvent ) )
	{
		// If we are connected and there was no session object to handle the event
		if ( !g_pMatchFramework->GetMatchSession() )
		{
			// Issue the disconnect command
			engine->ExecuteClientCmd( "disconnect" );
		}
	}
	else if ( !Q_stricmp( "OnMatchSessionUpdate", szEvent ) )
	{
		if ( !Q_stricmp( "error", pEvent->GetString( "state", "" ) ) )
		{
			g_pMatchFramework->CloseSession();

			char chErrorMsgBuffer[128] = {0};
			char const *szError = pEvent->GetString( "error", "" );
			char const *szErrorTitle = "#L4D360UI_MsgBx_DisconnectedFromSession";

			RemapText_t arrText[] = {
				{ "", "#SessionError_Unknown", RemapText_t::MATCH_FULL },
				{ "n/a", "#SessionError_NotAvailable", RemapText_t::MATCH_FULL },
				{ "create", "#SessionError_Create", RemapText_t::MATCH_FULL },
				{ "createclient", "#SessionError_NotAvailable", RemapText_t::MATCH_FULL },
				{ "connect", "#SessionError_Connect", RemapText_t::MATCH_FULL },
				{ "full", "#SessionError_Full", RemapText_t::MATCH_FULL },
				{ "lock", "#SessionError_Lock", RemapText_t::MATCH_FULL },
				{ "kicked", "#SessionError_Kicked", RemapText_t::MATCH_FULL },
				{ "migrate", "#SessionError_Migrate", RemapText_t::MATCH_FULL },
				{ "nomap", "#SessionError_NoMap", RemapText_t::MATCH_FULL },
				{ "SteamServersDisconnected", "#SessionError_SteamServersDisconnected", RemapText_t::MATCH_FULL },
				{ NULL, NULL, RemapText_t::MATCH_FULL }
			};

			szError = RemapText_t::RemapRawText( arrText, szError );

			if ( !Q_stricmp( "turequired", szError ) )
			{
				// Special case for TU required message
				// If we have a localization string for the TU message then this means that the other box
				// is running and older version of the TU
				char const *szTuRequiredCode = pEvent->GetString( "turequired" );
				CFmtStr strLocKey( "#SessionError_TU_Required_%s", szTuRequiredCode );
				if ( g_pVGuiLocalize->Find( strLocKey ) )
				{
					Q_strncpy( chErrorMsgBuffer, strLocKey, sizeof( chErrorMsgBuffer ) );
					szError = chErrorMsgBuffer;
				}
				else
				{
					szError = "#SessionError_TU_RequiredMessage";
				}
				szErrorTitle = "#SessionError_TU_RequiredTitle";
			}

			// Go to the main menu
			CBaseModPanel::GetSingleton().CloseAllWindows( CBaseModPanel::CLOSE_POLICY_EVEN_MSGS );

			// Show the message box
			GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, NULL, false ) );
			CBaseModPanel::GetSingleton().OpenWindow( WT_MAINMENU, NULL );

			GenericConfirmation::Data_t data;

			data.pWindowTitle = szErrorTitle;
			data.pMessageText = szError;
			data.bOkButtonEnabled = true;
			confirmation->SetUsageData(data);
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//
//
// A bunch of helper KeyValues hierarchy readers
//
//
//////////////////////////////////////////////////////////////////////////


bool GameModeHasDifficulty( char const *szGameMode )
{
	return true;	// all alien swarm game modes have difficulty
	//return !Q_stricmp( szGameMode, "campaign" ) || !Q_stricmp( szGameMode, "single_mission" );
}

char const * GameModeGetDefaultDifficulty( char const *szGameMode )
{
	if ( !GameModeHasDifficulty( szGameMode ) )
		return "normal";

	IPlayerLocal *pProfile = g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( XBX_GetPrimaryUserId() );
	if ( !pProfile )
		return "normal";

	UserProfileData const &upd = pProfile->GetPlayerProfileData();
	switch ( upd.difficulty )
	{
	case 1: return "easy";
	case 2: return "hard";
	default: return "normal";
	}
}

bool GameModeHasRoundLimit( char const *szGameMode )
{
	return !Q_stricmp( szGameMode, "scavenge" ) || !Q_stricmp( szGameMode, "teamscavenge" );
}

bool GameModeIsSingleChapter( char const *szGameMode )
{
	return !Q_stricmp( szGameMode, "survival" ) || !Q_stricmp( szGameMode, "scavenge" ) || !Q_stricmp( szGameMode, "teamscavenge" );
}


uint64 GetDlcInstalledMask()
{
	return 0;
}


