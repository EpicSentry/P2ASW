// RadialMenu.h
// Copyright (c) 2006 Turtle Rock Studios, Inc.

#ifndef RADIALBUTTON_H
#define RADIALBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/editablepanel.h>
//#include "asw_hudelement.h"
#include "hudelement.h"
#include "portal2_vgui_ingame_panel.h"
//#include "vgui/polygonbutton.h"
#include "polygonbutton.h"


#define PANEL_RADIAL_MENU			"RadialMenu"

// NOTE: I copied CRadialButton from swarm's radialmenu.cpp file, which doesn't exist in Portal 2's, but we still need to modify it

//--------------------------------------------------------------------------------------------------------
/**
 * A radial menu-specific subclass of CPolygonButton
 */
class CRadialButton : public CPolygonButton, public CPortal2_VGUI_Ingame_Panel
{
	DECLARE_CLASS_SIMPLE( CRadialButton, CPolygonButton );

public:

	CRadialButton( vgui::Panel *parent, const char *panelName );

	void ShowSubmenuIndicator( bool state );
	void SetPassthru( CRadialButton *button );

	CRadialButton *GetPassthru( void );

	void UpdateHotspots( KeyValues *data );
	void CRadialButton::PerformLayout( void );

	Color GetRadialFgColor( void );
	Color GetRadialBgColor( void );	

	void GetHotspotBounds( int *minX, int *minY, int *maxX, int *maxY );
	
	virtual void PaintBackground( void );
	virtual void PaintBorder( void );
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

	bool m_hasSubmenu;

	vgui::HFont m_armedFont;
	vgui::HFont m_defaultFont;

	CRadialButton *m_passthru;
	CRadialMenu *m_parent;
};



void OpenRadialMenu( const char *menuName );
void CloseRadialMenu( const char *menuName, bool sendCommand = false );
bool IsRadialMenuOpen( const char *menuName, bool includeFadingOut );


#endif // RADIALBUTTON_H
