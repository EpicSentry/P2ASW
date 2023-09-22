//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef PAINT_SWAPGUNS_H
#define PAINT_SWAPGUNS_H

class CPortal_Player;

bool CheckSwapProximity( CPortal_Player *pPaintGunPlayer, CPortal_Player *pPortalGunPlayer );
void SwapPaintAndPortalGuns( CPortal_Player *pPaintGunPlayer, CPortal_Player *pPortalGunPlayer );
void CopyWeaponInfo( CBaseCombatWeapon* src, CBaseCombatWeapon* dest );

#endif //PAINT_SWAPGUNS_H
