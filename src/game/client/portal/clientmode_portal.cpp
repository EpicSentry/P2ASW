//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws the normal TF2 or HL2 HUD.
//
//=============================================================================
#include "cbase.h"
#include "clientmode_portal.h"
#include "vgui_int.h"
#include "hud.h"
#include <vgui/IInput.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui_controls/AnimationController.h>
#include "iinput.h"
#include "ienginevgui.h"
#include "hud_macros.h"
#include "view_shared.h"
#include "hud_basechat.h"
#include "achievementmgr.h"
#include "fmtstr.h"
#include "c_portal_player.h"
#include "soundenvelope.h"
#include "ivmodemanager.h"
#include "voice_status.h"
#include "PanelMetaClassMgr.h"
#include "vgui/sdk_loading_panel.h"
#include "vgui/sdk_logo_panel.h"
#include "c_user_message_register.h"
#include "inetchannelinfo.h"
#include "engine/IVDebugOverlay.h"
#include "debugoverlay_shared.h"
#include "viewpostprocess.h"
#include "shaderapi/ishaderapi.h"
#include "tier2/renderutils.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "nb_header_footer.h"
#include "glow_outline_effect.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar fov_desired("fov_desired", "75", FCVAR_ARCHIVE | FCVAR_USERINFO, "Sets the base field-of-view.", true, 75.0, true, 90.0);
ConVar default_fov("default_fov", "90", FCVAR_CHEAT);

extern bool IsInCommentaryMode(void);

extern ConVar mat_object_motion_blur_enable;

DECLARE_HUDELEMENT(CBaseHudChat);

vgui::HScheme g_hVGuiCombineScheme = 0;

vgui::DHANDLE<CSDK_Logo_Panel> g_hLogoPanel;

static IClientMode *g_pClientMode[MAX_SPLITSCREEN_PLAYERS];
IClientMode *GetClientMode()
{
	Assert(engine->IsLocalPlayerResolvable());
	return g_pClientMode[engine->GetActiveSplitScreenPlayerSlot()];
}

// Voice data
void VoxCallback(IConVar *var, const char *oldString, float oldFloat)
{
	if (engine && engine->IsConnected())
	{
		ConVarRef voice_vox(var->GetName());
		if (voice_vox.GetBool() /*&& voice_modenable.GetBool()*/)
		{
			engine->ClientCmd("voicerecord_toggle on\n");
		}
		else
		{
			engine->ClientCmd("voicerecord_toggle off\n");
		}
	}
}
ConVar voice_vox("voice_vox", "0", FCVAR_ARCHIVE, "Voice chat uses a vox-style always on", false, 0, true, 1, VoxCallback);

//--------------------------------------------------------------------------------------------------------
class CVoxManager : public CAutoGameSystem
{
public:
	CVoxManager() : CAutoGameSystem("VoxManager") { }

	virtual void LevelInitPostEntity(void)
	{
		if (voice_vox.GetBool() /*&& voice_modenable.GetBool()*/)
		{
			engine->ClientCmd("voicerecord_toggle on\n");
		}
	}


	virtual void LevelShutdownPreEntity(void)
	{
		if (voice_vox.GetBool())
		{
			engine->ClientCmd("voicerecord_toggle off\n");
		}
	}
};


//--------------------------------------------------------------------------------------------------------
static CVoxManager s_VoxManager;

// --------------------------------------------------------------------------------- //
// CSDKModeManager.
// --------------------------------------------------------------------------------- //

class CSDKModeManager : public IVModeManager
{
public:
	virtual void	Init();
	virtual void	SwitchMode(bool commander, bool force) {}
	virtual void	LevelInit(const char *newmap);
	virtual void	LevelShutdown(void);
	virtual void	ActivateMouse(bool isactive) {}
};

static CSDKModeManager g_ModeManager;
IVModeManager *modemanager = (IVModeManager *)&g_ModeManager;


// --------------------------------------------------------------------------------- //
// CASWModeManager implementation.
// --------------------------------------------------------------------------------- //

#define SCREEN_FILE		"scripts/vgui_screens.txt"

void CSDKModeManager::Init()
{
	for (int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i)
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD(i);
		g_pClientMode[i] = GetClientModeNormal();
	}

	PanelMetaClassMgr()->LoadMetaClassDefinitionFile(SCREEN_FILE);

	GetClientVoiceMgr()->SetHeadLabelOffset(40);
}

void CSDKModeManager::LevelInit(const char *newmap)
{
	for (int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i)
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD(i);
		GetClientMode()->LevelInit(newmap);
	}
}

void CSDKModeManager::LevelShutdown(void)
{
	for (int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i)
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD(i);
		GetClientMode()->LevelShutdown();
	}
}

ClientModePortal g_ClientModeNormal[MAX_SPLITSCREEN_PLAYERS];
IClientMode *GetClientModeNormal()
{
	Assert(engine->IsLocalPlayerResolvable());
	return &g_ClientModeNormal[engine->GetActiveSplitScreenPlayerSlot()];
}

ClientModePortal* GetClientModePortal()
{
	Assert(engine->IsLocalPlayerResolvable());
	return &g_ClientModeNormal[engine->GetActiveSplitScreenPlayerSlot()];
}


/******************
Accessors
******************/
/*
static IClientMode* g_pClientMode[ MAX_SPLITSCREEN_PLAYERS ]; // Pointer to the active mode
IClientMode* GetClientMode()
{
Assert( engine->IsLocalPlayerResolvable() );
return g_pClientMode[ engine->GetActiveSplitScreenPlayerSlot() ];
}

ClientModePortal g_ClientModeNormal[ MAX_SPLITSCREEN_PLAYERS ]; // The default mode
IClientMode* GetClientModeNormal()
{
Assert( engine->IsLocalPlayerResolvable() );
return &g_ClientModeNormal[ engine->GetActiveSplitScreenPlayerSlot() ];
}
*/

//-----------------------------------------------------------------------------
// Purpose: this is the viewport that contains all the hud elements
//-----------------------------------------------------------------------------
class CHudViewport : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE(CHudViewport, CBaseViewport);

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);

		GetHud().InitColors(pScheme);

		SetPaintBackgroundEnabled(false);
	}

	virtual void CreateDefaultPanels(void) { /* don't create any panels yet*/ };
};
//--------------------------------------------------------------------------------------------------------

// See interface.h/.cpp for specifics:  basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule 
//  over and over again.
static CDllDemandLoader g_GameUI("gameui");

class FullscreenSDKViewport : public CHudViewport
{
private:
	DECLARE_CLASS_SIMPLE(FullscreenSDKViewport, CHudViewport);

private:
	virtual void InitViewportSingletons(void)
	{
		SetAsFullscreenViewportInterface();
	}
};

class ClientModePortalFullscreen : public	ClientModePortal
{
	DECLARE_CLASS_SIMPLE(ClientModePortalFullscreen, ClientModePortal);
public:
	virtual void InitViewport()
	{
		// Skip over BaseClass!!!
		BaseClass::BaseClass::InitViewport();
		m_pViewport = new FullscreenSDKViewport();
		m_pViewport->Start(gameuifuncs, gameeventmanager);
		m_pViewport->SetPaintBackgroundEnabled(false);
		m_pViewport->SetName("SDK viewport (fullscreen)");
	}
	virtual void Init()
	{
		CreateInterfaceFn gameUIFactory = g_GameUI.GetFactory();
		if (gameUIFactory)
		{
			IGameUI *pGameUI = (IGameUI *)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
			if (NULL != pGameUI)
			{
				// insert stats summary panel as the loading background dialog
				CSDK_Loading_Panel *pPanel = GSDKLoadingPanel();
				pPanel->InvalidateLayout(false, true);
				pPanel->SetVisible(false);
				pPanel->MakePopup(false);
				pGameUI->SetLoadingBackgroundDialog(pPanel->GetVPanel());

				// add ASI logo to main menu
				CSDK_Logo_Panel *pLogo = new CSDK_Logo_Panel(NULL, "ASILogo");
				vgui::VPANEL GameUIRoot = enginevgui->GetPanel(PANEL_GAMEUIDLL);
				pLogo->SetParent(GameUIRoot);
				g_hLogoPanel = pLogo;
			}
		}

		// 
		//CASW_VGUI_Debug_Panel *pDebugPanel = new CASW_VGUI_Debug_Panel( GetViewport(), "ASW Debug Panel" );
		//g_hDebugPanel = pDebugPanel;

		// Skip over BaseClass!!!
		BaseClass::BaseClass::Init();

		// Load up the combine control panel scheme
		if (!g_hVGuiCombineScheme)
		{
			g_hVGuiCombineScheme = vgui::scheme()->LoadSchemeFromFileEx(enginevgui->GetPanel(PANEL_CLIENTDLL), IsXbox() ? "resource/ClientScheme.res" : "resource/CombinePanelScheme.res", "CombineScheme");
			if (!g_hVGuiCombineScheme)
			{
				Warning("Couldn't load combine panel scheme!\n");
			}
		}
	}
	void Shutdown()
	{
		DestroySDKLoadingPanel();
		if (g_hLogoPanel.Get())
		{
			delete g_hLogoPanel.Get();
		}
	}
};

//--------------------------------------------------------------------------------------------------------
static ClientModePortalFullscreen g_FullscreenClientMode;
IClientMode *GetFullscreenClientMode(void)
{
	return &g_FullscreenClientMode;
}


ClientModePortal::ClientModePortal()
{
	m_pGameUI = NULL;

	m_pCurrentPostProcessController = NULL;
	m_PostProcessLerpTimer.Invalidate();
	m_pCurrentColorCorrection = NULL;
}

ClientModePortal::~ClientModePortal()
{
}

void ClientModePortal::Init()
{
	BaseClass::Init();
	gameeventmanager->AddListener(this, "game_newmap", false);
}

void ClientModePortal::Shutdown()
{
	DestroySDKLoadingPanel();
	if (g_hLogoPanel.Get())
	{
		delete g_hLogoPanel.Get();
	}
}

void ClientModePortal::InitViewport()
{
	m_pViewport = new CHudViewport();
	m_pViewport->Start(gameuifuncs, gameeventmanager);
}

void ClientModePortal::LevelInit(const char *newmap)
{
	// reset ambient light
	static ConVarRef mat_ambient_light_r("mat_ambient_light_r");
	static ConVarRef mat_ambient_light_g("mat_ambient_light_g");
	static ConVarRef mat_ambient_light_b("mat_ambient_light_b");

	if (mat_ambient_light_r.IsValid())
	{
		mat_ambient_light_r.SetValue("0");
	}

	if (mat_ambient_light_g.IsValid())
	{
		mat_ambient_light_g.SetValue("0");
	}

	if (mat_ambient_light_b.IsValid())
	{
		mat_ambient_light_b.SetValue("0");
	}

	BaseClass::LevelInit(newmap);

	// asw: make sure no windows are left open from before
	SDK_CloseAllWindows();

	// update keybinds shown on the HUD
	//engine->ClientCmd("asw_update_binds");

	// clear any DSP effects
	CLocalPlayerFilter filter;
	enginesound->SetRoomType(filter, 0);
	enginesound->SetPlayerDSP(filter, 0, true);
}

void ClientModePortal::LevelShutdown(void)
{
	BaseClass::LevelShutdown();
}

void ClientModePortal::FireGameEvent(IGameEvent *event)
{
	const char *eventname = event->GetName();

	if (Q_strcmp("game_newmap", eventname) == 0)
	{
		const char* mapname = engine->GetLevelName();
		char firstTwo[8];
		memcpy((void*)firstTwo, &mapname[0], 7);
		firstTwo[7] = '\0';

		if (strcmp(firstTwo, "maps/sp") == 0)
		{
			engine->ClientCmd("r_skybox 0");
		}

		engine->ClientCmd("exec newmapsettings\n");
	}

	BaseClass::FireGameEvent(event);
}

// Close all ASW specific VGUI windows that the player might have open
void ClientModePortal::SDK_CloseAllWindows()
{
	SDK_CloseAllWindowsFrom(GetViewport());
}

// these vgui panels will be closed at various times (e.g. when the level ends/starts)
static char const *s_CloseWindowNames[] = {
	"g_BriefingFrame",
	"g_PlayerListFrame",
	"m_CampaignFrame",
	"ComputerFrame",
	"ComputerContainer",
	"m_MissionCompleteFrame",
	"Credits",
	"CainMail",
	"TechFail",
	"QueenHealthPanel",
	"ForceReady",
	"ReviveDialog",
	"SkillToSpendDialog",
	"InfoMessageWindow",
	"SkipIntro",
	"InGameBriefingContainer",
	"HoldoutWaveAnnouncePanel",
	"HoldoutWaveEndPanel",
	"ResupplyFrame",
};

// recursive search for matching window names
void ClientModePortal::SDK_CloseAllWindowsFrom(vgui::Panel* pPanel)
{
	if (!pPanel)
		return;

	int num_names = NELEMS(s_CloseWindowNames);

	for (int k = 0; k<pPanel->GetChildCount(); k++)
	{
		Panel *pChild = pPanel->GetChild(k);
		if (pChild)
		{
			SDK_CloseAllWindowsFrom(pChild);
		}
	}

	// When VGUI is shutting down (i.e. if the player closes the window), GetName() can return NULL
	const char *pPanelName = pPanel->GetName();
	if (pPanelName != NULL)
	{
		for (int i = 0; i<num_names; i++)
		{
			if (!strcmp(pPanelName, s_CloseWindowNames[i]))
			{
				pPanel->SetVisible(false);
				pPanel->MarkForDeletion();
			}
		}
	}
}

void ClientModePortal::Update(void)
{
	UpdatePostProcessingEffects();
	engine->SetMouseWindowLock( /*ASWGameRules() && ASWGameRules()->GetGameState() == ASW_GS_INGAME &&*/ !enginevgui->IsGameUIVisible());
}

void ClientModePortal::DoPostScreenSpaceEffects(const CViewSetup *pSetup)
{
	CMatRenderContextPtr pRenderContext(materials);

	bool g_bRenderingGlows;

	if (mat_object_motion_blur_enable.GetBool())
	{
		DoObjectMotionBlur(pSetup);
	}

	// Render object glows and selectively-bloomed objects (under sniper scope)
	g_bRenderingGlows = true;
	g_GlowObjectManager.RenderGlowEffects(pSetup, GetSplitScreenPlayerSlot());
	g_bRenderingGlows = false;

}

void ClientModePortal::OnColorCorrectionWeightsReset(void)
{
	C_ColorCorrection *pNewColorCorrection = NULL;
	C_ColorCorrection *pOldColorCorrection = m_pCurrentColorCorrection;
	C_Portal_Player* pPlayer = C_Portal_Player::GetLocalPlayer();
	if (pPlayer)
	{
		pNewColorCorrection = pPlayer->GetActiveColorCorrection();
	}

	// Only blend between environmental color corrections if there is no failure/infested-induced color correction
	if (pNewColorCorrection != pOldColorCorrection)
	{
		if (pOldColorCorrection)
		{
			pOldColorCorrection->EnableOnClient(false);
		}
		if (pNewColorCorrection)
		{
			pNewColorCorrection->EnableOnClient(true, pOldColorCorrection == NULL);
		}
		m_pCurrentColorCorrection = pNewColorCorrection;
	}
}

#include "object_motion_blur_effect.h"
void ClientModePortal::DoObjectMotionBlur(const CViewSetup *pSetup)
{
	if (g_ObjectMotionBlurManager.GetDrawableObjectCount() <= 0)
		return;

	CMatRenderContextPtr pRenderContext(materials);

	ITexture *pFullFrameFB1 = materials->FindTexture("_rt_FullFrameFB1", TEXTURE_GROUP_RENDER_TARGET);

	//
	// Render Velocities into a full-frame FB1
	//
	IMaterial *pGlowColorMaterial = materials->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER, true);

	pRenderContext->PushRenderTargetAndViewport();
	pRenderContext->SetRenderTarget(pFullFrameFB1);
	pRenderContext->Viewport(0, 0, pSetup->width, pSetup->height);

	// Red and Green are x- and y- screen-space velocities biased and packed into the [0,1] range.
	// A value of 127 gets mapped to 0, a value of 0 gets mapped to -1, and a value of 255 gets mapped to 1.
	//
	// Blue is set to 1 within the object's bounds and 0 outside, and is used as a mask to ensure that
	// motion blur samples only pull from the core object itself and not surrounding pixels (even though
	// the area being blurred is larger than the core object).
	//
	// Alpha is not used
	pRenderContext->ClearColor4ub(127, 127, 0, 0);
	// Clear only color, not depth & stencil
	pRenderContext->ClearBuffers(true, false, false);

	// Save off state
	Vector vOrigColor;
	render->GetColorModulation(vOrigColor.Base());

	// Use a solid-color unlit material to render velocity into the buffer
	g_pStudioRender->ForcedMaterialOverride(pGlowColorMaterial);
	g_ObjectMotionBlurManager.DrawObjects();
	g_pStudioRender->ForcedMaterialOverride(NULL);

	render->SetColorModulation(vOrigColor.Base());

	pRenderContext->PopRenderTargetAndViewport();

	//
	// Render full-screen pass
	//
	IMaterial *pMotionBlurMaterial;
	IMaterialVar *pFBTextureVariable;
	IMaterialVar *pVelocityTextureVariable;
	bool bFound1 = false, bFound2 = false;

	// Make sure our render target of choice has the results of the engine post-process pass
	ITexture *pFullFrameFB = materials->FindTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
	pRenderContext->CopyRenderTargetToTexture(pFullFrameFB);

	pMotionBlurMaterial = materials->FindMaterial("effects/object_motion_blur", TEXTURE_GROUP_OTHER, true);
	pFBTextureVariable = pMotionBlurMaterial->FindVar("$fb_texture", &bFound1, true);
	pVelocityTextureVariable = pMotionBlurMaterial->FindVar("$velocity_texture", &bFound2, true);
	if (bFound1 && bFound2)
	{
		pFBTextureVariable->SetTextureValue(pFullFrameFB);

		pVelocityTextureVariable->SetTextureValue(pFullFrameFB1);

		int nWidth, nHeight;
		pRenderContext->GetRenderTargetDimensions(nWidth, nHeight);

		pRenderContext->DrawScreenSpaceRectangle(pMotionBlurMaterial, 0, 0, nWidth, nHeight, 0.0f, 0.0f, nWidth - 1, nHeight - 1, nWidth, nHeight);
	}
}

void ClientModePortal::UpdatePostProcessingEffects()
{
	C_PostProcessController *pNewPostProcessController = NULL;
	C_Portal_Player* pPlayer = C_Portal_Player::GetLocalPlayer();
	if (pPlayer)
	{
		pNewPostProcessController = pPlayer->GetActivePostProcessController();
	}

	// Figure out new endpoints for parameter lerping
	if (pNewPostProcessController != m_pCurrentPostProcessController)
	{
		m_LerpStartPostProcessParameters = m_CurrentPostProcessParameters;
		m_LerpEndPostProcessParameters = pNewPostProcessController ? pNewPostProcessController->m_PostProcessParameters : PostProcessParameters_t();
		m_pCurrentPostProcessController = pNewPostProcessController;

		float flFadeTime = pNewPostProcessController ? pNewPostProcessController->m_PostProcessParameters.m_flParameters[PPPN_FADE_TIME] : 0.0f;
		if (flFadeTime <= 0.0f)
		{
			flFadeTime = 0.001f;
		}
		m_PostProcessLerpTimer.Start(flFadeTime);
	}

	// Lerp between start and end
	float flLerpFactor = 1.0f - m_PostProcessLerpTimer.GetRemainingRatio();
	for (int nParameter = 0; nParameter < POST_PROCESS_PARAMETER_COUNT; ++nParameter)
	{
		m_CurrentPostProcessParameters.m_flParameters[nParameter] =
			Lerp(
				flLerpFactor,
				m_LerpStartPostProcessParameters.m_flParameters[nParameter],
				m_LerpEndPostProcessParameters.m_flParameters[nParameter]);
	}
	SetPostProcessParams(&m_CurrentPostProcessParameters);
}

/******************
ModeManager
******************/
/*
class SkeletonModeManager : public IVModeManager
{
void Init()
{
for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
{
ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
g_pClientMode[ i ] = GetClientModeNormal();
}
}

void SwitchMode( bool commander, bool force ) {}

void LevelInit( const char* newmap )
{
for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
{
ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
GetClientMode()->LevelInit(newmap);
}
GetFullscreenClientMode()->LevelInit(newmap);
}
void LevelShutdown()
{
for( int i = 0; i < MAX_SPLITSCREEN_PLAYERS; ++i )
{
ACTIVE_SPLITSCREEN_PLAYER_GUARD( i );
GetClientMode()->LevelShutdown();
}
GetFullscreenClientMode()->LevelShutdown();
}
};

IVModeManager* modemanager = (IVModeManager*)new SkeletonModeManager;
*/