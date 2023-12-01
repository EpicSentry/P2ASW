//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"

#include "c_paintblob_render.h"
#include "vgui_int.h"
#include "model_types.h"
#include "view_scene.h"
#include "viewrender.h"
#include "paint/paint_stream_shared.h"
#include "paint/paint_stream_manager.h"
#include "prop_portal_shared.h"
#include "videocfg/videocfg.h"
#include "CegClientWrapper.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


ConVar draw_paint_isosurface( "draw_paint_isosurface", "1" );
ConVar draw_paint_client_blobs( "draw_paint_client_blobs", "1" );
ConVar paintblob_draw_distance_from_eye( "paintblob_draw_distance_from_eye", "18.0f" );
ConVar paintblob_isosurface_box_width( "paintblob_isosurface_box_width", "8.0f" );

// local to listen server only
ConVar paintblob_interpolation_time_offset("paintblob_interpolation_time_offset", "0.1f", FCVAR_DEVELOPMENTONLY, "The offset is to make client time be behind the server update time, so blobs don't always take last update frame from the server.");


IMPLEMENT_AUTO_LIST( IPaintStreamAutoList );

IMPLEMENT_CLIENTCLASS_DT( C_PaintStream, DT_PaintStream, CPaintStream )

	RecvPropInt( RECVINFO( m_nPaintType ) ),
	RecvPropInt( RECVINFO( m_nRenderMode ) ),
	RecvPropVector( RECVINFO( m_vLightPosition ) ),
	RecvPropInt( RECVINFO( m_nMaxBlobCount ) ),

END_RECV_TABLE();

LINK_ENTITY_TO_CLASS( paint_stream, C_PaintStream );

C_PaintStream::C_PaintStream()
{
	RemoveFromLeafSystem();
}


C_PaintStream::~C_PaintStream()
{
	RemoveAllPaintBlobs();
	m_blobInterpolatedData.Purge();
}


void C_PaintStream::Spawn( void )
{
	// disable the fast path for these entities so our custom DrawModel() function gets called
	m_bCanUseFastPath = false;

	RemoveFromLeafSystem();

	BaseClass::Spawn();
}


void C_PaintStream::DebugDrawBlobs()
{
	bool bDebugDraw = false;
	Color debugColor;

	if( draw_paint_client_blobs.GetBool() )
	{
		bDebugDraw = true;
		debugColor = Color( 0, 255, 255 );
	}

	if ( bDebugDraw )
	{
		for ( int i=0; i<m_blobs.Count(); ++i )
		{
			NDebugOverlay::Cross3D( m_blobs[i]->GetPosition(), 2.0f, debugColor.r(), debugColor.g(), debugColor.b(), false, 0.1f );
		}
	}
}


const Vector& C_PaintStream::GetRenderOrigin( void )
{
	return m_vCachedRenderOrigin;
}


void C_PaintStream::GetRenderBoundsWorldspace( Vector& absMins, Vector& absMaxs )
{
	absMins = m_vCachedWorldMins;
	absMaxs = m_vCachedWorldMaxs;
}


void C_PaintStream::GetRenderBounds( Vector& theMins, Vector& theMaxs )
{
	theMins = m_vCachedWorldMins - m_vCachedRenderOrigin;
	theMaxs = m_vCachedWorldMaxs - m_vCachedRenderOrigin;
}


const Vector& C_PaintStream::WorldSpaceCenter()
{
	return GetRenderOrigin();
}


void C_PaintStream::ComputeWorldSpaceSurroundingBox( Vector *pMins, Vector *pMaxs )
{
	GetRenderBoundsWorldspace( *pMins, *pMaxs );
}


void C_PaintStream::AddPaintBlob( C_PaintBlob *pBlob )
{
	if ( pBlob )
	{
		m_blobs.AddToTail( pBlob );
	}
	else
	{
		//Warning("pBlob == NULL!!!\n");
	}
}


void CPaintStream::UpdateRenderBoundsAndOriginWorldspace( void )
{
	//if ( engine->IsClientLocalToActiveServer() )
	if ( false )
	{	
		GetInterpolatedBlobData( m_blobInterpolatedData );

		if ( m_blobInterpolatedData.Count() == 0 )
		{
			m_vCachedRenderOrigin = m_vCachedWorldMins = m_vCachedWorldMaxs = Vector(0,0,0);
			RemoveFromLeafSystem();
		}
		else
		{
			Vector vecPos = m_blobInterpolatedData[0].m_vPosition;

			m_vCachedWorldMins = vecPos;
			m_vCachedWorldMaxs = vecPos;

			for (int i = 1; i < m_blobInterpolatedData.Count(); i++)
			{
				vecPos = m_blobInterpolatedData[i].m_vPosition;
				VectorMin( vecPos, m_vCachedWorldMins, m_vCachedWorldMins );
				VectorMax( vecPos, m_vCachedWorldMaxs, m_vCachedWorldMaxs );
			}

			m_vCachedRenderOrigin = ( m_vCachedWorldMaxs + m_vCachedWorldMins ) * 0.5f;

			Vector vRadius = ReplicateToVector( PAINTBLOB_MAX_RADIUS * 12.0f );
			m_vCachedWorldMins -= vRadius;
			m_vCachedWorldMaxs += vRadius;

			CCollisionProperty *prop = CollisionProp();
			prop->MarkSurroundingBoundsDirty();
			AddToLeafSystem();
		}

		UpdateVisibility();
	}
	else
	{
		if( m_blobs.Count() == 0 )
		{
			m_vCachedRenderOrigin = m_vCachedWorldMins = m_vCachedWorldMaxs = Vector(0,0,0);
			RemoveFromLeafSystem();
		}
		else
		{
			Vector vecPos = m_blobs[0]->GetPosition();

			m_vCachedWorldMins = vecPos;
			m_vCachedWorldMaxs = vecPos;

			for (int i = 1; i < m_blobs.Count(); i++)
			{
				vecPos = m_blobs[i]->GetPosition();
				VectorMin( vecPos, m_vCachedWorldMins, m_vCachedWorldMins );
				VectorMax( vecPos, m_vCachedWorldMaxs, m_vCachedWorldMaxs );
			}

			m_vCachedRenderOrigin = ( m_vCachedWorldMaxs + m_vCachedWorldMins ) * 0.5f;

			Vector vRadius = ReplicateToVector( PAINTBLOB_MAX_RADIUS * 12.0f );
			m_vCachedWorldMins -= vRadius;
			m_vCachedWorldMaxs += vRadius;

			CCollisionProperty *prop = CollisionProp();
			prop->MarkSurroundingBoundsDirty();

			AddToLeafSystem();
		}

		UpdateVisibility();
	}
}


void BuildPortalMatrixListInAABB( const Vector& vCenter, const Vector& vExtents, PortalMatrixList_t& portalMatrixList )
{
	//Tony; no portal? no portalMatrixList . empty list is okay in blobulator.
#ifdef PORTAL2
	for ( int i=0; i<CProp_Portal_Shared::AllPortals.Count(); ++i )
	{
		C_Prop_Portal *pPortal = CProp_Portal_Shared::AllPortals[i];
		// only add portal to portal clip list if portal is active and linked and inside the box
		if ( pPortal && pPortal->IsActivedAndLinked() && UTIL_IsBoxIntersectingPortal( vCenter, vExtents, pPortal ) )
		{
			matrix3x4_t matTransform;
			AngleMatrix( pPortal->GetAbsAngles(), pPortal->GetAbsOrigin() - 5.f * pPortal->m_plane_Origin.normal, matTransform );
			portalMatrixList.AddToTail( matTransform );
		}
	}
#endif
}


void C_PaintStream::DrawBlobs( IMaterial* pMaterial )
{
	PortalMatrixList_t portalMatrixList;
	BuildPortalMatrixListInAABB( m_vCachedRenderOrigin, 0.5f * ( m_vCachedWorldMaxs - m_vCachedWorldMins ), portalMatrixList );

	//bool bListenServer = engine->IsClientLocalToActiveServer();

	int nBlobCount;
	//if ( bListenServer )
	{
	//	nBlobCount = m_blobInterpolatedData.Count();
	}
	//else
	{
		nBlobCount = m_blobs.Count();
	}

	if ( nBlobCount == 0 )
	{
		return;
	}

	if( !draw_paint_isosurface.GetBool() )
	{
		return;
	}

	//setup light for this cube of blobs
	modelrender->SetupLighting( m_vLightPosition );

#if defined ( USE_BLOBULATOR ) || defined ( USE_PARTICLE_BLOBULATOR )

#ifndef USE_PARTICLE_BLOBULATOR
	C_BasePlayer *pPlayer = GetSplitScreenViewPlayer();
	if ( !pPlayer )
		return;

	Vector vecPlayerPos = pPlayer->EyePosition();
	float flDistThreshold = pow( paintblob_draw_distance_from_eye.GetFloat(), 2.0f );

	int iNumParticles = 0;
	ImpParticleList particleList;
	if ( bListenServer )
	{
		particleList.EnsureCount( nBlobCount );

		for ( int i = 0; i < nBlobCount; ++i )
		{
			Vector vecWorldCenter = m_blobInterpolatedData[i].m_vPosition;

			if( vecWorldCenter.DistToSqr( vecPlayerPos ) > flDistThreshold )
			{
				ImpParticle* imp_particle = &(particleList[ iNumParticles ]);
				imp_particle->center = vecWorldCenter;
				imp_particle->setFieldScale( m_blobInterpolatedData[i].m_flScale );

				++iNumParticles;
			}
		}
	}
	else
	{
		// allocate double the size of blobs to include ghost blobs
		particleList.EnsureCount( 2 * nBlobCount );

		for ( int i = 0; i < nBlobCount; ++i )
		{
			CBasePaintBlob *pBlob = m_blobs[i];
			if ( pBlob->IsSilent() )
			{
				continue;
			}

			Vector vecWorldCenter = pBlob->GetPosition();
			if( vecWorldCenter.DistToSqr( vecPlayerPos ) > flDistThreshold )
			{
				ImpParticle* imp_particle = &( particleList[ iNumParticles ] );
				imp_particle->center = vecWorldCenter;
				imp_particle->setFieldScale( pBlob->GetRadiusScale() );

				++iNumParticles;
			}
		}
	}
	particleList.SetCountNonDestructively( iNumParticles );
	
	NPaintRenderer::Paintblob_Draw( m_nRenderMode, pMaterial, paintblob_isosurface_box_width.GetFloat(), portalMatrixList, particleList );
#else

	NPaintRenderer::Paintblob_Draw( m_nRenderMode, pMaterial, paintblob_isosurface_box_width.GetFloat(), portalMatrixList );

#endif // USE_PARTICLE_BLOBULATOR

#else

	//Msg("m_blobs.Count(): %i\n", m_blobs.Count());

#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
	for (int i = 0; i < m_blobs.Count(); ++i)
	{		
		if (!m_blobs[i])
			continue;
		
		m_blobs[i]->m_hRenderable->PerFrameUpdate();
		
	}
#endif

#endif

	portalMatrixList.Purge();
}


int C_PaintStream::DrawModel( int flags, const RenderableInstance_t &instance )
{
#if 1
	static ConVarRef gpu_level( "gpu_level" );

	// See if we should draw
	if ( m_bReadyToDraw == false )
		return 0;

	if( m_blobs.Count() == 0 && !engine->IsClientLocalToActiveServer() )
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

	CPaintStreamManager *pPaintStreamManager = &PaintStreamManager;

	if (!pPaintStreamManager)
		return 0;

	const char *pszPaintMaterial = PaintStreamManager.GetPaintMaterialName( m_nPaintType );

	IMaterial *pMaterial = materials->FindMaterial( pszPaintMaterial, TEXTURE_GROUP_OTHER, true );

	// FIXME: Only do this once a frame for the first ice sculpture rendered
	// This is for making blobs transparent
	if ( (CurrentViewID() != VIEW_SHADOW_DEPTH_TEXTURE) && pMaterial->NeedsFullFrameBufferTexture() )
	{
		UpdateRefractTexture();
	}
	
	//DebugDrawBlobs();
	DrawBlobs( pMaterial );

	return 1;
#else

	return BaseClass::DrawModel( flags, instance );

#endif
}


RenderableTranslucencyType_t C_PaintStream::ComputeTranslucencyType( )
{
	if ( m_nPaintType == NO_POWER )
	{
		// Only NO_POWER (aka the erasing paint) is translucent
		return RENDERABLE_IS_TRANSLUCENT;
	}
	else
	{
		return RENDERABLE_IS_OPAQUE;
	}
}


Vector InterpolateTeleportingBlob( float flCurrentTime, float flTimeA, float flTimeB, const BlobTeleportationHistoryVector_t& history, const Vector& posA, const Vector& posB, CUtlVector< VMatrix >& ghostTransformMatrices )
{
	int nHistoryCount = history.Count();
	Assert( nHistoryCount > 0 );

	// interpolate blobs before going through portal
	if ( flCurrentTime <= history[0].m_flTeleportTime )
	{
		ghostTransformMatrices.AddToTail( history[0].m_matSourceToLinked );
		float flPercent = RemapValClamped( flCurrentTime, flTimeA, history[0].m_flTeleportTime, 0.f, 1.f );
		return Lerp( flPercent, posA, history[0].m_vEnterPosition );
	}
	
	// going through multiple portals in one update (possible?)
	int nLastHistory = nHistoryCount - 1;
	for ( int i=0; i<nLastHistory; ++i )
	{
		if ( history[i].m_flTeleportTime <= flCurrentTime && flCurrentTime <= history[i+1].m_flTeleportTime )
		{
			ghostTransformMatrices.AddToTail( history[i].m_matLinkedToSource );
			ghostTransformMatrices.AddToTail( history[i+1].m_matSourceToLinked );

			float flPercent = RemapValClamped( flCurrentTime, history[i].m_flTeleportTime, history[i+1].m_flTeleportTime, 0.f, 1.f );
			return Lerp( flPercent, history[i].m_vExitPosition, history[i+1].m_vEnterPosition );
		}
	}

	// interpolate last exit portal
	ghostTransformMatrices.AddToTail( history[nLastHistory].m_matLinkedToSource );
	float flPercent = RemapValClamped( flCurrentTime, history[nLastHistory].m_flTeleportTime, flTimeB, 0.f, 1.f );
	return Lerp( flPercent, history[nLastHistory].m_vExitPosition, posB );
}


void InterpolateBlobData( float flCurrentTime, const BlobInterpolationData_t& dataA, const BlobInterpolationData_t& dataB, BlobDataVector_t& blobDataVector )
{
	float flTimeA = dataA.m_flUpdateTime;
	float flTimeB = dataB.m_flUpdateTime;
	float flDeltaTime = flCurrentTime - flTimeA;
	float flPercent = flDeltaTime / ( flTimeB - flTimeA );

	int a = 0;
	int b = 0;
	const BlobDataVector_t& blobVectorA = dataA.m_blobData;
	const BlobDataVector_t& blobVectorB = dataB.m_blobData;
	for ( ; a < blobVectorA.Count() && b < blobVectorB.Count(); )
	{
		const BlobData_t& blobA = blobVectorA[a];
		const BlobData_t& blobB = blobVectorB[b];

		if ( blobA.m_blobID == blobB.m_blobID )
		{
			BlobData_t blobData;
			blobData.m_blobID = blobA.m_blobID;
			blobData.m_flScale = Lerp( flPercent, blobA.m_flScale, blobB.m_flScale );

			CUtlVector< VMatrix > ghostTransformMatrices;

			// interpolate position
			if ( blobB.m_bTeleportedThisFrame )
			{
				// interpolate from A to B (interpolation history is always in B)
				const BlobTeleportationHistoryVector_t& history = blobB.m_teleportationHistory;
				blobData.m_vPosition = InterpolateTeleportingBlob( flCurrentTime, flTimeA, flTimeB, history, blobA.m_vPosition, blobB.m_vPosition, ghostTransformMatrices );
			}
			else
			{
				blobData.m_vPosition = Lerp( flPercent, blobA.m_vPosition, blobB.m_vPosition );
				
#ifdef PORTAL2
				if ( blobB.m_bGhosting )
				{
					ghostTransformMatrices.AddToTail( blobB.m_matGhostTransform );
				}
#endif
			}

			// add interpolated data to the list
			blobDataVector.AddToTail( blobData );

			Assert( ghostTransformMatrices.Count() <= 2 );
			for ( int i=0; i<ghostTransformMatrices.Count(); ++i )
			{
				BlobData_t ghostBlob( blobData );
				VectorTransform( blobData.m_vPosition, ghostTransformMatrices[i].As3x4(), ghostBlob.m_vPosition );

				blobDataVector.AddToTail( ghostBlob );
			}

			a++;
			b++;
		}
		else if ( blobA.m_blobID < blobB.m_blobID )
		{
			a++;
		}
		else
		{
			b++;
		}
	}
}


void C_PaintStream::GetInterpolatedBlobData( BlobDataVector_t& blobInterpolatedData )
{
	AUTO_LOCK_FM( m_sharedBlobDataMutex );

	// clear old data before adding new ones
	blobInterpolatedData.RemoveAll();
	int nTotalFrame = m_sharedBlobData.Count();
	if ( nTotalFrame == 0 )
		return;

	// super hack, client back in time
	const float flCurrentTime = gpGlobals->curtime - paintblob_interpolation_time_offset.GetFloat();

	// not supposed to update yet
	float flOldestDataTime = m_sharedBlobData.Get( 0 ).m_flUpdateTime;
	if ( flCurrentTime < flOldestDataTime )
	{
		return;
	}

	int nCurrentFrame;
	int nLastFrame = nTotalFrame - 1;
	for ( nCurrentFrame=0; nCurrentFrame < nLastFrame; ++nCurrentFrame )
	{
		float flTimeA = m_sharedBlobData.Get( nCurrentFrame ).m_flUpdateTime;
		float flTimeB = m_sharedBlobData.Get( nCurrentFrame + 1 ).m_flUpdateTime;

		if ( flTimeA <= flCurrentTime && flCurrentTime <= flTimeB )
		{	
			break;
		}
	}

	if ( nCurrentFrame == nLastFrame )
	{
		const BlobDataVector_t& temp = m_sharedBlobData.Get( nCurrentFrame ).m_blobData;
		blobInterpolatedData.CopyArray( temp.Base(), temp.Count() );
	}
	else
	{
		const BlobInterpolationData_t& dataA = m_sharedBlobData.Get( nCurrentFrame );
		const BlobInterpolationData_t& dataB = m_sharedBlobData.Get( nCurrentFrame + 1 );

		InterpolateBlobData( flCurrentTime, dataA, dataB, blobInterpolatedData );
	}
}

