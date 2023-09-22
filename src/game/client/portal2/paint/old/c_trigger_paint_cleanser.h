//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef C_TRIGGER_PAINT_CLEANSER_H
#define C_TRIGGER_PAINT_CLEANSER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_triggers.h"

class C_TriggerPaintCleanser : public C_BaseTrigger
{
public:

	DECLARE_CLASS( C_TriggerPaintCleanser, C_BaseTrigger );
	DECLARE_CLIENTCLASS();

	C_TriggerPaintCleanser( void );
	~C_TriggerPaintCleanser( void );

	virtual void Spawn( void );
	virtual void OnDataChanged( DataUpdateType_t type );

	virtual void UpdateOnRemove( void );

	bool IsEnabled() const { return !m_bDisabled; }

private:
	bool m_bDisabled;
};

typedef C_TriggerPaintCleanser CTriggerPaintCleanser;

#endif //C_TRIGGER_PAINT_CLEANSER_H
