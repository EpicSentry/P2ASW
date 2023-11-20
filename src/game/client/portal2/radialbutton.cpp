
#include "cbase.h"
#include <string.h>
#include <stdio.h>
#include "voice_status.h"
#include "c_playerresource.h"
#include "cliententitylist.h"
#include "c_baseplayer.h"
#include "materialsystem/imesh.h"
#include "view.h"
#include "materialsystem/imaterial.h"
#include "tier0/dbg.h"
#include "cdll_int.h"
#include "menu.h" // for chudmenu defs
#include "keyvalues.h"
#include <filesystem.h>
#include "c_team.h"
#include "vgui/isurface.h"
#include "iclientmode.h"
#include "portal_gamerules.h"
//#include "asw_vgui_ingame_panel.h"
#include "input.h"
#include "c_portal_player.h"

#include "swarm/polygonbutton.h"

#include "radialmenu.h"
#include "radialbutton.h"
#include "vgui/cursor.h"
#include "fmtstr.h"
#include "vgui_int.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar RadialMenuDebug;

// todo??

CRadialButton::CRadialButton( vgui::Panel *parent, const char *panelName )
	: CPolygonButton( parent, panelName )
{
	SetCursor( vgui::dc_blank );

	m_nMainMaterial = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile( m_nMainMaterial, "vgui/white" , true, false );

	m_chosen = false;
	m_hasBorders = true;
	m_armedFont = NULL;
	m_defaultFont = NULL;

	m_unscaledSubmenuPoints.RemoveAll();
	m_submenuPoints = NULL;
	m_numSubmenuPoints = 0;
	m_hasSubmenu = false;
	m_fakeArmed = false;

	m_passthru = NULL;
	m_parent = dynamic_cast< CRadialMenu * >(parent);
}

void CRadialButton::ShowSubmenuIndicator( bool state )
{
	m_hasSubmenu = state;
}

void CRadialButton::SetPassthru( CRadialButton *button )
{
	m_passthru = button;
	if ( button )
	{
		SetZPos( -1 );
	}
	else
	{
		SetZPos( 0 );
	}
}

CRadialButton *CRadialButton::GetPassthru( void )
{
	return m_passthru;
}

void CRadialButton::UpdateHotspots( KeyValues *data )
{
	BaseClass::UpdateHotspots( data );

	// clear out our old submenu hotspot
	if ( m_submenuPoints )
	{
		delete[] m_submenuPoints;
		m_submenuPoints = NULL;
		m_numSubmenuPoints = 0;
	}
	m_unscaledSubmenuPoints.RemoveAll();

	// read in a new one
	KeyValues *points = data->FindKey( "SubmenuHotspot", false );
	if ( points )
	{
		for ( KeyValues *value = points->GetFirstValue(); value; value = value->GetNextValue() )
		{
			const char *str = value->GetString();

			float x, y;
			if ( 2 == sscanf( str, "%f %f", &x, &y ) )
			{
				m_unscaledSubmenuPoints.AddToTail( Vector2D( x, y ) );
			}
		}
	}

	if ( RadialMenuDebug.GetBool() )
	{
		InvalidateLayout( false, true );
	}
}
void CRadialButton::PerformLayout( void )
{
	int wide, tall;
	GetSize( wide, tall );

	if ( m_submenuPoints )
	{
		delete[] m_submenuPoints;
		m_submenuPoints = NULL;
		m_numSubmenuPoints = 0;
	}

	// generate scaled points
	m_numSubmenuPoints = m_unscaledSubmenuPoints.Count();
	if ( m_numSubmenuPoints )
	{
		m_submenuPoints = new vgui::Vertex_t[ m_numSubmenuPoints ];
		for ( int i=0; i<m_numSubmenuPoints; ++i )
		{
			float x = m_unscaledSubmenuPoints[i].x * wide;
			float y = m_unscaledSubmenuPoints[i].y * tall;
			m_submenuPoints[i].Init( Vector2D( x, y ), m_unscaledSubmenuPoints[i] );
		}
	}

	bool isArmed = ( IsArmed() || m_fakeArmed );
	vgui::HFont currentFont = (isArmed) ? m_armedFont : m_defaultFont;
	if ( currentFont )
	{
		SetFont( currentFont );
	}

	BaseClass::PerformLayout();
}

Color CRadialButton::GetRadialFgColor( void )
{
	Color c = BaseClass::GetButtonFgColor();
	if ( !IsEnabled() || GetPassthru() )
	{
		c = m_disabledFgColor;
	}
	else if ( m_chosen )
	{
		c = m_chosenFgColor;
	}
	else if ( m_fakeArmed )
	{
		c = m_armedFgColor;
	}
	c[3] = 64;
	return c;
}

Color CRadialButton::GetRadialBgColor( void )
{
	Color c = BaseClass::GetButtonBgColor();
	if ( GetPassthru() )
	{
		if ( IsArmed() || m_fakeArmed )
		{
			for ( int i=0; i<4; ++i )
			{
				c[i] = m_disabledBgColor[i] * 0.8f + m_armedBgColor[i] * 0.2f;
			}
		}
		else
		{
			c = m_disabledBgColor;
		}
	}
	else if ( !IsEnabled() )
	{
		c = m_disabledBgColor;
	}
	else if ( m_chosen )
	{
		c = m_chosenBgColor;
	}
	else if ( m_fakeArmed )
	{
		c = m_armedBgColor;
	}
	return c;
}

void CRadialButton::GetHotspotBounds( int *minX, int *minY, int *maxX, int *maxY )
{
	if ( minX )
	{
		*minX = (int) m_hotspotMins.x;
	}

	if ( minY )
	{
		*minY = (int) m_hotspotMins.y;
	}

	if ( maxX )
	{
		*maxX = (int) m_hotspotMaxs.x;
	}

	if ( maxY )
	{
		*maxY = (int) m_hotspotMaxs.y;
	}
}

void CRadialButton::PaintBackground( void )
{
	if ( RadialMenuDebug.GetBool() && (IsArmed() || m_fakeArmed) )
	{
		Color c = GetRadialFgColor();
		vgui::surface()->DrawSetColor( c );
		vgui::surface()->DrawFilledRect( m_hotspotMins.x, m_hotspotMins.y, m_hotspotMaxs.x, m_hotspotMaxs.y );
	}

	Color c = GetRadialBgColor();
	vgui::surface()->DrawSetColor( c );
	vgui::surface()->DrawSetTexture( m_nMainMaterial );

	if ( RadialMenuDebug.GetInt() == 2 )
	{
		vgui::surface()->DrawTexturedPolygon( m_numHotspotPoints, m_hotspotPoints );
	}
	else
	{
		vgui::surface()->DrawTexturedPolygon( m_numVisibleHotspotPoints, m_visibleHotspotPoints );
	}

	if ( m_numSubmenuPoints && m_hasSubmenu )
	{
		vgui::surface()->DrawTexturedPolygon( m_numSubmenuPoints, m_submenuPoints );
	}
}
void CRadialButton::PaintBorder( void )
{
	if ( !m_hasBorders )
		return;

	Color c = GetRadialFgColor();
	vgui::surface()->DrawSetColor( c );
	vgui::surface()->DrawSetTexture( m_nWhiteMaterial );

	if ( RadialMenuDebug.GetInt() == 2 )
	{
		vgui::surface()->DrawTexturedPolyLine( m_hotspotPoints, m_numHotspotPoints );
	}
	else
	{
		vgui::surface()->DrawTexturedPolyLine( m_visibleHotspotPoints, m_numVisibleHotspotPoints );
	}

	if ( m_numSubmenuPoints && m_hasSubmenu )
	{
		vgui::surface()->DrawTexturedPolyLine( m_submenuPoints, m_numSubmenuPoints );
	}
}


void CRadialButton::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );
	MEM_ALLOC_CREDIT();
	SetDefaultColor( scheme->GetColor( "Rosetta.DefaultFgColor", Color( 255, 176, 0, 255 ) ), scheme->GetColor( "Rosetta.DefaultBgColor", Color( 0, 0, 0, 128 ) ) );
	m_armedFgColor = scheme->GetColor( "Rosetta.DefaultFgColor", Color( 255, 176, 0, 255 ) );
	m_armedBgColor = scheme->GetColor( "Rosetta.ArmedBgColor", Color( 0, 28, 192, 128 ) );
	SetArmedColor( m_armedFgColor, m_armedBgColor );
	m_disabledBgColor = scheme->GetColor( "Rosetta.DisabledBgColor", Color( 0, 0, 0, 128 ) );
	m_disabledFgColor = scheme->GetColor( "Rosetta.DisabledFgColor", Color( 0, 0, 0, 128 ) );
	m_chosenFgColor = scheme->GetColor( "Rosetta.DefaultFgColor", Color( 255, 176, 0, 255 ) );
	m_chosenBgColor = scheme->GetColor( "Rosetta.ArmedBgColor", Color( 0, 28, 192, 128 ) );
	SetMouseClickEnabled( MOUSE_RIGHT, true );
	SetButtonActivationType( ACTIVATE_ONPRESSED );

	m_hasBorders = false;
	const char *borderStr = scheme->GetResourceString( "Rosetta.DrawBorder" );
	if ( borderStr && atoi( borderStr ) )
	{
		m_hasBorders = true;
	}

	const char *fontStr = scheme->GetResourceString( "Rosetta.DefaultFont" );
	if ( fontStr )
	{
		m_defaultFont = scheme->GetFont( fontStr, true );
	}
	else
	{
		m_defaultFont = scheme->GetFont( "Default", true );
	}

	fontStr = scheme->GetResourceString( "Rosetta.ArmedFont" );
	if ( fontStr )
	{
		m_armedFont = scheme->GetFont( fontStr, true );
	}
	else
	{
		m_armedFont = scheme->GetFont( "Default", true );
	}
	SetFont( m_defaultFont );

	if ( RadialMenuDebug.GetBool() )
	{
		SetCursor( vgui::dc_crosshair );
	}
	else
	{
		SetCursor( vgui::dc_blank );
	}
}

void CRadialButton::SetChosen( bool chosen )
{
	m_chosen = chosen;
}

void CRadialButton::SetFakeArmed( bool armed )
{
	m_fakeArmed = armed;

	bool isArmed = ( IsArmed() || m_fakeArmed );
	vgui::HFont currentFont = (isArmed) ? m_armedFont : m_defaultFont;
	if ( currentFont )
	{
		SetFont( currentFont );
		InvalidateLayout( true );
	}
}

void CRadialButton::OnCursorEntered( void )
{
	int nSlot = vgui::ipanel()->GetMessageContextId( GetVPanel() );
	ACTIVE_SPLITSCREEN_PLAYER_GUARD( nSlot );

	int wide, tall;
	GetSize( wide, tall );

	int centerx = wide / 2;
	int centery = tall / 2;

	int x, y;
	vgui::surface()->SurfaceGetCursorPos( x, y );
	ScreenToLocal( x, y );

	if ( x != centerx || y != centery )
	{
		C_BasePlayer *localPlayer = C_BasePlayer::GetLocalPlayer();
		if ( localPlayer )
		{
			if ( !m_fakeArmed )
			{
				localPlayer->EmitSound("MouseMenu.rollover");
			}
		}

		if ( m_parent )
		{
			m_parent->OnCursorEnteredButton( x, y, this );
		}
	}

	m_fakeArmed = false;

	BaseClass::OnCursorEntered();
}


void CRadialButton::OnMousePressed( vgui::MouseCode code )
{
	if ( code == MOUSE_RIGHT )
	{
		SetCommand( "done" );
	}
	else
	{
		SetChosen( true );
	}
	BaseClass::OnMousePressed( code );
}

bool CRadialButton::MouseClick(int x, int y, bool bRightClick, bool bDown)
{
	if ( !m_fakeArmed )
		return false;

	vgui::Panel *pParent = GetParent();
	if ( !pParent )
		return false;

	if ( pParent->GetAlpha() <= 0 )
		return false;

	if ( bRightClick )
	{
		SetCommand( "done" );
	}

	BaseClass::OnMousePressed( bRightClick ? MOUSE_RIGHT : MOUSE_LEFT );

	return true;
}