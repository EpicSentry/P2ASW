//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef __VMAINMENU_H__
#define __VMAINMENU_H__

#include "basemodui.h"
#include "VFlyoutMenu.h"
#include <vgui_controls/HTML.h>

namespace BaseModUI {
	class CP2ASWBlog;
class MainMenu : public CBaseModFrame, public IBaseModFrameListener, public FlyoutMenuListener
{
	DECLARE_CLASS_SIMPLE( MainMenu, CBaseModFrame );

public:
	MainMenu(vgui::Panel *parent, const char *panelName);
	~MainMenu();

#ifdef _X360
	void Activate();
#endif //_X360

	void UpdateVisibility();

	MESSAGE_FUNC_CHARPTR( OpenMainMenuJoinFailed, "OpenMainMenuJoinFailed", msg );
	
	//flyout menu listener
	virtual void OnNotifyChildFocus( vgui::Panel* child );
	virtual void OnFlyoutMenuClose( vgui::Panel* flyTo );
	virtual void OnFlyoutMenuCancelled();

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void OnThink();
	virtual void OnOpen();
	virtual void RunFrame();
	virtual void PaintBackground();
	virtual void OpenChapterSelect();

	void	Demo_DisableButtons( void );

private:
	static void AcceptCommentaryRulesCallback();
	static void AcceptSplitscreenDisableCallback();
	static void AcceptVersusSoftLockCallback();
	static void AcceptQuitGameCallback();
	void SetFooterState();
	void ShowBlogPanel(bool show);
	void PerformLayout();
	void DefaultLayout();
	CP2ASWBlog		*m_pBlogPanel;
	vgui::HTML			*m_pHTMLPanel;

	enum MainMenuQuickJoinHelpText
	{
		MMQJHT_NONE,
		MMQJHT_QUICKMATCH,
		MMQJHT_QUICKSTART,
	};
	
	int					m_iQuickJoinHelpText;
};
class CP2ASWBlog : public CBaseModFrame
{
	DECLARE_CLASS_SIMPLE(CP2ASWBlog, CBaseModFrame);

public:
	CP2ASWBlog(vgui::Panel* parent, const char *panelName);
	virtual ~CP2ASWBlog();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void LoadBlogPost(const char* URL);

private:
	vgui::HTML			*m_pHTMLPanel;
};
}

#endif // __VMAINMENU_H__
