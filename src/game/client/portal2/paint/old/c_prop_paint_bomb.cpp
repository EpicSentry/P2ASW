//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"
#include "c_prop_paint_bomb.h"
#include "c_paintblob.h"
#include "debugoverlay_shared.h"
#include "vstdlib/jobthread.h"
#include "c_paintblob_render.h"
#include "model_types.h"
#include "paint/paint_stream_shared.h"
#include "paint/paint_stream_manager.h"
#include "cegclientwrapper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_PropPaintBomb, DT_PropPaintBomb, CPropPaintBomb )
	RecvPropInt( RECVINFO( m_nPaintPowerType ) ),
END_RECV_TABLE()

//Paint bomb
ConVar paintbomb_draw_num_paint_blobs( "paintbomb_draw_num_paint_blobs", "20" );
ConVar paintbomb_draw_sphere_radius( "paintbomb_draw_sphere_radius", "25.0f" );
ConVar paintbomb_draw_min_blob_radius( "paintbomb_draw_min_blob_radius", "0.6" );
ConVar paintbomb_draw_max_blob_radius( "paintbomb_draw_max_blob_radius", "0.8" );
ConVar paintbomb_draw_blob_speed_min( "paintbomb_draw_blob_speed_min", "50.0f" );
ConVar paintbomb_draw_blob_speed_max( "paintbomb_draw_blob_speed_max" , "80.0f" );

ConVar cl_draw_paint_bomb_with_blobs( "cl_draw_paint_bomb_with_blobs", "1" );

C_PropPaintBomb::C_PropPaintBomb( void )
				: m_nPaintPowerType( NO_POWER ),
				  m_nPrevPaintPowerType( NO_POWER ),
				  m_flAccumulatedTime( 0.f )
{
	m_flLastUpdateTime = gpGlobals->curtime;
}


C_PropPaintBomb::~C_PropPaintBomb( void )
{
}


void C_PropPaintBomb::Precache( void )
{
	BaseClass::Precache();
}


void C_PropPaintBomb::Spawn( void )
{
	BaseClass::Spawn();

	RANDOM_CEG_TEST_SECRET_PERIOD( 768, 1024 );

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}


void C_PropPaintBomb::UpdateOnRemove( void )
{
	m_blobs.Purge();

	BaseClass::UpdateOnRemove();
}


void C_PropPaintBomb::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_nPrevPaintPowerType = m_nPaintPowerType;
}


void CreateBlobsForPaintBomb( const Vector &vecBombPos, PaintBombBlobVector_t &blobList )
{
	//The number of blobs to create for the paint bomb
	int numBlobs = paintbomb_draw_num_paint_blobs.GetInt();
	blobList.EnsureCount( numBlobs );

	for( int i = 0; i < numBlobs; ++i )
	{
		//Get a random position on a sphere around the bomb
		Vector vecRand = RandomVector( -1.0f, 1.0f );
		VectorNormalize( vecRand );
		blobList[i].vPosition = vecBombPos + vecRand * paintbomb_draw_sphere_radius.GetFloat();

		//The blobs should go through the center of the bomb
		Vector vecBlobDir = blobList[i].vPosition - vecBombPos;
		VectorNormalize( vecBlobDir );
		blobList[i].vVelocity = -vecBlobDir * RandomFloat( paintbomb_draw_blob_speed_min.GetFloat(), paintbomb_draw_blob_speed_max.GetFloat() );

		blobList[i].flRadius = RandomFloat( paintbomb_draw_min_blob_radius.GetFloat(), paintbomb_draw_max_blob_radius.GetFloat() );
	}
}


void C_PropPaintBomb::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if( updateType == DATA_UPDATE_CREATED )
	{
		if( cl_draw_paint_bomb_with_blobs.GetBool() )
		{
			CreateBlobsForPaintBomb( GetAbsOrigin(), m_blobs );
		}

		m_vPrevPosition = GetAbsOrigin();
	}

	if( m_nPaintPowerType == NO_POWER && m_nPrevPaintPowerType != NO_POWER )
	{
		CleansePaintPower();
	}
}


void C_PropPaintBomb::PostDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PostDataUpdate( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		// disable the fast path for these entities so our custom DrawModel() function gets called
		m_bCanUseFastPath = false;
	}
}


void C_PropPaintBomb::UpdateBlobs( float flDeltaTime )
{
	Vector vecNewBombPos = GetAbsOrigin();
	float flMaxDistanceFromBomb = paintbomb_draw_sphere_radius.GetFloat() * paintbomb_draw_sphere_radius.GetFloat();

	int numBlobs = m_blobs.Count();
	for( int i = 0; i < numBlobs; ++i )
	{
		PaintBombBlob_t &blob = m_blobs[i];

		Vector vecBombPosOffset = vecNewBombPos - m_vPrevPosition;

		// update position with bomb center
		Vector vecNewBlobPos = vecBombPosOffset + blob.vPosition;

		//If the blob is too far away from the bomb
		const Vector& vecBlobDir = blob.vVelocity;
		if( ( vecNewBlobPos - vecNewBombPos ).LengthSqr() > flMaxDistanceFromBomb &&
			DotProduct( vecBlobDir, vecNewBombPos - vecNewBlobPos ) < 0.0f )
		{
			//Reverse the velocity of the blob
			blob.vVelocity = -vecBlobDir;
		}

		vecNewBlobPos = vecNewBlobPos + ( vecBlobDir * flDeltaTime );

		blob.vPosition = vecNewBlobPos;
	}

	m_vPrevPosition = vecNewBombPos;
}


extern ConVar paintblob_update_per_second;
void C_PropPaintBomb::ClientThink( void )
{
	BaseClass::ClientThink();

	m_flAccumulatedTime += gpGlobals->curtime - m_flLastUpdateTime;
	m_flLastUpdateTime = gpGlobals->curtime;
	float flDeltaTime = 1.f / paintblob_update_per_second.GetFloat();
	while ( m_flAccumulatedTime > 0.f )
	{
		if ( m_flAccumulatedTime < flDeltaTime )
		{
			flDeltaTime = m_flAccumulatedTime;
		}

		UpdateBlobs( flDeltaTime );
		m_flAccumulatedTime -= flDeltaTime;
		
		if ( m_flAccumulatedTime < 0.f )
		{
			m_flAccumulatedTime = 0.f;
		}
	}
	

	SetNextClientThink( flDeltaTime );
}


extern ConVar paintblob_draw_distance_from_eye;
extern ConVar draw_paint_isosurface;
extern ConVar paintblob_isosurface_box_width;
int C_PropPaintBomb::DrawModel( int flags, const RenderableInstance_t &instance )
{
	/*
	static ConVarRef gpu_level( "gpu_level" );

	if( !draw_paint_isosurface.GetBool() )
	{
		return 0;
	}

	if ( IsPlatformX360() || IsPlatformPS3() || ( gpu_level.GetInt() < GPU_LEVEL_HIGH ) )
	{
		// Current shadows shader is too costly for X360 and creates a GPU hang. Bug 60933.
		// This is not an issue on PC.
		// Disable the shadows on blobs on X360 and PS3 as well as we are short on perf anyway.
		if ( flags & STUDIO_SHADOWDEPTHTEXTURE )
		{
			return 0;
		}
	}

	int nCount = m_blobs.Count();
	if ( nCount == 0 )
	{
		return 0;
	}

	const Vector& vCenter = GetAbsOrigin();

	//setup light for this cube of blobs
	modelrender->SetupLighting( vCenter );

	C_BasePlayer *pPlayer = GetSplitScreenViewPlayer();
	Vector vecPlayerPos = pPlayer->EyePosition();

	int iNumParticles = 0;

	ImpParticleList particleList;
	particleList.EnsureCount( m_blobs.Count() );

	for ( int i = 0; i < nCount; ++i )
	{
		const Vector& vPos = m_blobs[i].vPosition;

		ImpParticle* imp_particle = &( particleList[ i ] );
		imp_particle->center = vPos;
		imp_particle->setFieldScale( m_blobs[i].flRadius );

		++iNumParticles;
	}

	particleList.SetCountNonDestructively( iNumParticles );

	IMaterial *pMaterial = materials->FindMaterial( PaintStreamManager.GetPaintMaterialName( m_nPaintPowerType ), TEXTURE_GROUP_OTHER, true );
	NPaintRenderer::PortalMatrixList_t portalMatrixList;
	NPaintRenderer::Paintblob_Draw( BLOB_RENDER_BLOBULATOR, pMaterial, paintblob_isosurface_box_width.GetFloat(), portalMatrixList, particleList );
	*/
	return 1; //cant return nothing, pretend we rendered a blob
}

RenderableTranslucencyType_t C_PropPaintBomb::ComputeTranslucencyType( )
{
	return RENDERABLE_IS_TRANSLUCENT;
}

void C_PropPaintBomb::CleansePaintPower( void )
{
}
