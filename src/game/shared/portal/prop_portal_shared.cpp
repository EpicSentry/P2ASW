//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "portal_player_shared.h"

#ifdef CLIENT_DLL
#include "c_basedoor.h"
#include "c_portal_player.h"
#else
#include "portal_player.h"
#endif

CUtlVector<CProp_Portal *> CProp_Portal_Shared::AllPortals;

const Vector CProp_Portal_Shared::vLocalMins( 0.0f, -PORTAL_HALF_WIDTH, -PORTAL_HALF_HEIGHT );
const Vector CProp_Portal_Shared::vLocalMaxs( 64.0f, PORTAL_HALF_WIDTH, PORTAL_HALF_HEIGHT );

void CProp_Portal_Shared::UpdatePortalTransformationMatrix( const matrix3x4_t &localToWorld, const matrix3x4_t &remoteToWorld, VMatrix *pMatrix )
{
	VMatrix matPortal1ToWorldInv, matPortal2ToWorld, matRotation;

	//inverse of this
	MatrixInverseTR( localToWorld, matPortal1ToWorldInv );

	//180 degree rotation about up
	matRotation.Identity();
	matRotation.m[0][0] = -1.0f;
	matRotation.m[1][1] = -1.0f;

	//final
	matPortal2ToWorld = remoteToWorld;	
	*pMatrix = matPortal2ToWorld * matRotation * matPortal1ToWorldInv;
}

static char *g_pszPortalNonTeleportable[] = 
{ 
	"func_door", 
	"func_door_rotating", 
	"prop_door_rotating",
	"func_tracktrain",
	//"env_ghostanimating",
	"physicsshadowclone"
};

bool CProp_Portal_Shared::IsEntityTeleportable( CBaseEntity *pEntity )
{

	do
	{

#ifdef CLIENT_DLL
		//client
	
		if( dynamic_cast<C_BaseDoor *>(pEntity) != NULL )
			return false;

#else
		//server
		
		for( int i = 0; i != ARRAYSIZE(g_pszPortalNonTeleportable); ++i )
		{
			if( FClassnameIs( pEntity, g_pszPortalNonTeleportable[i] ) )
				return false;
		}

#endif

		Assert( pEntity != pEntity->GetMoveParent() );
		pEntity = pEntity->GetMoveParent();
	} while( pEntity );

	return true;
}






void CProp_Portal::PortalSimulator_TookOwnershipOfEntity( CBaseEntity *pEntity )
{
	if( pEntity->IsPlayer() )
		((CPortal_Player *)pEntity)->m_hPortalEnvironment = this;
}

void CProp_Portal::PortalSimulator_ReleasedOwnershipOfEntity( CBaseEntity *pEntity )
{
	if( pEntity->IsPlayer() && (((CPortal_Player *)pEntity)->m_hPortalEnvironment.Get() == this) )
		((CPortal_Player *)pEntity)->m_hPortalEnvironment = NULL;
}

extern ConVar sv_gravity;

float CProp_Portal::GetMinimumExitSpeed( bool bPlayer, bool bEntranceOnFloor, bool bExitOnFloor, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity )
{
	if( bExitOnFloor )
	{
		if( bPlayer )
		{
			return 300.0f;
		}
		else
		{
			return bEntranceOnFloor ? 225.0f : 50.0f;
		}
	}
	else if( bPlayer )
	{
		//bExitOnFloor means the portal is facing almost entirely up, just because it's false doesn't mean the portal isn't facing significantly up
		//We also need to solve the case where the player's AABB rotates in such a way that we pull the ground out from under them
		if( m_vForward.z > 0.5f ) //forward facing up by at least 30 degrees
		{
			float fGravity = GetGravity();
			if ( fGravity != 0.0f )
			{
				fGravity *= sv_gravity.GetFloat();
			}
			else
			{
				fGravity = sv_gravity.GetFloat();
			}

			if( fGravity != 0.0f )
			{
				//Assuming our current velocity is zero. What's the minimum portal-forward velocity to perch the player on the bottom edge of the portal?
				Vector vPerchPoint = m_ptOrigin - (m_vUp * GetHalfHeight()); //a point along the bottom edge of the portal, horizontally centered
				Vector vPlayerExtents = (((CPortal_Player *)pEntity)->GetHullMaxs() - ((CPortal_Player *)pEntity)->GetHullMins()) * 0.5f;
				//Vector vPlayerCenterToPerch = vPerchPoint - vEntityCenterAtExit;
				Vector vTestBBoxPoint = vEntityCenterAtExit;
				//vTestBBoxPoint.x += Sign( vPlayerCenterToPerch.x ) * vPlayerExtents.x;
				//vTestBBoxPoint.y += Sign( vPlayerCenterToPerch.y ) * vPlayerExtents.y;
				vTestBBoxPoint.z -= vPlayerExtents.z;

				
				Vector vTestToPerch = vPerchPoint - vTestBBoxPoint;
				vTestToPerch -= vTestToPerch.Dot( m_vRight ) * m_vRight; //Project test vector onto horizontal center, so all x/y dist to perch point is actually distance to perch line
				float fHorzTestToPerch = vTestToPerch.Length2D();
				float fHorzVelocityComponent = m_vForward.Length2D(); //the portion of our velocity axis that will move us horizontally toward the perch
			

				float fRoot1, fRoot2;
				if( SolveQuadratic( (m_vForward.z * (-2.0f)) * ((fHorzTestToPerch * fHorzVelocityComponent) - (vTestToPerch.z*m_vForward.z)), 0, fHorzTestToPerch * fHorzTestToPerch * fGravity, fRoot1, fRoot2 ) )
				{
					float fMax = MAX( fRoot1, fRoot2 );
					if( fMax > 0.0f )
					{
						if( fMax > 300.0f ) //cap out at floor/floor minimum
							return 300.0f;
						else
							return fMax;
					}
				}
			}	
		}
	}

	return -FLT_MAX;
	//return BaseClass::GetMinimumExitSpeed( bPlayer, bEntranceOnFloor, bExitOnFloor, vEntityCenterAtExit, pEntity );
}

float CProp_Portal::GetMaximumExitSpeed( bool bPlayer, bool bEntranceOnFloor, bool bExitOnFloor, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity )
{
	return 1000.0f;
}

void CProp_Portal::GetExitSpeedRange( CPortal_Base2D *pEntrancePortal, bool bPlayer, float &fExitMinimum, float &fExitMaximum, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity )
{
	CPortal_Base2D *pExitPortal = pEntrancePortal ? pEntrancePortal->m_hLinkedPortal.Get() : NULL;
	if( !pExitPortal )
	{
		fExitMinimum = -FLT_MAX;
		fExitMaximum = FLT_MAX;
		return;
	}
		
	const float COS_PI_OVER_SIX = 0.86602540378443864676372317075294f; // cos( 30 degrees ) in radians
	bool bEntranceOnFloor = pEntrancePortal->m_plane_Origin.normal.z > COS_PI_OVER_SIX;
	bool bExitOnFloor = pExitPortal->m_plane_Origin.normal.z > COS_PI_OVER_SIX;

	fExitMinimum = pExitPortal->GetMinimumExitSpeed( bPlayer, bEntranceOnFloor, bExitOnFloor, vEntityCenterAtExit, pEntity );
	fExitMaximum = pExitPortal->GetMaximumExitSpeed( bPlayer, bEntranceOnFloor, bExitOnFloor, vEntityCenterAtExit, pEntity );
}

bool CProp_Portal::IsFloorPortal( float fThreshold ) const
{
	return m_PortalSimulator.GetInternalData().Placement.vForward.z > fThreshold;
}

bool CProp_Portal::IsCeilingPortal( float fThreshold ) const
{
	return m_PortalSimulator.GetInternalData().Placement.vForward.z < fThreshold;
}