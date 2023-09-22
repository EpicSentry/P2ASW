#ifndef _INCLUDED_CLIENTMODE_SDK_H
#define _INCLUDED_CLIENTMODE_SDK_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>
#include "GameUI/igameui.h"

class CHudViewport;

class ClientModeSDK : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeSDK, ClientModeShared );

	virtual void	Init();
	virtual void	InitViewport();
	virtual void	Shutdown();

	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	FireGameEvent( IGameEvent *event );
	virtual void	DoPostScreenSpaceEffects( const CViewSetup *pSetup );
	virtual void	SDK_CloseAllWindows();
	virtual void	SDK_CloseAllWindowsFrom(vgui::Panel* pPanel);	
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

extern ClientModeSDK* GetClientModeSDK();

#endif // _INCLUDED_CLIENTMODE_SDK_H
