#ifndef ENV_PORTAL_LASER_H
#define ENV_PORTAL_LASER_H

#include "cbase.h"
#include "beam_shared.h"
#include "baseanimating.h"
#include "info_placement_helper.h"

class CPropWeightedCube;

class CPortalLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS(CPortalLaser, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CPortalLaser();
	
	virtual void Spawn( void );
	virtual void Activate( void );
	virtual void Precache( void );
	virtual void UpdateOnRemove( void );
    virtual bool CreateVPhysics();
	virtual int UpdateTransmitState();
	virtual void TurnOff( void );
	virtual void TurnOn( void );
	
	void StrikeThink( void );
    void BeamDamage( trace_t &tr );
    void UpdateSoundPosition( Vector &vecStart, Vector &vecEnd );

	void TurnOnGlow();
	void TurnOffGlow();
    void TurnOffLaserSound();
	void UpdateLaser( void );
	void DoTraceFromPortal( CPortal_Base2D* pRemotePortal, trace_t &tr, Vector vecMuzzleDir );
	float LaserEndPointSize( void );
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
	bool m_bGlowInitialized;
	bool m_bNoPlacementHelper;
	
    void CreateHelperEntities();
    void CreateSoundProxies();
	
	Vector m_vecNearestSoundSource[MAX_PLAYERS];
    CBaseEntity *m_pSoundProxy[MAX_PLAYERS];
	CSoundPatch *m_pAmbientSound[MAX_PLAYERS];
    CInfoPlacementHelper *m_pPlacementHelper;

private:
	bool m_bStartOff; // To check the start state
	CNetworkVar( bool, m_bLaserOn );
	CNetworkVar( bool, m_bIsLethal );
	CNetworkVar( bool, m_bShouldSpark );
	CNetworkVar( bool, m_bUseParentDir );
	int m_iLaserAttachment;
	static const int LASER_ATTACHMENT;
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
