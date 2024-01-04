
#include "cbase.h"
#include "c_vguiscreen.h"
#include "vgui_controls/Label.h"
#include "vgui_BitmapPanel.h"
#include <vgui/IVGUI.h>
#include <vgui/ISurface.h>
#include "c_neurotoxin_countdown.h"
#include "ienginevgui.h"
#include "fmtstr.h"
#include "vgui_controls/ImagePanel.h"
#include "c_indicator_panel.h"

using namespace vgui;

class CIndicatorScreen : public CVGuiScreenPanel
{
	DECLARE_CLASS( CIndicatorScreen, CVGuiScreenPanel );

public:
	
	CIndicatorScreen( vgui::Panel *parent, const char *panelName );
    ~CIndicatorScreen();
	
	virtual void ApplySchemeSettings( IScheme *pScheme );
	
	virtual bool Init( KeyValues* pKeyValues, VGuiScreenInitData_t* pInitData );
    void OnTick();
    void Paint();
	
private:
	
    void PaintCountdownTimer();
    void PaintIndicator();
	
    int m_nCheckTextureID;
    int m_nUncheckedTextureID;
    int m_nXTextureID;
    int m_nOTextureID;
    int m_nTimerBackgroundTextureID;
    int m_nTimerBackgroundActiveTextureID;
    int m_nTimerUpperSliceTextureID;
    int m_nTimerLowerSliceTextureID;
	
    bool m_bWasCounting;
    float m_flFadeUpTime;
    CHandle<C_VGuiScreen> m_hVGUIScreen;
    CHandle<C_LabIndicatorPanel> m_hScreenEntity;
};

DECLARE_VGUI_SCREEN_FACTORY( CIndicatorScreen, "indicator_panel" );

CIndicatorScreen::CIndicatorScreen( vgui::Panel *parent, const char *panelName )
	: BaseClass( parent, "CIndicatorScreen", vgui::scheme()->LoadSchemeFromFileEx( enginevgui->GetPanel( PANEL_CLIENTDLL ), "scripts/screens/indicator_panel.res", "IndicatorPanelScreen" ) ) 
{

	m_hVGUIScreen = NULL;
	m_hScreenEntity = NULL;
	m_bWasCounting = false;
	m_flFadeUpTime = 0.0;
	m_nTimerBackgroundTextureID = -1;
	m_nTimerBackgroundActiveTextureID = -1;
	m_nTimerUpperSliceTextureID = -1;
	m_nTimerLowerSliceTextureID = -1;
	m_nCheckTextureID = -1;
	m_nUncheckedTextureID = -1;
	m_nXTextureID = -1;
	m_nOTextureID = -1;
}

CIndicatorScreen::~CIndicatorScreen()
{
	if ( vgui::surface() )
	{
		if (m_nTimerBackgroundTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nTimerBackgroundTextureID );
		}
		if (m_nTimerBackgroundActiveTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nTimerBackgroundActiveTextureID );
		}
		if (m_nTimerUpperSliceTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nTimerUpperSliceTextureID );
		}
		if (m_nTimerLowerSliceTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nTimerLowerSliceTextureID );
		}
		if (m_nCheckTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nCheckTextureID );
		}
		if (m_nUncheckedTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nUncheckedTextureID );
		}
		if (m_nXTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nXTextureID );
		}
		if (m_nOTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nOTextureID );
		}
	}

}

bool CIndicatorScreen::Init( KeyValues *pKeyValues, VGuiScreenInitData_t *pInitData )
{
	// Make sure we get ticked...
	vgui::ivgui()->AddTickSignal( GetVPanel() );
	
	if ( !BaseClass::Init( pKeyValues, pInitData ) )
		return false;
	
	CBaseEntity *pEntity = GetEntity();

	if (pEntity)
	{
		m_hVGUIScreen = assert_cast<C_VGuiScreen *>(pEntity);

		Assert( m_hVGUIScreen.IsValid() );

		if ( m_hVGUIScreen )
		{		
			m_hScreenEntity = assert_cast<C_LabIndicatorPanel*>( m_hVGUIScreen.Get()->GetOwnerEntity() );
			Assert( m_hScreenEntity.IsValid() );
		}
	}

	if (m_nTimerBackgroundTextureID == -1)
	{
		m_nTimerBackgroundTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nTimerBackgroundTextureID, "vgui/signage/vgui_countdown_background", 1, false );
	}
	if (m_nTimerBackgroundActiveTextureID == -1)
	{
		m_nTimerBackgroundActiveTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nTimerBackgroundActiveTextureID, "vgui/signage/vgui_countdown_background_on", 1, false );
	}
	if (m_nTimerUpperSliceTextureID == -1)
	{
		m_nTimerUpperSliceTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nTimerUpperSliceTextureID, "vgui/signage/vgui_countdown_slice1", 1, false );
	}
	if (m_nTimerLowerSliceTextureID == -1)
	{
		m_nTimerLowerSliceTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nTimerLowerSliceTextureID, "vgui/signage/vgui_countdown_slice2", 1, false );
	}
	if ( m_nCheckTextureID == -1)
	{
		m_nCheckTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nCheckTextureID, "vgui/signage/vgui_indicator_checked", 1, false );
	}
	if (m_nUncheckedTextureID == -1)
	{
		m_nUncheckedTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nUncheckedTextureID, "vgui/signage/vgui_indicator_unchecked", 1, false );
	}
	if (m_nXTextureID == -1)
	{
		m_nXTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nXTextureID, "vgui/signage/vgui_shape04", 1, false );
	}
	if (m_nOTextureID == -1)
	{
		m_nOTextureID = vgui::surface()->CreateNewTextureID( false );
		vgui::surface()->DrawSetTextureFile( m_nOTextureID, "vgui/signage/vgui_shape05", 1, false );
	}

	return true;
}

void CIndicatorScreen::OnTick( void )
{
	//BaseClass::OnTick();

	// Might be better to use baseclass instead, but this is what IDA gave us
	vgui::Panel::OnTick();
}

void CIndicatorScreen::ApplySchemeSettings( vgui::IScheme *HScheme )
{
	// fixme?
}

void CIndicatorScreen::Paint( void )
{
	vgui::Panel::Paint();

	if ( m_hScreenEntity )
	{
		if ( m_hScreenEntity->IsCountingDown() )
			PaintCountdownTimer();
		else
			PaintIndicator();
	}
}

void CIndicatorScreen::PaintIndicator( void )
{
	if (m_hScreenEntity)
	{
		int nCurTextureID = m_nOTextureID;
		
		if ( m_hScreenEntity->IsChecked() )
			nCurTextureID = m_nCheckTextureID;
		else
			nCurTextureID = m_nUncheckedTextureID;

		vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui::surface()->DrawSetTexture( nCurTextureID );		
		vgui::surface()->DrawTexturedRect( 0, 0, GetWide(), GetTall() );
	}
	else
	{
		vgui::surface()->DrawFilledRect( 0, 0, GetWide(), GetTall() );
	}
}

// This code is absolutely awful and needs to be fixed
void CIndicatorScreen::PaintCountdownTimer( void )
{	
	float flPerc = m_hScreenEntity->GetCountdownPercentage() * 8.0;
	
	float v5 = ceil(flPerc) - flPerc;

	if (v5 >= 0.0)
	{
		if (v5 > 1.0)
			v5 = 1.0;
	}
	else
	{
		v5 = 0.0;
	}

	float flAlpha = 255.0 - (v5 * 255.0);

	int iHalfWide = GetWide() / 2;
	int iHalfTall = GetTall() / 2;
	
	vgui::surface()->DrawSetColor( 255, 255, 255, 255 );
	
	if ( flPerc > 0.00000011920929 )
		vgui::surface()->DrawSetTexture( m_nTimerBackgroundActiveTextureID );
	else
		vgui::surface()->DrawSetTexture( m_nTimerBackgroundTextureID );

	vgui::surface()->DrawTexturedRect( 0, 0, GetWide(), GetTall() );

	float flGlobalAlpha = 255.0;
	float flNewAlpha;
	if (flPerc <= 0.00000011920929)
	{
		flPerc = 8.0;
		if (m_bWasCounting)
		{
			flNewAlpha = 0.0;
			m_flFadeUpTime = gpGlobals->curtime;
			flGlobalAlpha = 0.0;
			m_bWasCounting = false;
		}
		else
		{
			float v15 = m_flFadeUpTime + 0.25;
			if (m_flFadeUpTime == (m_flFadeUpTime + 0.25))
			{
				if ((gpGlobals->curtime - v15) < 0.0)
				{
					flNewAlpha = 0.0;
					flGlobalAlpha = 0.0;
				}
				else
				{
					flNewAlpha = 255.0;
					flGlobalAlpha = 255.0;
				}
			}
			else
			{
				float flFade = (gpGlobals->curtime - m_flFadeUpTime) / (v15 - m_flFadeUpTime);
				float v17 = 0;
				if (flFade < 0.0 || (v17 = 1.0, flFade > 1.0))
					flFade = v17;

				flNewAlpha = flFade * 255.0;
				flGlobalAlpha = flNewAlpha;
			}
		}
		goto LABEL_30;
	}

	int v57;
	m_bWasCounting = true;
	if (flPerc > 7.0)
	{
		flNewAlpha = flAlpha;
	LABEL_30:
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		vgui::surface()->DrawTexturedRect( GetWide() / 2, 0, iHalfWide + GetWide() / 2, GetTall() / 2 );
	}

	if (flPerc > 6.0 && 7.0 > flPerc)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		v57 = GetTall() / 2;

		vgui::surface()->DrawTexturedRect( GetWide() / 2, 0, iHalfWide + GetWide() - ( GetWide() / 2 ) , 0 );
	}
	else if (flPerc > 7.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		v57 = GetTall() / 2;
		
		vgui::surface()->DrawTexturedRect( GetWide() / 2, 0, iHalfWide + GetWide() - ( GetWide() / 2 ) , 0 );
	}

	if (flPerc > 5.0 && flPerc < 6.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		flGlobalAlpha = 0.0;
		flAlpha = 1.0;

		v57 = iHalfTall;

		vgui::surface()->DrawTexturedSubRect( 0, GetWide() / 2, 0, 0, 0, 0, 0, 0 );
	}
	else if (flPerc > 6.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		flGlobalAlpha = 0.0;
		flAlpha = 1.0;

		v57 = iHalfTall;

		vgui::surface()->DrawTexturedSubRect( 0, GetWide() / 2, 0, 0, 0, 0, 0, 0 );
	}

	if (flPerc > 4.0 && flPerc < 5.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		flGlobalAlpha = 0.0;
		flAlpha = 1.0;

		v57 = iHalfTall;

		vgui::surface()->DrawTexturedSubRect( 0, GetWide() / 2, 0, 0, 0, 0, 0, 0 );
	}
	else if (flPerc > 5.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );

		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		flGlobalAlpha = 0.0;
		flAlpha = 1.0;

		v57 = iHalfTall;
		
		vgui::surface()->DrawTexturedSubRect( 0, GetWide() / 2, 0, 0, 0, 0, 0, 0 );
	}

	if (flPerc > 3.0 && flPerc < 4.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		v57 = iHalfTall + GetTall() / 2;

		vgui::surface()->DrawTexturedSubRect( 0, iHalfTall, GetWide() / 2, 0, 0, 0, 0, 0 );
	}
	else if (flPerc > 4.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		v57 = iHalfTall + GetTall() / 2;
		
		vgui::surface()->DrawTexturedSubRect( 0, iHalfTall, GetWide() / 2, 0, 0, 0, 0, 0 );
	}

	if (flPerc > 2.0 && flPerc < 3.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		v57 = iHalfTall + GetTall() / 2;

		vgui::surface()->DrawTexturedSubRect( 0, iHalfTall, GetWide() / 2, 0, 0, 0, 0, 0 );
	}
	else if (flPerc > 3.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		
		v57 = iHalfTall + GetTall() / 2;
		
		vgui::surface()->DrawTexturedSubRect( 0, iHalfTall, GetWide() / 2, 0, 0, 0, 0, 0 );
	}

	if (flPerc > 1.0 && flPerc < 2.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		v57 = 1.0;

		vgui::surface()->DrawTexturedSubRect( 0, 0, GetWide() / 2, GetTall() - ( GetTall() / 2 ), 0, 0, 0, 0 );
	}
	else if (flPerc > 2.0)
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerLowerSliceTextureID );
		v57 = 1.0;
		
		vgui::surface()->DrawTexturedSubRect( 0, 0, GetWide() / 2, GetTall() - ( GetTall() / 2 ), 0, 0, 0, 0 );
	}

	int v52;

	if (flPerc <= 0.0 || flPerc >= 1.0)
	{
		if (flPerc <= 1.0)
			return;

		vgui::surface()->DrawSetColor( 255, 255, 255, flGlobalAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		
		v52 = GetTall() - (GetTall() >> 31);
	}
	else
	{
		vgui::surface()->DrawSetColor( 255, 255, 255, flAlpha );
		vgui::surface()->DrawSetTexture( m_nTimerUpperSliceTextureID );
		
		v52 = GetTall() - (GetTall() >> 31);
	}

	vgui::surface()->DrawTexturedSubRect( 0, 0, GetWide() / 2, v52 >> 1, 0, 0, 0, 0 );
}