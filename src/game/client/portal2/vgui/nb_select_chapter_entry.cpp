#include "cbase.h"
#include "nb_select_chapter_entry.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_bitmapbutton.h"
#include "KeyValues.h"
#include "missionchooser/iasw_mission_chooser.h"
#include "missionchooser/iasw_mission_chooser_source.h"
#include "filesystem.h"
#include "nb_select_chapter_panel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CNB_Select_Chapter_Entry::CNB_Select_Chapter_Entry( vgui::Panel *parent, const char *name, const char *levelname ) : BaseClass( parent, name )
{
	// == MANAGED_MEMBER_CREATION_START: Do not edit by hand ==
	m_pImage = new CBitmapButton( this, "Image", "" );
	m_pName = new vgui::Label( this, "Name", "" );
	// == MANAGED_MEMBER_CREATION_END ==

	StopUpdating = false;

	m_pImage->AddActionSignalTarget( this );
	m_pImage->SetCommand( "LevelClicked" );

	SetMap( levelname );
}

CNB_Select_Chapter_Entry::~CNB_Select_Chapter_Entry()
{

}

void CNB_Select_Chapter_Entry::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	LoadControlSettings( "resource/ui/nb_sdk_select_chapter_entry.res" );
}

void CNB_Select_Chapter_Entry::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CNB_Select_Chapter_Entry::OnThink()
{
	BaseClass::OnThink();

	color32 white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	white.a = 255;

	m_pName->SetText(m_szLevelName);
	const char *imagename = "vgui/swarm/MissionPics/UnknownMissionPic.vtf";

	// Check which chapter is selected based on the label text
	const char *labelText = m_szLevelName;
	if (!Q_stricmp(labelText, "The Courtesy Call"))
	{
		imagename = "vgui/portal2/chapter1";
	}
	else if (!Q_stricmp(labelText, "The Cold Boot"))
	{
		imagename = "vgui/portal2/chapter2";
	}
	else if (!Q_stricmp(labelText, "The Return"))
	{
		imagename = "vgui/portal2/chapter3";
	}
	else if (!Q_stricmp(labelText, "The Surprise"))
	{
		imagename = "vgui/portal2/chapter4";
	}
	else if (!Q_stricmp(labelText, "The Escape"))
	{
		imagename = "vgui/portal2/chapter5";
	}
	else if (!Q_stricmp(labelText, "The Fall"))
	{
		imagename = "vgui/portal2/chapter6";
	}
	else if (!Q_stricmp(labelText, "The Reunion"))
	{
		imagename = "vgui/portal2/chapter7";
	}
	else if (!Q_stricmp(labelText, "The Itch"))
	{
		imagename = "vgui/portal2/chapter8";
	}
	else if (!Q_stricmp(labelText, "The Part Where He Kills You"))
	{
		imagename = "vgui/portal2/chapter9";
	}
	else if (!Q_stricmp(labelText, "The Credits"))
	{
		imagename = "vgui/portal2/chapter10";
	}

	m_pImage->SetImage(CBitmapButton::BUTTON_ENABLED, imagename, white);
	m_pImage->SetImage(CBitmapButton::BUTTON_ENABLED_MOUSE_OVER, imagename, white);
	m_pImage->SetImage(CBitmapButton::BUTTON_PRESSED, imagename, white);
	m_pImage->SetVisible(true);
}

void CNB_Select_Chapter_Entry::OnCommand(const char *command)
{
	if (!Q_stricmp("LevelClicked", command))
	{
		Msg("Starting New Game At Chapter: %s\n", m_szLevelName);
		const char *labelText = m_szLevelName;

		// Check which chapter is selected based on the label text
		if (!Q_stricmp(labelText, "The Courtesy Call"))
		{
			// Execute the command for Chapter 1
			engine->ClientCmd("exec chapter1.cfg");
		}
		else if (!Q_stricmp(labelText, "The Cold Boot"))
		{
			// Execute the command for Chapter 2
			engine->ClientCmd("exec chapter2.cfg");
		}
		else if (!Q_stricmp(labelText, "The Return"))
		{
			// Execute the command for Chapter 3
			engine->ClientCmd("exec chapter3.cfg");
		}
		else if (!Q_stricmp(labelText, "The Surprise"))
		{
			// Execute the command for Chapter 4
			engine->ClientCmd("exec chapter4.cfg");
		}
		else if (!Q_stricmp(labelText, "The Escape"))
		{
			// Execute the command for Chapter 5
			engine->ClientCmd("exec chapter5.cfg");
		}
		else if (!Q_stricmp(labelText, "The Fall"))
		{
			// Execute the command for Chapter 6
			engine->ClientCmd("exec chapter6.cfg");
		}
		else if (!Q_stricmp(labelText, "The Reunion"))
		{
			// Execute the command for Chapter 7
			engine->ClientCmd("exec chapter7.cfg");
		}
		else if (!Q_stricmp(labelText, "The Itch"))
		{
			// Execute the command for Chapter 8
			engine->ClientCmd("exec chapter8.cfg");
		}
		else if (!Q_stricmp(labelText, "The Part Where He Kills You"))
		{
			// Execute the command for Chapter 9
			engine->ClientCmd("exec chapter9.cfg");
		}
		else if (!Q_stricmp(labelText, "The Credits"))
		{
			// Execute the command for Chapter 10
			engine->ClientCmd("exec chapter10.cfg");
		}
		// New game started, we dont need this anymore
		CNB_Select_Chapter_Panel *pPanel = dynamic_cast<CNB_Select_Chapter_Panel*>(GetParent()->GetParent()->GetParent());
		if (pPanel)
		{
			pPanel->SetVisible(false);
		}
	}
}


void CNB_Select_Chapter_Entry::SetMap(const char *levelname)
{
	m_szLevelName[0] = 0;
	Q_strncpy( m_szLevelName, levelname, 256 );
}