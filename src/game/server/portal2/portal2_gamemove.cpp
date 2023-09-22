//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "player_command.h"
#include "igamemovement.h"
#include "in_buttons.h"
#include "ipredictionsystem.h"
#include "portal_player.h"
#include "portal2_movedata.h"

static CPortal2_MoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;

IPredictionSystem *IPredictionSystem::g_pPredictionSystems = NULL;

extern IGameMovement *g_pGameMovement;
extern ConVar sv_noclipduringpause;

//-----------------------------------------------------------------------------
// Sets up the move data for Portal 2
//-----------------------------------------------------------------------------
class CPortal2_PlayerMove : public CPlayerMove
{
	DECLARE_CLASS(CPortal2_PlayerMove, CPlayerMove);

public:
	virtual void	SetupMove(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move);
	virtual void	RunCommand(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper);
};

// PlayerMove Interface
static CPortal2_PlayerMove g_PlayerMove;

//-----------------------------------------------------------------------------
// Singleton accessor
//-----------------------------------------------------------------------------
CPlayerMove *PlayerMove()
{
	return &g_PlayerMove;
}

//-----------------------------------------------------------------------------
// Purpose: This is called pre player movement and copies all the data necessary
//          from the player for movement. (Server-side, the client-side version
//          of this code can be found in prediction.cpp.)
//-----------------------------------------------------------------------------
void CPortal2_PlayerMove::SetupMove(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move)
{
	//player->AvoidPhysicsProps( ucmd );

	BaseClass::SetupMove(player, ucmd, pHelper, move);

	// setup trace optimization
	g_pGameMovement->SetupMovementBounds(move);
}

void CPortal2_PlayerMove::RunCommand(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper)
{
	BaseClass::RunCommand(player, ucmd, moveHelper);
}
