//========= Copyright 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
// MAJOR OVERHAUL WARNING
// This code is essentially filler functions to cause other scripts to compile properly, nothing useful is happening here
// blobulator leak when
#include "cbase.h"

#include "c_paintblob_render.h"
//#include "blobulator/iblob_renderer.h"

#include "view.h"
#include "viewrender.h"
#include "engine/ivdebugoverlay.h"
#include "vgui_int.h"
#include "gamerules.h"
#include "igamesystem.h"
#include "paint/paint_stream_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar r_paintblob_display_clip_box("r_paintblob_display_clip_box", "0", FCVAR_NONE );

ConVar r_paintblob_blr_render_radius( "r_paintblob_blr_render_radius", "1.3", FCVAR_NONE, "Set render radius (how far from particle center surface will be)" );
ConVar r_paintblob_blr_cutoff_radius( "r_paintblob_blr_cutoff_radius", "5.5", FCVAR_NONE, "Set cutoff radius (how far field extends from each particle)" );

namespace NPaintRenderer
{

// Note: This culling is too aggressive, we can see particles being removed when the blob is paused and we move the view
void Paintblob_CullOutOfViewParticles( /*ImpParticleList& particleList*/ )
{
	/*
	int iParticlesRemoved = 0;

	for ( int i = 0; i < particleList.Count() - iParticlesRemoved; ++i )
	{
		ImpParticle* imp_particle = &(particleList[ i ]);
		Vector vCenter = Vector( imp_particle->center[ 0 ], imp_particle->center[ 1 ], imp_particle->center[ 2 ] );

		if ( R_CullSphere( view->GetFrustum(), 5, &vCenter, imp_particle->scale * 24.0f ) )
		{
			ImpParticle* last_particle = &(particleList[ particleList.Count() - iParticlesRemoved - 1 ]);
			*imp_particle = *last_particle;

			++iParticlesRemoved;
			--i;
		}
	}

	if ( iParticlesRemoved )
	{
		particleList.SetCountNonDestructively( particleList.Count() - iParticlesRemoved );
	}
	*/
}


//Debug blob code used to be here
//DEBUG BLOB

void DisplayClipBox( const PortalMatrixList_t & portalMatrixList )
{
	/*
	Vector backwardPortalMins( Blobulator::g_ExclusionXMin, Blobulator::g_ExclusionYMin, Blobulator::g_ExclusionZMin );
	Vector backwardPortalMaxs( Blobulator::g_ExclusionXMax, Blobulator::g_ExclusionYMax, Blobulator::g_ExclusionZMax );

	uint8 colorTable[4][4] =
	{
		{ 255, 0, 0	},
		{ 0, 255, 0	},
		{ 0, 0, 255 },
		{ 255, 255, 255 }
	};

	for ( int i=0; i<portalMatrixList.Count(); ++i )
	{
		Vector pos;
		QAngle ang;
		MatrixAngles( portalMatrixList[i], ang, pos );

		int nColor = i % 4;
		int r = colorTable[nColor][0];
		int g = colorTable[nColor][1];
		int b = colorTable[nColor][2];
		int a = colorTable[nColor][3];
		NDebugOverlay::BoxAngles( pos, backwardPortalMins, backwardPortalMaxs, ang, r, g, b, a, 0.1f );
	}
	*/
}

void DumpParticles(/* const ImpParticleList& particleList*/ )
{
	/*
	int nSize = particleList.Count();
	DevMsg("{\n");
	for (int i = 0 ; i < nSize ; ++i)
	{
		const ImpParticle & particle = particleList[i];
		DevMsg("{ %f, %f, %f, %f },\n", particle.center[0], particle.center[1], particle.center[2], particle.scale);
	}
	DevMsg("};\n");
	*/
}

// Note that there is way too many parameters passed.
// TODO: Consider creating a structure to pass all these parameters.
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList/*, ImpParticleList& particleList*/ )
{
	/*
	if ( r_paintblob_display_clip_box.GetBool() )
	{
		DisplayClipBox( portalMatrixList );
	}

#ifdef ENABLE_DEBUG_BLOB
	SetupParticles();
#else
	// Reduce the complexity by first removing particles that won't visibly affect the view
	Paintblob_CullOutOfViewParticles( particleList );
#endif

	float flViewScale = 1.f;
#ifdef PORTAL2
	if ( IsGameConsole() )
	{
		CGameRules * pRules = GameRules();
		if ( ( pRules != NULL ) && ( pRules->IsMultiplayer() ) )
		{
			// If we are in co-op, we have twice as much portals. Reduce the number of polys by 4. (1.6 ^ 3 == 4.096).
			flViewScale = 1.6;

			if ( VGui_IsSplitScreen() && !VGui_IsSplitScreenPIP() )
			{
				// Then if we are in splitscreen, we have to reduce even further. (1.26 ^ 3 == 2).
				// In that case we have 8 times less polys than normal.
				// We disable it in PIP as we prefer be a little bit slower but not change the resolution from one frame to the other.
				flViewScale = 1.26;
			}
		}
	}
#endif

	CMatRenderContextPtr pRenderContext( materials );
	bool bUseBlobulator = renderMode == BLOB_RENDER_BLOBULATOR;

	Blobulator::BlobRenderInfo_t blobRenderInfo;
	blobRenderInfo.m_flCubeWidth = flCubeWidth;
	blobRenderInfo.m_flCutoffRadius = r_paintblob_blr_cutoff_radius.GetFloat();
	blobRenderInfo.m_flRenderRadius = r_paintblob_blr_render_radius.GetFloat();
	blobRenderInfo.m_flViewScale = flViewScale;
	blobRenderInfo.m_nViewID = CurrentViewID(); // We assume that the main view is 0 so we can use high res blobs

	Blobulator::RenderBlob( bUseBlobulator, pRenderContext, pMaterial, blobRenderInfo, portalMatrixList.Base(), portalMatrixList.Count(), particleList.Base(), particleList.Count() );
	*/
}

// This gamesystem provides a location to call higher-level management functions in the blobulator:
class C_Blobulator_AutoGameSystemPerFrame : public CAutoGameSystemPerFrame
{
	virtual void Update( float frametime )
	{
		//Blobulator::FrameUpdate( gpGlobals->framecount );
	}
};
C_Blobulator_AutoGameSystemPerFrame g_BlobulatorGameSystem;

} // NPaintRenderer
