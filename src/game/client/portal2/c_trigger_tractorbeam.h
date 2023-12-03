#ifndef C_TRIGGER_TRACTOR_BEAM_H
#define C_TRIGGER_TRACTOR_BEAM_H

#include "cbase.h"
#include "portal_shareddefs.h"
#include "c_triggers.h"
#include "vphysics_interface.h"
#include "c_baseprojectedentity.h"

#ifdef NO_TRACTOR_BEAM
#error "THIS FILE SHOULDN'T BE INCLUDED"
#endif

#define CTrigger_TractorBeam C_Trigger_TractorBeam

DECLARE_AUTO_LIST( ITriggerTractorBeamAutoList )

class C_Trigger_TractorBeam;
class C_PaintBlob;

class C_ProjectedTractorBeamEntity : public C_BaseProjectedEntity
{
public:
	DECLARE_CLASS( C_ProjectedTractorBeamEntity, C_BaseProjectedEntity );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	
	C_ProjectedTractorBeamEntity();
	~C_ProjectedTractorBeamEntity();
	virtual void GetProjectionExtents( Vector &outMins, Vector &outMaxs );
	virtual void OnProjected();
    
private:
	CHandle<C_Trigger_TractorBeam> m_hTractorBeamTrigger;
};

class C_Trigger_TractorBeam : public C_BaseVPhysicsTrigger, public IMotionEvent, public ITriggerTractorBeamAutoList
{	
public:
	DECLARE_CLASS( C_Trigger_TractorBeam, C_BaseVPhysicsTrigger );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	//C_Trigger_TractorBeam( C_Trigger_TractorBeam & ); // In pdb ripper, but: ???
	C_Trigger_TractorBeam();
	~C_Trigger_TractorBeam();

	virtual void	Spawn( void );
	virtual void	OnNewParticleEffect( const char *pszParticleName, CNewParticleEffect *pNewParticleEffect );
	
	virtual int		DrawModel( int flags, const RenderableInstance_t &instance );

	virtual bool	ShouldDraw( void );
	virtual void	UpdateOnRemove( void );

	virtual void	OnDataChanged( DataUpdateType_t updateType );

	virtual	C_BasePlayer *GetPredictionOwner( void );
	
	virtual bool	ShouldPredict( void );
	
	virtual RenderableTranslucencyType_t ComputeTranslucencyType( void );

	virtual void	UpdatePartitionListEntry( void );

	virtual C_BaseEntity * GetEntity( void ) { return this; }

	void	CalculateFrameMovement( IPhysicsObject *pObject, CBaseEntity *pEntity, float deltaTime, Vector &linear, AngularImpulse &angular );
	void	UpdateBeam( const Vector& vStartPoint, const Vector& vEndPoint, float flLinearForce );
	void	SetDirection( const Vector &vStart, const Vector &vEnd );
	
	float	GetSpeed( void );
	float	GetLinearLimit( void );
	float	GetLinearForce( void );

	bool	HasGravityScale( void );
	bool	HasAirDensity( void );
	bool	HasLinearLimit( void );
	bool	HasLinearScale( void );
	bool	HasAngularLimit( void );
	bool	HasAngularScale( void );
	bool	HasLinearForce( void );

	void	SetProxyEntity( C_ProjectedTractorBeamEntity *pTractorBeam );

	virtual void	GetToolRecordingState( KeyValues *msg );
	virtual void	RestoreToToolRecordedState( KeyValues *msg );
	virtual bool	GetSoundSpatialization( SpatializationInfo_t& info );
		
	virtual void	StartTouch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );
	virtual void	PhysicsSimulate();
	
	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );
	
	bool	IsReversed( void ) { return m_bReversed; }
    bool	IsFromPortal( void ) { return m_bFromPortal; }
	bool	IsToPortal( void ) { return m_bToPortal; }

	int		GetLastUpdateFrame() const;
	
	float GetBeamRadius() { return m_flRadius; }

	void	RemoveDeadBlobs( void );
	void	RemoveChangedBeamBlobs( void );
	void	RemoveAllBlobsFromBeam( void );

	Vector	GetForceDirection() const;
	Vector	GetStartPoint( void ) const { return m_vStart; }
	Vector	GetEndPoint( void ) const { return m_vEnd; }


	//PaintBlobVector_t m_blobs;

	CUtlVector<C_PaintBlob*> m_blobs;
	
protected:
	
	void CreateParticles();

	void DrawColumn( IMaterial *pMaterial, Vector &vecStart, Vector vDir, float flLength, Vector &vecXAxis, Vector &vecYAxis, 
										float flRadius, float flAlpha, bool bPinchIn, bool bPinchOut, float flTextureOffset );
	
	void DrawColumnSegment( CMeshBuilder &meshBuilder, Vector &vecStart, Vector &vDir, float flLength, Vector &vecXAxis,
										Vector &vecYAxis, float flRadius, float flAlpha, float flTextureOffset, VertexFormat_t vertexFormat );
	
	Vector m_vStart;
	Vector m_vEnd;

	float m_linearForce;
	float m_flRadius;
	bool m_bReversed;
	bool m_bFromPortal;
	bool m_bToPortal;
	bool m_bDisablePlayerMove;
	IMaterial *m_pMaterial1;
	IMaterial *m_pMaterial2;
	IMaterial *m_pMaterial3;
	bool m_bRecreateParticles;

	CHandle<C_ProjectedTractorBeamEntity> m_hProxyEntity;
	CUtlReference<CNewParticleEffect> m_hCoreEffect;
	IPhysicsMotionController *m_pController;

	float m_gravityScale;
	float m_addAirDensity;
	float m_linearLimit;
    float m_linearLimitDelta;
    float m_linearLimitTime;
    float m_linearLimitStart;
    float m_linearLimitStartTime;
    float m_linearScale;
    float m_angularLimit;
	float m_angularScale;
	QAngle m_linearForceAngles;

private:
	
	float m_flStartTime;
	int m_nLastUpdateFrame;
	
};

#endif