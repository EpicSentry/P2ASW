#ifndef _INCLUDED_NB_SELECT_CAMPAIGN_ENTRY_H
#define _INCLUDED_NB_SELECT_CAMPAIGN_ENTRY_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/EditablePanel.h>

// == MANAGED_CLASS_DECLARATIONS_START: Do not edit by hand ==
class CBitmapButton;
class vgui::Label;
// == MANAGED_CLASS_DECLARATIONS_END ==

class CNB_Select_Level_Entry : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CNB_Select_Level_Entry, vgui::EditablePanel );
public:
	CNB_Select_Level_Entry( vgui::Panel *parent, const char *name, const char *levelname );
	virtual ~CNB_Select_Level_Entry();
	
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void OnThink();
	virtual void OnCommand( const char *command );
	void		SetMap( const char *levelname );

	// == MANAGED_MEMBER_POINTERS_START: Do not edit by hand ==
	CBitmapButton	*m_pImage;
	vgui::Label	*m_pName;
	// == MANAGED_MEMBER_POINTERS_END ==

	char m_szLevelName[256];
};

#endif // _INCLUDED_NB_SELECT_CAMPAIGN_ENTRY_H


