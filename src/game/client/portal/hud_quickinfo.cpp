//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "engine/IEngineSound.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Controls.h"
#include "vgui_controls/Panel.h"
#include "vgui/ISurface.h"
#include "c_portal_player.h"
#include "c_weapon_portalgun.h"
#include "igameuifuncs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	HEALTH_WARNING_THRESHOLD	25


static ConVar	hud_quickinfo( "hud_quickinfo", "1", FCVAR_ARCHIVE );
static ConVar	hud_quickinfo_swap( "hud_quickinfo_swap", "0", FCVAR_ARCHIVE );

extern ConVar crosshair;

#define QUICKINFO_EVENT_DURATION	1.0f
#define	QUICKINFO_BRIGHTNESS_FULL	255
#define	QUICKINFO_BRIGHTNESS_DIM	64
#define	QUICKINFO_FADE_IN_TIME		0.5f
#define QUICKINFO_FADE_OUT_TIME		2.0f

/*
==================================================
CHUDQuickInfo 
==================================================
*/

using namespace vgui;

class CHUDQuickInfo : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHUDQuickInfo, vgui::Panel );
public:    
	CHUDQuickInfo( const char *pElementName );
    ~CHUDQuickInfo();
	void Init( void );
	void VidInit( void );
	bool ShouldDraw( void );
	//virtual void OnThink();
	virtual void Paint();
	
	virtual void ApplySchemeSettings( IScheme *scheme );	
private:

	void 	DrawCrosshair(class Color ,float );
    void 	DrawPortalHint(class Vector & ,bool );
    void 	DrawPortalHints();
	void	DrawWarning( int x, int y, CHudTexture *icon, float &time );
    void 	UpdateEventTime();
    bool 	EventTimeElapsed();
		
	float	m_flLastEventTime;
	
	float	m_fLastPlacedAlpha[2];
	bool	m_bLastPlacedAlphaCountingUp[2];

	CHudTexture	*m_icon_rbn;	// right bracket
	CHudTexture	*m_icon_lbn;	// left bracket

	CHudTexture	*m_icon_rb;		// right bracket, full
	CHudTexture	*m_icon_lb;		// left bracket, full
	
    int m_nArrowTexture;
    int m_nCursorRadius;
    int m_nPortalIconOffsetX;
    int m_nPortalIconOffsetY;
	
    float m_flPortalIconScale;
};

DECLARE_HUDELEMENT( CHUDQuickInfo );

CHUDQuickInfo::CHUDQuickInfo( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "HUDQuickInfo" )
{
	vgui::Panel *pParent = GetClientMode()->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_CROSSHAIR );

	m_fLastPlacedAlpha[0] = m_fLastPlacedAlpha[1] = 80;
	m_bLastPlacedAlphaCountingUp[0] = m_bLastPlacedAlphaCountingUp[1] = true;
}

CHUDQuickInfo::~CHUDQuickInfo()
{
	if (vgui::surface() && m_nArrowTexture != -1)
	{
		vgui::surface()->DestroyTextureID( m_nArrowTexture );
		m_nArrowTexture = -1;
	}
}

void CHUDQuickInfo::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	SetPaintBackgroundEnabled( false );
}


void CHUDQuickInfo::Init( void )
{
	m_flLastEventTime   = 0.0f;
}


void CHUDQuickInfo::VidInit( void )
{
	Init();
	
	m_icon_rb = HudIcons().GetIcon( "portal_crosshair_right_valid" );
	m_icon_lb = HudIcons().GetIcon( "portal_crosshair_left_valid" );
	m_icon_rbn = HudIcons().GetIcon( "portal_crosshair_right_invalid" );
	m_icon_lbn = HudIcons().GetIcon( "portal_crosshair_left_invalid" );
}


void CHUDQuickInfo::DrawWarning( int x, int y, CHudTexture *icon, float &time )
{
	float scale	= (int)( fabs(sin(gpGlobals->curtime*8.0f)) * 128.0);

	// Only fade out at the low point of our blink
	if ( time <= (gpGlobals->frametime * 200.0f) )
	{
		if ( scale < 40 )
		{
			time = 0.0f;
			return;
		}
		else
		{
			// Counteract the offset below to survive another frame
			time += (gpGlobals->frametime * 200.0f);
		}
	}
	
	// Update our time
	time -= (gpGlobals->frametime * 200.0f);
	Color caution = GetHud().m_clrCaution;
	caution[3] = scale * 255;

	icon->DrawSelf( x, y, caution );
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHUDQuickInfo::ShouldDraw( void )
{
	if (!m_icon_rb || !m_icon_rbn || !m_icon_lb || !m_icon_lbn)
		return false;

	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( player == NULL )
		return false;

	if ( !crosshair.GetBool() )
		return false;

	return ( CHudElement::ShouldDraw() && !engine->IsDrawingLoadingImage() );
}


void CHUDQuickInfo::Paint()
{
	C_Portal_Player *pPortalPlayer = (C_Portal_Player*)( C_BasePlayer::GetLocalPlayer() );
	if ( pPortalPlayer == NULL )
		return;

	C_BaseCombatWeapon *pWeapon = pPortalPlayer->GetActiveWeapon();
	if ( pWeapon == NULL )
		return;
	
	int		xCenter	= ( ScreenWidth() ) / 2;
	int		yCenter = ( ScreenHeight() ) / 2;

	Color clrNormal = GetHud().m_clrNormal;
	clrNormal[3] = 255;

	SetActive( true );

	//m_icon_c->DrawSelf( xCenter, yCenter, clrNormal );

	// adjust center for the bigger crosshairs
	xCenter	= ScreenWidth() / 2;
	yCenter = ( ScreenHeight() - m_icon_lb->Height() ) / 2;

	C_WeaponPortalgun *pPortalgun = dynamic_cast<C_WeaponPortalgun*>( pWeapon );

	bool bPortalPlacability[2];

	// NOTE: This may not be accurate, but it's good enough for now.
	if ( pPortalgun )
	{
		// Crosshair hides when an object is picked up in Portal 2
		if (pPortalgun->IsHoldingObject())
			return;

		if ( pPortalgun->GetAssociatedPortal( false ) )
			bPortalPlacability[0] = pPortalgun->GetAssociatedPortal( false )->IsActive();

		if ( pPortalgun->GetAssociatedPortal( true ) )
		bPortalPlacability[1] = pPortalgun->GetAssociatedPortal( true )->IsActive();

		//bPortalPlacability[0] = pPortalgun->GetPortal1Placablity() > 0.5f;
		//bPortalPlacability[1] = pPortalgun->GetPortal2Placablity() > 0.5f;
	}

	if ( !hud_quickinfo.GetInt() || !pPortalgun || ( !pPortalgun->CanFirePortal1() && !pPortalgun->CanFirePortal2() ) )
	{
		// no quickinfo or we can't fire either portal, just draw the small versions of the crosshairs
		clrNormal[3] = 196;
		//m_icon_lbnone->DrawSelf(xCenter - (m_icon_lbnone->Width() * 2), yCenter, clrNormal);
		//m_icon_rbnone->DrawSelf(xCenter + m_icon_rbnone->Width(), yCenter, clrNormal);
		return;
	}

	const unsigned char iAlphaStart = 150;	   

	Color portal1Color = UTIL_Portal_Color( 1 );
	Color portal2Color = UTIL_Portal_Color( 2 );

	portal1Color[ 3 ] = iAlphaStart;
	portal2Color[ 3 ] = iAlphaStart;

	const int iBaseLastPlacedAlpha = 128;
	Color lastPlaced1Color = Color( portal1Color[0], portal1Color[1], portal1Color[2], iBaseLastPlacedAlpha );
	Color lastPlaced2Color = Color( portal2Color[0], portal2Color[1], portal2Color[2], iBaseLastPlacedAlpha );

	const float fLastPlacedAlphaLerpSpeed = 300.0f;

	
	float fLeftPlaceBarFill = 0.0f;
	float fRightPlaceBarFill = 0.0f;

	if ( pPortalgun->CanFirePortal1() && pPortalgun->CanFirePortal2() )
	{
		int iDrawLastPlaced = 0;

		//do last placed indicator effects
		if ( pPortalgun->GetLastFiredPortal() == 1 )
		{
			iDrawLastPlaced = 0;
			fLeftPlaceBarFill = 1.0f;
		}
		else if ( pPortalgun->GetLastFiredPortal() == 2 )
		{
			iDrawLastPlaced = 1;
			fRightPlaceBarFill = 1.0f;			
		}

		if( m_bLastPlacedAlphaCountingUp[iDrawLastPlaced] )
		{
			m_fLastPlacedAlpha[iDrawLastPlaced] += gpGlobals->absoluteframetime * fLastPlacedAlphaLerpSpeed * 2.0f;
			if( m_fLastPlacedAlpha[iDrawLastPlaced] > 255.0f )
			{
				m_bLastPlacedAlphaCountingUp[iDrawLastPlaced] = false;
				m_fLastPlacedAlpha[iDrawLastPlaced] = 255.0f - (m_fLastPlacedAlpha[iDrawLastPlaced] - 255.0f);
			}
		}
		else
		{
			m_fLastPlacedAlpha[iDrawLastPlaced] -= gpGlobals->absoluteframetime * fLastPlacedAlphaLerpSpeed;
			if( m_fLastPlacedAlpha[iDrawLastPlaced] < (float)iBaseLastPlacedAlpha )
			{
				m_fLastPlacedAlpha[iDrawLastPlaced] = (float)iBaseLastPlacedAlpha;
			}
		}

		//reset the last placed indicator on the other side
		m_fLastPlacedAlpha[1 - iDrawLastPlaced] -= gpGlobals->absoluteframetime * fLastPlacedAlphaLerpSpeed;
		if( m_fLastPlacedAlpha[1 - iDrawLastPlaced] < 0.0f )
		{
			m_fLastPlacedAlpha[1 - iDrawLastPlaced] = 0.0f;
		}
		m_bLastPlacedAlphaCountingUp[1 - iDrawLastPlaced] = true;

		if ( pPortalgun->GetLastFiredPortal() != 0 )
		{
			lastPlaced1Color[3] = m_fLastPlacedAlpha[0];
			lastPlaced2Color[3] = m_fLastPlacedAlpha[1];
		}
		else
		{
			lastPlaced1Color[3] = 0.0f;
			lastPlaced2Color[3] = 0.0f;
		}
	}
	//can't fire both portals, and we want the crosshair to remain somewhat symmetrical without being confusing
	else if ( !pPortalgun->CanFirePortal1() )
	{
		// clone portal2 info to portal 1
		portal1Color = portal2Color;
		lastPlaced1Color[3] = 0.0f;
		lastPlaced2Color[3] = 0.0f;
		bPortalPlacability[0] = bPortalPlacability[1];
	}
	else if ( !pPortalgun->CanFirePortal2() )
	{
		// clone portal1 info to portal 2
		portal2Color = portal1Color;
		lastPlaced1Color[3] = 0.0f;
		lastPlaced2Color[3] = 0.0f;
		bPortalPlacability[1] = bPortalPlacability[0];
	}
		
	if ( !hud_quickinfo_swap.GetBool() )
	{
		if ( bPortalPlacability[0] )
			m_icon_lb->DrawSelf(xCenter - (m_icon_lb->Width() * 0.64f ), yCenter - ( m_icon_rb->Height() * 0.17f ), portal1Color);
		else
			m_icon_lbn->DrawSelf(xCenter - (m_icon_lbn->Width() * 0.64f ), yCenter - ( m_icon_rb->Height() * 0.17f ), portal1Color);

		if ( bPortalPlacability[1] )
			m_icon_rb->DrawSelf(xCenter + ( m_icon_rb->Width() * -0.35f ), yCenter + ( m_icon_rb->Height() * 0.17f ), portal2Color);
		else
			m_icon_rbn->DrawSelf(xCenter + ( m_icon_rbn->Width() * -0.35f ), yCenter + ( m_icon_rb->Height() * 0.17f ), portal2Color);
	}
	else
	{
		if ( bPortalPlacability[1] )
			m_icon_lb->DrawSelf(xCenter - (m_icon_lb->Width() * 0.64f ), yCenter - ( m_icon_rb->Height() * 0.17f ), portal2Color);
		else
			m_icon_lbn->DrawSelf(xCenter - (m_icon_lbn->Width() * 0.64f ), yCenter - ( m_icon_rb->Height() * 0.17f ), portal2Color);

		if ( bPortalPlacability[0] )
			m_icon_rb->DrawSelf(xCenter + ( m_icon_rb->Width() * -0.35f ), yCenter + ( m_icon_rb->Height() * 0.17f ), portal1Color);
		else
			m_icon_rbn->DrawSelf(xCenter + ( m_icon_rbn->Width() * -0.35f ), yCenter + ( m_icon_rb->Height() * 0.17f ), portal1Color);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHUDQuickInfo::UpdateEventTime( void )
{
	m_flLastEventTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHUDQuickInfo::EventTimeElapsed( void )
{
	if (( gpGlobals->curtime - m_flLastEventTime ) > QUICKINFO_EVENT_DURATION )
		return true;

	return false;
}

