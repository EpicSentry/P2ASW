//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef _BASEMODFACTORYBASEPANEL_H__
#define _BASEMODFACTORYBASEPANEL_H__

#include "vgui_controls/Panel.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/Button.h"
#include "tier1/utllinkedlist.h"
#include "../OptionsDialog.h"
#include "../OptionsSubKeyboard.h"
#include "avi/ibik.h"
#include "ixboxsystem.h"
//#include "matchmaking/imatchframework.h"

class COptionsDialog;

// must supply some non-trivial time to let the movie startup smoothly
// the attract screen also uses this so it doesn't pop in either
#define TRANSITION_TO_MOVIE_DELAY_TIME	0.5f	// how long to wait before starting the fade
#define TRANSITION_TO_MOVIE_FADE_TIME	1.2f	// how fast to fade

class IVTFTexture;

namespace BaseModUI 
{
	enum WINDOW_TYPE 
	{
		WT_NONE = 0,
		WT_ACHIEVEMENTS,
		WT_ATTRACTSCREEN,
		WT_AUDIO,
		WT_CONTROLLER,
		WT_CONTROLLER_STICKS,
		WT_CONTROLLER_BUTTONS,
		WT_GAMEOPTIONS,
		WT_GENERICCONFIRMATION,
		WT_INGAMEMAINMENU,
		WT_KEYBOARDMOUSE,
		WT_LOADINGPROGRESS,
		WT_MAINMENU,
		WT_OPTIONS,
		WT_SINGLEPLAYER, //what [str]
		WT_GENERICWAITSCREEN,
		WT_TRANSITIONSCREEN,
		WT_VIDEO,
		//---
		WT_NEWGAME,
		WT_LOADGAME,
		WT_SAVEGAME,
		WT_GAMEPLAYSETTINGS,
		WT_DEVELOPERCOMMENTARIES,
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
	};

	class CBaseModFrame;
	class CBaseModFooterPanel;

	//=============================================================================
	//
	//=============================================================================
	class CBaseModPanel : public vgui::EditablePanel
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

		WINDOW_TYPE GetActiveWindowType();
		WINDOW_PRIORITY GetActiveWindowPriority();
		void SetActiveWindow( CBaseModFrame * frame );

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
		void OpenFrontScreen();
		void RunFrame();
		void OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog );
		void OnLevelLoadingFinished( KeyValues *kvEvent );
		bool UpdateProgressBar(float progress, const char *statusText);
		void OnCreditsFinished(void);

		void SetHelpText( const char* helpText );
		void SetOkButtonEnabled( bool enabled );
		void SetCancelButtonEnabled( bool enabled );

		bool IsReadyToWriteConfig( void );
		const char *GetUISoundName(  UISound_t uiSound );
		void PlayUISound( UISound_t uiSound );
		void StartExitingProcess( bool bWarmRestart );

		CBaseModFooterPanel* GetFooterPanel();
		void SetLastActiveUserId( int userId );
		int GetLastActiveUserId();
		void OpenOptionsDialog( Panel *parent );
		void OpenKeyBindingsDialog( Panel *parent );

		MESSAGE_FUNC_CHARPTR( OnNavigateTo, "OnNavigateTo", panelName );

		bool IsMenuBackgroundMovieValid( void );

		bool IsBackgroundMusicPlaying();
		bool StartBackgroundMusic( float fVol );
		void UpdateBackgroundMusicVolume( float fVol );
		void ReleaseBackgroundMusic();

		void SafeNavigateTo( Panel *pExpectedFrom, Panel *pDesiredTo, bool bAllowStealFocus );

#if defined( _X360 ) && defined( _DEMO )
		void OnDemoTimeout();
#endif

	protected:
		CBaseModPanel(const CBaseModPanel&);
		CBaseModPanel& operator=(const CBaseModPanel&);

		void ApplySchemeSettings(vgui::IScheme *pScheme);
		void PaintBackground();

		void OnCommand(const char *command);
		void OnSetFocus();

		MESSAGE_FUNC( OnMovedPopupToFront, "OnMovedPopupToFront" );

	private:
		void DrawColoredText( vgui::HFont hFont, int x, int y, unsigned int color, const char *pAnsiText );
		void DrawCopyStats();
		void OnEngineLevelLoadingSession( KeyValues *pEvent );
		bool ActivateBackgroundEffects();

		static CBaseModPanel* m_CFactoryBasePanel;

		vgui::DHANDLE< CBaseModFrame > m_Frames[WT_WINDOW_COUNT];
		vgui::DHANDLE< CBaseModFooterPanel > m_FooterPanel;
		WINDOW_TYPE m_ActiveWindow[WPRI_COUNT];
		bool m_LevelLoading;
		vgui::HScheme m_UIScheme;
		vgui::DHANDLE<COptionsDialog> m_hOptionsDialog;	// standalone options dialog - PC only
		int m_lastActiveUserId;

		vgui::HFont m_hDefaultFont;

		int	m_iBackgroundImageID;
		int	m_iFadeToBackgroundImageID;
		float m_flMovieFadeInTime;

		int m_DelayActivation;
		int m_ExitingFrameCount;
		bool m_bWarmRestartMode;
		bool m_bClosingAllWindows;

		float m_flBlurScale;
		float m_flLastBlurTime;

		CUtlString m_backgroundMusic;
		int m_nBackgroundMusicGUID;

		int m_iProductImageID;
		int m_nProductImageX;
		int m_nProductImageY;
		int m_nProductImageWide;
		int m_nProductImageTall;

		char m_szFadeFilename[ MAX_PATH ];
		IMaterial *m_pBackgroundMaterial;
		KeyValues *m_pVMTKeyValues;

		void PrepareStartupGraphic();
		void ReleaseStartupGraphic();
		void DrawStartupGraphic( float flNormalizedAlpha );
		IVTFTexture			*m_pBackgroundTexture;
	};
};

#endif
