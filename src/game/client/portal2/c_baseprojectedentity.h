#ifndef C_BASEPROJECTEDENTITY_H
#define C_BASEPROJECTEDENTITY_H

#include "cbase.h"
#include "paint/paintable_entity.h"

class C_BaseProjector;

class C_BaseProjectedEntity : public C_BaseEntity
{
public:
	
	DECLARE_CLASS( C_BaseProjectedEntity, C_BaseEntity )
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	
	C_BaseProjectedEntity();
	~C_BaseProjectedEntity();
	Vector GetStartPoint() { return m_vecStartPoint; }
	Vector GetEndPoint() { return m_vecEndPoint; }
	Vector GetLengthVector();
    void RecursiveProjection( bool bShouldSpawn, C_BaseProjector *pParentProjector, C_Portal_Base2D *pExitPortal, const Vector &vProjectOrigin, const QAngle &qProjectAngles, int iRemainingProjections, bool bDisablePlacementHelper );
	void TestForProjectionChanges();
	void TestForReflectPaint();
	
    bool IsHittingPortal( Vector* pOutOrigin, QAngle* pOutAngles, C_Portal_Base2D** pOutPortal );
    bool DidRedirectionPortalMove( C_Portal_Base2D* pPortal );

	void FindProjectedEndpoints();
    virtual void GetProjectionExtents( Vector &outMins, Vector &outMaxs );
	virtual void OnProjected();
	virtual void HandleDataChange();
	virtual void HandlePredictionError(bool bErrorInThisEntity);
	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );
	C_BasePlayer *GetPredictionOwner();
	bool ShouldPredict();
	static void TestAllForProjectionChanges();
	
	struct BaseProjectedEntity_PreDataChanged
	{
		Vector vStartPoint;
		Vector vEndPoint;
		QAngle qAngles;
		
		BaseProjectedEntity_PreDataChanged()
		{
			vStartPoint = vec3_origin;
			vEndPoint = vec3_origin;
			qAngles = vec3_angle;
		}
    };
	
	
    void SetHitPortal( C_Portal_Base2D* pPortal );
    void SetSourcePortal( C_Portal_Base2D* pPortal );
	C_Portal_Base2D *GetHitPortal();
	C_Portal_Base2D *GetSourcePortal();
	
	BaseProjectedEntity_PreDataChanged PreDataChanged;

	Vector m_vecSourcePortalCenter;
	Vector m_vecSourcePortalRemoteCenter;
	QAngle m_vecSourcePortalAngle;
	QAngle m_vecSourcePortalRemoteAngle;
	Vector m_vecStartPoint;
	Vector m_vecEndPoint;
	int m_iMaxRemainingRecursions;
	bool m_bCreatePlacementHelper;

	CHandle<C_Portal_Base2D> m_hHitPortal;
	CHandle<C_Portal_Base2D> m_hSourcePortal;
	CHandle<C_BaseProjectedEntity> m_hChildSegment;
	CHandle<C_BaseEntity> m_hPlacementHelper;

};

#endif // C_BASEPROJECTEDENTITY_H