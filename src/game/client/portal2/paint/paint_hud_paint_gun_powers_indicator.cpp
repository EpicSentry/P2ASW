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

#include "c_weapon_paintgun.h"
#include "c_baseplayer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_hud_paint_gun_powers_indicator_show_current("cl_hud_paint_gun_powers_indicator_show_current", "0" );

using namespace vgui;

class CHUDPaintGunPowersIndicator : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHUDPaintGunPowersIndicator, EditablePanel );
public:
	CHUDPaintGunPowersIndicator( const char *pElementName );

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );

	virtual bool ShouldDraw( void );

	virtual void OnTick( void );

	virtual void ApplySchemeSettings( IScheme *pScheme );

private:

	void DimAllPaintPowers( void );
	void DimPaintPower( PaintPowerType paintPower );
	void BrightenPaintPower( PaintPowerType paintPower );

	vgui::ImagePanel *m_pPaintImage[STICK_POWER];
	vgui::ImagePanel *m_pCurrentPaint;
};


DECLARE_HUDELEMENT( CHUDPaintGunPowersIndicator );

CHUDPaintGunPowersIndicator::CHUDPaintGunPowersIndicator( const char *pElementName )
			: CHudElement( pElementName ), BaseClass( NULL, "PaintGunPowersIndicator" )
{
	SetParent( GetClientMode()->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/ClientScheme.res", "PaintScheme");
	SetScheme( scheme );

	//Set the images for the paint gun power indicators
	m_pPaintImage[BOUNCE_POWER] = new ImagePanel( this, "BouncePaintIndicator" );
	m_pPaintImage[STICK_POWER] = new ImagePanel( this, "StickPaintIndicator" );
	m_pPaintImage[SPEED_POWER] = new ImagePanel( this, "SpeedPaintIndicator" );

	for( int i = 0; i < STICK_POWER; ++i )
	{
		m_pPaintImage[i]->SetVisible( true );
		m_pPaintImage[i]->SetDrawColor( MapPowerToVisualColor( (PaintPowerType)i ) );
	}

	//Set the image for the current paint gun power indicator
	m_pCurrentPaint = new ImagePanel( this, "CurrentPaintIndicator" );
	m_pCurrentPaint->SetVisible( false );

	DimAllPaintPowers();

	vgui::ivgui()->AddTickSignal( GetVPanel(), 50 );
}


bool CHUDPaintGunPowersIndicator::ShouldDraw( void )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( pPlayer )
	{
		C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();

		//If the player is not holding the paint gun, don't draw the paint indicator
		if( !pWeapon || !FClassnameIs( pWeapon, "weapon_paintgun" ) )
		{
			DimAllPaintPowers();
			return false;
		}
	}

	return CHudElement::ShouldDraw();
}


void CHUDPaintGunPowersIndicator::OnTick( void )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( pPlayer )
	{
		C_WeaponPaintGun *pPaintGun = dynamic_cast<C_WeaponPaintGun*>(pPlayer->Weapon_OwnsThisType( "weapon_paintgun" ));

		if( pPaintGun )
		{
			//Get the current power from the paint gun
			PaintPowerType currentPower = pPaintGun->GetCurrentPaint();

			if( currentPower == NO_POWER || !pPaintGun->HasAnyPaintPower() )
			{
				m_pCurrentPaint->SetVisible( false );
				DimAllPaintPowers();
			}
			else
			{
				//Check the paint powers that the gun has
				for( int i = 0; i < STICK_POWER; ++i )
				{
					if( pPaintGun->HasPaintPower( (PaintPowerType)i ) )
					{
						BrightenPaintPower( (PaintPowerType)i );
					}
					else
					{
						DimPaintPower( (PaintPowerType)i );
					}
				}

				//Move the current paint selection box to the current paint
				int x, y;
				m_pPaintImage[currentPower]->GetPos( x, y );
				m_pCurrentPaint->SetPos( x, y );

				if( cl_hud_paint_gun_powers_indicator_show_current.GetBool() )
				{
					m_pCurrentPaint->SetVisible( true );
				}
				else
				{
					m_pCurrentPaint->SetVisible( false );
				}
			}
		}
	}
}


void CHUDPaintGunPowersIndicator::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/hud_paint_gun_powers_indicator.res" );
}


void CHUDPaintGunPowersIndicator::Init( void )
{
	Reset();
}


void CHUDPaintGunPowersIndicator::Reset( void )
{
}


void CHUDPaintGunPowersIndicator::VidInit( void )
{
	Reset();
}

void CHUDPaintGunPowersIndicator::DimPaintPower( PaintPowerType paintPower )
{
	Color drawColor = m_pPaintImage[paintPower]->GetDrawColor();
	drawColor[3] = 0;

	m_pPaintImage[paintPower]->SetDrawColor( drawColor );
}

void CHUDPaintGunPowersIndicator::BrightenPaintPower( PaintPowerType paintPower )
{
	Color drawColor = m_pPaintImage[paintPower]->GetDrawColor();
	drawColor[3] = 255;

	m_pPaintImage[paintPower]->SetDrawColor( drawColor );
}

void CHUDPaintGunPowersIndicator::DimAllPaintPowers( void )
{
	for( int i = 0; i < STICK_POWER; ++i )
	{
		DimPaintPower( (PaintPowerType)i );
	}
	m_pCurrentPaint->SetVisible( false );
}
