//--------------------------------------------------------------------------------------------------------
// Copyright (c) 2007 Turtle Rock Studios, Inc.

#if !defined( C_BASETOGGLE_H )
#define C_BASETOGGLE_H

#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"

//copy/paste from server subs.cpp
enum togglemovetypes_t
{
	MOVE_TOGGLE_NONE = 0,
	MOVE_TOGGLE_LINEAR = 1,
	MOVE_TOGGLE_ANGULAR = 2,
};

//--------------------------------------------------------------------------------------------------------
class C_BaseToggle: public C_BaseEntity
{
public:
	DECLARE_CLASS( C_BaseToggle, C_BaseEntity );
	DECLARE_CLIENTCLASS();
	
	virtual void GetGroundVelocityToApply( Vector &vecGroundVel );
	virtual bool ShouldPredict( void );
	virtual C_BasePlayer *GetPredictionOwner( void );
	virtual void PhysicsSimulate( void );
	virtual bool PredictionErrorShouldResetLatchedForAllPredictables( void ) { return false; }
	virtual bool PredictionIsPhysicallySimulated( void );
	virtual void PostDataUpdate( DataUpdateType_t updateType );

	Vector PredictPosition( float fCurTime );

	EHANDLE m_hPredictionOwner;
	Vector m_vLastNetworked;
	float m_fLastNetworkedTime;

	Vector m_vecFinalDest;
	int m_movementType;
	float m_flMoveTargetTime;
	bool m_bWasPredictingMotion;
};


//--------------------------------------------------------------------------------------------------------
class C_BaseButton: public C_BaseToggle
{
public:
	DECLARE_CLASS( C_BaseButton, C_BaseToggle );
	DECLARE_CLIENTCLASS();

	C_BaseButton()
	{
	}

	virtual bool IsPotentiallyUsable( void );

private:
	bool m_usable;
};


#endif // C_BASETOGGLE_H