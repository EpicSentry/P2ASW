#include "cbase.h"
#include "c_indicator_panel.h"

IMPLEMENT_CLIENTCLASS_DT( C_LabIndicatorPanel, DT_LabIndicatorPanel, CLabIndicatorPanel )

RecvPropBool( RECVINFO( m_bEnabled ) ),
RecvPropBool( RECVINFO( m_bStopped ) ),
RecvPropBool( RECVINFO( m_bCountingDown ) ),
RecvPropBool( RECVINFO( m_bIsCountdownTimer ) ),
RecvPropBool( RECVINFO( m_bIsChecked ) ),

RecvPropFloat( RECVINFO( m_flTimerDuration ) ),
RecvPropFloat( RECVINFO( m_flTimerStart ) ),

RecvPropInt( RECVINFO( m_iPlayerPinged ) ),

END_RECV_TABLE();

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C_LabIndicatorPanel::C_LabIndicatorPanel()
{

}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
C_LabIndicatorPanel::~C_LabIndicatorPanel()
{

}

//-----------------------------------------------------------------------------
// Purpose: Get time left percentage for our vgui screen.
//-----------------------------------------------------------------------------
float C_LabIndicatorPanel::GetCountdownPercentage( void )
{
	if ( !m_bCountingDown )
		return 1.0;
	
	float flEndTime = m_flTimerDuration + m_flTimerStart;

	if (m_flTimerStart != flEndTime)
	{
		float flTimeLeftPercent = (gpGlobals->curtime - m_flTimerStart) / (flEndTime - m_flTimerStart);
		if (flTimeLeftPercent >= 0.0)
		{
			if (flTimeLeftPercent <= 1.0)
				return 1.0 - flTimeLeftPercent;
			else
				return 0.0;
		}

		return 1.0;
	}

	return gpGlobals->curtime - flEndTime;
}