#ifndef ENV_PORTAL_LASER_H
#define ENV_PORTAL_LASER_H

#include "cbase.h"
#include "beam_shared.h"
#include "baseanimating.h"

class CPortalLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS(CPortalLaser, CBaseAnimating);
	DECLARE_DATADESC();
	//DECLARE_SERVERCLASS();

	CPortalLaser();

	virtual void Think(void);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void LaserOff(void);
	virtual void LaserOn(void);
	virtual void UpdateLaser(void);
	virtual void DoTraceFromPortal(CPortal_Base2D* pRemotePortal);
	virtual float LaserEndPointSize(void);
	void NotifyCubeLaserContact(CBaseEntity* pCube);
	// Input functions
	void InputTurnOn(inputdata_t& inputData);
	void InputTurnOff(inputdata_t& inputData);
	void InputToggle(inputdata_t& inputData);

	bool IsLaserOn() const { return m_bLaserOn; }

	// Keyvalues
	string_t m_modelName; // The model path for the laser entity
	//Handling laser on cube
	bool IsLaserHittingCube();
	bool m_bIsLaserHittingCube;
	//Handling laser on catcher
	bool IsLaserHittingCatcher();
	bool m_bIsLaserHittingCatcher;
	//Handling remote portal catcher
	bool IsLaserHittingPortalCatcher();
	bool m_bIsLaserHittingPortalCatcher;
	//Hitting a portal?
	bool m_bIsHittingPortal;
	/*
	CNetworkVar(bool, m_bIsHittingPortal);
	CNetworkVar(Vector, v_vHitPos);
	CNetworkVar(Vector, vecNetOrigin);
	CNetworkVar(Vector, vecNetMuzzleDir);
	*/
private:
	CBeam* m_pBeam;

	int m_spawnFlags;
	bool m_bStartOff; // To check the start state
	bool m_bLaserOn;
	int m_iLaserAttachmentIndex;
	static const int LASER_ATTACHMENT;
	static const int LASER_EYE_ATTACHMENT;
	static const float LASER_RANGE;
	static const char* LASER_ATTACHMENT_NAME; // The name of the laser attachment.
	float m_fPulseOffset;
	static const float LASER_END_POINT_PULSE_SCALE;
	// Cube instances
	string_t m_cubeUniqueID;
	
	float m_flLastDamageTime;
	float m_flLastDamageSoundTime;
	

};

#endif // ENV_PORTAL_LASER_H
