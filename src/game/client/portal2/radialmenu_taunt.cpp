#include "cbase.h"
#include "radialmenu_taunt.h"
#include "vgui/isurface.h"

extern ConVar catapult_physics_drag_boost;

ClientMenuManagerTaunt TheClientMenuManagerTaunt[MAX_SPLITSCREEN_PLAYERS];

ClientMenuManagerTaunt &GetClientMenuManagerTaunt( int nSlot )
{
	if ( nSlot == -1 )
		nSlot = GET_ACTIVE_SPLITSCREEN_SLOT();

	return TheClientMenuManagerTaunt[nSlot];
}

ClientMenuManagerTaunt::ClientMenuManagerTaunt()
{

}

ClientMenuManagerTaunt::~ClientMenuManagerTaunt()
{
	ClearTauntStatusData();
}

void ClientMenuManagerTaunt::ClearTauntStatusData( void )
{
	if ( vgui::surface() )
	{
		for ( int i = 0; i < m_TauntData.Count(); ++i )
		{
			TauntStatusData *data = &m_TauntData[i];
			if ( data->nTextureID != -1 )
			{
				vgui::surface()->DestroyTextureID( data->nTextureID );
			}

			if ( data->nTextureID2 != -1 )
			{
				vgui::surface()->DestroyTextureID( data->nTextureID2 );
			}
		}
	}

	m_TauntData.RemoveAll();
}

KeyValues *ClientMenuManagerTaunt::FindMenu( const char *menuName )
{
	UpdateDefaultTaunts();
	return m_customMenuKeys->FindKey( menuName );
}

void ClientMenuManagerTaunt::Flush( void )
{
	// Undone
}

void ClientMenuManagerTaunt::KeyValueProcessor( IPlayerLocal *pPlayer )
{
	// Undone
}

CUtlVector<TauntStatusData> *ClientMenuManagerTaunt::GetTauntData()
{
	return &m_TauntData;
}

bool ClientMenuManagerTaunt::IsTauntTeam( const char *pchTaunt )
{
	TauntStatusData *data = GetTauntDataByName( pchTaunt );
	return data && data->bTeamTaunt;
}

void ClientMenuManagerTaunt::SetTauntMenuStale( void )
{
	// Undone
}

void ClientMenuManagerTaunt::SetTauntOwned( const char *pchTaunt, bool bAwardSilently )
{
	// Undone
}

void ClientMenuManagerTaunt::SetTauntUsed( const char *pchTaunt )
{
	// Undone
}

void ClientMenuManagerTaunt::SetTauntPosition( const char *pchTaunt, const char *pchPosition )
{
	// Undone
}

void ClientMenuManagerTaunt::SetTauntLocked( const char *pchTaunt )
{
	// Undone
}

void ClientMenuManagerTaunt::SetAllTauntsLocked( void )
{
	// Undone
}

void ClientMenuManagerTaunt::UpdateStorageChange( TauntStatusData *pTauntData, unsigned int uiUpdateFlags )
{
	// Undone
}

int ClientMenuManagerTaunt::GetNumTauntsOwned( void )
{
	// Undone: Use Linux bins to RE this function
	return 0;
}

int ClientMenuManagerTaunt::GetNumTauntsUnused( void )
{
	// Undone: Use Linux bins to RE this function
	return 0;
}

TauntStatusData *ClientMenuManagerTaunt::GetUnusedTaunt( int nIndex )
{
	// Undone: Use Linux bins to RE this function
	return NULL;
}

void ClientMenuManagerTaunt::ClearCustomMenuKeys( void )
{
	if ( m_customMenuKeys )
		m_customMenuKeys->deleteThis();

	m_customMenuKeys = new KeyValues( "ClientMenu" );
	m_nNumCustomKeys = 0;
}

void ClientMenuManagerTaunt::AddCustomMenuKey( KeyValues *pKeys )
{
	m_customMenuKeys->FindKey( "Default", true )->AddSubKey( pKeys );
	++m_nNumCustomKeys;
}

KeyValues *ClientMenuManagerTaunt::GetAllTauntKeys( void )
{
	return m_menuKeys;
}

void ClientMenuManagerTaunt::UpdateDefaultTaunts( void )
{
	// Undone
}

TauntStatusData *ClientMenuManagerTaunt::GetTauntDataByName( const char *pchTaunt )
{
	// Undone
	return NULL;
}