//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "prediction.h"
#include "c_baseplayer.h"
#include "igamemovement.h"
#include "c_sdk_player.h"
#include "prediction_private.h"
#include "tier0/vprof.h"
#include "con_nprint.h"
#include "IClientVehicle.h"
#include "sdk_movedata.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CSDK_MoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;
extern IGameMovement *g_pGameMovement;

class CSDK_Prediction : public CPrediction
{
DECLARE_CLASS( CSDK_Prediction, CPrediction );

public:
	CSDK_Prediction();

	virtual void	SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void	RunCommand( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper );
	virtual void	FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move );
};

CSDK_Prediction::CSDK_Prediction()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSDK_Prediction::SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, 
	CMoveData *move )
{
	// Call the default SetupMove code.
	BaseClass::SetupMove( player, ucmd, pHelper, move );

	CBaseEntity *pMoveParent = player->GetMoveParent();
	if (!pMoveParent)
	{
		move->m_vecAbsViewAngles = move->m_vecViewAngles;
	}
	else
	{
		matrix3x4_t viewToParent, viewToWorld;
		AngleMatrix( move->m_vecViewAngles, viewToParent );
		ConcatTransforms( pMoveParent->EntityToWorldTransform(), viewToParent, viewToWorld );
		MatrixAngles( viewToWorld, move->m_vecAbsViewAngles );
	}

	// setup trace optimization
	g_pGameMovement->SetupMovementBounds( move );
}

extern void DiffPrint( bool bServer, int nCommandNumber, char const *fmt, ... );

//-----------------------------------------------------------------------------
// Purpose: Predicts a single movement command for player
// Input  : *moveHelper - 
//			*player - 
//			*u - 
//-----------------------------------------------------------------------------
void CSDK_Prediction::RunCommand( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper )
{
	BaseClass::RunCommand( player, ucmd, moveHelper );
}

void CSDK_Prediction::FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move )
{
	BaseClass::FinishMove( player, ucmd, move );
}

// Expose interface to engine
// Expose interface to engine
static CSDK_Prediction g_Prediction;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CSDK_Prediction, IPrediction, VCLIENT_PREDICTION_INTERFACE_VERSION, g_Prediction );

CPrediction *prediction = &g_Prediction;

