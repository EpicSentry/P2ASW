//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"

#include "cpaintblob.h"
#include "paint/paint_stream_shared.h"
#include "paint/paint_stream_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


ConVar draw_paint_server_blobs( "draw_paint_server_blobs", "0", FCVAR_DEVELOPMENTONLY );
ConVar paintblob_old_data_time_offset("paintblob_old_data_time_offset", "2.f", FCVAR_DEVELOPMENTONLY, "How much time offset from what client is asking before the data is deleted.");


IMPLEMENT_AUTO_LIST( IPaintStreamAutoList );

IMPLEMENT_SERVERCLASS_ST( CPaintStream, DT_PaintStream )

	SendPropInt( SENDINFO( m_nPaintType ) ),
	SendPropInt( SENDINFO( m_nRenderMode ) ),
	SendPropVector( SENDINFO( m_vLightPosition ) ),
	SendPropInt( SENDINFO( m_nMaxBlobCount ) ),

END_SEND_TABLE()

BEGIN_DATADESC( CPaintStream )

	DEFINE_FIELD( m_nBlobCounter, FIELD_INTEGER ),
	DEFINE_FIELD( m_nPaintType, FIELD_INTEGER ),
	DEFINE_FIELD( m_nRenderMode, FIELD_INTEGER ),
	DEFINE_FIELD( m_vLightPosition, FIELD_VECTOR ),
	DEFINE_FIELD( m_nMaxBlobCount, FIELD_INTEGER ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( paint_stream, CPaintStream );


CPaintStream::CPaintStream( void ) : m_nBlobCounter( 0 ),
									m_nRenderMode( BLOB_RENDER_BLOBULATOR ),
									m_nPaintType( NO_POWER ),
									m_nMaxBlobCount( 0 )
{
	m_vLightPosition = vec3_origin;
}


CPaintStream::~CPaintStream( void )
{
	RemoveAllPaintBlobs();

	m_sharedBlobDataMutex->Lock();
	m_sharedBlobData.GetForModify().Purge();
	m_sharedBlobDataMutex->Unlock();
}


void CPaintStream::Precache( void )
{
	PrecacheMaterial( PaintStreamManager.GetPaintMaterialName( m_nPaintType ) );

	BaseClass::Precache();
}


void CPaintStream::Spawn( void )
{
	Precache();

	AddEffects( EF_NOSHADOW | EF_NORECEIVESHADOW );

	BaseClass::Spawn();
}


void CPaintStream::Init( const Vector& vLightPosition, PaintPowerType paintType, BlobRenderMode_t blobRenderMode, int nMaxBlobCount )
{
	m_vLightPosition = vLightPosition;
	m_nPaintType = paintType;
	m_nRenderMode = blobRenderMode;
	m_nMaxBlobCount = nMaxBlobCount;
}


int CPaintStream::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}


void CPaintStream::AddPaintBlob( CPaintBlob *pBlob )
{
	if ( !pBlob )
		return;

	m_blobs.AddToTail( pBlob );
	pBlob->AssignBlobID( entindex() + m_nBlobCounter );
	++m_nBlobCounter;
}


void CPaintStream::UpdateRenderBoundsAndOriginWorldspace( void )
{
	if( m_blobs.Count() == 0 )
	{
		m_vCachedRenderOrigin = m_vCachedWorldMins = m_vCachedWorldMaxs = Vector(0,0,0);
		return;
	}

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
}


static int SortBlobDataByAscendingID( const BlobData_t* a, const BlobData_t* b )
{
	return a->m_blobID - b->m_blobID;
}


void CPaintStream::UpdateBlobSharedData()
{
	AUTO_LOCK_FM( m_sharedBlobDataMutex );

	int lastIndex = m_sharedBlobData.Count();
	m_sharedBlobData.GetForModify().AddToTail();
	m_sharedBlobData.GetForModify( lastIndex ).m_flUpdateTime = gpGlobals->curtime;

	const int nBlobCount = m_blobs.Count();

	BlobDataVector_t &blobDataVector = m_sharedBlobData.GetForModify( lastIndex ).m_blobData;
	blobDataVector.SetCount( nBlobCount );
	int nSilentBlob = 0;
	for ( int i=0; i<nBlobCount; ++i )
	{
		CPaintBlob *pBlob = assert_cast< CPaintBlob* >( m_blobs[i] );
		if ( pBlob->IsSilent() )
		{
			++nSilentBlob;
			continue;
		}

		bool bTeleportThisFrame = pBlob->HasBlobTeleportedThisFrame();

		// add blob data to the frame
		{
			BlobData_t &blobData = blobDataVector[i];
			blobData.m_blobID = pBlob->GetBlobID();
			blobData.m_flScale = pBlob->GetRadiusScale();
			blobData.m_vPosition = pBlob->GetPosition();
			/*
#ifdef PORTAL2
			// ghosting data
			blobData.m_bGhosting = pBlob->IsGhosting();
			if ( blobData.m_bGhosting )
			{
				pBlob->GetGhostMatrix( blobData.m_matGhostTransform );
			}
#endif
			*/

			// teleportation data
			pBlob->GetTeleportationHistory( blobData.m_teleportationHistory );
			blobData.m_bTeleportedThisFrame = bTeleportThisFrame;
		}

		// clear teleportation history
		if ( bTeleportThisFrame )
		{
			pBlob->ClearTeleportationHistory();
		}
	}
	blobDataVector.Sort( SortBlobDataByAscendingID );
	blobDataVector.RemoveMultipleFromHead( nSilentBlob );

	
	// remove data that's TOO OLD!
	const float flShouldRemoveTime = gpGlobals->curtime - paintblob_old_data_time_offset.GetFloat();
	int nToRemove = 0;
	for ( int i=0; i< m_sharedBlobData.Count(); ++i )
	{
		if ( m_sharedBlobData.GetForModify(i).m_flUpdateTime < flShouldRemoveTime )
		{
			++nToRemove;
		}
		else
		{
			break;
		}
	}

	if ( nToRemove )
	{
		m_sharedBlobData.GetForModify().RemoveMultipleFromHead( nToRemove );
	}
}


void CPaintStream::DebugDrawBlobs()
{
	bool bDebugDraw = false;
	Color debugColor;

	if( draw_paint_server_blobs.GetBool() )
	{
		bDebugDraw = true;
		debugColor = Color( 255, 0, 255 );
	}

	if ( bDebugDraw )
	{
		for ( int i=0; i<m_blobs.Count(); ++i )
		{
			NDebugOverlay::Cross3D( m_blobs[i]->GetPosition(), 2.0f, debugColor.r(), debugColor.g(), debugColor.b(), true, 0.f );
		}
	}
}


void CPaintStream::AddPaintToDatabase() const
{
	for ( int i=0; i<m_blobs.Count(); ++i )
	{
		CPaintBlob *pBlob = assert_cast< CPaintBlob* >( m_blobs[i] );
		pBlob->AddToPaintDatabase();
	}
}
