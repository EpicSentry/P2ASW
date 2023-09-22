//======== Copyright © 2013-2015, Stan Riders aka Strider[RUS], All rights reserved. ===========//
//
// Purpose: HL2Port UI-related defines.
#ifndef __UIDEFINES_H__
#define __UIDEFINES_H__
//==============================================================================================//

// --- Comment to disable.

#define UI_USING_LOGO									// logo
//#define UI_USING_OLDDIALOGS							// implement?
#define UI_USING_RANDOMMENUMOVIES						// random menu backgound movies
#define UI_USING_RANDOMLOADINGBGS						// random game loading backgrounds
//#define UI_USING_LOADINGSPINNER						// spinning texture in corner when game is loading
//#define UI_USING_TRANSITIONBG							// using background image in transition between levels
#define UI_USING_DEVCOMMENTARIES						// implement!!!
#define UI_USING_GAMEPLAYCONFIGDIALOG					// "gameplay" menu
//#define UI_USING_MAINMENUMUSIC						// main menu background music ("Misc.MainUI" by default)

// --- UI colors (random shades of gray for now)
#define UI_STYLE_ENABLED5			169, 169, 169, 255	// style5
#define UI_STYLE_ENABLED11			170, 170, 170, 255	// style11
#define UI_STYLE_ENABLED			125, 125, 125, 255	// generic
#define UI_STYLE_DISABLED			59, 59, 59, 255
#define UI_STYLE_FOCUSDISABLED		182, 182, 182, 255
#define UI_STYLE_OPEN				200, 200, 200, 255	// flyout menu is attached
#define UI_STYLE_FOCUS				255, 255, 255, 255	// active item

#define UI_STYLE_BACKGROUNDFILL		53, 53, 53, 255		// 53, 86, 117, 255
#define UI_STYLE_HIGHLIGHTS			210, 210, 210, 255	// 97, 210, 255, 255

#define UI_STYLE_BUTTON_OUTLINE		94, 94, 94, 255		// 78, 94, 110, 255
#define UI_STYLE_BUTTON_ARMED		59, 59, 59, 255		// 20, 59, 96, 255
#define UI_STYLE_BUTTON_ENABLED		43, 43, 43, 255		// 24, 43, 66, 255
#define UI_STYLE_BUTTON_BACKGROUND	78, 78, 78, 255		// 65, 78, 91, 255
#define UI_STYLE_BUTTON_CENTER		80, 80, 80, 255		// 28, 80, 130, 255

#define UI_STYLE_NB_TITLE_MEDIUM	79, 79, 79, 255		// 47, 79, 111, 255

#define UI_STYLE_FOOTER_GRADIENT	35, 35, 35, 255		// 19, 35, 65, 255
#define UI_STYLE_FOOTER_GRALINE		61, 61, 61, 255		// 35, 61, 87, 255

// --- Random Main Menu Movies
static const char *g_ppszRandomMenuMovies[] = 
{
#ifdef USING_HL2EM
	"media/background.bik",
#endif
	"media/bg_03.bik",
	"media/bg_02.bik",
	"media/bg_04.bik",
	"media/bg_01.bik",
};

// --- Random Loading Backgrounds (all path are inherited from materials/vgui/)
static const char g_ppszRandomLoadingBackgrounds[][64] = 
{
	"swarm/loading/BGFX01",
	"../console/background01",
	"../console/background02",
	"../console/background03",
	"../console/background04"
};
// !!! WIDESCREEN ARRAY SHOULD ALWAYS BE THE SAME SIZE AS REGULAR ONE
static const char g_ppszRandomLoadingBackgrounds_widescreen[][64] = 
{
	"swarm/loading/BGFX01_wide",
	"../console/background01_widescreen",
	"../console/background02_widescreen",
	"../console/background03_widescreen",
	"../console/background04_widescreen"
};

// --- !NON!-Random Loading Backgrounds
#define UI_DEFAULT_LOADINGBACKGROUND "swarm/loading/BGFX01"
#define UI_DEFAULT_LOADINGBACKGROUND_WIDE "swarm/loading/BGFX01_wide"
//=============================================================================

// ---
// Dashing through the snow
// In a one-horse open sleigh
// O’er the fields we go
// Laughing all the way
// Bells on bobtail ring
// Making spirits bright
// What fun it is to laugh and sing
// A sleighing song tonight!
//
// Jingle bells, jingle bells,
// Jingle all the way.
// Oh! what fun it is to ride
// In a one-horse open sleigh.
//
// Jingle bells, jingle bells,
// Jingle all the way;
// Oh! what fun it is to ride
// In a one-horse open sleigh.
// ---
// Happy NY, coder. 
// [str], 31st of December, 23:14.

#endif