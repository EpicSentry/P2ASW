//=============================================================================//
//
// Purpose:
//
//=============================================================================//
#include "cbase.h"
#include "func_tank.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CInfo_Player_Ping_Detector : public CPointEntity, public CGameEventListener
{
public:
	CInfo_Player_Ping_Detector();
	~CInfo_Player_Ping_Detector();

	void Spawn();
	void UpdateOnRemove();

	void FireGameEvent( IGameEvent *event );

    void InputToggle( inputdata_t &inputdata );
    void InputEnable( inputdata_t &inputdata );
    void InputDisable( inputdata_t &inputdata );
protected:
    void Toggle();
    void Enable();
    void Disable();

private:
	string_t m_iszFuncTankName;
	
	bool m_bEnabled;
	bool m_bDetectedNewPing;
	
	bool m_bLookAtPlayerPings;
	int m_nTeamToLookAt;

	Vector m_vecPingLocation;
	
	COutputEvent m_OnPingDetected;

	DECLARE_DATADESC();
};







BEGIN_DATADESC( CInfo_Player_Ping_Detector ) // Line 52

	DEFINE_FIELD( m_vecPingLocation, FIELD_VECTOR ),

	DEFINE_KEYFIELD( m_bEnabled, FIELD_BOOLEAN, "Enabled" ),
	DEFINE_KEYFIELD( m_iszFuncTankName, FIELD_STRING, "FuncTankName" ),
	DEFINE_KEYFIELD( m_bLookAtPlayerPings, FIELD_BOOLEAN, "LookAtPlayerPings" ),
	DEFINE_KEYFIELD( m_nTeamToLookAt, FIELD_INTEGER, "TeamToLookAt" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_OUTPUT( m_OnPingDetected, "OnPingDetected" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( info_player_ping_detector, CInfo_Player_Ping_Detector )







//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CInfo_Player_Ping_Detector::CInfo_Player_Ping_Detector() // Line 80
{
	m_vecPingLocation.Init();
	m_bDetectedNewPing = false;
}

CInfo_Player_Ping_Detector::~CInfo_Player_Ping_Detector() // Line 87
{

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::Spawn() // Line 95
{
	BaseClass::Spawn();

	ListenForGameEvent( "portal_player_ping" );
}


void CInfo_Player_Ping_Detector::UpdateOnRemove() // Line 103
{
	BaseClass::UpdateOnRemove();
}


void CInfo_Player_Ping_Detector::FireGameEvent( IGameEvent *event ) // Line 109
{
	if ( !m_bEnabled )
		return;

	const char *pszName = event->GetName();
	if ( !V_strcmp( pszName, "portal_player_ping" ) )
	{
		int userid = event->GetInt( "userid" );
		Vector vecPingPos;
		vecPingPos.x = event->GetFloat( "ping_x" );
		vecPingPos.y = event->GetFloat( "ping_y" );
		vecPingPos.z = event->GetFloat( "ping_z" );

		int teamNumber = TEAM_ANY;
		for ( int i = 0; i < MAX_PLAYERS; ++i )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
			if ( pPlayer && pPlayer->GetUserID() == userid )
			{
				teamNumber = pPlayer->GetTeamNumber();
				break;
			}
		}

		if ( m_nTeamToLookAt == TEAM_SPECTATOR || m_nTeamToLookAt == teamNumber )
		{
			m_bDetectedNewPing = true;
			m_vecPingLocation = vecPingPos;

			CBaseEntity *pEnt = NULL;
			while ( ( pEnt = gEntList.FindEntityByName( pEnt, m_iszFuncTankName.ToCStr() ) ) != NULL )
			{
				CFuncTank *pTank = dynamic_cast<CFuncTank*>( pEnt );
				if ( pTank )
				{
					pTank->AimAtTargetPosition( m_vecPingLocation );
				}
			}
			m_OnPingDetected.FireOutput( NULL, NULL );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::InputToggle( inputdata_t &inputdata ) // Line 156
{
	Toggle();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::InputEnable( inputdata_t &inputdata ) // Line 164
{
	Enable();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::InputDisable( inputdata_t &inputdata ) // Line 172
{
	Disable();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::Toggle() // Line 180
{
	if ( m_bEnabled )
	{
		Disable();
	}
	else
	{
		Enable();
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::Enable() // Line 196
{
	m_bEnabled = true;
	ListenForGameEvent( "portal_player_ping" );
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CInfo_Player_Ping_Detector::Disable() // Line 206
{
	m_bEnabled = false;
	StopListeningForAllEvents();
}