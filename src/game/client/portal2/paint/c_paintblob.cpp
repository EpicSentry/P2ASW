//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"

#include "c_paintblob.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

C_PaintBlob::C_PaintBlob()
{
	m_pRenderable = new C_PaintBlobRenderable( this );
		
	//cl_entitylist->AddNonNetworkableEntity( m_pRenderable->GetIClientUnknown() );
	//m_pRenderable->PrecacheModel( BLOB_MODEL );
	m_pRenderable->InitializeAsClientEntity( BLOB_MODEL, false );
	m_pRenderable->Spawn();
}


C_PaintBlob::~C_PaintBlob()
{
	delete m_pRenderable;
}


void C_PaintBlob::PaintBlobPaint( const trace_t &tr )
{
	if ( m_bDrawOnly )
		return;

	Vector vecTouchPos = tr.endpos;
	Vector vecNormal = tr.plane.normal;

	PlayEffect( vecTouchPos, vecNormal );
	
}


C_PaintBlobRenderable::C_PaintBlobRenderable( C_PaintBlob *pSourceBlob )
{
	m_pSourceBlob = pSourceBlob;
}

C_PaintBlobRenderable::~C_PaintBlobRenderable()
{
	//g_pClientLeafSystem->RemoveRenderable( RenderHandle() );
	cl_entitylist->RemoveEntity( GetIClientUnknown()->GetRefEHandle() );
	m_pSourceBlob = NULL;
}

void C_PaintBlobRenderable::Precache( void )
{
	PrecacheModel(BLOB_MODEL);
	BaseClass::Precache();
}

void C_PaintBlobRenderable::Spawn( void )
{
	BaseClass::Spawn();
	Precache();

	SetModel(BLOB_MODEL);

	g_pClientLeafSystem->DisableCachedRenderBounds( RenderHandle(), true );
}

Vector const& C_PaintBlobRenderable::GetRenderOrigin( void )
{
	return m_pSourceBlob->GetPosition();
}

QAngle const& C_PaintBlobRenderable::GetRenderAngles( void )
{
	return BaseClass::GetRenderAngles();
}

void C_PaintBlobRenderable::PerFrameUpdate( void )
{	
	//SetEffects( EF_NOINTERP );		

	// Set position and angles relative to the object it's ghosting
	Vector ptNewOrigin = m_pSourceBlob->GetPosition();

	SetNetworkOrigin( ptNewOrigin );
	SetLocalOrigin( ptNewOrigin );
	SetAbsOrigin( ptNewOrigin );

	//Warning("%f %f %f\n", ptNewOrigin.x, ptNewOrigin.y, ptNewOrigin.z);
	
	g_pClientLeafSystem->RenderableChanged( RenderHandle() );
}


void C_PaintBlobRenderable::GetRenderBounds( Vector& mins, Vector& maxs )
{
	
	mins = GetRenderOrigin() + Vector(-16, -16, -16);
	maxs = GetRenderOrigin() + Vector(16, 16, 16);
	
	
	BaseClass::GetRenderBounds( mins, maxs );
	
}

void C_PaintBlobRenderable::GetRenderBoundsWorldspace( Vector& mins, Vector& maxs )
{
	
	mins = GetRenderOrigin() + Vector(-16, -16, -16);
	maxs = GetRenderOrigin() + Vector(16, 16, 16);
	
	//BaseClass::GetRenderBoundsWorldspace( mins, maxs );
}