//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
// $NoKeywords: $
//======================================================================================//

#include "triggers.h"

class CTriggerPaintCleanser : public CBaseTrigger
{
public:
	DECLARE_SERVERCLASS()
	DECLARE_CLASS( CTriggerPaintCleanser, CBaseTrigger );
	CNetworkVar(bool, m_bDisabled);

	CTriggerPaintCleanser();

	virtual void Spawn( void );
	virtual void Activate( void );
	virtual void Enable( void );

	bool IsEnabled() const { return !m_bDisabled; }

	virtual void Touch( CBaseEntity *pOther );

	virtual int UpdateTransmitState( void );

	virtual void UpdateOnRemove( void );

	static bool PaintCleanserVisibilityCallback( CBaseEntity *pVisibleEntity, CBasePlayer *pViewingPlayer );
	static bool PaintCleanserVisibilityFilter( CBaseEntity *pVisibleEntity, CBasePlayer *pViewingPlayer );

	DECLARE_DATADESC();

public: // Only public because m_bDisabled is public in the base, and I don't feel like changing it. However, this only serves to save me time, while fueling my silent C++ nerd-rage. -Ted
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_bDisabled );
};
