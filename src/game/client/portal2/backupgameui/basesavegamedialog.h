//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef BASESAVEGAMEDIALOG_H
#define BASESAVEGAMEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Frame.h"
#include "vgui/MouseCode.h"
#include "KeyValues.h"
#include "UtlVector.h"


#define SAVEGAME_MAPNAME_LEN 32
#define SAVEGAME_COMMENT_LEN 80
#define SAVEGAME_ELAPSED_LEN 32


struct SaveGameDescription_t
{
	char szShortName[64];
	char szFileName[128];
	char szMapName[SAVEGAME_MAPNAME_LEN];
	char szComment[SAVEGAME_COMMENT_LEN];
	char szType[64];
	char szElapsedTime[SAVEGAME_ELAPSED_LEN];
	char szFileTime[32];
	unsigned int iTimestamp;
	unsigned int iSize;
};


int SaveReadNameAndComment( FileHandle_t f, char *name, char *comment );


//-----------------------------------------------------------------------------
// Purpose: Base class for save & load game dialogs
//-----------------------------------------------------------------------------
class CBaseSaveGameDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CBaseSaveGameDialog, vgui::Frame );

public:
	CBaseSaveGameDialog( vgui::Panel *parent, const char *name );
	static int __cdecl SaveGameSortFunc( const void *lhs, const void *rhs );

protected:
	CUtlVector<SaveGameDescription_t> m_SaveGames;
	vgui::PanelListPanel *m_pGameList;

	virtual void OnScanningSaveGames() {}

	void DeleteSaveGame( const char *fileName );
	void ScanSavedGames();
	void CreateSavedGamesList();
	int GetSelectedItemSaveIndex();
	void AddSaveGameItemToList( int saveIndex );

	bool ParseSaveData( char const *pszFileName, char const *pszShortName, SaveGameDescription_t &save );

private:
	MESSAGE_FUNC( OnPanelSelected, "PanelSelected" );
};


#endif // BASESAVEGAMEDIALOG_H
