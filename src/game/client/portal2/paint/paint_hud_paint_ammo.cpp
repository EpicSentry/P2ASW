//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <vgui_controls/editablepanel.h>
#include <vgui_controls/progressbar.h>
#include <vgui_controls/label.h>
#include <vgui/ivgui.h>
#include <vgui/isurface.h>

#include "paint/paint_color_manager.h"
#include "c_weapon_paintgun.h"
//#include "weapon_paintgun_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// FIXME: Bring this back for DLC2
extern ConVar paintgun_ammo_type;
extern ConVar paintgun_max_ammo;

ConVar cl_debug_paint_ammo_bar( "cl_debug_paint_ammo_bar", "0", FCVAR_CHEAT );


using namespace vgui;

class CHUDPaintAmmo : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHUDPaintAmmo, EditablePanel );
public:
	CHUDPaintAmmo( const char *pElementName );

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );

	virtual bool ShouldDraw( void );

	virtual void OnTick( void );

	virtual void ApplySchemeSettings( IScheme *pScheme );

private:
	vgui::ContinuousProgressBar *m_pProgressBar;
};


DECLARE_HUDELEMENT( CHUDPaintAmmo );

//CHUDPaintIcons paintIcons( "PaintIcons" );


CHUDPaintAmmo::CHUDPaintAmmo( const char *pElementName )
		: CHudElement( pElementName ), BaseClass( NULL, "PaintAmmo" )
{
	SetParent( GetClientMode()->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	m_pProgressBar = new vgui::ContinuousProgressBar( this, "PaintAmmoBar" );
	m_pProgressBar->SetProgressDirection( ProgressBar::PROGRESS_NORTH );

	vgui::ivgui()->AddTickSignal( GetVPanel(), 50 );
}


bool CHUDPaintAmmo::ShouldDraw( void )
{

	if( paintgun_ammo_type.GetInt() == 0 )
	{
			return false;
	}

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( pPlayer )
	{
		C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();

		//If the player is not holding the paint gun, don't draw the ammo bar
		if( !pWeapon || !FClassnameIs( pWeapon, "weapon_paintgun" ) )
		{
			return false;
		}
	}

	return CHudElement::ShouldDraw();
}


void CHUDPaintAmmo::OnTick( void )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( pPlayer )
	{
		C_WeaponPaintGun *pPaintGun = dynamic_cast<C_WeaponPaintGun*>(pPlayer->Weapon_OwnsThisType( "weapon_paintgun" ));

		if( pPaintGun )
		{
			float flAmmo = pPaintGun->GetPaintAmmo() / paintgun_max_ammo.GetInt();

			flAmmo = clamp( flAmmo, 0.0f, 1.0f );

			m_pProgressBar->SetProgress( flAmmo );

			if( cl_debug_paint_ammo_bar.GetBool() )
			{
				DevMsg( "Max: %d, Ammo: %d, Percent: %f\n",  paintgun_max_ammo, pPaintGun->GetPaintAmmo(), flAmmo );
			}

			//GetHud().DrawProgressBar( 300, 300, 10, 100, flAmmo, Color( 255, 0, 255, 255 ), CHud::HUDPB_VERTICAL );
		}
	}
}


void CHUDPaintAmmo::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/hud_paint_ammo.res" );
}


void CHUDPaintAmmo::Init( void )
{
	Reset();
}


void CHUDPaintAmmo::Reset( void )
{
	if( m_pProgressBar )
	{
		m_pProgressBar->SetBgColor( Color(0, 0, 0, 128) );
		m_pProgressBar->SetFgColor( Color(255, 255, 255, 255) );
	}
}


void CHUDPaintAmmo::VidInit( void )
{
	Reset();
}
