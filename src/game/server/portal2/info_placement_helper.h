#ifndef INFO_PLACEMENT_HELPER_H
#define INFO_PLACEMENT_HELPER_H

//========= Copyright © 1996-2014, Valve Corporation, All rights reserved.=============//
//
//
//
//=====================================================================================//

// ENTITY STATUS = UNFINISHED.

#include "cbase.h"

//-----------------------------------------------------------------------------
// Link between entities and cameras
//-----------------------------------------------------------------------------
class CInfoPlacementHelper : public CPointEntity //Or CLogicEntity?
{
	DECLARE_CLASS(CInfoPlacementHelper, CPointEntity);
	DECLARE_DATADESC();

public:
	CInfoPlacementHelper();

	virtual void Spawn();
	virtual void Activate();

	float m_flRadius;

	bool m_bForcePlacement;
	bool m_bSnapToHelperAngles;

private:


	string_t m_strTargetProxy;
	string_t m_strTargetEntity;

	//CHandle<CPointCamera> m_hCamera;
	EHANDLE m_hTargetEntity;

	//friend CBaseEntity *CreateInfoCameraLink(CBaseEntity *pTarget, CInfoPlacementHelper *pCamera);
	//friend void PointCameraSetupVisibility(CBaseEntity *pPlayer, int area, unsigned char *pvs, int pvssize);


	COutputEvent m_OnObjectPlaced;
	COutputEvent m_ObjectPlacedSize;

};
#endif // INFO_PLACEMENT_HELPER_H