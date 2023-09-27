//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef __VOPTIONS_H__
#define __VOPTIONS_H__

#include "basemodui.h"

namespace BaseModUI {

class BaseModHybridButton;

class Options : public CBaseModFrame
{
	DECLARE_CLASS_SIMPLE( Options, CBaseModFrame );

public:
	Options(vgui::Panel *parent, const char *panelName);
	~Options();

protected:
	virtual void OnCommand( const char *pCommand );
	virtual void Activate();
	
private:
	void UpdateFooter();

private:
	BaseModHybridButton* m_BtnGame;
	BaseModHybridButton* m_BtnAudioVideo;
	BaseModHybridButton* m_BtnController;
	BaseModHybridButton* m_BtnStorage;
	BaseModHybridButton* m_BtnCredits;
};

};

#endif // __VOPTIONS_H__