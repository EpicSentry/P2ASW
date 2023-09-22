//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef VLOADGAMEDIALOG_H
#define VLOADGAMEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Frame.h"
#include "vgui/MouseCode.h"
#include "KeyValues.h"
#include "UtlVector.h"
#include "basemodui.h"
#include "VFlyoutMenu.h"
#include "gameui_util.h"
#include "vfooterpanel.h"
#include "vSaveGameDialog.h"

#define SAVEGAME_MAPNAME_LEN 32
#define SAVEGAME_COMMENT_LEN 80
#define SAVEGAME_ELAPSED_LEN 32

using namespace BaseModUI;
using namespace vgui;
class CNB_Button;
class CNB_Header_Footer;
//-----------------------------------------------------------------------------
// Purpose: Base class for save & load game dialogs
//-----------------------------------------------------------------------------
class CLoadGameDialog : public CBaseModFrame
{
	DECLARE_CLASS_SIMPLE( CLoadGameDialog, CBaseModFrame );

public:
	CLoadGameDialog( vgui::Panel *parent, const char *name );
	~CLoadGameDialog();
	void ScanSavedGames();

	Panel* NavigateBack();
	
	virtual void Activate()
	{
		BaseClass::Activate();
		ScanSavedGames();
	}
protected:
	CUtlVector<SaveGameDescription_t> m_SaveGames;
	vgui::PanelListPanel *m_pGameList;

	void ApplySchemeSettings( IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings( pScheme );
	}
	virtual void	OnCommand( const char *command );
	virtual void OnScanningSaveGames() {}
	void UpdateFooter( void );
	void DeleteSaveGame( const char *fileName );
	void CreateSavedGamesList();
	int GetSelectedItemSaveIndex();
	void AddSaveGameItemToList( int saveIndex );

	bool ParseSaveData( char const *pszFileName, char const *pszShortName, SaveGameDescription_t &save );

private:
	static void DeleteConfirmedCallback();

	CNB_Header_Footer *m_pHeaderFooter;
	MESSAGE_FUNC( OnPanelSelected, "PanelSelected" );
};

#endif // BASESAVEGAMEDIALOG_H
