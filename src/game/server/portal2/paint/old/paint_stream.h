//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#ifndef PAINT_STREAM_H
#define PAINT_STREAM_H

#include "paint/paint_sprayer_shared.h"

typedef float TimeStamp;


DECLARE_AUTO_LIST( IPaintStreamAutoList );


class CPaintStream : public CBaseAnimating, public IPaintStreamAutoList
{
public:
	DECLARE_CLASS( CPaintStream, CBaseAnimating );

	DECLARE_SERVERCLASS();
	DECLARE_SHAREDCLASS();
	DECLARE_DATADESC();

	// IPaintStreamAutoList
	IMPLEMENT_AUTO_LIST_GET();

	CPaintStream();
	~CPaintStream();

	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void UpdateOnRemove();
	virtual int UpdateTransmitState( void );


	//Paint blob management
	void AddPaintBlob( CPaintBlob *pBlob );
	void RemoveAllPaintBlobs( void );

	const Vector&	WorldAlignMins( void ) const;
	const Vector&	WorldAlignMaxs( void ) const;

	void UpdateRenderBoundsAndOriginWorldspace( void );

	unsigned int GetBlobsCount() const;
	CPaintBlob* GetBlob( int id );
	const PaintBlobVector_t& GetBlobList() const { return m_blobs; }

	void PreUpdateBlobs();
	void PostUpdateBlobs();

	void Init( const Vector& vLightPosition, PaintPowerType paintType, BlobRenderMode_t blobRenderMode, int nMaxBlobCount );
private:
	// debug draw blobs
	void DebugDrawBlobs();

	// clean up dead blobs
	void RemoveDeadBlobs();

	void QueuePaintEffect();

	void RemoveTeleportedThisFrameBlobs();
	void ResetBlobsTeleportedThisFrame();

	Vector	m_vCachedRenderOrigin;
	Vector	m_vCachedWorldMins;
	Vector	m_vCachedWorldMaxs;

	int m_nBlobCounter; // also use this as blob's ID
	PaintBlobVector_t m_blobs;

	//Listen server only!
	CSharedUtlVector( BlobInterpolationData_t, m_sharedBlobData );
	CSharedVar( CThreadFastMutex, m_sharedBlobDataMutex );

	typedef CUtlVector<TimeStamp> TimeStampVector;
	TimeStampVector m_UsedChannelTimestamps;

	CNetworkVar( PaintPowerType, m_nPaintType );
	CNetworkVar( BlobRenderMode_t, m_nRenderMode );
	CNetworkVector( m_vLightPosition );
	CNetworkVar( int, m_nMaxBlobCount );

	// server only
	void AddPaintToDatabase() const;
	void UpdateBlobSharedData();
};

#endif
