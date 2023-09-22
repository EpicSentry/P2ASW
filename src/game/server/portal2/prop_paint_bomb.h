//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef PROP_PAINT_BOMB_H
#define PROP_PAINT_BOMB_H

#include "props.h"
#include "paint_color_manager.h"

class CPropPaintBomb : public CPhysicsProp
{
public:
	DECLARE_CLASS( CPropPaintBomb, CPhysicsProp );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPropPaintBomb();
	~CPropPaintBomb();

	virtual void Precache( void );
	virtual void Spawn( void );

	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual void OnFizzled( void );

	void SetPaintPowerType( PaintPowerType paintType );
	void CleansePaintPower( void );

	int UpdateTransmitState( void );

private:
	void InputDissolve( inputdata_t &in );
	void InputSilentDissolve( inputdata_t &in );
	void InputDisablePortalFunnel( inputdata_t &in );
	void InputEnablePortalFunnel( inputdata_t &in );

	COutputEvent m_OnFizzled;
	COutputEvent m_OnExploded;

	CNetworkVar( PaintPowerType, m_nPaintPowerType );
	bool m_bPlaySpawnSound;
	bool m_bAllowSilentDissolve;
};

#endif //PROP_PAINT_BOMB_H
