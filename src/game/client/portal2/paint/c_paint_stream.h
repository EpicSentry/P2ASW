//========= Copyright ?Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#ifndef C_PAINT_STREAM_H
#define C_PAINT_STREAM_H

#include "paint/paint_sprayer_shared.h"

typedef float TimeStamp;


DECLARE_AUTO_LIST( IPaintStreamAutoList );


class C_PaintStream : public C_BaseAnimating, public IPaintStreamAutoList
{
public:
	DECLARE_CLASS( C_PaintStream, C_BaseAnimating );
	DECLARE_CLIENTCLASS();
	DECLARE_SHAREDCLASS();

	// IPaintStreamAutoList
	IMPLEMENT_AUTO_LIST_GET();

	C_PaintStream();
	~C_PaintStream();

	virtual void Spawn( void );

	virtual const Vector&	GetRenderOrigin( void );
	virtual void			GetRenderBoundsWorldspace( Vector& absMins, Vector& absMaxs );
	virtual void			GetRenderBounds( Vector& theMins, Vector& theMaxs );

	// TODO: This sucks.... I should use a renderCenter variable.
	virtual const Vector&	WorldSpaceCenter( void );
	virtual void			ComputeWorldSpaceSurroundingBox( Vector *pMins, Vector *pMaxs );

	virtual int				DrawModel( int flags, const RenderableInstance_t &instance );
	virtual bool			ShouldDraw() { return true; }

	virtual void			UpdateOnRemove();

	// Needed to sort properly the transparent paint
	virtual RenderableTranslucencyType_t ComputeTranslucencyType( );

	//Paint blob management
	void AddPaintBlob( C_PaintBlob *pBlob );
	void RemoveAllPaintBlobs( void );

	const Vector&	WorldAlignMins( void ) const;
	const Vector&	WorldAlignMaxs( void ) const;

	void UpdateRenderBoundsAndOriginWorldspace( void );

	unsigned int GetBlobsCount() const;
	C_PaintBlob* GetBlob( int id );
	const PaintBlobVector_t& GetBlobList() const { return m_blobs; }

	void PreUpdateBlobs();
	void PostUpdateBlobs();

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

	PaintBlobVector_t m_blobs;

	// listen server shared data
	void GetInterpolatedBlobData( BlobDataVector_t& blobInterpolatedData );
	BlobDataVector_t m_blobInterpolatedData;
	CSharedUtlVector( BlobInterpolationData_t, m_sharedBlobData );
	CSharedVar( CThreadFastMutex, m_sharedBlobDataMutex );

	typedef CUtlVector<TimeStamp> TimeStampVector;
	TimeStampVector m_UsedChannelTimestamps;

	CNetworkVar( PaintPowerType, m_nPaintType );
	CNetworkVar( BlobRenderMode_t, m_nRenderMode );
	CNetworkVector( m_vLightPosition );
	CNetworkVar( int, m_nMaxBlobCount );

	// client only
	void DrawBlobs( IMaterial* pMaterial );
};

typedef C_PaintStream CPaintStream;

#endif //C_PAINT_STREAM_H
