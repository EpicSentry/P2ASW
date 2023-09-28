//====== Copyright 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef VGUI_AVATARIMAGE_H
#define VGUI_AVATARIMAGE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Image.h>
#include <vgui_controls/ImagePanel.h>
#ifndef NO_STEAM
#include "steam/steam_api.h"
#endif
#include "c_baseplayer.h"

// Avatar images, and avatar images with friends, don't scale with resolution.
#define AVATAR_INDENT_X			(22)
#define AVATAR_INDENT_Y			(1)
#define AVATAR_POSTDENT_X		(1)
#define AVATAR_POSTDENT_Y		(1)

enum EAvatarSize
{
	eAvatarSmall,
	eAvatarMedium,
	eAvatarLarge
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CAvatarImage : public vgui::IImage
{
public:
	CAvatarImage( void );
	virtual ~CAvatarImage();

	// Call this to set the steam ID associated with the avatar
#ifndef NO_STEAM
	bool SetAvatarSteamID( CSteamID steamIDUser );
#endif
	void UpdateFriendStatus( void );
	void ClearAvatarSteamID( void );

	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint( void );

	// Set the position of the image
	virtual void SetPos(int x, int y)
	{
		m_nX = x;
		m_nY = y;
	}

	// Gets the size of the content
	virtual void GetContentSize(int &wide, int &tall)
	{
		wide = m_nWide;
		tall = m_nTall;
	}

	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int &wide, int &tall)
	{
		GetContentSize( wide, tall );
	}

	// Sets the size of the image
	virtual void SetSize(int wide, int tall)	
	{ 
		m_nWide = wide; 
		m_nTall = tall; 
	}

	void SetAvatarSize(int wide, int tall)	
	{
		m_iAvatarWidth = wide;
		m_iAvatarHeight = tall;
		SetSize( wide + AVATAR_INDENT_X + AVATAR_POSTDENT_X, tall + AVATAR_INDENT_Y + AVATAR_POSTDENT_Y );
	}

	// Set the draw color 
	virtual void SetColor(Color col)			
	{ 
		m_Color = col; 
	}

#ifndef NO_STEAM
	void SetAvatarSize( int size )
	{
		m_SourceArtSize = size;
	}
#endif

	bool IsValid( void ) { return m_bValid; }

	virtual bool Evict() { return false; }
	virtual int GetNumFrames() { return 0; }
	virtual void SetFrame( int nFrame ) {}
	virtual vgui::HTexture GetID() { return m_iTextureID; }
	virtual void SetRotation( int iRotation ) { return; }
	int		GetWide( void ) { return m_nWide; }

protected:
	void InitFromRGBA( const byte *rgba, int width, int height );

private:
#ifndef NO_STEAM
	CSteamID m_steamIDUser;
#endif
	Color m_Color;
	int m_iTextureID;
	int m_nX, m_nY, m_nWide, m_nTall;
	bool m_bValid;
	bool m_bFriend;
	CHudTexture *m_pFriendIcon;
	int	 m_iAvatarWidth;
	int	 m_iAvatarHeight;
#ifndef NO_STEAM
	CSteamID	m_SteamID;
	int			m_SourceArtSize;
#endif
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CAvatarImagePanel : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CAvatarImagePanel, vgui::ImagePanel );

	CAvatarImagePanel( vgui::Panel *parent, const char *name );

	// Set the player that this Avatar should display for
	void SetPlayer( C_BasePlayer *pPlayer );
	void SetPlayerByIndex( int iIndex );
#ifndef NO_STEAM
	void SetAvatarBySteamID( CSteamID *friendsID );
#endif

	virtual void PaintBackground( void );
	bool	IsValid( void ) { return (GetImage() && ((CAvatarImage*)GetImage())->IsValid()); }

protected:
	CPanelAnimationVar( Color, m_clrOutline, "color_outline", "Black" );
};

#endif // VGUI_AVATARIMAGE_H
