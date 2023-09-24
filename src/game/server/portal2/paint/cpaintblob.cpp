//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"

#include "cpaintblob.h"
#include "paint_database.h"
#include "physics_prop_ragdoll.h"
#include "world.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_paintblob_damage( "sv_paintblob_damage", "0.1", FCVAR_CHEAT );

ConVar paintblob_applies_impulse( "paintblob_applies_impulse", "0", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT, "Should paint blobs apply an impulse when they impact physics objects?" );

CPaintBlob::CPaintBlob() : m_pPaintingEntity(NULL), m_ID( 0 )
{
}


CPaintBlob::~CPaintBlob()
{
	m_teleportationHistory.Purge();
}

void CPaintBlob::AddToPaintDatabase()
{
	if ( m_pPaintingEntity )
	{
		const float flPaintRadius = ( m_paintType == NO_POWER ) ? sv_erase_surface_sphere_radius.GetFloat() : sv_paint_surface_sphere_radius.GetFloat();
		PaintDatabase.AddPaint( m_pPaintingEntity, m_vecPosition, m_vContactNormal, m_paintType, flPaintRadius, sv_paint_alpha_coat.GetFloat() );

		m_pPaintingEntity = NULL;
	}
}

void CPaintBlob::SetupPaintInfo( const trace_t& tr )
{
	m_pPaintingEntity = tr.m_pEnt;

	SetPosition( tr.endpos );
	m_vContactNormal = tr.plane.normal;
	m_nSurfaceFlags = tr.surface.flags;
}


Vector GetPaintBlobDamage( const Vector &vecDir )
{
	float flForceScale = sv_paintblob_damage.GetFloat() * ImpulseScale( 75, 4 );
	Vector vecForce = vecDir;
	VectorNormalize( vecForce );
	vecForce *= flForceScale;
	vecForce *= phys_pushscale.GetFloat();

	return vecForce;
}


void CPaintBlob::PaintBlobPaint( const trace_t &tr )
{
	if ( m_bDrawOnly )
		return;

	Vector vecTouchPos = tr.endpos;
	Vector vecNormal = tr.plane.normal;

	PlayEffect( vecTouchPos, vecNormal );

	// Setup paint info to send to PaintDatabase later
	SetupPaintInfo( tr );

	if( paintblob_applies_impulse.GetBool() )
	{
		CBaseEntity *pOther = tr.m_pEnt;

		// push object
		if ( !pOther->IsPlayer() )
		{
			IPhysicsObject *pPhysics = pOther->VPhysicsGetObject();
			if ( pPhysics )
			{
				Vector vVel = GetPaintBlobDamage( m_vecVelocity.Normalized() ) / 10.f;
				Vector aVel = vVel;

				// Affect the object
				CRagdollProp *pRagdoll = dynamic_cast<CRagdollProp*>( pOther );
				if ( !pRagdoll )
				{
					pPhysics->AddVelocity( &vVel, &aVel );
				}
				else
				{
					ragdoll_t *pRagdollPhys = pRagdoll->GetRagdoll( );
					for ( int j = 0; j < pRagdollPhys->listCount; ++j )
					{
						pRagdollPhys->list[j].pObject->AddVelocity( &vVel, &aVel ); 
					}
				}
			}
		}
	}
}


void CPaintBlob::AddBlobTeleportationHistory( const BlobTeleportationHistory_t& history )
{
	m_teleportationHistory.AddToTail( history );
}


void CPaintBlob::ClearTeleportationHistory()
{
	m_teleportationHistory.RemoveAll();
}


void CPaintBlob::GetTeleportationHistory( BlobTeleportationHistoryVector_t& historyList )
{
	int nHistoryCount = m_teleportationHistory.Count();
	if ( nHistoryCount )
	{
		historyList.AddMultipleToTail( nHistoryCount, m_teleportationHistory.Base() );
	}
}
