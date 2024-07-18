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
		
	CRadialButton( vgui::Panel *parent, const char *panelName );
	~CRadialButton();

	//RadialMenu required functions

	void ShowSubmenuIndicator( bool state );

	void SetPassthru( CRadialButton *button );
	void SetImage( const char* lpszImage );

	void GetImagePos( int &, int & );

	// Return the icon image panel of the button
	vgui::ImagePanel *GetIcon() { return m_pIcon; }
	
	CRadialButton *GetPassthru( void );

	void UpdateHotspots( KeyValues *data );
	
	Color GetRadialFgColor( void );
	Color GetRadialBgColor( void );	

	// Set the maximum scale of the button
	void SetMaxScale(float flMaxScale) { m_fMaxScale = flMaxScale; }
	
	void GetHotspotBounds( int *minX, int *minY, int *maxX, int *maxY );
		
	void CRadialButton::PerformLayout( void );	
	virtual void PaintBackground( void );
	virtual void PaintBorder( void );
	virtual void Paint( void );
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

	void SetChosen( bool chosen );
	void SetFakeArmed( bool armed );

	bool IsFakedArmed( void ) { return m_fakeArmed; }

	virtual void OnCursorEntered( void );

	//----------------------------------------------------------------------------------------------------
	/**
	 * Right-click cancels, otherwise, the normal button logic applies
	 */
	virtual void OnMousePressed( vgui::MouseCode code );

	bool IsChosen( void ) { return m_chosen; }	

	// Set whether the button should pulse
	void SetPulse(bool bPulse) { m_bPulse = bPulse; }

	// Stub for GetGLaDOSResponse, implement this whenever you can, this isnt a very important function atm
	int GetGLaDOSResponse();

	// Stub for SetGLaDOSResponse
	void SetGLaDOSResponse( int nResponse );

private:
	CUtlVector< Vector2D > m_unscaledSubmenuPoints;
	vgui::Vertex_t *m_submenuPoints; // 154
	int m_numSubmenuPoints; // 155

	int m_nMainMaterial;
	bool m_hasBorders;
	
	Color m_disabledBgColor;
	Color m_disabledFgColor;

	Color m_chosenBgColor;
	Color m_chosenFgColor;
	bool m_chosen; // 645

	Color m_armedBgColor;
	Color m_armedFgColor;
	bool m_fakeArmed; // 654

	bool m_hasSubmenu; // 655?

	float m_fMaxScale; // 164
	bool m_bPulse; // 660

	int m_nGLaDOSResponse; // 166

	vgui::HFont m_armedFont; // 167
	vgui::HFont m_defaultFont; // 168

	CRadialButton *m_passthru; // 169
	CRadialMenu *m_parent; // 170

	vgui::ImagePanel *m_pIcon; // 171
};



void OpenRadialMenu( const char *menuName );
void CloseRadialMenu( const char *menuName, bool sendCommand = false );
bool IsRadialMenuOpen( const char *menuName, bool includeFadingOut );


#endif // RADIALBUTTON_H
