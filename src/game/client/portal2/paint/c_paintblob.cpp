//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"
#include "model_types.h"

#include "c_paintblob.h"
#include "c_prop_paint_bomb.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

C_PaintBlob::C_PaintBlob()
{
}


C_PaintBlob::~C_PaintBlob()
{
#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
	if (m_hRenderable && m_hRenderable.Get())
	{
		delete m_hRenderable.Get();
		m_hRenderable = NULL;
	}
#endif
}


void C_PaintBlob::PaintBlobPaint( const trace_t &tr )
{
	if ( m_bDrawOnly )
		return;

	Vector vecTouchPos = tr.endpos;
	Vector vecNormal = tr.plane.normal;

	PlayEffect( vecTouchPos, vecNormal );
	
}

void C_PaintBlob::Init( const Vector &vecOrigin, const Vector &vecVelocity, int paintType, float flMaxStreakTime, float flStreakSpeedDampenRate, CBaseEntity* pOwner, bool bSilent, bool bDrawOnly )
{
	CBasePaintBlob::Init( vecOrigin, vecVelocity, paintType, flMaxStreakTime, flStreakSpeedDampenRate, pOwner, bSilent, bDrawOnly );

#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
	C_PaintBlobRenderable *pRenderable = new C_PaintBlobRenderable( this, 0.75 );
		
	//cl_entitylist->AddNonNetworkableEntity( m_pRenderable->GetIClientUnknown() );
	//m_pRenderable->PrecacheModel( BLOB_MODEL );
	pRenderable->InitializeAsClientEntity( BLOB_MODEL, false );
	pRenderable->Spawn();

	m_hRenderable = pRenderable;
#endif
}

#if !defined ( USE_BLOBULATOR ) && !defined ( USE_PARTICLE_BLOBULATOR )
C_PaintBlobRenderable::C_PaintBlobRenderable( C_PropPaintBomb *pPaintBomb, float flModelScale )
{
	SetModelScale( flModelScale );
	m_pPaintBomb = pPaintBomb;
	m_pSourceBlob = NULL;
}

C_PaintBlobRenderable::C_PaintBlobRenderable( C_PaintBlob *pSourceBlob, float flModelScale )
{
	SetModelScale( flModelScale );
	m_pSourceBlob = pSourceBlob;
	m_pPaintBomb = NULL;
}

C_PaintBlobRenderable::~C_PaintBlobRenderable()
{
	//g_pClientLeafSystem->RemoveRenderable( RenderHandle() );
	cl_entitylist->RemoveEntity( GetIClientUnknown()->GetRefEHandle() );
	m_pSourceBlob = NULL;
	m_pPaintBomb = NULL;
}

void C_PaintBlobRenderable::Spawn( void )
{
	BaseClass::Spawn();
	Precache();

	SetModel(BLOB_MODEL);

	PaintPowerType powerType = NO_POWER;
	
	if ( m_pSourceBlob )
	{
		powerType = m_pSourceBlob->GetPaintPowerType();
	}
	else if ( m_pPaintBomb )
	{
		powerType = m_pPaintBomb->GetPaintPowerType();
	}

	
	if ( powerType != NO_POWER )
	{
		Color color = MapPowerToColor( powerType );

		SetRenderColorR( color.r() );
		SetRenderColorG( color.g() );
		SetRenderColorB( color.b() );
	}
	else
	{
		SetRenderMode( kRenderTransAlpha );
		SetRenderColorR( 128 );
		SetRenderColorG( 128 );
		SetRenderColorB( 128 );
		SetRenderAlpha( 160 );
	}

	g_pClientLeafSystem->DisableCachedRenderBounds( RenderHandle(), true );
}

void C_PaintBlobRenderable::Precache( void )
{
	PrecacheModel(BLOB_MODEL);
	BaseClass::Precache();
}

int C_PaintBlobRenderable::DrawModel( int flags, const RenderableInstance_t &instance )
{	
#if 0
	bool bOverride = false;
	if ( ( flags & STUDIO_RENDER ) )
	{
		//for ( int i = 0; i < GetSkin(); i++ )
		{
		//	if ( IsCustomMaterialValid( i ) )
			{
	
				IMaterial *pBlobMaterial = materials->FindMaterial( "materials/lights/white001", TEXTURE_GROUP_MODEL, true );
				modelrender->ForcedMaterialOverride( pBlobMaterial, OVERRIDE_NORMAL );
				bOverride = true;
			}
		}
	}

	if ( bOverride )
	{
	//	modelrender->ForcedMaterialOverride( NULL );
	}
#endif
	return BaseClass::DrawModel( flags, instance );
}

Vector const& C_PaintBlobRenderable::GetRenderOrigin( void )
{
	if ( m_pSourceBlob )
	{
		return m_pSourceBlob->GetPosition();
	}
	else if (m_pPaintBomb)
	{
		return m_pPaintBomb->GetAbsOrigin();
	}

	return vec3_origin;
}

QAngle const& C_PaintBlobRenderable::GetRenderAngles( void )
{
	return BaseClass::GetRenderAngles();
}

void C_PaintBlobRenderable::PerFrameUpdate( void )
{	
	//SetEffects( EF_NOINTERP );		

	// Set position and angles relative to the object it's ghosting
	Vector ptNewOrigin;
	if ( m_pSourceBlob )
		ptNewOrigin = m_pSourceBlob->GetPosition();
	else if ( m_pPaintBomb )
		ptNewOrigin = m_pPaintBomb->GetAbsOrigin();

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

#endif // USE_BLOBULATOR