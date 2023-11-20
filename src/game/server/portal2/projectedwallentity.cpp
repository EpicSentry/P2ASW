#include "cbase.h"
#include "projectedwallentity.h"
#include "paint/paint_database.h"
#include "vcollide_parse.h"

#ifndef NO_PROJECTED_WALL

IMPLEMENT_AUTO_LIST( IProjectedWallEntityAutoList )

LINK_ENTITY_TO_CLASS( projected_wall_entity, CProjectedWallEntity )

CProjectedWallEntity::CProjectedWallEntity()
{

}

CProjectedWallEntity::~CProjectedWallEntity()
{
	CleanupWall();
	PaintDatabase.RemovePaintedWall(this, 1);
}

void CProjectedWallEntity::Precache( void )
{
	PrecacheParticleSystem("projected_wall_impact");
}

void CProjectedWallEntity::OnRestore( void )
{
	BaseClass::OnRestore();
	SetTransmitState( 8 );
}

void CProjectedWallEntity::SetHitPortal( CPortal_Base2D *pPortal )
{
	if ( pPortal )
	{
		if( !DidRedirectionPortalMove( pPortal ) )
			goto LABEL_11;
	}
	else if ( !GetHitPortal() )
	{
		goto LABEL_11;
	}
	m_flParticleUpdateTime = gpGlobals->curtime + 0.5;

LABEL_11:
	BaseClass::SetHitPortal( pPortal );
}
void CProjectedWallEntity::SetSourcePortal( CPortal_Base2D *pPortal)
{
	m_flParticleUpdateTime = gpGlobals->curtime + 0.5;
	m_hColorPortal = pPortal;

	BaseClass::SetSourcePortal( pPortal );
}

void CProjectedWallEntity::GetProjectionExtents( Vector *const outMins, Vector *const outMaxs )
{
	BaseClass::GetExtents( outMins, outMaxs );
}

bool CProjectedWallEntity::ShouldSavePhysics( void )
{
	return false;
}

bool CProjectedWallEntity::CreateVPhysics( void )
{
	ProjectWall();
	return 1;
}

void CProjectedWallEntity::ProjectWall( void )
{
}

bool CProjectedWallEntity::TestCollision( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr )
{
	if ( !m_pWallCollideable )
		return false;

	physcollision->TraceBox( ray, fContentsMask, 0, m_pWallCollideable, vec3_origin, vec3_angle, &tr );
	
	tr.surface.name = "hard_light_bridge";
	tr.surface.flags = 0;
	tr.surface.surfaceProps = s_HardLightBridgeSurfaceProps;
	if ( !(tr.fraction >= 1.0) || tr.allsolid || tr.startsolid )
		return true;

	return false;
}

int CProjectedWallEntity::ObjectCaps( void )
{
	return ObjectCaps() & 1;
}

void CProjectedWallEntity::ComputeWorldSpaceSurroundingBox( Vector *pWorldMins, Vector *pWorldMaxs )
{
	pWorldMins = &m_vWorldSpace_WallMins.m_Value;
	pWorldMaxs = &m_vWorldSpace_WallMaxs.m_Value;
}

void CProjectedWallEntity::OnPreProjected( void )
{
	 CheckForSettledReflectorCubes();
}

void CProjectedWallEntity::OnProjected( void )
{
	BaseClass::OnProjected();
	ProjectWall();

	m_flParticleUpdateTime = gpGlobals->curtime + 0.5;
}

bool CProjectedWallEntity::IsWallPainted( const Vector &vecPosition )
{
	return CProjectedWallEntity::GetPaintPowerAtPoint( vecPosition ) != NO_POWER;
}

PaintPowerType CProjectedWallEntity::GetPaintPowerAtPoint( const Vector &worldContactPt ) const
{
	return m_PaintPowers[CProjectedWallEntity::ComputeSegmentIndex( worldContactPt )];
}

PaintPowerType CProjectedWallEntity::GetPaintPowerAtSegment( int i )
{
	return m_PaintPowers[i];
}

CBaseProjectedEntity *CProjectedWallEntity::CreateNewProjectedEntity( void )
{
	return CProjectedWallEntity::CreateNewInstance();
}

CProjectedWallEntity *CProjectedWallEntity::CreateNewInstance(void)
{
	return (CProjectedWallEntity *)CreateEntityByName( "projected_wall_entity" );
}

void WallPainted( int colorIndex, int nSegment, CBaseEntity *pWall )
{
	CBaseEntity *pEntity = g_TEWallPaintedEvent.m_hEntity;

	CRecipientFilter filter;
	filter.AddAllPlayers();

	if ( pEntity != pWall )
	{
		if ( pWall )
			g_TEWallPaintedEvent.m_hEntity = pWall;
		else
			g_TEWallPaintedEvent.m_hEntity = NULL;
	}

	g_TEWallPaintedEvent.m_colorIndex = colorIndex;
	g_TEWallPaintedEvent.m_nSegment = nSegment;

	g_TEWallPaintedEvent.Create( filter, 0.0 );
}

CTEWallPaintedEvent g_TEWallPaintedEvent("WallPaintedEvent");

#endif // NO_PROJECTED_WALL