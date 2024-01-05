#ifndef FUNC_PORTALLED_H
#define FUNC_PORTALLED_H

#include "func_portal_detector.h"

class CProp_Portal;

class CFunc_Portalled : public CFuncPortalDetector
{
public:
	DECLARE_DATADESC();
	CFunc_Portalled();
	~CFunc_Portalled();
	
	COutputEvent m_OnEntityPrePortalled;
	COutputEvent m_OnEntityPostPortalled;
	
	void OnPrePortalled( CBaseEntity *pOther, bool bDeparting );
	void OnPostPortalled( CBaseEntity *pOther, bool bDeparting );

protected:
	void PortalPlacedInsideBounds( CProp_Portal *pPortal );
	bool m_bFireOnPlayer;
	bool m_bFireOnDeparture;
	bool m_bFireOnArrival;
};

#endif // FUNC_PORTALLED_H