#include "cbase.h"
#include "c_projectedwallentity.h"

#undef CProjectedWallEntity

IMPLEMENT_AUTO_LIST( IProjectedWallEntityAutoList )

IMPLEMENT_CLIENTCLASS_DT( C_ProjectedWallEntity, DT_ProjectedWallEntity, CProjectedWallEntity )
	RecvPropFloat( RECVINFO( m_flLength ) ),
	RecvPropFloat( RECVINFO( m_flHeight ) ),
	RecvPropFloat( RECVINFO( m_flWidth ) ),
	RecvPropFloat( RECVINFO( m_flSegmentLength ) ),
	RecvPropFloat( RECVINFO( m_flParticleUpdateTime ) ),
	
	RecvPropBool( RECVINFO( m_bIsHorizontal ) ),
	RecvPropInt( RECVINFO( m_nNumSegments ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_ProjectedWallEntity )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( projected_wall_entity, C_ProjectedWallEntity )

C_ProjectedWallEntity::C_ProjectedWallEntity()
{
	m_hHitPortal = NULL;
	m_hSourcePortal = NULL;
	m_hChildSegment = NULL;
	m_hPlacementHelper = NULL;
}

void C_ProjectedWallEntity::Spawn( void )
{
	//C_BaseEntity::ThinkSet(&v1, this, (BASEPTR)349LL, 0.0, 0);

	// Skip BaseClass??
	C_BaseEntity::Spawn();
}

void C_ProjectedWallEntity::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

void C_ProjectedWallEntity::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );
}

void C_ProjectedWallEntity::PostDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PostDataUpdate( updateType );
}

void C_ProjectedWallEntity::OnProjected( void )
{
	BaseClass::OnProjected();
}

void C_ProjectedWallEntity::ProjectWall( void )
{
}

void C_ProjectedWallEntity::RestoreToToolRecordedState( KeyValues *pKV )
{

}

void C_ProjectedWallEntity::SetPaintPower( int nSegment, PaintPowerType power )
{

}

void C_ProjectedWallEntity::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}

bool C_ProjectedWallEntity::TestCollision( const Ray_t &ray, unsigned int mask, trace_t& trace )
{
	return BaseClass::TestCollision( ray, mask, trace );
}

bool C_ProjectedWallEntity::TestHitboxes( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr )
{
	return BaseClass::TestCollision( ray, fContentsMask, tr );
}

const QAngle &C_ProjectedWallEntity::GetRenderAngles( void )
{
	return vec3_angle;
}

void C_ProjectedWallEntity::ClientThink( void )
{
	BaseClass::ClientThink();
}

bool C_ProjectedWallEntity::InitMaterials( void )
{
	return false;
}

RenderableTranslucencyType_t C_ProjectedWallEntity::ComputeTranslucencyType( void )
{
	return RENDERABLE_IS_TRANSLUCENT;
}

int C_ProjectedWallEntity::DrawModel( int flags, const RenderableInstance_t &instance )
{
	return 0;
}

void C_ProjectedWallEntity::ComputeWorldSpaceSurroundingBox( Vector *mins, Vector *maxs )
{
	BaseClass::ComputeWorldSpaceSurroundingBox( mins, maxs );
}

void C_ProjectedWallEntity::GetRenderBounds( Vector& mins, Vector& maxs )
{
	BaseClass::GetRenderBounds( mins, maxs );
}

void C_ProjectedWallEntity::GetProjectionExtents( Vector &outmins, Vector& outmaxs )
{
	BaseClass::GetProjectionExtents( outmins, outmaxs );
}

bool C_ProjectedWallEntity::ShouldDraw( void )
{
	return BaseClass::ShouldDraw();
}

CollideType_t C_ProjectedWallEntity::GetCollideType( void )
{
	return BaseClass::GetCollideType();
}

void C_ProjectedWallEntity::GetToolRecordingState( KeyValues *msg )
{
	BaseClass::GetToolRecordingState( msg );
}