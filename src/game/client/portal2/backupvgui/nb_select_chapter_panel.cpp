#include "cbase.h"
#include "nb_select_chapter_panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Button.h"
#include "nb_select_chapter_entry.h"
#include "nb_horiz_list.h"
#include "nb_header_footer.h"
#include "nb_button.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CNB_Select_Chapter_Panel::CNB_Select_Chapter_Panel( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
	// == MANAGED_MEMBER_CREATION_START: Do not edit by hand ==
	m_pHeaderFooter = new CNB_Header_Footer( this, "HeaderFooter" );
	m_pHorizList = new CNB_Horiz_List( this, "HorizList" );
	// == MANAGED_MEMBER_CREATION_END ==
	m_pBackButton = new CNB_Button( this, "BackButton", "", this, "BackButton" );

	m_pHeaderFooter->SetTitle( "" );
	m_pHeaderFooter->SetHeaderEnabled( false );
}

CNB_Select_Chapter_Panel::~CNB_Select_Chapter_Panel()
{
	
}

void CNB_Select_Chapter_Panel::UpdateLevelList()
{
	const char* chapterNames[] = {
		"The Courtesy Call",
		"The Cold Boot",
		"The Return",
		"The Surprise",
		"The Escape",
		"The Fall",
		"The Reunion",
		"The Itch",
		"The Part Where He Kills You",
		"The Credits"
	};

	// Clear out existing map list
	m_MapList.RemoveAll();

	// Create entries
	for (int i = 0; i < 10; i++)
	{
		if (m_pHorizList->m_Entries.Count() <= i)
		{
			// Spawn instances of the level entries with the specified names
			CNB_Select_Chapter_Entry* pEntry = new CNB_Select_Chapter_Entry(NULL, "Select_Chapter_Entry", chapterNames[i]);
			m_pHorizList->AddEntry(pEntry);
		}
		else
		{
			CNB_Select_Chapter_Entry* pEntry = dynamic_cast<CNB_Select_Chapter_Entry*>(m_pHorizList->m_Entries[i].Get());
			pEntry->SetMap(chapterNames[i]);
		}
	}
}




void CNB_Select_Chapter_Panel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	LoadControlSettings( "resource/ui/nb_sdk_select_chapter_panel.res" );

	m_pHorizList->m_pBackgroundImage->SetImage( "briefing/select_marine_list_bg" );
	m_pHorizList->m_pForegroundImage->SetImage( "briefing/horiz_list_fg" );
}

void CNB_Select_Chapter_Panel::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CNB_Select_Chapter_Panel::OnThink()
{
	BaseClass::OnThink();

	UpdateLevelList();
}


void CNB_Select_Chapter_Panel::OnCommand( const char *command )
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