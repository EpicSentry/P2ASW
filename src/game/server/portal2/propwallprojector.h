#ifndef PROP_WALL_PROJECTOR_H
#define PROP_WALL_PROJECTOR_H

#include "baseprojector.h"
#include "prop_tractorbeam.h"
#include "projectedwallentity.h"

#ifndef NO_PROJECTED_WALL

class CPropWallProjector : public CBaseProjector
{
public:
	DECLARE_CLASS( CPropWallProjector, CBaseProjector );
	DECLARE_DATADESC();
    CPropWallProjector();
    ~CPropWallProjector();
    void Spawn();
    void Precache();
    void Project();
    void Shutdown();

protected:
    CBaseProjectedEntity *CreateNewProjectedEntity();

private:
    CSoundPatch *m_pAmbientSound;
    CSoundPatch *m_pAmbientMusic;
    CHandle<ProjectedEntityAmbientSoundProxy> m_hAmbientSoundProxy;
    char s_AmbientBridgeMusic[64];
};

#endif // NO_PROJECTED_WALL

#endif // PROP_WALL_PROJECTOR_H