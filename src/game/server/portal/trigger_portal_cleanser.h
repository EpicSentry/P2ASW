#ifndef TRIGGER_PORTAL_CLEANSER_H
#define TRIGGER_PORTAL_CLEANSER_H

#include "cbase.h"
#include "triggers.h"

static char *g_pszPortalNonCleansable[] = 
{ 
	"func_door", 
	"func_door_rotating", 
	"prop_door_rotating",
	"func_tracktrain",
	"env_ghostanimating",
	"physicsshadowclone",
	"prop_energy_ball",
	NULL,
};

//-----------------------------------------------------------------------------
// Purpose: Removes anything that touches it. If the trigger has a targetname,
//			firing it will toggle state.
//-----------------------------------------------------------------------------
class CTriggerPortalCleanser : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerPortalCleanser, CBaseTrigger );

	void Spawn( void );
	void Touch( CBaseEntity *pOther );

	//Use CBaseEntity for now
	static void FizzleBaseAnimating( CBaseEntity *pActivator, CBaseEntity *pEntity );

	bool m_bVisible;
	
	DECLARE_DATADESC();

	// Outputs
	COutputEvent m_OnDissolve;
	COutputEvent m_OnFizzle;
	COutputEvent m_OnDissolveBox;
};

#endif