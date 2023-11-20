#ifndef _DEFINED_ASW_VGUI_INGAME_PANEL_H
#define _DEFINED_ASW_VGUI_INGAME_PANEL_H

#include <vgui_controls/Frame.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/Slider.h>
#include <vgui/IScheme.h>

// base class for all panels that should be part of the ingame HUD while you're playing,
//  but still be interactive without showing the hardware mouse cursor or stopping
//  input from reaching the game code

class CPortal2_VGUI_Ingame_Panel
{
public:
	DECLARE_CLASS_SIMPLE_NOBASE( CPortal2_VGUI_Ingame_Panel );

	CPortal2_VGUI_Ingame_Panel();
	virtual ~CPortal2_VGUI_Ingame_Panel();

	virtual bool MouseClick(int x, int y, bool bRightClick, bool bDown);
	void SetMouseIsOver(int x, int y, bool b) { m_bMouseIsOver = b; m_iMouseX = x; m_iMouseY = y; }

protected:
	bool m_bMouseIsOver;
	int m_iMouseX, m_iMouseY;
};

// manager keeps track of the ingame_panels and their order
// tracks which panel the mouse is over and sends mouse clicks to the appropriate panel

class CPortal2_VGUI_Ingame_Panel_Manager
{
public:
	void AddPanel(CPortal2_VGUI_Ingame_Panel *pPanel);
	void RemovePanel(CPortal2_VGUI_Ingame_Panel *pPanel);
	void MovePanelToTop(CPortal2_VGUI_Ingame_Panel *pPanel);
	void UpdateMouseOvers();
	bool SendMouseClick(bool bRightClick, bool bDown);
	bool IngamePanelVisible(vgui::Panel* pPanel);

private:
	CUtlVector<CPortal2_VGUI_Ingame_Panel*>	m_IngamePanelList;
};

extern CPortal2_VGUI_Ingame_Panel_Manager g_IngamePanelManager;

#endif /* _DEFINED_ASW_VGUI_INGAME_PANEL_H */