#ifndef BASE_PROJECTED_ENTITY_H
#define BASE_PROJECTED_ENTITY_H


#include "cbase.h"
#include "portal_base2d.h"
#include "baseprojector.h"

class CBaseProjectedEntity : public CBaseEntity
{
public:
	DECLARE_CLASS( CBaseProjectedEntity, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CBaseProjectedEntity();
    ~CBaseProjectedEntity();
    
	virtual void Spawn();
    virtual void UpdateOnRemove();
    virtual void OnRestore();
    virtual void OnPreProjected();
    virtual void OnProjected();
	
    void FindProjectedEndpoints();
    void RecursiveProjection( bool bShouldSpawn, CBaseProjector *pParentProjector, CPortal_Base2D *pExitPortal, const Vector &vProjectOrigin, const QAngle &qProjectAngles, int iRemainingProjections, bool bDisablePlacementHelper );
    void TestForProjectionChanges();
    void TestForReflectPaint();
    void PlacePlacementHelper( CInfoPlacementHelper *pHelper );
    virtual void SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways );
    void CheckForSettledReflectorCubes();
	
    virtual void GetProjectionExtents( Vector &outMins, Vector &outMaxs );
	
    bool IsHittingPortal( Vector* pOutOrigin, QAngle* pOutAngles, CPortal_Base2D** pOutPortal );
    bool DidRedirectionPortalMove( CPortal_Base2D* pPortal );
	
    void SetHitPortal( CPortal_Base2D* pPortal );
    void SetSourcePortal( CPortal_Base2D* pPortal );
	
	Vector GetStartPoint() { return m_vecStartPoint; }
	Vector GetEndPoint() { return m_vecEndPoint; }
    Vector GetLengthVector( void );
	
    CPortal_Base2D *GetHitPortal();
    CPortal_Base2D *GetSourcePortal();
	
    static CBaseProjectedEntity *CreateNewProjectedEntity();
	
protected:
		
	CNetworkHandle( CPortal_Base2D, m_hHitPortal );
	CNetworkHandle( CPortal_Base2D, m_hSourcePortal );
	
	CNetworkVector( m_vecSourcePortalCenter );
	CNetworkVector( m_vecSourcePortalRemoteCenter );
	CNetworkQAngle( m_vecSourcePortalAngle );
	CNetworkQAngle( m_vecSourcePortalRemoteAngle );
	
	CNetworkVector( m_vecStartPoint );
	CNetworkVector( m_vecEndPoint );
	
	CNetworkHandle( CBaseProjectedEntity, m_hChildSegment );
	CNetworkHandle( CInfoPlacementHelper, m_hPlacementHelper );
	
	CNetworkVar( bool, m_bCreatePlacementHelper );
	CNetworkVar( int, m_iMaxRemainingRecursions );
	
};

#endif // BASE_PROJECTED_ENTITY_H