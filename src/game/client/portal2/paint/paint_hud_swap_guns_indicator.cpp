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

#include "c_portal_player.h"
#include "view.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar cl_draw_gun_swap_indicator( "cl_draw_gun_swap_indicator", "0", FCVAR_CHEAT );

class CHUDSwapGunsIndicator : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHUDSwapGunsIndicator, EditablePanel );
public:
	CHUDSwapGunsIndicator( const char *pElementName );

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );

	virtual bool ShouldDraw( void );

	virtual void OnTick( void );

	virtual void ApplySchemeSettings( IScheme *pScheme );

private:
	vgui::ImagePanel *m_pSwapGunsImage;
};


DECLARE_HUDELEMENT( CHUDSwapGunsIndicator );

CHUDSwapGunsIndicator::CHUDSwapGunsIndicator( const char *pElementName )
			: CHudElement( pElementName ), BaseClass( NULL, "PaintSwapGuns" )
{
	SetParent( GetClientMode()->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/ClientScheme.res", "PaintScheme");
	SetScheme( scheme );

	m_pSwapGunsImage = new vgui::ImagePanel( this, "SwapGunsIndicator" );

	vgui::ivgui()->AddTickSignal( GetVPanel(), 0 );
}


bool CHUDSwapGunsIndicator::ShouldDraw( void )
{
	if( cl_draw_gun_swap_indicator.GetBool() == false )
	{
		return false;
	}

	//If the game is single player
	if( gpGlobals->maxClients == 1)
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}


void GetSwapPosition( const Vector &vecDelta, float flRadius, float *xpos, float *ypos, float *flRotation )
{
	ASSERT_LOCAL_PLAYER_RESOLVABLE();
	int nSlot = GET_ACTIVE_SPLITSCREEN_SLOT();

	// Player Data
	Vector playerPosition = MainViewOrigin(nSlot);
	QAngle playerAngles = MainViewAngles(nSlot);

	Vector forward, right, up;
	AngleVectors (playerAngles, &forward, &right, &up );
	forward.z = 0;
	VectorNormalize(forward);
	CrossProduct( up, forward, right );
	float front = DotProduct(vecDelta, forward);
	float side = DotProduct(vecDelta, right);
	*xpos = flRadius * -side;
	*ypos = flRadius * -front;

	// Get the rotation (yaw)
	*flRotation = atan2(*xpos,*ypos) + M_PI;
	*flRotation *= 180 / M_PI;

	float yawRadians = -(*flRotation) * M_PI / 180.0f;
	float ca = cos( yawRadians );
	float sa = sin( yawRadians );

	// Rotate it around the circle
	*xpos = (int)((ScreenWidth() / 2) + (flRadius * sa));
	*ypos = (int)((ScreenHeight() / 2) - (flRadius * ca));
}


void CHUDSwapGunsIndicator::OnTick( void )
{
	m_pSwapGunsImage->SetVisible( false );

	//For all the players
	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		C_Portal_Player *pOtherPlayer = (C_Portal_Player*)( UTIL_PlayerByIndex( i ) );

		//If the other player does not exist or if the other player is the local player
		if( pOtherPlayer == NULL || pOtherPlayer == C_BasePlayer::GetLocalPlayer() )
			continue; 

		if( pOtherPlayer->WantsToSwapGuns() )
		{
			Vector vecOtherPlayerPosition = pOtherPlayer->GetAbsOrigin();
			//Vector vecForward, vecRight, vecUp;
			//AngleVectors( pOtherPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );
			Vector vecOffset = Vector( 0, 0, 18.0f );
			Vector vecTarget = vecOtherPlayerPosition + vecOffset;
			Vector vecDelta = vecTarget - MainViewOrigin( GET_ACTIVE_SPLITSCREEN_SLOT() );

			int nX, nY;
			bool bOnScreen = GetVectorInScreenSpace( vecTarget, nX, nY );

			int nHalfWidth = GetWide() / 2;

			if( !bOnScreen || nX < nHalfWidth || nX > ScreenWidth() - nHalfWidth )
			{
				//Target is off screen
				VectorNormalize( vecDelta );

				float flXPos, flYPos;
				float flRotation;
				float flRadius = YRES(100);

				GetSwapPosition( vecDelta, flRadius, &flXPos, &flYPos, &flRotation );

				nX = flXPos;
				nY = flYPos;

				SetPos( nX - nHalfWidth, nY - ( GetTall() / 2 ) );
			}
			else
			{
				//Target is on screen

				SetPos( nX - nHalfWidth, nY - ( GetTall() / 2 ) );
			}


			//Vector vecScreen;
			//ScreenTransform( vecOffset, vecScreen );

			//float xCenter = ScreenWidth() / 2;
			//float yCenter = ScreenHeight() / 2;

			//float x = xCenter;
			//float y = yCenter;

			//// Find the real screen position
			//x += 0.5 * vecScreen[0] * ScreenWidth() + 0.5;
			//y -= 0.5 * vecScreen[1] * ScreenHeight() + 0.5;

			//int nMaxX = ScreenWidth();
			//int nMinX = 0;
			//int nMaxY = ScreenHeight();
			//int nMinY = 0;

			//if ( x > nMaxX )
			//{
			//	x = nMaxX;
			//}
			//else if ( x < nMinX )
			//{
			//	x = nMinX;
			//}
			//
			//if ( y > nMaxY )
			//{
			//	y = nMaxY;
			//}
			//else if ( y < nMinY )
			//{
			//	y = nMinY;
			//}

			//SetPos( x, y );

			//m_pSwapGunsImage->SetPos( x, y );
			m_pSwapGunsImage->SetVisible( true );
		}
	}
}


void CHUDSwapGunsIndicator::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/hud_paint_swap_guns.res" );
}


void CHUDSwapGunsIndicator::Init( void )
{
	Reset();
}


void CHUDSwapGunsIndicator::Reset( void )
{
}


void CHUDSwapGunsIndicator::VidInit( void )
{
	Reset();
}
