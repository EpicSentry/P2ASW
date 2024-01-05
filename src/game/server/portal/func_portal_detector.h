//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which fires an output when a portal is placed in it.
//
// $NoKeywords: $
//======================================================================================//

#ifndef _FUNC_PORTAL_DETECTOR_H_
#define _FUNC_PORTAL_DETECTOR_H_

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

class CProp_Portal;

class CFuncPortalDetector : public CBaseEntity
{
public:
	DECLARE_CLASS( CFuncPortalDetector, CBaseEntity );

	CFuncPortalDetector::CFuncPortalDetector();
	CFuncPortalDetector::~CFuncPortalDetector();

	// Overloads from base entity
	virtual void	Spawn( void );
	
	void SetActive( bool bActive );

	// Inputs to flip functionality on and off
	void InputDisable( inputdata_t &inputdata );
	void InputEnable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
	
	virtual void NotifyPortalEvent( PortalEvent_t nEventType, CPortal_Base2D *pNotifier );
	virtual void UpdateOnPortalMoved( CProp_Portal *pPortal );
    virtual void UpdateOnPortalActivated( CProp_Portal *pPortal );

	// misc public methods
	bool IsActive( void ) { return m_bActive; }	// is this area currently detecting portals
	int GetLinkageGroupID( void ) { return m_iLinkageGroupID; }

	COutputEvent m_OnStartTouchPortal;
	COutputEvent m_OnStartTouchPortal1;
	COutputEvent m_OnStartTouchPortal2;
	COutputEvent m_OnStartTouchLinkedPortal;
	COutputEvent m_OnStartTouchBothLinkedPortals;
	COutputEvent m_OnEndTouchPortal;
	COutputEvent m_OnEndTouchPortal1;
	COutputEvent m_OnEndTouchPortal2;
	COutputEvent m_OnEndTouchLinkedPortal;
	COutputEvent m_OnEndTouchBothLinkedPortals;

	DECLARE_DATADESC();
    CFuncPortalDetector *m_pNext;
	
	bool IsPortalTouchingDetector( const CProp_Portal *pPortal );

protected:
	
	virtual void PortalPlacedInsideBounds( CProp_Portal *pPortal );
	virtual void PortalRemovedFromInsideBounds( CProp_Portal * );
	
	bool	m_bActive;			// are we currently detecting portals
	int		m_iLinkageGroupID;	// what set of portals are we testing for?
	bool	m_bCheckAllIDs;

	CHandle<CBaseEntity> m_phTouchingPortals[2];
	int m_iTouchingPortalCount;
	
};

CFuncPortalDetector* GetPortalDetectorList();

#endif
