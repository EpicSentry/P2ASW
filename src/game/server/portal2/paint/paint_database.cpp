//========= Copyright (c) 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//

#include "cbase.h"

#include <functional>

#include "paint_database.h"
#include "paint/paintable_entity.h"
#include "world.h"
#include "isaverestore.h"

#ifdef PORTAL2
#include "weapon_paintgun.h"
#include "projectedwallentity.h"
#include "portal_player.h"
#include "prop_weightedcube.h"
#endif
#include "cegclientwrapper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_paint_surface_sphere_radius( "sv_paint_surface_sphere_radius", "60.f", FCVAR_DEVELOPMENTONLY, "Radius of query sphere to paint all intersecting surfaces." );
ConVar sv_erase_surface_sphere_radius( "sv_erase_surface_sphere_radius", "80.f", FCVAR_DEVELOPMENTONLY, "Radius of query sphere to erase all intersecting surfaces." );
ConVar sv_paint_alpha_coat("sv_paint_alpha_coat", "0.4f", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );


#define PAINTED_ENTITY_AMMO 1
#define PAINTED_WALL_SEGMENT_AMMO 1

#define VPROF_BUDGETGROUP_PAINT	_T("paint")

//extern ConVar sv_limit_paint;

CPaintDatabase PaintDatabase( "CPaintDatabase" );

CPaintDatabase::CPaintDatabase( char const *name ) : CAutoGameSystemPerFrame( name ), CGameEventListener()
{
	//ListenForGameEvent( "game_newmap" );
	ClearPaintData();
}


CPaintDatabase::~CPaintDatabase()
{
	ClearPaintData();
}

void CPaintDatabase::LevelInitPreEntity()
{
	ClearPaintData();
}

void CPaintDatabase::LevelInitPostEntity()
{
	m_bCanPaint = true;

	SendPaintDataToEngine();
}

void CPaintDatabase::LevelShutdownPostEntity()
{
	m_bCanPaint = false;

	// clean up after load
	m_PaintRestoreData.m_PaintmapData.Purge();
}


void CPaintDatabase::AddPaint( CBaseEntity* pPaintedEntity, const Vector& vecPaintLocation, const Vector& vecNormal, PaintPowerType powerType, float flPaintRadius, float flAlphaPercent )
{
	// If this is the world
	if( pPaintedEntity->IsBSPModel() )
	{
		Vector vPushedContactPoint = vecPaintLocation + 0.1f * vecNormal;

		CBrushEntityList brushEnum;
		UTIL_FindBrushEntitiesInSphere( brushEnum, vPushedContactPoint, flPaintRadius );

		PaintLocationData_t data;
		data.type = powerType;
		data.flPaintRadius = flPaintRadius;
		data.flPaintAlphaPercent = flAlphaPercent;
		data.location = vPushedContactPoint;

		for ( int i=0; i<brushEnum.m_BrushEntitiesToPaint.Count(); ++i )
		{
			data.hBrushEntity = brushEnum.m_BrushEntitiesToPaint[i];

			// Add this paint into the database
			m_PaintThisFrame.AddToTail( data );
		}

		// add world entity
		data.hBrushEntity = GetWorldEntity();
		m_PaintThisFrame.AddToTail( data );
	}
	// Not the world
	else if( !( pPaintedEntity->GetFlags() & FL_UNPAINTABLE ) ) //If this entity is not flagged as unpaintable
	{
#ifdef PORTAL2
		// If this hit a paintable projected wall
		CProjectedWallEntity *pPaintableWall = dynamic_cast< CProjectedWallEntity* >( pPaintedEntity );
		if( pPaintableWall )
		{
			PaintDatabase.PaintProjectedWall( pPaintableWall, powerType, vecPaintLocation );
		}
		// Not a wall
		else
#endif
		{
			PaintDatabase.PaintEntity( pPaintedEntity, powerType, vecPaintLocation );
		}
	}
}


void CPaintDatabase::PaintEntity( CBaseEntity *pPaintedEntity, PaintPowerType newPowerType, const Vector &vecPosition )
{
	//Get the current color of the painted entity
	IPaintableEntity* pPaintableEntity = dynamic_cast< IPaintableEntity* >( pPaintedEntity );
	PaintPowerType paintedPowerType = NO_POWER;
	if( pPaintableEntity )
	{
		paintedPowerType = pPaintableEntity->GetPaintPowerAtPoint( vecPosition );

		// Client player's need to handle getting painted
		if( pPaintedEntity->IsPlayer() )
		{
			CBroadcastRecipientFilter filter;
			filter.MakeReliable();
			UserMessageBegin( filter, "PaintEntity" );
			WRITE_EHANDLE( pPaintedEntity );
			WRITE_BYTE( newPowerType );
			WRITE_FLOAT( vecPosition.x );
			WRITE_FLOAT( vecPosition.y );
			WRITE_FLOAT( vecPosition.z );
			MessageEnd();
		}
	}
	else
	{
		const color24 otherColor = pPaintedEntity->GetRenderColor();
		const Color paintedColor( otherColor.r, otherColor.g, otherColor.b );
		paintedPowerType = MapColorToPower( paintedColor );
	}

	//Check if the entity is already painted
	const bool bPainted = paintedPowerType != NO_POWER;

	//If the entity is painted and it is now being erased
	if ( bPainted && newPowerType == NO_POWER )
	{
		RemovePaintedEntity( pPaintedEntity, true );
	}
	//Else if the entity is not painted and it is not being erased
	else if ( !bPainted && newPowerType != NO_POWER )
	{
		//Add the painted entity to the paint database if it isn't already in there
		if( FindPaintedEntity( pPaintedEntity ) == m_PaintedEntities.InvalidIndex() )
		{
			PaintEntityData_t data;
			data.hEnt = pPaintedEntity;
			//data->pPaintGun = pPaintGun;

			m_PaintedEntities.AddToTail( data );
		}
	}

	//Change the render color of the painted entity
	if( pPaintableEntity )
	{
		// This is a fix for objects falling asleep after restoring a map
		IPhysicsObject* pPhysics = pPaintedEntity->VPhysicsGetObject();
		if ( pPhysics && pPhysics->IsAsleep() )
		{
			pPhysics->Wake();
		}

		pPaintableEntity->Paint( newPowerType, vecPosition );
	}
	else
	{
		//Set the color of the power
		Color newColor = MapPowerToVisualColor( newPowerType );
		pPaintedEntity->SetRenderColor( newColor.r(), newColor.g(), newColor.b() );
	}
}

#ifdef PORTAL2
void CPaintDatabase::PaintProjectedWall( CProjectedWallEntity *pWall, PaintPowerType powerType, const Vector &vecPosition )
{
	const bool bWallPainted = pWall->IsWallPainted( vecPosition );

	pWall->Paint( powerType, vecPosition );

	if( !bWallPainted && powerType != NO_POWER )
	{
		//Add this wall to the internal list
		if( m_PaintedProjectedWalls.Find( pWall ) == m_PaintedProjectedWalls.InvalidIndex() )
		{
			m_PaintedProjectedWalls.AddToTail( pWall );
		}
	}
}
#endif


void CPaintDatabase::RemoveAllPaint()
{
	//Remove the paint from all the painted entities
	const int nEntityCount = m_PaintedEntities.Count();
	for( int i = 0; i < nEntityCount; ++i )
	{
		CBaseEntity *pEnt = ( m_PaintedEntities[i].hEnt.Get() != NULL ) ? EntityFromEntityHandle( m_PaintedEntities[i].hEnt.Get() ) : NULL;
		if ( pEnt )
		{
			RemovePaintedEntity( i, false);
		}
	}

#ifdef PORTAL2
	//Remove the paint from all the painted projected walls
	const int nPaintedProjectedWallCount = m_PaintedProjectedWalls.Count();
	for( int i = 0; i < nPaintedProjectedWallCount; ++i )
	{
		RemovePaintedWall( m_PaintedProjectedWalls[i], false );
	}
#endif

	ClearPaintData();

	// broadcast RemoveAllPaint message
	engine->RemoveAllPaint();
	CBroadcastRecipientFilter filter;
	filter.MakeReliable();
	UserMessageBegin( filter, "RemoveAllPaint" );
	MessageEnd();
}


struct PositionsAboutEqual_t
{
	const Vector& point;

	inline bool operator()( const PaintLocationData_t& element ) const
	{
		return VectorsAreEqual( element.location, point, EQUAL_EPSILON );
	}

	PositionsAboutEqual_t( const Vector& vPoint ) : point( vPoint ) {}
	PositionsAboutEqual_t( const PositionsAboutEqual_t& rhs ) : point( rhs.point ) {}

private:
	PositionsAboutEqual_t& operator=( const PositionsAboutEqual_t& );
};


void CPaintDatabase::RemovePaintedEntity( const CBaseEntity *pPaintedEntity )
{
	//int i = 0;
	const int index = FindPaintedEntity( pPaintedEntity );

	if( index != m_PaintedEntities.InvalidIndex() )
	{
		RemovePaintedEntity( index, true );
	}
}


void CPaintDatabase::RemovePaintedEntity( const CBaseEntity *pPaintedEntity, bool bDeleteData )
{
	const int index = FindPaintedEntity( pPaintedEntity );
	if( index != m_PaintedEntities.InvalidIndex() )
	{
		RemovePaintedEntity( index, bDeleteData );
	}
}


void CPaintDatabase::RemovePaintedEntity( int index, bool bDeleteData )
{
	CBaseEntity *pEnt = ( m_PaintedEntities[index].hEnt.Get() != NULL ) ? EntityFromEntityHandle( m_PaintedEntities[index].hEnt.Get() ) : NULL;
	if ( pEnt )
	{
		IPaintableEntity* pPaintableEntity = dynamic_cast< IPaintableEntity* >( pEnt );
		if( pPaintableEntity )
		{
			pPaintableEntity->CleansePaint();
		}
		else
		{
			pEnt->SetRenderColor( 255, 255, 255 );
		}
	}

	if( bDeleteData )
	{
		m_PaintedEntities.Remove( index );
	}
}

#ifdef PORTAL2
void CPaintDatabase::RemovePaintedWall( CProjectedWallEntity *pWall, bool bDeleteData )
{
	//Get the paint gun to give back the ammo to
	//CWeaponPaintGun *pPaintGun = dynamic_cast<CWeaponPaintGun*>( gEntList.FindEntityByClassname( NULL, "weapon_paintgun" ) );

	//if( pPaintGun )
	//{
	//	//Get the vectors for the wall
	//	Vector vecWallNormal, vecWallForward;
	//	AngleVectors( pWall->GetAbsAngles(), &vecWallForward, NULL, &vecWallNormal );

	//	vecWallForward.NormalizeInPlace();
	//	vecWallNormal.NormalizeInPlace();

	//	Vector vecPosition = pWall->GetAbsOrigin();

	//	//For all the segments of the wall
	//	for( int i = 0; i < pWall->GetNumSegments(); ++i )
	//	{
	//		//If this segment is painted
	//		if( pWall->IsWallPainted( i ) )
	//		{
	//			//Get the position of the segment
	//			vecPosition = pWall->GetAbsOrigin() + (( pWall->GetSegmentLength() * i ) * vecWallForward ) + ( vecWallForward * ( pWall->GetSegmentLength()/2.0f ) );

	//			//Paint this segment with NO_POWER
	//			PaintDatabase.Paint( pWall, NO_POWER, vecPosition, vecWallNormal /*, pPaintGun */);
	//		}
	//	}
	//}

	if( bDeleteData )
	{
		const int index = m_PaintedProjectedWalls.Find( pWall );
		if( index != m_PaintedProjectedWalls.InvalidIndex() )
		{
			m_PaintedProjectedWalls.Remove( index );
		}
	}
}
#endif

struct OutsideRadius_t
{
	const Vector& point;
	float radiusSq;

	inline bool operator()( const PaintLocationData_t& element ) const
	{
		return element.location.DistToSqr( point ) > radiusSq;
	}

	OutsideRadius_t( const Vector& vPoint, float radius )
		: point( vPoint ),
		  radiusSq( radius * radius )
	{
	}

	OutsideRadius_t( const OutsideRadius_t& rhs )
		: point( rhs.point ),
		  radiusSq( rhs.radiusSq )
	{
	}

private:
	OutsideRadius_t& operator=( const OutsideRadius_t& rhs );
};


int CPaintDatabase::FindPaintedEntity( const CBaseEntity *pPaintedEntity ) const
{
	const int nPaintCount = m_PaintedEntities.Count();
	for( int i = 0; i < nPaintCount; ++i )
	{
		CBaseEntity *pEnt = ( m_PaintedEntities[i].hEnt.Get() != NULL ) ? EntityFromEntityHandle( m_PaintedEntities[i].hEnt.Get() ) : NULL;
		if( pEnt == pPaintedEntity )
		{
			return i;
		}
	}

	return m_PaintedEntities.InvalidIndex();
}


void CPaintDatabase::FireGameEvent( IGameEvent *event )
{
	/*const char *type = event->GetName();

	if( Q_strcmp( type, "player_connect" ) == 0 )
	{
		DevMsg("paint database player connected\n");
	}
	else if ( Q_strcmp( type, "player_disconnect" ) == 0 )
	{
		DevMsg("paint database player disconnected\n");
	}*/
}


void CPaintDatabase::ClearPaintData()
{
	m_PaintThisFrame.RemoveAll();
	m_PaintedEntities.RemoveAll();
#ifdef PORTAL2
	m_PaintedProjectedWalls.RemoveAll();
#endif
}


inline int AscendingPaintLocationCompare( const PaintLocationData_t* a, const PaintLocationData_t* b )
{
	return ( a->hBrushEntity == b->hBrushEntity ) ? a->type - b->type : a->hBrushEntity.Get() - b->hBrushEntity.Get();
}


ConVar paint_location_distance_threshold_square("paint_location_distance_threshold_square", "25");
static void RemoveRepeatedPaintData( PaintLocationVector_t& dataList )
{
	const float DistThresholdSqr = paint_location_distance_threshold_square.GetFloat();
	for ( int i=0; i< dataList.Count(); ++i )
	{
		PaintLocationData_t data = dataList[i];
		for ( int j=i+1; j<dataList.Count(); )
		{
			if ( data.hBrushEntity == dataList[j].hBrushEntity && data.type == dataList[j].type && data.location.DistToSqr( dataList[j].location ) < DistThresholdSqr )
			{
				dataList.FastRemove( j );
			}
			else
			{
				++j;
			}
		}
	}
}


#define MAX_CHANGE_BOUNDS_SQR ( 256 * 256 * 3 )
class CChangedPaintBoundsCache
{
public:
	struct Bounds_t
	{
		Vector m_vecMins;
		Vector m_vecMaxs;
	};

	void AddChangedBounds( const Vector &vecOrigin, float flRadius )
	{
		if ( flRadius == 0.0f )
			return;

		Vector vecMins, vecMaxs;
		Vector vecDelta( flRadius, flRadius, flRadius );
		VectorSubtract( vecOrigin, vecDelta, vecMins );
		VectorAdd( vecOrigin, vecDelta, vecMaxs );

		Vector vecNewMins, vecNewMaxs;
		int nCount = m_ChangedBoxes.Count();
		for ( int i = 0; i < nCount; ++i )
		{
			Bounds_t &b = m_ChangedBoxes[i];
			vecNewMins = VectorMin( b.m_vecMins, vecMins );
			vecNewMaxs = VectorMax( b.m_vecMaxs, vecMaxs );
			if ( vecNewMins.DistToSqr( vecNewMaxs ) < MAX_CHANGE_BOUNDS_SQR )
			{
				b.m_vecMaxs = vecNewMaxs;
				b.m_vecMins = vecNewMins;
				return;
			}
		}

		int j = m_ChangedBoxes.AddToTail();
		m_ChangedBoxes[j].m_vecMins = vecMins;
		m_ChangedBoxes[j].m_vecMaxs = vecMaxs;
	}

	CUtlVectorFixedGrowable< Bounds_t, 16 > m_ChangedBoxes;
};

class CPaintAffectedEntityList : public IEntityEnumerator
{
public:
	virtual bool EnumEntity( IHandleEntity *pHandleEntity )
	{
		if ( !pHandleEntity )
			return true;

		CBaseEntity *pEntity = gEntList.GetBaseEntity( pHandleEntity->GetRefEHandle() );
		if ( !pEntity )
			return true;

		if ( ( pEntity->GetFlags() & FL_AFFECTED_BY_PAINT ) == 0 )
			return true;

		// This prevents dups from showing up. We'll add this back later
		pEntity->RemoveFlag( FL_AFFECTED_BY_PAINT );

		m_EntitiesToUpdate.AddToTail( pEntity );
		return true;
	}

	CUtlVectorFixedGrowable< CBaseEntity*, 32 > m_EntitiesToUpdate;
};


void CPaintDatabase::PreClientUpdate()
{
	VPROF_BUDGET( "CPaintDatabase::PreClientUpdate", VPROF_BUDGETGROUP_PAINT );
	if ( !m_bCanPaint )
	{
		return;
	}

	int count = m_PaintThisFrame.Count();

	if ( count > 1 )
	{
		RemoveRepeatedPaintData( m_PaintThisFrame );
		count = m_PaintThisFrame.Count();
	}

	// Paint server side
	CChangedPaintBoundsCache boundsCache;
	for ( int i=0; i<count; ++i )
	{
		const PaintLocationData_t& data = m_PaintThisFrame[i];

		CBaseEntity *pPaintBrush = ( data.hBrushEntity.Get() != NULL ) ? EntityFromEntityHandle( data.hBrushEntity.Get() ) : NULL;
		float flChangedPaintRadius = UTIL_PaintBrushEntity( pPaintBrush, data.location, data.type, data.flPaintRadius, data.flPaintAlphaPercent );
		boundsCache.AddChangedBounds( data.location, flChangedPaintRadius );
		//DevMsg("paint pos = %f %f %f\n", XYZ(data.location) );
	}

	// Find all entities in the changed regions that we must update
	CPaintAffectedEntityList paintEnum;
	for ( int i = 0; i < boundsCache.m_ChangedBoxes.Count(); ++i )
	{
		enginetrace->EnumerateEntities( boundsCache.m_ChangedBoxes[i].m_vecMins, boundsCache.m_ChangedBoxes[i].m_vecMaxs, &paintEnum );
	}

	for ( int i = 0; i < paintEnum.m_EntitiesToUpdate.Count(); ++i )
	{
		CBaseEntity *pEntity = paintEnum.m_EntitiesToUpdate[i];

		// Need to re-add the flag; we removed it earlier to avoid dups
		pEntity->AddFlag( FL_AFFECTED_BY_PAINT );

		// Wake up the object so its friction snapshots will be valid
		IPhysicsObject* pPhysObject = pEntity->VPhysicsGetObject();
		if( pPhysObject )
		{
			pPhysObject->Wake();
		}
#ifdef PORTAL2
		if( FClassnameIs( pEntity, "prop_weighted_cube" ) )
		{
			CPropWeightedCube* pCube = static_cast<CPropWeightedCube*>(pEntity);
			pCube->ExitDisabledState();
		}
#endif

		pEntity->UpdatePaintPowersFromContacts();
	}


	// Sort the location data for this frame by paint type to
	// bundle all the messages for the same type together
	m_PaintThisFrame.Sort( &AscendingPaintLocationCompare );

	// Create the message filter
	CRecipientFilter filter;
	for ( int i=1; i<=gpGlobals->maxClients; ++i )
	{
		CPortal_Player *pPortalPlayer = ToPortalPlayer( UTIL_PlayerByIndex( i ) );
		// FIXME: OnFullyConnected() for CPortal_Player never gets called!!
		if ( pPortalPlayer /*&& pPortalPlayer->IsFullyConnected()*/ )
		{
			filter.AddRecipient( pPortalPlayer );
		}
	}

	filter.MakeReliable();

	PaintLocationConstIter cbegin = m_PaintThisFrame.Base();
	PaintLocationConstIter cend = cbegin + m_PaintThisFrame.Count();

	// For each unsent paint location
	for( PaintLocationConstIter currentLocation = cbegin; currentLocation != cend; )
	{
		// Begin the message
		UserMessageBegin( filter, "PaintWorld" );

		// Write the type
		PaintPowerType powerType = currentLocation->type;
		WRITE_BYTE( powerType );

		CBaseEntity *pBrushEntity = ( currentLocation->hBrushEntity.Get() != NULL ) ? EntityFromEntityHandle( currentLocation->hBrushEntity.Get() ) : NULL;
		WRITE_EHANDLE( pBrushEntity );

		WRITE_FLOAT( currentLocation->flPaintRadius );
		WRITE_FLOAT( currentLocation->flPaintAlphaPercent );

		// Compute the number of locations with the same type to send in this message
		const int SIZEOF_FLOAT_PAINT_POSITION = 3 * sizeof( float );	// Typical 3-float position vector
		const int SIZEOF_SHORT_PAINT_POSITION = 3 * sizeof( short );	// Short vector used for offsets
		const int SIZEOF_FLOAT_PAINT_RADIUS_AND_ALPHA = 2 * sizeof( float ); // paint sphere radius and alpha coat
		const int SIZEOF_COLOR_INDEX_AND_LOCATION_COUNT = 2 * sizeof( uint8 ); // Color index and number of locations
		const int SIZEOF_BRUSH_HANDLE = sizeof( long );
		const int SIZEOF_PAINT_ENTITY_MSG_HEADER = SIZEOF_FLOAT_PAINT_RADIUS_AND_ALPHA + SIZEOF_COLOR_INDEX_AND_LOCATION_COUNT + SIZEOF_BRUSH_HANDLE; 
		const int SIZE_OF_PAINT_ENTITY_MSG_BODY = MAX_USER_MSG_DATA - SIZEOF_PAINT_ENTITY_MSG_HEADER;

		PaintLocationConstIter firstDifferent = currentLocation;
		for( ; firstDifferent != cend; ++firstDifferent )
		{
			if( firstDifferent->hBrushEntity != pBrushEntity || firstDifferent->type != powerType )
				break;
		}
	
		const int paintLocationCount = MIN( firstDifferent - currentLocation, (SIZE_OF_PAINT_ENTITY_MSG_BODY - SIZEOF_FLOAT_PAINT_POSITION) / SIZEOF_SHORT_PAINT_POSITION );

		// Write the number of locations in this message
		// Note: User message data size is capped at 255, so a byte is sufficient.
		WRITE_BYTE( paintLocationCount );

		// Compute the center point of all the locations in this message
		Vector center( 0, 0, 0 );
		for( int j = 0; j < paintLocationCount; ++j )
		{
			center += currentLocation[j].location;
		}

		center *= 1.0f / paintLocationCount;

		// Write the center point
		WRITE_FLOAT( center.x );
		WRITE_FLOAT( center.y );
		WRITE_FLOAT( center.z );

		// Write the offset of each paint location from the center
		for( int j = 0; j < paintLocationCount; ++currentLocation, ++j )
		{
			const Vector offset = currentLocation->location - center;

			// Write the offset
			WRITE_SHORT( static_cast< short >( offset.x ) );
			WRITE_SHORT( static_cast< short >( offset.y ) );
			WRITE_SHORT( static_cast< short >( offset.z ) );
		}

		// End the message
		MessageEnd();
	}

	// Clear the paint locations for this frame
	m_PaintThisFrame.RemoveAll();
}


void CC_RemoveAllPaint()
{
	PaintDatabase.RemoveAllPaint();
}

static ConCommand removeallpaint("removeallpaint", CC_RemoveAllPaint );


CON_COMMAND_F( cast_ray_paint, "Test paint", FCVAR_CHEAT )
{
	CBasePlayer *pPlayer = UTIL_GetCommandClient();

	Vector forward;
	trace_t tr;

	pPlayer->EyeVectors( &forward );
	Vector start = pPlayer->EyePosition();
	UTIL_TraceLine(start, start + forward * MAX_COORD_RANGE, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );

	if ( tr.DidHit() )
	{
		DevMsg("paint ent = %s\n", tr.m_pEnt->GetClassname() );

		PaintDatabase.AddPaint( tr.m_pEnt, tr.endpos, tr.plane.normal, SPEED_POWER, 60.f, 1.f );
	}
}

void CPaintDatabase::SavePaintmapData( ISave* pSave )
{
	if ( !HASPAINTMAP )
		return;

	CUtlVector< uint32 > data;
	//engine->GetPaintmapDataRLE( data );

	pSave->StartBlock();
	{
		int count = data.Count();
		pSave->WriteInt( &count );
		pSave->WriteData( (const char *)data.Base(), count * sizeof(data.Base()[0]) );
	}
	pSave->EndBlock();
}


void CPaintDatabase::RestorePaintmapData( IRestore* pRestore )
{
	if ( !HASPAINTMAP )
		return;

	pRestore->StartBlock();
	{
		int count = pRestore->ReadInt();
		m_PaintRestoreData.m_PaintmapData.SetCount( count );
		pRestore->ReadData( (char *)m_PaintRestoreData.m_PaintmapData.Base(), count * sizeof(m_PaintRestoreData.m_PaintmapData.Base()[0]), 0 );
	}
	pRestore->EndBlock();
}


void CPaintDatabase::SendPaintDataTo( CBasePlayer* pPlayer )
{
	if ( !HASPAINTMAP )
		return;

	if ( pPlayer->IsConnected() )
	{
		//engine->SendPaintmapDataToClient( pPlayer->edict() );
	}
}


// this is for save/restore
void CPaintDatabase::SendPaintDataToEngine()
{
	if ( !HASPAINTMAP )
		return;

	if ( m_PaintRestoreData.m_PaintmapData.Count() > 0 )
	{
		//engine->LoadPaintmapDataRLE( m_PaintRestoreData.m_PaintmapData );
	}

	// clean up after load
	m_PaintRestoreData.m_PaintmapData.Purge();
}


#ifndef TI_REL
void CC_PaintAllSurfaces( const CCommand& args )
{
	PaintPowerType power = SPEED_POWER;

	if ( args.ArgC() == 2 )
	{
		power = static_cast< PaintPowerType >( atoi( args[1] ) );
	}

	//engine->PaintAllSurfaces( power );

	CBroadcastRecipientFilter filter;
	filter.MakeReliable();

	UserMessageBegin( filter, "PaintAllSurfaces" );

	WRITE_BYTE( power );

	MessageEnd();
}

static ConCommand paintallsurfaces( "paintallsurfaces", CC_PaintAllSurfaces );
#endif //!TI_REL
