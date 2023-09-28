//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include "vgui_avatarimage.h"

#if defined( _X360 )
#include "xbox/xbox_win32stubs.h"
#endif

#if defined( _PS3 )
#include "ps3/ps3_core.h"
#include "ps3/ps3_win32stubs.h"
#endif

#ifndef NO_STEAM
#include "steam/steam_api.h"
#endif
#include "hud.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


DECLARE_BUILD_FACTORY( CAvatarImagePanel );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CAvatarImage::CAvatarImage( void )
{
	m_iTextureID = -1;
	ClearAvatarSteamID();
#ifndef NO_STEAM
	m_SourceArtSize = eAvatarSmall;
#endif
	m_pFriendIcon = NULL;
	m_nX = 0;
	m_nY = 0;
}

CAvatarImage::~CAvatarImage()
{
	if ( vgui::surface() && m_iTextureID != -1 )
	{
		vgui::surface()->DestroyTextureID( m_iTextureID );
		m_iTextureID = -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImage::ClearAvatarSteamID( void ) 
{ 
	m_bValid = false; 
	m_bFriend = false;
#ifndef NO_STEAM
	m_SteamID.Set( 0, k_EUniverseInvalid, k_EAccountTypeInvalid );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#ifndef NO_STEAM
bool CAvatarImage::SetAvatarSteamID( CSteamID steamIDUser )
{
	if ( m_steamIDUser == steamIDUser && m_bValid )
		return true;

	ClearAvatarSteamID();
	m_steamIDUser = steamIDUser;

	if ( steamapicontext->SteamFriends() && steamapicontext->SteamUtils() )
	{
		m_SteamID = steamIDUser;

		int iAvatar = -1;
		switch ( m_SourceArtSize )
		{
		case eAvatarSmall:
			iAvatar = steamapicontext->SteamFriends()->GetSmallFriendAvatar( steamIDUser );
			break;
		case eAvatarMedium:
			iAvatar = steamapicontext->SteamFriends()->GetMediumFriendAvatar( steamIDUser );
			break;
		case eAvatarLarge:
			iAvatar = steamapicontext->SteamFriends()->GetLargeFriendAvatar( steamIDUser );
			break;
		}

		/*
		// See if it's in our list already
		*/

		uint32 wide, tall;
		if ( steamapicontext->SteamUtils()->GetImageSize( iAvatar, &wide, &tall ) )
		{
			int cubImage = wide * tall * 4;
			byte *rgubDest = (byte*)stackalloc( cubImage );
			steamapicontext->SteamUtils()->GetImageRGBA( iAvatar, rgubDest, cubImage );
			InitFromRGBA( rgubDest, wide, tall );

			/*
			// put it in the list
			RGBAImage *pRGBAImage = new RGBAImage( rgubDest, wide, tall );
			int iImageList = m_pImageList->AddImage( pRGBAImage );
			m_mapAvatarToIImageList.Insert( iAvatar, iImageList );
			*/
		}

		UpdateFriendStatus();
	}

	return m_bValid;
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImage::UpdateFriendStatus( void )
{
#ifndef NO_STEAM
	if ( !m_SteamID.IsValid() )
		return;

	if ( steamapicontext->SteamFriends() && steamapicontext->SteamUtils() )
	{
		m_bFriend = steamapicontext->SteamFriends()->HasFriend( m_SteamID, k_EFriendFlagImmediate );
		if ( m_bFriend && !m_pFriendIcon )
		{
			m_pFriendIcon = HudIcons().GetIcon( "ico_friend_indicator_avatar" );
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImage::InitFromRGBA( const byte *rgba, int width, int height )
{
	// Texture size may be changing, so recreate
	if ( m_iTextureID != -1 )
	{
		vgui::surface()->DestroyTextureID( m_iTextureID );
		m_iTextureID = -1;
	}

	m_iTextureID = vgui::surface()->CreateNewTextureID( true );

	vgui::surface()->DrawSetTextureRGBA( m_iTextureID, rgba, width, height );
	m_nWide = XRES(width);
	m_nTall = YRES(height);
	m_Color = Color( 255, 255, 255, 255 );

	m_bValid = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImage::Paint( void )
{
	if ( m_bValid )
	{
		if ( m_bFriend && m_pFriendIcon )
		{
			m_pFriendIcon->DrawSelf( m_nX, m_nY, m_nWide, m_nTall, m_Color );
		}

		vgui::surface()->DrawSetColor( m_Color );
		vgui::surface()->DrawSetTexture( m_iTextureID );
		vgui::surface()->DrawTexturedRect( m_nX + AVATAR_INDENT_X, m_nY + AVATAR_INDENT_Y, m_nX + AVATAR_INDENT_X + m_iAvatarWidth, m_nY + AVATAR_INDENT_Y + m_iAvatarHeight );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CAvatarImagePanel::CAvatarImagePanel( vgui::Panel *parent, const char *name ) : vgui::ImagePanel( parent, name )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImagePanel::SetPlayer( C_BasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		int iIndex = pPlayer->entindex();
		SetPlayerByIndex( iIndex );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImagePanel::SetPlayerByIndex( int iIndex )
{
#ifndef NO_STEAM
	if ( iIndex && steamapicontext->SteamUtils() )
	{
		player_info_t pi;
		if ( engine->GetPlayerInfo(iIndex, &pi) )
		{
			if ( pi.friendsID )
			{
				CSteamID steamIDForPlayer( pi.friendsID, 1, steamapicontext->SteamUtils()->GetConnectedUniverse(), k_EAccountTypeIndividual );
				SetAvatarBySteamID( &steamIDForPlayer );
			}
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAvatarImagePanel::PaintBackground( void )
{
	vgui::IImage *pImage = GetImage();
	if ( pImage )
	{
		pImage->SetColor( GetDrawColor() );
		pImage->Paint();
	}
}

#ifndef NO_STEAM
void CAvatarImagePanel::SetAvatarBySteamID( CSteamID *friendsID )
{
	if ( !GetImage() )
	{
		CAvatarImage *pImage = new CAvatarImage();
		SetImage( pImage );
	}

	// Indent the image. These are deliberately non-resolution-scaling.
	int iIndent = 2;
	GetImage()->SetPos( iIndent, iIndent );
	int wide = GetWide() - (iIndent*2);

	((CAvatarImage*)GetImage())->SetAvatarSize( ( wide > 32 ) ? eAvatarMedium : eAvatarSmall );
	((CAvatarImage*)GetImage())->SetAvatarSteamID( *friendsID );

	GetImage()->SetSize( wide, GetTall()-(iIndent*2) );
}
#endif
