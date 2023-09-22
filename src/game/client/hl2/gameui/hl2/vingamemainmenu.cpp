//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "VInGameMainMenu.h"
#include "VGenericConfirmation.h"
#include "VFooterPanel.h"
#include "VFlyoutMenu.h"
#include "VHybridButton.h"
#include "EngineInterface.h"

#include "fmtstr.h"

#include "game/client/IGameClientExports.h"
#include "GameUI_Interface.h"

#include "vgui/ILocalize.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui/ISurface.h"

#include "materialsystem/materialsystem_config.h"

#include "gameui_util.h"

// UI defines. Include if you want to implement some of them [str]
#include "ui_defines.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;

//extern class IMatchSystem *matchsystem;
extern IVEngineClient *engine;

void Demo_DisableButton( Button *pButton );
void OpenGammaDialog( VPANEL parent );

//=============================================================================
InGameMainMenu::InGameMainMenu( Panel *parent, const char *panelName ):
BaseClass( parent, panelName, false, true )
{
	SetDeleteSelfOnClose(true);

	SetProportional( true );
	SetTitle( "", false );

	SetLowerGarnishEnabled( true );

	SetFooterState();
}

//=============================================================================
static void LeaveGameOkCallback()
{
	COM_TimestampedLog( "Exit Game" );

	InGameMainMenu* self = 
		static_cast< InGameMainMenu* >( CBaseModPanel::GetSingleton().GetWindow( WT_INGAMEMAINMENU ) );

	if ( self )
	{
		self->Close();
	}

	engine->ExecuteClientCmd( "gameui_hide" );

	if ( IMatchSession *pMatchSession = g_pMatchFramework->GetMatchSession() )
	{
		// Closing an active session results in disconnecting from the game.
		g_pMatchFramework->CloseSession();
	}
	else
	{
		// On PC people can be playing via console bypassing matchmaking
		// and required session settings, so to leave game duplicate
		// session closure with an extra "disconnect" command.
		engine->ExecuteClientCmd( "disconnect" );
	}

	engine->ExecuteClientCmd( "gameui_activate" );

	CBaseModPanel::GetSingleton().CloseAllWindows();
	CBaseModPanel::GetSingleton().OpenFrontScreen();
}

void ShowPlayerList();

//=============================================================================
void InGameMainMenu::OnCommand( const char *command )
{
	int iUserSlot = CBaseModPanel::GetSingleton().GetLastActiveUserId();

	if ( UI_IsDebug() )
	{
		Msg("[GAMEUI] Handling ingame menu command %s from user%d ctrlr%d\n",
			command, iUserSlot, XBX_GetUserId( iUserSlot ) );
	}

	int iOldSlot = GetGameUIActiveSplitScreenPlayerSlot();

	SetGameUIActiveSplitScreenPlayerSlot( iUserSlot );

	GAMEUI_ACTIVE_SPLITSCREEN_PLAYER_GUARD( iUserSlot );

	if ( !Q_strcmp( command, "ReturnToGame" ) )
	{
		engine->ClientCmd("gameui_hide");
	}
	// --
	// SINGLEPLAYER
	else if ( !Q_strcmp( command, "StartNewGame" ) )
	{
#ifdef UI_USING_OLDDIALOGS
		CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
#else
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow( WT_NEWGAME, this, true );
#endif
	}
	else if ( !Q_strcmp( command, "SaveGame" ) )
	{
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow( WT_SAVEGAME, this, true );
	}
	else if ( !Q_strcmp( command, "LoadGame" ) )
	{
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow( WT_LOADGAME, this, true );
	}
	else if (!Q_strcmp(command, "Gameplay"))
	{
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow(WT_GAMEPLAYSETTINGS, this, true );
	}
	else if ( !Q_strcmp( command, "StatsAndAchievements" ) )
	{
		if ( CheckAndDisplayErrorIfNotLoggedIn() )
			return;
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow( WT_ACHIEVEMENTS, this, true );
	}
	else if (!Q_strcmp(command, "Controller"))
	{
		CBaseModPanel::GetSingleton().OpenWindow(WT_CONTROLLER, this, true );
	}
	else if (!Q_strcmp(command, "Audio"))
	{
		// audio options dialog, PC only
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow(WT_AUDIO, this, true );
	}
	else if (!Q_strcmp(command, "Video"))
	{
		// video options dialog, PC only
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow(WT_VIDEO, this, true );
	}
	else if (!Q_strcmp(command, "Brightness"))
	{
		// brightness options dialog, PC only
		OpenGammaDialog( GetVParent() );
	}
	else if (!Q_strcmp(command, "KeyboardMouse"))
	{
		// standalone keyboard/mouse dialog, PC only
		m_ActiveControl->NavigateFrom( );
		CBaseModPanel::GetSingleton().OpenWindow(WT_KEYBOARDMOUSE, this, true );
	}
	else if( Q_stricmp( "#L4D360UI_Controller_Edit_Keys_Buttons", command ) == 0 )
	{
		FlyoutMenu::CloseActiveMenu();
		CBaseModPanel::GetSingleton().OpenKeyBindingsDialog( this );
	}
	else if ( !Q_strcmp( command, "EnableSplitscreen" ) || !Q_strcmp( command, "DisableSplitscreen" ) )
	{
		GenericConfirmation* confirmation = 
			static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, true ) );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_LeaveMultiplayerConf";
		data.pMessageText = "#L4D360UI_MainMenu_SplitscreenChangeConfMsg";

		data.bOkButtonEnabled = true;
		data.pfnOkCallback = &LeaveGameOkCallback;
		data.bCancelButtonEnabled = true;

		confirmation->SetUsageData(data);
	}
	else if( !Q_strcmp( command, "ExitToMainMenu" ) )
	{
		GenericConfirmation* confirmation = 
			static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, true ) );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_LeaveMultiplayerConf";
		data.pMessageText = "#L4D360UI_LeaveMultiplayerConfMsg";
		data.bOkButtonEnabled = true;
		data.pfnOkCallback = &LeaveGameOkCallback;
		data.bCancelButtonEnabled = true;

		confirmation->SetUsageData(data);
	}
	else
	{
		const char *pchCommand = command;
		// does this command match a flyout menu?
		BaseModUI::FlyoutMenu *flyout = dynamic_cast< FlyoutMenu* >( FindChildByName( pchCommand ) );
		if ( flyout )
		{
			// If so, enumerate the buttons on the menu and find the button that issues this command.
			// (No other way to determine which button got pressed; no notion of "current" button on PC.)
			for ( int iChild = 0; iChild < GetChildCount(); iChild++ )
			{
				BaseModHybridButton *hybrid = dynamic_cast<BaseModHybridButton *>( GetChild( iChild ) );
				if ( hybrid && hybrid->GetCommand() && !Q_strcmp( hybrid->GetCommand()->GetString( "command"), command ) )
				{
#ifdef _X360
					hybrid->NavigateFrom( );
#endif //_X360
					// open the menu next to the button that got clicked
					flyout->OpenMenu( hybrid );
					break;
				}
			}
		}
	}

	SetGameUIActiveSplitScreenPlayerSlot( iOldSlot );
}

//=============================================================================
void InGameMainMenu::OnKeyCodePressed( KeyCode code )
{
	int userId = GetJoystickForCode( code );
	BaseModUI::CBaseModPanel::GetSingleton().SetLastActiveUserId( userId );

	switch( GetBaseButtonCode( code ) )
	{
	case KEY_XBUTTON_START:
	case KEY_XBUTTON_B:
		CBaseModPanel::GetSingleton().PlayUISound( UISOUND_BACK );
		OnCommand( "ReturnToGame" );
		break;
	default:
		BaseClass::OnKeyCodePressed( code );
		break;
	}
}

//=============================================================================
void InGameMainMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// We cant remove button and resize flyout menu from code, but we can use another .res file for base menu.
#ifdef UI_USING_GAMEPLAYCONFIGDIALOG
	if ( demo_ui_enable.GetString()[0] )
	{
		LoadControlSettings( CFmtStr( "Resource/UI/BaseModUI/InGameMainMenu_%s.res", demo_ui_enable.GetString() ) );
	}
	else
	{
		LoadControlSettings( "Resource/UI/BaseModUI/InGameMainMenu.res" );
	}
#else
	// all that "demoui" stuff confuse me [str]
	const char *pSettings = "Resource/UI/BaseModUI/InGameMainMenu_NoGameplay.res";
#endif

	BaseModHybridButton *button = dynamic_cast< BaseModHybridButton* >( FindChildByName( "BtnDeveloperCommentaries" ) );
	if ( button )
	{
#ifdef UI_USING_DEVCOMMENTARIES
		button->SetEnabled( true ); // we're not just removing button, because of ASWUI flyout system [str]
									// thought: maybe just use different control setting
#endif
	}

	SetPaintBackgroundEnabled( true );

	SetFooterState();
}

//=============================================================================
void InGameMainMenu::OnOpen()
{
	BaseClass::OnOpen();
	engine->ClientCmd_Unrestricted( "setpause" );	// thats hacky.
	/*engine->ServerCmd( "setpause" );				// thats even more hacky.
	engine->ServerCmd( "pause" );					// hacks. we love them.
	engine->ClientCmd( "pause" );					// everybody love them!
	engine->ClientCmd_Unrestricted( "pause" );*/	// i just love them more than others [str]
	if ( UI_IsDebug() )
	{
		Msg(" !! [GameUI] InGameMainMenu::OnOpen(): pause called \n");
	}
	SetFooterState();
}

void InGameMainMenu::OnClose()
{
	Unpause();

	// During shutdown this calls delete this, so Unpause should occur before this call
	BaseClass::OnClose();
}


void InGameMainMenu::OnThink()
{
	int iSlot = GetGameUIActiveSplitScreenPlayerSlot();

	GAMEUI_ACTIVE_SPLITSCREEN_PLAYER_GUARD( iSlot );

	{
		BaseModHybridButton *button = dynamic_cast< BaseModHybridButton* >( FindChildByName( "BtnOptions" ) );
		if ( button )
		{
			BaseModUI::FlyoutMenu *flyout = dynamic_cast< FlyoutMenu* >( FindChildByName( "FlmOptionsFlyout" ) );

			if ( flyout )
			{
#ifdef _X360
				bool bIsSplitscreen = ( XBX_GetNumGameUsers() > 1 );
#else
				bool bIsSplitscreen = false;
#endif

				Button *pButton = flyout->FindChildButtonByCommand( "EnableSplitscreen" );
				if ( pButton )
				{
					pButton->SetVisible( !bIsSplitscreen );
				}

				pButton = flyout->FindChildButtonByCommand( "DisableSplitscreen" );
				if ( pButton )
				{
					pButton->SetVisible( bIsSplitscreen );
				}
			}
		}
	}

	if ( IsPC() )
	{
		FlyoutMenu *pFlyout = dynamic_cast< FlyoutMenu* >( FindChildByName( "FlmOptionsFlyout" ) );
		if ( pFlyout )
		{
			const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();
			pFlyout->SetControlEnabled( "BtnBrightness", !config.Windowed() );
		}
	}

	BaseClass::OnThink();

	if ( IsVisible() )
	{
		// Yield to generic wait screen or message box if one of those is present
		WINDOW_TYPE arrYield[] = { WT_GENERICWAITSCREEN, WT_GENERICCONFIRMATION };
		for ( int j = 0; j < ARRAYSIZE( arrYield ); ++ j )
		{
			CBaseModFrame *pYield = CBaseModPanel::GetSingleton().GetWindow( arrYield[j] );
			if ( pYield && pYield->IsVisible() && !pYield->HasFocus() )
			{
				pYield->Activate();
				pYield->RequestFocus();
			}
		}
	}
}

//=============================================================================
void InGameMainMenu::PerformLayout( void )
{
	BaseClass::PerformLayout();

	BaseModUI::FlyoutMenu *flyout = dynamic_cast< FlyoutMenu* >( FindChildByName( "FlmIngameSingleplayerFlyout" ) );
	if ( flyout )
	{
		flyout->SetListener( this );
	}
	BaseModUI::FlyoutMenu *flyout2 = dynamic_cast< FlyoutMenu* >( FindChildByName( "FlmOptionsFlyout" ) );
	if ( flyout2 )
	{
		flyout2->SetListener( this );
	}
	BaseModHybridButton *button = dynamic_cast< BaseModHybridButton* >( FindChildByName( "BtnReturnToGame" ) );
	if ( button )
	{
		if( m_ActiveControl )
			m_ActiveControl->NavigateFrom();

		button->NavigateTo();
	}
}

void InGameMainMenu::Unpause( void )
{
	engine->ClientCmd_Unrestricted( "unpause" ); // thats kinda hacky, but whatever~ [str]
}

//=============================================================================
void InGameMainMenu::OnNotifyChildFocus( vgui::Panel* child )
{
}

void InGameMainMenu::OnFlyoutMenuClose( vgui::Panel* flyTo )
{
	SetFooterState();
}

void InGameMainMenu::OnFlyoutMenuCancelled()
{
}

//-----------------------------------------------------------------------------
// Purpose: Called when the GameUI is hidden
//-----------------------------------------------------------------------------
void InGameMainMenu::OnGameUIHidden()
{
	Unpause();
	Close();
}


//=============================================================================
void InGameMainMenu::PaintBackground()
{
	vgui::Panel *pPanel = FindChildByName( "PnlBackground" );
	if ( !pPanel )
		return;

	int x, y, wide, tall;
	pPanel->GetBounds( x, y, wide, tall );
	DrawSmearBackground( x, y, wide, tall );
}

//=============================================================================
void InGameMainMenu::SetFooterState()
{
	CBaseModFooterPanel *footer = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
	if ( footer )
	{
		footer->SetButtons( FB_ABUTTON | FB_BBUTTON, FF_AB_ONLY, false );
		footer->SetButtonText( FB_ABUTTON, "#L4D360UI_Select" );
		footer->SetButtonText( FB_BBUTTON, "#L4D360UI_Done" );
	}
}
