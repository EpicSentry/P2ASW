#ifndef RADIALMENU_TAUNT_H
#define RADIALMENU_TAUNT_H
#ifdef _WIN32
#pragma once
#endif

#include "radialmenu.h"
#include "matchmaking/iplayer.h"

struct TauntStatusData
{
	char szName[16];
	char szCommand[32];
	char szIcon[64];
	char szIcon2[64];
	int nTextureID;
	int nTextureID2;
	bool bTeamTaunt;
	char szPosition[16];
	bool bOwned;
	bool bUsed;
};

class ClientMenuManagerTaunt : public ClientMenuManager
{
public:
	ClientMenuManagerTaunt();
    ~ClientMenuManagerTaunt();

    void ClearTauntStatusData();
    
	KeyValues *FindMenu( const char *menuName );
    
	void Flush();
    
	void KeyValueProcessor( IPlayerLocal *pPlayer );
    CUtlVector<TauntStatusData> *GetTauntData();
    bool IsTauntTeam( const char *pchTaunt );
    void SetTauntMenuStale( void );
    void SetTauntOwned( const char *pchTaunt, bool bAwardSilently );
    void SetTauntUsed( const char *pchTaunt );
    void SetTauntPosition( const char *pchTaunt, const char *pchPosition );
    void SetTauntLocked( const char *pchTaunt );
    void SetAllTauntsLocked( void );
    enum UpdateStorageFlags_t
    {
        UPDATE_STORAGE_OWNED=1,
        UPDATE_STORAGE_USED=2
    };
    void UpdateStorageChange( TauntStatusData *pTauntData, unsigned int uiUpdateFlags );
    int GetNumTauntsOwned( void );
    int GetNumTauntsUnused( void );
    TauntStatusData *GetUnusedTaunt( int nIndex );
    void ClearCustomMenuKeys( void );
    void AddCustomMenuKey( KeyValues *pKeys );
    KeyValues *GetAllTauntKeys( void );
    void UpdateDefaultTaunts( void );

protected:
    TauntStatusData *GetTauntDataByName( const char *pchTaunt );
    KeyValues *m_customMenuKeys;
    int m_nNumCustomKeys;
    bool m_bLoadedSaveData;
    bool m_bMenuStale;
    bool m_bOldControllerMode;
    CUtlVector<TauntStatusData> m_TauntData;
};

extern ClientMenuManagerTaunt &GetClientMenuManagerTaunt( int nSlot = -1 );

#endif // RADIALMENU_TAUNT_H