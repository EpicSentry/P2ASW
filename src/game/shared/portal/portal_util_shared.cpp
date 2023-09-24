//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "portal_util_shared.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "portal_collideable_enumerator.h"
#include "beam_shared.h"
#include "CollisionUtils.h"
#include "util_shared.h"
#ifndef CLIENT_DLL
	#include "Util.h"
	#include "NDebugOverlay.h"
	#include "env_debughistory.h"
#else
	#include "c_portal_player.h"
#endif
#include "PortalSimulation.h"
#include "paint/paint_color_manager.h"
#include "CegClientWrapper.h"
#include "portal_mp_gamerules.h"

bool g_bAllowForcePortalTrace = false;
bool g_bForcePortalTrace = false;
bool g_bBulletPortalTrace = false;
// paint convars
ConVar sv_paint_detection_sphere_radius( "sv_paint_detection_sphere_radius", "16.f", FCVAR_REPLICATED | FCVAR_CHEAT, "The radius of the query sphere used to find the color of a light map at a contact point in world space." );


ConVar sv_portal_trace_vs_world ("sv_portal_trace_vs_world", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Use traces against portal environment world geometry" );
ConVar sv_portal_trace_vs_displacements ("sv_portal_trace_vs_displacements", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Use traces against portal environment displacement geometry" );
ConVar sv_portal_trace_vs_holywall ("sv_portal_trace_vs_holywall", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Use traces against portal environment carved wall" );
ConVar sv_portal_trace_vs_staticprops ("sv_portal_trace_vs_staticprops", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Use traces against portal environment static prop geometry" );
ConVar sv_use_find_closest_passable_space ("sv_use_find_closest_passable_space", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Enables heavy-handed player teleporting stuck fix code." );
ConVar sv_use_transformed_collideables("sv_use_transformed_collideables", "1", FCVAR_REPLICATED | FCVAR_CHEAT, "Disables traces against remote portal moving entities using transforms to bring them into local space." );
class CTransformedCollideable : public ICollideable //wraps an existing collideable, but transforms everything that pertains to world space by another transform
{
public:
	VMatrix m_matTransform; //the transformation we apply to the wrapped collideable
	VMatrix m_matInvTransform; //cached inverse of m_matTransform

	ICollideable *m_pWrappedCollideable; //the collideable we're transforming without it knowing

	struct CTC_ReferenceVars_t
	{
		Vector m_vCollisionOrigin;
		QAngle m_qCollisionAngles;
		matrix3x4_t m_matCollisionToWorldTransform;
		matrix3x4_t m_matRootParentToWorldTransform;
	}; 

	mutable CTC_ReferenceVars_t m_ReferencedVars; //when returning a const reference, it needs to point to something, so here we go

	//abstract functions which require no transforms, just pass them along to the wrapped collideable
	virtual IHandleEntity	*GetEntityHandle() { return m_pWrappedCollideable->GetEntityHandle(); }
	virtual const Vector&	OBBMins() const { return m_pWrappedCollideable->OBBMins(); };
	virtual const Vector&	OBBMaxs() const { return m_pWrappedCollideable->OBBMaxs(); };
	virtual int				GetCollisionModelIndex() { return m_pWrappedCollideable->GetCollisionModelIndex(); };
	virtual const model_t*	GetCollisionModel() { return m_pWrappedCollideable->GetCollisionModel(); };
	virtual SolidType_t		GetSolid() const { return m_pWrappedCollideable->GetSolid(); };
	virtual int				GetSolidFlags() const { return m_pWrappedCollideable->GetSolidFlags(); };
	virtual IClientUnknown*	GetIClientUnknown() { return m_pWrappedCollideable->GetIClientUnknown(); };
	virtual int				GetCollisionGroup() const { return m_pWrappedCollideable->GetCollisionGroup(); };
	virtual bool			ShouldTouchTrigger( int triggerSolidFlags ) const { return m_pWrappedCollideable->ShouldTouchTrigger(triggerSolidFlags); };

	//slightly trickier functions
	virtual void			WorldSpaceTriggerBounds( Vector *pVecWorldMins, Vector *pVecWorldMaxs ) const;
	virtual bool			TestCollision( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr );
	virtual bool			TestHitboxes( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr );
	virtual const Vector&	GetCollisionOrigin() const;
	virtual const QAngle&	GetCollisionAngles() const;
	virtual const matrix3x4_t&	CollisionToWorldTransform() const;
	virtual void			WorldSpaceSurroundingBounds( Vector *pVecMins, Vector *pVecMaxs );
	virtual const matrix3x4_t	*GetRootParentToWorldTransform() const;
};

void CTransformedCollideable::WorldSpaceTriggerBounds( Vector *pVecWorldMins, Vector *pVecWorldMaxs ) const
{
	m_pWrappedCollideable->WorldSpaceTriggerBounds( pVecWorldMins, pVecWorldMaxs );
	
	if( pVecWorldMins )
		*pVecWorldMins = m_matTransform * (*pVecWorldMins);

	if( pVecWorldMaxs )
		*pVecWorldMaxs = m_matTransform * (*pVecWorldMaxs);
}

bool CTransformedCollideable::TestCollision( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr )
{
	//TODO: Transform the ray by inverse matTransform and transform the trace results by matTransform? AABB Errors arise by transforming the ray.
    return m_pWrappedCollideable->TestCollision( ray, fContentsMask, tr );
}

bool CTransformedCollideable::TestHitboxes( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr )
{
	//TODO: Transform the ray by inverse matTransform and transform the trace results by matTransform? AABB Errors arise by transforming the ray.
	return m_pWrappedCollideable->TestHitboxes( ray, fContentsMask, tr );
}

const Vector& CTransformedCollideable::GetCollisionOrigin() const
{
	m_ReferencedVars.m_vCollisionOrigin = m_matTransform * m_pWrappedCollideable->GetCollisionOrigin();
	return m_ReferencedVars.m_vCollisionOrigin;
}

const QAngle& CTransformedCollideable::GetCollisionAngles() const
{
	m_ReferencedVars.m_qCollisionAngles = TransformAnglesToWorldSpace( m_pWrappedCollideable->GetCollisionAngles(), m_matTransform.As3x4() );
	return m_ReferencedVars.m_qCollisionAngles;
}

const matrix3x4_t& CTransformedCollideable::CollisionToWorldTransform() const
{
	//1-2 order correct?
	ConcatTransforms( m_matTransform.As3x4(), m_pWrappedCollideable->CollisionToWorldTransform(), m_ReferencedVars.m_matCollisionToWorldTransform );
	return m_ReferencedVars.m_matCollisionToWorldTransform;
}

void CTransformedCollideable::WorldSpaceSurroundingBounds( Vector *pVecMins, Vector *pVecMaxs )
{
	if( (pVecMins == NULL) && (pVecMaxs == NULL) )
		return;

	Vector vMins, vMaxs;
	m_pWrappedCollideable->WorldSpaceSurroundingBounds( &vMins, &vMaxs );

	TransformAABB( m_matTransform.As3x4(), vMins, vMaxs, vMins, vMaxs );

	if( pVecMins )
		*pVecMins = vMins;
	if( pVecMaxs )
		*pVecMaxs = vMaxs;
}

const matrix3x4_t* CTransformedCollideable::GetRootParentToWorldTransform() const
{
	const matrix3x4_t *pWrappedVersion = m_pWrappedCollideable->GetRootParentToWorldTransform();
	if( pWrappedVersion == NULL )
		return NULL;

	ConcatTransforms( m_matTransform.As3x4(), *pWrappedVersion, m_ReferencedVars.m_matRootParentToWorldTransform );
	return &m_ReferencedVars.m_matRootParentToWorldTransform;
}

#if defined ( CLIENT_DLL )
static const Color s_defaultPortalColors[2] = { Color( 64, 160, 255, 255 ), Color( 255, 160, 32, 255 ) };

Color UTIL_Portal_Color( int iPortal, int iTeamNumber /*= 0*/ )
{
	switch ( iPortal )
	{
		case 0:
			// GRAVITY BEAM
			return Color( 242, 202, 167, 255 );
		case 1:
			return Color( 64, 160, 255, 255 );
		case 2:
			return Color(255, 160, 32, 255);
		break;
	}

	Assert( 0 );
	return Color( 255, 255, 255, 255 );
}

Color UTIL_Portal_Color_Particles( int iPortal, int iTeamNumber /*= 0*/ )
{
	if ( GameRules()->IsMultiplayer() && !((CPortalMPGameRules *)g_pGameRules)->Is2GunsCoOp() )
		return UTIL_Portal_Color( iPortal, iTeamNumber );
	else
		return (iPortal - 1)?( Color( 233, 78, 2, 255 ) ):( Color( 0, 60, 255, 255 ) );
}
#endif
void UTIL_Portal_Trace_Filter( CTraceFilterSimpleClassnameList *traceFilterPortalShot )
{
	traceFilterPortalShot->AddClassnameToIgnore( "prop_physics" );
	traceFilterPortalShot->AddClassnameToIgnore( "func_physbox" );
	traceFilterPortalShot->AddClassnameToIgnore( "npc_portal_turret_floor" );
	traceFilterPortalShot->AddClassnameToIgnore( "prop_energy_ball" );
	traceFilterPortalShot->AddClassnameToIgnore( "npc_security_camera" );
	traceFilterPortalShot->AddClassnameToIgnore( "player" );
	traceFilterPortalShot->AddClassnameToIgnore( "simple_physics_prop" );
	traceFilterPortalShot->AddClassnameToIgnore( "simple_physics_brush" );
	traceFilterPortalShot->AddClassnameToIgnore( "prop_ragdoll" );
	traceFilterPortalShot->AddClassnameToIgnore( "prop_glados_core" );
#ifdef PORTAL2
	traceFilterPortalShot->AddClassnameToIgnore("npc_wheatley_core");
	traceFilterPortalShot->AddClassnameToIgnore("prop_weighted_cube");
#endif
}


CProp_Portal* UTIL_Portal_FirstAlongRay( const Ray_t &ray, float &fMustBeCloserThan )
{
	CProp_Portal *pIntersectedPortal = NULL;

	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount != 0 )
	{
		CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();

		for( int i = 0; i != iPortalCount; ++i )
		{
			CProp_Portal *pTempPortal = pPortals[i];
			if( pTempPortal->IsActivedAndLinked() )
			{
				float fIntersection = UTIL_IntersectRayWithPortal( ray, pTempPortal );
				if( fIntersection >= 0.0f && fIntersection < fMustBeCloserThan )
				{
					//within range, now check directionality
					if( pTempPortal->m_plane_Origin.normal.Dot( ray.m_Delta ) < 0.0f )
					{
						//qualifies for consideration, now it just has to compete for closest
						pIntersectedPortal = pTempPortal;
						fMustBeCloserThan = fIntersection;
					}
				}
			}
		}
	}

	return pIntersectedPortal;
}


bool UTIL_Portal_TraceRay_Bullets( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace, bool bTraceHolyWall )
{
	if( !pPortal || !pPortal->IsActivedAndLinked() )
	{
		//not in a portal environment, use regular traces
		enginetrace->TraceRay( ray, fMask, pTraceFilter, pTrace );
		return false;
	}

	trace_t trReal;

	enginetrace->TraceRay( ray, fMask, pTraceFilter, &trReal );

	Vector vRayNormal = ray.m_Delta;
	VectorNormalize( vRayNormal );

	Vector vPortalForward;
	pPortal->GetVectors( &vPortalForward, 0, 0 );

	// If the ray isn't going into the front of the portal, just use the real trace
	if ( vPortalForward.Dot( vRayNormal ) > 0.0f )
	{
		*pTrace = trReal;
		return false;
	}

	// If the real trace collides before the portal plane, just use the real trace
	float fPortalFraction = UTIL_IntersectRayWithPortal( ray, pPortal );

	if ( fPortalFraction == -1.0f || trReal.fraction + 0.0001f < fPortalFraction )
	{
		// Didn't intersect or the real trace intersected closer
		*pTrace = trReal;
		return false;
	}

	Ray_t rayPostPortal;
	rayPostPortal = ray;
	rayPostPortal.m_Start = ray.m_Start + ray.m_Delta * fPortalFraction;
	rayPostPortal.m_Delta = ray.m_Delta * ( 1.0f - fPortalFraction );

	VMatrix matThisToLinked = pPortal->MatrixThisToLinked();

	Ray_t rayTransformed;
	UTIL_Portal_RayTransform( matThisToLinked, rayPostPortal, rayTransformed );

	// After a bullet traces through a portal it can hit the player that fired it
	CTraceFilterSimple *pSimpleFilter = dynamic_cast<CTraceFilterSimple*>(pTraceFilter);
	const IHandleEntity *pPassEntity = NULL;
	if ( pSimpleFilter )
	{
		pPassEntity = pSimpleFilter->GetPassEntity();
		pSimpleFilter->SetPassEntity( 0 );
	}

	trace_t trPostPortal;
	enginetrace->TraceRay( rayTransformed, fMask, pTraceFilter, &trPostPortal );

	if ( pSimpleFilter )
	{
		pSimpleFilter->SetPassEntity( pPassEntity );
	}

	//trPostPortal.startpos = ray.m_Start;
	UTIL_Portal_PointTransform( matThisToLinked, ray.m_Start, trPostPortal.startpos );
	trPostPortal.fraction = trPostPortal.fraction * ( 1.0f - fPortalFraction ) + fPortalFraction;

	*pTrace = trPostPortal;

	return true;
}

CProp_Portal* UTIL_Portal_TraceRay_Beam( const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, float *pfFraction )
{
	// Do a regular trace
	trace_t tr;
	UTIL_TraceLine( ray.m_Start, ray.m_Start + ray.m_Delta, fMask, pTraceFilter, &tr );
	float fMustBeCloserThan = tr.fraction + 0.0001f;

	CProp_Portal *pIntersectedPortal = UTIL_Portal_FirstAlongRay( ray, fMustBeCloserThan );

	*pfFraction = fMustBeCloserThan; //will be real trace distance if it didn't hit a portal
	return pIntersectedPortal;
}

bool UTIL_Portal_Trace_Beam( const CBeam *pBeam, Vector &vecStart, Vector &vecEnd, Vector &vecIntersectionStart, Vector &vecIntersectionEnd, ITraceFilter *pTraceFilter )
{
	vecStart = pBeam->GetAbsStartPos();
	vecEnd = pBeam->GetAbsEndPos();

	// Trace to see if we've intersected a portal
	float fEndFraction;
	Ray_t rayBeam;

	bool bIsReversed = ( pBeam->GetBeamFlags() & FBEAM_REVERSED ) != 0x0;

	if ( !bIsReversed )
		rayBeam.Init( vecStart, vecEnd );
	else
		rayBeam.Init( vecEnd, vecStart );

	CProp_Portal *pPortal = UTIL_Portal_TraceRay_Beam( rayBeam, MASK_SHOT, pTraceFilter, &fEndFraction );

	// If we intersected a portal we need to modify the start and end points to match the actual trace through portal drawing extents
	if ( !pPortal )
		return false;

	// Modify the start and end points to match the actual trace through portal drawing extents
	vecStart = rayBeam.m_Start;

	Vector vecIntersection = rayBeam.m_Start + rayBeam.m_Delta * fEndFraction;

	int iNumLoops = 0;

	// Loop through the portals (at most 16 times)
	while ( pPortal && iNumLoops < 16 )
	{
		// Get the point that we hit a portal or wall
		vecIntersectionStart = vecIntersection;

		VMatrix matThisToLinked = pPortal->MatrixThisToLinked();

		// Get the transformed positions of the sub beam in the other portal's space
		UTIL_Portal_PointTransform( matThisToLinked, vecIntersectionStart, vecIntersectionEnd );
		UTIL_Portal_PointTransform( matThisToLinked, rayBeam.m_Start + rayBeam.m_Delta, vecEnd );

		CTraceFilterSkipClassname traceFilter( pPortal->m_hLinkedPortal, "prop_energy_ball", COLLISION_GROUP_NONE );

		rayBeam.Init( vecIntersectionEnd, vecEnd );
		pPortal = UTIL_Portal_TraceRay_Beam( rayBeam, MASK_SHOT, &traceFilter, &fEndFraction );
		vecIntersection = rayBeam.m_Start + rayBeam.m_Delta * fEndFraction;

		++iNumLoops;
	}

	vecEnd = vecIntersection;

	return true;
}


void UTIL_Portal_TraceRay_With( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace, bool bTraceHolyWall )
{
	//check to see if the player is theoretically in a portal environment
	if( !pPortal || !pPortal->m_PortalSimulator.IsReadyToSimulate() )
	{
		//not in a portal environment, use regular traces
		enginetrace->TraceRay( ray, fMask, pTraceFilter, pTrace );
	}
	else
	{		
		trace_t RealTrace;
		enginetrace->TraceRay( ray, fMask, pTraceFilter, &RealTrace );

		trace_t PortalTrace;
		UTIL_Portal_TraceRay( pPortal, ray, fMask, pTraceFilter, &PortalTrace, bTraceHolyWall );

		if( !g_bForcePortalTrace && !RealTrace.startsolid && PortalTrace.fraction <= RealTrace.fraction )
		{
			*pTrace = RealTrace;
			return;
		}

		if ( g_bAllowForcePortalTrace )
		{
			g_bForcePortalTrace = true;
		}

		*pTrace = PortalTrace;

		// If this ray has a delta, make sure its towards the portal before we try to trace across portals
		Vector vDirection = ray.m_Delta;
		VectorNormalize( vDirection );
		Vector vPortalForward;
		pPortal->GetVectors( &vPortalForward, 0, 0 );
		
		float flDot = -1.0f;
		if ( ray.m_IsSwept )
		{
			flDot = vDirection.Dot( vPortalForward );
		} 

		// TODO: Translate extents of rays properly, tracing extruded box rays across portals causes collision bugs
		//		 Until this is fixed, we'll only test true rays across portals
		if ( flDot < 0.0f && /*PortalTrace.fraction == 1.0f &&*/ ray.m_IsRay)
		{
			// Check if we're hitting stuff on the other side of the portal
			trace_t PortalLinkedTrace;
			UTIL_PortalLinked_TraceRay( pPortal, ray, fMask, pTraceFilter, &PortalLinkedTrace, bTraceHolyWall );

			if ( PortalLinkedTrace.fraction < pTrace->fraction )
			{
				// Only collide with the cross-portal objects if this trace crossed a portal
				if ( UTIL_DidTraceTouchPortals( ray, PortalLinkedTrace ) )
				{
					*pTrace = PortalLinkedTrace;
				}
			}
		}

		if( pTrace->fraction < 1.0f )
		{
			pTrace->contents = RealTrace.contents;
			pTrace->surface = RealTrace.surface;
		}

	}
}


//-----------------------------------------------------------------------------
// Purpose: Tests if a ray touches the surface of any portals
// Input  : ray - the ray to be tested against portal surfaces
//			trace - a filled-in trace corresponding to the parameter ray 
// Output : bool - false if the 'ray' parameter failed to hit any portal surface
//		    pOutLocal - the portal touched (if any)
//			pOutRemote - the portal linked to the portal touched
//-----------------------------------------------------------------------------
bool UTIL_DidTraceTouchPortals( const Ray_t& ray, const trace_t& trace, CProp_Portal** pOutLocal, CProp_Portal** pOutRemote )
{
	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount == 0 )
	{
		if( pOutLocal )
			*pOutLocal = NULL;

		if( pOutRemote )
			*pOutRemote = NULL;

		return false;
	}

	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	CProp_Portal *pIntersectedPortal = NULL;

	if( ray.m_IsSwept )
	{
		float fMustBeCloserThan = trace.fraction + 0.0001f;

		pIntersectedPortal = UTIL_Portal_FirstAlongRay( ray, fMustBeCloserThan );
	}
	
	if( (pIntersectedPortal == NULL) && !ray.m_IsRay )
	{
		//haven't hit anything yet, try again with box tests

		Vector ptRayEndPoint = trace.endpos - ray.m_StartOffset; // The trace added the start offset to the end position, so remove it for the box test
		CProp_Portal **pBoxIntersectsPortals = (CProp_Portal **)stackalloc( sizeof(CProp_Portal *) * iPortalCount );
		int iBoxIntersectsPortalsCount = 0;

		for( int i = 0; i != iPortalCount; ++i )
		{
			CProp_Portal *pTempPortal = pPortals[i];
			if( (pTempPortal->m_bActivated) && 
				(pTempPortal->m_hLinkedPortal.Get() != NULL) )
			{
				if( UTIL_IsBoxIntersectingPortal( ptRayEndPoint, ray.m_Extents, pTempPortal, 0.00f ) )
				{
					pBoxIntersectsPortals[iBoxIntersectsPortalsCount] = pTempPortal;
					++iBoxIntersectsPortalsCount;
				}
			}
		}

		if( iBoxIntersectsPortalsCount > 0 )
		{
			pIntersectedPortal = pBoxIntersectsPortals[0];
			
			if( iBoxIntersectsPortalsCount > 1 )
			{
				//hit more than one, use the closest
				float fDistToBeat = (ptRayEndPoint - pIntersectedPortal->GetAbsOrigin()).LengthSqr();

				for( int i = 1; i != iBoxIntersectsPortalsCount; ++i )
				{
					float fDist = (ptRayEndPoint - pBoxIntersectsPortals[i]->GetAbsOrigin()).LengthSqr();
					if( fDist < fDistToBeat )
					{
						pIntersectedPortal = pBoxIntersectsPortals[i];
						fDistToBeat = fDist;
					}
				}
			}
		}
	}

	if( pIntersectedPortal == NULL )
	{
		if( pOutLocal )
			*pOutLocal = NULL;

		if( pOutRemote )
			*pOutRemote = NULL;

		return false;
	}
	else
	{
		// Record the touched portals and return
		if( pOutLocal )
			*pOutLocal = pIntersectedPortal;

		if( pOutRemote )
			*pOutRemote = pIntersectedPortal->m_hLinkedPortal.Get();

		return true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Redirects the trace to either a trace that uses portal environments, or if a 
//			global boolean is set, trace with a special bullets trace.
//			NOTE: UTIL_Portal_TraceRay_With will use the default world trace if it gets a NULL portal pointer
// Input  : &ray - the ray to use to trace
//			fMask - collision mask
//			*pTraceFilter - customizable filter on the trace
//			*pTrace - trace struct to fill with output info
//-----------------------------------------------------------------------------
CProp_Portal* UTIL_Portal_TraceRay( const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace, bool bTraceHolyWall )
{
	float fMustBeCloserThan = 2.0f;
	CProp_Portal *pIntersectedPortal = UTIL_Portal_FirstAlongRay( ray, fMustBeCloserThan );

	if ( g_bBulletPortalTrace )
	{
		if ( UTIL_Portal_TraceRay_Bullets( pIntersectedPortal, ray, fMask, pTraceFilter, pTrace, bTraceHolyWall ) )
			return pIntersectedPortal;

		// Bullet didn't actually go through portal
		return NULL;

	}
	else
	{
		UTIL_Portal_TraceRay_With( pIntersectedPortal, ray, fMask, pTraceFilter, pTrace, bTraceHolyWall );
		return pIntersectedPortal;
	}
}

CProp_Portal* UTIL_Portal_TraceRay( const Ray_t &ray, unsigned int fMask, const IHandleEntity *ignore, int collisionGroup, trace_t *pTrace, bool bTraceHolyWall )
{
	CTraceFilterSimple traceFilter( ignore, collisionGroup );
	return UTIL_Portal_TraceRay( ray, fMask, &traceFilter, pTrace, bTraceHolyWall );
}


//-----------------------------------------------------------------------------
// Purpose: This version of traceray only traces against the portal environment of the specified portal.
// Input  : *pPortal - the portal whose physics we will trace against
//			&ray - the ray to trace with
//			fMask - collision mask
//			*pTraceFilter - customizable filter to determine what it hits
//			*pTrace - the trace struct to fill in with results
//			bTraceHolyWall - if this trace is to test against the 'holy wall' geometry
//-----------------------------------------------------------------------------
void UTIL_Portal_TraceRay( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace, bool bTraceHolyWall )
{
	Assert( pPortal->m_PortalSimulator.IsReadyToSimulate() ); //a trace shouldn't make it down this far if the portal is incapable of changing the results of the trace

	CTraceFilterHitAll traceFilterHitAll;
	if ( !pTraceFilter )
	{
		pTraceFilter = &traceFilterHitAll;
	}

	pTrace->fraction = 2.0f;
	pTrace->startsolid = true;
	pTrace->allsolid = true;

	trace_t TempTrace;
	int counter;

	const CPortalSimulator &portalSimulator = pPortal->m_PortalSimulator;
	CPortalSimulator *pLinkedPortalSimulator = portalSimulator.GetLinkedPortalSimulator();

	//bool bTraceDisplacements = sv_portal_trace_vs_displacements.GetBool();
	bool bTraceStaticProps = sv_portal_trace_vs_staticprops.GetBool();
	if( sv_portal_trace_vs_holywall.GetBool() == false )
		bTraceHolyWall = false;

	bool bTraceTransformedGeometry = ( (pLinkedPortalSimulator != NULL) && bTraceHolyWall && portalSimulator.RayIsInPortalHole( ray ) );	

	bool bCopyBackBrushTraceData = false;

	

	// Traces vs world
	if( pTraceFilter->GetTraceType() != TRACE_ENTITIES_ONLY )
	{
		//trace_t RealTrace;
		//enginetrace->TraceRay( ray, fMask, pTraceFilter, &RealTrace );
		if( portalSimulator.GetInternalData().Simulation.Static.World.Brushes.pCollideable && sv_portal_trace_vs_world.GetBool() )
		{
			physcollision->TraceBox( ray, portalSimulator.GetInternalData().Simulation.Static.World.Brushes.pCollideable, vec3_origin, vec3_angle, pTrace );
			bCopyBackBrushTraceData = true;
		}

		if( bTraceHolyWall )
		{
			if( portalSimulator.GetInternalData().Simulation.Static.Wall.Local.Tube.pCollideable )
			{
				physcollision->TraceBox( ray, portalSimulator.GetInternalData().Simulation.Static.Wall.Local.Tube.pCollideable, vec3_origin, vec3_angle, &TempTrace );

				if( (TempTrace.startsolid == false) && (TempTrace.fraction < pTrace->fraction) ) //never allow something to be stuck in the tube, it's more of a last-resort guide than a real collideable
				{
					*pTrace = TempTrace;
					bCopyBackBrushTraceData = true;
				}
			}

			if( portalSimulator.GetInternalData().Simulation.Static.Wall.Local.Brushes.pCollideable )
			{
				physcollision->TraceBox( ray, portalSimulator.GetInternalData().Simulation.Static.Wall.Local.Brushes.pCollideable, vec3_origin, vec3_angle, &TempTrace );
				if( (TempTrace.fraction < pTrace->fraction) )
				{
					*pTrace = TempTrace;
					bCopyBackBrushTraceData = true;
				}
			}

			//if( portalSimulator.GetInternalData().Simulation.Static.Wall.RemoteTransformedToLocal.Brushes.pCollideable && sv_portal_trace_vs_world.GetBool() )
			if( bTraceTransformedGeometry && pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable )
			{
				physcollision->TraceBox( ray, pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable, portalSimulator.GetInternalData().Placement.ptaap_LinkedToThis.ptOriginTransform, portalSimulator.GetInternalData().Placement.ptaap_LinkedToThis.qAngleTransform, &TempTrace );
				if( (TempTrace.fraction < pTrace->fraction) )
				{
					*pTrace = TempTrace;
					bCopyBackBrushTraceData = true;
				}
			}
		}	

		if( bCopyBackBrushTraceData )
		{
			pTrace->surface = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.surface;
			pTrace->contents = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.contents;
			pTrace->m_pEnt = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.pEntity;

			bCopyBackBrushTraceData = false;
		}
	}
	
	// Traces vs entities
	if( pTraceFilter->GetTraceType() != TRACE_WORLD_ONLY )
	{
		bool bFilterStaticProps = (pTraceFilter->GetTraceType() == TRACE_EVERYTHING_FILTER_PROPS);

		//solid entities
		CPortalCollideableEnumerator enumerator( pPortal );
		partition->EnumerateElementsAlongRay( PARTITION_ENGINE_SOLID_EDICTS | PARTITION_ENGINE_STATIC_PROPS, ray, false, &enumerator );
		for( counter = 0; counter != enumerator.m_iHandleCount; ++counter )
		{
			if( staticpropmgr->IsStaticProp( enumerator.m_pHandles[counter] ) )
			{
				//if( bFilterStaticProps && !pTraceFilter->ShouldHitEntity( enumerator.m_pHandles[counter], fMask ) )
				continue; //static props are handled separately, with clipped versions
			}
            else if ( !pTraceFilter->ShouldHitEntity( enumerator.m_pHandles[counter], fMask ) )
			{
				continue;
			}

			enginetrace->ClipRayToEntity( ray, fMask, enumerator.m_pHandles[counter], &TempTrace );
			if( (TempTrace.fraction < pTrace->fraction)  )
				*pTrace = TempTrace;
		}



		
		if( bTraceStaticProps )
		{
			//local clipped static props
			{
				int iLocalStaticCount = portalSimulator.GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Count();
				if( iLocalStaticCount != 0 && portalSimulator.GetInternalData().Simulation.Static.World.StaticProps.bCollisionExists )
				{
					const PS_SD_Static_World_StaticProps_ClippedProp_t *pCurrentProp = portalSimulator.GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Base();
					const PS_SD_Static_World_StaticProps_ClippedProp_t *pStop = pCurrentProp + iLocalStaticCount;
					Vector vTransform = vec3_origin;
					QAngle qTransform = vec3_angle;

					do
					{
						if( (!bFilterStaticProps) || pTraceFilter->ShouldHitEntity( pCurrentProp->pSourceProp, fMask ) )
						{
							physcollision->TraceBox( ray, pCurrentProp->pCollide, vTransform, qTransform, &TempTrace );
							if( (TempTrace.fraction < pTrace->fraction) )
							{
								*pTrace = TempTrace;
								pTrace->surface.flags = pCurrentProp->iTraceSurfaceFlags;
								pTrace->surface.surfaceProps = pCurrentProp->iTraceSurfaceProps;
								pTrace->surface.name = pCurrentProp->szTraceSurfaceName;
								pTrace->contents = pCurrentProp->iTraceContents;
								pTrace->m_pEnt = pCurrentProp->pTraceEntity;
							}
						}

						++pCurrentProp;
					}
					while( pCurrentProp != pStop );
				}
			}

			if( bTraceHolyWall )
			{
				//remote clipped static props transformed into our wall space
				if( bTraceTransformedGeometry && (pTraceFilter->GetTraceType() != TRACE_WORLD_ONLY) && sv_portal_trace_vs_staticprops.GetBool() )
				{
					int iLocalStaticCount = pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Count();
					if( iLocalStaticCount != 0 )
					{
						const PS_SD_Static_World_StaticProps_ClippedProp_t *pCurrentProp = pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Base();
						const PS_SD_Static_World_StaticProps_ClippedProp_t *pStop = pCurrentProp + iLocalStaticCount;
						Vector vTransform = portalSimulator.GetInternalData().Placement.ptaap_LinkedToThis.ptOriginTransform;
						QAngle qTransform = portalSimulator.GetInternalData().Placement.ptaap_LinkedToThis.qAngleTransform;

						do
						{
							if( (!bFilterStaticProps) || pTraceFilter->ShouldHitEntity( pCurrentProp->pSourceProp, fMask ) )
							{
								physcollision->TraceBox( ray, pCurrentProp->pCollide, vTransform, qTransform, &TempTrace );
								if( (TempTrace.fraction < pTrace->fraction) )
								{
									*pTrace = TempTrace;
									pTrace->surface.flags = pCurrentProp->iTraceSurfaceFlags;
									pTrace->surface.surfaceProps = pCurrentProp->iTraceSurfaceProps;
									pTrace->surface.name = pCurrentProp->szTraceSurfaceName;
									pTrace->contents = pCurrentProp->iTraceContents;
									pTrace->m_pEnt = pCurrentProp->pTraceEntity;
								}
							}

							++pCurrentProp;
						}
						while( pCurrentProp != pStop );
					}
				}
			}
		}
	}

	if( pTrace->fraction > 1.0f ) //this should only happen if there was absolutely nothing to trace against
	{
		//AssertMsg( 0, "Nothing to trace against" );
		memset( pTrace, 0, sizeof( trace_t ) );
		pTrace->fraction = 1.0f;
		pTrace->startpos = ray.m_Start - ray.m_StartOffset;
		pTrace->endpos = pTrace->startpos + ray.m_Delta;
	}
	else if ( pTrace->fraction < 0 )
	{
		// For all brush traces, use the 'portal backbrush' surface surface contents
		// BUGBUG: Doing this is a great solution because brushes near a portal
		// will have their contents and surface properties homogenized to the brush the portal ray hit.
		pTrace->contents = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.contents;
		pTrace->surface = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.surface;
		pTrace->m_pEnt = portalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.pEntity;
	}
}

void UTIL_Portal_TraceRay( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, const IHandleEntity *ignore, int collisionGroup, trace_t *pTrace, bool bTraceHolyWall )
{
	CTraceFilterSimple traceFilter( ignore, collisionGroup );
	UTIL_Portal_TraceRay( pPortal, ray, fMask, &traceFilter, pTrace, bTraceHolyWall );
}

//-----------------------------------------------------------------------------
// Purpose: Trace a ray 'past' a portal's surface, hitting objects in the linked portal's collision environment
// Input  : *pPortal - The portal being traced 'through'
//			&ray - The ray being traced
//			fMask - trace mask to cull results
//			*pTraceFilter - trace filter to cull results
//			*pTrace - Empty trace to return the result (value will be overwritten)
//-----------------------------------------------------------------------------
void UTIL_PortalLinked_TraceRay( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace, bool bTraceHolyWall )
{
#ifdef CLIENT_DLL
	Assert( (GameRules() == NULL) || GameRules()->IsMultiplayer() );
#endif
	// Transform the specified ray to the remote portal's space
	Ray_t rayTransformed;
	UTIL_Portal_RayTransform( pPortal->MatrixThisToLinked(), ray, rayTransformed );

	AssertMsg ( ray.m_IsRay, "Ray with extents across portal tracing not implemented!" );

	const CPortalSimulator &portalSimulator = pPortal->m_PortalSimulator;
	CProp_Portal *pLinkedPortal = (CProp_Portal*)(pPortal->m_hLinkedPortal.Get());
	if( (pLinkedPortal == NULL) || (portalSimulator.RayIsInPortalHole( ray ) == false) )
	{
		memset( pTrace, 0, sizeof(trace_t));
		pTrace->fraction = 1.0f;
		pTrace->fractionleftsolid = 0;

		pTrace->contents = pPortal->m_PortalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.contents;
		pTrace->surface  = pPortal->m_PortalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.surface;
		pTrace->m_pEnt	 = pPortal->m_PortalSimulator.GetInternalData().Simulation.Static.SurfaceProperties.pEntity;
		return;
	}
	UTIL_Portal_TraceRay( pLinkedPortal, rayTransformed, fMask, pTraceFilter, pTrace, bTraceHolyWall );

	// Transform the ray's start, end and plane back into this portal's space, 
	// because we react to the collision as it is displayed, and the image is displayed with this local portal's orientation.
	VMatrix matLinkedToThis = pLinkedPortal->MatrixThisToLinked();
	UTIL_Portal_PointTransform( matLinkedToThis, pTrace->startpos, pTrace->startpos );
	UTIL_Portal_PointTransform( matLinkedToThis, pTrace->endpos, pTrace->endpos );
	UTIL_Portal_PlaneTransform( matLinkedToThis, pTrace->plane, pTrace->plane );
}

void UTIL_PortalLinked_TraceRay( const CProp_Portal *pPortal, const Ray_t &ray, unsigned int fMask, const IHandleEntity *ignore, int collisionGroup, trace_t *pTrace, bool bTraceHolyWall )
{
	CTraceFilterSimple traceFilter( ignore, collisionGroup );
	UTIL_PortalLinked_TraceRay( pPortal, ray, fMask, &traceFilter, pTrace, bTraceHolyWall );
}

//-----------------------------------------------------------------------------
// Purpose: A version of trace entity which detects portals and translates the trace through portals
//-----------------------------------------------------------------------------
void UTIL_Portal_TraceEntity( CBaseEntity *pEntity, const Vector &vecAbsStart, const Vector &vecAbsEnd, 
							 unsigned int mask, ITraceFilter *pFilter, trace_t *pTrace )
{
#ifdef CLIENT_DLL
	Assert( (GameRules() == NULL) || GameRules()->IsMultiplayer() );
	Assert( pEntity->IsPlayer() );

	CPortalSimulator *pPortalSimulator = NULL;
	if( pEntity->IsPlayer() )
	{
		C_Prop_Portal *pPortal = ((C_Portal_Player *)pEntity)->m_hPortalEnvironment.Get();
		if( pPortal )
			pPortalSimulator = &pPortal->m_PortalSimulator;
	}
#else
	CPortalSimulator *pPortalSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity( pEntity );
#endif

	memset( pTrace, 0, sizeof(trace_t));
	pTrace->fraction = 1.0f;
	pTrace->fractionleftsolid = 0;

	ICollideable* pCollision = enginetrace->GetCollideable( pEntity );

	// If main is simulating this object, trace as UTIL_TraceEntity would
	trace_t realTrace;
	QAngle qCollisionAngles = pCollision->GetCollisionAngles();
	enginetrace->SweepCollideable( pCollision, vecAbsStart, vecAbsEnd, qCollisionAngles, mask, pFilter, &realTrace );

	// For the below box test, we need to add the tolerance onto the extents, because the underlying
	// box on plane side test doesn't use the parameter tolerance.
	float flTolerance = 0.1f;
	Vector vEntExtents = pEntity->WorldAlignSize() * 0.5 + Vector ( flTolerance, flTolerance, flTolerance );
	Vector vColCenter = realTrace.endpos + ( pEntity->WorldAlignMaxs() + pEntity->WorldAlignMins() ) * 0.5f;

	// If this entity is not simulated in a portal environment, trace as normal
    if( pPortalSimulator == NULL )
	{
		// If main is simulating this object, trace as UTIL_TraceEntity would
		*pTrace = realTrace;
	}
	else
	{
		CPortalSimulator *pLinkedPortalSimulator = pPortalSimulator->GetLinkedPortalSimulator();

		Ray_t entRay;
		entRay.Init( vecAbsStart, vecAbsEnd, pCollision->OBBMins(), pCollision->OBBMaxs() );

#if 0 // this trace for brush ents made sense at one time, but it's 'overcolliding' during portal transitions (bugzilla#25)
		if( realTrace.m_pEnt && (realTrace.m_pEnt->GetMoveType() != MOVETYPE_NONE) ) //started by hitting something moving which wouldn't be detected in the following traces
		{
			float fFirstPortalFraction = 2.0f;
			CProp_Portal *pFirstPortal = UTIL_Portal_FirstAlongRay( entRay, fFirstPortalFraction );

			if ( !pFirstPortal )
				*pTrace = realTrace;
			else
			{
				Vector vFirstPortalForward;
				pFirstPortal->GetVectors( &vFirstPortalForward, NULL, NULL );
				if ( vFirstPortalForward.Dot( realTrace.endpos - pFirstPortal->GetAbsOrigin() ) > 0.0f )
					*pTrace = realTrace;
			}
		}
#endif

		// We require both environments to be active in order to trace against them
		Assert ( pCollision );
		if ( !pCollision  )
		{
			return;
		}

		// World, displacements and holy wall are stored in separate collideables
		// Traces against each and keep the closest intersection (if any)
		trace_t tempTrace;

		// Hit the world
		if ( pFilter->GetTraceType() != TRACE_ENTITIES_ONLY )
		{	
			if( pPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable && 
				sv_portal_trace_vs_world.GetBool() )
			{
				//physcollision->TraceCollide( vecAbsStart, vecAbsEnd, pCollision, qCollisionAngles, 
				//							pPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable, vec3_origin, vec3_angle, &tempTrace );
				
				physcollision->TraceBox( entRay, MASK_ALL, NULL, pPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable, vec3_origin, vec3_angle, &tempTrace );

				if ( tempTrace.startsolid || (tempTrace.fraction < pTrace->fraction) )
				{
					*pTrace = tempTrace;
				}
			}

			//if( pPortalSimulator->GetInternalData().Simulation.Static.Wall.RemoteTransformedToLocal.Brushes.pCollideable &&
			if( pLinkedPortalSimulator &&
				pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable &&
				sv_portal_trace_vs_world.GetBool() && 
				sv_portal_trace_vs_holywall.GetBool() )
			{
				//physcollision->TraceCollide( vecAbsStart, vecAbsEnd, pCollision, qCollisionAngles,
				//							pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable, pPortalSimulator->GetInternalData().Placement.ptaap_LinkedToThis.ptOriginTransform, pPortalSimulator->GetInternalData().Placement.ptaap_LinkedToThis.qAngleTransform, &tempTrace );

				physcollision->TraceBox( entRay, MASK_ALL, NULL, pLinkedPortalSimulator->GetInternalData().Simulation.Static.World.Brushes.pCollideable, pPortalSimulator->GetInternalData().Placement.ptaap_LinkedToThis.ptOriginTransform, pPortalSimulator->GetInternalData().Placement.ptaap_LinkedToThis.qAngleTransform, &tempTrace );

				if ( tempTrace.startsolid || (tempTrace.fraction < pTrace->fraction) )
				{
					*pTrace = tempTrace;
				}
			}

			if ( pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Brushes.pCollideable && 
				sv_portal_trace_vs_holywall.GetBool() )
			{
				//physcollision->TraceCollide( vecAbsStart, vecAbsEnd, pCollision, qCollisionAngles,
				//							pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Brushes.pCollideable, vec3_origin, vec3_angle, &tempTrace );

				physcollision->TraceBox( entRay, MASK_ALL, NULL, pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Brushes.pCollideable, vec3_origin, vec3_angle, &tempTrace );

				if ( tempTrace.startsolid || (tempTrace.fraction < pTrace->fraction) )
				{
					if( tempTrace.fraction == 0.0f )
						tempTrace.startsolid = true;

					if( tempTrace.fractionleftsolid == 1.0f )
						tempTrace.allsolid = true;

					*pTrace = tempTrace;
				}
			}

			if ( pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Tube.pCollideable && 
				sv_portal_trace_vs_holywall.GetBool() )
			{
				//physcollision->TraceCollide( vecAbsStart, vecAbsEnd, pCollision, qCollisionAngles,
				//							pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Tube.pCollideable, vec3_origin, vec3_angle, &tempTrace );

				physcollision->TraceBox( entRay, MASK_ALL, NULL, pPortalSimulator->GetInternalData().Simulation.Static.Wall.Local.Tube.pCollideable, vec3_origin, vec3_angle, &tempTrace );

				if( (tempTrace.startsolid == false) && (tempTrace.fraction < pTrace->fraction) ) //never allow something to be stuck in the tube, it's more of a last-resort guide than a real collideable
				{
					*pTrace = tempTrace;
				}
			}

			// For all brush traces, use the 'portal backbrush' surface surface contents
			// BUGBUG: Doing this is a great solution because brushes near a portal
			// will have their contents and surface properties homogenized to the brush the portal ray hit.
			if ( pTrace->startsolid || (pTrace->fraction < 1.0f) )
			{
				pTrace->surface = pPortalSimulator->GetInternalData().Simulation.Static.SurfaceProperties.surface;
				pTrace->contents = pPortalSimulator->GetInternalData().Simulation.Static.SurfaceProperties.contents;
				pTrace->m_pEnt = pPortalSimulator->GetInternalData().Simulation.Static.SurfaceProperties.pEntity;
			}
		}

		// Trace vs entities
		if ( pFilter->GetTraceType() != TRACE_WORLD_ONLY )
		{
			if( sv_portal_trace_vs_staticprops.GetBool() && (pFilter->GetTraceType() != TRACE_ENTITIES_ONLY) )
			{
				bool bFilterStaticProps = (pFilter->GetTraceType() == TRACE_EVERYTHING_FILTER_PROPS);
				
				//local clipped static props
				{
					int iLocalStaticCount = pPortalSimulator->GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Count();
					if( iLocalStaticCount != 0 && pPortalSimulator->GetInternalData().Simulation.Static.World.StaticProps.bCollisionExists )
					{
						const PS_SD_Static_World_StaticProps_ClippedProp_t *pCurrentProp = pPortalSimulator->GetInternalData().Simulation.Static.World.StaticProps.ClippedRepresentations.Base();
						const PS_SD_Static_World_StaticProps_ClippedProp_t *pStop = pCurrentProp + iLocalStaticCount;
						Vector vTransform = vec3_origin;
						QAngle qTransform = vec3_angle;

						do
						{
							if( (!bFilterStaticProps) || pFilter->ShouldHitEntity( pCurrentProp->pSourceProp, mask ) )
							{
								//physcollision->TraceCollide( vecAbsStart, vecAbsEnd, pCollision, qCollisionAngles,
								//							pCurrentProp->pCollide, vTransform, qTransform, &tempTrace );

								physcollision->TraceBox( entRay, MASK_ALL, NULL, pCurrentProp->pCollide, vTransform, qTransform, &tempTrace );

								if( tempTrace.startsolid || (tempTrace.fraction < pTrace->fraction) )
								{
									*pTrace = tempTrace;
									pTrace->surface.flags = pCurrentProp->iTraceSurfaceFlags;
									pTrace->surface.surfaceProps = pCurrentProp->iTraceSurfaceProps;
									pTrace->surface.name = pCurrentProp->szTraceSurfaceName;
									pTrace->contents = pCurrentProp->iTraceContents;
									pTrace->m_pEnt = pCurrentProp->pTraceEntity;
								}
							}

							++pCurrentProp;
						}
						while( pCurrentProp != pStop );
					}
				}

				if( pLinkedPortalSimulator && pPortalSimulator->EntityIsInPortalHole( pEntity ) )
				{

#ifndef CLIENT_DLL
					if( sv_use_transformed_collideables.GetBool() ) //if this never gets turned off, it should be removed before release
					{
						//moving entities near the remote portal
						CBaseEntity *pEnts[1024];
						int iEntCount = pLinkedPortalSimulator->GetMoveableOwnedEntities( pEnts, 1024 );

						CTransformedCollideable transformedCollideable;
						transformedCollideable.m_matTransform = pLinkedPortalSimulator->GetInternalData().Placement.matThisToLinked;
						transformedCollideable.m_matInvTransform = pLinkedPortalSimulator->GetInternalData().Placement.matLinkedToThis;
						for( int i = 0; i != iEntCount; ++i )
						{
							CBaseEntity *pRemoteEntity = pEnts[i];
							if( pRemoteEntity->GetSolid() == SOLID_NONE )
								continue;

							transformedCollideable.m_pWrappedCollideable = pRemoteEntity->GetCollideable();
							Assert( transformedCollideable.m_pWrappedCollideable != NULL );
	                        						
							//enginetrace->ClipRayToCollideable( entRay, mask, &transformedCollideable, pTrace );

							enginetrace->ClipRayToCollideable( entRay, mask, &transformedCollideable, &tempTrace );
							if( tempTrace.startsolid || (tempTrace.fraction < pTrace->fraction) )
							{
								*pTrace = tempTrace;
							}
						}
					}
#endif //#ifndef CLIENT_DLL
				}
			}
		}

		if( pTrace->fraction == 1.0f ) 
		{
			memset( pTrace, 0, sizeof( trace_t ) );
			pTrace->fraction = 1.0f;
			pTrace->startpos = vecAbsStart;
			pTrace->endpos = vecAbsEnd;
		}
//#endif
	}
}

void UTIL_Portal_PointTransform( const VMatrix matThisToLinked, const Vector &ptSource, Vector &ptTransformed )
{
	ptTransformed = matThisToLinked * ptSource;
}

void UTIL_Portal_VectorTransform( const VMatrix matThisToLinked, const Vector &vSource, Vector &vTransformed )
{
	vTransformed = matThisToLinked.ApplyRotation( vSource );
}

void UTIL_Portal_AngleTransform( const VMatrix matThisToLinked, const QAngle &qSource, QAngle &qTransformed )
{
	qTransformed = TransformAnglesToWorldSpace( qSource, matThisToLinked.As3x4() );
}

void UTIL_Portal_RayTransform( const VMatrix matThisToLinked, const Ray_t &raySource, Ray_t &rayTransformed )
{
	rayTransformed = raySource;

	UTIL_Portal_PointTransform( matThisToLinked, raySource.m_Start, rayTransformed.m_Start );
	UTIL_Portal_VectorTransform( matThisToLinked, raySource.m_StartOffset, rayTransformed.m_StartOffset );
	UTIL_Portal_VectorTransform( matThisToLinked, raySource.m_Delta, rayTransformed.m_Delta );

	//BUGBUG: Extents are axis aligned, so rotating it won't necessarily give us what we're expecting
	UTIL_Portal_VectorTransform( matThisToLinked, raySource.m_Extents, rayTransformed.m_Extents );
	
	//HACKHACK: Negative extents hang in traces, make each positive because we rotated it above
	if ( rayTransformed.m_Extents.x < 0.0f )
	{
		rayTransformed.m_Extents.x = -rayTransformed.m_Extents.x;
	}
	if ( rayTransformed.m_Extents.y < 0.0f )
	{
		rayTransformed.m_Extents.y = -rayTransformed.m_Extents.y;
	}
	if ( rayTransformed.m_Extents.z < 0.0f )
	{
		rayTransformed.m_Extents.z = -rayTransformed.m_Extents.z;
	}

}

void UTIL_Portal_PlaneTransform( const VMatrix matThisToLinked, const cplane_t &planeSource, cplane_t &planeTransformed )
{
	planeTransformed = planeSource;

	Vector vTrans;
	UTIL_Portal_VectorTransform( matThisToLinked, planeSource.normal, planeTransformed.normal );
	planeTransformed.dist = planeSource.dist * DotProduct( planeTransformed.normal, planeTransformed.normal );
	planeTransformed.dist += DotProduct( planeTransformed.normal, matThisToLinked.GetTranslation( vTrans ) );
}

void UTIL_Portal_PlaneTransform( const VMatrix matThisToLinked, const VPlane &planeSource, VPlane &planeTransformed )
{
	Vector vTranformedNormal;
	float fTransformedDist;

	Vector vTrans;
	UTIL_Portal_VectorTransform( matThisToLinked, planeSource.m_Normal, vTranformedNormal );
	fTransformedDist = planeSource.m_Dist * DotProduct( vTranformedNormal, vTranformedNormal );
	fTransformedDist += DotProduct( vTranformedNormal, matThisToLinked.GetTranslation( vTrans ) );

	planeTransformed.Init( vTranformedNormal, fTransformedDist );
}

void UTIL_Portal_Triangles( const Vector &ptPortalCenter, const QAngle &qPortalAngles, Vector pvTri1[ 3 ], Vector pvTri2[ 3 ] )
{
	// Get points to make triangles
	Vector vRight, vUp;
	AngleVectors( qPortalAngles, NULL, &vRight, &vUp );

	Vector vTopEdge = vUp * PORTAL_HALF_HEIGHT;
	Vector vBottomEdge = -vTopEdge;
	Vector vRightEdge = vRight * PORTAL_HALF_WIDTH;
	Vector vLeftEdge = -vRightEdge;

	Vector vTopLeft = ptPortalCenter + vTopEdge + vLeftEdge;
	Vector vTopRight = ptPortalCenter + vTopEdge + vRightEdge;
	Vector vBottomLeft = ptPortalCenter + vBottomEdge + vLeftEdge;
	Vector vBottomRight = ptPortalCenter + vBottomEdge + vRightEdge;

	// Make triangles
	pvTri1[ 0 ] = vTopRight;
	pvTri1[ 1 ] = vTopLeft;
	pvTri1[ 2 ] = vBottomLeft;

	pvTri2[ 0 ] = vTopRight;
	pvTri2[ 1 ] = vBottomLeft;
	pvTri2[ 2 ] = vBottomRight;
}

void UTIL_Portal_Triangles( const CProp_Portal *pPortal, Vector pvTri1[ 3 ], Vector pvTri2[ 3 ] )
{
	UTIL_Portal_Triangles( pPortal->m_ptOrigin, pPortal->m_qAbsAngle, pvTri1, pvTri2 );
}

float UTIL_Portal_DistanceThroughPortal( const CProp_Portal *pPortal, const Vector &vPoint1, const Vector &vPoint2 )
{
	return FastSqrt( UTIL_Portal_DistanceThroughPortalSqr( pPortal, vPoint1, vPoint2 ) );
}

float UTIL_Portal_DistanceThroughPortalSqr( const CProp_Portal *pPortal, const Vector &vPoint1, const Vector &vPoint2 )
{
	if ( !pPortal || !pPortal->m_bActivated )
		return -1.0f;

	CProp_Portal *pPortalLinked = pPortal->m_hLinkedPortal;
	if ( !pPortalLinked || !pPortalLinked->m_bActivated )
		return -1.0f;

	return vPoint1.DistToSqr( pPortal->GetAbsOrigin() ) + pPortalLinked->GetAbsOrigin().DistToSqr( vPoint2 );
}

float UTIL_Portal_ShortestDistance( const Vector &vPoint1, const Vector &vPoint2, CProp_Portal **pShortestDistPortal_Out /*= NULL*/, bool bRequireStraightLine /*= false*/ )
{
	return FastSqrt( UTIL_Portal_ShortestDistanceSqr( vPoint1, vPoint2, pShortestDistPortal_Out, bRequireStraightLine ) );
}

float UTIL_Portal_ShortestDistanceSqr( const Vector &vPoint1, const Vector &vPoint2, CProp_Portal **pShortestDistPortal_Out /*= NULL*/, bool bRequireStraightLine /*= false*/ )
{
	float fMinDist = vPoint1.DistToSqr( vPoint2 );	
	
	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount == 0 )
	{
		if( pShortestDistPortal_Out )
			*pShortestDistPortal_Out = NULL;

		return fMinDist;
	}
	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	CProp_Portal *pShortestDistPortal = NULL;

	for( int i = 0; i != iPortalCount; ++i )
	{
		CProp_Portal *pTempPortal = pPortals[i];
		if( pTempPortal->m_bActivated )
		{
			CProp_Portal *pLinkedPortal = pTempPortal->m_hLinkedPortal.Get();
			if( pLinkedPortal != NULL )
			{
				Vector vPoint1Transformed = pTempPortal->MatrixThisToLinked() * vPoint1;

				float fDirectDist = vPoint1Transformed.DistToSqr( vPoint2 );
				if( fDirectDist < fMinDist )
				{
					//worth investigating further
					//find out if it's a straight line through the portal, or if we have to wrap around a corner
					float fPoint1TransformedDist = pLinkedPortal->m_plane_Origin.normal.Dot( vPoint1Transformed ) - pLinkedPortal->m_plane_Origin.dist;
					float fPoint2Dist = pLinkedPortal->m_plane_Origin.normal.Dot( vPoint2 ) - pLinkedPortal->m_plane_Origin.dist;

					bool bStraightLine = true;
					if( (fPoint1TransformedDist > 0.0f) || (fPoint2Dist < 0.0f) ) //straight line through portal impossible, part of the line has to backtrack to get to the portal surface
						bStraightLine = false;

					if( bStraightLine ) //if we're not already doing some crazy wrapping, find an intersection point
					{
						float fTotalDist = fPoint2Dist - fPoint1TransformedDist; //fPoint1TransformedDist is known to be negative
						Vector ptPlaneIntersection;

						if( fTotalDist != 0.0f )
						{
							float fInvTotalDist = 1.0f / fTotalDist;
							ptPlaneIntersection = (vPoint1Transformed * (fPoint2Dist * fInvTotalDist)) + (vPoint2 * ((-fPoint1TransformedDist) * fInvTotalDist));
						}
						else
						{
							ptPlaneIntersection = vPoint1Transformed;
						}

						Vector vRight, vUp;
						pLinkedPortal->GetVectors( NULL, &vRight, &vUp );
						
						Vector ptLinkedCenter = pLinkedPortal->GetAbsOrigin();
						Vector vCenterToIntersection = ptPlaneIntersection - ptLinkedCenter;
						float fRight = vRight.Dot( vCenterToIntersection );
						float fUp = vUp.Dot( vCenterToIntersection );

						float fAbsRight = fabs( fRight );
						float fAbsUp = fabs( fUp );
						if( (fAbsRight > PORTAL_HALF_WIDTH) ||
							(fAbsUp > PORTAL_HALF_HEIGHT) )
							bStraightLine = false;

						if( bStraightLine == false )
						{
							if( bRequireStraightLine )
								continue;

							//find the offending extent and shorten both extents to bring it into the portal quad
							float fNormalizer;
							if( fAbsRight > PORTAL_HALF_WIDTH )
							{
								fNormalizer = fAbsRight/PORTAL_HALF_WIDTH;

								if( fAbsUp > PORTAL_HALF_HEIGHT )
								{
									float fUpNormalizer = fAbsUp/PORTAL_HALF_HEIGHT;
									if( fUpNormalizer > fNormalizer )
										fNormalizer = fUpNormalizer;
								}
							}
							else
							{
								fNormalizer = fAbsUp/PORTAL_HALF_HEIGHT;
							}

							vCenterToIntersection *= (1.0f/fNormalizer);
							ptPlaneIntersection = ptLinkedCenter + vCenterToIntersection;

							float fWrapDist = vPoint1Transformed.DistToSqr( ptPlaneIntersection ) + vPoint2.DistToSqr( ptPlaneIntersection );
							if( fWrapDist < fMinDist )
							{
								fMinDist = fWrapDist;
								pShortestDistPortal = pTempPortal;
							}
						}
						else
						{
							//it's a straight shot from point 1 to 2 through the portal
							fMinDist = fDirectDist;
							pShortestDistPortal = pTempPortal;
						}
					}
					else
					{
						if( bRequireStraightLine )
							continue;

						//do some crazy wrapped line intersection algorithm

						//for now, just do the cheap and easy solution
						float fWrapDist = vPoint1.DistToSqr( pTempPortal->GetAbsOrigin() ) + pLinkedPortal->GetAbsOrigin().DistToSqr( vPoint2 );
						if( fWrapDist < fMinDist )
						{
							fMinDist = fWrapDist;
							pShortestDistPortal = pTempPortal;
						}
					}
				}
			}
		}
	}

	return fMinDist;
}

void UTIL_Portal_AABB( const CProp_Portal *pPortal, Vector &vMin, Vector &vMax )
{
	Vector vOrigin = pPortal->m_ptOrigin;
	QAngle qAngles = pPortal->m_qAbsAngle;

	Vector vOBBForward;
	Vector vOBBRight;
	Vector vOBBUp;

	AngleVectors( qAngles, &vOBBForward, &vOBBRight, &vOBBUp );

	//scale the extents to usable sizes
	vOBBForward *= PORTAL_HALF_DEPTH;
	vOBBRight *= PORTAL_HALF_WIDTH;
	vOBBUp *= PORTAL_HALF_HEIGHT;

	vOrigin -= vOBBForward + vOBBRight + vOBBUp;

	vOBBForward *= 2.0f;
	vOBBRight *= 2.0f;
	vOBBUp *= 2.0f;

	vMin = vMax = vOrigin;

	for( int i = 1; i != 8; ++i )
	{
		Vector ptTest = vOrigin;
		if( i & (1 << 0) ) ptTest += vOBBForward;
		if( i & (1 << 1) ) ptTest += vOBBRight;
		if( i & (1 << 2) ) ptTest += vOBBUp;

		if( ptTest.x < vMin.x ) vMin.x = ptTest.x;
		if( ptTest.y < vMin.y ) vMin.y = ptTest.y;
		if( ptTest.z < vMin.z ) vMin.z = ptTest.z;
		if( ptTest.x > vMax.x ) vMax.x = ptTest.x;
		if( ptTest.y > vMax.y ) vMax.y = ptTest.y;
		if( ptTest.z > vMax.z ) vMax.z = ptTest.z;
	}
}

float UTIL_IntersectRayWithPortal( const Ray_t &ray, const CProp_Portal *pPortal )
{
	if ( !pPortal || !pPortal->m_bActivated )
	{
		return -1.0f;
	}

	Vector vForward;
	pPortal->GetVectors( &vForward, NULL, NULL );

	// Discount rays not coming from the front of the portal
	float fDot = DotProduct( vForward, ray.m_Delta );
	if ( fDot > 0.0f  )
	{
		return -1.0f;
	}

	Vector pvTri1[ 3 ], pvTri2[ 3 ];

	UTIL_Portal_Triangles( pPortal, pvTri1, pvTri2 );

	float fT;

	// Test triangle 1
	fT = IntersectRayWithTriangle( ray, pvTri1[ 0 ], pvTri1[ 1 ], pvTri1[ 2 ], false );

	// If there was an intersection return the T
	if ( fT >= 0.0f )
		return fT;

	// Return the result of collision with the other face triangle
	return IntersectRayWithTriangle( ray, pvTri2[ 0 ], pvTri2[ 1 ], pvTri2[ 2 ], false );
}

bool UTIL_IntersectRayWithPortalOBB( const CProp_Portal *pPortal, const Ray_t &ray, trace_t *pTrace )
{
	return IntersectRayWithOBB( ray, pPortal->m_ptOrigin, pPortal->m_qAbsAngle, CProp_Portal_Shared::vLocalMins, CProp_Portal_Shared::vLocalMaxs, 0.0f, pTrace );
}

bool UTIL_IntersectRayWithPortalOBBAsAABB( const CProp_Portal *pPortal, const Ray_t &ray, trace_t *pTrace )
{
	Vector vAABBMins, vAABBMaxs;

	UTIL_Portal_AABB( pPortal, vAABBMins, vAABBMaxs );

	return IntersectRayWithBox( ray, vAABBMins, vAABBMaxs, 0.0f, pTrace );
}

bool UTIL_IsBoxIntersectingPortal( const Vector &vecBoxCenter, const Vector &vecBoxExtents, const Vector &ptPortalCenter, const QAngle &qPortalAngles, float flTolerance )
{
	Vector pvTri1[ 3 ], pvTri2[ 3 ];

	UTIL_Portal_Triangles( ptPortalCenter, qPortalAngles, pvTri1, pvTri2 );

	cplane_t plane;

	ComputeTrianglePlane( pvTri1[ 0 ], pvTri1[ 1 ], pvTri1[ 2 ], plane.normal, plane.dist );
	plane.type = PLANE_ANYZ;
	plane.signbits = SignbitsForPlane( &plane );

	if ( IsBoxIntersectingTriangle( vecBoxCenter, vecBoxExtents, pvTri1[ 0 ], pvTri1[ 1 ], pvTri1[ 2 ], plane, flTolerance ) )
	{
		return true;
	}

	ComputeTrianglePlane( pvTri2[ 0 ], pvTri2[ 1 ], pvTri2[ 2 ], plane.normal, plane.dist );
	plane.type = PLANE_ANYZ;
	plane.signbits = SignbitsForPlane( &plane );

	return IsBoxIntersectingTriangle( vecBoxCenter, vecBoxExtents, pvTri2[ 0 ], pvTri2[ 1 ], pvTri2[ 2 ], plane, flTolerance );
}

bool UTIL_IsBoxIntersectingPortal( const Vector &vecBoxCenter, const Vector &vecBoxExtents, const CProp_Portal *pPortal, float flTolerance )
{
	if( pPortal == NULL )
		return false;

	return UTIL_IsBoxIntersectingPortal( vecBoxCenter, vecBoxExtents, pPortal->m_ptOrigin, pPortal->m_qAbsAngle, flTolerance );
}

CProp_Portal *UTIL_IntersectEntityExtentsWithPortal( const CBaseEntity *pEntity )
{
	int iPortalCount = CProp_Portal_Shared::AllPortals.Count();
	if( iPortalCount == 0 )
		return NULL;

	Vector vMin, vMax;
	pEntity->CollisionProp()->WorldSpaceAABB( &vMin, &vMax );
	Vector ptCenter = ( vMin + vMax ) * 0.5f;
	Vector vExtents = ( vMax - vMin ) * 0.5f;

	CProp_Portal **pPortals = CProp_Portal_Shared::AllPortals.Base();
	for( int i = 0; i != iPortalCount; ++i )
	{
		CProp_Portal *pTempPortal = pPortals[i];
		if( pTempPortal->m_bActivated && 
			(pTempPortal->m_hLinkedPortal.Get() != NULL) &&
			UTIL_IsBoxIntersectingPortal( ptCenter, vExtents, pTempPortal )	)
		{
			return pPortals[i];
		}
	}

	return NULL;
}

void UTIL_Portal_NDebugOverlay( const Vector &ptPortalCenter, const QAngle &qPortalAngles, int r, int g, int b, int a, bool noDepthTest, float duration )
{
#ifndef CLIENT_DLL
	Vector pvTri1[ 3 ], pvTri2[ 3 ];

	UTIL_Portal_Triangles( ptPortalCenter, qPortalAngles, pvTri1, pvTri2 );

	NDebugOverlay::Triangle( pvTri1[ 0 ], pvTri1[ 1 ], pvTri1[ 2 ], r, g, b, a, noDepthTest, duration );
	NDebugOverlay::Triangle( pvTri2[ 0 ], pvTri2[ 1 ], pvTri2[ 2 ], r, g, b, a, noDepthTest, duration );
#endif //#ifndef CLIENT_DLL
}

void UTIL_Portal_NDebugOverlay( const CProp_Portal *pPortal, int r, int g, int b, int a, bool noDepthTest, float duration )
{
#ifndef CLIENT_DLL
	UTIL_Portal_NDebugOverlay( pPortal->m_ptOrigin, pPortal->m_qAbsAngle, r, g, b, a, noDepthTest, duration );
#endif //#ifndef CLIENT_DLL
}


bool FindClosestPassableSpace( CBaseEntity *pEntity, const Vector &vIndecisivePush, unsigned int fMask ) //assumes the object is already in a mostly passable space
{
	if ( sv_use_find_closest_passable_space.GetBool() == false )
		return true;

	// Don't ever do this to entities with a move parent
	if ( pEntity->GetMoveParent() )
		return true;

#ifndef CLIENT_DLL
	ADD_DEBUG_HISTORY( HISTORY_PLAYER_DAMAGE, UTIL_VarArgs( "RUNNING FIND CLOSEST PASSABLE SPACE on %s..\n", pEntity->GetDebugName() ) );
#endif

	Vector ptExtents[8]; //ordering is going to be like 3 bits, where 0 is a min on the related axis, and 1 is a max on the same axis, axis order x y z

	float fExtentsValidation[8]; //some points are more valid than others, and this is our measure


	Vector vEntityMaxs;// = pEntity->WorldAlignMaxs();
	Vector vEntityMins;// = pEntity->WorldAlignMins();
	CCollisionProperty *pEntityCollision = pEntity->CollisionProp();
	pEntityCollision->WorldSpaceAABB( &vEntityMins, &vEntityMaxs );



	Vector ptEntityCenter = ((vEntityMins + vEntityMaxs) / 2.0f);
	vEntityMins -= ptEntityCenter;
	vEntityMaxs -= ptEntityCenter;

	Vector ptEntityOriginalCenter = ptEntityCenter;
	
	ptEntityCenter.z += 0.001f; //to satisfy m_IsSwept on first pass

	int iEntityCollisionGroup = pEntity->GetCollisionGroup();

	trace_t traces[2];
	Ray_t entRay;
	//entRay.Init( ptEntityCenter, ptEntityCenter, vEntityMins, vEntityMaxs );
	entRay.m_Extents = vEntityMaxs;
	entRay.m_IsRay = false;
	entRay.m_IsSwept = true;
	entRay.m_StartOffset = vec3_origin;

	Vector vOriginalExtents = vEntityMaxs;	

	Vector vGrowSize = vEntityMaxs / 101.0f;
	vEntityMaxs -= vGrowSize;
	vEntityMins += vGrowSize;
	
	
	Ray_t testRay;
	testRay.m_Extents = vGrowSize;
	testRay.m_IsRay = false;
	testRay.m_IsSwept = true;
	testRay.m_StartOffset = vec3_origin;



	unsigned int iFailCount;
	for( iFailCount = 0; iFailCount != 100; ++iFailCount )
	{
		entRay.m_Start = ptEntityCenter;
		entRay.m_Delta = ptEntityOriginalCenter - ptEntityCenter;

		UTIL_TraceRay( entRay, fMask, pEntity, iEntityCollisionGroup, &traces[0] );
		if( traces[0].startsolid == false )
		{
			Vector vNewPos = traces[0].endpos + (pEntity->GetAbsOrigin() - ptEntityOriginalCenter);
#ifdef CLIENT_DLL
			pEntity->SetAbsOrigin( vNewPos );
#else
			pEntity->Teleport( &vNewPos, NULL, NULL );
#endif
			return true; //current placement worked
		}

		bool bExtentInvalid[8];
		for( int i = 0; i != 8; ++i )
		{
			fExtentsValidation[i] = 0.0f;
			ptExtents[i] = ptEntityCenter;
			ptExtents[i].x += ((i & (1<<0)) ? vEntityMaxs.x : vEntityMins.x);
			ptExtents[i].y += ((i & (1<<1)) ? vEntityMaxs.y : vEntityMins.y);
			ptExtents[i].z += ((i & (1<<2)) ? vEntityMaxs.z : vEntityMins.z);

			bExtentInvalid[i] = enginetrace->PointOutsideWorld( ptExtents[i] );
		}

		unsigned int counter, counter2;
		for( counter = 0; counter != 7; ++counter )
		{
			for( counter2 = counter + 1; counter2 != 8; ++counter2 )
			{

				testRay.m_Delta = ptExtents[counter2] - ptExtents[counter];
				
				if( bExtentInvalid[counter] )
					traces[0].startsolid = true;
				else
				{
					testRay.m_Start = ptExtents[counter];
					UTIL_TraceRay( testRay, fMask, pEntity, iEntityCollisionGroup, &traces[0] );
				}

				if( bExtentInvalid[counter2] )
					traces[1].startsolid = true;
				else
				{
					testRay.m_Start = ptExtents[counter2];
					testRay.m_Delta = -testRay.m_Delta;
					UTIL_TraceRay( testRay, fMask, pEntity, iEntityCollisionGroup, &traces[1] );
				}

				float fDistance = testRay.m_Delta.Length();

				for( int i = 0; i != 2; ++i )
				{
					int iExtent = (i==0)?(counter):(counter2);

					if( traces[i].startsolid )
					{
						fExtentsValidation[iExtent] -= 100.0f;
					}
					else
					{
						fExtentsValidation[iExtent] += traces[i].fraction * fDistance;
					}
				}
			}
		}

		Vector vNewOriginDirection( 0.0f, 0.0f, 0.0f );
		float fTotalValidation = 0.0f;
		for( counter = 0; counter != 8; ++counter )
		{
			if( fExtentsValidation[counter] > 0.0f )
			{
				vNewOriginDirection += (ptExtents[counter] - ptEntityCenter) * fExtentsValidation[counter];
				fTotalValidation += fExtentsValidation[counter];
			}
		}

		if( fTotalValidation != 0.0f )
		{
			ptEntityCenter += (vNewOriginDirection / fTotalValidation);

			//increase sizing
			testRay.m_Extents += vGrowSize;
			vEntityMaxs -= vGrowSize;
			vEntityMins = -vEntityMaxs;
		}
		else
		{
			//no point was valid, apply the indecisive vector
			ptEntityCenter += vIndecisivePush;

			//reset sizing
			testRay.m_Extents = vGrowSize;
			vEntityMaxs = vOriginalExtents;
			vEntityMins = -vEntityMaxs;
		}		
	}

	// X360TBD: Hits in portal devtest
	AssertMsg( IsX360() || iFailCount != 100, "FindClosestPassableSpace() failure." );
	return false;
}

bool UTIL_Portal_EntityIsInPortalHole( const CProp_Portal *pPortal, CBaseEntity *pEntity )
{
	CCollisionProperty *pCollisionProp = pEntity->CollisionProp();
	Vector vMins = pCollisionProp->OBBMins();
	Vector vMaxs = pCollisionProp->OBBMaxs();
	Vector vForward, vUp, vRight;
	AngleVectors( pCollisionProp->GetCollisionAngles(), &vForward, &vRight, &vUp );
	Vector ptOrigin = pEntity->GetAbsOrigin();

	Vector ptOBBCenter = pEntity->GetAbsOrigin() + (vMins + vMaxs * 0.5f);
	Vector vExtents = (vMaxs - vMins) * 0.5f;

	vForward *= vExtents.x;
	vRight *= vExtents.y;
	vUp *= vExtents.z;

	Vector vPortalForward, vPortalRight, vPortalUp;
	pPortal->GetVectors( &vPortalForward, &vPortalRight, &vPortalUp );
	Vector ptPortalCenter = pPortal->GetAbsOrigin();

	return OBBHasFullyContainedIntersectionWithQuad( vForward, vRight, vUp, ptOBBCenter, 
		vPortalForward, vPortalForward.Dot( ptPortalCenter ), ptPortalCenter, 
		vPortalRight, PORTAL_HALF_WIDTH + 1.0f, vPortalUp, PORTAL_HALF_HEIGHT + 1.0f );
}

const Vector UTIL_ProjectPointOntoPlane( const Vector& point, const cplane_t& plane )
{
	return point - (DotProduct( point, plane.normal ) - plane.dist) * plane.normal;
}

bool UTIL_PointIsNearPortal( const Vector& point, const CProp_Portal* pPortal2D, float planeDist, float radiusReduction )
{
	AssertMsg( pPortal2D != NULL, "Null pointers are bad, and you should feel bad." );

	const cplane_t& portalPlane = pPortal2D->m_plane_Origin;
	Vector transformedPt, origin;
	pPortal2D->WorldToEntitySpace( point, &transformedPt );
	pPortal2D->WorldToEntitySpace( UTIL_ProjectPointOntoPlane( pPortal2D->WorldSpaceCenter(), portalPlane ), &origin );

	AssertMsg( PORTAL_HALF_WIDTH > radiusReduction && PORTAL_HALF_HEIGHT > radiusReduction, "Reduction of the box is too high." );
	const float halfWidth = PORTAL_HALF_WIDTH - radiusReduction;
	const float halfHeight = PORTAL_HALF_HEIGHT - radiusReduction;
	const Vector boxMin( origin[0] - planeDist, origin[1] - halfWidth, origin[2] - halfHeight );
	const Vector boxMax( origin[0] + planeDist, origin[1] + halfWidth, origin[2] + halfHeight );

	return (transformedPt[0] >= boxMin[0] && transformedPt[0] <= boxMax[0]) &&
		   (transformedPt[1] >= boxMin[1] && transformedPt[1] <= boxMax[1]) &&
		   (transformedPt[2] >= boxMin[2] && transformedPt[2] <= boxMax[2]);
}

#ifdef CLIENT_DLL
void UTIL_TransformInterpolatedAngle( CInterpolatedVar< QAngle > &qInterped, matrix3x4_t matTransform, float fUpToTime )
{
	int iHead = qInterped.GetHead();
	if( !qInterped.IsValidIndex( iHead ) )
		return;

#ifdef _DEBUG
	float fHeadTime;
	qInterped.GetHistoryValue( iHead, fHeadTime );
#endif

	float fTime;
	QAngle *pCurrent;
	int iCurrent;

	//if( bSkipNewest )
	//	iCurrent = qInterped.GetNext( iHead );
	//else
		iCurrent = iHead;

	while( (pCurrent = qInterped.GetHistoryValue( iCurrent, fTime )) != NULL )
	{
		Assert( (fTime <= fHeadTime) || (iCurrent == iHead) ); //asserting that head is always newest
		
		if( fTime < fUpToTime )
			*pCurrent = TransformAnglesToWorldSpace( *pCurrent, matTransform );

		iCurrent = qInterped.GetNext( iCurrent );
		if( iCurrent == iHead )
			break;
	}

	//qInterped.Interpolate( gpGlobals->curtime );
}

void UTIL_TransformInterpolatedPosition( CInterpolatedVar< Vector > &vInterped, const VMatrix& matTransform, float fUpToTime )
{
	int iHead = vInterped.GetHead();
	if( !vInterped.IsValidIndex( iHead ) )
		return;

#ifdef _DEBUG
	float fHeadTime;
	vInterped.GetHistoryValue( iHead, fHeadTime );
#endif

	float fTime;
	Vector *pCurrent;
	int iCurrent;

	//if( bSkipNewest )
	//	iCurrent = vInterped.GetNext( iHead );
	//else
		iCurrent = iHead;

	while( (pCurrent = vInterped.GetHistoryValue( iCurrent, fTime )) != NULL )
	{
		Assert( (fTime <= fHeadTime) || (iCurrent == iHead) );

		if( fTime < fUpToTime )
		{
			*pCurrent = matTransform * (*pCurrent);


		}

		iCurrent = vInterped.GetNext( iCurrent );
		if( iCurrent == iHead )
			break;
	}

	//vInterped.Interpolate( gpGlobals->curtime );
}
#endif


#ifndef CLIENT_DLL

void CC_Debug_FixMyPosition( void )
{
	CBaseEntity *pPlayer = UTIL_GetCommandClient();

	FindClosestPassableSpace( pPlayer, vec3_origin );
}

static ConCommand debug_fixmyposition("debug_fixmyposition", CC_Debug_FixMyPosition, "Runs FindsClosestPassableSpace() on player.", FCVAR_CHEAT );
#endif

//it turns out that using MatrixInverseTR() is theoretically correct. But we need to ensure that these matrices match exactly on the client/server. 
//And computing inverses screws that up just enough (differences of ~0.00005 in the translation some times) to matter. So we compute each from scratch every time
#if defined( CLIENT_DLL )
void UTIL_Portal_ComputeMatrix_ForReal( CPortalRenderable_FlatBasic *pLocalPortal, CPortalRenderable_FlatBasic *pRemotePortal )
#else
void UTIL_Portal_ComputeMatrix_ForReal( CPortal_Base2D *pLocalPortal, CPortal_Base2D *pRemotePortal )
#endif
{
	// FIXME:
#ifdef GAME_DLL
#define m_ptOrigin GetAbsOrigin()
#endif
	VMatrix worldToLocal_Rotated;
	worldToLocal_Rotated.m[0][0] = -pLocalPortal->m_vForward.x;
	worldToLocal_Rotated.m[0][1] = -pLocalPortal->m_vForward.y;
	worldToLocal_Rotated.m[0][2] = -pLocalPortal->m_vForward.z;
	worldToLocal_Rotated.m[0][3] = ((Vector)pLocalPortal->m_ptOrigin).Dot( pLocalPortal->m_vForward );

	worldToLocal_Rotated.m[1][0] = pLocalPortal->m_vRight.x;
	worldToLocal_Rotated.m[1][1] = pLocalPortal->m_vRight.y;
	worldToLocal_Rotated.m[1][2] = pLocalPortal->m_vRight.z;
	worldToLocal_Rotated.m[1][3] = -((Vector)pLocalPortal->m_ptOrigin).Dot( pLocalPortal->m_vRight );

	worldToLocal_Rotated.m[2][0] = pLocalPortal->m_vUp.x;
	worldToLocal_Rotated.m[2][1] = pLocalPortal->m_vUp.y;
	worldToLocal_Rotated.m[2][2] = pLocalPortal->m_vUp.z;
	worldToLocal_Rotated.m[2][3] = -((Vector)pLocalPortal->m_ptOrigin).Dot( pLocalPortal->m_vUp );		

	worldToLocal_Rotated.m[3][0] = 0.0f;
	worldToLocal_Rotated.m[3][1] = 0.0f;
	worldToLocal_Rotated.m[3][2] = 0.0f;
	worldToLocal_Rotated.m[3][3] = 1.0f;

	VMatrix remoteToWorld( pRemotePortal->m_vForward, -pRemotePortal->m_vRight, pRemotePortal->m_vUp );
	remoteToWorld.SetTranslation( pRemotePortal->m_ptOrigin );

	//final
	pLocalPortal->m_matrixThisToLinked = remoteToWorld * worldToLocal_Rotated;

#undef m_ptOrigin
}

//MUST be a shared function to prevent floating point precision weirdness in the 100,000th decimal place between client/server that we're attributing to differing register usage.
#if defined( CLIENT_DLL )
void UTIL_Portal_ComputeMatrix( CPortalRenderable_FlatBasic *pLocalPortal, CPortalRenderable_FlatBasic *pRemotePortal )
#else
void UTIL_Portal_ComputeMatrix( CPortal_Base2D *pLocalPortal, CPortal_Base2D *pRemotePortal )
#endif
{
	if ( pRemotePortal != NULL )
	{
		UTIL_Portal_ComputeMatrix_ForReal( pLocalPortal, pRemotePortal );
		UTIL_Portal_ComputeMatrix_ForReal( pRemotePortal, pLocalPortal );
	}
	else
	{
		pLocalPortal->m_matrixThisToLinked.Identity(); //don't accidentally teleport objects to zero space
	}
}

CBasePlayer* UTIL_OtherPlayer( CBasePlayer const* pPlayer )
{
	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer* pOtherPlayer = UTIL_PlayerByIndex( i );
		if ( pOtherPlayer != NULL && pOtherPlayer != pPlayer )
			return pOtherPlayer;
	}

	return NULL;
}

#ifdef GAME_DLL

CBasePlayer* UTIL_OtherConnectedPlayer( CBasePlayer const* pPlayer )
{
	CBasePlayer *pOtherPlayer = UTIL_OtherPlayer( pPlayer );
	if ( pOtherPlayer && pOtherPlayer->IsConnected() )
	{
		return pOtherPlayer;
	}

	return NULL;
}

#endif

CEG_NOINLINE bool UTIL_IsPaintableSurface( const csurface_t& surface )
{
	// What the fuck?
	/*
	CEG_GCV_PRE();
	static const unsigned short CEG_SURF_NO_PAINT_FLAG = CEG_GET_CONSTANT_VALUE( SurfNoPaintFlag );
	CEG_GCV_POST();
	return !( surface.flags & CEG_SURF_NO_PAINT_FLAG );
	*/
	return true;
}



float UTIL_PaintBrushEntity( CBaseEntity* pBrushEntity, const Vector& contactPoint, PaintPowerType power, float flPaintRadius, float flAlphaPercent )
{
	if ( !pBrushEntity )
	{
		// HACK HACK: Fix it for real Bank! :)
		return 0.0f;
	}

	if ( !pBrushEntity->IsBSPModel() )
	{
		return 0.0f;
	}

	Vector vEntitySpaceContactPoint;
	pBrushEntity->WorldToEntitySpace( contactPoint, &vEntitySpaceContactPoint );

	// Doesn't exist in Alien Swarm engine
	//if ( !engine->SpherePaintSurface( pBrushEntity->GetModel(), vEntitySpaceContactPoint, power, flPaintRadius, flAlphaPercent ) )
	//	return 0.0f;

#if 0
	Msg("power: %i\n", power);
#endif

	Color preColor = MapPowerToColor(power);
	
	Color color = preColor;

	//engine->TracePaintSurface( pBrushEntity->GetModel(), vEntitySpaceContactPoint, flPaintRadius, color );
	
	// We need to run this twice to make sure we have 2 color values.
	engine->PaintSurface( pBrushEntity->GetModel(), vEntitySpaceContactPoint, color, sv_paint_detection_sphere_radius.GetFloat() );

#if 0
#ifdef GAME_DLL
	Warning("(server)Pre Painted Color: %i %i %i %i\n", preColor.r(), preColor.b(), preColor.g(), preColor.a());
	Warning("(server)Post Painted Color: %i %i %i %i\n", color.r(), preColor.b(), color.g(), color.a());
#else
	Warning("(client)Pre Painted Color: %i %i %i %i\n", preColor.r(), preColor.b(), preColor.g(), preColor.a());
	Warning("(client)Post Painted Color: %i %i %i %i\n", color.r(), preColor.b(), color.g(), color.a());
#endif
#endif
	//if (color == preColor)
	//	return 0.0f;
	return flPaintRadius;
}

Color GetAveragePaintColorFromVector( CUtlVector<Color> &color )
{
	if ( color.Count() == 0 )
		return Color( 0, 0, 0, 0 );

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;
	
	// We really need to get these power colors
	for (int i = 0; i < color.Count(); ++i)
	{
		//Color tColor = color;
		Color tColor = color.Element(i);
#if 0
#ifdef GAME_DLL
		Msg("(server)tColor: %i %i %i %i\n", tColor.r(), tColor.g(), tColor.b(), tColor.a());
#else
		Msg("(client)tColor: %i %i %i %i\n", tColor.r(), tColor.g(), tColor.b(), tColor.a());
#endif
#endif
		// Add up all values
		r += tColor.r();
		g += tColor.g();
		b += tColor.b();
		a += tColor.a();
		
	}

	// Get the average
	r = r / color.Count();
	g = g / color.Count();
	b = b / color.Count();
	a = a / color.Count();

	Assert( r >= 0 || r <= 255 );
	Assert( b >= 0 || b <= 255 );
	Assert( g >= 0 || g <= 255 );
	Assert( a >= 0 || a <= 255 );
#if 0
#ifdef GAME_DLL
	Msg("(server)Average Color: %i %i %i %i\n", r, g, b, a);
#else
	Msg("(client)Average Color: %i %i %i %i\n", r, g, b, a);
#endif
#endif
	return Color( r, g, b, a );
}

PaintPowerType UTIL_Paint_TracePower( CBaseEntity* pBrushEntity, const Vector& contactPoint, const Vector& vContactNormal )
{
	if ( !pBrushEntity->IsBSPModel() )
	{
		return NO_POWER;
	}

	CUtlVector<Color> color;

	// Transform contact point from world to entity space
	Vector vEntitySpaceContactPoint;
	pBrushEntity->WorldToEntitySpace( contactPoint, &vEntitySpaceContactPoint );

	// transform contact normal
	Vector vTransformedContactNormal;
	VectorRotate( vContactNormal, -pBrushEntity->GetAbsAngles(), vTransformedContactNormal );

	// Doesn't exist in Alien Swarm engine
	//engine->SphereTracePaintSurface( pBrushEntity->GetModel(), vEntitySpaceContactPoint, vTransformedContactNormal, sv_paint_detection_sphere_radius.GetFloat(), color );
	
	engine->TracePaintSurface( pBrushEntity->GetModel(), vEntitySpaceContactPoint, sv_paint_detection_sphere_radius.GetFloat(), color );
	
	//Msg("color.Count(): %i\n", color.Count() );
	
	//if ( color.Count() != 0 && &color.Element( 0 ) != NULL )
		//return MapColorToPower( color.Element(0) );
	
	//return NO_POWER;

	return MapColorToPower( GetAveragePaintColorFromVector( color ) );

}


bool UTIL_Paint_Reflect( const trace_t& tr, Vector& vStart, Vector& vDir, PaintPowerType reflectPower /* = REFLECT_POWER */ )
{
	// check for reflect paint
	if ( HASPAINTMAP && tr.m_pEnt && tr.m_pEnt->IsBSPModel() )
	{
		PaintPowerType power = UTIL_Paint_TracePower( tr.m_pEnt, tr.endpos, tr.plane.normal );
		if ( power == reflectPower )
		{
			Vector vecIn = tr.endpos - tr.startpos;
			if ( DotProduct( vecIn.Normalized(), tr.plane.normal ) > -0.99f )
			{
				Vector vecReflect = vecIn - 2 * DotProduct( vecIn, tr.plane.normal ) * tr.plane.normal;

				vStart = tr.endpos;
				vDir = vecReflect.Normalized();

				// FIXME: Bring this back for DLC2
				//if ( reflect_paint_vertical_snap.GetBool() )
				{
					float flDot = DotProduct( tr.plane.normal, Vector(0.f,0.f,1.f) );

					if ( vDir.z > 0.1 && flDot > 0 && flDot < 1  )
					{
						vDir = Vector( 0.f, 0.f, 1.f );
					}
					else if( vDir.z < -0.1f && flDot > 0 && flDot < 1 )
					{
						vDir = Vector( 0.f, 0.f, -1.f );
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool IsIn180( float f )
{
	return f >= 0.f && f <= 180.f;
}

bool IsInNeg180( float f )
{
	return f < 0.f && f >= -180.f;
}


float DistTo180( float f )
{
	if ( IsIn180( f ) )
	{
		return 180.f - f;
	}
	else if ( IsInNeg180( f ) )
	{
		return -180 - f;
	}
	else
	{
		Assert("We are out of bound [-180,180]");
		return 0.f;
	}
}


void UTIL_NormalizedAngleDiff( const QAngle& start, const QAngle& end, QAngle* result )
{
	if ( result )
	{
		QAngle angles;
		for ( int i=0; i<3; ++i )
		{
			float a = start[ i ];
			float b = end[ i ];

			float distAto180 = DistTo180( a );
			float distBto180 = DistTo180( b );

			bool bUse180 = ( fabs( distAto180 ) + fabs( distBto180 ) ) < ( fabs( a ) + fabs( b ) );
			bool bDiffSign = Sign( a ) * Sign( b ) < 0;

			if ( bDiffSign )
			{
				if ( bUse180 )
				{
					angles[ i ] = distBto180 - distAto180;
				}
				else
				{
					angles[ i ] = a - b;
				}
			}
			else
			{
				angles[ i ] = a - b;
			}
		}
		
		*result = -angles;
	}
}

#ifdef GAME_DLL
bool CBrushEntityList::EnumEntity( IHandleEntity *pHandleEntity )
{
	if ( !pHandleEntity )
		return true;

	CBaseEntity *pEntity = gEntList.GetBaseEntity( pHandleEntity->GetRefEHandle() );
	if ( !pEntity )
		return true;

	if ( pEntity->IsBSPModel() )
	{
		m_BrushEntitiesToPaint.AddToTail( pEntity );
	}

	return true;
}


void UTIL_FindBrushEntitiesInSphere( CBrushEntityList& brushEnum, const Vector& vCenter, float flRadius )
{
	Vector vExtents = flRadius * Vector( 1.f, 1.f, 1.f );
	enginetrace->EnumerateEntities( vCenter - vExtents, vCenter + vExtents, &brushEnum );
}
#endif

struct FindClosestPassableSpace_TraceAdapter_Portal_t : public FindClosestPassableSpace_TraceAdapter_t
{
	const CPortal_Base2D *pPortal;
};

static void PortalTraceFunc( const Ray_t &ray, trace_t *pResult, FindClosestPassableSpace_TraceAdapter_t *pTraceAdapter )
{
	UTIL_Portal_TraceRay( ((FindClosestPassableSpace_TraceAdapter_Portal_t *)pTraceAdapter)->pPortal, ray, pTraceAdapter->fMask, pTraceAdapter->pTraceFilter, pResult, true );
}

struct FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t : FindClosestPassableSpace_TraceAdapter_Portal_t
{
	VPlane shiftedPlane;//we only want the center of the starting box to stay in front, not every trace. So we create a plane of solidity that is not necessarily coplanar with the portal quad
	Vector vExtentSigns; //when testing planar distance we need to use the extent closest to the solidity plane, multiply the ray extents by these to get that point.
};

static bool PortalPointOutsideWorldFunc( const Vector &vTest, FindClosestPassableSpace_TraceAdapter_t *pTraceAdapter )
{
	trace_t tr;
	Ray_t testRay;
	testRay.Init( vTest, vTest );

	CTraceFilterWorldOnly traceFilter;

	UTIL_Portal_TraceRay( ((FindClosestPassableSpace_TraceAdapter_Portal_t *)pTraceAdapter)->pPortal, testRay, MASK_SOLID_BRUSHONLY, &traceFilter, &tr );
	return tr.startsolid;
}

bool UTIL_FindClosestPassableSpace_InPortal( const CPortal_Base2D *pPortal, const Vector &vCenter, const Vector &vExtents, const Vector &vIndecisivePush, ITraceFilter *pTraceFilter, unsigned int fMask, unsigned int iIterations, Vector &vCenterOut )
{
	FindClosestPassableSpace_TraceAdapter_Portal_t adapter;
	adapter.pTraceFunc = PortalTraceFunc;
	adapter.pPointOutsideWorldFunc = PortalPointOutsideWorldFunc;
	adapter.pTraceFilter = pTraceFilter;
	adapter.fMask = fMask;

	adapter.pPortal = pPortal;

	return UTIL_FindClosestPassableSpace( vCenter, vExtents, vIndecisivePush, iIterations, vCenterOut, FL_AXIS_DIRECTION_NONE, &adapter );
}

static bool PortalPointOutsideWorldFunc_CenterMustStayInFront( const Vector &vTest, FindClosestPassableSpace_TraceAdapter_t *pTraceAdapter )
{
	const FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t *pCastedData = (const FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t *)pTraceAdapter;
	if( pCastedData->shiftedPlane.DistTo( vTest ) < 0.0f )
		return true;

	return PortalPointOutsideWorldFunc( vTest, pTraceAdapter );
}

static const Ray_t *AdjustRayToPlane( const Ray_t &originalRay, Ray_t &alterableRay, const Vector &vExtentSigns, const VPlane &Plane, float &fDeltaScale )
{
	Vector vExtentShift;
	vExtentShift.x = originalRay.m_Extents.x * vExtentSigns.x;
	vExtentShift.y = originalRay.m_Extents.y * vExtentSigns.y;
	vExtentShift.z = originalRay.m_Extents.z * vExtentSigns.z;
	float fExtentShiftDist = Plane.m_Normal.Dot( vExtentShift );

	if( (Plane.DistTo( originalRay.m_Start ) + fExtentShiftDist) < 0.0f )
	{
		//start solid
		NULL;
	}
	else if( (Plane.DistTo( originalRay.m_Start + originalRay.m_Delta ) + fExtentShiftDist) < 0.0f )
	{
		//end will be behind shifted plane, shorten the delta now, then expand the results on the tail end
		alterableRay = originalRay;

		float fEndDist = (Plane.DistTo( originalRay.m_Start + originalRay.m_Delta ) + fExtentShiftDist);
		float fDeltaLength = originalRay.m_Delta.Length();
		Vector vDeltaNormalized = originalRay.m_Delta / fDeltaLength;
		float fNewDeltaLength = fDeltaLength - (Plane.m_Normal.Dot( vDeltaNormalized ) * fEndDist);
		alterableRay.m_Delta = vDeltaNormalized * fNewDeltaLength;

		fDeltaScale = fNewDeltaLength / fDeltaLength;
		return &alterableRay;
	}
	else
	{
		fDeltaScale = 1.0f;
	}
	return &originalRay;
}

static void PortalTraceFunc_CenterMustStayInFront( const Ray_t &ray, trace_t *pResult, FindClosestPassableSpace_TraceAdapter_t *pTraceAdapter )
{
	const FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t *pCastedData = (const FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t *)pTraceAdapter;
	
	float fDeltaScale;
	Ray_t alterableRay;
	const Ray_t *pUseRay = AdjustRayToPlane( ray, alterableRay, pCastedData->vExtentSigns, pCastedData->shiftedPlane, fDeltaScale );
	if( pUseRay == NULL )
	{
		//start solid
		UTIL_ClearTrace( *pResult );
		pResult->startsolid = true;
		pResult->fraction = 0.0f;
		pResult->allsolid = true; //TODO: bother calculating if it leaves solid?
		return;
	}

	UTIL_Portal_TraceRay( pCastedData->pPortal, *pUseRay, pTraceAdapter->fMask, pTraceAdapter->pTraceFilter, pResult, true );

	if( pUseRay == &alterableRay )
	{
		//fixup any abnormalities from using a shortened ray
		if( !pResult->DidHit() )
		{
			pResult->m_pEnt = (CPortal_Base2D *)pCastedData->pPortal;
			pResult->plane	= pCastedData->pPortal->m_plane_Origin;
			pResult->plane.dist = pCastedData->shiftedPlane.m_Dist;
		}
		
		pResult->fraction *= fDeltaScale;
		pResult->fractionleftsolid *= fDeltaScale;		
	}
}

bool UTIL_FindClosestPassableSpace_InPortal_CenterMustStayInFront( const CPortal_Base2D *pPortal, const Vector &vCenter, const Vector &vExtents, const Vector &vIndecisivePush, ITraceFilter *pTraceFilter, unsigned int fMask, unsigned int iIterations, Vector &vCenterOut )
{
	FindClosestPassableSpace_TraceAdapter_Portal_StayInFront_t adapter;
	adapter.pTraceFunc = PortalTraceFunc_CenterMustStayInFront;
	adapter.pPointOutsideWorldFunc = PortalPointOutsideWorldFunc_CenterMustStayInFront;
	adapter.pTraceFilter = pTraceFilter;
	adapter.fMask = fMask;

	adapter.pPortal = pPortal;

	adapter.vExtentSigns.x = -Sign( pPortal->m_plane_Origin.normal.x );
	adapter.vExtentSigns.y = -Sign( pPortal->m_plane_Origin.normal.y );
	adapter.vExtentSigns.z = -Sign( pPortal->m_plane_Origin.normal.z );
	
	//when caclulating the shift plane, all we need to be sure of is that the most penetrating extent is coplanar with the shift plane when the center would be coplanar with the original plane
	Vector vCoplanarExtent;
	vCoplanarExtent.x = vExtents.x * adapter.vExtentSigns.x;
	vCoplanarExtent.y = vExtents.y * adapter.vExtentSigns.y;
	vCoplanarExtent.z = vExtents.z * adapter.vExtentSigns.z;

	adapter.shiftedPlane.m_Normal = pPortal->m_plane_Origin.normal;
	adapter.shiftedPlane.m_Dist = pPortal->m_plane_Origin.dist + (pPortal->m_plane_Origin.normal.Dot( vCoplanarExtent ) ); //the dot is known to be negative, shifting the plane back so the extent is coplanar with it.
	
	return UTIL_FindClosestPassableSpace( vCenter, vExtents, vIndecisivePush, iIterations, vCenterOut, FL_AXIS_DIRECTION_NONE, &adapter );
}