// RadialMenu.h
// Copyright (c) 2006 Turtle Rock Studios, Inc.

#ifndef RADIALBUTTON_H
#define RADIALBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/editablepanel.h>
#include <vgui_controls/ImagePanel.h>
//#include "asw_hudelement.h"
#include "hudelement.h"
#include "portal2_vgui_ingame_panel.h"
#include "polygonbutton.h"

#ifndef PANEL_RADIAL_MENU
#define PANEL_RADIAL_MENU			"RadialMenu"
#endif
// NOTE: I copied CRadialButton from swarm's radialmenu.cpp file, which doesn't exist in Portal 2's leak, so we need to modify it

//--------------------------------------------------------------------------------------------------------
/**
 * A radial menu-specific subclass of CPolygonButton
 */
class CRadialButton : public CPolygonButton
{
	DECLARE_CLASS_SIMPLE( CRadialButton, CPolygonButton );

public:

	//RadialMenu required functions

	// Return the icon image panel of the button
	vgui::ImagePanel *GetIcon() { return m_pIcon; }

	// Set the maximum scale of the button
	void SetMaxScale(float flMaxScale) { m_flMaxScale = flMaxScale; }

	// Stub for GetGLaDOSResponse, implement this whenever you can, this isnt a very important function atm
	int GetGLaDOSResponse();

	// Stub for SetGLaDOSResponse
	void SetGLaDOSResponse( int nResponse );

	// Set whether the button should pulse
	void SetPulse(bool bPulse) { m_bPulse = bPulse; }

	CRadialButton( vgui::Panel *parent, const char *panelName );

	void ShowSubmenuIndicator( bool state );
	void SetPassthru( CRadialButton *button );

	CRadialButton *GetPassthru( void );

	void UpdateHotspots( KeyValues *data );
	void CRadialButton::PerformLayout( void );

	Color GetRadialFgColor( void );
	Color GetRadialBgColor( void );	

	void SetImage( const char* lpszImage );

	void GetHotspotBounds( int *minX, int *minY, int *maxX, int *maxY );
	
	virtual void PaintBackground( void );
	virtual void PaintBorder( void );
	virtual void Paint( void );
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

	void SetChosen( bool chosen );
	void SetFakeArmed( bool armed );

	virtual void OnCursorEntered( void );

	//----------------------------------------------------------------------------------------------------
	/**
	 * Right-click cancels, otherwise, the normal button logic applies
	 */
	virtual void OnMousePressed( vgui::MouseCode code );

	virtual bool MouseClick( int x, int y, bool bRightClick, bool bDown );

private:
	CUtlVector< Vector2D > m_unscaledSubmenuPoints;
	vgui::Vertex_t *m_submenuPoints;
	int m_numSubmenuPoints;

	int m_nMainMaterial;
	bool m_hasBorders;

	Color m_disabledBgColor;
	Color m_disabledFgColor;

	Color m_chosenBgColor;
	Color m_chosenFgColor;
	bool m_chosen;

	Color m_armedBgColor;
	Color m_armedFgColor;
	bool m_fakeArmed;

	vgui::ImagePanel *m_pIcon;
	float m_flMaxScale;
	bool m_bPulse;

	bool m_hasSubmenu;

	vgui::HFont m_armedFont;
	vgui::HFont m_defaultFont;

	CRadialButton *m_passthru;
	CRadialMenu *m_parent;

	int m_nGladosResponse;

};



void OpenRadialMenu( const char *menuName );
void CloseRadialMenu( const char *menuName, bool sendCommand = false );
bool IsRadialMenuOpen( const char *menuName, bool includeFadingOut );


#endif // RADIALBUTTON_H
