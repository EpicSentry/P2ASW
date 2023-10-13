//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef _BASEMODFACTORYBASEPANEL_H__
#define _BASEMODFACTORYBASEPANEL_H__

#define GAMEUI_BASEMODPANEL_VGUI
#define GAMEUI_BASEMODPANEL_SCHEME "basemodui_scheme"

#include "vgui_controls/Panel.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/Button.h"
#include "tier1/utllinkedlist.h"
#include "../OptionsDialog.h"
#include "../OptionsSubKeyboard.h"
#include "avi/ibik.h"
#include "ixboxsystem.h"
#include "matchmaking/imatchframework.h"
#include "utlmap.h"


#define BASEMODPANEL_SINGLETON BaseModUI::CBaseModPanel::GetSingleton()


// must supply some non-trivial time to let the movie startup smoothly
#define TRANSITION_FROM_OVERLAY_DELAY_TIME	0.5f	// how long to wait before starting the fade
#define TRANSITION_OVERLAY_FADE_TIME		0.7f	// how fast to fade

enum OverlayResult_t
{
	RESULT_OK = 0,
	RESULT_FAIL_OVERLAY_DISABLED,
	RESULT_FAIL_INVALID_UNIVERSE,
	RESULT_FAIL_INVALID_USER_ID,
	RESULT_FAIL_MISSING_API
};

struct SaveGameInfo_t
{
	SaveGameInfo_t()
	{
		m_nFileTime = 0;
		m_nElapsedSeconds = 0;
		m_nChapterNum = 1;
		m_bIsAutoSave = false;
		m_bIsCloudSave = false;
		m_bIsInCloud = false;
	}

	CUtlString		m_InternalIDname;
	CUtlString		m_Filename;
	CUtlString		m_FullFilename;
	CUtlString		m_ScreenshotFilename;
	CUtlString		m_MapName;
	CUtlString		m_Comment;
	time_t			m_nFileTime;
	unsigned int	m_nElapsedSeconds;
	int				m_nChapterNum;
	bool			m_bIsAutoSave;
	bool			m_bIsCloudSave;
	bool			m_bIsInCloud;
};

namespace BaseModUI 
{
	enum WINDOW_TYPE 
	{
		WT_NONE = 0,
		WT_ACHIEVEMENTS,
		WT_AUDIO,
		WT_AUDIOVIDEO,
		WT_CLOUD,
		WT_CONTROLLER,
		WT_CONTROLLER_STICKS,
		WT_CONTROLLER_BUTTONS,
		WT_DOWNLOADS,
		WT_GAMELOBBY,
		WT_GAMEOPTIONS,
		WT_GAMESETTINGS,
		WT_GENERICCONFIRMATION,
		WT_INGAMEDIFFICULTYSELECT,
		WT_INGAMEMAINMENU,
		WT_INGAMECHAPTERSELECT,
		WT_INGAMEKICKPLAYERLIST,
		WT_VOTEOPTIONS,
		WT_KEYBOARDMOUSE,
		WT_LOADINGPROGRESS,
		WT_MAINMENU,
		WT_ENDINGSPLITSCREEN,
		WT_MULTIPLAYER,
		WT_OPTIONS,
		WT_OPTIONSCLOUD,
		WT_SEARCHINGFORLIVEGAMES,
		WT_SIGNINDIALOG,
		WT_STEAMLINKDIALOG,
		WT_SINGLEPLAYER,
		WT_COOPMODESELECT,
		WT_GENERICWAITSCREEN,
		WT_ATTRACTSCREEN,
		WT_STARTCOOPGAME,
		WT_ALLGAMESEARCHRESULTS,
		WT_PVP_LOBBY,
		WT_FOUNDPUBLICGAMES,
		WT_TRANSITIONSCREEN,
		WT_PASSWORDENTRY,
		WT_VIDEO,
		WT_STEAMCLOUDCONFIRM,
		WT_STEAMGROUPSERVERS,
		WT_CUSTOMCAMPAIGNS,
		WT_ADDONS,
		WT_DOWNLOADCAMPAIGN,
		WT_LEADERBOARD,
		WT_ADDONASSOCIATION,
		WT_GETLEGACYDATA,
		WT_NEWGAME,
		WT_CHALLENGEMODE,
		WT_SAVEGAME,
		WT_LOADGAME,
		WT_MOVIEPLAYER,
		WT_COMMENTARY,
		WT_XBOXLIVE,
		WT_AUTOSAVENOTICE,
		WT_FADEOUTSTARTGAME,
		WT_ADVANCEDVIDEO,
		WT_KEYBINDINGS,
		WT_SOUNDTEST,
		WT_PORTALLEADERBOARD,
		WT_PORTALCOOPLEADERBOARD,
		WT_PORTALLEADERBOARDHUD,
		WT_COOPEXITCHOICE,
		WT_EXTRAS,
		WT_FADEOUTTOECONUI,
		WT_WINDOW_COUNT // WT_WINDOW_COUNT must be last in the list!
	};

	enum WINDOW_PRIORITY 
	{
		WPRI_NONE,
		WPRI_BKGNDSCREEN,
		WPRI_NORMAL,
		WPRI_WAITSCREEN,
		WPRI_MESSAGE,
		WPRI_LOADINGPLAQUE,
		WPRI_TOPMOST,			// must be highest priority, no other windows can obscure
		WPRI_COUNT				// WPRI_COUNT must be last in the list!
	};

	enum UISound_t
	{
		UISOUND_BACK,
		UISOUND_ACCEPT,
		UISOUND_INVALID,
		UISOUND_COUNTDOWN,
		UISOUND_FOCUS,
		UISOUND_CLICK,
		UISOUND_DENY,
		UISOUND_TILE_CLICK1,
		UISOUND_TILE_CLICK2,
	};

	class CBaseModFrame;
	class CBaseModFooterPanel;
	class CBaseModTransitionPanel;

	//=============================================================================
	//
	//=============================================================================
	class CBaseModPanel : public vgui::EditablePanel, public IMatchEventsSink
	{
		DECLARE_CLASS_SIMPLE( CBaseModPanel, vgui::EditablePanel );

	public:
		CBaseModPanel();
		~CBaseModPanel();

		// IMatchEventSink implementation
	public:
		virtual void OnEvent( KeyValues *pEvent );

	public:
		static CBaseModPanel& GetSingleton();
		static CBaseModPanel* GetSingletonPtr();

		void ReloadScheme();

		CBaseModFrame* OpenWindow( const WINDOW_TYPE& wt, CBaseModFrame * caller, bool hidePrevious = true, KeyValues *pParameters = NULL );
		CBaseModFrame* GetWindow( const WINDOW_TYPE& wt );

		void OnFrameClosed( WINDOW_PRIORITY pri, WINDOW_TYPE wt );
		void DbgShowCurrentUIState();
		bool IsLevelLoading();

		void OnClientReady();

		WINDOW_TYPE GetActiveWindowType();
		WINDOW_PRIORITY GetActiveWindowPriority();
		void SetActiveWindow( CBaseModFrame * frame );

		bool OpenMessageDialog( const char *lpszTitle, const char *lpszMessage );

		enum CloseWindowsPolicy_t
		{
			CLOSE_POLICY_DEFAULT = 0,			// will keep msg boxes alive
			CLOSE_POLICY_EVEN_MSGS = 1,			// will kill even msg boxes
			CLOSE_POLICY_EVEN_LOADING = 2,		// will kill even loading screen
			CLOSE_POLICY_KEEP_BKGND = 0x100,	// will keep bkgnd screen
		};
		void CloseAllWindows( int ePolicyFlags = CLOSE_POLICY_DEFAULT );

		void OnGameUIActivated();
		void OnGameUIHidden();
		void OpenFrontScreen( bool bIgnoreMatchSession = false );
		void RunFrame();
		void OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog );
		void OnLevelLoadingFinished( KeyValues *kvEvent );
		bool UpdateProgressBar(float progress, const char *statusText);
		void OnCreditsFinished(void);

		void SetOkButtonEnabled( bool enabled );
		void SetCancelButtonEnabled( bool enabled );

		const char *GetUISoundName(  UISound_t uiSound );
		void PlayUISound( UISound_t uiSound );
		void StartExitingProcess( bool bWarmRestart );

		CBaseModFooterPanel* GetFooterPanel();
		void SetLastActiveUserId( int userId );
		int GetLastActiveUserId();
		void OpenOptionsDialog( Panel *parent );
		void OpenKeyBindingsDialog( Panel *parent );

		MESSAGE_FUNC_CHARPTR( OnNavigateTo, "OnNavigateTo", panelName );
		MESSAGE_FUNC_PARAMS( MigrateHostToClient, "MigrateHostToClient", params );

		bool IsMenuBackgroundMovieValid( void );

		bool IsBackgroundMusicPlaying();
		bool StartBackgroundMusic( float fVol );
		void UpdateBackgroundMusicVolume( float fVol );
		void ReleaseBackgroundMusic();

		void OnTileSetChanged( void );	// We've swapped from a menu with one tile set to another

		void SafeNavigateTo( Panel *pExpectedFrom, Panel *pDesiredTo, bool bAllowStealFocus );

#if defined( _X360 ) && defined( _DEMO )
		void OnDemoTimeout();
#endif
		int MapNameToChapter( const char *pMapName, bool bSinglePlayer = true );
		int MapNameToSubChapter( const char *pMapName );
		const char *ChapterToMapName( int nChapter );
		const char *ActToMapName( int nAct );
		const char *GetMapName( int nChapter, int nMap, bool bSinglePlayer = true );
		int GetMapNumInChapter( int nChapter, const char *pMapName, bool bSinglePlayer = true );
		int ChapterToAct( int nChapter );
		int GetChapterProgress();
		int GetNumChapters( bool bSinglePlayer = true );
		int GetNumMaps( bool bSinglePlayer = true );
		int GetNumMapsInChapter( int nChapter, bool bSinglePlayer = true );
		int GetCoopTrackFromChapter( int nChapterNum );
		int GetCoopChapterFromTrack( int nTrackNum );

		bool GetSaveGameInfos( CUtlVector< SaveGameInfo_t > &saveGameInfos, bool bFindAll = true );
		int GetMostRecentSaveGame( CUtlVector< SaveGameInfo_t > &saveGameInfos );

		bool IsOpaqueOverlayActive();

		bool LoadingProgressWantsIsolatedRender( bool bContextValid );

		bool RenderMovie( BIKMaterial_t hBIKMaterial );
		void CalculateMovieParameters( BIKMaterial_t hBIKMaterial, bool bLetterbox = false );

		void ResetAttractDemoTimeout( bool bForce = false );
		void DrawColoredText( int x, int y, Color color, const char *pAnsiText, vgui::HFont hFont = vgui::INVALID_FONT );

		void SetupPartnerInScience();
		XUID GetPartnerXUID() { return m_xuidAvatarImage; }
		CUtlString &GetPartnerName() { return m_PartnerNameString; }
		vgui::IImage *GetPartnerImage() { return m_pAvatarImage; }
		char const *GetPartnerDescKey();
		

		bool IsTransitionEffectEnabled();
		CBaseModTransitionPanel *GetTransitionEffectPanel();
		void AddFadeinDelayAfterOverlay( float flDelay, bool bHideAndFadeinLater = false );

		void SetupBackgroundPresentation();

		int GetImageId( const char *pImageName );

		void ComputeCroppedTexcoords( float flBackgroundSourceAspectRatio /* ie. 16/9, 16/10 */, float flPhysicalAspectRatio /*GetWidth() / GetHeight() */, float &sMin, float &tMin, float &sMax, float &tMax );

		void MoveToEditorMainMenu( void ) { m_bMoveToEditorMainMenu = true; }

#if !defined( NO_STEAM )

		//
		// Queue history methods
		//

		void						QueryForQueueHistory( void );
		float						GetQueueHistoryBaselineRequestTime( void ) const { return m_flQueueHistoryBaselineRequestTime; }
		bool						HasReceivedQueueHistoryBaseline( void ) const { return m_bReceivedQueueHistoryBaseline; }
		bool						QueueHistoryReady( void ) const;

		//
		// Local map play order (for scripts playing sounds)
		//
		
		bool						LoadLocalMapPlayOrder( void );	// Load the local play order off disk
		bool						SaveLocalMapPlayOrder( void );

		// 
		// UGC methods
		// 

		//
		// Basic queue / history helper functions
		//
		
		// Quick play options
		float						GetQuickPlayBaselineRequestTime( void ) const { return m_flQuickPlayBaselineRequestTime; }
		bool						HasReceivedQuickPlayBaseline( void ) const { return m_bReceivedQuickPlayBaseline; }
		bool						QuickPlayEntriesReady( void ) const;
		bool						QuickPlayEntriesError( void ) const;

#endif // !NO_STEAM


		// --------------------------------------

	protected:
		CBaseModPanel(const CBaseModPanel&);
		CBaseModPanel& operator=(const CBaseModPanel&);

		void ApplySchemeSettings(vgui::IScheme *pScheme);
		void PaintBackground();
		virtual void PostChildPaint();

		void OnCommand(const char *command);
		void OnSetFocus();
		virtual void OnKeyCodePressed( vgui::KeyCode code );

		MESSAGE_FUNC( OnMovedPopupToFront, "OnMovedPopupToFront" );
		
	private:
		void DrawCopyStats();
		void OnEngineLevelLoadingSession( KeyValues *pEvent );
		bool ActivateBackgroundEffects();
		void SelectBackgroundPresentation();
		bool IsUserIdleForAttractMode();
		void PrecacheCommonImages();

		// Background movie playback
		bool			InitBackgroundMovie( void );
		bool			RenderBackgroundMovie();
		void			ShutdownBackgroundMovie( void );
		void			GetBackgroundMovieName( char *pOutBuffer, int nOutBufferSize );
		void			GetBackgroundMusicName( char *pOutBuffer, int nOutBufferSize, bool bMono );
		void			TransitionToNewBackgroundMovie( void );

#if !defined( NO_STEAM )

		//
		// Community map files
		//
		// This decides what the next quickplay map ID will be based on the passed in map ID
		
		bool				m_bReceivedQueueBaseline;					// Whether or not we've heard back successfully from Steam on our queue status
		float				m_flQueueBaselineRequestTime;				// Time that the baseline was requested from the GC
		bool				m_bQueueReady;								// We have all our information from Steam about our queue now

		//
		// Queue history
		//

		float							m_flQueueHistoryBaselineRequestTime;// Time that the baseline was requested from the GC
		bool							m_bReceivedQueueHistoryBaseline;	// Whether or not we've heard back successfully from Steam on our queue history entries
		int								m_nTotalQueueHistoryEntriesLoaded;	// Number of queue history entries we've received from the Steam server. This may not be the total number available, meaning we need to requery.
		bool							m_bQueueHistoryReady;
		
		// 
		// UGC Request Manager
		//

		bool							m_bUGCRequestsPaused;

		//
		// Quick play queue
		//

		int32							m_nTotalQuickPlayEntriesLoaded;
		bool							m_bReceivedQuickPlayBaseline;
		float							m_flQuickPlayBaselineRequestTime;
		bool							m_bQuickPlayQueueReady;
		bool							m_bQuickPlayQueueError;

#endif // !NO_STEAM

		bool			m_bSetup;
		int				m_nStartupFrames;

		BIKMaterial_t	m_BIKHandle;
		float			m_flU0, m_flV0, m_flU1, m_flV1;
		float			m_flMovieFadeInTime;					// Time to be fully faded in
		int				m_nMoviePlaybackWidth;
		int				m_nMoviePlaybackHeight;
		bool			m_bMovieFailed;
		bool			m_bMovieLetterbox;

		static CBaseModPanel* m_CFactoryBasePanel;

		vgui::HScheme m_UIScheme;
		vgui::HFont m_hDefaultFont;
		vgui::HFont m_hSteamCloudFont;

		vgui::DHANDLE< CBaseModFrame > m_Frames[WT_WINDOW_COUNT];
		vgui::DHANDLE< CBaseModFooterPanel > m_FooterPanel;
		vgui::DHANDLE< COptionsDialog > m_hOptionsDialog;	// standalone options dialog - PC only
		vgui::DHANDLE< CBaseModTransitionPanel > m_pTransitionPanel;

		WINDOW_TYPE m_ActiveWindow[WPRI_COUNT];

		int m_lastActiveUserId;

		float m_flFadeinDelayAfterOverlay;
		bool m_bHideAndFadeinLater;

		float m_flOverlayFadeOutTime;
		float m_flMusicFadeInTime;
		bool  m_bAllowMovie;

		int	m_iStartupImageID;
		int	m_iBackgroundImageID;
		int m_iFadeOutOverlayImageID;

		int m_DelayActivation;
		int m_ExitingFrameCount;

		bool m_LevelLoading;
		bool m_bWarmRestartMode;
		bool m_bClosingAllWindows;

		float m_flBlurScale;
		float m_flLastBlurTime;

		CUtlString m_BackgroundMusicString;
		int m_nBackgroundMusicGUID;
		bool m_bFadeMusicUp;

		int	m_iProductImageID;
		int m_iAltProductImageID;
		int m_nProductImageX;
		int m_nProductImageY;
		int m_nProductImageWide;
		int m_nProductImageTall;

		int m_iCloudImageID[2]; // 0 = downloading, 1 = uploading
		int m_iCloudPosition[4];
		int m_iCloudProgressPosition[4];
		int m_iCloudTextPosition[2];

		Color m_clrCloudRemaining, m_clrCloudDone, m_clrCloudDoneFade;
		Color m_clrSteamCloudText;

		int m_nCurrentActPresentation;

		float m_flAttractDemoTimeout;

		CUtlString m_LastLoadedLevelName;

		vgui::IImage	*m_pAvatarImage;
		XUID			m_xuidAvatarImage;
		CUtlString		m_PartnerNameString;

		bool			m_bMoveToEditorMainMenu;

		int	m_nActivationCount;

		bool			m_bForceUseAlternateTileSet;

		CUtlVector< int >	m_CommonImages;
	};
};

#endif
