
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
	if (vgui::surface())
	{
		if (m_nTimerBackgroundTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nTimerBackgroundTextureID );
		}
		if (m_nTimerBackgroundActiveTextureID != -1)
		{
			vgui::surface()->DestroyTextureID(m_nTimerBackgroundActiveTextureID);
		}
		if (m_nTimerUpperSliceTextureID != -1)
		{
			vgui::surface()->DestroyTextureID(m_nTimerUpperSliceTextureID);
		}
		if (m_nTimerLowerSliceTextureID != -1)
		{
			vgui::surface()->DestroyTextureID(m_nTimerLowerSliceTextureID);
		}
		if (m_nCheckTextureID != -1)
		{
			vgui::surface()->DestroyTextureID( m_nCheckTextureID );
		}
		if (m_nUncheckedTextureID != -1)
		{
			vgui::surface()->DestroyTextureID(m_nUncheckedTextureID);
		}
		if (m_nXTextureID != -1)
		{
			vgui::surface()->DestroyTextureID(m_nXTextureID);
		}
		if (m_nOTextureID != -1)
			vgui::surface()->DestroyTextureID( m_nOTextureID );
	}

}

bool CIndicatorScreen::Init( KeyValues *pKeyValues, VGuiScreenInitData_t *pInitData )
{
	// Make sure we get ticked...
	vgui::ivgui()->AddTickSignal(GetVPanel());
	
	if (!BaseClass::Init(pKeyValues, pInitData))
		return false;
	
	CBaseEntity *pEntity = GetEntity();

	Assert(pEntity);

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

	if (this->m_nTimerBackgroundTextureID == -1)
	{
		m_nTimerBackgroundTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nTimerBackgroundTextureID, "vgui/signage/vgui_countdown_background", 1, 0);
	}
	if (this->m_nTimerBackgroundActiveTextureID == -1)
	{
		m_nTimerBackgroundActiveTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nTimerBackgroundActiveTextureID, "vgui/signage/vgui_countdown_background_on", 1, 0);
	}
	if (this->m_nTimerUpperSliceTextureID == -1)
	{
		m_nTimerUpperSliceTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nTimerUpperSliceTextureID, "vgui/signage/vgui_countdown_slice1", 1, 0);
	}
	if (this->m_nTimerLowerSliceTextureID == -1)
	{
		m_nTimerLowerSliceTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nTimerLowerSliceTextureID, "vgui/signage/vgui_countdown_slice2", 1, 0);
	}
	if ( m_nCheckTextureID == -1)
	{
		m_nCheckTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nCheckTextureID, "vgui/signage/vgui_indicator_checked", 1, 0);
	}
	if (m_nUncheckedTextureID == -1)
	{
		m_nUncheckedTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nUncheckedTextureID, "vgui/signage/vgui_indicator_unchecked", 1, 0);
	}
	if (this->m_nXTextureID == -1)
	{
		m_nXTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nXTextureID, "vgui/signage/vgui_shape04", 1, 0);
	}
	if (this->m_nOTextureID == -1)
	{
		m_nOTextureID = vgui::surface()->CreateNewTextureID(false);
		vgui::surface()->DrawSetTextureFile(m_nOTextureID, "vgui/signage/vgui_shape05", 1, 0);
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
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		vgui::surface()->DrawSetTexture( m_nOTextureID );		
		vgui::surface()->DrawTexturedRect( 0, 0, GetWide(), GetTall() );
	}
	else
	{
		vgui::surface()->DrawFilledRect( 0, 0, GetWide(), GetTall() );
	}
}

void CIndicatorScreen::PaintCountdownTimer( void )
{
	// TODO: Implement
	// Note: When implementing this code, use accessors if necessary instead of the actual variables.
}