#include "cbase.h"
#include "projectedwallentity.h"
#include "paint/paint_database.h"
#include "vcollide_parse.h"
#include "portal_player.h"
#include "physicsshadowclone.h"
#include "mathlib/polyhedron.h"

#ifndef NO_PROJECTED_WALL

int CProjectedWallEntity::s_HardLightBridgeSurfaceProps = -1;

extern ConVar sv_thinnerprojectedwalls;

IMPLEMENT_AUTO_LIST( IProjectedWallEntityAutoList )

BEGIN_DATADESC( CProjectedWallEntity )

	DEFINE_FIELD( m_vWorldSpace_WallMins, FIELD_VECTOR ),
	DEFINE_FIELD( m_vWorldSpace_WallMaxs, FIELD_VECTOR ),

	DEFINE_FIELD( m_hColorPortal, FIELD_EHANDLE ),
	
	DEFINE_FIELD( m_flLength, FIELD_FLOAT ),
	DEFINE_FIELD( m_flHeight, FIELD_FLOAT ),
	DEFINE_FIELD( m_flWidth, FIELD_FLOAT ),
	DEFINE_FIELD( m_flSegmentLength, FIELD_FLOAT ),
	DEFINE_FIELD( m_flParticleUpdateTime, FIELD_FLOAT ),
	
	DEFINE_FIELD( m_bIsHorizontal, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_nNumSegments, FIELD_INTEGER ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CProjectedWallEntity, DT_ProjectedWallEntity )
	SendPropFloat( SENDINFO( m_flLength ) ),
	SendPropFloat( SENDINFO( m_flHeight ) ),
	SendPropFloat( SENDINFO( m_flWidth ) ),
	SendPropFloat( SENDINFO( m_flSegmentLength ) ),
	SendPropFloat( SENDINFO( m_flParticleUpdateTime ) ),
	
	SendPropBool( SENDINFO( m_bIsHorizontal ) ),
	SendPropInt( SENDINFO( m_nNumSegments ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( projected_wall_entity, CProjectedWallEntity )

CProjectedWallEntity::CProjectedWallEntity()
{

}

CProjectedWallEntity::~CProjectedWallEntity()
{
	CleanupWall();
	PaintDatabase.RemovePaintedWall( this );
}

void CProjectedWallEntity::Spawn( void )
{
	BaseClass::Spawn();
	Precache();
	CollisionProp()->SetSolid( SOLID_CUSTOM );
	CollisionProp()->SetSolidFlags( 67 );
	CollisionProp()->SetSurroundingBoundsType( USE_GAME_CODE );
	SetMoveType(MOVETYPE_NONE, MOVECOLLIDE_DEFAULT );
	CreateVPhysics();
	SetTransmitState( 8 );
	if (CProjectedWallEntity::s_HardLightBridgeSurfaceProps == -1)
		CProjectedWallEntity::s_HardLightBridgeSurfaceProps = physprops->GetSurfaceIndex("hard_light_bridge");
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

void CProjectedWallEntity::UpdateOnRemove( void )
{
	CheckForPlayersOnBridge();
	CheckForSettledReflectorCubes();
	CleanupWall();
	BaseClass::UpdateOnRemove();
}

void CProjectedWallEntity::NotifyPortalEvent( PortalEvent_t nEventType, CPortal_Base2D *pNotifier )
{
	if (nEventType == PORTALEVENT_LINKED)
	{
		pNotifier->RemovePortalEventListener( this );
	}
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

void CProjectedWallEntity::GetProjectionExtents( Vector &outMins, Vector &outMaxs )
{
	GetExtents( outMins, outMaxs, 0.5 );
}

bool CProjectedWallEntity::ShouldSavePhysics( void )
{
	return false;
}

bool CProjectedWallEntity::CreateVPhysics( void )
{
	ProjectWall();
	return true;
}

void CProjectedWallEntity::ProjectWall( void )
{
	float x; // xmm0_4
	float y; // xmm2_4
	float z; // xmm4_4
	float v10; // xmm1_4
	bool v11; // zf
	float v12; // xmm3_4
	float v13; // xmm5_4
	float v18; // xmm5_4
	float v19; // xmm4_4
	float v20; // xmm3_4
	float v22; // xmm4_4
	float v24; // xmm3_4
	solid_t solid; // [esp+64h] [ebp-724h] BYREF
	Vector vSetMaxs; // [esp+6A4h] [ebp-E4h] BYREF
	float v37; // [esp+6B0h] [ebp-D8h] BYREF
	Vector v38; // [esp+6B4h] [ebp-D4h] BYREF
	float v39; // [esp+6C0h] [ebp-C8h]
	Vector v40; // [esp+6C4h] [ebp-C4h] BYREF
	float v41; // [esp+6D0h] [ebp-B8h]
	float v45; // [esp+6E0h] [ebp-A8h]
	Vector vSetMins; // [esp+704h] [ebp-84h] BYREF
	Vector vecForward; // [esp+714h] [ebp-74h] BYREF
	Vector vecRight; // [esp+720h] [ebp-68h] BYREF
	Vector vecUp; // [esp+72Ch] [ebp-5Ch] BYREF
	Vector vMins; // [esp+738h] [ebp-50h] BYREF
	Vector vMaxs; // [esp+744h] [ebp-44h] BYREF
	Vector vUp; // [esp+750h] [ebp-38h] BYREF
	Vector vRight; // [esp+75Ch] [ebp-2Ch] BYREF
	CPhysConvex *pTempConvex; // [esp+768h] [ebp-20h] BYREF

	CleanupWall();
	AddEffects( 8 );
	CheckForPlayersOnBridge();

	const Vector vStartPoint = GetStartPoint();
	const float flStartPointX = vStartPoint.x;
	const float flStartPointY = vStartPoint.y;
	const float flStartPointZ = vStartPoint.z;
	const Vector vEndPoint = GetEndPoint();
	const float flEndPointX = vEndPoint.x;
	const float flEndPointY = vEndPoint.y;
	const float flEndPointZ = vEndPoint.z;
	GetVectors( &vecForward, &vecRight, &vecUp );

	Vector *vVerts[4];

	if (sv_thinnerprojectedwalls.GetInt())
	{
		vVerts[3] = &v40;
		vVerts[2] = &v38;
		vVerts[1] = &vSetMins;
		vVerts[0] = &vSetMaxs;
		vSetMaxs.x = flStartPointX + (vecRight.x * 32.0);
		vSetMaxs.y = flStartPointY + (vecRight.y * 32.0);
		vSetMaxs.z = flStartPointZ + (vecRight.z * 32.0);
		v37 = flStartPointX - (vecRight.x * 32.0);
		v38.x = flStartPointY - (vecRight.y * 32.0);
		v38.z = flEndPointX - (vecRight.x * 32.0);
		v38.y = flStartPointZ - (32.0 * vecRight.z);
		v40.y = (vecRight.x * 32.0) + flEndPointX;
		v39 = flEndPointY - (vecRight.y * 32.0);
		v40.z = (vecRight.y * 32.0) + flEndPointY;
		v40.x = flEndPointZ - (32.0 * vecRight.z);
		v41 = (32.0 * vecRight.z) + flEndPointZ;
		
		pTempConvex = physcollision->ConvexFromVerts( vVerts, 4 );
	LABEL_3:
		m_pWallCollideable = physcollision->ConvertConvexToCollide( &pTempConvex, 1 );
		if (m_pWallCollideable)
		{
			V_strncpy(solid.surfaceprop, "hard_light_bridge", 512);
			solid.params.massCenterOverride = g_PhysDefaultObjectParams.massCenterOverride;
			solid.params.pGameData = this;
			solid.params.mass = g_PhysDefaultObjectParams.mass;
			solid.params.inertia = g_PhysDefaultObjectParams.inertia;
			solid.params.damping = g_PhysDefaultObjectParams.damping;
			solid.params.rotdamping = g_PhysDefaultObjectParams.rotdamping;
			solid.params.rotInertiaLimit = g_PhysDefaultObjectParams.rotInertiaLimit;
			solid.params.pName = g_PhysDefaultObjectParams.pName;
			solid.params.volume = g_PhysDefaultObjectParams.volume;
			solid.params.dragCoefficient = g_PhysDefaultObjectParams.dragCoefficient;
			// Swarm: FIXME!!
			//*(_DWORD *)&solid.params.enableCollisions = *(_DWORD *)&g_PhysDefaultObjectParams.enableCollisions;
			IPhysicsObject *v5 = PhysModelCreateCustom( this, m_pWallCollideable, vec3_origin, vec3_angle, "hard_light_bridge", true, &solid );
			if (v5)
			{
				if ( VPhysicsGetObject() )
					VPhysicsDestroyObject();
				VPhysicsSetObject( v5 );
				v5->RecheckContactPoints();
				if ( v5->GetCollide() )
				{
					vMaxs = vec3_origin;
					vMins = vec3_origin;
					physcollision->CollideGetAABB(&vMins, &vMaxs, v5->GetCollide(), vec3_origin, vec3_angle);
					x = vMins.x;

					if (vMins.x == m_vWorldSpace_WallMins.m_Value.x
						&& (y = vMins.y, vMins.y == m_vWorldSpace_WallMins.m_Value.y)
						&& (z = vMins.z, vMins.z == m_vWorldSpace_WallMins.m_Value.z))
					{
						v10 = vMaxs.x;
						v11 = vMaxs.x == m_vWorldSpace_WallMaxs.m_Value.x;
					}
					else
					{
						NetworkStateChanged( &m_vWorldSpace_WallMins );
						v10 = vMaxs.x;
						x = vMins.x;
						v11 = vMaxs.x == m_vWorldSpace_WallMaxs.m_Value.x;
						y = vMins.y;
						m_vWorldSpace_WallMins.m_Value.x = vMins.x;
						z = vMins.z;
						m_vWorldSpace_WallMins.m_Value.y = y;
						m_vWorldSpace_WallMins.m_Value.z = z;
					}

					if (!v11
						|| (v12 = vMaxs.y, vMaxs.y != m_vWorldSpace_WallMaxs.m_Value.y)
						|| (v13 = vMaxs.z, vMaxs.z != m_vWorldSpace_WallMaxs.m_Value.z))
					{
						CBaseEntity::NetworkStateChanged( &m_vWorldSpace_WallMaxs);
						v10 = vMaxs.x;
						v12 = vMaxs.y;
						v13 = vMaxs.z;
						m_vWorldSpace_WallMaxs.m_Value.x = vMaxs.x;
						m_vWorldSpace_WallMaxs.m_Value.y = v12;
						m_vWorldSpace_WallMaxs.m_Value.z = v13;
						x = vMins.x;
						y = vMins.y;
						z = vMins.z;
					}
					vSetMins.x = x - flStartPointX;
					vSetMins.y = y - flStartPointY;
					vSetMaxs.x = v10 - flStartPointX;
					vSetMins.z = z - flStartPointZ;
					vSetMaxs.y = v12 - flStartPointY;
					vSetMaxs.z = v13 - flStartPointZ;
					SetSize( vSetMins, vSetMaxs );
					float v14 = sqrt(
						(((flStartPointX - flEndPointX) * (flStartPointX - flEndPointX))
						+ ((flStartPointY - flEndPointY) * (flStartPointY - flEndPointY)))
						+ ((flStartPointZ - flEndPointZ) * (flStartPointZ - flEndPointZ)));
					m_flLength = v14;
					m_flWidth = 64.0;
					m_flHeight = 0.015625;

					CollisionProp()->MarkSurroundingBoundsDirty();
					CollisionProp()->MarkPartitionHandleDirty();
					CollisionProp()->UpdatePartition();
					AngleVectors( GetAbsAngles(), NULL, &vRight, &vUp);
					m_bIsHorizontal = (vUp.z > STEEP_SLOPE || vUp.z < -STEEP_SLOPE) && vRight.z > -STEEP_SLOPE && vRight.z < STEEP_SLOPE;
					DisplaceObstructingEntities();
					m_nNumSegments = ceil( ( m_flLength / m_flSegmentLength ) );
					//m_PaintPowers.SetCount( ceil( ( m_flLength / m_flSegmentLength ) ) );
					CleansePaint();
				}
			}
		}
		return;
	}
	v38 = vecForward;

	// This is dumb, stupid, and dumb!!!
	//vSetMaxs.x = ((int)vecForward.x ^ 0x80000000);
	//vSetMaxs.y = ((int)vecForward.y ^ 0x80000000);
	//vSetMaxs.z = ((int)vecForward.z ^ 0x80000000);
	v40 = vecRight;
	v39 = ((vecForward.x * flEndPointX) + (vecForward.y * flEndPointY)) + (vecForward.z * flEndPointZ);

	v37 = (vecForward.x * flStartPointX)
		+ (vecForward.y * flStartPointY)
		+ (vecForward.z * flStartPointZ);
	v18 = (vecRight.x * 64.0) * 0.5;
	v19 = (vecRight.y * 64.0) * 0.5;
	v20 = (64.0 * vecRight.z) * 0.5;

	v41 = (((flStartPointX + v18) * vecRight.x) + ((flStartPointY + v19) * vecRight.y))
		+ ((flStartPointZ + v20) * vecRight.z);

	float v21 = ((flStartPointX - v18) * (vecRight.x))
		+ ((flStartPointY - v19) * (vecRight.y));

	v45 = v21 + ((flStartPointZ - v20) * (vecRight.z));
	float v23 = (vecUp.x * 0.015625) * 0.5;
	v22 = (vecUp.y * 0.015625) * 0.5;
	v24 = (0.015625 * vecUp.z) * 0.5;

	float v47 = (((flStartPointX + v23) * vecUp.x) + ((flStartPointY + v22) * vecUp.y))
		+ ((flStartPointZ + v24) * vecUp.z);

	float v51 = (((flStartPointX - v23) * -vecUp.x)
		+ ((flStartPointY - v22) * -vecUp.y))
		+ ((flStartPointZ - v24) * -vecUp.z);

	float fPlanes[6 * 4];

	// Forward plane
	fPlanes[(0 * 4) + 0] = vecForward.x;
	fPlanes[(0 * 4) + 1] = vecForward.y;
	fPlanes[(0 * 4) + 2] = vecForward.z;
	fPlanes[(0 * 4) + 3] = v37 + m_flLength;

	// Back plane
	fPlanes[(1 * 4) + 0] = -vecForward.x;
	fPlanes[(1 * 4) + 1] = -vecForward.y;
	fPlanes[(1 * 4) + 2] = -vecForward.z;
	fPlanes[(1 * 4) + 3] = v39 + m_flLength;

	// Up plane
	fPlanes[(2 * 4) + 0] = vecUp.x;
	fPlanes[(2 * 4) + 1] = vecUp.y;
	fPlanes[(2 * 4) + 2] = vecUp.z;
	fPlanes[(2 * 4) + 3] = v47 + m_flHeight;

	// Down plane
	fPlanes[(3 * 4) + 0] = -vecUp.x;
	fPlanes[(3 * 4) + 1] = -vecUp.y;
	fPlanes[(3 * 4) + 2] = -vecUp.z;
	fPlanes[(3 * 4) + 3] = v51 + m_flHeight;
	
	// Right plane
	fPlanes[(4 * 4) + 0] = vecRight.x;
	fPlanes[(4 * 4) + 1] = vecRight.y;
	fPlanes[(4 * 4) + 2] = vecRight.z;
	fPlanes[(4 * 4) + 3] = v41 + m_flWidth;

	// Left plane
	fPlanes[(5 * 4) + 0] = -vecRight.x;
	fPlanes[(5 * 4) + 1] = -vecRight.y;
	fPlanes[(5 * 4) + 2] = -vecRight.z;
	fPlanes[(5 * 4) + 3] = v45 + m_flWidth;

	CPolyhedron *pPolyhedron = GeneratePolyhedronFromPlanes( fPlanes, 6, 0.0 );
	if (!pPolyhedron)
	{
		Warning( "CProjectedWallEntity: GeneratePolyhedronFromPlanes failed! Get a save game for me!.\n" );
		return;
	}
	pTempConvex = physcollision->ConvexFromConvexPolyhedron( *pPolyhedron );
	pPolyhedron->Release();
	if (pTempConvex)
		goto LABEL_3;
}

void CProjectedWallEntity::CheckForPlayersOnBridge( void )
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CPortal_Player *pPlayer = (CPortal_Player *)UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->GetGroundEntity() == this)
		{
			if ( pPlayer->m_Shared.InCond( 0 ) )
				pPlayer->WasDroppedByOtherPlayerWhileTaunting();
			SetGroundEntity( NULL );
			pPlayer->BridgeRemovedFromUnder();
		}
	}
}

bool CProjectedWallEntity::TestCollision( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr )
{
	if ( !m_pWallCollideable )
		return false;

	physcollision->TraceBox( ray, fContentsMask, 0, m_pWallCollideable, vec3_origin, vec3_angle, &tr );
	
	tr.surface.name = "hard_light_bridge";
	tr.surface.flags = 0;
	tr.surface.surfaceProps = CProjectedWallEntity::s_HardLightBridgeSurfaceProps;
	if ( !(tr.fraction >= 1.0) || tr.allsolid || tr.startsolid )
		return true;

	return false;
}

int CProjectedWallEntity::ObjectCaps( void )
{	
	return BaseClass::ObjectCaps() & 1;
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

void CProjectedWallEntity::CleanupWall( void )
{
	if ( VPhysicsGetObject() )
	{
		CPhysicsShadowClone::NotifyDestroy( VPhysicsGetObject(), this );
		VPhysicsDestroyObject();
	}
	if (m_pWallCollideable)
	{
		CPhysicsShadowClone::NotifyDestroy( m_pWallCollideable, this );
#if 0 // Doesn't exist in Swarm
		physenv->DestroyCollideOnDeadObjectFlush( m_pWallCollideable );
#else
		physcollision->DestroyCollide( m_pWallCollideable );
#endif
		m_pWallCollideable = NULL;
	}

	m_vWorldSpace_WallMins = m_vWorldSpace_WallMaxs = vec3_origin;
	m_flHeight = m_flWidth = m_flLength = 0.0;
	m_hHitPortal = NULL;
}

float CProjectedWallEntity::GetSegmentLength( void )
{
	return m_flSegmentLength;
}

int CProjectedWallEntity::GetNumSegments( void )
{
	return m_nNumSegments;
}

bool CProjectedWallEntity::IsWallPainted( const Vector &vecPosition )
{
	return CProjectedWallEntity::GetPaintPowerAtPoint( vecPosition ) != NO_POWER;
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

#ifndef PROJECTED_WALL_EVENT_SERVERONLY
IMPLEMENT_SERVERCLASS_ST( CTEWallPaintedEvent, DT_TEWallPaintedEvent )

	SendPropEHandle( SENDINFO( m_hEntity ) ),
	SendPropInt( SENDINFO( m_colorIndex ) ),
	SendPropInt( SENDINFO( m_nSegment ) ),

END_SEND_TABLE();
#endif

CTEWallPaintedEvent::~CTEWallPaintedEvent()
{
	m_hEntity = NULL;
}


CTEWallPaintedEvent g_TEWallPaintedEvent("WallPaintedEvent");

#endif // NO_PROJECTED_WALL