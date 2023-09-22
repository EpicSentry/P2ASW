#include "cbase.h"
#include "nb_select_level_panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Button.h"
#include "nb_select_level_entry.h"
#include "nb_horiz_list.h"
#include "nb_header_footer.h"
#include "nb_button.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CNB_Select_Level_Panel::CNB_Select_Level_Panel( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
	// == MANAGED_MEMBER_CREATION_START: Do not edit by hand ==
	m_pHeaderFooter = new CNB_Header_Footer( this, "HeaderFooter" );
	m_pHorizList = new CNB_Horiz_List( this, "HorizList" );
	// == MANAGED_MEMBER_CREATION_END ==
	m_pBackButton = new CNB_Button( this, "BackButton", "", this, "BackButton" );

	m_pHeaderFooter->SetTitle( "" );
	m_pHeaderFooter->SetHeaderEnabled( false );
}

CNB_Select_Level_Panel::~CNB_Select_Level_Panel()
{
	
}

void StripChar(char *szBuffer, const char cWhiteSpace )
{

	while ( char *pSpace = strchr( szBuffer, cWhiteSpace ) )
	{
		char *pNextChar = pSpace + sizeof(char);
		V_strcpy( pSpace, pNextChar );
	}
}

void CNB_Select_Level_Panel::UpdateLevelList() 
{
	const char *mapcfile = "maplist.txt";

	// Check the time of the mapcycle file and re-populate the list of level names if the file has been modified
	const int nMapCycleTimeStamp = filesystem->GetPathTime( mapcfile, "GAME" );

	char *szDefaultMapName = "sdk_teams_hdr";

	if ( 0 == nMapCycleTimeStamp )
	{
		// Map cycle file does not exist, make a list containing only the default map
		m_MapList.AddToTail( szDefaultMapName );
	}
	else
	{
		// Clear out existing map list. Not using Purge() because I don't think that it will do a 'delete []'
		for ( int i = 0; i < m_MapList.Count(); i++ )
		{
			delete [] m_MapList[i];
		}

		m_MapList.RemoveAll();

		// Repopulate map list from mapcycle file
		int nFileLength;
		char *aFileList = (char*)UTIL_LoadFileForMe( mapcfile, &nFileLength );
		if ( aFileList && nFileLength )
		{
			V_SplitString( aFileList, "\n", m_MapList );

			for ( int i = 0; i < m_MapList.Count(); i++ )
			{
				bool bIgnore = false;

				// Strip out the spaces in the name
				StripChar( m_MapList[i] , '\r');
				StripChar( m_MapList[i] , ' ');
						
				/*
				if ( !engine->IsMapValid( m_MapList[i] ) )
				{
					bIgnore = true;

					// If the engine doesn't consider it a valid map remove it from the lists
					char szWarningMessage[MAX_PATH];
					V_snprintf( szWarningMessage, MAX_PATH, "Invalid map '%s' included in map cycle file. Ignored.\n", m_MapList[i] );
					Warning( szWarningMessage );
				}
				else */if ( !Q_strncmp( m_MapList[i], "//", 2 ) )
				{
					bIgnore = true;
				}

				if ( bIgnore )
				{
					delete [] m_MapList[i];
					m_MapList.Remove( i );
					--i;
				}
			}

			UTIL_FreeFile( (byte *)aFileList );
		}

		// If somehow we have no maps in the list then add the default one
		if ( 0 == m_MapList.Count() )
		{
			m_MapList.AddToTail( szDefaultMapName );
		}

		// Create entries
		for ( int i = 0; i < m_MapList.Count(); i++ )
		{
			if ( m_pHorizList->m_Entries.Count() <= i )
			{
				CNB_Select_Level_Entry *pEntry = new CNB_Select_Level_Entry( NULL, "Select_Level_Entry", m_MapList[i] );
				m_pHorizList->AddEntry( pEntry );
			}
			else
			{
				CNB_Select_Level_Entry *pEntry = dynamic_cast<CNB_Select_Level_Entry*>( m_pHorizList->m_Entries[i].Get() );
				pEntry->SetMap( m_MapList[i] );
			}
		}
	}
}

void CNB_Select_Level_Panel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	LoadControlSettings( "resource/ui/nb_sdk_select_level_panel.res" );

	m_pHorizList->m_pBackgroundImage->SetImage( "briefing/select_marine_list_bg" );
	m_pHorizList->m_pForegroundImage->SetImage( "briefing/horiz_list_fg" );
}

void CNB_Select_Level_Panel::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CNB_Select_Level_Panel::OnThink()
{
	BaseClass::OnThink();

	UpdateLevelList();
}


void CNB_Select_Level_Panel::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "BackButton" ) )
	{
		MarkForDeletion();
		return;
	}
	else if ( !Q_stricmp( command, "AcceptButton" ) )
	{

		GetParent()->OnCommand( command );
		return;
	}
	BaseClass::OnCommand( command );
}

void CNB_Select_Level_Panel::LevelSelected( const char *pLevelName )
{
	if ( !pLevelName || pLevelName[0] == 0 )
		return;

	// pass selected mission name up to vgamesettings
	char buffer[ 256 ];
	Q_snprintf( buffer, sizeof( buffer ), "cmd_level_selected_%s", pLevelName );
	GetParent()->OnCommand( buffer );

	MarkForDeletion();
}