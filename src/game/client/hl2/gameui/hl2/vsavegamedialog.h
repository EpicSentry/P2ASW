//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef VBASESAVEGAMEDIALOG_H
#define VBASESAVEGAMEDIALOG_H
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


int VSaveReadNameAndComment( FileHandle_t f, char *name, char *comment );
static int __cdecl SaveGameSortFunc( const void *lhs, const void *rhs )
{
	const SaveGameDescription_t *s1 = (const SaveGameDescription_t *)lhs;
	const SaveGameDescription_t *s2 = (const SaveGameDescription_t *)rhs;

	if (s1->iTimestamp < s2->iTimestamp)
		return 1;
	else if (s1->iTimestamp > s2->iTimestamp)
		return -1;

	// timestamps are equal, so just sort by filename
	return strcmp(s1->szFileName, s2->szFileName);
};
using namespace BaseModUI;
using namespace vgui;
class CNB_Button;
class CNB_Header_Footer;
//-----------------------------------------------------------------------------
// Purpose: Base class for save & load game dialogs
//-----------------------------------------------------------------------------
class CVBaseSaveGameDialog : public CBaseModFrame/*vgui::Frame*///CBaseModFrame//, public FlyoutMenuListener
{
	DECLARE_CLASS_SIMPLE( CVBaseSaveGameDialog, CBaseModFrame );

public:
	CVBaseSaveGameDialog( vgui::Panel *parent, const char *name );
	~CVBaseSaveGameDialog();
	void ScanSavedGames();


	//FloutMenuListener
	/*virtual void OnNotifyChildFocus( vgui::Panel* child );
	virtual void OnFlyoutMenuClose( vgui::Panel* flyTo );
	virtual void OnFlyoutMenuCancelled();
	*/
	Panel* NavigateBack();

	static void FindSaveSlot( char *buffer, int bufsize );

	virtual void Activate()
	{
		BaseClass::Activate();
		ScanSavedGames();
	}
protected:

	CUtlVector<SaveGameDescription_t> m_SaveGames;
	vgui::PanelListPanel *m_pGameList;

	void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void	OnCommand( const char *command );
	virtual void OnScanningSaveGames();// {}
	void UpdateFooter( void );
	void DeleteSaveGame( const char *fileName );
	void CreateSavedGamesList();
	int GetSelectedItemSaveIndex();
	void AddSaveGameItemToList( int saveIndex );

	bool ParseSaveData( char const *pszFileName, char const *pszShortName, SaveGameDescription_t &save );

private:
	static void AcceptSaveOverwriteCallback();
	CNB_Header_Footer *m_pHeaderFooter;
	MESSAGE_FUNC( OnPanelSelected, "PanelSelected" );
};

#endif // BASESAVEGAMEDIALOG_H
