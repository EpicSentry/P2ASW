#include "cbase.h"
#include "te_effect_dispatch.h"
#include "props.h"

class CPortalLaserTarget : public CPointEntity
{
	DECLARE_CLASS(CPortalLaserTarget, CPointEntity);
public:
	void Spawn() override;
	int OnTakeDamage(CTakeDamageInfo* dmgInfo) override;
	void DisableThink();
	DECLARE_DATADESC();

	bool m_bPowered;
	bool m_bTerminalPoint;
	int m_iLaserType;

	COutputEvent m_OnPowered;
	COutputEvent m_OnUnpowered;

};

BEGIN_DATADESC(CPortalLaserTarget)
DEFINE_FIELD(m_bPowered, FIELD_BOOLEAN),
DEFINE_FIELD(m_bTerminalPoint, FIELD_BOOLEAN),
DEFINE_FIELD(m_iLaserType, FIELD_INTEGER),
END_DATADESC()

LINK_ENTITY_TO_CLASS(point_laser_target, CPortalLaserTarget);

void CPortalLaserTarget::Spawn()
{
	BaseClass::Spawn();

	Precache();

	AddFlag(FL_WORLDBRUSH);

	m_bPowered = false;

	if (m_bTerminalPoint && (m_hParent.Get() != nullptr))
	{
		const char* modelPath = STRING(m_hParent->GetModelName());
		const char* centerModelPath = "models/props/laser_catcher_center.mdl";

		if (Q_strcmp(modelPath, centerModelPath) == 0)
		{
			Vector vLaserCatcherExtents;
			Vector origin;
			float localFloat1, localFloat2, localFloat3;

			m_hParent->GetCollisionBounds(&extents, &origin);

			VectorRelayBounds(vRelayExtents);
			VectorCatcherBounds(vLaserCatcherExtents);

			m_hParent->GetAttachment(0, origin, localFloat1, localFloat2);

			localFloat3 = localFloat2 + localFloat1;

			origin.x = vLaserCatcherExtents.x * 15.0f + (localFloat3 + localFloat2) * 20.0f;
			origin.y = vLaserCatcherExtents.y * 15.0f + (localFloat3 + localFloat1) * 20.0f;
			origin.z = vLaserCatcherExtents.z * 15.0f + (localFloat3 + localFloat1) * 20.0f;

			UTIL_SetSize(this, -origin, origin);
		}
	}
}

int CPortalLaserTarget::OnTakeDamage(CTakeDamageInfo* dmgInfo)
{
	if (!m_bPowered)
	{
		CBaseEntity* attacker = dmgInfo->GetAttacker();
		if (!attacker)
			return 0;

		if (!FClassnameIs(attacker, "env_portal_laser"))
			return 0;

		m_bPowered = true;
		m_OnPowered.FireOutput(this, this, 0.0f);

		//if (m_pCatcher)
		//	(*m_pCatcher)();

		SetThink(&CPortalLaserTarget::DisableThink);
		SetNextThink(gpGlobals->curtime);

		return 0;
	}

	SetNextThink(gpGlobals->curtime + 0.1f);
	return 0;
}

void CPortalLaserTarget::DisableThink()
{
	m_bPowered = false;
	m_OnUnpowered.FireOutput(this, this, 0.0f);

	//if (m_pCatcher)
	//	(*m_pCatcher)();

	SetThink(NULL);
}