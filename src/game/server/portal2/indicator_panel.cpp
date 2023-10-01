#include "cbase.h"
#include "indicator_panel.h"
#include "vguiscreen.h"

BEGIN_DATADESC( CLabIndicatorPanel )

DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bStopped, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bCountingDown, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bIsCountdownTimer, FIELD_BOOLEAN ),
DEFINE_FIELD( m_bIsChecked, FIELD_BOOLEAN ),
DEFINE_FIELD( m_flTimerDuration, FIELD_FLOAT ),
DEFINE_FIELD( m_flTimerStart, FIELD_FLOAT ),
DEFINE_FIELD( m_iPlayerPinged, FIELD_INTEGER ),
DEFINE_FIELD( m_hScreen, FIELD_EHANDLE ),

END_DATADESC();

IMPLEMENT_SERVERCLASS_ST( CLabIndicatorPanel, DT_LabIndicatorPanel )

SendPropBool( SENDINFO( m_bEnabled ) ),
SendPropBool( SENDINFO( m_bStopped ) ),
SendPropBool( SENDINFO( m_bCountingDown ) ),
SendPropBool( SENDINFO( m_bIsCountdownTimer ) ),
SendPropBool( SENDINFO( m_bIsChecked ) ),

SendPropFloat( SENDINFO( m_flTimerDuration ) ),
SendPropFloat( SENDINFO( m_flTimerStart ) ),

SendPropInt( SENDINFO( m_iPlayerPinged ) ),

END_SEND_TABLE();

LINK_ENTITY_TO_CLASS( vgui_indicator_panel, CLabIndicatorPanel )

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CLabIndicatorPanel::CLabIndicatorPanel( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CLabIndicatorPanel::~CLabIndicatorPanel( void )
{		
	if ( m_hScreen )
		DestroyVGuiScreen( m_hScreen.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::Spawn( void )
{
	Precache();

	BaseClass::Spawn();
	
	SpawnControlPanels();

	m_bEnabled = true;

	ScreenVisible( true );

	m_bCountingDown = false;
	m_bStopped = false;
	m_flTimerStart = 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::Precache( void )
{
	PrecacheVGuiScreen("indicator_panel");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::OnRestore( void )
{
	BaseClass::OnRestore();
	RestoreControlPanels();
	ScreenVisible( m_bEnabled );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::RestoreControlPanels( void )
{
	if ( m_hScreen )
		UTIL_RemoveImmediate( m_hScreen );
	
	SpawnControlPanels();
}


//-----------------------------------------------------------------------------
// Purpose: Enables the screen
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::Enable( void )
{
	if ( !m_bEnabled )
	{
		m_bEnabled = true;
		ScreenVisible( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Disables the screen
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::Disable( void )
{
	if ( m_bEnabled )
	{
		m_bEnabled = false;
		ScreenVisible( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Starts the timer
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::StartTimer( float flDuration )
{
	m_flTimerStart = gpGlobals->curtime;
	m_bStopped = false;
	m_bCountingDown = true;
	m_bIsCountdownTimer = true;
	m_flTimerDuration = flDuration;
}

//-----------------------------------------------------------------------------
// Purpose: Resets the timer
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::ResetTimer( void )
{
	m_bStopped = true;
	m_bCountingDown = false;
	m_bIsCountdownTimer = true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the player pinged value to the value entered
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::SetPlayerPinged( int iPlayerPinged )
{
	m_iPlayerPinged = iPlayerPinged;
	m_bIsCountdownTimer = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::GetControlPanelInfo( int nPanelIndex, const char *&pPanelName )
{
	pPanelName = "indicator_panel";
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::GetControlPanelClassName( int nPanelIndex, const char *&pPanelName )
{
	pPanelName = "vgui_screen";
}

//-----------------------------------------------------------------------------
// Purpose: Create VGUI Screen
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::SpawnControlPanels( void )
{
	CVGuiScreen *VGuiScreen = CreateVGuiScreen("vgui_screen", "indicator_panel", this, this, -1);

	VGuiScreen->ChangeTeam( GetTeamNumber() );
	VGuiScreen->SetActualSize( 32.0, 32.0);
	VGuiScreen->SetActive( true );
	VGuiScreen->MakeVisibleOnlyToTeammates( false );
	VGuiScreen->SetTransparency( true );

	m_hScreen = VGuiScreen;
}
void CLabIndicatorPanel::ScreenVisible( bool bVisible )
{
	if (m_hScreen)
	{
		if (bVisible)
		{
#if 0
			m_pPev = entscreen2->m_Network.m_pPev;
			if (m_pPev)
				m_pPev->m_fStateFlags |= 0x80u;
#endif
			m_hScreen->RemoveEffects(EF_NODRAW);
			//m_hScreen->DispatchUpdateTransmitState();

		}
		else
		{
			m_hScreen->AddEffects( EF_NODRAW );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets if this panel is a timer or not
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::SetIsTimer( bool bIsTimer )
{
	m_bIsCountdownTimer = bIsTimer;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the panel's "checked" state
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::SetChecked( bool bIsChecked )
{
	m_bIsChecked = true;
	m_bIsCountdownTimer = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CLabIndicatorPanel::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_FULLCHECK );
}

//-----------------------------------------------------------------------------
// Purpose: Make sure our screen's transmission is the same as our's
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways )
{
	BaseClass::SetTransmit( pInfo, bAlways );
	if ( m_hScreen )
		m_hScreen->SetTransmit( pInfo, bAlways );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::InputEnable( inputdata_t &inputdata )
{
	Enable();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLabIndicatorPanel::InputDisable( inputdata_t &inputdata )
{
	Disable();
}

BEGIN_DATADESC( CPropIndicatorPanel )

	DEFINE_FIELD( m_bStopped, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bCountingDown, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_flTimerStart, FIELD_BOOLEAN ),

	DEFINE_KEYFIELD( m_bEnabled, FIELD_BOOLEAN, "Enabled" ),
	DEFINE_KEYFIELD( m_bIsCountdownTimer, FIELD_BOOLEAN, "IsTimer" ),
	DEFINE_KEYFIELD( m_bIsChecked, FIELD_BOOLEAN, "IsChecked" ),
	DEFINE_KEYFIELD( m_strIndicatorLights, FIELD_STRING, "IndicatorLights" ),
	DEFINE_KEYFIELD( m_flTimerDuration, FIELD_BOOLEAN, "TimerDuration" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Check", InputCheck ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Uncheck", InputUncheck ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Start", InputStart ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Stop", InputStop ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Reset", InputReset ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( prop_indicator_panel, CPropIndicatorPanel )

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPropIndicatorPanel::CPropIndicatorPanel( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPropIndicatorPanel::~CPropIndicatorPanel( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::Spawn( void )
{
	Precache();
	CollisionProp()->SetSolidFlags( FSOLID_NOT_SOLID );
	SetMoveType( MOVETYPE_NONE, MOVECOLLIDE_DEFAULT );
			
	AddEffects( EF_NODRAW );

	DispatchUpdateTransmitState();
	CreateIndicatorPanel();
}

//-----------------------------------------------------------------------------
// Purpose: Precache
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::Precache( void )
{
	UTIL_PrecacheOther("vgui_indicator_panel");
}

//-----------------------------------------------------------------------------
// Purpose: Remove me and my indicator panel
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::UpdateOnRemove( void )
{
	UTIL_Remove( m_hIndicatorPanel );
	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::ResetTimer( void )
{
	if ( m_hIndicatorPanel )
		m_hIndicatorPanel->ResetTimer();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::SetTimerDuration( float flDuration )
{
	m_flTimerDuration = flDuration;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::ToggleIndicatorLights( bool bLightsOn )
{
	CBaseEntity *pIndicatorLight = NULL;
	while (1)
	{
		if (!m_strIndicatorLights.ToCStr())
			break;

		pIndicatorLight = gEntList.FindEntityByName( pIndicatorLight, m_strIndicatorLights.ToCStr() );

		if (!pIndicatorLight)
			break;

		pIndicatorLight->SetTextureFrameIndex( bLightsOn );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::StartTimer( void )
{
	if ( m_hIndicatorPanel )
	{
		m_hIndicatorPanel->StartTimer( m_flTimerDuration );
		ToggleIndicatorLights( true );
		SetThink( &CPropIndicatorPanel::ToggleThink );
		SetNextThink( gpGlobals->curtime + m_flTimerDuration );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::StopTimer( void )
{
	// FIXME
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::ToggleThink( void )
{
	ToggleIndicatorLights( false );	
	SetThink( NULL );
}

//-----------------------------------------------------------------------------
// Purpose: Create our indicator panel
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::CreateIndicatorPanel( void )
{
	m_hIndicatorPanel = (CLabIndicatorPanel*)CreateEntityByName("vgui_indicator_panel");
	
	if ( m_hIndicatorPanel )
	{
		VMatrix mat;
		MatrixFromAngles( GetAbsAngles() , mat);
		
		Vector vecOffset;
		vecOffset.x = 0.0;
		vecOffset.z = 0.0;
		vecOffset.y = 1.0;

		VMatrix tmp;
		VMatrix rotation;
		MatrixBuildRotationAboutAxis( rotation, vecOffset, 90.0 );
		MatrixMultiply( mat, rotation, tmp );
		MatrixBuildRotateZ(rotation, 90.0);
		
		MatrixMultiply( tmp, rotation, mat);

		QAngle angles;
		MatrixAngles( mat.As3x4(), angles );
				
		m_hIndicatorPanel->SetAbsAngles( angles );

		Vector vecForward, vecUp, vecRight;
		
		GetVectors( &vecForward, &vecRight, &vecUp);

		vecOffset = ((vecForward * -1.0) + (vecRight * 16.0)) + (vecUp * -16.0);
		
		m_hIndicatorPanel->SetAbsOrigin( GetAbsOrigin() + vecOffset );
		m_hIndicatorPanel->SetPlayerPinged( -1 );
		m_hIndicatorPanel->SetChecked( m_bIsChecked );
		m_hIndicatorPanel->SetIsTimer( m_bIsCountdownTimer );

		DispatchSpawn( m_hIndicatorPanel.Get() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::InputStart( inputdata_t &inputdata )
{
	StartTimer();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::InputStop( inputdata_t &inputdata )
{
	StopTimer();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::InputReset( inputdata_t &inputdata )
{
	ResetTimer();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::InputCheck( inputdata_t &inputdata )
{
	m_hIndicatorPanel->SetChecked( true );

	SetThink( NULL );
	ToggleIndicatorLights( true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::InputUncheck( inputdata_t &inputdata )
{
	m_hIndicatorPanel->SetChecked( false );

	SetThink( NULL );
	ToggleIndicatorLights( false );
}