//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef C_PROP_PAINT_BOMB_H
#define C_PROP_PAINT_BOMB_H

#include "c_physicsprop.h"

struct PaintBombBlob_t
{
	Vector vPosition;
	Vector vVelocity;
	float flRadius;
};

typedef CUtlVector< PaintBombBlob_t > PaintBombBlobVector_t;

#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
class C_PaintBlobRenderable;
#endif

class C_PropPaintBomb : public C_PhysicsProp
{
public:
	DECLARE_CLASS( C_PropPaintBomb, C_PhysicsProp );
	DECLARE_CLIENTCLASS();
	C_PropPaintBomb( void );
	~C_PropPaintBomb( void );

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void UpdateOnRemove( void );

	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void PostDataUpdate( DataUpdateType_t updateType );

	virtual bool ShouldDraw( void ) { return true; }
	virtual int DrawModel( int flags, const RenderableInstance_t &instance );

	virtual void ClientThink( void );

#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
	virtual bool Simulate();
#endif

	// Needed to sort properly the transparent paint
	virtual RenderableTranslucencyType_t ComputeTranslucencyType( );

	PaintPowerType GetPaintPowerType( void ) { return m_nPaintPowerType; }
		
private:

	void UpdateBlobs( float flDeltaTime );
	void CreatePaintBombPaintBlobs( void );
	void CleansePaintPower( void );

	PaintPowerType m_nPaintPowerType;
	PaintPowerType m_nPrevPaintPowerType;

	PaintBombBlobVector_t m_blobs;

	Vector m_vPrevPosition;
	float m_flAccumulatedTime;
	float m_flLastUpdateTime;
#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
	C_PaintBlobRenderable *m_pRenderable;
#endif
};

#endif	//C_PROP_PAINT_BOMB_H
