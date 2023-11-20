#ifndef SDK_LOGO_PANEL_H
#define SDK_LOGO_PANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/ImagePanel.h>

class ChatEchoPanel;

//-----------------------------------------------------------------------------
// Purpose: Panel that holds a single image - shows the backdrop image in the briefing
//-----------------------------------------------------------------------------
class CSDK_Logo_Panel : public vgui::ImagePanel
{
	DECLARE_CLASS_SIMPLE( CSDK_Logo_Panel, vgui::ImagePanel );
public:
	CSDK_Logo_Panel(vgui::Panel *parent, const char *name);
	virtual ~CSDK_Logo_Panel();

	virtual void PerformLayout();
};


#endif // SDK_LOGO_PANEL_H
