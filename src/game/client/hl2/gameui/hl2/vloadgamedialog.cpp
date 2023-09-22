//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#include "vLoadGameDialog.h"
#include "FileSystem.h"
#include "savegame_version.h"
#include "vgui_controls/PanelListPanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/tgaimagepanel.h"
#include "tier1/utlbuffer.h"
#include "tier2/resourceprecacher.h"
#include <stdio.h>
#include <stdlib.h>
#include "FileSystem.h"
#include "basemodui.h"
#include "MouseMessageForwardingPanel.h"
#include "nb_header_footer.h"
#include "nb_button.h"

#include "vGenericConfirmation.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;
#define TGA_IMAGE_PANEL_WIDTH 180
#define TGA_IMAGE_PANEL_HEIGHT 100

#define MAX_LISTED_SAVE_GAMES	128

PRECACHE_REGISTER_BEGIN( GLOBAL, VLoadGameDialog )
PRECACHE( MATERIAL, "vgui/resource/autosave.vmt" )
PRECACHE_REGISTER_END()

//-----------------------------------------------------------------------------
// Purpose: Describes the layout of a same game pic
//-----------------------------------------------------------------------------
class CSaveGamePanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CSaveGamePanel, vgui::EditablePanel );
public:
	CSaveGamePanel( PanelListPanel *parent, const char *name, int saveGameListItemID ) : BaseClass( parent, name )
	{
		m_iSaveGameListItemID = saveGameListItemID;
		m_pParent = parent;
		m_pSaveGameImage = new CTGAImagePanel( this, "SaveGameImage" );
		m_pAutoSaveImage = new ImagePanel( this, "AutoSaveImage" );
		m_pSaveGameScreenshotBackground = new ImagePanel( this, "SaveGameScreenshotBackground" );
		m_pChapterLabel = new Label( this, "ChapterLabel", "" );
		m_pTypeLabel = new Label( this, "TypeLabel", "" );
		m_pElapsedTimeLabel = new Label( this, "ElapsedTimeLabel", "" );
		m_pFileTimeLabel = new Label( this, "FileTimeLabel", "" );

		CMouseMessageForwardingPanel *panel = new CMouseMessageForwardingPanel(this, NULL);
		panel->SetZPos(2);

		SetSize( 300, 240 );

		LoadControlSettings( "resource/SaveGamePanel.res" );

		m_FillColor = m_pSaveGameScreenshotBackground->GetFillColor();
	}

	void SetSaveGameInfo( SaveGameDescription_t &save )
	{
		// set the bitmap to display
		char tga[_MAX_PATH];
		Q_strncpy( tga, save.szFileName, sizeof(tga) );
		char *ext = strstr( tga, ".sav" );
		if ( ext )
		{
			strcpy( ext, ".tga" );
		}

		// If a TGA file exists then it is a user created savegame
		if ( g_pFullFileSystem->FileExists( tga ) )
		{
			m_pSaveGameImage->SetTGAFilename( tga );
		}
		// If there is no TGA then it is either an autosave or the user TGA file has been deleted
		else
		{
			m_pSaveGameImage->SetVisible( false );
			m_pAutoSaveImage->SetVisible( true );
			m_pAutoSaveImage->SetImage( "resource\\autosave" );
		}

		// set the title text
		m_pChapterLabel->SetText( save.szComment );

		// type
		SetControlString( "TypeLabel", save.szType );
		SetControlString( "ElapsedTimeLabel", save.szElapsedTime );
		SetControlString( "FileTimeLabel", save.szFileTime );
	}

	MESSAGE_FUNC_INT( OnPanelSelected, "PanelSelected", state )
	{
		if ( state )
		{
			// set the text color to be orange, and the pic border to be orange
			m_pSaveGameScreenshotBackground->SetFillColor( m_SelectedColor );
			m_pChapterLabel->SetFgColor( m_SelectedColor );
			m_pTypeLabel->SetFgColor( m_SelectedColor );
			m_pElapsedTimeLabel->SetFgColor( m_SelectedColor );
			m_pFileTimeLabel->SetFgColor( m_SelectedColor );
		}
		else
		{
			m_pSaveGameScreenshotBackground->SetFillColor( m_FillColor );
			m_pChapterLabel->SetFgColor( m_TextColor );
			m_pTypeLabel->SetFgColor( m_TextColor );
			m_pElapsedTimeLabel->SetFgColor( m_TextColor );
			m_pFileTimeLabel->SetFgColor( m_TextColor );
		}

		PostMessage( m_pParent->GetVParent(), new KeyValues("PanelSelected") );
	}

	virtual void OnMousePressed( vgui::MouseCode code )
	{
		m_pParent->SetSelectedPanel( this );
	}

	virtual void ApplySchemeSettings( IScheme *pScheme )
	{
		m_TextColor = pScheme->GetColor( "NewGame.TextColor", Color(255, 255, 255, 255) );
		m_SelectedColor = pScheme->GetColor( "NewGame.SelectionColor", Color(255, 255, 255, 255) );

		BaseClass::ApplySchemeSettings( pScheme );
	}

	virtual void OnMouseDoublePressed( vgui::MouseCode code )
	{
		// call the panel
		OnMousePressed( code );
		PostMessage( m_pParent->GetParent(), new KeyValues("Command", "command", "loadsave") );
	}

	int GetSaveGameListItemID()
	{
		return m_iSaveGameListItemID;
	}

private:
	vgui::PanelListPanel *m_pParent;
	vgui::Label *m_pChapterLabel;
	CTGAImagePanel *m_pSaveGameImage;
	ImagePanel *m_pAutoSaveImage;
	
	// things to change color when the selection changes
	ImagePanel *m_pSaveGameScreenshotBackground;
	Label *m_pTypeLabel;
	Label *m_pElapsedTimeLabel;
	Label *m_pFileTimeLabel;
	Color m_TextColor, m_FillColor, m_SelectedColor;

	int m_iSaveGameListItemID;
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CLoadGameDialog::CLoadGameDialog( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
	GameUI().PreventEngineHideGameUI();

	m_pHeaderFooter = new CNB_Header_Footer( this, "HeaderFooter" );
	m_pHeaderFooter->SetTitle( "" );
	m_pHeaderFooter->SetHeaderEnabled( false );
	m_pHeaderFooter->SetFooterEnabled( true );
	m_pHeaderFooter->SetGradientBarEnabled( true );
	m_pHeaderFooter->SetGradientBarPos( 70, 330 );
	LoadControlSettings( "resource/ui/basemodui/LoadGame.res" );

	SetDeleteSelfOnClose(true);
	SetProportional( true );

	SetUpperGarnishEnabled(true);
	SetLowerGarnishEnabled( true );
	SetOkButtonEnabled( false );

	CreateSavedGamesList();
	ScanSavedGames();

	//new vgui::Button( this, "loadsave", "" );
	SetControlEnabled( "loadsave", false );
	SetControlEnabled( "Delete", false );
}
//=============================================================================
CLoadGameDialog::~CLoadGameDialog()
{
	GameUI().AllowEngineHideGameUI();
}
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *command - 
//-----------------------------------------------------------------------------
void CLoadGameDialog::OnCommand( const char *command )
{
	if ( !stricmp( command, "loadsave" )  )
	{
		SaveGameDescription_t &save = m_SaveGames.Element(GetSelectedItemSaveIndex());
		CSaveGamePanel *panel = dynamic_cast<CSaveGamePanel *>(m_pGameList->GetSelectedPanel());
		if ( panel )
		{
			const char *shortName = save.szShortName/*( "ShortName", "" )*/;
			if ( shortName && shortName[ 0 ] )
			{
				// Load the game, return to top and switch to engine
				char sz[ 256 ];
				sprintf(sz, "progress_enable\nload %s\n", shortName );
				
				engine->ClientCmd( sz );

				// Close this dialog
				OnClose();
			}
		}
	}
	else if ( !stricmp( command, "Delete" ) )
	{
		int saveIndex = GetSelectedItemSaveIndex();
		if ( m_SaveGames.IsValidIndex(saveIndex) )
		{
				GenericConfirmation* confirmation = 
				static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, false ) );

				GenericConfirmation::Data_t data;

				data.pWindowTitle = "#GameUI_ConfirmDeleteSaveGame_Title"; //valve surely use strange namings...[str]
				data.pMessageText = "#GameUI_ConfirmDeleteSaveGame_Info";

				data.bOkButtonEnabled = true;
				data.pfnOkCallback = &DeleteConfirmedCallback;
				data.bCancelButtonEnabled = true;

				confirmation->SetUsageData(data);
		}
	}
	else if ( !stricmp( command, "DeleteConfirmed" ) )
	{
		int saveIndex = GetSelectedItemSaveIndex();
		if ( m_SaveGames.IsValidIndex(saveIndex) )
		{
			DeleteSaveGame( m_SaveGames[saveIndex].szFileName );

			// reset the list
			ScanSavedGames();
			m_pGameList->MoveScrollBarToTop();
		}
	}
	else if( Q_stricmp( "Back", command ) == 0 )
	{
		NavigateBack();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}
//-----------------------------------------------------------------------------
// Purpose: Creates the load game display list
//-----------------------------------------------------------------------------
void CLoadGameDialog::CreateSavedGamesList()
{
	m_pGameList = new vgui::PanelListPanel( this, "listpanel_loadgame" );
	m_pGameList->SetFirstColumnWidth( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: returns the save file name of the selected item
//-----------------------------------------------------------------------------
int CLoadGameDialog::GetSelectedItemSaveIndex()
{
	CSaveGamePanel *panel = dynamic_cast<CSaveGamePanel *>(m_pGameList->GetSelectedPanel());
	if ( panel )
	{
		// find the panel in the list
		for ( int i = 0; i < m_SaveGames.Count(); i++ )
		{
			if ( i == panel->GetSaveGameListItemID() )
			{
				return i;
			}
		}
	}
	return m_SaveGames.InvalidIndex();
}

//-----------------------------------------------------------------------------
// Purpose: builds save game list from directory
//-----------------------------------------------------------------------------
void CLoadGameDialog::ScanSavedGames()
{
	// populate list box with all saved games on record:
	char	szDirectory[_MAX_PATH];
	Q_snprintf( szDirectory, sizeof( szDirectory ), "save/*.sav" );

	// clear the current list
	m_pGameList->DeleteAllItems();
	m_SaveGames.RemoveAll();
	
	// iterate the saved files
	FileFindHandle_t handle;
	const char *pFileName = g_pFullFileSystem->FindFirst( szDirectory, &handle );
	while (pFileName)
	{
		if ( !Q_strnicmp(pFileName, "HLSave", strlen( "HLSave" ) ) )
		{
			pFileName = g_pFullFileSystem->FindNext( handle );
			continue;
		}

		char szFileName[_MAX_PATH];
		Q_snprintf(szFileName, sizeof( szFileName ), "save/%s", pFileName);

		// Only load save games from the current mod's save dir
		if( !g_pFullFileSystem->FileExists( szFileName, "MOD" ) )
		{
			pFileName = g_pFullFileSystem->FindNext( handle );
			continue;
		}
		
		SaveGameDescription_t save;
		if ( ParseSaveData( szFileName, pFileName, save ) )
		{
			m_SaveGames.AddToTail( save );
		}
		
		pFileName = g_pFullFileSystem->FindNext( handle );
	}
	
	g_pFullFileSystem->FindClose( handle );

	// notify derived classes that save games are being scanned (so they can insert their own)
	OnScanningSaveGames();

	// sort the save list
	qsort( m_SaveGames.Base(), m_SaveGames.Count(), sizeof(SaveGameDescription_t), &SaveGameSortFunc );

	// add to the list
	for ( int saveIndex = 0; saveIndex < m_SaveGames.Count() && saveIndex < MAX_LISTED_SAVE_GAMES; saveIndex++ )
	{
		// add the item to the panel
		AddSaveGameItemToList( saveIndex );
	}

	// display a message if there are no save games
	if ( !m_SaveGames.Count() )
	{
		vgui::Label *pNoSavesLabel = SETUP_PANEL(new Label(m_pGameList, "NoSavesLabel", "#GameUI_NoSaveGamesToDisplay"));
		pNoSavesLabel->SetTextColorState(vgui::Label::CS_DULL);
		m_pGameList->AddItem( NULL, pNoSavesLabel );
	}
	InvalidateLayout();
	SetControlEnabled( "loadsave", false );
	SetControlEnabled( "Delete", false );
}

//-----------------------------------------------------------------------------
// Purpose: Adds an item to the list
//-----------------------------------------------------------------------------
void CLoadGameDialog::AddSaveGameItemToList( int saveIndex )
{
	// create the new panel and add to the list
	CSaveGamePanel *saveGamePanel = new CSaveGamePanel( m_pGameList, "SaveGamePanel", saveIndex );
	saveGamePanel->SetSaveGameInfo( m_SaveGames[saveIndex] );
	m_pGameList->AddItem( NULL, saveGamePanel );
	
}
extern int VSaveReadNameAndComment( FileHandle_t f, char *name, char *comment );
//-----------------------------------------------------------------------------
// Purpose: Parses the save game info out of the .sav file header
//-----------------------------------------------------------------------------
bool CLoadGameDialog::ParseSaveData( char const *pszFileName, char const *pszShortName, SaveGameDescription_t &save )
{
	char    szMapName[SAVEGAME_MAPNAME_LEN];
	char    szComment[SAVEGAME_COMMENT_LEN];
	char    szElapsedTime[SAVEGAME_ELAPSED_LEN];

	if ( !pszFileName || !pszShortName )
		return false;

	Q_strncpy( save.szShortName, pszShortName, sizeof(save.szShortName) );
	Q_strncpy( save.szFileName, pszFileName, sizeof(save.szFileName) );

	FileHandle_t fh = g_pFullFileSystem->Open( pszFileName, "rb", "MOD" );
	if (fh == FILESYSTEM_INVALID_HANDLE)
		return false;

	int readok = VSaveReadNameAndComment( fh, szMapName, szComment );
	g_pFullFileSystem->Close(fh);

	if ( !readok )
	{
		return false;
	}

	Q_strncpy( save.szMapName, szMapName, sizeof(save.szMapName) );

	// Elapsed time is the last 6 characters in comment. (mmm:ss)
	int i;
	i = strlen( szComment );
	Q_strncpy( szElapsedTime, "??", sizeof( szElapsedTime ) );
	if (i >= 6)
	{
		Q_strncpy( szElapsedTime, (char *)&szComment[i - 6], 7 );
		szElapsedTime[6] = '\0';

		// parse out
		int minutes = atoi( szElapsedTime );
		int seconds = atoi( szElapsedTime + 4);

		// reformat
		if ( minutes )
		{
			Q_snprintf( szElapsedTime, sizeof(szElapsedTime), "%d %s %d seconds", minutes, minutes > 1 ? "minutes" : "minute", seconds );
		}
		else
		{
			Q_snprintf( szElapsedTime, sizeof(szElapsedTime), "%d seconds", seconds );
		}

		// Chop elapsed out of comment.
		int n;

		n = i - 6;
		szComment[n] = '\0';
	
		n--;

		// Strip back the spaces at the end.
		while ((n >= 1) &&
			szComment[n] &&
			szComment[n] == ' ')
		{
			szComment[n--] = '\0';
		}
	}

	// calculate the file name to print
	const char *pszType = "";
	if (strstr(pszFileName, "quick"))
	{
		pszType = "#GameUI_QuickSave";
	}
	else if (strstr(pszFileName, "autosave"))
	{
		pszType = "#GameUI_AutoSave";
	}

	Q_strncpy( save.szType, pszType, sizeof(save.szType) );
	Q_strncpy( save.szComment, szComment, sizeof(save.szComment) );
	Q_strncpy( save.szElapsedTime, szElapsedTime, sizeof(save.szElapsedTime) );

	// Now get file time stamp.
	long fileTime = g_pFullFileSystem->GetFileTime(pszFileName);
	char szFileTime[32];
	g_pFullFileSystem->FileTimeToString(szFileTime, sizeof(szFileTime), fileTime);
	char *newline = strstr(szFileTime, "\n");
	if (newline)
	{
		*newline = 0;
	}
	Q_strncpy( save.szFileTime, szFileTime, sizeof(save.szFileTime) );
	save.iTimestamp = fileTime;
	return true;
}

//=============================================================================
Panel* CLoadGameDialog::NavigateBack()
{
	//DevMsg(1, " !! Load Game Dialog: Navigatin back \n");
	return BaseClass::NavigateBack();
}
void CLoadGameDialog::UpdateFooter()
{
	CBaseModFooterPanel *footer = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
	if ( footer )
	{
		footer->SetButtons( FB_ABUTTON | FB_BBUTTON | FB_XBUTTON, FF_ABX_ONLY, false );
		footer->SetButtonText( FB_ABUTTON, "#L4D360UI_Select" );
		footer->SetButtonText( FB_BBUTTON, "#L4D360UI_Done" );
		footer->SetButtonText( FB_XBUTTON, "#GameUI_Delete" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: deletes an existing save game
//-----------------------------------------------------------------------------
void CLoadGameDialog::DeleteSaveGame( const char *fileName )
{
	if ( !fileName || !fileName[0] )
		return;

	// delete the save game file
	g_pFullFileSystem->RemoveFile( fileName, "MOD" );

	// delete the associated tga
	char tga[_MAX_PATH];
	Q_strncpy( tga, fileName, sizeof(tga) );
	char *ext = strstr( tga, ".sav" );
	if ( ext )
	{
		strcpy( ext, ".tga" );
	}
	g_pFullFileSystem->RemoveFile( tga, "MOD" );
}

//-----------------------------------------------------------------------------
// Purpose: One item has been selected
//-----------------------------------------------------------------------------
void CLoadGameDialog::OnPanelSelected()
{
	SetControlEnabled( "loadsave", true );
	SetControlEnabled( "delete", true );
}
void CLoadGameDialog::DeleteConfirmedCallback()
{
	if ( CLoadGameDialog *pLoadGameDialog = static_cast< CLoadGameDialog* >( CBaseModPanel::GetSingleton().GetWindow( WT_MAINMENU ) ) )
	{
		pLoadGameDialog->OnCommand( "DeleteConfirmed" );
	}
}