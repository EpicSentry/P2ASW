//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"
#include "iclientmode.h"
#include <vgui_controls/editablepanel.h>
#include <vgui_controls/imagepanel.h>
#include <vgui/ivgui.h>
#include <vgui/isurface.h>
#include "view.h"
#include "debugoverlay_shared.h"
#include "fmtstr.h"
#include "hudelement.h"

#include "C_Portal_Player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar cl_hud_show_paint_indicator( "cl_hud_show_paint_indicator", "0", FCVAR_DEVELOPMENTONLY );
ConVar cl_hud_paint_indicator_fade_time( "cl_hud_paint_indicator_fade_time", "0.5f", FCVAR_DEVELOPMENTONLY );
ConVar cl_hud_paint_indicator_start_alpha( "cl_hud_paint_indicator_start_alpha", "200", FCVAR_DEVELOPMENTONLY );
ConVar cl_hud_paint_indicator_show_outline( "cl_hud_paint_indicator_show_outline", "1", FCVAR_DEVELOPMENTONLY );
ConVar cl_hud_debug_paint_indicator( "cl_hud_debug_paint_indicator", "0", FCVAR_DEVELOPMENTONLY );

enum PaintIndicatorLocation
{
	PAINT_LOCATION_LEFT = 0,
	PAINT_LOCATION_RIGHT,
	PAINT_LOCATION_TOP,
	PAINT_LOCATION_BOTTOM,
	PAINT_LOCATION_FORWARD,
	PAINT_LOCATION_BEHIND
};

struct PaintIndicatorInfo_t
{
	PaintIndicatorLocation location;
	Vector paintPosition;
	Vector paintNormal;
	PaintPowerType paintType;
	float fadeTimer;
	bool shouldUpdate;
};

enum IndicatorScreenLocation
{
	SCREEN_LEFT = 0,
	SCREEN_RIGHT,
	SCREEN_TOP,
	SCREEN_BOTTOM,

	NUM_SCREEN_LOCATIONS
};

typedef CUtlVector<PaintIndicatorInfo_t> PaintIndicatorInfoVector;

class CHUDPaintIndicator : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHUDPaintIndicator, EditablePanel );
public:
	CHUDPaintIndicator( const char *pElementName );

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	virtual void ApplySchemeSettings( IScheme *pScheme );

	virtual bool ShouldDraw( void );

	virtual void Paint( void );

	virtual void OnTick( void );

private:
	void UpdatePaintLocations( float flDeltaTime );
	void AddPaintLocation( const Vector &vecPaintPosition, const Vector &vecPaintNormal, PaintPowerType paintType );
	void DrawPaintIndicator( const PaintIndicatorInfo_t &indicatorInfo );
	void DrawPaintIndicatorQuad( IndicatorScreenLocation screenLocation, Color indicatorColor, PaintPowerType paintType );
	void ResetDrawnLocationsCounters( void );

	CMaterialReference m_WhiteAdditiveMaterial;

	PaintIndicatorInfoVector m_PaintLocations;

	float m_flLastTickTime;

	int m_NumIndicatorsDrawn[NUM_SCREEN_LOCATIONS];
	bool m_bPaintIndicatorDrawn[NUM_SCREEN_LOCATIONS][PAINT_POWER_TYPE_COUNT];

	Vector m_vecPlayerUp;
};

DECLARE_HUDELEMENT( CHUDPaintIndicator );

CHUDPaintIndicator::CHUDPaintIndicator( const char *pElementName )
			: CHudElement( pElementName ), BaseClass( NULL, "PaintIndicator" ),
			  m_vecPlayerUp( vec3_origin )
{
	SetParent( GetClientMode()->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/ClientScheme.res", "PaintScheme");
	SetScheme( scheme );

	m_WhiteAdditiveMaterial.Init( "vgui/white", TEXTURE_GROUP_VGUI ); 

	vgui::ivgui()->AddTickSignal( GetVPanel(), 0 );

	m_flLastTickTime = gpGlobals->curtime;
}


bool CHUDPaintIndicator::ShouldDraw( void )
{
	if( !cl_hud_show_paint_indicator.GetBool() )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}


void CHUDPaintIndicator::OnTick( void )
{
	if( m_flLastTickTime > gpGlobals->curtime )
	{
		m_flLastTickTime = gpGlobals->curtime;
	}
	float flDeltaTime = gpGlobals->curtime - m_flLastTickTime;
	UpdatePaintLocations( flDeltaTime );
	m_flLastTickTime = gpGlobals->curtime;

	C_Portal_Player *pPlayer = C_Portal_Player::GetLocalPortalPlayer();

	if( pPlayer )
	{
		const C_PortalPlayerLocalData &playerData = pPlayer->GetPortalPlayerLocalData();
		m_vecPlayerUp = playerData.m_Up;

		//Add all the valid paint power locations
		for( int i = 0; i < PAINT_POWER_TYPE_COUNT; ++i )
		{
			if( playerData.m_PaintPowerHudInfoValidity[i] )
			{
				AddPaintLocation( playerData.m_PaintPowerHudInfoPosition[i], playerData.m_PaintPowerHudInfoNormal[i], (PaintPowerType)( i ) );
			}
		}
	}
}


void CHUDPaintIndicator::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetPaintBackgroundEnabled( false );

	int wide, tall;
	GetHudSize( wide, tall );
	SetSize( wide, tall );
}


void CHUDPaintIndicator::Init( void )
{
	Reset();
}


void CHUDPaintIndicator::Reset( void )
{
}


void CHUDPaintIndicator::VidInit( void )
{
	Reset();
}


void CHUDPaintIndicator::Paint( void )
{
	ResetDrawnLocationsCounters();

	for( int i = 0; i < m_PaintLocations.Count(); ++i )
	{
		DrawPaintIndicator( m_PaintLocations[i] );
	}
}

void CHUDPaintIndicator::ResetDrawnLocationsCounters( void )
{
	for( int i = 0; i < NUM_SCREEN_LOCATIONS; ++i )
	{
		m_NumIndicatorsDrawn[i] = 0;

		for( int j = 0; j < PAINT_POWER_TYPE_COUNT; ++j )
		{
			m_bPaintIndicatorDrawn[i][j] = false;
		}
	}
}

void CHUDPaintIndicator::DrawPaintIndicator( const PaintIndicatorInfo_t &indicatorInfo )
{
	//Get the color for the indicator
	Color indicatorColor = MapPowerToVisualColor( indicatorInfo.paintType );
	indicatorColor[3] = Lerp( indicatorInfo.fadeTimer/cl_hud_paint_indicator_fade_time.GetFloat(), 0.0f, cl_hud_paint_indicator_start_alpha.GetFloat() );

	//Draw the indicators at the corresponding screen locations
	switch( indicatorInfo.location )
	{
	case PAINT_LOCATION_LEFT:
		{
			DrawPaintIndicatorQuad( SCREEN_LEFT, indicatorColor, indicatorInfo.paintType );
		}
		break;
	case PAINT_LOCATION_RIGHT:
		{
			DrawPaintIndicatorQuad( SCREEN_RIGHT, indicatorColor, indicatorInfo.paintType );
		}
		break;
	case PAINT_LOCATION_TOP:
		{
			DrawPaintIndicatorQuad( SCREEN_TOP, indicatorColor, indicatorInfo.paintType );
		}
		break;
	case PAINT_LOCATION_BOTTOM:
		{
			DrawPaintIndicatorQuad( SCREEN_BOTTOM, indicatorColor, indicatorInfo.paintType );
		}
		break;
	case PAINT_LOCATION_FORWARD:
		{
			//DrawPaintIndicatorQuad( SCREEN_LEFT, indicatorColor, indicatorInfo.paintType );
			//DrawPaintIndicatorQuad( SCREEN_TOP, indicatorColor, indicatorInfo.paintType );
			//DrawPaintIndicatorQuad( SCREEN_RIGHT, indicatorColor, indicatorInfo.paintType );
			DrawPaintIndicatorQuad( SCREEN_BOTTOM, indicatorColor, indicatorInfo.paintType );
		}
		break;
	case PAINT_LOCATION_BEHIND:
		{
			//DrawPaintIndicatorQuad( SCREEN_LEFT, indicatorColor, indicatorInfo.paintType );
			DrawPaintIndicatorQuad( SCREEN_TOP, indicatorColor, indicatorInfo.paintType );
			//DrawPaintIndicatorQuad( SCREEN_RIGHT, indicatorColor, indicatorInfo.paintType );
			//DrawPaintIndicatorQuad( SCREEN_BOTTOM, indicatorColor, indicatorInfo.paintType );
		}
		break;
	}
}


void CHUDPaintIndicator::DrawPaintIndicatorQuad( IndicatorScreenLocation screenLocation, Color indicatorColor, PaintPowerType paintType )
{
	if( m_bPaintIndicatorDrawn[screenLocation][paintType] )
	{
		m_NumIndicatorsDrawn[screenLocation]--;
		//return;
	}

	m_bPaintIndicatorDrawn[screenLocation][paintType] = true;

	CMatRenderContextPtr pRenderContext( materials );
	IMesh *pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, m_WhiteAdditiveMaterial );

	int x[4] = {0};
	int y[4] = {0};

	int indicatorWidth = YRES( 15 );
	int indicatorHeight = YRES( 400 );
	int inset = YRES( 15 );
	int distanceFromScreenEdge = YRES( 10 );

	switch( screenLocation )
	{
	case SCREEN_LEFT:
		{
			x[0] = distanceFromScreenEdge + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[1] = distanceFromScreenEdge + indicatorWidth + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[2] = distanceFromScreenEdge + indicatorWidth + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[3] = distanceFromScreenEdge + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );

			int yStart = ( GetTall() / 2 ) - ( indicatorHeight / 2 );
			y[0] = yStart + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[1] = yStart + inset + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[2] = yStart + indicatorHeight - inset - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[3] = yStart + indicatorHeight - ( inset * m_NumIndicatorsDrawn[screenLocation] );
		}
		break;
	case SCREEN_RIGHT:
		{
			x[0] = GetWide() - distanceFromScreenEdge - indicatorWidth - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[1] = GetWide() - distanceFromScreenEdge - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[2] = GetWide() - distanceFromScreenEdge - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			x[3] = GetWide() - distanceFromScreenEdge - indicatorWidth - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );

			int yStart = ( GetTall() / 2 ) - ( indicatorHeight / 2 );
			y[0] = yStart + inset + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[1] = yStart + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[2] = yStart + indicatorHeight - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			y[3] = yStart + indicatorHeight - inset - ( inset * m_NumIndicatorsDrawn[screenLocation] );
		}
		break;
	case SCREEN_TOP:
		{
			int xStart = ( GetWide() / 2 ) - ( indicatorHeight / 2 );
			x[0] = xStart + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[1] = xStart + indicatorHeight - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[2] = xStart + indicatorHeight - inset - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[3] = xStart + inset + ( inset * m_NumIndicatorsDrawn[screenLocation] );

			y[0] = distanceFromScreenEdge + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[1] = distanceFromScreenEdge + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[2] = distanceFromScreenEdge + indicatorWidth + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[3] = distanceFromScreenEdge + indicatorWidth + ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
		}
		break;
	case SCREEN_BOTTOM:
		{
			int xStart = ( GetWide() / 2 ) - ( indicatorHeight / 2 );
			x[0] = xStart + inset + ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[1] = xStart + indicatorHeight - inset - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[2] = xStart + indicatorHeight - ( inset * m_NumIndicatorsDrawn[screenLocation] );
			x[3] = xStart + ( inset * m_NumIndicatorsDrawn[screenLocation] );

			y[0] = GetTall() - distanceFromScreenEdge - indicatorWidth - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[1] = GetTall() - distanceFromScreenEdge - indicatorWidth - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[2] = GetTall() - distanceFromScreenEdge - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
			y[3] = GetTall() - distanceFromScreenEdge - ( indicatorWidth * m_NumIndicatorsDrawn[screenLocation] );
		}
		break;
	}

	float zpos = surface()->GetZPos();

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	//Top left
	meshBuilder.Color4ub( indicatorColor.r(), indicatorColor.g(), indicatorColor.b(), indicatorColor.a() );
	meshBuilder.TexCoord2f( 0, 0, 0 );
	meshBuilder.Position3f( x[0], y[0], zpos );
	meshBuilder.AdvanceVertex();

	//Top right
	meshBuilder.Color4ub( indicatorColor.r(), indicatorColor.g(), indicatorColor.b(), indicatorColor.a() );
	meshBuilder.TexCoord2f( 0, 1, 0 );
	meshBuilder.Position3f( x[1], y[1], zpos );
	meshBuilder.AdvanceVertex();

	//Bottom right
	meshBuilder.Color4ub( indicatorColor.r(), indicatorColor.g(), indicatorColor.b(), indicatorColor.a() );
	meshBuilder.TexCoord2f( 0, 1, 1 );
	meshBuilder.Position3f( x[2], y[2], zpos );
	meshBuilder.AdvanceVertex();

	//Bottom left
	meshBuilder.Color4ub( indicatorColor.r(), indicatorColor.g(), indicatorColor.b(), indicatorColor.a() );
	meshBuilder.TexCoord2f( 0, 0, 1 );
	meshBuilder.Position3f( x[3], y[3], zpos );
	meshBuilder.AdvanceVertex();

	//Draw the indicator
	meshBuilder.End();
	pMesh->Draw();

	if( cl_hud_paint_indicator_show_outline.GetBool() )
	{
		Color outlineColor( 0, 0, 0, indicatorColor.a() );

		//Draw an outline
		CMeshBuilder lineBuilder;
		lineBuilder.Begin( pMesh, MATERIAL_LINE_LOOP, 4 );

		lineBuilder.Color4ub( outlineColor.r(), outlineColor.g(), outlineColor.b(), outlineColor.a() );
		lineBuilder.Position3f( x[0], y[0], zpos );
		lineBuilder.AdvanceVertex();

		lineBuilder.Color4ub( outlineColor.r(), outlineColor.g(), outlineColor.b(), outlineColor.a() );
		lineBuilder.Position3f( x[1], y[1], zpos );
		lineBuilder.AdvanceVertex();

		lineBuilder.Color4ub( outlineColor.r(), outlineColor.g(), outlineColor.b(), outlineColor.a() );
		lineBuilder.Position3f( x[2], y[2], zpos );
		lineBuilder.AdvanceVertex();

		lineBuilder.Color4ub( outlineColor.r(), outlineColor.g(), outlineColor.b(), outlineColor.a() );
		lineBuilder.Position3f( x[3], y[3], zpos );
		lineBuilder.AdvanceVertex();

		lineBuilder.End();
		pMesh->Draw();
	}

	//Update the counter for the number of indicators drawn at this location
	m_NumIndicatorsDrawn[screenLocation]++;
}


PaintIndicatorLocation GetPaintLocationOnScreen( const Vector &vecPaintPosition, const Vector &vecPaintNormal, PaintSurfaceType surfaceType )
{
	ASSERT_LOCAL_PLAYER_RESOLVABLE();
	int nSlot = GET_ACTIVE_SPLITSCREEN_SLOT();
	float flAngle;

	//Player data
	Vector vecPlayerPosition = MainViewOrigin( nSlot );
	QAngle angPlayerAngles = MainViewAngles( nSlot );

	Vector vecDelta = vecPaintPosition - vecPlayerPosition;
	float flRadius = 500.0f;

	Vector vecViewForward, vecViewRight, vecViewUp;
	AngleVectors( angPlayerAngles, &vecViewForward, &vecViewRight, &vecViewUp );
	vecViewForward.z = 0.0f;
	VectorNormalize( vecViewForward );
	VectorNormalize( vecViewUp );
	CrossProduct( vecViewUp, vecViewForward, vecViewRight );
	float front = DotProduct( vecDelta, vecViewForward );
	float side = DotProduct( vecDelta, vecViewRight );
	float xPos = flRadius * -side;
	float yPos = flRadius * -front;

	//Get the rotation (yaw)
	flAngle = atan2( xPos, yPos ) + M_PI;
	flAngle *= 180 / M_PI;

	float yawRadians = -( flAngle ) * M_PI / 180.0f;
	float ca = cos( yawRadians );
	float sa = sin( yawRadians );

	//Rotate it around the circle
	xPos = (int)( ( ScreenWidth() / 2.0f ) + ( flRadius * sa ) );
	yPos = (int)( ( ScreenHeight() / 2.0f ) + ( flRadius * ca ) );

	if( cl_hud_debug_paint_indicator.GetBool() )
	{
		CFmtStr msg;
		msg.sprintf( "Angle: %f, XPos: %f, YPos: %f\n", flAngle, xPos, yPos );
		NDebugOverlay::ScreenText( 0.5f, 0.5f, msg, 255, 255, 0, 255, 0.0f );
		NDebugOverlay::ScreenText( xPos/(float)ScreenWidth(), yPos/(float)ScreenHeight(), "0", 255, 0, 255, 255, 0.0f );
		NDebugOverlay::Sphere( vecPaintPosition, 5.0f, 255, 0, 255, false, 0.0f );
		NDebugOverlay::VertArrow( vecPaintPosition, vecPaintPosition + ( vecPaintNormal * 50.0f ), 5.0f, 255, 255, 0, 255, false, 0.0f );
	}

	//Figure out the location of the paint on the screen based on the angle and the surface type
	if( surfaceType == FLOOR_SURFACE )
	{
		return PAINT_LOCATION_BOTTOM;
	}
	else if( surfaceType == CEILING_SURFACE )
	{
		return PAINT_LOCATION_TOP;
	}
	else if( surfaceType == WALL_SURFACE )
	{
		if( flAngle > 320.0f || flAngle < 40.0f )
		{
			return PAINT_LOCATION_FORWARD;
		}
		else if( flAngle > 130.0f && flAngle < 230.0f )
		{
			return PAINT_LOCATION_BEHIND;
		}
		else if( flAngle < 130.0f )
		{
			return PAINT_LOCATION_LEFT;
		}
		else if( flAngle > 230.0f )
		{
			return PAINT_LOCATION_RIGHT;
		}
	}

	return PAINT_LOCATION_BOTTOM;
}


PaintSurfaceType GetPaintSurfaceType( const Vector &vecPaintNormal, const Vector &vecPlayerUp )
{
	PaintSurfaceType surfaceType;

	float flDot = DotProduct( vecPaintNormal, vecPlayerUp );

	if( flDot > STEEP_SLOPE )
	{
		surfaceType = FLOOR_SURFACE;
	}
	else if( flDot < -STEEP_SLOPE )
	{
		surfaceType = CEILING_SURFACE;
	}
	else
	{
		surfaceType = WALL_SURFACE;
	}

	return surfaceType;
}


void CHUDPaintIndicator::AddPaintLocation( const Vector &vecPaintPosition, const Vector &vecPaintNormal, PaintPowerType paintType )
{
	PaintSurfaceType surfaceType = GetPaintSurfaceType( vecPaintNormal, m_vecPlayerUp );

	//Don't detect speed paint on walls
	if( surfaceType == WALL_SURFACE && paintType == SPEED_POWER )
	{
		return;
	}

	bool bAddToList = true;
	//Check for duplicate entries in the list
	for( int i = 0; i < m_PaintLocations.Count(); ++i )
	{
		//If there is already an indicator of the same color at this location
		if( m_PaintLocations[i].paintType == paintType &&
			AlmostEqual( m_PaintLocations[i].paintNormal, vecPaintNormal ) )
		{
			//Reset the fade timer
			m_PaintLocations[i].fadeTimer = cl_hud_paint_indicator_fade_time.GetFloat();
			m_PaintLocations[i].shouldUpdate = true;

			//Update the position of the paint
			m_PaintLocations[i].paintPosition = vecPaintPosition;
			m_PaintLocations[i].paintNormal = vecPaintNormal;

			//Don't add the new info to the list
			bAddToList = false;
			break;
		}
	}

	//This is a new paint location
	if( bAddToList )
	{
		PaintIndicatorInfo_t indicatorInfo;

		indicatorInfo.paintPosition = vecPaintPosition;
		indicatorInfo.paintNormal = vecPaintNormal;
		indicatorInfo.location = GetPaintLocationOnScreen( vecPaintPosition, vecPaintNormal, surfaceType );
		indicatorInfo.paintType = paintType;
		indicatorInfo.fadeTimer = cl_hud_paint_indicator_fade_time.GetFloat();
		indicatorInfo.shouldUpdate = true;

		m_PaintLocations.AddToTail( indicatorInfo );
	}
}


void CHUDPaintIndicator::UpdatePaintLocations( float flDeltaTime )
{
	int i = 0;

	while( i < m_PaintLocations.Count() )
	{
		//Fade out the paint indicator
		m_PaintLocations[i].fadeTimer -= flDeltaTime;

		if( m_PaintLocations[i].shouldUpdate )
		{
			//Update the position of the paint on the screen
			PaintSurfaceType surfaceType = GetPaintSurfaceType( m_PaintLocations[i].paintNormal, m_vecPlayerUp );
			m_PaintLocations[i].location = GetPaintLocationOnScreen( m_PaintLocations[i].paintPosition, m_PaintLocations[i].paintNormal, surfaceType );
			m_PaintLocations[i].shouldUpdate = false;
		}

		//Remove paint locations that have been completely faded out
		if( m_PaintLocations[i].fadeTimer <= 0.0f || m_PaintLocations[i].fadeTimer > cl_hud_paint_indicator_fade_time.GetFloat() )
		{
			m_PaintLocations.Remove( i );
		}
		else
		{
			++i;
		}
	}
}
