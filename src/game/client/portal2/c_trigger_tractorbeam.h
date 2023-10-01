#ifndef C_TRIGGER_TRACTOR_BEAM_H
#define C_TRIGGER_TRACTOR_BEAM_H

#include "cbase.h"
#include "portal_shareddefs.h"
#include "c_triggers.h"
#include "trigger_tractorbeam_shared.h"
#include "paint/paint_blobs_shared.h"
#include "vphysics_interface.h"

#ifdef NO_TRACTOR_BEAM
#error "THIS FILE SHOULDN'T BE INCLUDED"
#endif

#define CTrigger_TractorBeam C_Trigger_TractorBeam

class C_ProjectedTractorBeamEntity;

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

	virtual C_BaseEntity * GetEntity( void ) { return this; } // NOTE: This is just my best guess, could be inaccurate.

	void	CalculateFrameMovement( IPhysicsObject *pObject, CBaseEntity *pEntity, float deltaTime, Vector &linear, AngularImpulse &angular );
	void	UpdateBeam( const Vector& vStartPoint, const Vector& vEndPoint, float flLinearForce );
	void	SetDirection( const Vector &vStart, const Vector &vEnd );
	
	float	GetSpeed( void );
	float	GetLinearLimit( void );
	float	GetLinearForce( void );

	bool	HasGravityScale( void );
	bool	HasAirDensity( void );
	bool	HasLinearLimit( void );
	bool	HasAngularLimit( void );
	bool	HasAngularScale( void );
	bool	HasLinearForce( void );

	void	SetProxyEntity( C_ProjectedTractorBeamEntity *pTractorBeam );

	virtual void	GetToolRecordingState( KeyValues *msg );
	virtual void	RestoreToToolRecordedState( KeyValues *msg );
	virtual bool	GetSoundSpatialization( SpatializationInfo_t& info );
	

	void	CreateParticles( void );
	void	RemoveAllBlobsFromBeam( void );
	
	virtual void	StartTouch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );
	virtual void	PhysicsSimulate();
	
	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );

	bool	IsFromPortal( void );
	bool	IsToPortal( void );
	bool	IsReversed( void );

	int		GetLastUpdateFrame() const;

	float	GetBeamRadius( void );

	void	RemoveDeadBlobs( void );
	void	RemoveChangedBeamBlobs( void );
	void	RemoveAllBlobsFromBeam( void );

	Vector	GetForceDirection( void );
	Vector	GetStartPoint( void ) const { return m_vStart; }
	Vector	GetEndPoint( void ) const { return m_vEnd; }

	Vector	GetForceDirection() const;
	
	PaintBlobVector_t m_blobs;

	int		GetLastUpdateFrame( void );

protected:
	
	void CreateParticles();
	void DrawColumn( IMaterial *pMaterial, Vector &, Vector &, float , Vector &, Vector &, float, float, bool, bool, float );
	void DrawColumnSegment( CMeshBuilder &meshBuilder, Vector &, Vector &, float, Vector &, Vector &, float, float, float, int );
	
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
	IPhysicsMotionController * m_pController;

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