//========= Copyright � 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef __UIAVATARIMAGE_H__
#define __UIAVATARIMAGE_H__

#include "vgui/IImage.h"

#ifndef NO_STEAM
#include "steam/steam_api.h"
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CGameUiAvatarImage : public vgui::IImage
{
public:
	CGameUiAvatarImage( void );
	virtual ~CGameUiAvatarImage( void );

	enum AvatarSize_t
	{
		SMALL = 0,
		MEDIUM,
		LARGE
	};

	// Call this to set the steam ID associated with the avatar
	bool SetAvatarXUID( XUID xuid, AvatarSize_t nSize = MEDIUM );
	void ClearAvatarXUID( void );
	AvatarSize_t GetAvatarSize( void ) const { return m_nAvatarSize; }

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

	// Set the draw color 
	virtual void SetColor(Color col)			
	{ 
		m_Color = col; 
	}

	virtual void SetRotation( int )
	{
		( void ) 0;		// Not implemented
	}

	virtual bool Evict() { return false; }

	virtual int GetNumFrames() { return 0; }
	virtual void SetFrame( int nFrame ) {}

	virtual vgui::HTexture GetID() { return m_iTextureID; }

	bool	IsValid( void ) { return m_bValid; }

	float	GetFetchedTime() const { return m_flFetchedTime; }

	int		AdjustRefCount( int nAdjustment )
	{
		m_nRefcount += nAdjustment;
		int nReturnValue = m_nRefcount;
		if ( nReturnValue <= 0 )
			OnFinalRelease();
		return nReturnValue;
	}
	virtual void OnFinalRelease();

protected:
	void InitFromRGBA( const byte *rgba, int width, int height );

private:
	Color m_Color;
	int m_iTextureID;
	int m_nX, m_nY, m_nWide, m_nTall;
	bool m_bValid;
	float m_flFetchedTime;
	float m_flPaintedTime;
	int m_nRefcount;
	CUtlBuffer m_bufRgbaBuffer;
	AvatarSize_t m_nAvatarSize;

#ifdef _X360
	enum AsyncState_t
	{
		STATE_DEFAULT,
		STATE_AWAITING_KEY,
		STATE_AWAITING_RGBA,
		STATE_COMPLETE
	}
	m_eAsyncState;
	XOVERLAPPED m_xOverlapped;
	CUtlBuffer m_xBufKey;
	XUID m_xXUID;
	DWORD m_xSetting;
	DWORD m_xCbResult;
	void X360_UpdateImageState();
	void X360_ResetAsyncImageState();
#endif
};



#endif // __UIAVATARIMAGE_H__
