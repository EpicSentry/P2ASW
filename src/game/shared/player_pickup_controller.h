//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#if !defined ( WEAPON_PHYSCANNON_H ) && 0
#define WEAPON_PHYSCANNON_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
// derive from this so we can add save/load data to it
struct game_shadowcontrol_params_t : public hlshadowcontrol_params_t
{
	DECLARE_SIMPLE_DATADESC();
};

//-----------------------------------------------------------------------------
class CGrabController : public IMotionEvent
{
	DECLARE_SIMPLE_DATADESC();

public:

	CGrabController(void);
	~CGrabController(void);
	void AttachEntity(CBasePlayer *pPlayer, CBaseEntity *pEntity, IPhysicsObject *pPhys, bool bIsMegaPhysCannon, const Vector &vGrabPosition, bool bUseGrabPosition);
	void DetachEntity(bool bClearVelocity);
	void OnRestore();

	bool UpdateObject(CBasePlayer *pPlayer, float flError);

	void SetTargetPosition(const Vector &target, const QAngle &targetOrientation);
	void GetTargetPosition(Vector *target, QAngle *targetOrientation);
	float ComputeError();
	float GetLoadWeight(void) const { return m_flLoadWeight; }
	void SetAngleAlignment(float alignAngleCosine) { m_angleAlignment = alignAngleCosine; }
	void SetIgnorePitch(bool bIgnore) { m_bIgnoreRelativePitch = bIgnore; }
	QAngle TransformAnglesToPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer);
	QAngle TransformAnglesFromPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer);

	CBaseEntity *GetAttached() { return (CBaseEntity *)m_attachedEntity; }

	IMotionEvent::simresult_e Simulate(IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular);
	float GetSavedMass(IPhysicsObject *pObject);

	bool IsObjectAllowedOverhead(CBaseEntity *pEntity);

	//set when a held entity is penetrating another through a portal. Needed for special fixes
	void SetPortalPenetratingEntity(CBaseEntity *pPenetrated);

private:
	// Compute the max speed for an attached object
	void ComputeMaxSpeed(CBaseEntity *pEntity, IPhysicsObject *pPhysics);

	game_shadowcontrol_params_t	m_shadow;
	float			m_timeToArrive;
	float			m_errorTime;
	float			m_error;
	float			m_contactAmount;
	float			m_angleAlignment;
	bool			m_bCarriedEntityBlocksLOS;
	bool			m_bIgnoreRelativePitch;

	float			m_flLoadWeight;
	float			m_savedRotDamping[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	float			m_savedMass[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	EHANDLE			m_attachedEntity;
	QAngle			m_vecPreferredCarryAngles;
	bool			m_bHasPreferredCarryAngles;
	float			m_flDistanceOffset;

	QAngle			m_attachedAnglesPlayerSpace;
	Vector			m_attachedPositionObjectSpace;

	IPhysicsMotionController *m_controller;

	bool			m_bAllowObjectOverhead; // Can the player hold this object directly overhead? (Default is NO)

											//set when a held entity is penetrating another through a portal. Needed for special fixes
	EHANDLE			m_PenetratedEntity;

	friend void GetSavedParamsForCarriedPhysObject(CGrabController *pGrabController, IPhysicsObject *pObject, float *pSavedMassOut, float *pSavedRotationalDampingOut);
};


bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupController, CBaseEntity *pHeldEntity );
void ShutdownPickupController( CBaseEntity *pPickupControllerEntity );
float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );

CBaseEntity *GetPlayerHeldEntity( CBasePlayer *pPlayer );
CBasePlayer *GetPlayerHoldingEntity( CBaseEntity *pEntity );

CGrabController *GetGrabControllerForPlayer( CBasePlayer *pPlayer );
void GetSavedParamsForCarriedPhysObject( CGrabController *pGrabController, IPhysicsObject *pObject, float *pSavedMassOut, float *pSavedRotationalDampingOut );
void UpdateGrabControllerTargetPosition( CBasePlayer *pPlayer, Vector *vPosition, QAngle *qAngles );

void GrabController_SetPortalPenetratingEntity( CGrabController *pController, CBaseEntity *pPenetrated );

#endif // WEAPON_PHYSCANNON_H
