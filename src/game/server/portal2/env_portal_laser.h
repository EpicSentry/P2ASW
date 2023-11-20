#ifndef ENV_PORTAL_LASER_H
#define ENV_PORTAL_LASER_H

#include "cbase.h"
#include "beam_shared.h"
#include "baseanimating.h"

class CPropWeightedCube;

class CPortalLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS(CPortalLaser, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CPortalLaser();
	
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void UpdateOnRemove( void );
	virtual void Think( void );
	virtual void LaserOff( void );
	virtual void LaserOn( void );
	virtual void UpdateLaser( void );
	virtual void DoTraceFromPortal( CPortal_Base2D* pRemotePortal, trace_t &tr, Vector vecMuzzleDir );
	virtual float LaserEndPointSize( void );
	void NotifyCubeLaserContact( CBaseEntity* pCube );
	// Input functions
	void InputTurnOn(inputdata_t& inputdata);
	void InputTurnOff(inputdata_t& inputdata);
	void InputToggle(inputdata_t& inputdata);

	void DamageAllTargetsInRay( Ray_t &ray );

	bool IsLaserOn() const { return m_bLaserOn; }

	// Keyvalues
	string_t m_modelName; // The model path for the laser entity
	//Handling laser on cube
	bool IsLaserHittingCube();
	bool m_bFromReflectedCube;
	/*
	//Handling laser on catcher
	bool IsLaserHittingCatcher();
	bool m_bIsLaserHittingCatcher;
	//Handling remote portal catcher
	bool IsLaserHittingPortalCatcher();
	bool m_bIsLaserHittingPortalCatcher;
	*/
	//Hitting a portal?
	bool m_bIsHittingPortal;
	/*
	CNetworkVar(bool, m_bIsHittingPortal);
	CNetworkVar(Vector, v_vHitPos);
	CNetworkVar(Vector, vecNetOrigin);
	CNetworkVar(Vector, vecNetMuzzleDir);
	*/

	bool m_bLaserOnly;
	bool m_bIsLaserExtender;

private:
	CBeam* m_pBeam;

	bool m_bStartOff; // To check the start state
	CNetworkVar( bool, m_bLaserOn );
	CNetworkVar( bool, m_bShouldSpark );
	CNetworkVar( bool, m_bUseParentDir );
	int m_iLaserAttachmentIndex;
	static const int LASER_ATTACHMENT;
	static const int LASER_EYE_ATTACHMENT;
	static const float LASER_RANGE;
	static const char* LASER_ATTACHMENT_NAME; // The name of the laser attachment.
	float m_fPulseOffset;
	static const float LASER_END_POINT_PULSE_SCALE;
	// Cube instances
	string_t m_cubeUniqueID;
	
	CNetworkVector( m_vStartPoint );
	CNetworkVector( m_vEndPoint );
	CNetworkQAngle( m_angParentAngles );

	float m_flLastDamageTime;
	float m_flLastDamageSoundTime;
	float m_flLastSparkTime;

	CHandle<CPropWeightedCube> m_hTouchingReflector;
	CNetworkHandle( CPropWeightedCube, m_hReflector );

};

#endif // ENV_PORTAL_LASER_H
