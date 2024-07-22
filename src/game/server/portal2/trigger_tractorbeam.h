#ifndef TRIGGER_TRACTORBEAM_H
#define TRIGGER_TRACTORBEAM_H

#include "cbase.h"
#include "triggers.h"
#include "baseprojectedentity.h"


//#include "trigger_tractorbeam_shared.h"


class CTrigger_TractorBeam;
class CPaintBlob;

class CProjectedTractorBeamEntity : public CBaseProjectedEntity
{
public:

	DECLARE_CLASS( CProjectedTractorBeamEntity, CBaseProjectedEntity );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
    CProjectedTractorBeamEntity();
    ~CProjectedTractorBeamEntity();
	
    virtual void Spawn();
    virtual void UpdateOnRemove();
    virtual void GetProjectionExtents( Vector &outMins, Vector &outMaxs );
    virtual void OnProjected();
    virtual void OnPreProjected();

	float GetLinearForce();

    static CProjectedTractorBeamEntity *CreateNewInstance();
    virtual CBaseProjectedEntity *CreateNewProjectedEntity();

private:
	
	CNetworkHandle( CTrigger_TractorBeam, m_hTractorBeamTrigger );
};

DECLARE_AUTO_LIST( ITriggerTractorBeamAutoList )

class CTrigger_TractorBeam : public CBaseVPhysicsTrigger, public IMotionEvent, public ITriggerTractorBeamAutoList
{
public:
	DECLARE_CLASS( CTrigger_TractorBeam, CBaseVPhysicsTrigger )
	DECLARE_DATADESC()
	DECLARE_SERVERCLASS();

	CTrigger_TractorBeam();
    ~CTrigger_TractorBeam();    
	void Spawn();
    void Precache();
    void Activate();
    void UpdateOnRemove();
    void StopLoopingSounds();
    bool CreateVPhysics();
    void OnRestore();
	
    void StartTouch( CBaseEntity *pOther );
    void EndTouch( CBaseEntity *pOther );
	
    void InputSetVelocityLimitTime( inputdata_t &inputdata );
	
    float GetLinearLimit();
    float GetLinearForce();
    void SetLinearForce( float flLinearForce );
    void SetLinearForce( Vector vDir, float flLinearForce );
    void SetAsReversed( bool bReversed );
    void SetGravityScale( float flGravityScale );
    void SetAirDensity( float flAirDensity );
    void SetLinearLimit( float flLinearLimit );
    void SetAngularLimt( float flAngularLimit );
	void SetProxyEntity( CProjectedTractorBeamEntity *pProxy );
    void DisablePlayerMovement( bool );
    void SetDirection( const Vector &vStart, const Vector &vEnd );
    void UpdateBeam( const Vector& vStartPoint, const Vector& vEndPoint, float flLinearForce );
	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );
    void WakeTouchingObjects();
    void CalculateFrameMovement( IPhysicsObject *pObject, CBaseEntity *pEntity, float deltaTime, Vector &linear, AngularImpulse &angular );
    void SetBeamRadius(float );
    void RemoveDeadBlobs();
    void RemoveChangedBeamBlobs();
    void RemoveAllBlobsFromBeam();

	float GetBeamRadius() { return m_flRadius; }

	bool HasGravityScale() { return m_gravityScale != 0.0; }
	bool HasAirDensity();
	bool HasLinearLimit();
	bool HasLinearScale();
	bool HasAngularLimit();
	bool HasAngularScale();
	bool HasLinearForce();

    static CTrigger_TractorBeam *CreateTractorBeam( const Vector &vStart, const Vector &vEnd, CProjectedTractorBeamEntity *pOwner );
    
	//PaintBlobVector_t m_blobs;
    
	CUtlVector<CPaintBlob*> m_blobs;

	int GetLastUpdateFrame() const;
    void ForceAttachEntity( CBaseEntity *pEntity );
    void ForceDetachEntity( CBaseEntity *pEntity );
	
	bool IsReversed() { return m_bReversed; }
    bool IsFromPortal() { return m_bFromPortal; }
	bool IsToPortal() { return m_bToPortal; }
	
	Vector GetForceDirection() const;
	Vector GetStartPoint() const { return m_vStart; }
	Vector GetEndPoint() const { return m_vEnd; }

	CBaseEntity *GetEntity() { return this; }
	
private:
	
    void TractorThink();
    IPhysicsMotionController *m_pController;
	
	CNetworkVar( float, m_gravityScale );
	CNetworkVar( float, m_addAirDensity );
	CNetworkVar( float, m_linearLimit );
	CNetworkVar( float, m_linearLimitDelta );
	CNetworkVar( float, m_linearLimitTime );
	CNetworkVar( float, m_linearLimitStart );	
	CNetworkVar( float, m_linearLimitStartTime );
	CNetworkVar( float, m_linearScale );	
	CNetworkVar( float, m_angularLimit );
	CNetworkVar( float, m_angularScale );
	
	CNetworkQAngle( m_linearForceAngles )
	
	CNetworkHandle( CProjectedTractorBeamEntity, m_hProxyEntity );
	
	CNetworkVector( m_vStart );
	CNetworkVector( m_vEnd );
	
	CNetworkVar( float, m_linearForce );
	CNetworkVar( float, m_flRadius );
	
	
	CNetworkVar( bool, m_bDisabled );
	CNetworkVar( bool, m_bReversed );
	CNetworkVar( bool, m_bFromPortal );
	CNetworkVar( bool, m_bToPortal );
	CNetworkVar( bool, m_bDisablePlayerMove );
    
	CSoundPatch *m_sndPlayerInBeam;
	
    int m_nLastUpdateFrame;
};


#endif // TRIGGER_TRACTORBEAM_H