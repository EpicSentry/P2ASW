//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#ifndef C_PAINTBLOB_H
#define C_PAINTBLOB_H

#include "paint/paint_blobs_shared.h"

class CPaintBlob : public CBasePaintBlob
{
public:
	CPaintBlob();
	~CPaintBlob();

	void AddToPaintDatabase();

	void AssignBlobID( int id ) { m_ID = id; }
	int GetBlobID() { return m_ID; }

	void AddBlobTeleportationHistory( const BlobTeleportationHistory_t& history );
	void ClearTeleportationHistory();
	void GetTeleportationHistory( BlobTeleportationHistoryVector_t& historyList );

	virtual void PaintBlobPaint( const trace_t &tr );

private:

	void SetupPaintInfo( const trace_t& tr );

	// paint
	CBaseEntity *m_pPaintingEntity;
	unsigned short m_nSurfaceFlags;
	int m_ID;
	bool m_bTeleportedThisFrame;
	BlobTeleportationHistoryVector_t m_teleportationHistory;
};

#endif
