//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//
#include "cbase.h"
#include "VLoadingProgress.h"
#include "EngineInterface.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ProgressBar.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "vgui_controls/Image.h"
#include "vgui_controls/ImagePanel.h"
#include "gameui_util.h"
#include "KeyValues.h"
#include "fmtstr.h"
#include "FileSystem.h"

// UI defines. Include if you want to implement some of them [str]
#include "hl2/ui_defines.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;

//=============================================================================
LoadingProgress::LoadingProgress(Panel *parent, const char *panelName, LoadingWindowType eLoadingType ):
	BaseClass( parent, panelName, false, false, false ),
	m_LoadingWindowType( eLoadingType )
{
	if ( IsPC() ) // useless check C:
	{
		MakePopup( false );
	}

	SetDeleteSelfOnClose( true );
	SetProportional( true );


	m_flPeakProgress = 0.0f;
	m_pProTotalProgress = NULL;
	m_pWorkingAnim = NULL;
	m_LoadingType = LT_UNDEFINED;

	m_pBGImage = NULL;

	m_pFooter = NULL;

	// purposely not pre-caching the poster images
	// as they do not appear in-game, and are 1MB each, we will demand load them and ALWAYS discard them
	m_textureID_LoadingBar = -1;
	m_textureID_LoadingBarBG = -1;

	m_bDrawBackground = false;
	m_bDrawPoster = false;
	m_bDrawProgress = false;
	m_bDrawSpinner = false;

	m_flLastEngineTime = 0;

	// marked to indicate the controls exist
	m_bValid = false;

	LoadControlSettings("Resource/UI/BaseModUI/loadingprogress.res"); // not sure if needed, but let it be [str]

	MEM_ALLOC_CREDIT();
}

//=============================================================================
LoadingProgress::~LoadingProgress()
{
	if ( m_pBGImage )
	{
		m_pBGImage->EvictImage();
	}
}

//=============================================================================
void LoadingProgress::OnThink()
{
	UpdateWorkingAnim();
}

//=============================================================================
void LoadingProgress::OnCommand(const char *command)
{
}

//=============================================================================
void LoadingProgress::ApplySchemeSettings( IScheme *pScheme )
{
	// will cause the controls to be instanced
	BaseClass::ApplySchemeSettings( pScheme );

	SetPaintBackgroundEnabled( true );
	
	// now have controls, can now do further initing
	m_bValid = true;

	// find or create pattern
	// purposely not freeing these, not worth the i/o hitch for something so small
	const char *pImageName = "vgui/loadingbar";
	m_textureID_LoadingBar = vgui::surface()->DrawGetTextureId( pImageName );
	if ( m_textureID_LoadingBar == -1 )
	{
		m_textureID_LoadingBar = vgui::surface()->CreateNewTextureID();
		vgui::surface()->DrawSetTextureFile( m_textureID_LoadingBar, pImageName, true, false );	
	}

	// find or create pattern
	// purposely not freeing these, not worth the i/o hitch for something so small
	pImageName = "vgui/loadingbar_bg";
	m_textureID_LoadingBarBG = vgui::surface()->DrawGetTextureId( pImageName );
	if ( m_textureID_LoadingBarBG == -1 )
	{
		m_textureID_LoadingBarBG = vgui::surface()->CreateNewTextureID();
		vgui::surface()->DrawSetTextureFile( m_textureID_LoadingBarBG, pImageName, true, false );	
	}
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] LoadingProgress::ApplySchemeSettings | is valid? %i\n", m_bValid);
	}
	SetupControlStates();
}

void LoadingProgress::Close()
{
	// hint to force any of the movie posters out of memory (purposely ignoring specific map search)
	// if the image is unreferenced (as it should be), it will evict, if already evicted or empty, then harmless
	if ( m_pBGImage )
	{
		m_pBGImage->EvictImage();
	}
		
	BaseClass::Close();
}

//=============================================================================
// this is where the spinner gets updated.
void LoadingProgress::UpdateWorkingAnim()
{
	if ( m_pWorkingAnim && m_bDrawSpinner )
	{
		// clock the anim at 10hz
		float time = Plat_FloatTime();
		if ( ( m_flLastEngineTime + 0.1f ) < time )
		{
			m_flLastEngineTime = time;
			m_pWorkingAnim->SetFrame( m_pWorkingAnim->GetFrame() + 1 );
		}
	}
}

//=============================================================================
void LoadingProgress::SetProgress( float progress )
{
	if ( m_pProTotalProgress && m_bDrawProgress )
	{
		if ( progress > m_flPeakProgress )
		{
			m_flPeakProgress = progress;
		}
		m_pProTotalProgress->SetProgress( m_flPeakProgress );
	}

	UpdateWorkingAnim();
}

//=============================================================================
float LoadingProgress::GetProgress()
{
	float retVal = -1.0f;

	if ( m_pProTotalProgress )
	{
		retVal = m_pProTotalProgress->GetProgress();
	}

	return retVal;
}

void LoadingProgress::PaintBackground()
{
	int screenWide, screenTall;
	surface()->GetScreenSize( screenWide, screenTall );

	if ( m_bDrawBackground && m_pBGImage )
	{
		int x, y, wide, tall;
		m_pBGImage->GetBounds( x, y, wide, tall );
		surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		surface()->DrawSetTexture( m_pBGImage->GetImage()->GetID() );
		surface()->DrawTexturedRect( x, y, x+wide, y+tall );
	}

	if ( /*m_bDrawPoster &&*/ m_pFooter )
	{
		int screenWidth, screenHeight;
		CBaseModPanel::GetSingleton().GetSize( screenWidth, screenHeight );

		int x, y, wide, tall;
		m_pFooter->GetBounds( x, y, wide, tall );
		surface()->DrawSetColor( m_pFooter->GetBgColor() );
		surface()->DrawFilledRect( 0, y, x+screenWidth, y+tall );
	}

	// this is where the spinner draws
	bool bRenderSpinner = ( m_bDrawSpinner && m_pWorkingAnim );
	Panel *pWaitscreen = CBaseModPanel::GetSingleton().GetWindow( WT_GENERICWAITSCREEN );
	if ( pWaitscreen && pWaitscreen->IsVisible() && ( m_LoadingWindowType == LWT_BKGNDSCREEN ) )
		bRenderSpinner = false;	// Don't render spinner if the progress screen is displaying progress
	if ( bRenderSpinner  )
	{
		int x, y, wide, tall;

		wide = tall = scheme()->GetProportionalScaledValue( 45 );
		x = scheme()->GetProportionalScaledValue( 45 ) - wide/2;
		y = screenTall - scheme()->GetProportionalScaledValue( 32 ) - tall/2;

		m_pWorkingAnim->GetImage()->SetFrame( m_pWorkingAnim->GetFrame() );

		surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		surface()->DrawSetTexture( m_pWorkingAnim->GetImage()->GetID() );
		surface()->DrawTexturedRect( x, y, x+wide, y+tall );
	}

	if ( m_bDrawProgress && m_pProTotalProgress )
	{
		int x, y, wide, tall;
		m_pProTotalProgress->GetBounds( x, y, wide, tall );

		int iScreenWide, iScreenTall;
		surface()->GetScreenSize( iScreenWide, iScreenTall );

		float f = m_pProTotalProgress->GetProgress();
		f = clamp( f, 0, 1.0f );

		// Textured bar
		surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );

		// Texture BG
		surface()->DrawSetTexture( m_textureID_LoadingBarBG );
		surface()->DrawTexturedRect( x, y, x + wide, y + tall );

		surface()->DrawSetTexture( m_textureID_LoadingBar );

		// YWB 10/13/2009:  If we don't crop the texture coordinate down then we will see a jitter of the texture as the texture coordinate and the rounded width 
		//  alias

		int nIntegerWide = f * wide;		
		float flUsedFrac = (float)nIntegerWide / (float)wide;
		
		DrawTexturedRectParms_t params;
		params.x0 = x;
		params.y0 = y;
		params.x1 = x + nIntegerWide;
		params.y1 = y + tall;
		params.s0 = 0;
		params.s1 = flUsedFrac;
		surface()->DrawTexturedRectEx( &params );		
	}

	// Need to call this periodically to collect sign in and sign out notifications,
	// do NOT dispatch events here in the middle of loading and rendering!
	if ( ThreadInMainThread() )
	{
		XBX_ProcessEvents();
	}
}

//=============================================================================
// Must be called first. Establishes the loading style
//=============================================================================
void LoadingProgress::SetLoadingType( LoadingProgress::LoadingType loadingType )
{
	m_LoadingType = loadingType;
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] LoadingProgress::SetLoadingType(loadingType %i)...\n", loadingType);
		Msg( "[GAMEUI] LoadingProgress::SetLoadingType | is valid? %i\n", m_bValid);
	}
	// the first time initing occurs during ApplySchemeSettings() or if the panel is deleted
	// if the panel is re-used, this is for the second time the panel gets used
	//SetupControlStates();
}

//=============================================================================
LoadingProgress::LoadingType LoadingProgress::GetLoadingType()
{
	return m_LoadingType;
}

//=============================================================================
void LoadingProgress::SetupControlStates()
{
	m_flPeakProgress = 0.0f;

	if ( !m_bValid )
	{
		// haven't been functionally initialized yet
		// can't set or query control states until they get established
		return;
	}
	
	m_bDrawBackground = false;
	m_bDrawPoster = false;
	m_bDrawSpinner = false;
	m_bDrawProgress = false;

	switch( m_LoadingType )
	{
	case LT_TRANSITION: // transition between levels
#ifdef UI_USING_TRANSITIONBG
		m_bDrawBackground = true;
#endif
		m_bDrawProgress = true;
#ifdef UI_USING_LOADINGSPINNER
		m_bDrawSpinner = true;
#endif
		break;
	case LT_POSTER: // strating up new level
		m_bDrawBackground = true;
		m_bDrawProgress = true;
#ifdef UI_USING_LOADINGSPINNER
		m_bDrawSpinner = true;
#endif
		break;
	default:
		break;
	}

	m_pFooter = dynamic_cast< vgui::EditablePanel* >( FindChildByName( "CampaignFooter" ) );
	
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] LoadingProgress::SetupControlStates()...\n");
		Msg( "[GAMEUI] LoadingProgress::SetupControlStates | is valid? %i\n", m_bValid);
	}

	m_pBGImage = dynamic_cast< vgui::ImagePanel* >( FindChildByName( "BGImage" ) );
	if ( m_pBGImage )
	{
		int screenWide, screenTall;
		surface()->GetScreenSize( screenWide, screenTall );
		float aspectRatio = (float)screenWide/(float)screenTall;
		bool bIsWidescreen = aspectRatio >= 1.5999f;

		const char *pszPosterImage;
		pszPosterImage = ( bIsWidescreen ) ? UI_DEFAULT_LOADINGBACKGROUND_WIDE : UI_DEFAULT_LOADINGBACKGROUND; // reassuring that poster img exist [str]

#ifdef UI_USING_RANDOMLOADINGBGS
		int nChosenLoadingImage = random->RandomInt(0, ARRAYSIZE(g_ppszRandomLoadingBackgrounds));
		pszPosterImage = ( bIsWidescreen ) ? g_ppszRandomLoadingBackgrounds_widescreen[nChosenLoadingImage] : g_ppszRandomLoadingBackgrounds[nChosenLoadingImage];
#endif
		char filename[MAX_PATH];
		V_snprintf( filename, sizeof( filename ), pszPosterImage );

		m_pBGImage->SetImage( filename );

		// we will custom draw
		m_pBGImage->SetVisible( false );
	}
	
	m_pProTotalProgress = dynamic_cast< vgui::ProgressBar* >( FindChildByName( "ProTotalProgress" ) );
	if ( m_pProTotalProgress )
	{
		// we will custom draw
		m_pProTotalProgress->SetVisible( false );
	}

	m_pWorkingAnim = dynamic_cast< vgui::ImagePanel* >( FindChildByName( "WorkingAnim" ) );
	if ( m_pWorkingAnim )
	{
		// we will custom draw
		m_pWorkingAnim->SetVisible( false );
	}

	vgui::Label *pLoadingLabel = dynamic_cast< vgui::Label *>( FindChildByName( "LoadingText" ) );
	if ( pLoadingLabel )
	{
		pLoadingLabel->SetVisible( m_bDrawProgress );
	}

	// Hold on to start frame slightly
	m_flLastEngineTime = Plat_FloatTime() + 0.2f;
}