//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_SDK_H )
#define CLIENTMODE_SDK_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>
#include "GameUI/igameui.h"

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
	class Panel;
	class Frame;
}

// This is an implementation that does nearly nothing. It's only included to make Swarm Skeleton compile out of the box.
// You DEFINITELY want to replace it with your own class!
class ClientModePortal : public ClientModeShared
{
public:
	DECLARE_CLASS(ClientModePortal, ClientModeShared);

	ClientModePortal();
	~ClientModePortal();

	virtual void	Init();
	virtual void	InitWeaponSelectionHudElement() { return; }
	virtual void	InitViewport();
	virtual void	Shutdown();
	//virtual void	OverrideView( CViewSetup *pSetup );
	//virtual void	OverrideAudioState( AudioState_t *pAudioState );
	virtual bool	ShouldDrawCrosshair(void) { return true; }	// draw the HL2 crosshair

	virtual void	LevelInit(const char *newmap);
	virtual void	LevelShutdown(void);

	virtual void	Update(void);
	virtual void	FireGameEvent(IGameEvent *event);
	virtual void	DoPostScreenSpaceEffects(const CViewSetup *pSetup);
	virtual void	OnColorCorrectionWeightsReset(void);
	virtual float	GetColorCorrectionScale(void) const { return 1.0f; }
	virtual void	ClearCurrentColorCorrection() { m_pCurrentColorCorrection = NULL; }

	//virtual int		KeyInput( int down, ButtonCode_t keynum, const char *pszCurrentBinding );

	virtual void SDK_CloseAllWindows();
	virtual void SDK_CloseAllWindowsFrom(vgui::Panel* pPanel);

	// Gets at the viewport, if there is one...
	//vgui::Panel *GetViewport() { return m_pViewport; }

	// Gets at the viewports vgui panel animation controller, if there is one...
	//vgui::AnimationController *GetViewportAnimationController() { return m_pViewport->GetAnimationController(); }

	bool	CanRecordDemo(char *errorMsg, int length) const { return true; }

	//void SetBlurFade( float scale ) {}
	//float	GetBlurFade( void ) { return 0; }

private:
	IGameUI			*m_pGameUI;

	void DrawSniperScopeStencilMask();
	void DoObjectMotionBlur(const CViewSetup *pSetup);
	void UpdatePostProcessingEffects();

	const C_PostProcessController *m_pCurrentPostProcessController;
	PostProcessParameters_t m_CurrentPostProcessParameters;
	PostProcessParameters_t m_LerpStartPostProcessParameters, m_LerpEndPostProcessParameters;
	CountdownTimer m_PostProcessLerpTimer;

	CHandle<C_ColorCorrection> m_pCurrentColorCorrection;

	//bool m_bOfficialMap;
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

extern ClientModePortal* GetClientModePortal();

#endif // CLIENTMODE_SDK_H