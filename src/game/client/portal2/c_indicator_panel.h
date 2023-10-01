#include "cbase.h"

class C_LabIndicatorPanel : public C_BaseEntity
{

public:
	DECLARE_CLASS( C_LabIndicatorPanel, C_BaseEntity );
	DECLARE_CLIENTCLASS();
	
    C_LabIndicatorPanel();
    ~C_LabIndicatorPanel();

    float GetCountdownPercentage();
	
	bool IsCountingDown()	{ return m_bCountingDown; }
	bool IsCountdownTimer()	{ return m_bIsCountdownTimer; }
	bool IsPaused()			{ return m_bStopped; }
	bool IsChecked()		{ return m_bIsChecked; }
	bool IsEnabled()		{ return m_bEnabled; }
	
	int GetPlayerPinged()	{ return m_iPlayerPinged; }
	
private:
	
    bool m_bEnabled;
    bool m_bStopped;
    bool m_bCountingDown;
    bool m_bIsCountdownTimer;
    bool m_bIsChecked;
	
    float m_flTimerDuration;
    float m_flTimerStart;
	
    int m_iPlayerPinged;
};