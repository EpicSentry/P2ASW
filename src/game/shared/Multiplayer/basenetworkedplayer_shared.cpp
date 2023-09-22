#include "cbase.h"
#ifdef GAME_DLL
#include "multiplayer/basenetworkedplayer.h"
#else
#include "multiplayer/basenetworkedplayer_cl.h"
#endif

ConVar player_normspeed("player_normspeed", "175", FCVAR_NEVER_AS_STRING | FCVAR_REPLICATED);
ConVar player_walkspeed("player_walkspeed", "175", FCVAR_NEVER_AS_STRING | FCVAR_REPLICATED);
ConVar player_crouchspeed("player_crouchspeed", "58.33", FCVAR_NEVER_AS_STRING | FCVAR_REPLICATED);

void CBaseNetworkedPlayer::MakeAnimState()
{
#ifdef CLIENT_DLL
	MDLCACHE_CRITICAL_SECTION();
#endif
	MultiPlayerMovementData_t mv;
	mv.m_flBodyYawRate = 360;
	mv.m_flRunSpeed = player_normspeed.GetInt();
	mv.m_flWalkSpeed = player_walkspeed.GetInt();
	mv.m_flSprintSpeed = -1.0f;
	m_PlayerAnimState = new CMultiPlayerAnimState( this,mv );
}