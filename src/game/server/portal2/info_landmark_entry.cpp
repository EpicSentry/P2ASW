#include "cbase.h"

class CInfoLandmarkEntry : public CPointEntity
{
public:
	DECLARE_CLASS(CInfoLandmarkEntry, CPointEntity);
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(info_landmark_entry, CInfoLandmarkEntry);

BEGIN_DATADESC(CInfoLandmarkEntry)
END_DATADESC()
