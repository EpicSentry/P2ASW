#ifndef C_TRIGGER_TRACTOR_BEAM_H
#define C_TRIGGER_TRACTOR_BEAM_H

#include "c_triggers.h"
#include "trigger_tractorbeam_shared.h"
#include "vphysics_interface.h"

#define CTrigger_TractorBeam C_Trigger_TractorBeam

class C_Trigger_TractorBeam : public C_BaseVPhysicsTrigger
{
	DECLARE_CLASS( C_Trigger_TractorBeam, C_BaseVPhysicsTrigger );
public:
	
	void CalculateFrameMovement( IPhysicsObject *pObject, CBaseEntity *pEntity, float deltaTime, Vector &linear, AngularImpulse &angular );
	void UpdateBeam( const Vector& vStartPoint, const Vector& vEndPoint, float flLinearForce );
	void SetDirection( const Vector &vStart, const Vector &vEnd );

	void CreateParticles( void );
	void RemoveAllBlobsFromBeam( void );

	void EndTouch( CBaseEntity *pOther );

	bool IsReversed();

	int GetLastUpdateFrame() const;

	float GetBeamRadius( void );

	Vector GetStartPoint( void ) const { return m_vStart; }
	Vector GetEndPoint( void ) const { return m_vEnd; }

	Vector GetForceDirection() const;
	
	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );

private:

	float m_nLastUpdateFrame;

	bool m_bReversed;

	Vector m_vStart;
	Vector m_vEnd;

	EHANDLE m_hProxyEntity;
	IPhysicsMotionController *m_pController;
};

#endif