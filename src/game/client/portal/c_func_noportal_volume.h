//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume in which no portal can be placed. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals wholly or partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#ifndef _FUNC_NOPORTAL_VOLUME_H_
#define _FUNC_NOPORTAL_VOLUME_H_

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "c_triggers.h"

#define CFuncNoPortalVolume C_FuncNoPortalVolume

class C_FuncNoPortalVolume : public C_BaseTrigger
{
public:
	DECLARE_CLASS( C_FuncNoPortalVolume, C_BaseTrigger );
	DECLARE_CLIENTCLASS()

	C_FuncNoPortalVolume();
	~C_FuncNoPortalVolume();

	// Overloads from base entity
	virtual void	Spawn( void );
	
	void OnActivate( void );
	
	// misc public methods
	unsigned int GetIndex () { return m_iListIndex; } // returns the list index of this camera
	bool IsActive() { return m_bActive; }	// is this area currently blocking portals

	C_FuncNoPortalVolume		*m_pNext;			// Needed for the template list	

	DECLARE_DATADESC();

private:
	bool					m_bActive;			// are we currently blocking portals
	unsigned int			m_iListIndex;		// what is my index into the global noportal_volume list
	
};

// Global interface for getting the list of noportal_volumes
C_FuncNoPortalVolume* GetNoPortalVolumeList();


#endif