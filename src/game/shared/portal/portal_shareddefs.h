//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef PORTAL_SHAREDDEFS_H
#define PORTAL_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
//#define NO_TRACTOR_BEAM
#endif


#define PORTAL2_MP_SAVE_FILE "coop_data.txt"
#define PORTAL2_MP_TEAM_TAUNT_FORCE_LENGTH 64

#define PORTAL_PLAYER_PREDICTION

#ifdef DYNAMIC_BOUNDS
#define PORTAL_HALF_WIDTH portal_half_width.GetInt()
#define PORTAL_HALF_HEIGHT portal_half_height.GetInt()
#endif

#define PORTAL_HALF_DEPTH 2.0f
#define PORTAL_BUMP_FORGIVENESS 2.0f

enum PortalPlacementResult_t
{
	// Success cases
	PORTAL_PLACEMENT_SUCCESS,				// Placed exactly where shot
	PORTAL_PLACEMENT_USED_HELPER,			// A placement helper positioned this portal
	PORTAL_PLACEMENT_BUMPED,				// Placed but needed to move to fit
	
	// Fail cases
	PORTAL_PLACEMENT_CANT_FIT,				// No space to fit
	PORTAL_PLACEMENT_CLEANSER,				// Hit a cleanser
	PORTAL_PLACEMENT_OVERLAP_LINKED,		// Hit a linked portal that cannot move
	PORTAL_PLACEMENT_OVERLAP_PARTNER_PORTAL,// Hit a partner portal that cannot move
	PORTAL_PLACEMENT_INVALID_VOLUME,		// Inside a "no portal" volume
	PORTAL_PLACEMENT_INVALID_SURFACE,		// Hit a "no portal" surface
	PORTAL_PLACEMENT_PASSTHROUGH_SURFACE,	// Error case (portal trace failed to hit anything)
};

#define PORTAL_ANALOG_SUCCESS_NO_BUMP 1.0f
#define PORTAL_ANALOG_SUCCESS_BUMPED 0.3f
#define PORTAL_ANALOG_SUCCESS_CANT_FIT 0.1f
#define PORTAL_ANALOG_SUCCESS_CLEANSER 0.028f
#define PORTAL_ANALOG_SUCCESS_OVERLAP_LINKED 0.027f
#define PORTAL_ANALOG_SUCCESS_INVALID_VOLUME 0.026f
#define PORTAL_ANALOG_SUCCESS_INVALID_SURFACE 0.025f
#define PORTAL_ANALOG_SUCCESS_PASSTHROUGH_SURFACE 0.0f

#define MIN_FLING_SPEED 300

#define PORTAL_HIDE_PLAYER_RAGDOLL 1

enum PortalFizzleType_t
{
	PORTAL_FIZZLE_SUCCESS = 0,			// Placed fine (no fizzle)
	PORTAL_FIZZLE_CANT_FIT,
	PORTAL_FIZZLE_OVERLAPPED_LINKED,
	PORTAL_FIZZLE_BAD_VOLUME,
	PORTAL_FIZZLE_BAD_SURFACE,
	PORTAL_FIZZLE_KILLED,
	PORTAL_FIZZLE_CLEANSER,
	PORTAL_FIZZLE_CLOSE,
	PORTAL_FIZZLE_NEAR_BLUE,
	PORTAL_FIZZLE_NEAR_RED,
	PORTAL_FIZZLE_NONE,

	NUM_PORTAL_FIZZLE_TYPES
};


enum PortalPlacedBy_t
{
	PORTAL_PLACED_BY_FIXED = 0,
	PORTAL_PLACED_BY_PEDESTAL,
	PORTAL_PLACED_BY_PLAYER
};

enum PortalLevelStatType
{
	PORTAL_LEVEL_STAT_NUM_PORTALS = 0,
	PORTAL_LEVEL_STAT_NUM_STEPS,
	PORTAL_LEVEL_STAT_NUM_SECONDS,

	PORTAL_LEVEL_STAT_TOTAL
};

enum PortalChallengeType
{
	PORTAL_CHALLENGE_NONE = 0,
	PORTAL_CHALLENGE_PORTALS,
	PORTAL_CHALLENGE_STEPS,
	PORTAL_CHALLENGE_TIME,

	PORTAL_CHALLENGE_TOTAL
};

enum PortalEvent_t
{
	PORTALEVENT_LINKED,					// This portal has linked to another portal and opened
	PORTALEVENT_FIZZLE,					// Portal has fizzled 
	PORTALEVENT_MOVED,					// Portal has moved its position
	PORTALEVENT_ENTITY_TELEPORTED_TO,	// Entity (player or not) has teleported to this portal
	PORTALEVENT_ENTITY_TELEPORTED_FROM,	// Entity (player or not) has teleported away from this portal
	PORTALEVENT_PLAYER_TELEPORTED_TO,	// Player has teleported to this portal
	PORTALEVENT_PLAYER_TELEPORTED_FROM,	// Player has teleported away from this portal
};

enum TeamTauntState_t
{
	TEAM_TAUNT_NONE,
	TEAM_TAUNT_NEED_PARTNER,
	TEAM_TAUNT_HAS_PARTNER,
	TEAM_TAUNT_SUCCESS,
	TEAM_TAUNT_STATE_TOTAL
};

enum PortalSurfaceType_t
{
	PORTAL_SURFACE_PAINT = 0,
	PORTAL_SURFACE_VALID = 1,
	PORTAL_SURFACE_INVALID
};

#define PORTAL2_PROMO_SKINS		( 1 << 0 )
#define PORTAL2_PROMO_HELMETS	( 1 << 1 )
#define PORTAL2_PROMO_ANTENNA	( 1 << 2 )

extern int g_nPortal2PromoFlags;

extern char *g_ppszPortalPassThroughMaterials[];

#endif // PORTAL_SHAREDDEFS_H
