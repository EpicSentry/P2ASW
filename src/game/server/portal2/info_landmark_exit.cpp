#include "cbase.h"

class CInfoLandmarkExit : public CPointEntity
{
public:
	DECLARE_CLASS(CInfoLandmarkExit, CPointEntity);
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(info_landmark_exit, CInfoLandmarkExit);

BEGIN_DATADESC(CInfoLandmarkExit)
END_DATADESC()
