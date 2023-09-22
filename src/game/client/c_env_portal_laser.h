#ifndef C_ENV_PORTAL_LASER_H
#define C_ENV_PORTAL_LASER_H

#include "c_baseentity.h"

class C_PortalLaser_Client : public C_BaseEntity
{
public:
	DECLARE_CLASS(C_PortalLaser_Client, C_BaseEntity);
	DECLARE_CLIENTCLASS();

	C_PortalLaser_Client();
	virtual ~C_PortalLaser_Client();

	void Spawn();
	void Think();

private:
	bool m_bPreviousLaserState; // Store the previous laser state to avoid unnecessary updates
};

#endif // C_ENV_PORTAL_LASER_H