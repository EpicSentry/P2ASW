//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"

#include "paint_swap_guns.h"
#include "portal_player.h"
#include "weapon_paintgun.h"
#include "weapon_portalgun.h"
#include "mathlib/ssemath.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define sv_max_swap_distance			100.f	//ConVar sv_max_swap_distance( "sv_max_swap_distance", "100", FCVAR_CHEAT );
#define sv_max_swap_look_angle			50		//ConVar sv_max_swap_look_angle( "sv_max_swap_look_angle", "50", FCVAR_CHEAT );
#define sv_max_swap_portal_look_angle	50		//ConVar sv_max_swap_portal_look_angle( "sv_max_swap_portal_look_angle", "50", FCVAR_CHEAT );
#define g_debug_swap_guns_trace			0		//ConVar g_debug_swap_guns_trace( "g_debug_swap_guns_trace", "0", FCVAR_CHEAT );
//extern ConVar sv_can_swap_guns;
extern void PaintPowerPickup( int colorIndex, CBasePlayer *pPlayer );

enum SwapGunTraceResult
{
	SWAP_TRACE_HIT_NOTHING = 0,
	SWAP_TRACE_HIT_SOMETHING
};


void CheckSwapPaintGun( CBaseCombatWeapon *pWeapon )
{
	CWeaponPaintGun *pPaintGun = dynamic_cast< CWeaponPaintGun* >( pWeapon );
	if ( pPaintGun )
	{
		PaintPowerPickup( pPaintGun->GetCurrentPaint(), ToBasePlayer( pPaintGun->GetOwner() ) );
	}	
}


void SwapPaintAndPortalGuns( CPortal_Player *pPlayer1, CPortal_Player *pPlayer2 )
{
	CBaseCombatWeapon *pWeapon1 = pPlayer1->GetActiveWeapon();
	CBaseCombatWeapon *pWeapon2 = pPlayer2->GetActiveWeapon();

	CWeaponPortalgun *pPortalGun = NULL;

	if( pWeapon1 && FClassnameIs( pWeapon1, "weapon_portalgun" ) )
	{
		pPortalGun = dynamic_cast<CWeaponPortalgun*>( pWeapon1 );
	}
	else if( pWeapon2 && FClassnameIs( pWeapon2, "weapon_portalgun" ) )
	{
		pPortalGun = dynamic_cast<CWeaponPortalgun*>( pWeapon2 );
	}

	char linkageID = pPortalGun->GetLinkageGroupID();

	pPlayer1->Weapon_Drop( pWeapon1, NULL, NULL );
	pPlayer2->Weapon_Drop( pWeapon2, NULL, NULL );

	if ( pWeapon1 )
	{
		pPlayer2->Weapon_Equip( pWeapon1 );
		CheckSwapPaintGun( pWeapon1 );
	}

	if ( pWeapon2 )
	{
		pPlayer1->Weapon_Equip( pWeapon2 );
		CheckSwapPaintGun( pWeapon2 );
	}

	pPortalGun->SetLinkageGroupID( linkageID );
}


SwapGunTraceResult SwapGunsTraceLine( const Vector& vecStartPos, const Vector& vecEndPos, CBaseEntity *pStartEnt, trace_t *trace )
{
	CTraceFilterSimpleList filter( COLLISION_GROUP_PROJECTILE );
	filter.AddEntityToIgnore( pStartEnt );

	UTIL_TraceLine( vecStartPos, vecEndPos, MASK_SOLID , &filter, trace );

	if( g_debug_swap_guns_trace )
	{
		NDebugOverlay::Line( trace->endpos, vecEndPos, 0, 255, 0, true, 0.1f );
		NDebugOverlay::Line( trace->startpos, trace->endpos, 255, 0, 0, true, 0.1f );
		NDebugOverlay::Sphere( vecEndPos, 2, 0, 255, 0, true, 0.1f );
		NDebugOverlay::Sphere( trace->endpos, 2, 0, 0, 255, true, 0.1f );
	}


	CBaseEntity *list[1024];

	Ray_t ray;
	ray.Init( vecStartPos, vecEndPos );

	int nCount = UTIL_EntitiesAlongRay( list, 1024, ray, 0 );

	for( int i = 0; i < nCount; ++i )
	{
		if( FClassnameIs( list[i], "trigger_paint_cleanser" ) )
		{
			return SWAP_TRACE_HIT_SOMETHING;
		}
		else if( FClassnameIs( list[i], "trigger_portal_cleanser" ) )
		{
			return SWAP_TRACE_HIT_SOMETHING;
		}
	}

	// If we just hit the player or the trace went all the way then we say we didn't hit anything
	if( ( trace->DidHit() && trace->m_pEnt->IsPlayer() ) ||
		trace->fraction == 1.f )
	{
		return SWAP_TRACE_HIT_NOTHING;
	}

	return SWAP_TRACE_HIT_SOMETHING;
}


bool CheckSwapThroughPortals( CPortal_Player *pPlayer1, CProp_Portal *pPlayer1Portal, CPortal_Player *pPlayer2, CProp_Portal *pPlayer2Portal )
{
	//Trace from the paint gun player to the paint gun player portal
	trace_t tr;
	SwapGunTraceResult result = SwapGunsTraceLine( pPlayer1->EyePosition(), pPlayer1Portal->GetAbsOrigin(), pPlayer1, &tr );

	//If the trace hit the paint gun player portal
	if( result == SWAP_TRACE_HIT_NOTHING && tr.fraction == 1.0f )
	{
		//Trace from the portal gun player portal to the portal gun player
		trace_t tr2;
		result = SwapGunsTraceLine( pPlayer2Portal->GetAbsOrigin(), pPlayer2->EyePosition(), pPlayer2Portal, &tr2 );

		//If the trace hit the portal gun player
		if( result == SWAP_TRACE_HIT_NOTHING && tr.fraction == 1.0f )
		{
			return true;
		}
	}

	return false;
}


bool CanSeePositionThroughPortal( const Vector &vecStartPos, const Vector &vecForwardDir, const Vector &vecEndPos, CProp_Portal *pPortal )
{
	Vector vecNewEndPos;
	UTIL_Portal_PointTransform( pPortal->m_hLinkedPortal->MatrixThisToLinked(), vecEndPos, vecNewEndPos );

	Vector vecNewEndDir = vecNewEndPos - vecStartPos;
	VectorNormalize( vecNewEndDir );

	float flAngleDiff = RAD2DEG( acos( DotProduct( vecForwardDir, vecNewEndDir ) ) );

	return flAngleDiff <= sv_max_swap_look_angle;
}


bool CheckSwapProximity( CPortal_Player *pPlayer1, CPortal_Player *pPlayer2 )
{
	//Get the positions of both the players
	Vector vecPaintGunPlayerPos = pPlayer1->EyePosition();
	Vector vecPortalGunPlayerPos = pPlayer2->EyePosition();

	//Get the look direction of each player
	Vector vecPaintPlayerForward, vecPortalPlayerForward;
	AngleVectors( pPlayer1->EyeAngles(), &vecPaintPlayerForward );
	VectorNormalize( vecPaintPlayerForward );
	AngleVectors( pPlayer2->EyeAngles(), &vecPortalPlayerForward );
	VectorNormalize( vecPortalPlayerForward );

	//Test straight line path
	Vector vecStraightLineDir = vecPortalGunPlayerPos - vecPaintGunPlayerPos;
	float flStraightLineDistance = vecStraightLineDir.LengthSqr();
	VectorNormalize( vecStraightLineDir );

	//Get the square of the max swap distance
	float flMaxSwapDistance = pow( sv_max_swap_distance, 2.f );

	//If the players are close enough to each other
	if( flStraightLineDistance <= flMaxSwapDistance )
	{
		//Get the look angles of each player
		float flPaintPlayerAngleDiff = RAD2DEG( acos( DotProduct( vecPaintPlayerForward, vecStraightLineDir ) ) );
		float flPortalPlayerAngleDiff = RAD2DEG( acos( DotProduct( vecPortalPlayerForward, -vecStraightLineDir ) ) );

		//If both the players are look at each other
		if( flPaintPlayerAngleDiff <= sv_max_swap_look_angle && flPortalPlayerAngleDiff <= sv_max_swap_look_angle )
		{
			//Trace a line between the players to see if anything is between them
			trace_t trace;
			SwapGunTraceResult result = SwapGunsTraceLine( vecPaintGunPlayerPos, vecPortalGunPlayerPos, pPlayer2, &trace );

			//If the trace hit the portal gun player
			if( result == SWAP_TRACE_HIT_NOTHING )
			{
				return true;
			}
		}
	}

	// Assume we only have 1 portalgun in the world
	CWeaponPortalgun *pPortalGun = dynamic_cast< CWeaponPortalgun* >( gEntList.FindEntityByClassname( NULL, "weapon_portalgun" ) );

	if ( !pPortalGun )
		return false;

	//Get both the portals
	CProp_Portal *pPortal1 = CProp_Portal::FindPortal( pPortalGun->GetLinkageGroupID(), false );
	CProp_Portal *pPortal2 = CProp_Portal::FindPortal( pPortalGun->GetLinkageGroupID(), true );

	if( pPortal1 && pPortal1->IsActive() && pPortal2 && pPortal2->IsActive() )
	{
		//Check if the paint player is looking through a portal
		Vector vecPortal1PaintGunPlayerDir = pPortal1->GetAbsOrigin() - vecPaintGunPlayerPos;
		VectorNormalize( vecPortal1PaintGunPlayerDir );
		Vector vecPortal2PaintGunPlayerDir = pPortal2->GetAbsOrigin() - vecPaintGunPlayerPos;
		VectorNormalize( vecPortal2PaintGunPlayerDir );

		float flPaintPlayerPortal1Angle = RAD2DEG( acos( DotProduct( vecPaintPlayerForward, vecPortal1PaintGunPlayerDir ) ) );
		float flPaintPlayerPortal2Angle = RAD2DEG( acos( DotProduct( vecPaintPlayerForward, vecPortal2PaintGunPlayerDir ) ) );

		//Check if the portal player is looking through a portal
		Vector vecPortal1PortalGunPlayerDir = pPortal1->GetAbsOrigin() - vecPortalGunPlayerPos;
		VectorNormalize( vecPortal1PortalGunPlayerDir );
		Vector vecPortal2PortalGunPlayerDir = pPortal2->GetAbsOrigin() - vecPortalGunPlayerPos;
		VectorNormalize( vecPortal2PortalGunPlayerDir );

		float flPortalPlayerPortal1Angle = RAD2DEG( acos( DotProduct( vecPortalPlayerForward, vecPortal1PortalGunPlayerDir ) ) );
		float flPortalPlayerPortal2Angle = RAD2DEG( acos( DotProduct( vecPortalPlayerForward, vecPortal2PortalGunPlayerDir ) ) );

		//If the paint player is looking through portal1 and the portal player is looking through portal2
		if( flPaintPlayerPortal1Angle <= sv_max_swap_portal_look_angle && flPortalPlayerPortal2Angle <= sv_max_swap_portal_look_angle )
		{
			bool bCanSeeOtherPlayer = CanSeePositionThroughPortal( vecPaintGunPlayerPos, vecPaintPlayerForward, vecPortalGunPlayerPos, pPortal1 );

			//If the distance through portal 1 is close enough
			float flThroughPortal1Distance = UTIL_Portal_DistanceThroughPortalSqr( pPortal1, vecPaintGunPlayerPos, vecPortalGunPlayerPos );
			if( bCanSeeOtherPlayer && flThroughPortal1Distance != -1.0f && flThroughPortal1Distance <= flMaxSwapDistance )
			{
				return CheckSwapThroughPortals( pPlayer1, pPortal1, pPlayer2, pPortal2 );
			}
		}
		//If the paint player is looking through portal2 and the portal player is looking through portal1
		else if( flPaintPlayerPortal2Angle <= sv_max_swap_portal_look_angle && flPortalPlayerPortal1Angle <= sv_max_swap_portal_look_angle )
		{
			bool bCanSeeOtherPlayer = CanSeePositionThroughPortal( vecPaintGunPlayerPos, vecPaintPlayerForward, vecPortalGunPlayerPos, pPortal2 );

			//If the distance through portal 2 is close enough
			float flThroughPortal2Distance = UTIL_Portal_DistanceThroughPortalSqr( pPortal2, vecPaintGunPlayerPos, vecPortalGunPlayerPos );
			if( bCanSeeOtherPlayer && flThroughPortal2Distance != -1.0f && flThroughPortal2Distance <= flMaxSwapDistance )
			{
				return CheckSwapThroughPortals( pPlayer1, pPortal2, pPlayer2, pPortal1 );
			}
		}
	}

	return false;
}


//CON_COMMAND( swap_paint_and_portal_guns, "Swaps the paint and portal guns if allowed" )
//{
//	//Do nothing if we can't swap guns
//	if( !sv_can_swap_guns.GetBool() ||
//		gpGlobals->maxClients <= 1 )
//	{
//		return;
//	}
//
//	//Get both the active players
//	CPortal_Player *pPlayer1 = GetPortalPlayer( 1 );
//	CPortal_Player *pPlayer2 = GetPortalPlayer( 2 );
//
//	//If both players are valid
//	if( pPlayer1 != NULL && pPlayer1->IsConnected() &&
//		pPlayer2 != NULL && pPlayer2->IsConnected() &&
//		pPlayer1 != pPlayer2 )
//	{
//		SwapPaintAndPortalGuns( pPlayer1, pPlayer2 );
//	}
//}
