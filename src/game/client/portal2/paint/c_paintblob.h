//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#ifndef C_PAINTBLOB_H
#define C_PAINTBLOB_H

#define MAX_BLOB_RENDERABLES 32

#include "paint/paint_blobs_shared.h"

class C_PaintBlobRenderable;

class C_PaintBlob : public CBasePaintBlob
{
public:
	C_PaintBlob();
	~C_PaintBlob();

	virtual void PaintBlobPaint( const trace_t &tr );

	virtual void Init( const Vector &vecOrigin, const Vector &vecVelocity, int paintType, float flMaxStreakTime, float flStreakSpeedDampenRate, CBaseEntity* pOwner, bool bSilent, bool bDrawOnly );
	
	C_PaintBlobRenderable *m_pRenderable;
};

//==========================================================================//
// Purpose: Create models that represents a blob's location
//==========================================================================//

class C_PaintBlobRenderable : public C_BaseAnimating
{
public:

	C_PaintBlobRenderable( C_PaintBlob *pSourceBlob );
	~C_PaintBlobRenderable();

	virtual void	Spawn( void );
	virtual void	Precache( void );
	
	virtual int DrawModel( int flags, const RenderableInstance_t &instance );

	// Data accessors
	virtual Vector const&			GetRenderOrigin( void );
	virtual QAngle const&			GetRenderAngles( void );
	
	// Returns the bounds relative to the origin (render bounds)
	virtual void	GetRenderBounds( Vector& mins, Vector& maxs );

	// returns the bounds as an AABB in worldspace
	virtual void	GetRenderBoundsWorldspace( Vector& mins, Vector& maxs );

	void PerFrameUpdate( void );
	
	virtual bool					ShouldDraw( void ) { return true; }

private:

	C_PaintBlob *m_pSourceBlob;

};

#endif
