#ifndef BASEPROJECTOR_H
#define BASEPROJECTOR_H

#include "cbase.h"


class CBaseProjectedEntity;

class CBaseProjector : public CBaseAnimating
{
public:
	
	DECLARE_CLASS( CBaseProjector, CBaseAnimating );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

    CBaseProjector();
    ~CBaseProjector();
    
	virtual void Spawn();
    virtual void Activate();
    virtual void UpdateOnRemove();
    virtual void OnRestore();
    virtual void Project();
    
	virtual CBaseProjectedEntity *CreateNewProjectedEntity();
    void EnableProjection( bool bEnable );
    void TestProjectionChangesThink();
    static void TestAllForProjectionChanges();
	
	CNetworkVar( bool, m_bEnabled );
	
    void SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways );
	
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