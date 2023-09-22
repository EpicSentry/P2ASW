//========= Copyright (c) Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//============================================================================//

#include "cbase.h"
#include <algorithm>
#include <functional>
#include "paint_blobs_shared.h"
#include "debugoverlay_shared.h"

#include "paint_cleanser_manager.h"
#include "fmtstr.h"
#include "paintable_entity.h"
#include "vprof.h"
#include "datacache/imdlcache.h"
#include "raytrace.h"
#include "mathlib/ssequaternion.h"

// define this when we want to prefetch blob data in PaintBlobUpdate()
//#define BLOB_PREFETCH

// define this to debug blob SIMD update
//#define BLOB_SIMD_DEBUG
#ifdef BLOB_SIMD_DEBUG
#define BLOB_IN_BEAM_ERROR 1.f
#endif

#ifdef BLOB_PREFETCH
#include "cache_hints.h"
#endif

#ifdef CLIENT_DLL
#include "paint/c_trigger_paint_cleanser.h"
#include "paint/c_paintblob.h"
#include "c_world.h"

const Color g_BlobDebugColor( 0, 255, 255 );

ConVar debug_paint_client_blobs( "debug_paint_client_blobs", "0" );

#else

#include "paint/trigger_paint_cleanser.h"
#include "paint/cpaintblob.h"
#include "world.h"

ConVar debug_paint_server_blobs( "debug_paint_server_blobs", "0", FCVAR_DEVELOPMENTONLY );
ConVar debug_paintblobs_streaking( "debug_paintblobs_streaking", "0", FCVAR_DEVELOPMENTONLY );

extern ConVar phys_pushscale;

const Color g_BlobDebugColor( 255, 0, 255 );
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VPROF_BUDGETGROUP_PAINTBLOB	_T("Paintblob")

ConVar paintblob_collision_box_size("paintblob_collision_box_size", "60.f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);

ConVar paintblob_gravity_scale( "paintblob_gravity_scale", "1.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "The gravity scale of the paint blobs." );
ConVar paintblob_air_drag( "paintblob_air_drag", "0.1f", FCVAR_REPLICATED | FCVAR_CHEAT, "The air drag applied to the paint blobs." );

// Blobulator radius scale
ConVar paintblob_min_radius_scale("paintblob_min_radius_scale", "0.7f", FCVAR_REPLICATED | FCVAR_CHEAT );
ConVar paintblob_max_radius_scale("paintblob_max_radius_scale", "1.0f", FCVAR_REPLICATED | FCVAR_CHEAT );

// streak
ConVar paintblob_radius_while_streaking( "paintblob_radius_while_streaking", "0.3f", FCVAR_REPLICATED | FCVAR_CHEAT );
ConVar paintblob_streak_angle_threshold( "paintblob_streak_angle_threshold", "45.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "The angle of impact below which the paint blobs will streak paint." );
ConVar paintblob_streak_trace_range( "paintblob_streak_trace_range", "20.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "The range of the trace for the paint blobs while streaking." );
ConVar paintblob_streak_particles_enabled("paintblob_streak_particles_enabled", "0", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT | FCVAR_REPLICATED );

//Limited range for blobs
ConVar paintblob_limited_range( "paintblob_limited_range", "0", FCVAR_REPLICATED | FCVAR_CHEAT, "If the paintblobs have a limited range." );
ConVar paintblob_lifetime( "paintblob_lifetime", "1.5f", FCVAR_REPLICATED | FCVAR_CHEAT, "The lifetime of the paintblobs if they have a limited range." );

#ifdef _X360
ConVar paintblob_update_per_second( "paintblob_update_per_second", "30.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "The number of times the blobs movement code is run per second." );
#else
ConVar paintblob_update_per_second( "paintblob_update_per_second", "60.0f", FCVAR_REPLICATED | FCVAR_CHEAT, "The number of times the blobs movement code is run per second." );
#endif

ConVar debug_beam_badsection( "debug_beam_badsection", "0", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

const int BLOB_TRACE_STATIC_MASK = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_HITBOX | CONTENTS_DEBRIS | CONTENTS_WATER | CONTENTS_SLIME;
const int BLOB_TRACE_DYNAMIC_MASK = CONTENTS_SOLID | CONTENTS_HITBOX | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_DEBRIS;

const int MAX_BLOB_TRACE_ENTITY_RESULTS = 64;

extern ConVar sv_gravity;
extern ConVar player_can_use_painted_power;
extern ConVar player_paint_effects_enabled;


ConVar paintblob_beam_radius_offset("paintblob_beam_radius_offset", "15.f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
float UTil_Blob_BeamRadiusOffset( float flBeamRadius )
{
	float flOutput = flBeamRadius - paintblob_beam_radius_offset.GetFloat();
	Assert( flOutput > 0.f );
	return ( flOutput > 0.f ) ? flOutput : 1.f;
}

float UTil_Blob_TBeamLinearForce( float flLinearForce )
{
	// motion controller of the beam moves other entities half speed of the blobs
	return 0.5f * fabs( flLinearForce );
}

CBasePaintBlob::CBasePaintBlob() : m_flDestVortexRadius( 0.f ),
									m_flCurrentVortexRadius( 0.f ),
									m_flCurrentVortexSpeed( 0.f ),
									m_flVortexDirection(1.f), // -1.f or 1.f

									// positions & velocities
									m_vecTempEndPosition( vec3_origin ),
									m_vecTempEndVelocity( vec3_origin ),
									m_vecPosition( vec3_origin ),
									m_vecPrevPosition( vec3_origin ),
									m_vecVelocity( vec3_origin ),

									// normal for particles effect
									m_vContactNormal( vec3_origin ),

									m_paintType( NO_POWER ),
									m_hOwner( NULL ),
									m_MoveState( PAINT_BLOB_AIR_MOVE ),

									// life time
									m_flLifeTime( 0.f ),
									
									//Streaking
									m_vecStreakDir( vec3_origin ),
									m_bStreakDirChanged( false ),
									m_flStreakTimer( 0.f ),
									m_flStreakSpeedDampenRate( 0.f ),
									
					
									m_bDeleteFlag( false ),
									
									// update time
									m_flAccumulatedTime( 0.0 ),
									m_flLastUpdateTime( 0.0 ),
									
									m_flRadiusScale( 0.f ),
									
									m_bShouldPlayEffect( false ),
									
									m_bSilent( false ),
									
									// optimize trace
									m_vCollisionBoxCenter( vec3_origin ),
									m_bCollisionBoxHitSolid( false ),

									m_bDrawOnly( false ),
									
									// num teleported
									m_bTeleportedThisFrame( false ),
									m_nTeleportationCount( 0 )
{
}


CBasePaintBlob::~CBasePaintBlob()
{
}


void CBasePaintBlob::Init( const Vector &vecOrigin, const Vector &vecVelocity, int paintType, float flMaxStreakTime, float flStreakSpeedDampenRate, CBaseEntity* pOwner, bool bSilent, bool bDrawOnly )
{
	m_vecPosition = vecOrigin;
	m_vecPrevPosition = vecOrigin;
	SetVelocity( vecVelocity );
	m_paintType = static_cast<PaintPowerType>( paintType );

	//Set up the streaking properties of the blob
	m_flStreakTimer = flMaxStreakTime;
	m_flStreakSpeedDampenRate = flStreakSpeedDampenRate;
	m_vecStreakDir = vec3_origin;

	//Set the default move state for the blob
	m_MoveState = PAINT_BLOB_AIR_MOVE;

	m_flVortexDirection = Sign( RandomFloat(-1.f, 1.f) );
	m_flDestVortexRadius = RandomFloat( 0.1, 1.f );
	m_flCurrentVortexSpeed = 0.f;

	m_flAccumulatedTime = 0.f;
	m_flLastUpdateTime = gpGlobals->curtime;

	//Set up the radius for the blob
	m_flRadiusScale = RandomFloat( paintblob_min_radius_scale.GetFloat(), paintblob_max_radius_scale.GetFloat() );

	m_bShouldPlayEffect = false;

	m_bSilent = bSilent;

	m_vCollisionBoxCenter = vecOrigin;
	m_bCollisionBoxHitSolid = CheckCollisionBoxAgainstWorldAndStaticProps();

	m_hOwner = pOwner;
	m_bShouldPlaySound = false;

	m_bDrawOnly = bDrawOnly;
}

const Vector& CBasePaintBlob::GetTempEndPosition( void ) const
{
	return m_vecTempEndPosition;
}

void CBasePaintBlob::SetTempEndPosition( const Vector &vecTempEndPosition )
{
	m_vecTempEndPosition = vecTempEndPosition;
}


const Vector& CBasePaintBlob::GetTempEndVelocity( void ) const
{
	return m_vecTempEndVelocity;
}


void CBasePaintBlob::SetTempEndVelocity( const Vector &vecTempEndVelocity )
{
	m_vecTempEndVelocity = vecTempEndVelocity;
}


const Vector& CBasePaintBlob::GetPosition( void ) const
{
	return m_vecPosition;
}


void CBasePaintBlob::SetPosition( const Vector &vecPosition )
{
	m_vecPrevPosition = m_vecPosition;
	m_vecPosition = vecPosition;
}


const Vector& CBasePaintBlob::GetPrevPosition() const
{
	return m_vecPrevPosition;
}


void CBasePaintBlob::SetPrevPosition( const Vector& vPrevPosition )
{
	m_vecPrevPosition = vPrevPosition;
}


const Vector& CBasePaintBlob::GetVelocity( void ) const
{
	return m_vecVelocity;
}


void CBasePaintBlob::SetVelocity( const Vector &vecVelocity )
{
	m_vecVelocity = vecVelocity;
}


const Vector& CBasePaintBlob::GetStreakDir() const
{
	return m_vecStreakDir;
}


PaintPowerType CBasePaintBlob::GetPaintPowerType( void ) const
{
	return m_paintType;
}


PaintBlobMoveState CBasePaintBlob::GetMoveState( void ) const
{
	return m_MoveState;
}


void CBasePaintBlob::SetMoveState( PaintBlobMoveState moveState )
{
	m_MoveState = moveState;
}


float CBasePaintBlob::GetVortexDirection() const
{
	return m_flVortexDirection;
}


bool CBasePaintBlob::ShouldDeleteThis() const
{
	return m_bDeleteFlag;
}


void CBasePaintBlob::SetDeletionFlag( bool bDelete )
{
	m_bDeleteFlag = bDelete;
}


bool CBasePaintBlob::IsStreaking( void ) const
{
	return m_MoveState == PAINT_BLOB_STREAK_MOVE;
}


float CBasePaintBlob::GetLifeTime() const
{
	return m_flLifeTime;
}


void CBasePaintBlob::UpdateLifeTime( float flLifeTime )
{
	m_flLifeTime += flLifeTime;
}


void CBasePaintBlob::SetRadiusScale( float flRadiusScale )
{
	m_flRadiusScale = flRadiusScale;
}


float CBasePaintBlob::GetRadiusScale( void ) const
{
	if( m_MoveState == PAINT_BLOB_STREAK_MOVE )
	{
		return paintblob_radius_while_streaking.GetFloat();
	}

	return m_flRadiusScale;
}

class BlobTraceEnum : public ICountedPartitionEnumerator
{
public:
	BlobTraceEnum( CBaseEntity **pList, int listMax, int contentMask );

	virtual IterationRetval_t EnumElement( IHandleEntity *pHandleEntity );
	virtual int GetCount() const;
	bool AddToList( CBaseEntity *pEntity );

private:
	CBaseEntity** m_pList;
	int	m_listMax;
	int	m_count;
	int m_contentMask;
};


BlobTraceEnum::BlobTraceEnum( CBaseEntity **pList, int listMax, int contentMask )
	: m_pList( pList ),
	  m_listMax( listMax ),
	  m_count( 0 ),
	  m_contentMask( m_contentMask )
{
}


IterationRetval_t BlobTraceEnum::EnumElement( IHandleEntity *pHandleEntity )
{
#if defined( CLIENT_DLL )
	IClientEntity *pClientEntity = cl_entitylist->GetClientEntityFromHandle( pHandleEntity->GetRefEHandle() );
	C_BaseEntity *pEntity = pClientEntity ? pClientEntity->GetBaseEntity() : NULL;
#else
	CBaseEntity *pEntity = gEntList.GetBaseEntity( pHandleEntity->GetRefEHandle() );
#endif
	if( pEntity )
	{
		// Does this collide with blobs?
		if( ( !pEntity->ShouldCollide( COLLISION_GROUP_PROJECTILE, m_contentMask ) ) /*&& bNotPortalOrTBeam*/ )
			return ITERATION_CONTINUE;

		if( !AddToList( pEntity ) )
			return ITERATION_STOP;
	}

	return ITERATION_CONTINUE;
}


int BlobTraceEnum::GetCount() const
{
	return m_count;
}


bool BlobTraceEnum::AddToList( CBaseEntity *pEntity )
{
	if( m_count >= m_listMax )
	{
		AssertMsgOnce( 0, "reached enumerated list limit.  Increase limit, decrease radius, or make it so entity flags will work for you" );
		return false;
	}
	m_pList[m_count++] = pEntity;
	return true;
}


BlobTraceResult CBasePaintBlob::BlobHitSolid( CBaseEntity* pHitEntity )
{
	if ( !pHitEntity )
		return BLOB_TRACE_HIT_NOTHING;

	if( pHitEntity->IsWorld() )
	{
		return BLOB_TRACE_HIT_WORLD;
	}
	
	// Player
	if( pHitEntity->IsPlayer() )
	{
		// If the blob started in the player box, it didn't hit. Otherwise, it did.
		// Compensate for updating out-of-sync by sweeping the box along the
		// displacement for the frame.
		Vector mins = pHitEntity->GetAbsOrigin() + pHitEntity->WorldAlignMins();
		Vector maxs = pHitEntity->GetAbsOrigin() + pHitEntity->WorldAlignMaxs();
		const Vector frameDisplacement = pHitEntity->GetAbsVelocity() * gpGlobals->frametime;
		//ExpandAABB( mins, maxs, -frameDisplacement );

		// Blobs can only collide if player painting is enabled
		const bool usePaintEffects = player_can_use_painted_power.GetBool() || player_paint_effects_enabled.GetBool();
		const bool canCollideWithPlayer = usePaintEffects && !IsPointInBounds( m_vecPosition, mins, maxs );

		return canCollideWithPlayer ? BLOB_TRACE_HIT_PLAYER : BLOB_TRACE_HIT_NOTHING;
	}

	return BLOB_TRACE_HIT_SOMETHING;	
}


void UTIL_Blob_TraceWorldAndStaticPropsOnly( const Ray_t& ray, trace_t& tr )
{
	CTraceFilterWorldAndPropsOnly traceFilter;
	UTIL_TraceRay( ray, BLOB_TRACE_STATIC_MASK, &traceFilter, &tr );
}


void UTIL_Blob_EnumerateEntitiesAlongRay( const Ray_t& ray, ICountedPartitionEnumerator* pEntEnum )
{
#ifdef GAME_DLL
	if( ray.m_Delta.IsZeroFast() )
		::partition->EnumerateElementsAtPoint( PARTITION_SERVER_GAME_EDICTS, ray.m_Start, false, pEntEnum );
	else
		::partition->EnumerateElementsAlongRay( PARTITION_SERVER_GAME_EDICTS, ray, false, pEntEnum );
#else
	if( ray.m_Delta.IsZeroFast() )
		::partition->EnumerateElementsAtPoint( PARTITION_CLIENT_GAME_EDICTS, ray.m_Start, false, pEntEnum );
	else
		::partition->EnumerateElementsAlongRay( PARTITION_CLIENT_GAME_EDICTS, ray, false, pEntEnum );
#endif
}


bool CBasePaintBlob::CheckCollisionBoxAgainstWorldAndStaticProps()
{
	const float flBoxSize = paintblob_collision_box_size.GetFloat();
	Vector vExtents( flBoxSize, flBoxSize, flBoxSize );

	Ray_t ray;
	ray.Init( m_vecPosition, m_vecTempEndPosition, -vExtents, vExtents );
	trace_t tr;
	UTIL_Blob_TraceWorldAndStaticPropsOnly( ray, tr );

	m_vCollisionBoxCenter = m_vecTempEndPosition;

	return tr.DidHit();
}


void CBasePaintBlob::CheckCollisionAgainstWorldAndStaticProps( BlobCollisionRecord& solidHitRecord, float& flHitFraction )
{
	const float flBoxSize = paintblob_collision_box_size.GetFloat();
	Vector vExtents( flBoxSize, flBoxSize, flBoxSize );
	// if blob moves outside the collision box, recompute the new collision box
	if ( !IsPointInBounds( m_vecTempEndPosition, m_vCollisionBoxCenter - vExtents, m_vCollisionBoxCenter + vExtents ) )
	{
		m_bCollisionBoxHitSolid = CheckCollisionBoxAgainstWorldAndStaticProps();
	}

	// always check if we are hitting world this frame if the flag is set to true
	if ( m_bCollisionBoxHitSolid )
	{
		Ray_t ray;
		ray.Init( m_vecPosition, m_vecTempEndPosition );
		trace_t tr;
		UTIL_ClearTrace( tr );
		UTIL_Blob_TraceWorldAndStaticPropsOnly( ray, tr );

		if ( tr.DidHit() && tr.fraction < flHitFraction )
		{
			flHitFraction = tr.fraction;

			solidHitRecord.trace = tr;
			solidHitRecord.traceResultType = BlobHitSolid( tr.m_pEnt );
			solidHitRecord.targetEndPos = tr.endpos;
		}
	}
}


int CBasePaintBlob::CheckCollision( BlobCollisionRecord *pCollisions, int maxCollisions, const Vector &vecEndPos )
{
	const Vector& vecStartPos = m_vecPosition;

	Ray_t fastRay;
	fastRay.Init( vecStartPos, vecEndPos );

	{
		CBaseEntity* ppEntities[ MAX_BLOB_TRACE_ENTITY_RESULTS ];
		BlobTraceEnum entEnum( ppEntities, ARRAYSIZE( ppEntities ), BLOB_TRACE_DYNAMIC_MASK );
		UTIL_Blob_EnumerateEntitiesAlongRay( fastRay, &entEnum );

		int nEntAlongRay = entEnum.GetCount();

		int nCollisionCount = 0;

		float firstHitSolidFraction = 1.f;
		BlobCollisionRecord solidHitRecord;

		// check against world and static props if needed
		CheckCollisionAgainstWorldAndStaticProps( solidHitRecord, firstHitSolidFraction );

		for ( int i=0; i<nEntAlongRay; ++i )
		{
			CBaseEntity* pHitEntity = ppEntities[i];

			if ( FClassnameIs( pHitEntity, "trigger_paint_cleanser" ) )
			{
				trace_t tempTrace;
				enginetrace->ClipRayToEntity( fastRay, BLOB_TRACE_DYNAMIC_MASK, pHitEntity, &tempTrace );

				CTriggerPaintCleanser *pPaintCleanser = assert_cast< CTriggerPaintCleanser* >( pHitEntity );
				if ( pPaintCleanser->IsEnabled() && tempTrace.DidHit() && tempTrace.fraction < firstHitSolidFraction )
				{
					firstHitSolidFraction = tempTrace.fraction;

					solidHitRecord.trace = tempTrace;
					solidHitRecord.traceResultType = BLOB_TRACE_HIT_PAINT_CLEANSER;
					solidHitRecord.targetEndPos = tempTrace.endpos;
				}
			}
			// entity that stops the blob
			else if ( pHitEntity->IsSolid() )
			{
				trace_t tempTrace;
				enginetrace->ClipRayToEntity( fastRay, BLOB_TRACE_DYNAMIC_MASK, pHitEntity, &tempTrace );

				if ( tempTrace.DidHit() && tempTrace.fraction < firstHitSolidFraction )
				{
					BlobTraceResult traceResultType = BlobHitSolid( pHitEntity ); 					

					if( traceResultType != BLOB_TRACE_HIT_NOTHING )
					{
						firstHitSolidFraction = tempTrace.fraction;

						solidHitRecord.trace = tempTrace;
						solidHitRecord.traceResultType = traceResultType;
						solidHitRecord.targetEndPos = tempTrace.endpos;
					}
				}
			}
		}

		// add solid collision
		if ( firstHitSolidFraction < 1.f )
		{
			pCollisions[nCollisionCount++] = solidHitRecord;
		}

		return nCollisionCount;
	}
}

void CBasePaintBlob::ResolveCollision( bool& bDeleted, const BlobCollisionRecord& collision, Vector& targetVelocity, float deltaTime )
{
	VPROF_BUDGET( "CBasePaintBlob::ResolveCollision", VPROF_BUDGETGROUP_PAINTBLOB );

	//Check what the blob hit
	switch( collision.traceResultType )
	{
		case BLOB_TRACE_HIT_PAINT_CLEANSER:
			{
				PlayEffect( collision.targetEndPos, collision.trace.plane.normal );
				SetDeletionFlag( true );
				bDeleted = true;
			}
			break;
		case BLOB_TRACE_HIT_WORLD:
		case BLOB_TRACE_HIT_SOMETHING:
		case BLOB_TRACE_HIT_PLAYER:
			//If the blob hit something
			{
				SetVelocity( targetVelocity );

				//Remove the blob if the blob should not streak on this surface
				bDeleted = !PaintBlobCheckShouldStreak( collision.trace ); 
			}
			break;
	}
}


void CBasePaintBlob::UpdateBlobCollision( float flDeltaTime, const Vector& vecEndPos, Vector& vecEndVelocity )
{
	//Debugging flags
	bool bDebuggingBlobs = false;

#ifdef CLIENT_DLL
	if( debug_paint_client_blobs.GetBool() )
#else
	if( debug_paint_server_blobs.GetBool() )
#endif
	{
		bDebuggingBlobs = true;
	}

	// disable blob particles and blob render if the blob is streaking
	if ( m_MoveState == PAINT_BLOB_STREAK_MOVE && !paintblob_streak_particles_enabled.GetBool() )
	{
		m_bSilent = true;
	}

	//Check if the blob collided with anything
	BlobCollisionRecord collisions[MAX_BLOB_TRACE_ENTITY_RESULTS];

	int collisionCount = CheckCollision( collisions, MAX_BLOB_TRACE_ENTITY_RESULTS, vecEndPos );

	//Draw a tracer line to show the blobs path
	if( bDebuggingBlobs )
	{
		NDebugOverlay::Line( GetPrevPosition(), vecEndPos, g_BlobDebugColor.r(), g_BlobDebugColor.g(), g_BlobDebugColor.b(), false, 10.0f );
		NDebugOverlay::VertArrow( GetPrevPosition(), vecEndPos, 4.0f, 0, 255, 0, 255, true, 0.01f );
	}

	//If the blob didn't touch anything then move it
	if( collisionCount == 0 )
	{
		//Move the blob to its end pos
		SetPosition( vecEndPos );
		SetVelocity( vecEndVelocity );
		if ( m_MoveState != PAINT_BLOB_STREAK_MOVE )
		{
			SetMoveState( PAINT_BLOB_AIR_MOVE );
		}
	}
	// If the blob collided with things, resolve the collisions
	else
	{
		bool bDeleted = false;
		for( int i = 0; i < collisionCount && !bDeleted; ++i )
		{
			ResolveCollision( bDeleted, collisions[i], vecEndVelocity, flDeltaTime );
		}
	}
}


void CBasePaintBlob::UpdateBlobPostCollision( float flDeltaTime )
{
	//If the paint blob is streaking
	if( m_MoveState == PAINT_BLOB_STREAK_MOVE )
	{
		Vector vVelocity = m_vecVelocity;

		// just remove the blob if the it's trying to streak with speed == 0
		if ( vVelocity.IsZero() )
		{
			SetDeletionFlag( true );
			return;
		}

		//Update the streak timer
		m_flStreakTimer -= flDeltaTime;

		// apply streak paint
		bool bDeleted = PaintBlobStreakPaint( m_vecPosition );

		//Dampen the speed of the blobs while streaking
		float flSpeed = VectorNormalize( vVelocity );
		flSpeed -= m_flStreakSpeedDampenRate * flDeltaTime;

		//If the blob should still be streaking
		if( !bDeleted && m_flStreakTimer >= 0.0f && flSpeed >= 0.0f )
		{
			SetVelocity( vVelocity * flSpeed );

			//Reset the streak dir changed flag
			m_bStreakDirChanged = false;
		}
		else
		{
			SetDeletionFlag( true );
			return;
		}
	}

	{
		DecayVortexSpeed( flDeltaTime );
	}
}

bool CBasePaintBlob::PaintBlobCheckShouldStreak( const trace_t &trace )
{
	bool bDebuggingStreaking = false;
#ifdef GAME_DLL
	if( debug_paintblobs_streaking.GetBool() )
	{
		bDebuggingStreaking = true;
	}
#endif

	// don't streak if blob is in tractor beam or out of streak time
	if( m_flStreakTimer <= 0.0f )
	{
		CPaintBlob *pBlob = assert_cast< CPaintBlob* >( this );
		pBlob->PaintBlobPaint( trace );
		SetDeletionFlag( true );
		return false;
	}


	if( bDebuggingStreaking )
	{
		//Draw the collision position
		NDebugOverlay::Cross3D( trace.startpos, 2.0f, 255 - g_BlobDebugColor.r(), 255 - g_BlobDebugColor.g(), 255 - g_BlobDebugColor.b(), true, 10.0f );
		NDebugOverlay::Cross3D( trace.endpos, 2.0f, g_BlobDebugColor.r(), g_BlobDebugColor.g(), g_BlobDebugColor.b(), true, 10.0f );
	}

	const Vector& vecSurfaceNormal = trace.plane.normal;
	Vector vecStreakVelocity = m_vecVelocity - DotProduct( vecSurfaceNormal, m_vecVelocity ) * vecSurfaceNormal;

	Vector vecVelocityDir = m_vecVelocity.Normalized();
	Vector vecStreakVelocityDir = vecStreakVelocity.Normalized();

	//Check the angle of impact for the blob
	float flBlobImpactAngle = RAD2DEG( acos( clamp( DotProduct( vecStreakVelocityDir, vecVelocityDir ), -1.f, 1.f ) ) );

	if( bDebuggingStreaking )
	{
		Vector vecDrawPos = trace.endpos + vecSurfaceNormal * 50;

		//Draw the surface normal
		NDebugOverlay::VertArrow( vecDrawPos, vecDrawPos + vecSurfaceNormal * 50, 2, 0, 255, 0, 255, true, 10.0f );

		//Draw the velocity dir of the blob
		NDebugOverlay::VertArrow( vecDrawPos, vecDrawPos + (vecVelocityDir * 50), 2, 0, 0, 255, 255, true, 10.0f );

		//Draw the streak velocity of the blob
		NDebugOverlay::VertArrow( vecDrawPos, vecDrawPos + vecStreakVelocityDir * 50, 2, 0, 255, 255, 255, true, 10.0f );

		//Display the impact angle of the blob
		CFmtStr msg;
		msg.sprintf( "Impact angle: %f\n", flBlobImpactAngle );
		NDebugOverlay::Text( vecDrawPos, msg, true, 10.0f );
	}

	bool bShouldStreak = false;

	//Check the streaking conditions
	if( ( vecSurfaceNormal.z > -0.5f && vecSurfaceNormal.z < 0.5f ) || //If the blob hit a wall
		flBlobImpactAngle <= paintblob_streak_angle_threshold.GetFloat() ) //If the impact angle of the blob is within the threshold
	{
		bShouldStreak = true;
	}

	//Check if the blob should streak paint
	Vector vecHitPlaneDir = trace.plane.normal.Normalized();
	bool bSameSurface = AlmostEqual( -m_vecStreakDir, vecHitPlaneDir );

	//Set the streaking data if
	CPaintBlob *pBlob = assert_cast< CPaintBlob* >( this );
	if( ( m_MoveState != PAINT_BLOB_STREAK_MOVE && bShouldStreak ) || //The blob was not already streaking and it should streak
		( m_MoveState == PAINT_BLOB_STREAK_MOVE && bShouldStreak && !bSameSurface ) )//The blob was streaking and is should streak on a different surface
	{
		//Set the streaking data for the blob
		SetMoveState( PAINT_BLOB_STREAK_MOVE );
		SetVelocity( vecStreakVelocity );
		pBlob->PaintBlobPaint( trace );
		m_vecStreakDir = -( vecHitPlaneDir );

		return true;
	}

	pBlob->PaintBlobPaint( trace );
	SetDeletionFlag( true );

	return bShouldStreak;
}


bool CBasePaintBlob::PaintBlobStreakPaint( const Vector &vecBlobStartPos )
{
	bool bRemoveBlob = false;

	Ray_t blobRay;
	blobRay.Init( vecBlobStartPos, vecBlobStartPos + m_vecStreakDir * paintblob_streak_trace_range.GetFloat() );

#ifndef CLIENT_DLL
	if( debug_paintblobs_streaking.GetBool() )
	{
		NDebugOverlay::Line( vecBlobStartPos, vecBlobStartPos + m_vecStreakDir * paintblob_streak_trace_range.GetFloat(), 255, 0, 255, true, 10.0f );
	}
#endif

	//See if the blob hit a portal or anything else
	trace_t trace;
	CTraceFilterNoNPCsOrPlayer traceFilter;
	const int contentMask = BLOB_TRACE_STATIC_MASK | BLOB_TRACE_DYNAMIC_MASK;
	UTIL_TraceRay( blobRay, contentMask, &traceFilter, &trace );

	if( trace.DidHit() )
	{
		PaintBlobPaint( trace );
	}
	else //The blob hit nothing
	{
		//Don't remove the blob if the streak direction changed this update
		if( !m_bStreakDirChanged )
		{
			bRemoveBlob = true;
		}
	}

	return bRemoveBlob;
}
void CBasePaintBlob::DecayVortexSpeed( float flDeltaTime )
{
}


const Vector& CBasePaintBlob::GetContactNormal() const
{
	return m_vContactNormal;
}


void CBasePaintBlob::PlayEffect( const Vector& vPosition, const Vector& vNormal )
{
	SetPosition( vPosition );
	m_vContactNormal = vNormal;
	m_bShouldPlayEffect = true;
}

struct BlobInAir_t : std::unary_function< CPaintBlob*, bool >
{
	inline bool operator()( const CPaintBlob* pBlob ) const
	{
		return pBlob->GetMoveState() == PAINT_BLOB_AIR_MOVE;
	}
};


void SplitBlobsIntoMovementGroup( PaintBlobVector_t& blobs, PaintBlobVector_t& blobsInBeam, PaintBlobVector_t& blobsInAir, PaintBlobVector_t& blobsInStreak )
{
	// split blobs in beam
	CPaintBlob** begin = blobs.Base();
	CPaintBlob** end = begin + blobs.Count();
	CPaintBlob** middle;

	middle = std::partition( begin, end, BlobInAir_t() );
	int numBlobsInAir = middle - begin;
	blobsInAir.CopyArray( begin, numBlobsInAir );

	int numBlobsInStreak = end - middle;
	blobsInStreak.CopyArray( middle, numBlobsInStreak );
}

class BlobsInAirUpdate_SIMD
{
public:
	BlobsInAirUpdate_SIMD( const PaintBlobVector_t& blobs )
	{
		if ( blobs.Count() == 0 )
			return;

		m_flDeltaTime = gpGlobals->curtime - blobs[0]->GetLastUpdateTime();

		int numBlobs = blobs.Count();
		m_data.EnsureCount( numBlobs );
		for ( int i=0; i<numBlobs; ++i )
		{
			m_data[i].m_f4Position = LoadUnaligned3SIMD( blobs[i]->GetPosition().Base() );
			m_data[i].m_f4Velocity = LoadUnaligned3SIMD( blobs[i]->GetVelocity().Base() );
#ifdef BLOB_SIMD_DEBUG
			m_data[i].m_vPosition = blobs[i]->GetPosition();
			m_data[i].m_vVelocity = blobs[i]->GetVelocity();
#endif
		}

		UpdateBlobsInAir_SIMD();

		for ( int i=0; i<numBlobs; ++i )
		{
			Vector pos;
			StoreUnaligned3SIMD( pos.Base(), m_data[i].m_f4Position );
			blobs[i]->SetTempEndPosition( pos );

			Vector vel;
			StoreUnaligned3SIMD( vel.Base(), m_data[i].m_f4Velocity );
			blobs[i]->SetTempEndVelocity( vel );
		}
	}

	~BlobsInAirUpdate_SIMD()
	{
		m_data.Purge();
	}
private:
	struct BlobAirUpdateData_t
	{
		fltx4 m_f4Position;
		fltx4 m_f4Velocity;
#ifdef BLOB_SIMD_DEBUG
		Vector m_vPosition;
		Vector m_vVelocity;
#endif
	};

	void UpdateBlobsInAir_SIMD()
	{
#ifdef BLOB_SIMD_DEBUG
		const float flDeltaTime = m_flDeltaTime;
		float flNewSpeedFraction = ( 1.f - paintblob_air_drag.GetFloat() * flDeltaTime );

		for ( int j=0; j<m_data.Count(); ++j )
		{
			Vector vecMove;

			Vector vecAbsVelocity = m_data[j].m_vVelocity;

			vecMove.x = vecAbsVelocity.x * flDeltaTime;
			vecMove.y = vecAbsVelocity.y * flDeltaTime;

			//Apply gravity
			float newZVelocity = vecAbsVelocity.z - paintblob_gravity_scale.GetFloat() * sv_gravity.GetFloat() * flDeltaTime;
			vecMove.z = 0.5f * ( vecAbsVelocity.z + newZVelocity ) * flDeltaTime;

			vecAbsVelocity.z = newZVelocity;

			//Apply air drag
			m_data[j].m_vVelocity = vecAbsVelocity * flNewSpeedFraction;
			m_data[j].m_vPosition = m_data[j].m_vPosition + vecMove;
		}
#endif

		fltx4 f4DeltaTime = ReplicateX4( m_flDeltaTime );
		fltx4 f4Gravity = { 0.f, 0.f, paintblob_gravity_scale.GetFloat() * sv_gravity.GetFloat(), 0.f };
		fltx4 f4GravityDT = MulSIMD( f4Gravity, f4DeltaTime );
		fltx4 f4NewSpeedFraction = MsubSIMD( ReplicateX4( paintblob_air_drag.GetFloat() ), f4DeltaTime, Four_Ones );
		fltx4 f4HalfDeltaTime = MulSIMD( Four_PointFives, f4DeltaTime );

		int count = m_data.Count();
		int i = 0;
		while ( count >= 4 )
		{
			fltx4 f4Vel0 = m_data[i].m_f4Velocity;
			fltx4 f4Vel1 = m_data[i+1].m_f4Velocity;
			fltx4 f4Vel2 = m_data[i+2].m_f4Velocity;
			fltx4 f4Vel3 = m_data[i+3].m_f4Velocity;

			fltx4 f4NewVel0 = SubSIMD( f4Vel0, f4GravityDT );
			fltx4 f4NewVel1 = SubSIMD( f4Vel1, f4GravityDT );
			fltx4 f4NewVel2 = SubSIMD( f4Vel2, f4GravityDT );
			fltx4 f4NewVel3 = SubSIMD( f4Vel3, f4GravityDT );

			m_data[i].m_f4Velocity = MulSIMD( f4NewVel0, f4NewSpeedFraction );
			m_data[i+1].m_f4Velocity = MulSIMD( f4NewVel1, f4NewSpeedFraction );
			m_data[i+2].m_f4Velocity = MulSIMD( f4NewVel2, f4NewSpeedFraction );
			m_data[i+3].m_f4Velocity = MulSIMD( f4NewVel3, f4NewSpeedFraction );

			m_data[i].m_f4Position = MaddSIMD( AddSIMD( f4Vel0, f4NewVel0 ), f4HalfDeltaTime, m_data[i].m_f4Position );
			m_data[i+1].m_f4Position = MaddSIMD( AddSIMD( f4Vel1, f4NewVel1 ), f4HalfDeltaTime, m_data[i+1].m_f4Position );
			m_data[i+2].m_f4Position = MaddSIMD( AddSIMD( f4Vel2, f4NewVel2 ), f4HalfDeltaTime, m_data[i+2].m_f4Position );
			m_data[i+3].m_f4Position = MaddSIMD( AddSIMD( f4Vel3, f4NewVel3 ), f4HalfDeltaTime, m_data[i+3].m_f4Position );

#ifdef BLOB_SIMD_DEBUG
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 0 ) - m_data[i].m_vVelocity.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 1 ) - m_data[i].m_vVelocity.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 2 ) - m_data[i].m_vVelocity.z ) < 0.001f );

			Assert( fabs( SubFloat( m_data[i].m_f4Position, 0 ) - m_data[i].m_vPosition.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 1 ) - m_data[i].m_vPosition.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 2 ) - m_data[i].m_vPosition.z ) < 0.001f );
#endif

			i += 4;
			count -= 4;
		}

		// do the rest
		while ( count > 0 )
		{
			fltx4 f4Vel = m_data[i].m_f4Velocity;
			fltx4 f4NewVel = SubSIMD( f4Vel, f4GravityDT );

			m_data[i].m_f4Velocity = MulSIMD( f4NewVel, f4NewSpeedFraction );
			m_data[i].m_f4Position = MaddSIMD( AddSIMD( f4Vel, f4NewVel ), f4HalfDeltaTime, m_data[i].m_f4Position );

#ifdef BLOB_SIMD_DEBUG
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 0 ) - m_data[i].m_vVelocity.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 1 ) - m_data[i].m_vVelocity.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 2 ) - m_data[i].m_vVelocity.z ) < 0.001f );

			Assert( fabs( SubFloat( m_data[i].m_f4Position, 0 ) - m_data[i].m_vPosition.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 1 ) - m_data[i].m_vPosition.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 2 ) - m_data[i].m_vPosition.z ) < 0.001f );
#endif
			++i;
			--count;
		}
	}

	float m_flDeltaTime;
	CUtlVector< BlobAirUpdateData_t, CUtlMemoryAligned< BlobAirUpdateData_t, 16 > > m_data;
};

class BlobsInStreakUpdate_SIMD
{
public:
	BlobsInStreakUpdate_SIMD( const PaintBlobVector_t& blobs )
	{
		if ( blobs.Count() == 0 )
			return;

		m_flDeltaTime = gpGlobals->curtime - blobs[0]->GetLastUpdateTime();

		int numBlobs = blobs.Count();
		m_data.EnsureCount( numBlobs );
		for ( int i=0; i<numBlobs; ++i )
		{
			m_data[i].m_f4Position = LoadUnaligned3SIMD( blobs[i]->GetPosition().Base() );
			m_data[i].m_f4Velocity = LoadUnaligned3SIMD( blobs[i]->GetVelocity().Base() );
			m_data[i].m_f4StreakDir = LoadUnaligned3SIMD( blobs[i]->GetStreakDir().Base() );

#ifdef BLOB_SIMD_DEBUG
			m_data[i].m_vPosition = blobs[i]->GetPosition();
			m_data[i].m_vVelocity = blobs[i]->GetVelocity();
			m_data[i].m_vStreakDir = blobs[i]->GetStreakDir();
#endif
		}

		UpdateBlobsInStreak_SIMD();

		for ( int i=0; i<numBlobs; ++i )
		{
			Vector pos;
			StoreUnaligned3SIMD( pos.Base(), m_data[i].m_f4Position );
			blobs[i]->SetTempEndPosition( pos );

			Vector vel;
			StoreUnaligned3SIMD( vel.Base(), m_data[i].m_f4Velocity );
			blobs[i]->SetTempEndVelocity( vel );
		}
	}

	~BlobsInStreakUpdate_SIMD()
	{
		m_data.Purge();
	}

private:
	struct BlobStreakUpdateData_t
	{
		fltx4 m_f4Position;
		fltx4 m_f4Velocity;
		fltx4 m_f4StreakDir;

#ifdef BLOB_SIMD_DEBUG
		Vector m_vPosition;
		Vector m_vVelocity;
		Vector m_vStreakDir;
#endif
	};

	void UpdateBlobsInStreak_SIMD()
	{
		fltx4 f4DeltaTime = ReplicateX4( m_flDeltaTime );
		fltx4 f4Gravity = { 0.f, 0.f, paintblob_gravity_scale.GetFloat() * sv_gravity.GetFloat(), 0.f };
		fltx4 f4GravityDT = MulSIMD( f4Gravity, f4DeltaTime );
		fltx4 f4HalfDeltaTime = MulSIMD( Four_PointFives, f4DeltaTime );

#ifdef BLOB_SIMD_DEBUG
		const float flDeltaTime = m_flDeltaTime;
		float flGravity = paintblob_gravity_scale.GetFloat() * sv_gravity.GetFloat() * flDeltaTime;
#endif

		for ( int i=0; i<m_data.Count(); ++i )
		{
#ifdef BLOB_SIMD_DEBUG
			Vector vecMove;
			Vector vecVelocity = m_data[i].m_vVelocity;

			Vector vecAbsVelocity = vecVelocity;
			vecMove.x = vecAbsVelocity.x * flDeltaTime;
			vecMove.y = vecAbsVelocity.y * flDeltaTime;

			//Apply gravity
			float newZVelocity = vecAbsVelocity.z - flGravity;
			vecMove.z = 0.5f * ( vecAbsVelocity.z + newZVelocity ) * flDeltaTime;
			vecAbsVelocity.z = newZVelocity;

			//Clip the velocity to the streak surface if the blob is streaking
			Vector vecSurfaceNormal = -m_data[i].m_vStreakDir;
			m_data[i].m_vVelocity = vecVelocity - DotProduct( vecSurfaceNormal, vecVelocity ) * vecSurfaceNormal;
			m_data[i].m_vPosition = ( vecMove - DotProduct( vecSurfaceNormal, vecMove ) * vecSurfaceNormal ) + m_data[i].m_vPosition;
#endif

			// SIMD
			fltx4 f4Vel = m_data[i].m_f4Velocity;
			fltx4 f4NewVel = SubSIMD( f4Vel, f4GravityDT );
			fltx4 f4Move = MulSIMD( AddSIMD( f4Vel, f4NewVel ), f4HalfDeltaTime );

			fltx4 f4SurfaceNormal = m_data[i].m_f4StreakDir;
			m_data[i].m_f4Velocity = SubSIMD( f4Vel, MulSIMD( Dot3SIMD( f4SurfaceNormal, f4Vel ), f4SurfaceNormal ) );
			m_data[i].m_f4Position = AddSIMD( MsubSIMD( Dot3SIMD( f4SurfaceNormal, f4Move), f4SurfaceNormal, f4Move ), m_data[i].m_f4Position );

#ifdef BLOB_SIMD_DEBUG
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 0 ) - m_data[i].m_vVelocity.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 1 ) - m_data[i].m_vVelocity.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Velocity, 2 ) - m_data[i].m_vVelocity.z ) < 0.001f );

			Assert( fabs( SubFloat( m_data[i].m_f4Position, 0 ) - m_data[i].m_vPosition.x ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 1 ) - m_data[i].m_vPosition.y ) < 0.001f );
			Assert( fabs( SubFloat( m_data[i].m_f4Position, 2 ) - m_data[i].m_vPosition.z ) < 0.001f );
#endif
		}
	}

	float m_flDeltaTime;
	CUtlVector< BlobStreakUpdateData_t, CUtlMemoryAligned< BlobStreakUpdateData_t, 16 > > m_data;
};


void PaintBlobUpdate( const PaintBlobVector_t& blobList )
{
	if ( blobList.Count() == 0 )
		return;

	PaintBlobVector_t firstPass;
	firstPass.EnsureCount( blobList.Count() );
	int nFirstPassCount = 0;
	for ( int i=0; i<blobList.Count(); ++i )
	{
		CPaintBlob *pBlob = blobList[i];
		Assert( !pBlob->ShouldDeleteThis() );

		float updateDeltaTime = gpGlobals->curtime - pBlob->GetLastUpdateTime();
		if ( updateDeltaTime < 0.001f )
		{
			continue;
		}

		//Update the lifetime of the blob
		pBlob->UpdateLifeTime( updateDeltaTime );

		//If the paint blobs have a limited range
		if( paintblob_limited_range.GetBool() )
		{
			if( pBlob->GetLifeTime() >= paintblob_lifetime.GetFloat() )
			{
				pBlob->SetDeletionFlag( true );
				continue;
			}
		}

		firstPass[ nFirstPassCount ] = pBlob;
		++nFirstPassCount;
	}
	firstPass.RemoveMultipleFromTail( blobList.Count() - nFirstPassCount );

	if ( nFirstPassCount == 0 )
		return;

	PaintBlobVector_t blobsInBeam;
	PaintBlobVector_t blobsInAir;
	PaintBlobVector_t blobsInStreak;
	SplitBlobsIntoMovementGroup( firstPass, blobsInBeam, blobsInAir, blobsInStreak );
	// do SIMD update here
	BlobsInAirUpdate_SIMD blobAirSIMD( blobsInAir );
	BlobsInStreakUpdate_SIMD blobStreakSIMD( blobsInStreak );


	int nSecondPassCount = 0;
	PaintBlobVector_t secondPass;
	secondPass.EnsureCount( firstPass.Count() );

	V_memcpy( secondPass.Base(), blobsInAir.Base(), blobsInAir.Count() * sizeof( CBasePaintBlob* ) );
	nSecondPassCount += blobsInAir.Count();
	V_memcpy( secondPass.Base() + nSecondPassCount, blobsInStreak.Base(), blobsInStreak.Count() * sizeof( CBasePaintBlob* ) );
	nSecondPassCount += blobsInStreak.Count();

	// do collision
	MDLCACHE_CRITICAL_SECTION_( g_pMDLCache );
	for ( int i=0; i<nSecondPassCount; ++i )
	{
		CBasePaintBlob *pBlob = secondPass[i];

		if ( pBlob->ShouldDeleteThis() )
			continue;

		float updateDeltaTime = gpGlobals->curtime - pBlob->GetLastUpdateTime();
		const Vector& vecEndPos = pBlob->GetTempEndPosition();
		Vector vecEndVelocity = pBlob->GetTempEndVelocity();

		// Exit early if the blob isn't moving
		if( pBlob->GetPosition() == vecEndPos )
		{
			continue;
		}

		pBlob->UpdateBlobCollision( updateDeltaTime, vecEndPos, vecEndVelocity );
		if ( pBlob->ShouldDeleteThis() )
		{
			continue;
		}

		pBlob->UpdateBlobPostCollision( updateDeltaTime );
	}


	// update time for blobs that get in first pass
	for ( int i=0; i<nFirstPassCount; ++i )
	{
		CBasePaintBlob *pBlob = firstPass[i];

		pBlob->SetLastUpdateTime( gpGlobals->curtime );
	}
}

void CBasePaintBlob::SetShouldPlaySound( bool shouldPlaySound )
{
	m_bShouldPlaySound = shouldPlaySound;
}


bool CBasePaintBlob::ShouldPlaySound() const
{
	return m_bShouldPlaySound && !m_bDrawOnly;
}


bool CBasePaintBlob::ShouldPlayEffect() const
{
	return m_bShouldPlayEffect && !m_bDrawOnly;
}
