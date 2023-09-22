#ifndef TRIGGER_PORTAL_CLEANSER_H
#define TRIGGER_PORTAL_CLEANSER_H
#include "cbase.h"
#include "triggers.h"
#include "portal_player.h"
#include "weapon_portalgun.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "physobj.h"
#include "portal/weapon_physcannon.h"
#include "model_types.h"
#include "rumble_shared.h"

class CTriggerPortalCleanser : public CBaseTrigger
{
public:
	DECLARE_CLASS(CTriggerPortalCleanser, CBaseTrigger);

	void Spawn(void);
	void Touch(CBaseEntity *pOther);

	DECLARE_DATADESC();

	// Outputs
	COutputEvent m_OnDissolve;
	COutputEvent m_OnFizzle;
	COutputEvent m_OnDissolveBox;
};

BEGIN_DATADESC(CTriggerPortalCleanser)

// Outputs
DEFINE_OUTPUT(m_OnDissolve, "OnDissolve"),
DEFINE_OUTPUT(m_OnFizzle, "OnFizzle"),
DEFINE_OUTPUT(m_OnDissolveBox, "OnDissolveBox"),

END_DATADESC()


LINK_ENTITY_TO_CLASS(trigger_portal_cleanser, CTriggerPortalCleanser);
#endif // ifndef TRIGGER_PORTAL_CLEANSER_H