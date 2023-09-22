//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: No need it in ASW [str]
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_HLNORMAL_H )
#define CLIENTMODE_HLNORMAL_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>
#include "GameUI/igameui.h"

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class ClientModeHLNormal : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeHLNormal, ClientModeShared );

	virtual void	Init();
	virtual bool	ShouldDrawCrosshair( void );
	virtual void	InitWeaponSelectionHudElement() { return; }
	virtual void	InitViewport();
	virtual void	Shutdown();

	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	FireGameEvent( IGameEvent *event );
	virtual void	DoPostScreenSpaceEffects( const CViewSetup *pSetup );
	virtual void SDK_CloseAllWindows();
	virtual void SDK_CloseAllWindowsFrom(vgui::Panel* pPanel);	
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

extern ClientModeHLNormal* GetClientModeSDK();
#endif // CLIENTMODE_HLNORMAL_H
