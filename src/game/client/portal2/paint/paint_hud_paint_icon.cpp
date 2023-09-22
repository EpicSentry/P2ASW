//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <vgui_controls/editablepanel.h>
#include <vgui_controls/imagepanel.h>
#include <vgui/ivgui.h>
#include <vgui/isurface.h>

#include "portal_player_shared.h"
#include "c_portal_player.h"
#include "paint/paint_color_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar cl_hud_paint_icon_fade_timer( "cl_hud_paint_icon_fade_timer", "0.5f", FCVAR_DEVELOPMENTONLY );
ConVar cl_hud_show_paint_icons( "cl_hud_show_paint_icons", "0", FCVAR_DEVELOPMENTONLY );

class CHUDPaintIcons : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHUDPaintIcons, EditablePanel );
public:
	CHUDPaintIcons( const char *pElementName );

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );

	virtual bool ShouldDraw( void );

	virtual void OnTick( void );

	virtual void ApplySchemeSettings( IScheme *pScheme );

	virtual void Paint();
private:

	void HideAllPaintIcons();

	vgui::ImagePanel *m_pPaintImage[STICK_POWER];
	float m_fImageTimers[ STICK_POWER ];
	float m_fLastTickTime;
};


DECLARE_HUDELEMENT( CHUDPaintIcons );

//CHUDPaintIcons paintIcons( "PaintIcons" );


CHUDPaintIcons::CHUDPaintIcons( const char *pElementName )
		: CHudElement( pElementName ), EditablePanel( NULL, "PaintIcon" )
{
	SetParent( GetClientMode()->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", "PaintScheme");
	SetScheme(scheme);

	SetVisible(false);
	
	LoadControlSettings( "resource/UI/hud_paint_icons.res" );
	
	m_pPaintImage[BOUNCE_POWER] = dynamic_cast< vgui::ImagePanel* >( FindChildByName( "PaintBounceImage" ) );
	m_pPaintImage[SPEED_POWER] = dynamic_cast< vgui::ImagePanel* >( FindChildByName( "PaintStickImage" ) );
	m_pPaintImage[STICK_POWER] = dynamic_cast< vgui::ImagePanel* >( FindChildByName( "PaintSpeedImage" ) );

	for( int i = 0; i < STICK_POWER; ++i )
	{
		m_fImageTimers[i] = 0.0f;
		m_pPaintImage[i]->SetVisible( true );
		m_pPaintImage[i]->SetDrawColor( MapPowerToColor( (PaintPowerType)i ) );
	}

	vgui::ivgui()->AddTickSignal( GetVPanel(), 50 );

	m_fLastTickTime = gpGlobals->curtime;
}


void CHUDPaintIcons::Init( void )
{
	Reset();
}


void CHUDPaintIcons::Reset( void )
{
	//HideAllPaintIcons();
}


void CHUDPaintIcons::VidInit( void )
{
	Reset();
}


bool CHUDPaintIcons::ShouldDraw( void )
{
	if( !cl_hud_show_paint_icons.GetBool() )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}


void CHUDPaintIcons::OnTick( void )
{
	float fDeltaTime = gpGlobals->curtime - m_fLastTickTime;
	m_fLastTickTime = gpGlobals->curtime;

	//Update all the fade timers for the icons
	for( int i = 0; i < STICK_POWER; ++i )
	{
		if( m_fImageTimers[i] > 0.0f )
		{
			m_fImageTimers[i] -= fDeltaTime;
		}
		else
		{
			m_fImageTimers[i] = 0.0f;
		}
	}

	//Check the active power on the paint player
	C_Portal_Player *pPaintPlayer = ToPortalPlayer( C_BasePlayer::GetLocalPlayer() );

	if( pPaintPlayer )
	{
		const C_PortalPlayerLocalData& localData = pPaintPlayer->GetPortalPlayerLocalData();

		// Reset the time for all active powers
		for( unsigned i = 0; i < STICK_POWER; ++i )
		{
			if( localData.m_PaintPowerHudInfoValidity[i] )
				m_fImageTimers[i] = cl_hud_paint_icon_fade_timer.GetFloat();
		}

		// Reset the time for bounce and speed if they are deactivating in the air.
		// The bounce HUD icon needs to stay around long enough to be noticed, and
		// speed doesn't deactivate until the player is on the ground again.
		//const bool bInAir = pPaintPlayer->GetGroundEntity() == NULL;
		//if( localData.m_PaintPowerStates[BOUNCE_POWER] == DEACTIVATING_PAINT_POWER )
		//	m_fImageTimers[BOUNCE_POWER] = cl_hud_paint_icon_fade_timer.GetFloat();

		//if( bInAir && localData.m_PaintPowerStates[SPEED_POWER] == DEACTIVATING_PAINT_POWER )
		//	m_fImageTimers[SPEED_POWER] = cl_hud_paint_icon_fade_timer.GetFloat();
	}
	else
	{
		for( int i = 0; i < STICK_POWER; ++i )
		{
			m_fImageTimers[i] = 0.0f;
		}
	}
	
	//Update the colors for all the icons
	for( int i = 0; i < STICK_POWER; ++i )
	{
		Color drawColor = m_pPaintImage[i]->GetDrawColor();
		float fPercent = m_fImageTimers[i]/cl_hud_paint_icon_fade_timer.GetFloat();
		fPercent = clamp( fPercent, 0.0f, 1.0f );
		drawColor[3] = Lerp( fPercent, 0, 255 );
		m_pPaintImage[i]->SetDrawColor( drawColor );
	}
}


void CHUDPaintIcons::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/hud_paint_icons.res" );
}


void CHUDPaintIcons::HideAllPaintIcons()
{
	//for( int i = 0; i < PAINT_POWER_TYPE_COUNT; ++i)
	//{
	//	m_pPaintImage[i]->SetVisible( false );
	//}
}


void CHUDPaintIcons::Paint()
{
	//SetPaintBorderEnabled( true );
	//surface()->DrawSetTexture( m_nTextureID );
	//surface()->DrawSetColor( 0, 255, 0, 255 );
	//surface()->DrawFilledRect( 0, 0, 128, 128 );
	//surface()->DrawTexturedRect( 0, 0, 128, 128 );
	//surface()->DrawSetTextureFile( );
}
