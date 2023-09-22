//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"
#include "prop_paint_bomb.h"
#include "paint/paint_sprayer_shared.h"
#include "datacache/imdlcache.h"
#include "particle_parse.h"
#include "paint/paint_stream_manager.h"
#include "paint/paint_blobs_shared.h"
#include "paint_database.h"

#ifdef PORTAL2
#include "portal_player.h"
//#include "trigger_portal_cleanser.h"
#endif //PORTAL2

#include "paint/paint_cleanser_manager.h"
#include "trigger_paint_cleanser.h"
#include "collisionutils.h"
#include "cegclientwrapper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PAINT_BOMB_MODEL_NAME		"models/props/futbol.mdl"


extern ConVar player_can_use_painted_power;
extern ConVar player_paint_effects_enabled;


//Paintbomb convars
ConVar paintbomb_explosion_radius( "paintbomb_explosion_radius", "100.f", FCVAR_CHEAT, "Radius of the trace from the center of the explosion" );

ConVar paintbomb_streak_speed_min("paintbomb_streak_speed_min", "150.f", FCVAR_CHEAT );
ConVar paintbomb_streak_speed_max("paintbomb_streak_speed_max", "250.f", FCVAR_CHEAT );

ConVar paintbomb_blobs_min_streak_time( "paintbomb_blobs_min_streak_time", "0.1f", FCVAR_CHEAT );
ConVar paintbomb_blobs_max_streak_time( "paintbomb_blobs_max_streak_time", "0.2f", FCVAR_CHEAT );
ConVar paintbomb_blobs_min_streak_speed_dampen( "paintbomb_blobs_min_streak_speed_dampen", "500.f", FCVAR_CHEAT );
ConVar paintbomb_blobs_max_streak_speed_dampen( "paintbomb_blobs_max_streak_speed_dampen", "800.0f", FCVAR_CHEAT );

ConVar paintbomb_vertical_angle_split( "paintbomb_vertical_angle_split", "8", FCVAR_CHEAT );
ConVar paintbomb_horizontal_angle_split( "paintbomb_horizontal_angle_split", "8", FCVAR_CHEAT );


// debug convar
ConVar debug_paintbomb_explosion( "debug_paintbomb_explosion", "0", FCVAR_CHEAT );



BEGIN_DATADESC( CPropPaintBomb )
	DEFINE_KEYFIELD( m_nPaintPowerType, FIELD_INTEGER, "PaintType" ),
	DEFINE_KEYFIELD( m_bPlaySpawnSound, FIELD_BOOLEAN, "Play Spawn Sound" ),
	DEFINE_KEYFIELD( m_bAllowSilentDissolve, FIELD_BOOLEAN, "AllowSilentDissolve" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Dissolve", InputDissolve ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SilentDissolve", InputSilentDissolve ),
	DEFINE_INPUTFUNC( FIELD_VOID, "DisablePortalFunnel", InputDisablePortalFunnel ),
	DEFINE_INPUTFUNC( FIELD_VOID, "EnablePortalFunnel", InputEnablePortalFunnel ),

	DEFINE_OUTPUT( m_OnFizzled, "OnFizzled" ),
	DEFINE_OUTPUT( m_OnExploded, "OnExploded" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CPropPaintBomb, DT_PropPaintBomb )
	SendPropInt( SENDINFO( m_nPaintPowerType ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( prop_paint_bomb, CPropPaintBomb );

CPropPaintBomb::CPropPaintBomb( void )
			   : m_nPaintPowerType( NO_POWER ),
				 m_bPlaySpawnSound( true ),
				 m_bAllowSilentDissolve( false )
{
}


CPropPaintBomb::~CPropPaintBomb( void )
{
}

void CPropPaintBomb::Precache( void )
{
	PrecacheModel( PAINT_BOMB_MODEL_NAME );
	PrecacheModel( "models/props/futbol_gib01.mdl" );
	PrecacheModel( "models/props/futbol_gib02.mdl" );
	PrecacheModel( "models/props/futbol_gib03.mdl" );
	PrecacheModel( "models/props/futbol_gib04.mdl" );
	PrecacheScriptSound( "PaintBomb.Splat" );
	PrecacheScriptSound( "PaintBlob.Inception" );

	switch ( m_nPaintPowerType )
	{
	case BOUNCE_POWER:
		PrecacheParticleSystem( "paint_bomb_bounce" );
		break;
	case SPEED_POWER:
		PrecacheParticleSystem( "paint_bomb_speed" );
		break;
	case REFLECT_POWER:
		PrecacheParticleSystem( "paint_bomb_stick" );
		break;
	default:
		PrecacheParticleSystem( "paint_bomb_erase" );
	}

	PrecacheMaterial( PaintStreamManager.GetPaintMaterialName( m_nPaintPowerType ) );
	
	BaseClass::Precache();
}

void CPropPaintBomb::Spawn( void )
{
	Precache();

	SetModel( PAINT_BOMB_MODEL_NAME );
	AddEffects( EF_NODRAW );

	//Color color = MapPowerToVisualColor( m_nPaintPowerType );
	//SetRenderColor( color.r(), color.g(), color.b() );

	AddSpawnFlags( SF_PHYSPROP_PREVENT_PICKUP );

	BaseClass::Spawn();

	//HACKHACK : Set the surface properties of the paint bomb.
	//This should be temp since we are using the glass futbol model for the paint bomb.
	/*
	int nPaintBombSurfaceIndex = GetPhysicsSurfaceProps()->GetSurfaceIndex( "PaintBomb" );
	IPhysicsObject* pPhysObject = VPhysicsGetObject();
	if( pPhysObject )
	{
		pPhysObject->SetMaterialIndex( nPaintBombSurfaceIndex );
	}
	*/
	if( m_bPlaySpawnSound )
	{
		CBroadcastRecipientFilter filter;
		CBaseEntity::EmitSound( filter,
								entindex(),
								"PaintBlob.Inception",
								&GetAbsOrigin() );
	}

	// Never let crucial game components fade out!
	SetFadeDistance( -1.0f, 0.0f );
	SetGlobalFadeScale( 0.0f );
}

int CPropPaintBomb::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_PVSCHECK );
}


void CPropPaintBomb::SetPaintPowerType( PaintPowerType paintType )
{
	m_nPaintPowerType = paintType;

	Color color = MapPowerToVisualColor( m_nPaintPowerType );
	SetRenderColor( color.r(), color.g(), color.b() );
}


void GenerateBombDirections( CUtlVector< Vector >& directions )
{
	if ( paintbomb_vertical_angle_split.GetInt() <= 0 || paintbomb_horizontal_angle_split.GetInt() <= 0 )
		return;

	directions.Purge();

	// generate explosion directions
	float flVerticalAngle = 180.f / paintbomb_vertical_angle_split.GetFloat();
	float flHorizontalAngle = 180.f / paintbomb_horizontal_angle_split.GetFloat();

	// fan the vector around Y-axis, so we get vectors on XZ plane
	Vector vecTemp = Vector( 0, 0, 1 );
	VMatrix matRotation;
	MatrixBuildRotationAboutAxis( matRotation, Vector( 0, 1, 0 ), flVerticalAngle );
	int nVerticalSplit = paintbomb_vertical_angle_split.GetInt() - 1;
	for ( int i = 0; i < nVerticalSplit; ++i )
	{
		vecTemp = matRotation * vecTemp;
		directions.AddToTail( vecTemp );
		directions.AddToHead( -vecTemp );
	}

	// rotate the faned vectors around Z-axis to generate sphere
	MatrixBuildRotateZ( matRotation, flHorizontalAngle );
	for ( int x = 1; x < paintbomb_vertical_angle_split.GetInt(); ++x )
	{
		int startIndex = directions.Count() - nVerticalSplit;
		for ( int y = 0; y < nVerticalSplit; ++y )
		{
			vecTemp = directions[ startIndex + y ];
			vecTemp = matRotation * vecTemp;
			directions.AddToTail( vecTemp );
			directions.AddToHead( -vecTemp );
			++startIndex;
		}
	}
	directions.AddToHead( Vector( 0, 0, 1 ) );
	directions.AddToTail( Vector( 0, 0, -1 ) );
}


void DispatchDryBombParticleEffect( const Vector& pos, PaintPowerType paintType, QAngle angSplash )
{
	switch ( paintType )
	{
	case BOUNCE_POWER:
		DispatchParticleEffect( "paint_bomb_bounce", pos, angSplash );
		break;
	case SPEED_POWER:
		DispatchParticleEffect( "paint_bomb_speed", pos, angSplash );
		break;
	case REFLECT_POWER:
		DispatchParticleEffect( "paint_bomb_stick", pos, angSplash );
		break;
	case PORTAL_POWER:
		DispatchParticleEffect( "paint_bomb_erase", pos, angSplash ); // FIX THIS!!
		break;
	case NO_POWER:
		DispatchParticleEffect( "paint_bomb_erase", pos, angSplash );
		break;
	}
}


CEG_NOINLINE void CreatePaintBombExplosion( PaintPowerType paintType, const Vector& vecExplosionPos, bool bSpawnBlobs )
{
	//Only do the particle effect if not spawning blobs
	if( !bSpawnBlobs )
	{
		// Dispatch particles here!!!
		DispatchDryBombParticleEffect( vecExplosionPos, paintType, QAngle( 0, 0, 1 ) );

		return;
	}

	CUtlVector< Vector > dirs;
	QAngle angSplashAngle( 0, 0, 1 );
	GenerateBombDirections( dirs );

	const float flPaintRadius = ( paintType == NO_POWER ) ? sv_erase_surface_sphere_radius.GetFloat() : sv_paint_surface_sphere_radius.GetFloat();
	const float flAlphaPercent = sv_paint_alpha_coat.GetFloat();

	const float flBombRadius = paintbomb_explosion_radius.GetFloat();

	// check for paint cleansers in bomb radius
	PaintCleanserVector_t paintCleansers;
	PaintCleanserManager.GetPaintCleansers( paintCleansers );
	PaintCleanserVector_t intersectingCleansers;
	for ( int i=0; i<paintCleansers.Count(); ++i )
	{
		CTriggerPaintCleanser *pCleanser = paintCleansers[i];
		if ( !pCleanser->IsEnabled() )
			continue;

		Vector vMins, vMaxs;
		
		pCleanser->CollisionProp()->WorldSpaceAABB( &vMins, &vMaxs );
		if ( IsBoxIntersectingSphere( vMins, vMaxs, vecExplosionPos, flBombRadius ) )
		{
			intersectingCleansers.AddToTail( pCleanser );
		}
	}

	for( int i = 0; i < dirs.Count(); ++i )
	{
		//The direction of the blob
		Vector vecBlobFireDir = dirs[i].Normalized();

		trace_t tr;
		CTraceFilterSimpleClassnameList filter( NULL, COLLISION_GROUP_NONE );
		filter.AddClassnameToIgnore( "prop_paint_bomb" );

		Ray_t ray;
		ray.Init( vecExplosionPos, vecExplosionPos + flBombRadius * vecBlobFireDir );

		// check if bomb hits paint cleanser
		float flCleanserFraction = 1.f;
		for ( int i=0; i<intersectingCleansers.Count(); ++i )
		{
			

			trace_t cleanserTrace;
			UTIL_ClearTrace( cleanserTrace );
			enginetrace->ClipRayToEntity( ray, MASK_SHOT, (CBaseEntity*)intersectingCleansers[i], &cleanserTrace );
			if ( cleanserTrace.fraction < flCleanserFraction )
			{
				flCleanserFraction = cleanserTrace.fraction;
			}
		}
#ifdef PORTAL2
		CProp_Portal *pPortal = UTIL_Portal_TraceRay( ray, MASK_SHOT, &filter, &tr );
		if ( tr.fraction < flCleanserFraction && tr.m_pEnt != NULL && pPortal == NULL )
#else
		if ( tr.fraction < flCleanserFraction && tr.m_pEnt != NULL )
#endif
		{
			PaintDatabase.AddPaint( tr.m_pEnt, tr.endpos, tr.plane.normal, paintType, flPaintRadius, flAlphaPercent );

			// we flip the z and the x to match the orientation of how the splash particles are authored
			// all impact particles are authored with the effect going "up" (0, 0, 1)
			VectorAngles( -Vector( tr.plane.normal.z, tr.plane.normal.y, tr.plane.normal.x ), angSplashAngle );
		}
	}

	if( player_can_use_painted_power.GetBool() || player_paint_effects_enabled.GetBool() )
	{
		for( int i = 1; i <= gpGlobals->maxClients; ++i )
		{
			CBasePlayer *pPlayer = ToBasePlayer( UTIL_PlayerByIndex( i ) );

			//If the other player exists and is connected
			if( pPlayer != NULL && pPlayer->IsConnected() )
			{
				const Vector& playerCenter = pPlayer->WorldSpaceCenter();
				const Vector playerToBomb = vecExplosionPos - playerCenter;
				if( playerToBomb.LengthSqr() < flBombRadius * flBombRadius )
				{
					PaintDatabase.PaintEntity( pPlayer, paintType, playerCenter );
				}
			}
		}
	}

	// Dispatch particles here!!!
	DispatchDryBombParticleEffect( vecExplosionPos, paintType, angSplashAngle );

	if( debug_paintbomb_explosion.GetBool() )
	{
		for( int i = 0; i < dirs.Count(); ++i )
		{
			//The direction of the blob
			Vector vecBlobFireDir = dirs[i].Normalized();
			NDebugOverlay::Line( vecExplosionPos, vecExplosionPos + (vecBlobFireDir * 50.f ), 0, 255, 0, false, 5.f );
		}
	}
}

CEG_PROTECT_FUNCTION( CreatePaintBombExplosion );

void CPropPaintBomb::Event_Killed( const CTakeDamageInfo &info )
{
	EmitSound( "PaintBomb.Splat" );

	Vector vecPos = GetAbsOrigin();

	//Don't spawn blobs if the bomb exploded by hitting a paint cleanser
	bool bSpawnBlobs = !FClassnameIs( info.GetInflictor(), "trigger_paint_cleanser" );

	//Create the explosion of paint
	CreatePaintBombExplosion( static_cast<PaintPowerType>( m_nPaintPowerType.Get() ), vecPos, bSpawnBlobs );

	//Send the output
	m_OnExploded.FireOutput( this, this );

	// This code below is from BaseEntity::Event_Killed, we want to skip CBreakableProp::Event_Killed which is our base
	// We don't want to call BaseClass::Event_Killed because we don't want to call Break() which spawns futbol gibs
	if( info.GetAttacker() )
	{
		info.GetAttacker()->Event_KilledOther(this, info);
	}

	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_DEAD;
	UTIL_Remove( this );
}


void CPropPaintBomb::CleansePaintPower( void )
{
	SetPaintPowerType( NO_POWER );
}


void CPropPaintBomb::OnFizzled( void )
{
	m_OnFizzled.FireOutput( this, this );
}

void CPropPaintBomb::InputSilentDissolve( inputdata_t &in ) 
{
	if( m_bAllowSilentDissolve )
	{
		OnFizzled();
		UTIL_Remove( this );
	}
}


void CPropPaintBomb::InputDissolve( inputdata_t &in ) 
{ 
	/*
#ifdef PORTAL2
	CTriggerPortalCleanser::FizzleBaseAnimating( NULL, this );
#endif
	*/
}

void CPropPaintBomb::InputDisablePortalFunnel( inputdata_t &in )
{
	/*
#ifdef PORTAL2
	m_bAllowPortalFunnel = false;
#endif
	*/
}

void CPropPaintBomb::InputEnablePortalFunnel( inputdata_t &in )
{
	/*
#ifdef PORTAL2
	m_bAllowPortalFunnel = true;
#endif
	*/
	
}

void CreatePaintBomb( PaintPowerType paintType )
{
	MDLCACHE_CRITICAL_SECTION();

	bool bAllowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache( true );

	//Try to create the paint bomb
	CPropPaintBomb *pPaintBomb = ( CPropPaintBomb* )CreateEntityByName( "prop_paint_bomb" );
	if( pPaintBomb )
	{
		pPaintBomb->Precache();
		pPaintBomb->SetPaintPowerType( paintType );
		DispatchSpawn( pPaintBomb );

		//Now attempt to drop into the world
		CBasePlayer* pPlayer = UTIL_GetCommandClient();
		trace_t tr;
		Vector vecForward;
		pPlayer->EyeVectors( &vecForward );
		UTIL_TraceLine( pPlayer->EyePosition(), pPlayer->EyePosition() + ( vecForward * MAX_TRACE_LENGTH ), MASK_SOLID , pPlayer, COLLISION_GROUP_NONE, &tr );

		if( tr.fraction != 1.0 )
		{
			tr.endpos.z += 12;
			pPaintBomb->Teleport( &tr.endpos, NULL, NULL );
			UTIL_DropToFloor( pPaintBomb, MASK_SOLID );
		}

		//This entity should send its object caps to the client
		//pPaintBomb->UpdateObjectCapsCache();
	}

	CBaseEntity::SetAllowPrecache( bAllowPrecache );
}

CON_COMMAND_F( ent_create_paint_bomb_jump, "Creates a paint bomb with the jump paint paint power", FCVAR_CHEAT )
{
	CreatePaintBomb( BOUNCE_POWER );
}

CON_COMMAND_F( ent_create_paint_bomb_speed, "Creates a paint bomb with the speed paint paint power", FCVAR_CHEAT )
{
	CreatePaintBomb( SPEED_POWER );
}

// FIXME: Bring this back for DLC2
//CON_COMMAND_F( ent_create_paint_bomb_reflect, "Creates a paint bomb with the reflect paint paint power", FCVAR_CHEAT )
//{
//	CreatePaintBomb( REFLECT_POWER );
//}

CON_COMMAND_F( ent_create_paint_bomb_portal, "Creates a paint bomb with the portal paint paint power", FCVAR_CHEAT )
{
	CreatePaintBomb( PORTAL_POWER );
}

CON_COMMAND_F( ent_create_paint_bomb_erase, "Creates a paint bomb with the erase paint paint power", FCVAR_CHEAT )
{
	CreatePaintBomb( NO_POWER );
}
