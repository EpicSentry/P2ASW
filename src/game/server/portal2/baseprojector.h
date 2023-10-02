#ifndef BASEPROJECTOR_H
#define BASEPROJECTOR_H

#include "cbase.h"

#define NO_CLIENT_PROJECTOR

class CBaseProjector : public CBaseAnimating
{
public:
	
	DECLARE_CLASS( CBaseProjector, CBaseAnimating );
	DECLARE_DATADESC();
#ifndef NO_CLIENT_PROJECTOR
	DECLARE_SERVERCLASS();
#endif
    CBaseProjector();
    ~CBaseProjector();
    
	virtual void Spawn();
    virtual void Activate();
    virtual void UpdateOnRemove();
    virtual void OnRestore();
    virtual void Project();
    
	class CBaseProjectedEntity * CreateNewProjectedEntity();
    void EnableProjection( bool bEnable );
    void TestProjectionChangesThink();
    void TestAllForProjectionChanges();
	
	CNetworkVar( bool, m_bEnabled );
	
    void SetTransmit(class CCheckTransmitInfo * ,bool );
	
protected:
	
    void Shutdown();
    void Cleanup();
    int DrawDebugTextOverlays();
    void DrawBoundsHighlight();
    void TestForPortalChanges();
    void InputEnable( inputdata_t &inputdata );
    void InputDisable( inputdata_t &inputdata );
	
	CNetworkHandle( CBaseProjectedEntity, m_hFirstChild );
	
    bool m_bDisablePlacementHelper;
};

#endif //BASEPROJECTOR_H