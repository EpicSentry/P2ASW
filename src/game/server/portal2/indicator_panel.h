#include "cbase.h"

class CLabIndicatorPanel : public CBaseEntity
{	
public:
	
	DECLARE_CLASS( CLabIndicatorPanel, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	
    CLabIndicatorPanel();
    ~CLabIndicatorPanel();
    int UpdateTransmitState();
	void SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways );
    void Spawn( void );
    void Precache( void );
    void OnRestore( void );
    void ScreenVisible( bool bVisible );
    void Disable( void );
    void Enable( void );
    void StartTimer( float flDuration );
    void ResetTimer( void );
    void SetChecked( bool bIsChecked );
    void SetPlayerPinged( int iPlayerPinged );
    void SetIsTimer( bool bIsTimer );
	
    void InputDisable( inputdata_t &inputdata );
    void InputEnable( inputdata_t &inputdata );
	
private:
	
	void GetControlPanelInfo( int nPanelIndex, const char *&pPanelName );
	void GetControlPanelClassName( int nPanelIndex, const char *&pPanelName );
	void SpawnControlPanels( void );
	void RestoreControlPanels( void );
      
	CNetworkVar( bool, m_bEnabled );
	CNetworkVar( bool, m_bStopped );
	CNetworkVar( bool, m_bCountingDown );
	CNetworkVar( bool, m_bIsCountdownTimer );
	CNetworkVar( bool, m_bIsChecked );
	CNetworkVar( float, m_flTimerDuration );
	CNetworkVar( float, m_flTimerStart );
	
	CNetworkVar( int, m_iPlayerPinged );

	// NOTE: This isn't faithful to the code, just trying to fix an issue where GetEntity() for the client is NULL
	bool m_bDoFullTransmit;

	CHandle<CVGuiScreen> m_hScreen;
	
};

class CPropIndicatorPanel : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPropIndicatorPanel, CBaseAnimating );
	DECLARE_DATADESC();

	CPropIndicatorPanel();
	~CPropIndicatorPanel();
	
	void Precache();
	void Spawn();
	void UpdateOnRemove();
	void SetTimerDuration( float flDuration );
	void StartTimer();
	void StopTimer();
	void ResetTimer();

	void InputStart( inputdata_t &inputdata );
	void InputStop( inputdata_t &inputdata );
	void InputReset( inputdata_t &inputdata );
	void InputCheck( inputdata_t &inputdata );
	void InputUncheck( inputdata_t &inputdata );

private:

	void CreateIndicatorPanel();
	void ToggleIndicatorLights(bool);
	void ToggleThink();

	bool m_bEnabled;
	bool m_bStopped;
	bool m_bCountingDown;
	bool m_bIsCountdownTimer;
	bool m_bIsChecked;

	float m_flTimerDuration;
	float m_flTimerStart;

	string_t m_strIndicatorLights;

protected:

	CHandle<CLabIndicatorPanel> m_hIndicatorPanel;

};