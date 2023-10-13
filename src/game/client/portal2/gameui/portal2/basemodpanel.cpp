//========= Copyright  1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"

#include "basemodpanel.h"

#include "./GameUI/IGameUI.h"
#include "ienginevgui.h"
#include "engine/ienginesound.h"
#include "EngineInterface.h"
#include "tier0/dbg.h"
#include "ixboxsystem.h"
#include "GameUI_Interface.h"
#include "cdll_client_int.h"
#include "game/client/IGameClientExports.h"
#include "gameui/igameconsole.h"
#include "inputsystem/iinputsystem.h"
#include "FileSystem.h"
#include "filesystem/IXboxInstaller.h"

#ifdef _X360
	#include "xbox/xbox_launch.h"
#elif defined(_PS3)
#include "ps3/saverestore_ps3_api_ui.h"
#endif
#include "steamoverlay/isteamoverlaymgr.h"

// BaseModUI High-level windows
#include "VTransitionScreen.h"
#include "VAchievements.h"
#include "vaddonassociation.h"
#include "VAddons.h"
#include "VAttractScreen.h"
#include "VAudio.h"
#include "VAudioVideo.h"
#include "VCloud.h"
#include "VControllerOptions.h"
#include "VControllerOptionsButtons.h"
#include "VControllerOptionsSticks.h"
#include "VDownloads.h"
#include "vendingsplitscreen.h"
#include "VFoundGames.h"
#include "VFoundGroupGames.h"
#include "vfoundpublicgames.h"
#include "VGameLobby.h"
#include "VGameOptions.h"
#include "VGameSettings.h"
#include "VGenericConfirmation.h"
#include "VGenericWaitScreen.h"
#include "vgetlegacydata.h"
#include "VInGameDifficultySelect.h"
#include "VInGameMainMenu.h"
#include "VInGameChapterSelect.h"
#include "VInGameKickPlayerList.h"
#include "VKeyboardMouse.h"
#include "vpvplobby.h"
#include "VVoteOptions.h"
#include "VLoadingProgress.h"
#include "VMainMenu.h"
#include "VMultiplayer.h"
#include "VOptions.h"
#include "VXboxLIVE.h"
#include "VOptionsCloud.h"
#include "VSignInDialog.h"
#include "vsteamlinkdialog.h"
#include "VStartCoopGame.h"
#include "VFooterPanel.h"
#include "VPasswordEntry.h"
#include "VVideo.h"
#include "VSteamCloudConfirmation.h"
#include "vcustomcampaigns.h"
#include "vdownloadcampaign.h"
#include "vleaderboard.h"
#include "vportalleaderboard.h"
#include "vportalleaderboardhud.h"
#include "vcoopexitchoice.h"
#include "gameconsole.h"
#include "vgui/ISystem.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "vgui_controls/AnimationController.h"
#include "gameui_util.h"
#include "vguimatsurface/imatsystemsurface.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imesh.h"
#include "tier0/icommandline.h"
#include "fmtstr.h"
#include "smartptr.h"
#include "vsingleplayer.h"
#include "vcoopmode.h"
#include "vnewgamedialog.h"
#include "vchallengemodedialog.h"
#include "vsaveloadgamedialog.h"
#include "vmovieplayer.h"
#include "steamcloudsync.h"
#include "transitionpanel.h"
#include "vautosavenotice.h"
#include "vfadeoutstartgame.h"
#include "vadvancedvideo.h"
#include "vkeybindings.h"
#include "vsoundtest.h"
#include "vextrasdialog.h"
#include "vfadeouttoeconui.h"
#include "materialsystem/materialsystem_config.h"
#include "utlmap.h"
#include "vgui_int.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static LoggingFileHandle_t s_WorkshopLogHandle;

using namespace BaseModUI;
using namespace vgui;

//setup in GameUI_Interface.cpp
extern class IMatchSystem *matchsystem;

extern CBaseModFrame *OpenPortal2EconUI( vgui::Panel *pParent );

static int playingMusicFromAct = 1;
extern ConVar portal2_current_act;

#define MAX_QUICK_PLAY_ENTRIES	50	// We want at least this many in the queue to consider ourselves "full"

struct ChapterContext_t
{
	const char	*m_pMapName;
	int			m_nChapterNum;
	int			m_nSubChapterNum;
};

ChapterContext_t g_ChapterContextNames[] = 
{
#define CFG( spmapname, chapternum, subchapter ) { #spmapname, chapternum, subchapter },
// Needs fixing?
//#include "../common/xlast_portal2/inc_sp_maps.inc"
#undef CFG
      { NULL, 0 },
};

ChapterContext_t g_ChapterMPContextNames[] = 
{
#define CFG( coopmapname, chapternum, subchapter, total ) { #coopmapname, chapternum, subchapter },
// Needs fixing?
//#include "../common/xlast_portal2/inc_coop_maps.inc"
#undef CFG
	{ NULL, 0 },
};

// The current map ID (in string form to encode a uint64 inside ConVar class)
ConVar cm_current_community_map( "cm_current_community_map", "0", FCVAR_HIDDEN | FCVAR_REPLICATED );
extern ConVar cm_is_current_community_map_coop;
ConVar cm_community_debug_spew( "cm_community_debug_spew", "0" );
Color rgbaCommunityDebug = Color( 64, 200, 255, 255 );
ConVar cm_filter_quickplay_with_history( "cm_filter_quickplay_with_history", "1", FCVAR_CHEAT, "Tells the client if it should filter maps in QuickPlay mode with history queue." );

//=============================================================================
CBaseModPanel* CBaseModPanel::m_CFactoryBasePanel = 0;

#ifndef _CERT
#ifdef _GAMECONSOLE
ConVar ui_gameui_debug( "ui_gameui_debug", "1" );
#else
ConVar ui_gameui_debug( "ui_gameui_debug", "0", FCVAR_RELEASE );
#endif
int UI_IsDebug()
{
	return (*(int *)(&ui_gameui_debug)) ? ui_gameui_debug.GetInt() : 0;
}
#endif

#if defined( _X360 )
static void InstallStatusChanged( IConVar *pConVar, const char *pOldValue, float flOldValue )
{
	// spew out status
	if ( ((ConVar *)pConVar)->GetBool() && g_pXboxInstaller )
	{
		SpewInstallStatus();
	}
}
ConVar xbox_install_status( "xbox_install_status", "0", 0, "Show install status", InstallStatusChanged );
#endif

ConVar sys_attract_mode_timeout( "sys_attract_mode_timeout", "30", FCVAR_DEVELOPMENTONLY );

#if defined( _GAMECONSOLE )
CON_COMMAND_F( ui_force_attract, "", FCVAR_DEVELOPMENTONLY )
{
	// for development only testing, force the attract mode to begin its timeout
	// enable it, regardles of initial setup
	CBaseModPanel::GetSingleton().ResetAttractDemoTimeout( true );
}
#endif

// Use for show demos to force the correct campaign poster
ConVar demo_campaign_name( "demo_campaign_name", "L4D2C5", FCVAR_DEVELOPMENTONLY, "Short name of campaign (i.e. L4D2C5), used to show correct poster in demo mode." );

ConVar ui_lobby_noresults_create_msg_time( "ui_lobby_noresults_create_msg_time", "2.5", FCVAR_DEVELOPMENTONLY );
ConVar ui_lobby_noautostart( "ui_lobby_noautostart", "0", FCVAR_DEVELOPMENTONLY );
ConVar ui_lobby_jointimeout( "ui_lobby_jointimeout", "75", FCVAR_DEVELOPMENTONLY );

ConVar ui_fadexui_time( "ui_fadexui_time", "0.5", FCVAR_DEVELOPMENTONLY );
ConVar ui_fadecloud_time( "ui_fadecloud_time", "1.5", FCVAR_DEVELOPMENTONLY );

ConVar ui_lastact_played( "ui_lastact_played", "0", FCVAR_HIDDEN | FCVAR_ARCHIVE, "", true, 0, true, 99 );

//=============================================================================
CBaseModPanel::CBaseModPanel(): BaseClass(0, "CBaseModPanel"),
	m_bClosingAllWindows( false ),
	m_lastActiveUserId( 0 ),
	m_bSetup( false ),
	m_flFadeinDelayAfterOverlay( 0 ),
	m_bHideAndFadeinLater( false ),
	m_bMoveToEditorMainMenu( false )
{
#if !defined( NO_STEAM )
	// Set Steam overlay position
	if ( steamapicontext && steamapicontext->SteamUtils() )
	{
		steamapicontext->SteamUtils()->SetOverlayNotificationPosition( k_EPositionTopRight );
	}
#endif

	MakePopup( false );

	Assert(m_CFactoryBasePanel == 0);
	m_CFactoryBasePanel = this;

	g_pVGuiLocalize->AddFile( "Resource/basemodui_%language%.txt");
	g_pVGuiLocalize->AddFile( "Resource/basemodui_tu_%language%.txt" );

	m_LevelLoading = false;
	
	for ( int k = 0; k < WPRI_COUNT; ++ k )
	{
		m_ActiveWindow[k] = WT_NONE;
	}

	// delay 3 frames before doing activation on initialization
	// needed to allow engine to exec startup commands (background map signal is 1 frame behind) 
	m_DelayActivation = 3;

	m_nActivationCount = 0;

	m_nStartupFrames = 0;

	m_UIScheme = vgui::scheme()->LoadSchemeFromFileEx( 0, "resource/" GAMEUI_BASEMODPANEL_SCHEME ".res", GAMEUI_BASEMODPANEL_SCHEME );
	SetScheme( m_UIScheme );

	// Only one user on the PC, so set it now
	SetLastActiveUserId( IsPC() ? 0 : -1 );

	m_pAvatarImage = NULL;
	m_xuidAvatarImage = 0ull;

	m_FooterPanel = new CBaseModFooterPanel( this, "FooterPanel" );

	m_pTransitionPanel = new CBaseModTransitionPanel( "TransitionPanel" );
	m_pTransitionPanel->SetParent( enginevguifuncs->GetPanel( PANEL_GAMEUIDLL ) ); // PANEL_TRANSITIONEFFECT doesn't exist :(

	m_hOptionsDialog = NULL;

	m_bWarmRestartMode = false;
	m_ExitingFrameCount = 0;

	m_flBlurScale = 0;
	m_flLastBlurTime = 0;

	// Background movie
	m_BIKHandle = BIKHANDLE_INVALID;
	m_flU0 = m_flV0 = m_flU1 = m_flV1 = 0.0f;
	m_bMovieFailed = false;
	m_bMovieLetterbox = false;
	m_nMoviePlaybackWidth = 0;
	m_nMoviePlaybackHeight = 0;

	m_iStartupImageID = -1;

	m_iProductImageID = -1;
	m_iAltProductImageID = -1;

	for ( int jj = 0; jj < ARRAYSIZE( m_iCloudImageID ); ++ jj )
		m_iCloudImageID[jj] = -1;
	
	m_iBackgroundImageID = -1;

	memset( m_iCloudPosition, 0, sizeof( m_iCloudPosition ) );
	memset( m_iCloudProgressPosition, 0, sizeof( m_iCloudProgressPosition ) );
	memset( m_iCloudTextPosition, 0, sizeof( m_iCloudTextPosition ) );

	m_flOverlayFadeOutTime = -1;
	m_iFadeOutOverlayImageID = -1;

	m_BackgroundMusicString = "";
	m_nBackgroundMusicGUID = 0;

	m_flMusicFadeInTime = 0;
	m_bAllowMovie = false;

	m_nProductImageX = 0;
	m_nProductImageY = 0;
	m_nProductImageWide = 0;
	m_nProductImageTall = 0;

	m_clrCloudRemaining = Color( 0, 0, 0, 0 );
	m_clrCloudDone = Color( 0, 0, 0, 0 );
	m_clrCloudDoneFade = Color( 0, 0, 0, 0 );
	m_clrSteamCloudText = Color( 255, 255, 255, 255 );

	m_hDefaultFont = vgui::INVALID_FONT;
	m_hSteamCloudFont = vgui::INVALID_FONT;

	m_nCurrentActPresentation = 0;

	m_bForceUseAlternateTileSet = false;

	// Subscribe to event notifications
	g_pMatchFramework->GetEventsSubscription()->Subscribe( this );

	// -1 means reset timer when viable
	// 0 means NEVER timeout
	// >0 means enabled, is the high water mark time
	m_flAttractDemoTimeout = -1;
#if defined( _X360 ) && defined( _DEMO )
	if ( engine->IsDemoHostedFromShell() )
	{
		// there is already a master demo timeout that overrides
		// prevent any attract mode timeout
		m_flAttractDemoTimeout = 0;
	}
#endif

	if ( !IsGameConsole() )
	{
		// only for game console temporarily
		m_flAttractDemoTimeout = 0;
	}

#if !defined( NO_STEAM )	
	
	m_bUGCRequestsPaused = false;

	m_flQueueHistoryBaselineRequestTime = -1;	// No requests yet made
	m_bReceivedQueueHistoryBaseline = false;
	m_bQueueReady = false;
	
	m_nTotalQueueHistoryEntriesLoaded = 0;
	m_flQueueBaselineRequestTime = -1;	// No requests yet made

	m_bReceivedQueueBaseline = false;

	m_bQuickPlayQueueReady = false;
	m_bQuickPlayQueueError = false;
	m_nTotalQuickPlayEntriesLoaded = 0;
	m_bReceivedQuickPlayBaseline = false;
	m_flQuickPlayBaselineRequestTime = -1.0f;
	
	// Start out in an invalid state to ensure that this is being set properly in all cases

#endif // !NO_STEAM

}

//=============================================================================
CBaseModPanel::~CBaseModPanel()
{
	// Unsubscribe from event notifications
	g_pMatchFramework->GetEventsSubscription()->Unsubscribe( this );

	if ( m_FooterPanel )
	{
		m_FooterPanel->MarkForDeletion();
		m_FooterPanel = NULL;
	}	

	if ( m_pTransitionPanel )
	{
		m_pTransitionPanel->MarkForDeletion();
		m_pTransitionPanel = NULL;
	}

	Assert(m_CFactoryBasePanel == this);
	m_CFactoryBasePanel = 0;

	// Free our movie resources
	ShutdownBackgroundMovie();

	if ( surface() )
	{
		if ( m_iStartupImageID != -1 )
		{
			surface()->DestroyTextureID( m_iStartupImageID );
			m_iStartupImageID = -1;
		}

		if ( m_iProductImageID != -1 )
		{
			surface()->DestroyTextureID( m_iProductImageID );
			m_iProductImageID = -1;
		}

		if ( m_iAltProductImageID != -1 )
		{
			surface()->DestroyTextureID( m_iAltProductImageID );
			m_iAltProductImageID = -1;
		}

		for ( int jj = 0; jj < ARRAYSIZE( m_iCloudImageID ); ++ jj )
		{
			if ( m_iCloudImageID[jj] != -1 )
			{
				surface()->DestroyTextureID( m_iCloudImageID[jj] );
				m_iCloudImageID[jj] = -1;
			}
		}

		if ( m_iBackgroundImageID != -1 )
		{
			surface()->DestroyTextureID( m_iBackgroundImageID );
			m_iBackgroundImageID = -1;
		}
	}

	// Stop logging
	if ( filelogginglistener != NULL )
	{
		filelogginglistener->EndLoggingToFile( s_WorkshopLogHandle );
	}
}

//=============================================================================
CBaseModPanel& CBaseModPanel::GetSingleton()
{
	Assert(m_CFactoryBasePanel != 0);
	return *m_CFactoryBasePanel;
}

//=============================================================================
CBaseModPanel* CBaseModPanel::GetSingletonPtr()
{
	return m_CFactoryBasePanel;
}

//=============================================================================
void CBaseModPanel::ReloadScheme()
{
}

//=============================================================================
CBaseModFrame* CBaseModPanel::OpenWindow( const WINDOW_TYPE & wt, CBaseModFrame *caller, bool hidePrevious, KeyValues *pParameters )
{
	ResetAttractDemoTimeout();

	CBaseModFrame *newNav = m_Frames[ wt ].Get();
	bool setActiveWindow = true;

	// Get the background state of the old window
	CBaseModFrame *pOldWindow = m_Frames[ GetActiveWindowType() ].Get();
	bool bOldWindowUsedAlternateTiles = ( pOldWindow && pOldWindow->UsesAlternateTiles() );
	bool bUseAlternateTiles = false;


	// Window priority is used to track which windows are visible at all times
	// it is used to resolve the situations when a game requests an error box to popup
	// while a loading progress bar is active.
	// Windows with a higher priority force all other windows to get hidden.
	// After the high-priority window goes away it falls back to restore the low priority windows.
	WINDOW_PRIORITY nWindowPriority = WPRI_NORMAL;

	switch ( wt )
	{
	case WT_PASSWORDENTRY:
		setActiveWindow = false;
		break;

	case WT_GENERICWAITSCREEN:
	case WT_GENERICCONFIRMATION:
		// per the new ui scheme, all confirmations now hide previous
		hidePrevious = true;
		break;
	}

	switch ( wt )
	{
	case WT_GENERICWAITSCREEN:
		nWindowPriority = WPRI_WAITSCREEN;
		break;
	case WT_GENERICCONFIRMATION:
		nWindowPriority = WPRI_MESSAGE;
		break;
	case WT_LOADINGPROGRESS:
		nWindowPriority = WPRI_LOADINGPLAQUE;
		break;
	case WT_PASSWORDENTRY:
		nWindowPriority = WPRI_TOPMOST;
		break;
	case WT_TRANSITIONSCREEN:
		nWindowPriority = WPRI_TOPMOST;
		break;
	}

	if ( !newNav )
	{
		switch ( wt )
		{
#if 0
		case WT_ACHIEVEMENTS:
			m_Frames[wt] = new Achievements(this, "Achievements");
			break;
#endif
		case WT_AUDIO:
			m_Frames[wt] = new Audio(this, "Audio");
			break;

		case WT_AUDIOVIDEO:
			m_Frames[wt] = new AudioVideo(this, "AudioVideo");
			break;

		case WT_CLOUD:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new Cloud(this, "Cloud");
#endif
			break;

		case WT_CONTROLLER:
			m_Frames[wt] = new ControllerOptions( this, "ControllerOptions" );
			break;

		case WT_CONTROLLER_STICKS:
			m_Frames[wt] = new ControllerOptionsSticks( this, "ControllerOptionsSticks" );
			break;

		case WT_CONTROLLER_BUTTONS:
			m_Frames[wt] = new ControllerOptionsButtons( this, "ControllerOptionsButtons" );
			break;

		case WT_DOWNLOADS:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new Downloads(this, "Downloads");
#endif
			break;

		case WT_GAMELOBBY:
			m_Frames[wt] = new GameLobby(this, "GameLobby");
			break;

		case WT_GAMEOPTIONS:
			m_Frames[wt] = new GameOptions(this, "GameOptions");
			break;

		case WT_GAMESETTINGS:
			m_Frames[wt] = new GameSettings(this, "GameSettings");
			break;

		case WT_GENERICCONFIRMATION:
			m_Frames[wt] = new GenericConfirmation(this, "GenericConfirmation");
			break;

		case WT_INGAMEDIFFICULTYSELECT:
			m_Frames[wt] = new InGameDifficultySelect(this, "InGameDifficultySelect");
			break;
		case WT_INGAMEMAINMENU:
			m_Frames[wt] = new InGameMainMenu(this, "InGameMainMenu");
			break;

		case WT_INGAMECHAPTERSELECT:
			m_Frames[wt] = new InGameChapterSelect(this, "InGameChapterSelect");
			break;

		case WT_INGAMEKICKPLAYERLIST:
			m_Frames[wt] = new InGameKickPlayerList(this, "InGameKickPlayerList");
			break;

		case WT_VOTEOPTIONS:
			m_Frames[wt] = new VoteOptions(this, "VoteOptions");
			break;

		case WT_KEYBOARDMOUSE:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new KeyboardMouse(this, "KeyboardMouse");
#endif
			break;

		case WT_LOADINGPROGRESS:
			m_Frames[wt] = new LoadingProgress( this, "LoadingProgress" );
			break;

		case WT_MAINMENU:
			m_Frames[wt] = new MainMenu(this, "MainMenu");
			SetupBackgroundPresentation();
			break;

		case WT_ENDINGSPLITSCREEN:
			m_Frames[wt] = new EndingSplitscreenDialog( this, "EndingSplitscreen" );
			break;

		case WT_MULTIPLAYER:
			m_Frames[wt] = new Multiplayer(this, "Multiplayer");
			break;

		case WT_OPTIONS:
			m_Frames[wt] = new Options( this, "Options" );
			break;

		case WT_XBOXLIVE:
			m_Frames[wt] = new XboxLiveOptions( this, "XboxLIVE" );
			break;

		case WT_OPTIONSCLOUD:
			m_Frames[wt] = new OptionsCloud( this, "OptionsCloud" );
			break;

		case WT_SIGNINDIALOG:
			m_Frames[wt] = new SignInDialog(this, "SignInDialog");
			break;

		case WT_STEAMLINKDIALOG:
#if defined( _PS3 )
			m_Frames[wt] = new SteamLinkDialog(this, "steamlinkdialog");
#else
			Assert( 0 );
#endif
			break;

		case WT_GENERICWAITSCREEN:
			m_Frames[ wt ] = new GenericWaitScreen( this, "GenericWaitScreen" );
			break;

		case WT_PASSWORDENTRY:
			m_Frames[ wt ] = new PasswordEntry( this, "PasswordEntry" );
			break;

		case WT_ATTRACTSCREEN:
			m_Frames[ wt ] = new CAttractScreen( this, "AttractScreen" );
			SetupBackgroundPresentation();
			break;

		case WT_SINGLEPLAYER:
			m_Frames[ wt ] = new CSinglePlayer( this, "SinglePlayer" );
			break;

		case WT_COOPMODESELECT:
			m_Frames[ wt ] = new CCoopMode( this, "CoopMode" );
			break;

		case WT_STARTCOOPGAME:
			m_Frames[ wt ] = new CStartCoopGame( this, "StartCoopGame" );
			break;

		case WT_ALLGAMESEARCHRESULTS:
			m_Frames[ wt ] = new FoundGames( this, "FoundGames" );
			break;

		case WT_PVP_LOBBY:
			m_Frames[ wt ] = new PvpLobby( this, "pvplobby", pParameters );
			break;

		case WT_FOUNDPUBLICGAMES:
			m_Frames[ wt ] = new FoundPublicGames( this, "FoundPublicGames" );
			break;

		case WT_TRANSITIONSCREEN:
			m_Frames[wt] = new CTransitionScreen( this, "TransitionScreen" );
			break;

		case WT_VIDEO:
			m_Frames[wt] = new Video(this, "Video");
			break;

		case WT_STEAMCLOUDCONFIRM:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new SteamCloudConfirmation(this, "SteamCloudConfirmation");
#endif
			break;

		case WT_STEAMGROUPSERVERS:
			m_Frames[ wt ] = new FoundGroupGames( this, "FoundGames" );
			break;

		case WT_CUSTOMCAMPAIGNS:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[ wt ] = new CustomCampaigns( this, "CustomCampaigns" );
#endif
			break;

		case WT_DOWNLOADCAMPAIGN:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[ wt ] = new DownloadCampaign( this, "DownloadCampaign" );
#endif
			break;

		case WT_LEADERBOARD:
			m_Frames[ wt ] = new Leaderboard( this );
			break;

		case WT_ADDONS:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new Addons( this, "Addons" );
#endif
			break;

		case WT_ADDONASSOCIATION:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new AddonAssociation( this, "AddonAssociation" );
#endif
			break;

		case WT_GETLEGACYDATA:
#if defined( _GAMECONSOLE )
			// not for xbox
			Assert( 0 );
			break;
#else
			m_Frames[wt] = new GetLegacyData( this, "GetLegacyData" );
#endif
			break;

		case WT_NEWGAME:
			m_Frames[ wt ] = new CNewGameDialog( this, "NewGameDialog", false );
			break;

		case WT_SAVEGAME:
			m_Frames[wt] = new SaveLoadGameDialog( this, "SaveLoadGameDialog", true );
			break;

		case WT_LOADGAME:
			m_Frames[wt] = new SaveLoadGameDialog( this, "SaveLoadGameDialog", false );
			break;

		case WT_COMMENTARY:
			m_Frames[wt] = new CNewGameDialog( this, "NewGameDialog", true );
			break;

		case WT_CHALLENGEMODE:
			m_Frames[wt] = new CChallengeModeDialog( this, "ChallengeModeDialog" );
			break;

		case WT_PORTALLEADERBOARD:
			m_Frames[wt] = new CPortalLeaderboardPanel( this, "PortalLeaderboard", true );
			break;

		case WT_PORTALCOOPLEADERBOARD:
			m_Frames[wt] = new CPortalLeaderboardPanel( this, "PortalLeaderboard", false );
			break;

		case  WT_PORTALLEADERBOARDHUD:
			m_Frames[wt] = new CPortalHUDLeaderboard( this, "PortalHUDLeaderboard" );
			break;

		case WT_COOPEXITCHOICE:
			m_Frames[wt] = new CCoopExitChoice( this, "CoopExitChoice" );
			break;

		case WT_MOVIEPLAYER:
			m_Frames[wt] = new CMoviePlayer( this, "MoviePlayer" );
			break;

		case WT_AUTOSAVENOTICE:
			m_Frames[wt] = new CAutoSaveNotice( this, "AutoSaveNotice" );
			break;

		case WT_FADEOUTSTARTGAME:
			m_Frames[wt] = new CFadeOutStartGame( this, "FadeOutStartGame" );
			break;

		case WT_ADVANCEDVIDEO:
			m_Frames[wt] = new CAdvancedVideo( this, "AdvancedVideo" );
			break;

		case WT_KEYBINDINGS:
			m_Frames[wt] = new CKeyBindings( this, "KeyBindings" );
			break;

		case WT_SOUNDTEST:
			m_Frames[wt] = new CSoundTest( this, "SoundTest" );
			break;

		case WT_EXTRAS:
			m_Frames[wt] = new CExtrasDialog( this, "ExtrasDialog" );
			break;

		case WT_FADEOUTTOECONUI:
			m_Frames[wt] = new CFadeOutToEconUI( this, "FadeOutToEconUI" );
			break;
		default:
			Assert( false );	// unknown window type
			break;
		}

		//
		// Finish setting up the window
		//

		newNav = m_Frames[wt].Get();
		if ( !newNav )
			return NULL;

		newNav->SetUseAlternateTiles( bUseAlternateTiles );
		newNav->SetWindowPriority( nWindowPriority );
		newNav->SetWindowType(wt);
		newNav->SetVisible( false );
	}

	newNav->SetDataSettings( pParameters );

	if (setActiveWindow)
	{
		m_ActiveWindow[ nWindowPriority ] = wt;
		newNav->AddActionSignalTarget(this);
		newNav->SetCanBeActiveWindowType(true);
	}
	else if ( nWindowPriority == WPRI_MESSAGE )
	{
		m_ActiveWindow[ nWindowPriority ] = wt;
	}

	//
	// Now the window has been created, set it up
	//

	if ( UI_IsDebug() && (wt != WT_LOADINGPROGRESS) )
	{
		Msg( "[GAMEUI] OnOpen( `%s`, caller = `%s`, hidePrev = %d, setActive = %d, wt=%d, wpri=%d )\n",
			newNav->GetName(), caller ? caller->GetName() : "<NULL>", int(hidePrevious),
			int( setActiveWindow ), wt, nWindowPriority );
		KeyValuesDumpAsDevMsg( pParameters, 1 );
	}

	newNav->SetNavBack(caller);

	if (hidePrevious && caller != 0)
	{
		caller->SetVisible( false );
		if ( wt == WT_GENERICCONFIRMATION )
		{
			// Hide the waitscreen below message priority too
			if ( CBaseModFrame *pWaitscreen = GetWindow( WT_GENERICWAITSCREEN ) )
				pWaitscreen->SetVisible( false );
		}
	}
	else if (caller != 0)
	{
		caller->FindAndSetActiveControl();
		//caller->SetAlpha(128);
	}

	// Check if a higher priority window is open
	if ( GetActiveWindowPriority() > newNav->GetWindowPriority() )
	{
		if ( UI_IsDebug() )
		{
			CBaseModFrame *pOther;
			pOther = m_Frames[ GetActiveWindowType() ].Get();
			Warning( "[GAMEUI] OpenWindow: Another window %p`%s` is having priority %d, deferring `%s`!\n",
				pOther, pOther ? pOther->GetName() : "<<null>>",
				GetActiveWindowPriority(), newNav->GetName() );
		}

		// There's a higher priority window that was open at the moment,
		// hide our window for now, it will get restored later.
		// newNav->SetVisible( false );
	}
	else
	{
		CBaseModFooterPanel *pFooter = GetFooterPanel();
		if ( pFooter )
		{
			FooterType_t footerType = FOOTER_MENUS;
			if ( nWindowPriority == WPRI_MESSAGE )
				footerType = FOOTER_GENERICCONFIRMATION;
			else if ( nWindowPriority == WPRI_WAITSCREEN )
				footerType = FOOTER_GENERICWAITSCREEN;
			pFooter->SetFooterType( footerType );
		}

		newNav->InvalidateLayout(false, false);
		newNav->OnOpen();

		// Decide if we need to swap out the background video
		if ( newNav->UsesAlternateTiles() != bOldWindowUsedAlternateTiles )
		{
			OnTileSetChanged();
		}
	}

	GetTransitionEffectPanel()->SetExpectedDirection( true, wt );

	if ( UI_IsDebug() && (wt != WT_LOADINGPROGRESS) )
	{
		DbgShowCurrentUIState();
	}

	return newNav;
}

//-----------------------------------------------------------------------------
// Purpose: Called when our tile set changes from one type to another
//-----------------------------------------------------------------------------
void CBaseModPanel::OnTileSetChanged( void )
{
	TransitionToNewBackgroundMovie();
}

//=============================================================================
CBaseModFrame * CBaseModPanel::GetWindow( const WINDOW_TYPE& wt )
{
	return m_Frames[wt].Get();
}

//=============================================================================
WINDOW_TYPE CBaseModPanel::GetActiveWindowType()
{
	for ( int k = WPRI_COUNT; k -- > 0; )
	{
		if ( m_ActiveWindow[ k ] != WT_NONE )
		{
			CBaseModFrame *pFrame = m_Frames[ m_ActiveWindow[k] ].Get();
			if ( !pFrame || !pFrame->IsVisible() )
				continue;
			
			return m_ActiveWindow[ k ];
		}
	}
	return WT_NONE;
}

//=============================================================================
WINDOW_PRIORITY CBaseModPanel::GetActiveWindowPriority()
{
	for ( int k = WPRI_COUNT; k -- > 0; )
	{
		if ( m_ActiveWindow[ k ] != WT_NONE )
		{
			CBaseModFrame *pFrame = m_Frames[ m_ActiveWindow[k] ].Get();
			if ( !pFrame || !pFrame->IsVisible() )
				continue;

			return WINDOW_PRIORITY(k);
		}
	}
	return WPRI_NONE;
}

//=============================================================================
void CBaseModPanel::SetActiveWindow( CBaseModFrame * frame )
{
	if( !frame )
		return;
	
	m_ActiveWindow[ frame->GetWindowPriority() ] = frame->GetWindowType();

	if ( GetActiveWindowPriority() > frame->GetWindowPriority() )
	{
		if ( UI_IsDebug() )
		{
			CBaseModFrame *pOther;
			pOther = m_Frames[ GetActiveWindowType() ].Get();
			Warning( "[GAMEUI] SetActiveWindow: Another window %p`%s` is having priority %d, deferring `%s`!\n",
				pOther, pOther ? pOther->GetName() : "<<null>>",
				GetActiveWindowPriority(), frame->GetName() );
		}

		// frame->SetVisible( false );
	}
	else
	{
		frame->OnOpen();
	}
}

//=============================================================================
void CBaseModPanel::OnFrameClosed( WINDOW_PRIORITY pri, WINDOW_TYPE wt )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnFrameClosed( %d, %d )\n", pri, wt );
		DbgShowCurrentUIState();
	}

	// Mark the frame that just closed as NULL so that nobody could find it
	m_Frames[wt] = NULL;

	if ( m_bClosingAllWindows )
	{
		if ( UI_IsDebug() )
		{
			Msg( "[GAMEUI] Closing all windows\n" );
		}
		return;
	}

	if ( pri <= WPRI_NORMAL )
		return;

	for ( int k = 0; k < WPRI_COUNT; ++ k )
	{
		if ( m_ActiveWindow[k] == wt )
			m_ActiveWindow[k] = WT_NONE;
	}

	//
	// We only care to resurrect windows of lower priority when
	// higher priority windows close
	//

	for ( int k = WPRI_COUNT; k -- > 0; )
	{
		if ( m_ActiveWindow[ k ] == WT_NONE )
			continue;

		CBaseModFrame *pFrame = m_Frames[ m_ActiveWindow[k] ].Get();
		if ( !pFrame )
			continue;

		// pFrame->AddActionSignalTarget(this);

		CBaseModFooterPanel *pFooter = GetFooterPanel();
		if ( pFooter )
		{
			FooterType_t footerType = FOOTER_MENUS;
			if ( k == WPRI_MESSAGE )
				footerType = FOOTER_GENERICCONFIRMATION;
			else if ( k == WPRI_WAITSCREEN )
				footerType = FOOTER_GENERICWAITSCREEN;
			pFooter->SetFooterType( footerType );
		}

		pFrame->InvalidateLayout(false, false);
		pFrame->OnOpen();
		pFrame->SetVisible( true );
		pFrame->Activate();

		if ( UI_IsDebug() )
		{
			Msg( "[GAMEUI] CBaseModPanel::OnFrameClosed( %d, %d ) -> Activated `%s`, pri=%d\n",
				pri, wt, pFrame->GetName(), pFrame->GetWindowPriority() );
			DbgShowCurrentUIState();
		}

		return;
	}
}

void CBaseModPanel::DbgShowCurrentUIState()
{
	if ( UI_IsDebug() < 2 )
		return;

	Msg( "[GAMEUI] Priorities WT: " );
	for ( int i = 0; i < WPRI_COUNT; ++ i )
	{
		Msg( " %d ", m_ActiveWindow[i] );
	}
	Msg( "\n" );
	for ( int i = 0; i < WT_WINDOW_COUNT; ++ i )
	{
		CBaseModFrame *pFrame = m_Frames[i].Get();
		if ( pFrame )
		{
			Msg( "        %2d. `%s` pri%d vis%d\n",
				i, pFrame->GetName(), pFrame->GetWindowPriority(), pFrame->IsVisible() );
		}
		else
		{
			Msg( "        %2d. NULL\n", i );
		}
	}
}

bool CBaseModPanel::IsLevelLoading()
{
	return m_LevelLoading;
}

//=============================================================================
void CBaseModPanel::CloseAllWindows( int ePolicyFlags )
{
	CAutoPushPop< bool > auto_m_bClosingAllWindows( m_bClosingAllWindows, true );

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::CloseAllWindows( 0x%x )\n", ePolicyFlags );
	}

	// make sure we also close any active flyout menus that might be hanging out.
	FlyoutMenu::CloseActiveMenu();

	for (int i = 0; i < WT_WINDOW_COUNT; ++i)
	{
		CBaseModFrame *pFrame = m_Frames[i].Get();
		if ( !pFrame )
			continue;

		int nPriority = pFrame->GetWindowPriority();

		switch ( nPriority )
		{
		case WPRI_LOADINGPLAQUE:
			if ( !(ePolicyFlags & CLOSE_POLICY_EVEN_LOADING) )
			{
				if ( UI_IsDebug() )
				{
					Msg( "[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping loading type %d of priority %d.\n", i, nPriority );
				}

				continue;
				m_ActiveWindow[ WPRI_LOADINGPLAQUE ] = WT_NONE;
			}
			break;

		case WPRI_MESSAGE:
			if ( !(ePolicyFlags & CLOSE_POLICY_EVEN_MSGS) )
			{
				if ( UI_IsDebug() )
				{
					Msg( "[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping msgbox type %d of priority %d.\n", i, nPriority );
				}

				continue;
				m_ActiveWindow[ WPRI_MESSAGE ] = WT_NONE;
			}
			break;

		case WPRI_BKGNDSCREEN:
			if ( ePolicyFlags & CLOSE_POLICY_KEEP_BKGND )
			{
				if ( UI_IsDebug() )
				{
					Msg( "[GAMEUI] CBaseModPanel::CloseAllWindows() - Keeping bkgnd type %d of priority %d.\n", i, nPriority );
				}

				continue;
				m_ActiveWindow[ WPRI_BKGNDSCREEN ] = WT_NONE;
			}
			break;
		}

		// Close the window
		pFrame->Close();
		m_Frames[i] = NULL;
	}

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] After close all windows:\n" );
		DbgShowCurrentUIState();
	}

	m_ActiveWindow[ WPRI_NORMAL ] = WT_NONE;
}

#if defined( _X360 ) && defined( _DEMO )
void CBaseModPanel::OnDemoTimeout()
{
	if ( !engine->IsInGame() && !engine->IsConnected() && !engine->IsDrawingLoadingImage() )
	{
		// exit is terminal and unstoppable
		StartExitingProcess( false );
	}
	else
	{
		engine->ExecuteClientCmd( "disconnect" );
	}
}
#endif

bool CBaseModPanel::ActivateBackgroundEffects()
{
	bool bHasMovie = ( m_BIKHandle != BIKHANDLE_INVALID );

	// PC needs to keep starting music, can't loop MP3's
	// special logic for PC and ps3 to restart background music if stopped due to disconnect
	// 360 already shipped, so not changing
	if ( IsPS3() && bHasMovie && !m_flMusicFadeInTime && !IsBackgroundMusicPlaying() )
	{
		// game consoles fade non-playing music up
		if ( StartBackgroundMusic( 0 ) )
		{
			// do the fade in a little bit after the movie starts (needs to be stable)
			m_flMusicFadeInTime	= Plat_FloatTime() + TRANSITION_FROM_OVERLAY_DELAY_TIME;
		}
		else
		{
			// music was not started, don't fade
			m_flMusicFadeInTime = 0;
		}
	}

	if ( !bHasMovie )
	{
		// try to get the movie
		bHasMovie = InitBackgroundMovie();

		if ( !IsBackgroundMusicPlaying() )
		{
			// game consoles fade non-playing music up
			if ( StartBackgroundMusic( 0 ) )
			{
				// do the fade in a little bit after the movie starts (needs to be stable)
				m_flMusicFadeInTime	= Plat_FloatTime() + TRANSITION_FROM_OVERLAY_DELAY_TIME;
			}
			else
			{
				// music was not started, don't fade
				m_flMusicFadeInTime = 0;
			}
		}

#ifdef _X360
		if ( bHasMovie && g_pBIK->IsMovieResidentInMemory( m_BIKHandle ) )
		{
			// safely past any i/o that may need to be done
			// both music and movie should be in memory
			// the installer runs in the background during the main menu
			g_pXboxInstaller->Start();
		}
#endif
	}

	// Activate cloud syncing
	if ( g_pGameSteamCloudSync )
		g_pGameSteamCloudSync->Sync();

	// either the movie isn't there or it finished loading into memory
	// delay the overlay fading out to give the movie time to stabilize
	if ( !m_flOverlayFadeOutTime )
	{
		m_flOverlayFadeOutTime = Plat_FloatTime() + TRANSITION_FROM_OVERLAY_DELAY_TIME;
	}

	return bHasMovie;
}

//=============================================================================
void CBaseModPanel::OnGameUIActivated()
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnGameUIActivated( delay = %d )\n", m_DelayActivation );
	}

	if ( m_DelayActivation )
	{
		return;
	}

	m_nActivationCount++;

	COM_TimestampedLog( "CBaseModPanel::OnGameUIActivated()" );

#if defined( _GAMECONSOLE )
	if ( !engine->IsInGame() && !engine->IsConnected() && !engine->IsDrawingLoadingImage() )
	{
#if defined( _DEMO )
		if ( engine->IsDemoExiting() )
		{
			// just got activated, maybe from a disconnect
			// exit is terminal and unstoppable
			SetVisible( true );
			StartExitingProcess( false );
			return;
		}

		// ui valid can now adhere to demo timeout rules
		engine->EnableDemoTimeout( true );
#endif
	}
#endif

	SetVisible( true );

	WINDOW_TYPE currentActiveWindow = GetActiveWindowType();

	// This is terrible, why are we directing the window that we open when we are only trying to activate the UI?
	if ( WT_GAMELOBBY == currentActiveWindow )
	{
		return;
	}
	else if ( WT_LOADINGPROGRESS == currentActiveWindow )
	{
		// Ignore UI activations when loading poster is up
		return;
	}
	else if ( ( !m_LevelLoading && !engine->IsConnected() ) || GameUI().IsInBackgroundLevel() )
	{
		// may force a new active window
		// depending on policy window may already be present
		OpenFrontScreen();
	}
	else if ( engine->IsConnected() && !m_LevelLoading )
	{
		CBaseModFrame *pInGameMainMenu = m_Frames[ WT_INGAMEMAINMENU ].Get();
		if ( !pInGameMainMenu || !pInGameMainMenu->IsAutoDeleteSet() )
		{
			// Prevent in game menu from opening if it already exists!
			// It might be hiding behind a modal window that needs to keep focus
			OpenWindow( WT_INGAMEMAINMENU, 0 );		
		}
	}

	SetupBackgroundPresentation();
}

//=============================================================================
void CBaseModPanel::OnGameUIHidden()
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnGameUIHidden()\n" );
	}

#if defined( _X360 )
	// signal the installer to stop
	g_pXboxInstaller->Stop();
#endif

	if ( g_pGameSteamCloudSync )
		g_pGameSteamCloudSync->AbortAll();

// 	// We want to check here if we have any pending message boxes and
// 	// if so, then we cannot just simply destroy all the UI elements
// 	for ( int k = WPRI_NORMAL + 1; k < WPRI_LOADINGPLAQUE; ++ k )
// 	{
// 		WINDOW_TYPE wt = m_ActiveWindow[k];
// 		if ( wt != WT_NONE )
// 		{
// 			Msg( "[GAMEUI] CBaseModPanel::OnGameUIHidden() - not destroying UI because of wt %d pri %d\n",
// 				wt, k );
// 			return;
// 		}
// 	}

	SetVisible( false );
	
	// Notify the options dialog that game UI is closing
	if ( m_hOptionsDialog.Get() )
	{
		PostMessage( m_hOptionsDialog.Get(), new KeyValues( "GameUIHidden" ) );
	}

	// Notify the in game menu that game UI is closing
	CBaseModFrame *pInGameMainMenu = GetWindow( WT_INGAMEMAINMENU );
	if ( pInGameMainMenu )
	{
		PostMessage( pInGameMainMenu, new KeyValues( "GameUIHidden" ) );
	}

	// Close achievements
	if ( CBaseModFrame *pFrame = GetWindow( WT_ACHIEVEMENTS ) )
	{
		pFrame->Close();
	}

	// Free the movie resources
	ShutdownBackgroundMovie();

	GetTransitionEffectPanel()->TerminateEffect();

	// ensure any opaque overlay in canceled
	m_flOverlayFadeOutTime = -1;
	m_flMusicFadeInTime = 0;
	m_bAllowMovie = false;

	m_nActivationCount = 0;
}

void CBaseModPanel::OpenFrontScreen( bool bIgnoreMatchSession )
{
	WINDOW_TYPE frontWindow = WT_NONE;

	// make sure we are in the startup menu.
	if ( !GameUI().IsInBackgroundLevel() )
	{
		engine->ClientCmd( "startupmenu" );
	}

#ifdef _GAMECONSOLE
	if ( !bIgnoreMatchSession && g_pMatchFramework->GetMatchSession() )
	{
		Warning( "CBaseModPanel::OpenFrontScreen during active game ignored!\n" );
		return;
	}

	frontWindow = WT_ATTRACTSCREEN;
	if ( XBX_GetNumGameUsers() > 0 )
	{
		if ( !GetWindow( WT_ATTRACTSCREEN ) )
		{
			if ( XBX_GetNumGameUsers() < 2 || CUIGameData::Get()->AllowSplitscreenMainMenu() )
				frontWindow = WT_MAINMENU;
			else
				frontWindow = WT_ENDINGSPLITSCREEN;
		}
	}
#else
	frontWindow = WT_MAINMENU;
#endif

	if ( frontWindow != WT_NONE )
	{
		if ( GetActiveWindowType() != frontWindow )
		{
			CloseAllWindows();
			
			OpenWindow( frontWindow, NULL );
		}

		if ( bIgnoreMatchSession )
		{
			 GetTransitionEffectPanel()->PreventTransitions( false );
		}
	}
}

void CBaseModPanel::SetupBackgroundPresentation()
{
	static bool s_bFirstGameBootActivate = true;
	if ( s_bFirstGameBootActivate )
	{
		if ( !m_bSetup )
		{
			// There is a different order of startup sequence on consoles versus PC
			// likely due to the attract screen or timing. The consoles need to stall until
			// ApplySchemeSettings() hits, the PC hits ApplySchemeSettings first, then OnGameuiActivated().
			return;
		}

		// one time power up state
		s_bFirstGameBootActivate = false;

		SelectBackgroundPresentation();

		GetTransitionEffectPanel()->SetInitialState();

		// the overlay is initially the startup screen, the same placed by the non-interactive startup mode
		m_iFadeOutOverlayImageID = m_iStartupImageID;

		// hold the overlay opaque until the background effects are stable
		m_flOverlayFadeOutTime = 0;

		// allow the movie to play (it won't actually begin until the map has fully unloaded)
		m_bAllowMovie = true;
	}

	if ( m_LevelLoading || engine->IsConnected() || !IsVisible() || !m_nActivationCount )
	{
		return;
	}

	if ( m_bAllowMovie )
	{
		// already started or running
		return;
	}

	bool bNowAtMainMenu = GetWindow( WT_MAINMENU ) || 
							GetWindow( WT_ATTRACTSCREEN ) ||
							GetWindow( WT_ENDINGSPLITSCREEN ) ||
							( m_bMoveToEditorMainMenu );

	if ( bNowAtMainMenu )
	{
		// main menu wants the movie presentation
		SelectBackgroundPresentation();

		GetTransitionEffectPanel()->SetInitialState();

		// fade out from the background image into the background movie
		m_iFadeOutOverlayImageID = m_iBackgroundImageID;

		// hold the overlay opaque until the background effects are stable
		m_flOverlayFadeOutTime = 0;

		// allow the movie to play (it won't actually begin until the map has fully unloaded)
		m_bAllowMovie = true;
	}
	else
	{
		// unexpected state, no active overlay
		m_iFadeOutOverlayImageID = -1;
	}
}

//=============================================================================
void CBaseModPanel::RunFrame()
{
	if ( s_NavLock > 0 )
	{
		--s_NavLock;
	}

	GetAnimationController()->UpdateAnimations( Plat_FloatTime() );

	CBaseModFrame::RunFrameOnListeners();

	CUIGameData::Get()->RunFrame();

	if ( m_DelayActivation )
	{
		m_DelayActivation--;
		if ( !m_LevelLoading && !m_DelayActivation )
		{
			if ( UI_IsDebug() )
			{
				Msg( "[GAMEUI] Executing delayed UI activation\n");
			}
			OnGameUIActivated();
		}
	}

	bool bDoBlur = true;
	WINDOW_TYPE wt = GetActiveWindowType();
	switch ( wt )
	{
	case WT_NONE:
	case WT_MAINMENU:
	case WT_LOADINGPROGRESS:
	case WT_AUDIOVIDEO:
		bDoBlur = false;
		break;
	}

	if ( wt == WT_VIDEO )
	{
		const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();
		if ( !config.Windowed() )
		{
			// only the full screen mode can adjust brightness, shutdown the blur so user can modify
			bDoBlur = false;
		}
	}

	if ( GetWindow( WT_ATTRACTSCREEN ) || ( enginevguifuncs && !enginevguifuncs->IsGameUIVisible() ) )
	{
		// attract screen might be open, but not topmost due to notification dialogs
		bDoBlur = false;
	}

	if ( !bDoBlur )
	{
		bDoBlur = GameClientExports()->ClientWantsBlurEffect();
	}

	float nowTime = Plat_FloatTime();
	float deltaTime = nowTime - m_flLastBlurTime;
	if ( deltaTime > 0 )
	{
		m_flLastBlurTime = nowTime;
		m_flBlurScale += deltaTime * ( bDoBlur ? +3.0f : -3.0f );
		m_flBlurScale = clamp( m_flBlurScale, 0, 0.85f );
		engine->SetBlurFade( m_flBlurScale );
	}

	// Implement baseui fadein/out when overlays are active
	static bool s_bLastFrameVisible = true;
	static bool s_bVisibility = true;
	static float s_flVisibilityTime = 0.0f;
	static int s_nStartingAlpha = 255;
	// This is just a fancy way of saying true since this Swarm mod obviously doesn't run on a game console
	bool bVisibility =  true;//!IsGameConsole() || !CUIGameData::Get() || !( CUIGameData::Get()->IsXUIOpen() || CUIGameData::Get()->IsSteamOverlayActive() );
	if ( m_bHideAndFadeinLater )
	{
		bVisibility = false;
		m_bHideAndFadeinLater = false;
		s_bLastFrameVisible = false;
	}
	enum { ALPHA_VISIBLE = 255, ALPHA_HIDDEN = 1 };
	int nTargetAlpha = bVisibility ? ALPHA_VISIBLE : ALPHA_HIDDEN;
	if ( s_bVisibility != bVisibility )
	{
		if ( !s_bLastFrameVisible && !bVisibility )
			SetAlpha( ALPHA_HIDDEN ); // to prevent a flicker when UI activates just for overlay

		s_flVisibilityTime = Plat_FloatTime();
		if ( bVisibility )
		{
			s_flVisibilityTime += m_flFadeinDelayAfterOverlay;
			m_flFadeinDelayAfterOverlay = 0;
		}
		s_nStartingAlpha = GetAlpha();
		s_bVisibility = bVisibility;
	}
	if ( s_flVisibilityTime > 0 )
	{
		float flFactor = ( Plat_FloatTime() - s_flVisibilityTime ) / ui_fadexui_time.GetFloat();
		int nAlpha = ( int )( float( nTargetAlpha ) * flFactor + float( s_nStartingAlpha ) * ( 1.0 - flFactor ) );
		nAlpha = MIN( nAlpha, ALPHA_VISIBLE );
		nAlpha = MAX( nAlpha, ALPHA_HIDDEN );
		if ( nAlpha == nTargetAlpha )
			s_flVisibilityTime = 0;
		SetAlpha( nAlpha );
	}
	WINDOW_TYPE wtActiveWT = GetActiveWindowType();
	switch ( wtActiveWT )
	{
	case WT_LOADINGPROGRESS:
	case WT_MOVIEPLAYER:
	case WT_TRANSITIONSCREEN:
		SetAlpha( ALPHA_VISIBLE ); // during loading ensure that we never fade out
	}
	s_bLastFrameVisible = enginevguifuncs && enginevguifuncs->IsGameUIVisible();
}

void CBaseModPanel::AddFadeinDelayAfterOverlay( float flDelay, bool bHideAndFadeinLater )
{
	m_flFadeinDelayAfterOverlay += flDelay;
	m_bHideAndFadeinLater = m_bHideAndFadeinLater || bHideAndFadeinLater;
	if ( m_bHideAndFadeinLater )
	{
		SetAlpha( 1 );
	}
}

static IMatchSession *g_pMatchSessionChatRestrictionsAck;
static void ChatRestrictionsAcknowledged()
{
	g_pMatchSessionChatRestrictionsAck = g_pMatchFramework->GetMatchSession();
}


//=============================================================================
void CBaseModPanel::OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog )
{
	Assert( !m_LevelLoading );

#if defined( _X360 )
	// stop the installer
	g_pXboxInstaller->Stop();
#endif

	if ( g_pGameSteamCloudSync )
		g_pGameSteamCloudSync->AbortAll();

	SpewInstallStatus();

#if defined( _X360 )
	// If the installer has finished while we are in the menus, then this is the ONLY place we
	// know that there is no open files and we can redirect the search paths
	if ( g_pXboxInstaller->ForceCachePaths() )
	{
		// the search paths got changed
		// notify other systems who may have hooked absolute paths
		engine->SearchPathsChangedAfterInstall();
	}
#endif

	CloseAllWindows();

	// Stop the background movie
	ShutdownBackgroundMovie();

	// ensure any opaque overlay is canceled
	m_flOverlayFadeOutTime = -1;
	m_flMusicFadeInTime = 0;
	m_bAllowMovie = false;

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] OnLevelLoadingStarted - opening loading progress (%s)...\n",
			levelName ? levelName : "<< no level specified >>" );
	}

	LoadingProgress *pLoadingProgress = static_cast<LoadingProgress*>( OpenWindow( WT_LOADINGPROGRESS, 0 ) );
	
	KeyValues *pMissionInfo = NULL;
	KeyValues *pChapterInfo = NULL;
	
	char chGameMode[64] = {0};
	bool bIsCoop = false;

	//
	// If playing on listen server then "levelName" is set to the map being loaded,
	// so it is authoritative - it might be a background map or a real level.
	//
	if ( levelName )
	{
		//Check if this is a coop map:
		//Playing a campaign map, check the map prefix
		if ( StringHasPrefix( levelName, "mp_coop" ) )
		{
			bIsCoop = true;
		}

		// Derive the mission info from the server game details
		KeyValues *pGameSettings = g_pMatchFramework->GetMatchNetworkMsgController()->GetActiveServerGameDetails( NULL );
		if ( !pGameSettings )
		{
			// In this particular case we need to speculate about game details
			// this happens when user types "map c5m2 versus easy" from console, so there's no
			// active server spawned yet, nor is the local client connected to any server.
			// We have to force server DLL to apply the map command line to the settings and then
			// speculatively construct the settings key.
			if ( IServerGameDLL *pServerDLL = ( IServerGameDLL * ) g_pMatchFramework->GetMatchExtensions()->GetRegisteredExtensionInterface( INTERFACEVERSION_SERVERGAMEDLL ) )
			{
				KeyValues *pApplyServerSettings = new KeyValues( "::ExecGameTypeCfg" );
				KeyValues::AutoDelete autodelete_pApplyServerSettings( pApplyServerSettings );

				pApplyServerSettings->SetString( "map/mapname", levelName );

				pServerDLL->ApplyGameSettings( pApplyServerSettings );
			}

			static ConVarRef r_mp_gamemode( "mp_gamemode" );
			if ( r_mp_gamemode.IsValid() )
			{
				pGameSettings = new KeyValues( "CmdLineSettings" );
				pGameSettings->SetString( "game/mode", r_mp_gamemode.GetString() );
			}
		}
		
		KeyValues::AutoDelete autodelete_pGameSettings( pGameSettings );
		if ( pGameSettings )
		{
			// It is critical to get map info by the actual levelname that is being loaded, because
			// for level transitions the server is still in the old map and the game settings returned
			// will reflect the old state of the server.
			pChapterInfo = g_pMatchExt->GetMapInfoByBspName( pGameSettings, levelName, &pMissionInfo );
			Q_strncpy( chGameMode, pGameSettings->GetString( "game/mode", "" ), ARRAYSIZE( chGameMode ) );
		}
	}

	if ( bIsCoop )
	{
		Q_strncpy( chGameMode, "coop", sizeof( chGameMode ) );
	}
	else
	{
		chGameMode[ 0 ] = '\0';
	}
	
	IMatchSession *pSession = g_pMatchFramework->GetMatchSession();
	char const *szMapNameFromSession = "";
	if ( !pChapterInfo && pSession  )
	{
		if ( KeyValues *pSettings = pSession->GetSessionSettings() )
		{
			pChapterInfo = g_pMatchExt->GetMapInfo( pSettings, &pMissionInfo );
			szMapNameFromSession = pSettings->GetString( "game/map" );
			Q_strncpy( chGameMode, pSettings->GetString( "game/mode", "" ), ARRAYSIZE( chGameMode ) );
		}
	}

	//
	// If we are just loading into some unknown map, then fake chapter information
	// (static lifetime of fake keyvalues so that we didn't worry about ownership)
	//
	if ( !pChapterInfo )
	{
		static KeyValues *s_pFakeChapterInfo = new KeyValues( "1" );
		pChapterInfo = s_pFakeChapterInfo;
		pChapterInfo->SetString( "map", levelName ? levelName : szMapNameFromSession );
		pChapterInfo->SetString( "state", pSession ? pSession->GetSessionSettings()->GetString( "game/state", "lobby" ) : "lobby" );
	}

	pChapterInfo->SetString( "lastmap", m_LastLoadedLevelName.Get() );
	m_LastLoadedLevelName = pChapterInfo->GetString( "map", "" );
	ui_lastact_played.SetValue( ChapterToAct( MapNameToChapter( m_LastLoadedLevelName.Get() ) ) );

	pLoadingProgress->SetPosterData( pChapterInfo, chGameMode );
	pLoadingProgress->SetProgress( 0.0f );

	m_LevelLoading = true;

	if ( g_pISteamOverlayMgr )
		g_pISteamOverlayMgr->SetEnhancedOverlayInput( true );

#if defined( _PS3 ) && !defined( NO_STEAM )
	// See if we need to tell user about chat restrictions
	if ( pSession )
	{
		KeyValues *pSessionSettings = pSession->GetSessionSettings();
		if ( ( g_pMatchSessionChatRestrictionsAck != pSession ) &&
			!V_stricmp( pSessionSettings->GetString( "system/network" ), "LIVE" ) &&
			steamapicontext->SteamFriends()->GetUserRestrictions() )
		{
			// Pop a message about chat restrictions
			GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION,
				CUIGameData::Get()->GetParentWindowForSystemMessageBox(), false ) );

			GenericConfirmation::Data_t data;
			data.pWindowTitle = "#Valve_Chat_Messages";
			data.pMessageText = "#L4D360UI_Steam_Error_ChatRestriction";
			data.bOkButtonEnabled = true;
			data.pfnOkCallback = ChatRestrictionsAcknowledged;
			confirmation->SetUsageData(data);
		}
	}
#endif
}

void CBaseModPanel::OnEngineLevelLoadingSession( KeyValues *pEvent )
{
	float flProgress = 0.0f;
	if ( LoadingProgress *pLoadingProgress = static_cast<LoadingProgress*>( GetWindow( WT_LOADINGPROGRESS ) ) )
	{
		flProgress = pLoadingProgress->GetProgress();
		pLoadingProgress->Close();
		m_Frames[ WT_LOADINGPROGRESS ] = NULL;
	}

	CloseAllWindows( CLOSE_POLICY_DEFAULT );

	if ( LoadingProgress *pLoadingProgress = static_cast<LoadingProgress*>( OpenWindow( WT_LOADINGPROGRESS, NULL ) ) )
	{
		pLoadingProgress->SetProgress( flProgress );
	}
}

//=============================================================================
void CBaseModPanel::OnLevelLoadingFinished( KeyValues *kvEvent )
{
	int bError = kvEvent->GetInt( "error" );
	const char *failureReason = kvEvent->GetString( "reason" );
	
	Assert( m_LevelLoading );

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnLevelLoadingFinished( %s, %s )\n", bError ? "Had Error" : "No Error", failureReason );
	}

#if defined( _GAMECONSOLE )
	if ( GameUI().IsInBackgroundLevel() )
	{
		// apply background level entity
		g_BackgroundMapActiveControlManager.Apply();
	}
#endif

	LoadingProgress *pLoadingProgress = static_cast<LoadingProgress*>( GetWindow( WT_LOADINGPROGRESS ) );
	if ( pLoadingProgress )
	{
		pLoadingProgress->SetProgress( 1.0f );

		// always close loading progress, this frees costly resources
		pLoadingProgress->Close();
	}

	m_LevelLoading = false;

	CBaseModFrame *pFrame = CBaseModPanel::GetSingleton().GetWindow( WT_GENERICCONFIRMATION );
	if ( !pFrame )
	{
		// no confirmation up, hide the UI
		GameUI().HideGameUI();
	}

	// if we are loading into the lobby, then skip the UIActivation code path
	// this can happen if we accepted an invite to player who is in the lobby while we were in-game
	if ( WT_GAMELOBBY != GetActiveWindowType() )
	{
		// if we are loading into the front-end, then activate the main menu (or attract screen, depending on state)
		// or if a message box is pending force open game ui
		if ( GameUI().IsInBackgroundLevel() || pFrame )
		{
			GameUI().OnGameUIHidden();		// force re-activation
			GameUI().OnGameUIActivated();
		}
	}

	if ( bError )
	{
		GenericConfirmation* pMsg = ( GenericConfirmation* ) OpenWindow( WT_GENERICCONFIRMATION, NULL, false );		
		if ( pMsg )
		{
			GenericConfirmation::Data_t data;
			data.pWindowTitle = "#L4D360UI_MsgBx_DisconnectedFromServer";			
			data.bOkButtonEnabled = true;
			data.pMessageText = failureReason;
			pMsg->SetUsageData( data );
		}		
	}

	if ( g_pISteamOverlayMgr )
		g_pISteamOverlayMgr->SetEnhancedOverlayInput( false );
}

class CMatchSessionCreationAsyncOperation : public IMatchAsyncOperation
{
public:
	CMatchSessionCreationAsyncOperation() : m_eState( AOS_RUNNING ) {}

public:
	virtual bool IsFinished() { return false; }
	virtual AsyncOperationState_t GetState() { return m_eState; }
	virtual uint64 GetResult() { return 0ull; }
	virtual void Abort();
	virtual void Release() { Assert( 0 ); } // we are a global object, cannot release

public:
	IMatchAsyncOperation * Prepare() { m_eState = AOS_RUNNING; return this; }

protected:
	AsyncOperationState_t m_eState;
}
g_MatchSessionCreationAsyncOperation;

class CMatchSessionJoinRemoteHostCallbackInterface : public IWaitscreenCallbackInterface
{
protected:
	void TimedOut()
	{
		// Go to the main menu
		g_pMatchFramework->CloseSession();
		CBaseModPanel::GetSingleton().CloseAllWindows( CBaseModPanel::CLOSE_POLICY_EVEN_MSGS );

		// Show the message box
		GenericConfirmation* confirmation = static_cast<GenericConfirmation*>( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, NULL, false ) );
		CBaseModPanel::GetSingleton().OpenWindow( WT_MAINMENU, NULL );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_MsgBx_DisconnectedFromSession";	// "Disconnect"
		data.pMessageText = "#SessionError_NoPartner";
		data.bOkButtonEnabled = true;
		confirmation->SetUsageData(data);
	}
	virtual void OnThink()
	{
		if ( Plat_FloatTime() - m_flStartTimeStamp > ui_lobby_jointimeout.GetFloat() )
			TimedOut();
	}

public:
	IWaitscreenCallbackInterface * Prepare() { m_flStartTimeStamp = Plat_FloatTime(); return this; }

protected:
	float m_flStartTimeStamp;
}
g_MatchSessionJoinRemoteHostCallbackInterface;

void CMatchSessionCreationAsyncOperation::Abort()
{
	m_eState = AOS_ABORTING;
	
	Assert( g_pMatchFramework->GetMatchSession() );
	g_pMatchFramework->CloseSession();

	CBaseModPanel::GetSingleton().CloseAllWindows();
	CBaseModPanel::GetSingleton().OpenFrontScreen();
}


void CBaseModPanel::OnClientReady()
{
	// Wait screen options to cancel async process
	KeyValues *pSettings = new KeyValues( "WaitScreen" );
	KeyValues::AutoDelete autodelete_pSettings( pSettings );
	pSettings->SetPtr( "options/asyncoperation", g_MatchSessionCreationAsyncOperation.Prepare() );
	pSettings->SetPtr( "options/waitscreencallback", g_MatchSessionJoinRemoteHostCallbackInterface.Prepare() );

	// We are joining a lobby
	CUIGameData::Get()->OpenWaitScreen( "#Portal2UI_Matchmaking_JoiningGame", 0.0f, pSettings );

	// Tell the host to start the game since client is all ready
	KeyValues *kvCommand = new KeyValues( "Portal2::ClientReadyToStart" );
	kvCommand->SetString( "run", "host" );
	kvCommand->SetUint64( "clxuid", g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( XBX_GetUserId( 0 ) )->GetXUID() );
	kvCommand->SetString( "clflags", "default" );
	g_pMatchFramework->GetMatchSession()->Command( KeyValues::AutoDeleteInline( kvCommand ) );
}


void CBaseModPanel::OnEvent( KeyValues *pEvent )
{
	char const *szEvent = pEvent->GetName();

	if ( !V_stricmp( "OnSysXUIEvent", szEvent ) )
	{
		// XUI blade activity resets the attract demo timeout
		ResetAttractDemoTimeout();
	}

	// Must make sure that UI is activated before events start pouring in
	if ( m_DelayActivation )
	{
		m_DelayActivation = 1;
		RunFrame();
	}

	if ( !Q_stricmp( "OnMatchSessionUpdate", szEvent ) )
	{
		char const *szState = pEvent->GetString( "state", "" );
		if ( !Q_stricmp( "ready", szState ) )
		{
			// Session has finished creating:
			IMatchSession *pSession = g_pMatchFramework->GetMatchSession();
			if ( !pSession )
				return;

			KeyValues *pSettings = pSession->GetSessionSettings();
			if ( !pSettings )
				return;

			char const *szNetwork = pSettings->GetString( "system/network", "" );
			if ( !Q_stricmp( "offline", szNetwork ) )
				// Offline sessions are automatically started by caller
				return;
			
			// Special case when we are creating a public session after empty search
			if ( !Q_stricmp( pSettings->GetString( "options/createreason" ), "searchempty" ) &&
				!Q_stricmp( pSettings->GetString( "system/access" ), "public" ) )
			{
				// Wait screen options to cancel async processt
				KeyValues *pSettings = new KeyValues( "WaitScreen" );
				KeyValues::AutoDelete autodelete_pSettings( pSettings );
				pSettings->SetPtr( "options/asyncoperation", g_MatchSessionCreationAsyncOperation.Prepare() );

				// We are creating a public lobby after our search turned out empty
				char const *szWaitScreenText = "#Portal2UI_Matchmaking_Hosting_QuickMatch";
				CUIGameData::Get()->OpenWaitScreen( szWaitScreenText, 0.0f, pSettings );

				// Delete the "createreason" key from the session settings
				pSession->UpdateSessionSettings( KeyValues::AutoDeleteInline( KeyValues::FromString( "delete",
					" delete { options { createreason delete } } " ) ) );

				return;
			}

			
			const char* szGameMode = pSettings->GetString( "game/mode" );

			// If we are a client then the game will automatically start, so just hold the waitscreen
			if ( Q_stricmp( "host", g_pMatchFramework->GetMatchSession()
				->GetSessionSystemData()->GetString( "type", "host" ) ) )
			{

				OnClientReady();

				return;
			}

			// We have created a session
			CloseAllWindows();

			// For single player or splitscreen, just let the game start
			if ( !V_stricmp( szGameMode, "sp" ) ||
				 ( !V_stricmp( szNetwork, "lan" ) && ( XBX_GetNumGameUsers() >= 2 ) ) )
			{
				pSession->Command( KeyValues::AutoDeleteInline( new KeyValues( "Start" ) ) );
				return;
			}

			OpenWindow( WT_PVP_LOBBY, NULL, true, pSettings ); // derive from session

			// Check for a special case when we lost connection to host and that's why we are going to lobby
			if ( KeyValues *pOnEngineDisconnectReason = g_pMatchFramework->GetEventsSubscription()->GetEventData( "OnEngineDisconnectReason" ) )
			{
				if ( !Q_stricmp( "lobby", pOnEngineDisconnectReason->GetString( "disconnecthdlr" ) ) )
				{
					CUIGameData::Get()->OpenWaitScreen( "#L4D360UI_MsgBx_DisconnectedFromServer" );
					CUIGameData::Get()->CloseWaitScreen( NULL, NULL );
				}
			}
		}
		else if ( !Q_stricmp( "created", szState ) )
		{
			//
			// This section of code catches when we just connected to a lobby that
			// is playing a campaign that we do not have installed.
			// In this case we abort loading, forcefully close all windows including
			// loading poster and game lobby and display the download info msg.
			//

			IMatchSession *pSession = g_pMatchFramework->GetMatchSession();
			if ( !pSession )
				return;

			KeyValues *pSettings = pSession->GetSessionSettings();

			KeyValues *pInfoMission = NULL;
			KeyValues *pInfoChapter = GetMapInfoRespectingAnyChapter( pSettings, &pInfoMission );

			if ( 1 ) // Portal 2 doesn't have custom campaigns that are quickmatchable
				return;

			// If we do not have a valid chapter/mission, then we need to quit
			if ( pInfoChapter && pInfoMission &&
				( !*pInfoMission->GetName() || pInfoMission->GetInt( "version" ) == pSettings->GetInt( "game/missioninfo/version", -1 ) ) )
				return;

			if ( pSettings )
				pSettings = pSettings->MakeCopy();

			engine->ExecuteClientCmd( "disconnect" );
			g_pMatchFramework->CloseSession();

			CloseAllWindows( CLOSE_POLICY_EVEN_MSGS | CLOSE_POLICY_EVEN_LOADING );
			OpenFrontScreen();

			const char *szCampaignWebsite = pSettings->GetString( "game/missioninfo/website", NULL );
			if ( szCampaignWebsite && *szCampaignWebsite )
			{
				OpenWindow( WT_DOWNLOADCAMPAIGN,
					GetWindow( CBaseModPanel::GetSingleton().GetActiveWindowType() ),
					true, pSettings );
			}
			else
			{
				GenericConfirmation::Data_t data;

				data.pWindowTitle = "#L4D360UI_Lobby_MissingContent";
				data.pMessageText = "#L4D360UI_Lobby_MissingContent_Message";
				data.bOkButtonEnabled = true;

				GenericConfirmation* confirmation = 
					static_cast< GenericConfirmation* >( OpenWindow( WT_GENERICCONFIRMATION, NULL, true ) );

				confirmation->SetUsageData(data);
			}
		}
		else if ( !Q_stricmp( "progress", szState ) )
		{
			struct WaitText_t
			{
				char const *m_szProgress;
				char const *m_szText;
				int m_eCloseAllWindowsFlags;
			};

			int eDefaultFlags = CLOSE_POLICY_EVEN_MSGS | CLOSE_POLICY_KEEP_BKGND;
			WaitText_t arrWaits[] = {
				// { "creating",	"#Matchmaking_creating",	eDefaultFlags },
				{ "creating",	"#L4D360UI_StartCoopGame_Searching",	eDefaultFlags },
				{ "joining",	"#Matchmaking_joining",		eDefaultFlags },
				{ "searching",	"#Matchmaking_searching",	eDefaultFlags },
			};
			if ( IsX360() && g_pMatchFramework->GetMatchSession() &&
				!V_stricmp( "lan", g_pMatchFramework->GetMatchSession()->GetSessionSettings()->GetString( "system/network" ) ) )
			{
				arrWaits[0].m_szText = "#L4D360UI_FoundGames_Searching";
			}

			char const *szProgress = pEvent->GetString( "progress", "" );
			WaitText_t const *pWaitText = NULL;
			for ( int k = 0; k < ARRAYSIZE( arrWaits ); ++ k )
			{
				if ( !Q_stricmp( arrWaits[k].m_szProgress, szProgress ) )
				{
					pWaitText = &arrWaits[k];
					break;
				}
			}

			// Wait screen options to cancel async process
			KeyValues *pSettings = new KeyValues( "WaitScreen" );
			KeyValues::AutoDelete autodelete_pSettings( pSettings );
			pSettings->SetPtr( "options/asyncoperation", g_MatchSessionCreationAsyncOperation.Prepare() );

			// For PC we don't want to cancel lobby creation
			if ( IsPC() && !Q_stricmp( "creating", szProgress ) )
				pSettings = NULL;

			// Put up a wait screen
			if ( pWaitText )
			{
				if ( pWaitText->m_eCloseAllWindowsFlags != -1 )
					CloseAllWindows( pWaitText->m_eCloseAllWindowsFlags );

				char const *szWaitScreenText = pWaitText->m_szText;
				float flMinDisplayTime = 0.0f;
				
				if ( IMatchSession *pMatchSession = g_pMatchFramework->GetMatchSession() )
				{
					KeyValues *pMatchSettings = pMatchSession->GetSessionSettings();
					if ( !Q_stricmp( szProgress, "creating" ) &&
						 !Q_stricmp( pMatchSettings->GetString( "options/createreason" ), "searchempty" ) &&
						 !Q_stricmp( pMatchSettings->GetString( "system/access" ), "public" ) )
					{
						// We are creating a public lobby after our search turned out empty
						szWaitScreenText = "#Portal2UI_Matchmaking_Hosting_QuickMatch";
					}
				}

				CUIGameData::Get()->OpenWaitScreen( szWaitScreenText, flMinDisplayTime, pSettings );
			}
			else if ( !Q_stricmp( "searchresult", szProgress ) )
			{
				char const *arrText[] = { "#Matchmaking_SearchResults",
					"#Matchmaking_SearchResults1", "#Matchmaking_SearchResults2", "#Matchmaking_SearchResults3" };
				int numResults = pEvent->GetInt( "numResults", 0 );
				if ( numResults < 0 || numResults >= ARRAYSIZE( arrText ) )
					numResults = 0;
				CUIGameData::Get()->OpenWaitScreen( arrText[numResults], 0.0f, pSettings );
			}
		}
	}
	else if ( !Q_stricmp( "OnEngineLevelLoadingSession", szEvent ) )
	{
		OnEngineLevelLoadingSession( pEvent );
	}
	else if ( !Q_stricmp( "OnEngineLevelLoadingFinished", szEvent ) )
	{
		OnLevelLoadingFinished( pEvent );
	}
	else if ( !Q_stricmp( "Command::Portal2::ClientReadyToStart", szEvent ) )
	{
		// Client is now ready to start the game - let's start it!
		IMatchSession *pIMatchSession = g_pMatchFramework->GetMatchSession();
		if ( !pIMatchSession )
			return;
		KeyValues *pSettings = pIMatchSession->GetSessionSettings();
		if ( pSettings->GetInt( "members/numMachines" ) > 1 )
		{
			// Make the session private automatically
			if ( !Q_stricmp( pSettings->GetString( "system/network", "LIVE" ), "LIVE" ) )
			{
				KeyValues *pSettings = new KeyValues( "update" );
				KeyValues::AutoDelete autodelete( pSettings );
				pSettings->SetString( "update/system/access", "private" );
				pIMatchSession->UpdateSessionSettings( pSettings );
			}

			// Check that maybe a game console is the client who is connecting to us
			// then as a PC we need to relinquish our host status and let the console
			// be the server
			//if ( !IsGameConsole() )
			{
				XUID clxuid = pEvent->GetUint64( "clxuid" );
				KeyValues *pClMachine = NULL;
				KeyValues *pClPlayer = SessionMembersFindPlayer( pSettings, clxuid, &pClMachine );
				if ( pClPlayer && pClMachine
					&& !Q_stricmp( pEvent->GetString( "clflags" ), "console" ) )
				{
					// Wait screen options to cancel async process
					KeyValues *pSettings = new KeyValues( "WaitScreen" );
					KeyValues::AutoDelete autodelete_pSettings( pSettings );
					pSettings->SetPtr( "options/asyncoperation", g_MatchSessionCreationAsyncOperation.Prepare() );
					pSettings->SetPtr( "options/waitscreencallback", g_MatchSessionJoinRemoteHostCallbackInterface.Prepare() );

					// We are joining a lobby
					char const *szWaitScreenText = "#Portal2UI_Matchmaking_JoiningGame";
					CUIGameData::Get()->OpenWaitScreen( szWaitScreenText, 0.0f, pSettings );

					KeyValues *kvParamsMigrate = new KeyValues( "MigrateHostToClient" );
					kvParamsMigrate->SetUint64( "session", (uint64)(uint32)pIMatchSession );
					kvParamsMigrate->SetUint64( "clxuid", clxuid );
					this->PostMessage( this, kvParamsMigrate );
					return;
				}
			}

			if ( !ui_lobby_noautostart.GetBool() )
			{
				pIMatchSession->Command( KeyValues::AutoDeleteInline( new KeyValues( "Start" ) ) );
			}
			else
			{
				Warning( "ui_lobby_noautostart prevented map start!\n" );
			}
		}
	}
}

void CBaseModPanel::OnKeyCodePressed( KeyCode code )
{
	// any key activity anywhere resets the attract timeout
	ResetAttractDemoTimeout();

	BaseClass::OnKeyCodePressed( code );
}

void CBaseModPanel::MigrateHostToClient( KeyValues *params )
{
	IMatchSession *pIMatchSession = g_pMatchFramework->GetMatchSession();
	if ( !pIMatchSession || ( (uint64)(uint32)pIMatchSession != params->GetUint64( "session" ) ) )
		return;

	// Relinquish the host rights
	KeyValues *kvMigrate = new KeyValues( "Migrate" );
	KeyValues::AutoDelete autodelete_kvMigrate( kvMigrate );
	kvMigrate->SetString( "run", "host" );
	kvMigrate->SetUint64( "runxuid", params->GetUint64( "clxuid" ) );
	kvMigrate->SetUint64( "xuid", params->GetUint64( "clxuid" ) );
	kvMigrate->SetString( "migrate", "host>client" );
	pIMatchSession->Command( kvMigrate );

	// We have a new session object now!
	pIMatchSession = g_pMatchFramework->GetMatchSession();
	if ( !pIMatchSession )
		return;

	// Migrate the lobby off to console
	kvMigrate->SetString( "run", "xuid" );
	kvMigrate->SetString( "migrate", "client>host" );
	pIMatchSession->Command( kvMigrate );

	// Tell the host to start the game since client is all ready
	KeyValues *kvCommand = new KeyValues( "Portal2::ClientReadyToStart" );
	kvCommand->SetString( "run", "host" );
	kvCommand->SetUint64( "clxuid", g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( XBX_GetUserId( 0 ) )->GetXUID() );
	kvCommand->SetString( "clflags", "default" );
	pIMatchSession->Command( KeyValues::AutoDeleteInline( kvCommand ) );
}

//=============================================================================
bool CBaseModPanel::UpdateProgressBar( float progress, const char *statusText )
{
	if ( !m_LevelLoading )
	{
		// Assert( m_LevelLoading );
		// Warning( "WARN: CBaseModPanel::UpdateProgressBar called outside of level loading, discarded!\n" );
		return false;
	}

	LoadingProgress *pLoadingProgress = static_cast<LoadingProgress*>( GetWindow( WT_LOADINGPROGRESS ) );
	if ( !pLoadingProgress )
	{
		pLoadingProgress = static_cast<LoadingProgress*>( OpenWindow( WT_LOADINGPROGRESS, 0 ) );
	}

	// Even if the progress hasn't advanced, we want to go ahead and refresh if it has been more than 1/10 seconds since last refresh to keep the spinny thing going.
	static float s_LastEngineTime = -1.0f;
	// clock the anim at 10hz
	float time = Plat_FloatTime();
	float deltaTime = time - s_LastEngineTime;

	if ( pLoadingProgress && ( ( pLoadingProgress->IsDrawingProgressBar() && ( pLoadingProgress->GetProgress() < progress ) ) || ( deltaTime > 0.06f ) ) )
	{
		// update progress
		pLoadingProgress->SetProgress( progress );
		s_LastEngineTime = time;
		return true;
	}

	// no update required
	return false;
}

void CBaseModPanel::SetOkButtonEnabled( bool bEnabled )
{
	if ( m_FooterPanel )
	{
		FooterButtons_t buttons = m_FooterPanel->GetButtons();
		if ( bEnabled )
			buttons |= FB_ABUTTON;
		else
			buttons &= ~FB_ABUTTON;
		m_FooterPanel->SetButtons( buttons, m_FooterPanel->GetFormat() );
	}
}

void CBaseModPanel::SetCancelButtonEnabled( bool bEnabled )
{
	if ( m_FooterPanel )
	{
		FooterButtons_t buttons = m_FooterPanel->GetButtons();
		if ( bEnabled )
			buttons |= FB_BBUTTON;
		else
			buttons &= ~FB_BBUTTON;
		m_FooterPanel->SetButtons( buttons, m_FooterPanel->GetFormat() );
	}
}

BaseModUI::CBaseModFooterPanel* CBaseModPanel::GetFooterPanel()
{
	// EVIL HACK
	if ( !this )
	{
		Assert( 0 );
		Warning( "CBaseModPanel::GetFooterPanel() called on NULL CBaseModPanel!!!\n" );
		return NULL;
	}
	return m_FooterPanel;
}

void CBaseModPanel::SetLastActiveUserId( int userId )
{
	if ( m_lastActiveUserId != userId )
	{
		DevWarning( "SetLastActiveUserId: %d -> %d\n", m_lastActiveUserId, userId );
	}

	m_lastActiveUserId = userId;
}

int CBaseModPanel::GetLastActiveUserId( )
{
	return m_lastActiveUserId;
}

//-----------------------------------------------------------------------------
// Purpose: moves the game menu button to the right place on the taskbar
//-----------------------------------------------------------------------------
static void BaseUI_PositionDialog(vgui::PHandle dlg)
{
	if (!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds( x, y, ww, wt );
	dlg->GetSize(wide, tall);

	// Center it, keeping requested size
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}


//=============================================================================
void CBaseModPanel::OpenOptionsDialog( Panel *parent )
{
	if ( IsPC() )
	{			
		if ( !m_hOptionsDialog.Get() )
		{
			m_hOptionsDialog = new COptionsDialog( parent );
			BaseUI_PositionDialog( m_hOptionsDialog );
		}

		m_hOptionsDialog->Activate();
	}
}

//=============================================================================
void CBaseModPanel::OnNavigateTo( const char* panelName )
{
	CBaseModFrame* currentFrame = 
		static_cast<CBaseModFrame*>(FindChildByName(panelName, false));

	if (currentFrame && currentFrame->GetCanBeActiveWindowType())
	{
		m_ActiveWindow[ currentFrame->GetWindowPriority() ] = currentFrame->GetWindowType();
	}
}

extern void RadialMenuMouseCallback( uint8 *pData, size_t iSize );

//=============================================================================
void CBaseModPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );

	// there is an overlay effect to opaque transitions
	SetPostChildPaintEnabled( true );

	m_hDefaultFont = pScheme->GetFont( "DefaultSystemUI", true );

	SetBgColor( pScheme->GetColor( "Blank", Color( 0, 0, 0, 0 ) ) );

	// need the startup image instantly to take over from the non-interactive refresh on first paint
#if 0 // FIXME: We're missing engine functions!
	char filename[MAX_PATH];
	engine->GetStartupImage( filename, sizeof( filename ) ); // TODO: There must be a way to do this!
	m_iStartupImageID = surface()->CreateNewTextureID();

	surface()->DrawSetTextureFile( m_iStartupImageID, filename, true, false );
#else
	m_iStartupImageID = surface()->CreateNewTextureID();
#endif

	m_iProductImageID = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iProductImageID, "vgui/portal2logo", true, false );
	m_iAltProductImageID = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iAltProductImageID, "logo/logo_aperture", true, false );

	m_iCloudImageID[0] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCloudImageID[0], "vgui/resource/icon_cloud_small", true, false );
	m_iCloudImageID[1] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iCloudImageID[1], "vgui/resource/icon_cloud_small", true, false );

	m_nProductImageX = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "Logo.X" ) ) );
	m_nProductImageY = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "Logo.Y" ) ) );
	m_nProductImageWide = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "Logo.Width" ) ) );
	m_nProductImageTall = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "Logo.Height" ) ) );

	m_iCloudPosition[0] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloud.X" ) ) );
	m_iCloudPosition[1] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloud.Y" ) ) );
	m_iCloudPosition[2] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloud.Width" ) ) );
	m_iCloudPosition[3] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloud.Height" ) ) );

	m_iCloudProgressPosition[0] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudProgress.X" ) ) );
	m_iCloudProgressPosition[1] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudProgress.Y" ) ) );
	m_iCloudProgressPosition[2] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudProgress.Width" ) ) );
	m_iCloudProgressPosition[3] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudProgress.Height" ) ) );

	m_hSteamCloudFont = pScheme->GetFont( pScheme->GetResourceString( "SteamCloudText.Font" ), true );
	m_clrSteamCloudText = pScheme->GetColor( "SteamCloudText.Color", m_clrSteamCloudText );
	m_iCloudTextPosition[0] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudText.X" ) ) );
	m_iCloudTextPosition[1] = vgui::scheme()->GetProportionalScaledValue( atoi( pScheme->GetResourceString( "SteamCloudText.Y" ) ) );

	m_clrCloudRemaining = pScheme->GetColor( "SteamCloudProgress.ColorRemaining", m_clrCloudRemaining );
	m_clrCloudDone = pScheme->GetColor( "SteamCloudProgress.ColorDone", m_clrCloudDone );
	m_clrCloudDoneFade = pScheme->GetColor( "SteamCloudProgress.ColorDoneFade", m_clrCloudDoneFade );

	// preload the common images needed by other UI that needs to be done early
	PrecacheCommonImages();

	// Recalculate the movie parameters at next render
	m_flU1 = m_flV1 = 0.0f;
	
	// set up the radial menu to record correctly in playest demos - must be done before demo starts recording
	engine->RegisterDemoCustomDataCallback( "RadialMenuMouseCallback", RadialMenuMouseCallback );

	// caller's can know ApplySchemeSettings() has finalized
	// and thus rely on the above members
	m_bSetup = true;

	SetupBackgroundPresentation();
}

int CBaseModPanel::GetImageId( const char *pImageName )
{
	int nImageId = vgui::surface()->DrawGetTextureId( pImageName );
	if ( nImageId == -1 )
	{
		nImageId = vgui::surface()->CreateNewTextureID();
		vgui::surface()->DrawSetTextureFile( nImageId, pImageName, true, false );	
	}

	return nImageId;
}

void CBaseModPanel::PrecacheCommonImages()
{
	m_CommonImages.Purge();

	m_CommonImages.AddToTail( GetImageId( "vgui/chapters/vignette" ) );

	// add the sp chapters
	int nNumChapters = GetNumChapters();
	for ( int i = 0; i < nNumChapters; i++ )
	{
		// get the chapter images
		m_CommonImages.AddToTail( GetImageId( CFmtStr( "vgui/chapters/chapter%d", i + 1 ) ) );
	}

	m_CommonImages.AddToTail( GetImageId( "vgui/chapters/coopcommentary_chapter1" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/chapters/coopcommentary_chapter2" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/chapters/coopcommentary_chapter3" ) );

	m_CommonImages.AddToTail( GetImageId( "vgui/no_save_game" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/new_save_game" ) );

	m_CommonImages.AddToTail( GetImageId( "vgui/hud/spinner_saving" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/hud/icon_info" ) );

	m_CommonImages.AddToTail( GetImageId( IsPS3() ? "vgui/controller_stick_callouts_ps3" : "vgui/controller_stick_callouts_360" ) );
	m_CommonImages.AddToTail( GetImageId( IsPS3() ? "vgui/controller_layout_sticks_ps3" : "vgui/controller_layout_sticks" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/controller_layout_sticks_look" ) );
	m_CommonImages.AddToTail( GetImageId( "vgui/controller_layout_sticks_move" ) );

	m_CommonImages.AddToTail( GetImageId( "vgui/loadbar_dots" ) );

#if defined( _PS3 )
	m_CommonImages.AddToTail( GetImageId( "vgui/steam_avatar_border_ingame" ) );
#endif
}

void CBaseModPanel::DrawColoredText( int x, int y, Color color, const char *pAnsiText, vgui::HFont hFont )
{
	wchar_t szconverted[256];
	g_pVGuiLocalize->ConvertANSIToUnicode( pAnsiText, szconverted, sizeof( szconverted ) );
	
	int len = wcslen( szconverted );
	if ( len == 0 )
		return;

	if ( hFont == vgui::INVALID_FONT )
	{
		hFont = m_hDefaultFont;
	}

	vgui::surface()->DrawSetTextFont( hFont );
	vgui::surface()->DrawSetTextPos( x, y );
	vgui::surface()->DrawSetTextColor( color );
	vgui::surface()->DrawPrintText( szconverted, len );
}

void CBaseModPanel::DrawCopyStats()
{
#if defined( _X360 )
	int wide, tall;
	GetSize( wide, tall );

	int xPos = 0.1f * wide;
	int yPos = 0.1f * tall;

	// draw copy status
	char textBuffer[256];
	const CopyStats_t *pCopyStats = g_pXboxInstaller->GetCopyStats();	

	V_snprintf( textBuffer, sizeof( textBuffer ), "Version: %d (%s)", g_pXboxInstaller->GetVersion(), XBX_GetLanguageString() );
	DrawColoredText( xPos, yPos, Color( 255, 255, 0, 255 ), textBuffer );
	yPos += 20;

	V_snprintf( textBuffer, sizeof( textBuffer ), "DVD Hosted: %s", g_pFullFileSystem->IsDVDHosted() ? "Enabled" : "Disabled" );
	DrawColoredText( xPos, yPos, Color( 255, 255, 0, 255 ), textBuffer );
	yPos += 20;

	bool bDrawProgress = true;
	if ( g_pFullFileSystem->IsInstalledToXboxHDDCache() )
	{
		DrawColoredText( xPos, yPos, Color( 0, 255, 0, 255 ), "Existing Image Found." );
		yPos += 20;
		bDrawProgress = false;
	}
	if ( !g_pXboxInstaller->IsInstallEnabled() )
	{
		DrawColoredText( xPos, yPos, Color( 255, 0, 0, 255 ), "Install Disabled." );
		yPos += 20;
		bDrawProgress = false;
	}
	if ( g_pXboxInstaller->IsFullyInstalled() )
	{
		DrawColoredText( xPos, yPos, Color( 0, 255, 0, 255 ), "Install Completed." );
		yPos += 20;
	}

	if ( bDrawProgress )
	{
		yPos += 20;
		V_snprintf( textBuffer, sizeof( textBuffer ), "From: %s (%.2f MB)", pCopyStats->m_srcFilename, (float)pCopyStats->m_ReadSize/(1024.0f*1024.0f) );
		DrawColoredText( xPos, yPos, Color( 255, 255, 0, 255 ), textBuffer );
		V_snprintf( textBuffer, sizeof( textBuffer ), "To: %s (%.2f MB)", pCopyStats->m_dstFilename, (float)pCopyStats->m_WriteSize/(1024.0f*1024.0f)  );
		DrawColoredText( xPos, yPos + 20, Color( 255, 255, 0, 255 ), textBuffer );

		float elapsed = 0;
		float rate = 0;
		if ( pCopyStats->m_InstallStartTime )
		{
			elapsed = (float)(GetTickCount() - pCopyStats->m_InstallStartTime) * 0.001f;
		}
		if ( pCopyStats->m_InstallStopTime )
		{
			elapsed = (float)(pCopyStats->m_InstallStopTime - pCopyStats->m_InstallStartTime) * 0.001f;
		}
		if ( elapsed )
		{
			rate = pCopyStats->m_TotalWriteSize/elapsed;
		}
		V_snprintf( textBuffer, sizeof( textBuffer ), "Progress: %d/%d MB Elapsed: %d secs (%.2f MB/s)", pCopyStats->m_BytesCopied/(1024*1024), g_pXboxInstaller->GetTotalSize()/(1024*1024), (int)elapsed, rate/(1024.0f*1024.0f) );
		DrawColoredText( xPos, yPos + 40, Color( 255, 255, 0, 255 ), textBuffer );
	}
#endif
}

//-----------------------------------------------------------------------------
// Returns true if menu background movie is valid
//-----------------------------------------------------------------------------
bool CBaseModPanel::IsMenuBackgroundMovieValid( void )
{
	if ( !m_bMovieFailed && m_BIKHandle != BIKHANDLE_INVALID )
	{
		return true;
	}

	return false;
}

//=============================================================================
void CBaseModPanel::CalculateMovieParameters( BIKMaterial_t hBIKMaterial, bool bLetterbox )
{
	if ( hBIKMaterial == BIKHANDLE_INVALID )
		return;
	
	// avoid divide by zero
	if ( GetTall() == 0 )
	{
		return;
	}

	m_flU0 = m_flV0 = 0.0f;
	g_pBIK->GetTexCoordRange( hBIKMaterial, &m_flU1, &m_flV1 );

	int nWidth, nHeight;
	g_pBIK->GetFrameSize( hBIKMaterial, &nWidth, &nHeight );
	
	// avoid divide by zero
	if ( nHeight == 0 )
	{
		return;
	}
	// TODO:
#if 0
	const AspectRatioInfo_t &aspectRatioInfo = materials->GetAspectRatioInfo();
	float flPhysicalFrameRatio = aspectRatioInfo.m_flFrameBuffertoPhysicalScalar * ( ( float ) GetWide() / ( float ) GetTall() );

	float  flFrameBufferRatio = aspectRatioInfo.m_flPhysicalToFrameBufferScalar;

#else
	// The constructor for AspectRatioInfo_t sets m_flFrameBuffertoPhysicalScalar to 1.0f, let's just use that for now.
	float flPhysicalFrameRatio = 1.0 * ( ( float ) GetWide() / ( float ) GetTall() );

	float flFrameBufferRatio = 1.0f;

#endif
	// Assume that the video is authored for square pixels.
	float flVideoRatio = ( ( float ) nWidth / ( float ) nHeight );
	
	m_bMovieLetterbox = bLetterbox;
	if ( bLetterbox )
	{
		// m_nPlaybackWidth and m_nPlaybackHeight are the dimensions of a panel that contains the entire bink movie in device pixels.
		// This code-path assumes that we want to letterbox.
		if ( flVideoRatio > flPhysicalFrameRatio )
		{
			m_nMoviePlaybackWidth = GetWide();
			// Have to account for the difference between physical and pixel aspect ratios.
			m_nMoviePlaybackHeight = ( ( float )GetWide() / flFrameBufferRatio ) / flVideoRatio;
		}
		else if ( flVideoRatio < flPhysicalFrameRatio )
		{
			// Have to account for the difference between physical and pixel aspect ratios.
			m_nMoviePlaybackWidth = ( float )GetTall() * flVideoRatio * flFrameBufferRatio;
			m_nMoviePlaybackHeight = GetTall();
		}
		else
		{
			m_nMoviePlaybackWidth = GetWide();
			m_nMoviePlaybackHeight = GetTall();
		}

		return;
	}

	if ( flVideoRatio > flPhysicalFrameRatio )
	{
		// Width must be adjusted.  Lop of the left and right.
		float flImageWidth = ( float )GetTall() * flVideoRatio;
		// convert from physical to pixels
		flImageWidth *= flFrameBufferRatio;
		const float flSpanScaled = ( m_flU1 - m_flU0 ) * GetWide() / flImageWidth;
		m_flU0 = ( m_flU1 - flSpanScaled ) / 2.0f;
		m_flU1 = m_flU0 + flSpanScaled;
	}
	else if ( flVideoRatio < flPhysicalFrameRatio )
	{
		// Height must be adjusted.  Lop of the top and bottom.
		float flImageHeight = ( float )GetWide() * ( ( float )nHeight / ( float )nWidth );
		// convert from physical to pixels
		// ( would divide by m_flPhysicalToFrameBufferScalar, but m_flFrameBuffertoPhysicalScalar = 1.0f / m_flPhysicalToFrameBufferScalar

		//*= 1.0f is literally useless
#if 0
		flImageHeight *= aspectRatioInfo.m_flFrameBuffertoPhysicalScalar;
#endif

		const float flSpanScaled = ( m_flV1 - m_flV0 ) * GetTall() / flImageHeight;
		m_flV0 = ( m_flV1 - flSpanScaled ) / 2.0f;
		m_flV1 = m_flV0 + flSpanScaled;
	}

	m_nMoviePlaybackWidth = GetWide();
	m_nMoviePlaybackHeight = GetTall();
}

//=============================================================================
void CBaseModPanel::TransitionToNewBackgroundMovie( void )
{
	CBaseModPanel::GetSingleton().GetTransitionEffectPanel()->MarkTilesInRect( 0, 0, -1, -1, WT_NONE );

	// This needs to 
	InitBackgroundMovie();	
}


//=============================================================================
bool CBaseModPanel::InitBackgroundMovie( void )
{
	if ( m_bMovieFailed || m_ExitingFrameCount )
	{
		// prevent constant i/o testing after failure condition
		// do not restart the movie (after its been stopped), we are trying to stabilize the app for exit
		return false;
	}

	if ( CommandLine()->FindParm( "-nomenuvid" ) )
	{
		// mimic movie i/o failure, render will fallback to use alternate static image
		m_bMovieFailed = true;
		return false;
	}

	// Destroy any previously allocated video
	if ( m_BIKHandle != BIKHANDLE_INVALID )
	{
		g_pBIK->DestroyMaterial( m_BIKHandle );
		m_BIKHandle = BIKHANDLE_INVALID;
	}

	const char *pFilename;
	char movieFilename[MAX_PATH] = {0};
	pFilename = movieFilename;
	GetBackgroundMovieName( movieFilename, sizeof( movieFilename ) );

	COM_TimestampedLog( "Load Background Movie - %s", pFilename );

	if ( g_pBIK )
	{
		// Menu background movie needs to loop and 
		// !!reside!! in memory (CRITICAL: Xbox is installing to HDD, w/o this it will frag the drive).
		int nBikFlags = BIK_LOOP | BIK_PRELOAD;
		if ( V_stristr( pFilename, "menu_act05" ) )
		{
			// For safety, except for menu_act05 which is the final movie at 22 MB, we will prevent the install if this movie is playing
			// as this movie can only occur and the end playing the game
			nBikFlags &= ~BIK_PRELOAD;
		}

		// Load and create our BINK video
		m_BIKHandle = g_pBIK->CreateMaterial( "VideoBIKMaterial_Background", pFilename, "GAME", nBikFlags );
	}
	if ( m_BIKHandle == BIKHANDLE_INVALID )
	{
		m_bMovieFailed = true;
		return false;
	}
	
	COM_TimestampedLog( "Load Background Movie - End" );

	// Find frame size and letterboxing information
	CalculateMovieParameters( m_BIKHandle );

	return true;
}

//=============================================================================
void CBaseModPanel::ShutdownBackgroundMovie( void )
{
	if ( m_BIKHandle != BIKHANDLE_INVALID )
	{
		// FIXME: Make sure the m_pMaterial is actually destroyed at this point!
		g_pBIK->DestroyMaterial( m_BIKHandle );
		m_BIKHandle = BIKHANDLE_INVALID;
	}

	// allow a retry
	m_bMovieFailed = false;

	ReleaseBackgroundMusic();
}

//=============================================================================
bool CBaseModPanel::RenderBackgroundMovie()
{
	// Bring up the video if we haven't before or Alt+Tab has made it invalid
	if ( !ActivateBackgroundEffects() )
	{
		return false;
	}

	bool bRendered = RenderMovie( m_BIKHandle );
	if ( !bRendered )
	{
		// Issue a close command
		ShutdownBackgroundMovie();
	}

	return bRendered;
}

bool CBaseModPanel::RenderMovie( BIKMaterial_t hBIKMaterial )
{
	if ( hBIKMaterial == BIKHANDLE_INVALID )
		return false;

	// There are cases where our texture may never have been rendered (immediately alt+tabbing away on startup).  This check allows us to 
	// recalculate the correct UVs in that case.
	if ( m_flU1 == 0.0f || m_flV1 == 0.0f )
	{
		CalculateMovieParameters( hBIKMaterial );
	}

	// Update our frame, but only if Bink is ready for us to process another frame.
	// We aren't really swapping here, but ReadyForSwap is a good way to throttle.
	// We'd rather throttle this way so that we don't limit the overall frame rate of the system.
	if ( g_pBIK && g_pBIK->ReadyForSwap( hBIKMaterial ) )
	{
		if ( g_pBIK->Update( hBIKMaterial ) == false )
		{
			return false;
		}
	}

	if ( !g_pBIK )
		return false;
	
	int xpos = 0;
	int ypos = 0;

	int nScreenWide = GetWide();
	int nScreenTall = GetTall();

	if ( m_bMovieLetterbox && ( m_nMoviePlaybackWidth != nScreenWide || m_nMoviePlaybackHeight != nScreenTall ) )
	{
		ypos = ( nScreenTall - m_nMoviePlaybackHeight )/2;

		vgui::surface()->DrawSetColor(  0, 0, 0, 255 );
		vgui::surface()->DrawFilledRect( 0, 0, nScreenWide, nScreenTall );
	}

	// Draw the polys to draw the movie out
	CMatRenderContextPtr pRenderContext( materials );

	pRenderContext->MatrixMode( MATERIAL_VIEW );
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	pRenderContext->MatrixMode( MATERIAL_PROJECTION );
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	IMaterial *pMaterial = g_pBIK->GetMaterial( hBIKMaterial );

	pRenderContext->Bind( pMaterial, NULL );

	CMeshBuilder meshBuilder;
	IMesh* pMesh = pRenderContext->GetDynamicMesh( true );
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	float flLeftX = xpos;
	float flRightX = xpos + ( m_nMoviePlaybackWidth - 1 );

	float flTopY = ypos;
	float flBottomY = ypos + ( m_nMoviePlaybackHeight - 1 );

	// Map our UVs to cut out just the portion of the video we're interested in
	float flLeftU = m_flU0;
	float flTopV = m_flV0;

	// We need to subtract off a pixel to make sure we don't bleed
	float flRightU = m_flU1 - ( 1.0f / (float)nScreenWide );
	float flBottomV = m_flV1 - ( 1.0f / (float)nScreenTall );

	// Get the current viewport size
	int vx, vy, vw, vh;
	pRenderContext->GetViewport( vx, vy, vw, vh );

	// map from screen pixel coords to -1..1
	flRightX = FLerp( -1, 1, 0, vw, flRightX );
	flLeftX = FLerp( -1, 1, 0, vw, flLeftX );
	flTopY = FLerp( 1, -1, 0, vh ,flTopY );
	flBottomY = FLerp( 1, -1, 0, vh, flBottomY );

	for ( int corner=0; corner<4; corner++ )
	{
		bool bLeft = (corner==0) || (corner==3);
		meshBuilder.Position3f( (bLeft) ? flLeftX : flRightX, (corner & 2) ? flBottomY : flTopY, 0.0f );
		meshBuilder.Normal3f( 0.0f, 0.0f, 1.0f );
		meshBuilder.TexCoord2f( 0, (bLeft) ? flLeftU : flRightU, (corner & 2) ? flBottomV : flTopV );
		meshBuilder.TangentS3f( 0.0f, 1.0f, 0.0f );
		meshBuilder.TangentT3f( 1.0f, 0.0f, 0.0f );
		meshBuilder.Color4f( 1.0f, 1.0f, 1.0f, 1.0f );
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();
	pMesh->Draw();

	pRenderContext->MatrixMode( MATERIAL_VIEW );
	pRenderContext->PopMatrix();

	pRenderContext->MatrixMode( MATERIAL_PROJECTION );
	pRenderContext->PopMatrix();

#if defined( ENABLE_BIK_PERF_SPEW ) && ENABLE_BIK_PERF_SPEW
	{
		// timing debug code for bink playback
		static double flPreviousTime = -1.0;
		double flTime = Plat_FloatTime();
		double flDeltaTime = flTime - flPreviousTime;
		if ( flDeltaTime > 0.0 )
		{
			Warning( "%0.2lf sec*60 %0.2lf fps\n", flDeltaTime * 60.0, 1.0 / flDeltaTime );
		}
		flPreviousTime = flTime;
	}
#endif

	return true;
}

//=============================================================================
void CBaseModPanel::PaintBackground()
{
	if ( !m_LevelLoading &&
		!GameUI().IsInLevel() &&
		!GameUI().IsInBackgroundLevel() )
	{
		int wide, tall;
		GetSize( wide, tall );

		if ( engine->IsTransitioningToLoad() )
		{
			// ensure the background is clear
			// the loading progress is about to take over in a few frames
			surface()->DrawSetColor( Color( 0, 0, 0, 255 ) );
			surface()->DrawFilledRect( 0, 0, wide, tall );
		}
		else
		{
			bool bDoBackgroundEffects = true;
			CMoviePlayer *pMoviePlayer = (CMoviePlayer *)GetWindow( WT_MOVIEPLAYER );
			if ( pMoviePlayer && pMoviePlayer->IsMoviePlayerOpaque() )
			{
				bDoBackgroundEffects = false;
				ShutdownBackgroundMovie();
			}

			if ( bDoBackgroundEffects )
			{
				// Render the background movie
				if ( !RenderBackgroundMovie() )
				{
					// movie not yet started or failed, use the static substitute
					surface()->DrawSetColor( 255, 255, 255, 255 );
					surface()->DrawSetTexture( m_iBackgroundImageID );
					surface()->DrawTexturedRect( 0, 0, wide, tall );
				}
			
				if ( m_flMusicFadeInTime )
				{
					float flFadeDelta = RemapValClamped( Plat_FloatTime(), m_flMusicFadeInTime, m_flMusicFadeInTime + TRANSITION_OVERLAY_FADE_TIME, 0.0f, 1.0f );
					CBaseModPanel::GetSingleton().UpdateBackgroundMusicVolume( flFadeDelta );
					if ( flFadeDelta >= 1.0f )
					{
						// stop updating
						m_flMusicFadeInTime = 0;
					}
				}

				int logoAlpha = 128;
				if ( CBaseModFrame *pAttractScreen = GetWindow( WT_ATTRACTSCREEN ) )
				{
					// logo is brighter only at the attract screen as a more dominant element
					logoAlpha = 255;
				}

				// place the product logo
				CBaseModFrame *pActiveWindow = GetWindow( GetActiveWindowType() );
				int iLogoTexture = ( pActiveWindow && pActiveWindow->UsesAlternateTiles() ) ? m_iAltProductImageID : m_iProductImageID;
				surface()->DrawSetColor( 255, 255, 255, logoAlpha );
				surface()->DrawSetTexture( iLogoTexture );
				surface()->DrawTexturedRect( m_nProductImageX, m_nProductImageY, m_nProductImageX + m_nProductImageWide, m_nProductImageY + m_nProductImageTall );	
			}
		}
	}
	else if ( !m_LevelLoading && GameUI().IsInLevel() )
	{
		// place the product logo on all the ingame menus
		CBaseModFrame *pActiveWindow = GetWindow( GetActiveWindowType() );
		int iLogoTexture = ( pActiveWindow && pActiveWindow->UsesAlternateTiles() ) ? m_iAltProductImageID : m_iProductImageID;
		surface()->DrawSetColor( 255, 255, 255, 128 );
		surface()->DrawSetTexture( iLogoTexture );
		surface()->DrawTexturedRect( m_nProductImageX, m_nProductImageY, m_nProductImageX + m_nProductImageWide, m_nProductImageY + m_nProductImageTall );
	}

	// Draw cloud syncing progress
	if ( g_pGameSteamCloudSync )
	{
		GameSteamCloudSyncInfo_t gscsi;
		static GameSteamCloudSyncInfo_t s_LastTimeInfo;
		static float s_flLastTime = 0.0f;
		bool bRenderThisFrame = g_pGameSteamCloudSync->IsSyncInProgress( &gscsi );
		if ( bRenderThisFrame )
		{
			s_LastTimeInfo = gscsi;
			s_flLastTime = Plat_FloatTime();
		}
		if ( !bRenderThisFrame && ( s_flLastTime > 0 ) )
		{
			if ( Plat_FloatTime() - s_flLastTime < ui_fadecloud_time.GetFloat() )
			{
				gscsi = s_LastTimeInfo;
				gscsi.m_flProgress = RemapValClamped( Plat_FloatTime() - s_flLastTime, 0.0f, ui_fadecloud_time.GetFloat(), s_LastTimeInfo.m_flProgress, 1.0f );
				bRenderThisFrame = true;
			}
			else
			{
				s_flLastTime = 0;
			}
		}
		if ( bRenderThisFrame && !engine->IsDrawingLoadingImage() && !engine->IsInGame() )
		{
			surface()->DrawSetColor( 255, 255, 255, 255 );
			surface()->DrawSetTexture( m_iCloudImageID[!!gscsi.m_bUploadingToCloud] );
			surface()->DrawTexturedRect( m_iCloudPosition[0] - m_iCloudPosition[2], m_iCloudPosition[1], m_iCloudPosition[0], m_iCloudPosition[1] + m_iCloudPosition[3] );

			int xSplit = m_iCloudProgressPosition[0] - m_iCloudProgressPosition[2] + gscsi.m_flProgress*m_iCloudProgressPosition[2];
			surface()->DrawSetColor( m_clrCloudRemaining );
			surface()->DrawFilledRect( xSplit, m_iCloudProgressPosition[1], m_iCloudProgressPosition[0], m_iCloudProgressPosition[1] + m_iCloudProgressPosition[3] );
			float flFader = ( 1 + sin( 4*Plat_FloatTime() ) )/2;
			surface()->DrawSetColor(
				m_clrCloudDoneFade[0] + ( m_clrCloudDone[0] - m_clrCloudDoneFade[0] )*flFader,
				m_clrCloudDoneFade[1] + ( m_clrCloudDone[1] - m_clrCloudDoneFade[1] )*flFader,
				m_clrCloudDoneFade[2] + ( m_clrCloudDone[2] - m_clrCloudDoneFade[2] )*flFader,
				m_clrCloudDoneFade[3] + ( m_clrCloudDone[3] - m_clrCloudDoneFade[3] )*flFader
				);
			surface()->DrawFilledRect( m_iCloudProgressPosition[0] - m_iCloudProgressPosition[2], m_iCloudProgressPosition[1], xSplit, m_iCloudProgressPosition[1] + m_iCloudProgressPosition[3] );
			// surface()->DrawFilledRect( m_iCloudProgressPosition[0] - 1, m_iCloudProgressPosition[1], m_iCloudProgressPosition[0], m_iCloudProgressPosition[1] + m_iCloudProgressPosition[3] );

			const wchar_t *pString = g_pVGuiLocalize->Find( "#L4D360UI_Cloud_Syncing" );
			if ( pString && pString[0] )
			{
				surface()->DrawSetTextFont( m_hSteamCloudFont );
				surface()->DrawSetTextPos( m_iCloudTextPosition[0], m_iCloudTextPosition[1] );
				surface()->DrawSetTextColor( m_clrSteamCloudText );

				int nLen = V_wcslen( pString );
				surface()->DrawPrintText( pString, nLen );
			}
		}
	}

#if defined( _X360 )
	if ( !m_LevelLoading && !GameUI().IsInLevel() && xbox_install_status.GetBool() )
	{
		DrawCopyStats();
	}
#endif
}

bool CBaseModPanel::IsOpaqueOverlayActive()
{
	if ( m_flOverlayFadeOutTime < 0 || m_iFadeOutOverlayImageID == -1 )
	{
		// not active
		return false;
	}

	return true;
}

bool CBaseModPanel::LoadingProgressWantsIsolatedRender( bool bContextValid )
{
	if ( !m_LevelLoading )
	{
		return false;
	}

	LoadingProgress *pLoadingProgress = static_cast< LoadingProgress* >( GetWindow( WT_LOADINGPROGRESS ) );
	if ( !pLoadingProgress )
	{
		return false;
	}

	return pLoadingProgress->LoadingProgressWantsIsolatedRender( bContextValid );
}

bool CBaseModPanel::IsTransitionEffectEnabled()
{
	if ( !m_pTransitionPanel )
		return false;

	return m_pTransitionPanel->IsEffectEnabled();
}

BaseModUI::CBaseModTransitionPanel *CBaseModPanel::GetTransitionEffectPanel()
{
	return m_pTransitionPanel;
}

void CBaseModPanel::ComputeCroppedTexcoords( float flBackgroundSourceAspectRatio /* ie. 16/9, 16/10 */, float flPhysicalAspectRatio /*GetWidth() / GetHeight() */, float &sMin, float &tMin, float &sMax, float &tMax )
{
	if ( flBackgroundSourceAspectRatio == flPhysicalAspectRatio )
	{
		sMin = 0.0f;
		sMax = 1.0f;
		tMin = 0.0f;
		tMax = 1.0f;
	}
	else if ( flBackgroundSourceAspectRatio > flPhysicalAspectRatio )
	{
		// Width must be adjusted.  Lop of the left and right of the image.
		float flAdjust = 0.5f * ( 1.0f - ( flPhysicalAspectRatio / flBackgroundSourceAspectRatio ) );
		sMin = flAdjust;
		sMax = 1.0f - flAdjust;
		tMin = 0.0f;
		tMax = 1.0f;
	}
	else
	{
		// Height must be adjusted.  Lop of the top and bottom of the image.
		float flAdjust = 0.5f * ( 1.0f - ( flBackgroundSourceAspectRatio / flPhysicalAspectRatio ) );
		sMin = 0.0f;
		sMax = 1.0f;
		tMin = flAdjust;
		tMax = 1.0f - flAdjust;
	}
}

void CBaseModPanel::PostChildPaint()
{
	if ( !m_LevelLoading && !GameUI().IsInLevel() && engine->IsTransitioningToLoad() )
	{
		// prevent a one frame glitch where there is no background, but the menus are still in the foreground
		// this is due to the ui shutdown does not occurr on the same frame when the engine has started the loading process
		// cover the foreground menus
		// the loading progress is about to take over in a few frames
		surface()->DrawSetColor( Color( 0, 0, 0, 255 ) );
		surface()->DrawFilledRect( 0, 0, GetWide(), GetTall() );
	}

	if ( !IsOpaqueOverlayActive() )
	{
		return;
	}

	if ( m_iFadeOutOverlayImageID == m_iStartupImageID )
	{
		if ( m_nStartupFrames < 10 )
		{
			// need some non trivial number of frames to let the movie settle
			m_nStartupFrames++;
			if ( m_nStartupFrames < 9 )
			{
				// hold the overlay opaque for one full frame to let the transition effect capture the complete frame
				m_flOverlayFadeOutTime = 0;
			}
			else
			{
				// stop the overlay, the transition will run this frame
				m_flOverlayFadeOutTime = -1;
				GetTransitionEffectPanel()->MarkTilesInRect( 0, 0, -1, -1, WT_NONE );
			}
		}
	}

	// by default hold the overlay fully opaque until the timing is stable enough that its gets set
	// then the fade out runs
	float flFadeDelta = 1.0f;
	if ( m_flOverlayFadeOutTime > 0 )
	{
		flFadeDelta = RemapValClamped( Plat_FloatTime(), m_flOverlayFadeOutTime, m_flOverlayFadeOutTime + TRANSITION_OVERLAY_FADE_TIME, 1.0f, 0.0f );
	}
	if ( flFadeDelta > 0.0f )
	{
		surface()->DrawSetColor( 255, 255, 255, flFadeDelta * 255.0f );
		surface()->DrawSetTexture( m_iFadeOutOverlayImageID );
		
		// only the background images (first frame movie snap) that overlay the movies need to adjust their texcoords to match the movie
		if ( m_iBackgroundImageID != -1 && m_iFadeOutOverlayImageID == m_iBackgroundImageID )
		{
			float sMin, tMin, sMax, tMax;

			// needs to match image aspect ratio (known to be either 16:9 or 4:3), resolved in SelectBackgroundPresentation()
			ComputeCroppedTexcoords( IsWidescreen() ? 16.0f/9.0f : 4.0f/3.0f, ( float )GetWide() / ( float )GetTall(), sMin, tMin, sMax, tMax );
			surface()->DrawTexturedSubRect( 0, 0, GetWide(), GetTall(), sMin, tMin, sMax, tMax );
		}
		else
		{
			surface()->DrawTexturedRect( 0, 0, GetWide(), GetTall() );
		}
	}
	else
	{
		// finished
		m_flOverlayFadeOutTime = -1;
	}
}

void CBaseModPanel::OnCommand(const char *command)
{
	if ( !Q_stricmp( command, "QuitRestartNoConfirm" ) )
	{
	}
	else if ( !Q_stricmp( command, "RestartWithNewLanguage" ) )
	{
		if ( !IsGameConsole() )
		{
			const char *pUpdatedAudioLanguage = Audio::GetUpdatedAudioLanguage();

			if ( pUpdatedAudioLanguage[ 0 ] != '\0' )
			{
				char szSteamURL[50];

				// hide everything while we quit
				SetVisible( false );
				vgui::surface()->RestrictPaintToSinglePanel( GetVPanel() );
				engine->ClientCmd_Unrestricted( "quit\n" );

				// Construct Steam URL. Pattern is steam://run/<appid>/<language>. (e.g. Ep1 In French ==> steam://run/380/french)
				Q_snprintf( szSteamURL, sizeof( szSteamURL ), "steam://run/%d/%s",
					engine->GetAppID(), pUpdatedAudioLanguage );

				// Set Steam URL for re-launch in registry. Launcher will check this registry key and exec it in order to re-load the game in the proper language
				vgui::system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Source\\Relaunch URL", szSteamURL );
			}
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

const char *CBaseModPanel::GetUISoundName(  UISound_t UISound )
{
	switch ( UISound )
	{
	case UISOUND_BACK:
		return "UI/menu_back.wav";
	case UISOUND_ACCEPT:
		return "UI/menu_accept.wav";
	case UISOUND_INVALID:
		return "UI/menu_invalid.wav";
	case UISOUND_COUNTDOWN:
		return "UI/menu_countdown.wav";
	case UISOUND_FOCUS:
		return "UI/menu_focus.wav";
	case UISOUND_CLICK:
		return "UI/buttonclick.wav";
	case UISOUND_DENY:
		return "UI/menu_invalid.wav";
	case UISOUND_TILE_CLICK1:
		return "UI/ui_menu_flip_single_01.wav";
	case UISOUND_TILE_CLICK2:
		return "UI/ui_menu_flip_single_02.wav";
	}
	return NULL;
}

void CBaseModPanel::PlayUISound( UISound_t UISound )
{
	const char *pSound = GetUISoundName( UISound );
	if ( pSound )
	{
		vgui::surface()->PlaySound( pSound );
	}
}

//=============================================================================
// Start system shutdown. Cannot be stopped.
// A Restart is cold restart, plays the intro movie again.
//=============================================================================
void CBaseModPanel::StartExitingProcess( bool bWarmRestart )
{
	if ( !IsGameConsole() )
	{
		// xbox only
		Assert( 0 );
		return;
	}

	if ( m_ExitingFrameCount )
	{
		// already fired
		return;
	}

#if defined( _X360 )
	// signal the installer to stop
	g_pXboxInstaller->Stop();
#endif

	// cold restart or warm
	m_bWarmRestartMode = bWarmRestart;

	// the exiting screen will transition to obscure all the game and UI
	OpenWindow( WT_TRANSITIONSCREEN, 0, false );

	// must let a non trivial number of screen swaps occur to stabilize image
	// ui runs in a constrained state, while shutdown is occurring
	m_ExitingFrameCount = 15;

	// exiting cannot be stopped
	// do not allow any input to occur
	g_pInputSystem->DetachFromWindow();

	// start shutting down systems
	engine->StartXboxExitingProcess();
}

void CBaseModPanel::OnSetFocus()
{
	BaseClass::OnSetFocus();
	if ( IsPC() )
	{
		GameConsole().Hide();
	}
}

void CBaseModPanel::OnMovedPopupToFront()
{
	if ( IsPC() )
	{
		GameConsole().Hide();
	}
}

bool CBaseModPanel::IsBackgroundMusicPlaying()
{
	int currentAct = 1;
	switch (portal2_current_act.GetInt())
	{
	default:
	case 1:
		currentAct = 1; break;
	case 2:
		currentAct = 2; break;
	case 3:
		currentAct = 3; break;
	case 4:
		currentAct = 4; break;
	case 5:
		currentAct = 5; break;
	}

	if (currentAct != playingMusicFromAct)
	{
		ReleaseBackgroundMusic();
		return false;
	}

	if ( m_BackgroundMusicString.IsEmpty() )
		return false;

	if ( m_nBackgroundMusicGUID == 0 )
		return false;
	
	return enginesound->IsSoundStillPlaying( m_nBackgroundMusicGUID );
}

// per Morasky
#define BACKGROUND_MUSIC_DUCK	0.35f

bool CBaseModPanel::StartBackgroundMusic( float fVol )
{
	if ( IsBackgroundMusicPlaying() )
		return true;
	
	if ( m_BackgroundMusicString.IsEmpty() )
		return false;

	// trying to exit, cannot start it
	if ( m_ExitingFrameCount )
		return false;
	
	switch (portal2_current_act.GetInt())
	{
	default:
	case 1:
		playingMusicFromAct = 1;
		enginesound->EmitAmbientSound("music/mainmenu/portal2_background01.wav", 1.0f * fVol, 100); break;
	case 2:
		playingMusicFromAct = 2;
		enginesound->EmitAmbientSound("music/mainmenu/portal2_background02.wav", 1.0f * fVol, 100); break;
	case 3:
		playingMusicFromAct = 3;
		enginesound->EmitAmbientSound("music/mainmenu/portal2_background03.wav", 1.0f * fVol, 100); break;
	case 4:
		playingMusicFromAct = 4;
		enginesound->EmitAmbientSound("music/mainmenu/portal2_background04.wav", 1.0f * fVol, 100); break;
	case 5:
		playingMusicFromAct = 5;
		enginesound->EmitAmbientSound("music/mainmenu/portal2_background05.wav", 1.0f * fVol, 100); break;
	}
	m_nBackgroundMusicGUID = enginesound->GetGuidForLastSoundEmitted();
		
	return ( m_nBackgroundMusicGUID != 0 );
}

void CBaseModPanel::UpdateBackgroundMusicVolume( float fVol )
{
	if ( !IsBackgroundMusicPlaying() )
		return;

	// mixes too loud against soft ui sounds
	enginesound->SetVolumeByGuid( m_nBackgroundMusicGUID, BACKGROUND_MUSIC_DUCK * fVol );
}

void CBaseModPanel::ReleaseBackgroundMusic()
{
	if ( m_BackgroundMusicString.IsEmpty() )
		return;

	if ( m_nBackgroundMusicGUID == 0 )
		return;

	// need to stop the sound now, do not queue the stop
	// we must release the 2-5 MB held by this resource
	enginesound->StopSoundByGuid( m_nBackgroundMusicGUID );
#if defined( _GAMECONSOLE )
	enginesound->UnloadSound( m_BackgroundMusicString );
#endif

	m_nBackgroundMusicGUID = 0;
}

void CBaseModPanel::SafeNavigateTo( Panel *pExpectedFrom, Panel *pDesiredTo, bool bAllowStealFocus )
{
	Panel *pOriginalFocus = ipanel()->GetPanel( GetCurrentKeyFocus(), GetModuleName() );
	bool bSomeoneElseHasFocus = pOriginalFocus && (pOriginalFocus != pExpectedFrom);
	bool bActuallyChangingFocus = (pExpectedFrom != pDesiredTo);
	bool bNeedToReturnKeyFocus = !bAllowStealFocus && bSomeoneElseHasFocus && bActuallyChangingFocus;

	pDesiredTo->NavigateTo();

	if ( bNeedToReturnKeyFocus )
	{
		pDesiredTo->NavigateFrom();
		pOriginalFocus->NavigateTo();
	}
}

int CBaseModPanel::MapNameToChapter( const char *pMapName, bool bSinglePlayer /*= true*/ )
{
	char baseMapName[MAX_PATH];
	V_FileBase( pMapName, baseMapName, sizeof( baseMapName ) );

	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;

	for ( int i = 0; ; i++ )
	{
		if ( !pChapterNames[i].m_pMapName )
			break;

		if ( !V_stricmp( pChapterNames[i].m_pMapName, baseMapName ) )
		{
			if ( bSinglePlayer )
				return pChapterNames[i].m_nChapterNum;
			else
				return GetCoopTrackFromChapter( pChapterNames[i].m_nChapterNum );
		}
	}

	// unknown
	return 0;
}

int CBaseModPanel::MapNameToSubChapter( const char *pMapName )
{
	char baseMapName[MAX_PATH];
	V_FileBase( pMapName, baseMapName, sizeof( baseMapName ) );

	for ( int i = 0; ; i++ )
	{
		if ( !g_ChapterContextNames[i].m_pMapName )
			break;

		if ( !V_stricmp( g_ChapterContextNames[i].m_pMapName, baseMapName ) )
		{
			return g_ChapterContextNames[i].m_nSubChapterNum;
		}
	}

	// unknown
	return 0;
}

const char *CBaseModPanel::ActToMapName( int nAct )
{
	// find first matching act
	for ( int i = 0; ; i++ )
	{
		if ( !g_ChapterContextNames[i].m_pMapName )
			break;

		const char *pMapName = g_ChapterContextNames[i].m_pMapName;
		const char *pActPrefix = V_stristr( pMapName, "sp_a" );
		if ( pActPrefix )
		{
			if ( atoi( pActPrefix + 4 ) == nAct ) 
			{
				return g_ChapterContextNames[i].m_pMapName;
			}
		}
	}

	// unknown
	return "";
}

const char *CBaseModPanel::ChapterToMapName( int nChapter )
{
	for ( int i = 0; ; i++ )
	{
		if ( !g_ChapterContextNames[i].m_pMapName )
			break;

		if ( g_ChapterContextNames[i].m_nChapterNum == nChapter )
		{
			return g_ChapterContextNames[i].m_pMapName;
		}
	}

	// unknown
	return "";
}

int CBaseModPanel::GetMapNumInChapter( int nChapter, const char *pMapName, bool bSinglePlayer /*= true*/  )
{
	int nNumMaps = GetNumMaps( bSinglePlayer );
	int i = 0;
	int nMapNum = 0;
	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;
	int nRealChapter = bSinglePlayer ? nChapter : GetCoopChapterFromTrack( nChapter );

	while (  i < nNumMaps && pChapterNames[i].m_nChapterNum <= nRealChapter )
	{
		if ( pChapterNames[i].m_nChapterNum == nRealChapter )
		{
			nMapNum++;
			if ( !V_stricmp(pChapterNames[i].m_pMapName, pMapName)  )
			{
				if ( !bSinglePlayer )
					return pChapterNames[i].m_nSubChapterNum;

				break;
			}
		}

		i++;
	}
	
	if ( i >= nNumMaps || pChapterNames[i].m_nChapterNum > nRealChapter )
		return -1;
	else
		return nMapNum;
}

int CBaseModPanel::ChapterToAct( int nChapter )
{
	const char *pMapName = ChapterToMapName( nChapter );

	const char *pPrefix = V_stristr( pMapName, "sp_a" );
	if ( pPrefix )
	{
		int nAct = atoi( pPrefix + 4 );
		if ( nAct )
			return nAct;
	}

	// unknown
	return 0;
}


int CBaseModPanel::GetNumChapters( bool bSinglePlayer /*= true*/ )
{
	static int nNumSPChapters = 0;
	static int nNumMPChapters = 0;

	int &nNumChapters = bSinglePlayer ? nNumSPChapters : nNumMPChapters;

	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;

	if ( !nNumChapters )
	{
		for ( int i = 0; ; i++ )
		{
			if ( !pChapterNames[i].m_pMapName )
				break;

			nNumChapters = MAX( nNumChapters, pChapterNames[i].m_nChapterNum );
		}
	}
	return nNumChapters;
}

int CBaseModPanel::GetNumMaps( bool bSinglePlayer /*= true*/ )
{
	static int nNumSPMaps = 0;
	static int nNumMPMaps = 0;

	int &nNumMaps = bSinglePlayer ? nNumSPMaps : nNumMPMaps;

	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;

	if ( !nNumMaps )
	{
		for ( int i = 0; ; i++ )
		{
			if ( !pChapterNames[i].m_pMapName )
				break;

			nNumMaps++;
		}
	}
	return nNumMaps;
}

int CBaseModPanel::GetNumMapsInChapter( int nChapter, bool bSinglePlayer /*= true*/ )
{
	int nNumMaps = 0;
	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;

	int nRealChapter = bSinglePlayer ? nChapter : GetCoopChapterFromTrack( nChapter );

	for ( int i = 0; ; i++ )
	{
		if ( !pChapterNames[i].m_pMapName )
			break;

		if ( pChapterNames[i].m_nChapterNum == nRealChapter )
			nNumMaps++;
	}
	return nNumMaps;
}

const char * CBaseModPanel::GetMapName( int nChapter, int nMap, bool bSinglePlayer /*= true*/ )
{
	ChapterContext_t *pChapterNames = bSinglePlayer ? g_ChapterContextNames : g_ChapterMPContextNames;

	int nRealChapter = bSinglePlayer ? nChapter : GetCoopChapterFromTrack( nChapter );

	for ( int i = 0; ; i++ )
	{
		if ( !pChapterNames[i].m_pMapName )
			break;

		if ( pChapterNames[i].m_nChapterNum == nRealChapter )
		{
			return pChapterNames[i + nMap - 1].m_pMapName;
		}
	}
	return "";
}

int CBaseModPanel::GetCoopTrackFromChapter( int nChapterNum )
{
	if ( nChapterNum >= 3 && nChapterNum <= 7 )
	{
		return nChapterNum - 2;
	}
	else if ( nChapterNum == 9 )
	{
		return nChapterNum - 3;
	}
	else
	{
		return 0; // ??
	}
}

int CBaseModPanel::GetCoopChapterFromTrack( int nTrackNum )
{
	 if ( nTrackNum <= 5 )
	 {
		 return nTrackNum + 2;
	 }
	 else if ( nTrackNum == 6 )
	 {
		 return 9;
	 }
	 else
	 {
		 return 0; // unknown
	 }
}


int CBaseModPanel::GetChapterProgress()
{
#if !defined( _CERT )
	bool bUnlockChapters = CommandLine()->FindParm( "-dev" ) && CommandLine()->FindParm( "-unlockchapters" );
	if ( bUnlockChapters )
	{
		// all chapters unlocked
		return GetNumChapters();
	}

	if ( CommandLine()->FindParm( "-dev" ) && CommandLine()->FindParm( "-nochapterprogress" ) )
	{
		return 0;
	}
#endif

	// determine progress
	int iUserSlot = GetLastActiveUserId();
	int iController = XBX_GetUserId( iUserSlot );
	IPlayerLocal *pPlayer = g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( iController );
	if ( pPlayer )
	{
		int nNumChapters = GetNumChapters();

		// No awards in our Swarm mod
#if 0
		// Check if player has unlocked "ACH.SHOOT_THE_MOON", then all chapters should be available
		KeyValues *kvAwards = new KeyValues( "read_awards", "ACH.SHOOT_THE_MOON", int(0) );
		KeyValues::AutoDelete autodelete_kvAwards( kvAwards );
		pPlayer->GetAwardsData( kvAwards );
		if ( kvAwards->GetInt( "ACH.SHOOT_THE_MOON" ) )
			return nNumChapters; // player has unlocked all chapters
#endif

		// Read players progress
		TitleData1 const *pTitleData = ( TitleData1 const * )pPlayer->GetPlayerTitleData( 0 );
		if ( pTitleData && nNumChapters )
		{			
			nNumChapters = MIN( (unsigned int)nNumChapters, pTitleData->uiSinglePlayerProgressChapter );
			nNumChapters = MAX( nNumChapters, 0 );
			return nNumChapters;
		}
	}

	// no progress
	return 0;
}

void CBaseModPanel::GetBackgroundMovieName( char *pOutBuffer, int nOutBufferSize )
{
	CBaseModFrame *pActiveWindow = GetWindow( GetActiveWindowType() );
	if ( pActiveWindow && pActiveWindow->UsesAlternateTiles() )
	{
		V_snprintf( pOutBuffer, nOutBufferSize, "media/community_bg1.bik" );
	}
	else
	{
		V_snprintf( pOutBuffer, nOutBufferSize, "media/menu_act%02d.bik", m_nCurrentActPresentation );
	}
}

void CBaseModPanel::GetBackgroundMusicName( char *pOutBuffer, int nOutBufferSize, bool bMono )
{
	if ( IsPC() && bMono )
	{
		// pc did not ship these, new code might not be aware of that
		Assert( 0 );
		bMono = false;
	}

	V_snprintf( pOutBuffer, nOutBufferSize, "*#music/mainmenu/portal2_background%02d%s.wav", m_nCurrentActPresentation, ( bMono ? "_mono" : "" ) );
}

void CBaseModPanel::SelectBackgroundPresentation()
{
	static ConVarRef cl_finale_completed( "cl_finale_completed" );

	// determine player's progess
	// background is determined by last level played
	int nMaxActs = ChapterToAct( GetNumChapters() );

	int nAct;
	if ( !m_LastLoadedLevelName.IsEmpty() )
	{
		nAct = ChapterToAct( MapNameToChapter( m_LastLoadedLevelName.Get() ) );
	}
	else
	{
		// user could have been with archived convars, validate to ensure expected results
		// not allowing the finale background to be a restore candidate
		nAct = ui_lastact_played.GetInt();
		if ( nAct < 0 || nAct >= nMaxActs )
		{
			nAct = 0;
		}
	}

	if ( nAct == nMaxActs-1 && cl_finale_completed.GetBool() )
	{
		// finale was just completed, use the finale background
		nAct = nMaxActs;
		cl_finale_completed.SetValue( false );
	}

#if !defined( _CERT )
	// dev override to view backgrounds
	int nOverride = CommandLine()->ParmValue( "-background", 0 );
	if ( nOverride > 0 )
	{
		nAct = nOverride;
	}
#endif

	nAct = clamp( nAct, 1, nMaxActs );
	m_nCurrentActPresentation = nAct;

	bool bIsWidescreen = IsWidescreen();

	// get the substitute movie image, matched to the movie chosen
	// used to hide long i/o on loading entire menu movie, blends away to reveal movie
	CFmtStr pFadeFilename("");
	if ( m_bMoveToEditorMainMenu )
	{
		pFadeFilename.AppendFormat( "vgui/backgrounds/community_background%s", ( bIsWidescreen ? "_widescreen" : "" ) );
	}
	else
	{
		pFadeFilename.AppendFormat( "vgui/backgrounds/background%02d%s", m_nCurrentActPresentation, ( bIsWidescreen ? "_widescreen" : "" ) );
	}

	if ( m_iBackgroundImageID != -1 )
	{
		// evict prior image
		surface()->DestroyTextureID( m_iBackgroundImageID );
		m_iBackgroundImageID = -1;
	}

	m_iBackgroundImageID = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iBackgroundImageID, pFadeFilename, true, false );

	bool bUseMono = false;
#if defined( _X360 )
	// cannot use the very large stereo version during the install
	// need a lighter memory version
	bUseMono = g_pXboxInstaller->IsInstallEnabled() && !g_pXboxInstaller->IsFullyInstalled();
#if defined( _DEMO )
	bUseMono = true;
#endif
#endif

	// the precache will be a memory or stream wave as needed 
	// on 360 the sound system will detect the install state and force it to a memory wave to finalize the the i/o now
	// it will be a stream resource if the installer is dormant
	// On PC it will be a streaming MP3
	char backgroundMusic[MAX_PATH];
	GetBackgroundMusicName( backgroundMusic, sizeof( backgroundMusic ), bUseMono );
	if ( enginesound->PrecacheSound( backgroundMusic, true, false ) )
	{
		// successfully precached
		m_BackgroundMusicString = backgroundMusic;
	}
}

extern int SaveReadNameAndComment( FileHandle_t f, char *name, char *comment );

bool CBaseModPanel::GetSaveGameInfos( CUtlVector< SaveGameInfo_t > &saveGameInfos, bool bFindAll )
{
	// clear prior results
	saveGameInfos.Purge();
		
	char directory[MAX_PATH];

#if 0
	char path[MAX_PATH];
	const char *pSaveDir = engine->GetSaveDirName();
	V_snprintf( path, sizeof( path ), pSaveDir );
	V_snprintf( directory, sizeof( directory ), "%s*.sav", pSaveDir );
	
	V_FixSlashes( path );
#else // Limited by engine code, use legacy method instead
	
	Q_snprintf( directory, sizeof( directory ), "save/*.sav" );
#endif

	V_FixSlashes( directory );

	// iterate the saved files
	FileFindHandle_t hFind = -1;
	const char *pFileName = g_pFullFileSystem->FindFirst( directory, &hFind );
	while ( pFileName )
	{
		char fullFileName[MAX_PATH];
#if 0
		V_snprintf( fullFileName, sizeof( fullFileName ), "%s%s", path, pFileName );
#else // Legacy	
		Q_snprintf(fullFileName, sizeof( fullFileName ), "save/%s", pFileName);
#endif
		char szMapName[MAX_PATH];
		char szComment[MAX_PATH];

		FileHandle_t hFile = g_pFullFileSystem->Open( fullFileName, "rb", "MOD" );
		if ( hFile != FILESYSTEM_INVALID_HANDLE )
		{
			if ( SaveReadNameAndComment( hFile, szMapName, szComment ) )
			{
				char szTempMapName[MAX_PATH];
				V_strncpy( szTempMapName, szMapName, sizeof( szMapName ) );
				V_FixSlashes( szTempMapName );
				bool bInvalidMapName = V_stristr( szTempMapName, "puzzlemaker\\" ) != NULL || V_stristr( szTempMapName, "workshop\\" ) != NULL;

				if ( !bInvalidMapName )
				{
					int iIndex = saveGameInfos.AddToTail();

					// recover elapsed play time, encoded at tail of comment as 000:00
					char elapsedTime[32];
					int nMinutes = 0;
					int nSeconds = 0;
					int commentLength = strlen( szComment );
					if ( commentLength >= 6 )
					{
						// format mmm:ss
						V_strncpy( elapsedTime, &szComment[commentLength - 6], sizeof( elapsedTime ) );
						elapsedTime[6] = '\0';
						nMinutes = atoi( elapsedTime );
						nSeconds = atoi( elapsedTime + 4 );
					}

					saveGameInfos[iIndex].m_InternalIDname = pFileName;
					saveGameInfos[iIndex].m_Filename = pFileName;
					saveGameInfos[iIndex].m_FullFilename = fullFileName;
					saveGameInfos[iIndex].m_MapName = szMapName;
					saveGameInfos[iIndex].m_Comment = szComment;
					saveGameInfos[iIndex].m_nFileTime = g_pFullFileSystem->GetFileTime( fullFileName );
					saveGameInfos[iIndex].m_nElapsedSeconds = nMinutes * 60 + nSeconds;
					saveGameInfos[iIndex].m_nChapterNum = MapNameToChapter( szMapName );
					saveGameInfos[iIndex].m_bIsAutoSave = ( V_stristr( pFileName, "autosave" ) != NULL );
					saveGameInfos[iIndex].m_bIsCloudSave = false;
					saveGameInfos[iIndex].m_bIsInCloud = false;

					// look for possible screenshot
					char screenshotFilename[MAX_PATH];
					V_StripExtension( fullFileName, screenshotFilename, sizeof( screenshotFilename ) );
					V_strncat( screenshotFilename, ".tga", sizeof( screenshotFilename ) );
					if ( g_pFullFileSystem->FileExists( screenshotFilename, "MOD" ) )
					{
						saveGameInfos[iIndex].m_ScreenshotFilename = screenshotFilename;
					}
				}
			}

			g_pFullFileSystem->Close( hFile );

			if ( !bFindAll && saveGameInfos.Count() )
			{
				// caller wants any first found
				break;
			}
		}

		pFileName = g_pFullFileSystem->FindNext( hFind );
	}
	g_pFullFileSystem->FindClose( hFind );

	return ( saveGameInfos.Count() != 0 );
}

int CBaseModPanel::GetMostRecentSaveGame( CUtlVector< SaveGameInfo_t > &saveGameInfos )
{
	long nBestFileTime = 0;
	int iBestIndex = 0;

	if ( saveGameInfos.Count() > 1 )
	{
		// scan for largest file time, which would be most recent
		for ( int i = 0; i < saveGameInfos.Count(); i++ )
		{
			if ( nBestFileTime < saveGameInfos[i].m_nFileTime )
			{
				nBestFileTime = saveGameInfos[i].m_nFileTime;
				iBestIndex = i;
			}
		}
	}

	return iBestIndex;
}

bool CBaseModPanel::IsUserIdleForAttractMode()
{
	if ( GetWindow( WT_GENERICCONFIRMATION ) )
	{
		// confirmation is up
		return false;
	}

	if ( GetWindow( WT_GENERICWAITSCREEN ) )
	{
		// progress spinner is up
		return false;
	}

	if ( GetWindow( WT_MOVIEPLAYER ) )
	{
		// attract mode movie player is already up
		return false;
	}

	if ( CUIGameData::Get() && ( CUIGameData::Get()->IsXUIOpen() || CUIGameData::Get()->IsSteamOverlayActive() ) )
	{
		// X360 blade UI is open
		return false;
	}

#if defined( _X360 )
	if ( g_pXboxInstaller->IsInstallEnabled() && !g_pXboxInstaller->IsFullyInstalled() )
	{
		// attract mode cannot run while installer is active
		return false;
	}
#endif

	CAttractScreen *pAttractScreen = (CAttractScreen *)GetWindow( WT_ATTRACTSCREEN );
	if ( pAttractScreen && !pAttractScreen->IsUserIdleForAttractMode() )
	{
		// the attract screen says no
		return false;
	}

	WINDOW_TYPE activeWindow = GetActiveWindowType();
	if ( activeWindow != WT_ATTRACTSCREEN && activeWindow != WT_MAINMENU )
	{
		// only allow the attart mode to kick off from the attract or main menu
		return false;
	}

	return true;
}

void CBaseModPanel::ResetAttractDemoTimeout( bool bForce )
{
	// only reset if enabled
	if ( bForce || m_flAttractDemoTimeout )
	{
		// reset
		m_flAttractDemoTimeout = -1;
	}
}

void CBaseModPanel::SetupPartnerInScience()
{
	XUID xuidMachine = 0ull;
	const char *pNetworkString = "";

	IMatchSession *pSession = g_pMatchFramework->GetMatchSession();
	if ( pSession ) 
	{
		KeyValues *pSettings = pSession->GetSessionSettings();

		pNetworkString = pSettings->GetString( "system/network" );
		uint64 uiJoinFlags = pSettings->GetUint64( "members/joinflags", 0ull );
		bool bConsole = !!(uiJoinFlags & 1);

		int numMachines = pSettings->GetInt( "members/numMachines" );
		for ( int k = 0; k < numMachines; ++k )
		{
			XUID xuidThisMachine = pSettings->GetUint64( CFmtStr( "members/machine%d/id", k ) );
			if ( xuidThisMachine != g_pMatchFramework->GetMatchSystem()->GetPlayerManager()->GetLocalPlayer( XBX_GetActiveUserId() )->GetXUID() )
			{
				if ( bConsole )
				{
					xuidThisMachine = pSettings->GetUint64( CFmtStr( "members/machine%d/psnid", k ) );
				}
				if ( xuidThisMachine )
				{
					char const *pNamePartner = pSettings->GetString( CFmtStr( "members/machine%d/player0/name", k ) );
					m_PartnerNameString = CUIGameData::Get()->GetPlayerName( xuidThisMachine, pNamePartner );
					xuidMachine = xuidThisMachine;
					break;
				}
			}
		}
	}

	if ( !xuidMachine || xuidMachine == m_xuidAvatarImage )
	{
		// per Vitaliy, if we somehow lose the xuid don't update prior results
		// or the xuid has not changed
		return;
	}

	if ( m_pAvatarImage )
	{
		// release prior results
		CUIGameData::Get()->AccessAvatarImage( m_xuidAvatarImage, CUIGameData::kAvatarImageRelease, CGameUiAvatarImage::LARGE );
		m_pAvatarImage = NULL;
		m_xuidAvatarImage = 0ull;
	}

	m_xuidAvatarImage = xuidMachine;

	// only want the avatar image for live enabled
	// callers use a better default image
	if ( !V_stricmp( pNetworkString, "live" ) )
	{
		m_pAvatarImage = CUIGameData::Get()->AccessAvatarImage( m_xuidAvatarImage, CUIGameData::kAvatarImageRequest, CGameUiAvatarImage::LARGE );
	}
}

char const * CBaseModPanel::GetPartnerDescKey()
{
	char const *szResult = "#PORTAL2_Coop_YourPartnerInScience";
#ifdef _PS3
	IMatchSession *pSession = g_pMatchFramework->GetMatchSession();
	if ( pSession )
	{
		uint64 uiJoinFlags = pSession->GetSessionSettings()->GetUint64( "members/joinflags", 0ull );
		bool bConsole = !!(uiJoinFlags & 1);
		return bConsole ? "#PORTAL2_Coop_YourPartnerOnPSN" : "#PORTAL2_Coop_YourPartnerOnSteam";
	}
#endif
	return szResult;
}

#if !defined( NO_STEAM )

//-----------------------------------------------------------------------------
// Purpose: Have we received all of our quick play entries?
//-----------------------------------------------------------------------------
bool CBaseModPanel::QuickPlayEntriesReady( void ) const 
{
	// Need to have at least got the first response
	if ( HasReceivedQuickPlayBaseline() == false )
		return false;

	// Must be marked as being complete with our 
	return m_bQuickPlayQueueReady;
}

//-----------------------------------------------------------------------------
// Purpose: Have we received all of our quick play entries?
//-----------------------------------------------------------------------------
bool CBaseModPanel::QuickPlayEntriesError( void ) const 
{
	// Need to have at least got the first response
	if ( HasReceivedQuickPlayBaseline() == false )
		return false;

	// Must be marked as being complete with our 
	return m_bQuickPlayQueueError;
}

//-----------------------------------------------------------------------------
// Purpose: Determine if there's already a request for a file
//-----------------------------------------------------------------------------
bool CBaseModPanel::QueueHistoryReady( void ) const
{
	if ( HasReceivedQueueHistoryBaseline() == false )
		return false;

	{
		return m_bQueueHistoryReady;
	}
}


#endif // NO_STEAM

//-----------------------------------------------------------------------------
// Purpose: Opens a generic message dialog with an "OK" button and nothing else
//-----------------------------------------------------------------------------
bool CBaseModPanel::OpenMessageDialog( const char *lpszTitle, const char *lpszMessage )
{
	// Open the generic confirmation window
	GenericConfirmation *pMsg = ( GenericConfirmation* ) OpenWindow( WT_GENERICCONFIRMATION, NULL, true );
	if ( pMsg == NULL )
		return false;

	// Package the setup
	GenericConfirmation::Data_t data;
	data.pWindowTitle = lpszTitle;
	data.pMessageText = lpszMessage;
	data.bOkButtonEnabled = true;
	pMsg->SetUsageData( data );

	return true;
}