//========= Copyright 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "c_paintblob_render.h"
#ifdef USE_BLOBULATOR
#include "blobulator/iblob_renderer.h"
#endif

#include "view.h"
#include "viewrender.h"
#include "engine/ivdebugoverlay.h"
#include "vgui_int.h"
#include "gamerules.h"
#include "igamesystem.h"
#include "paint/paint_stream_shared.h"
#include "particles/particles.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar r_paintblob_display_clip_box("r_paintblob_display_clip_box", "0", FCVAR_NONE );

ConVar r_paintblob_blr_render_radius( "r_paintblob_blr_render_radius", "1.3", FCVAR_NONE, "Set render radius (how far from particle center surface will be)" );
ConVar r_paintblob_blr_cutoff_radius( "r_paintblob_blr_cutoff_radius", "5.5", FCVAR_NONE, "Set cutoff radius (how far field extends from each particle)" );

#ifdef USE_PARTICLE_BLOBULATOR

CNewParticleEffect *g_pBlobParticles = NULL;

CParticleProperty g_BlobParticleProp;

class C_OP_RenderBlobs_Access // Use this class for when we need to call functions for C_OP_RenderBlobs
{
public:

	static void Render( C_OP_RenderBlobs *pBlobs, IMatRenderContext *pRenderContext, CParticleCollection *pParticles, const Vector4D &vecDiffuseModulation, void *pContext )
	{
		g_pBlobParticles->m_pRenderOp = pBlobs;
		pBlobs->Render( pRenderContext, pParticles, vecDiffuseModulation, pContext );
	}
};

static C_OP_RenderBlobs_Access s_ParticleBlobulatorHack();


class C_ParticleBlobulator_AutoGameSystemPerFrame : public CAutoGameSystemPerFrame
{
	virtual void LevelInitPostEntity( void )
	{
		Msg("Initiate blobs\n");
		Assert( g_pBlobParticles == NULL );
		// WHY TF DOES THIS CRASH!!?!??!
		g_pBlobParticles = g_BlobParticleProp.Create( "global_blob", PATTACH_ABSORIGIN );
		Assert( g_pBlobParticles );
	}
};

C_ParticleBlobulator_AutoGameSystemPerFrame g_ParticleBlobulatorGameSystem;

#endif

// No Blob rendering :(
#if defined ( USE_BLOBULATOR ) || defined ( USE_PARTICLE_BLOBULATOR )
namespace NPaintRenderer
{

#ifndef USE_PARTICLE_BLOBULATOR
// Note: This culling is too aggressive, we can see particles being removed when the blob is paused and we move the view
void Paintblob_CullOutOfViewParticles( ImpParticleList& particleList )
{
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
}
#endif

//#define ENABLE_DEBUG_BLOB	1
#ifdef ENABLE_DEBUG_BLOB
volatile int g_nNumberOfParticles = 100;
volatile float g_ParticleDistance = 400.0f;
volatile float g_ParticleFieldScale = 2.0f;
volatile float g_ParticleStep = (275.0f / 32.f);
volatile float g_ParticleStepSpeed = 0.1f;
volatile float g_fParticleOffsetX = -30.0f;
volatile float g_fParticleOffsetY = 0.0f;
volatile float g_fParticleOffsetZ = 0.0f;

uint m_z = 7, m_w = 13;

float g_SomeParticles[][4] =
{
	{ 104.861092, -424.683655, 210.949524, 0.886974 },
	{ -16.283091, -442.107758, 37.653053, 0.909765 },
	{ 100.589005, -404.668671, 111.165298, 0.779157 },
	{ -77.695587, -436.344421, 69.478424, 0.992477 },
	/* more in the middle */
	{ 230.510178, 193.598877, 230.630219, 0.859862 },
	{ -249.114609, -413.951477, 156.916718, 0.811316 },
	{ -287.072266, -473.137695, 148.126022, 0.724648 },
	{ -344.930939, -454.151489, 101.607201, 0.777266 },
	{ 216.871994, 189.054382, 207.136108, 0.977917 }
};

#ifndef _CERT
// From here: http://www.codeproject.com/KB/recipes/SimpleRNG.aspx
// To not worry about any potential licensing issue, this code is deactivated in CERT
// so we are effectively not shipping with this code.
static uint GetRandomUint()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + (m_w & 65535);
}

static double GetRandomDouble()
{
	// 0 <= u <= 2^32
	uint u = GetRandomUint();
	// The magic number below is 1/(2^32 + 2).
	// The result is strictly between 0 and 1.
	return (u + 1) * 2.328306435454494e-10;
}
#endif

void SetupParticles( ImpParticleList& particleList )
{
	g_ParticleStep += g_ParticleStepSpeed;
	if (g_ParticleStep > 32.0f)
	{
		g_ParticleStep = (275.0f / 32.f);
	}

	particleList.SetCount(0);

	m_z = 101;	// Reset the seeds every frame
	m_w = 97;

	for (int i = 0 ; i < g_nNumberOfParticles ; i++)
	{
		float x, y, z, fScale;
		float coord[4];
		for (int j = 0 ; j < 4 ; ++j)
		{
			coord[j] = GetRandomDouble();
		}
/*
		x = (coord[0] - 0.5f) * g_ParticleDistance;
		y = (coord[1] - 0.5f) * g_ParticleDistance;
		z = (coord[2] - 0.5f) * g_ParticleDistance + 200.0f;
		fScale = 1.0f + (coord[3] * g_ParticleFieldScale);
*/

/*
		x = (i * g_ParticleStep) - 550.0f;
		y = 0.0f;
		z = 200.f;
		fScale = 2.0f;
*/

/*
		x = -452.0f + g_fParticleOffsetX;		// + g_fParticleOffsetX + (coord[0] - 0.5f) * 400.0f;
		y = -442.0f + g_fParticleOffsetY;		// + g_fParticleOffsetY + (coord[1] - 0.5f) * 0.0f;
		z = 113.0f + g_fParticleOffsetZ;		// + g_fParticleOffsetZ + (coord[2] - 0.5f) * 0.0f;
		fScale = 0.5f;
*/
		x = g_SomeParticles[i][0];
		y = g_SomeParticles[i][1];
		z = g_SomeParticles[i][2];
		fScale = g_SomeParticles[i][3];

		ImpParticleWithFourInterpolants particle;
		particle.center.set(x, y, z, 0.0f);
		particle.setFieldScale(fScale);
		particleList.AddToTail(particle);
	}
}
#endif

#ifndef USE_PARTICLE_BLOBULATOR
void DisplayClipBox( const PortalMatrixList_t & portalMatrixList )
{
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
}

void DumpParticles( const ImpParticleList& particleList )
{
	int nSize = particleList.Count();
	DevMsg("{\n");
	for (int i = 0 ; i < nSize ; ++i)
	{
		const ImpParticle & particle = particleList[i];
		DevMsg("{ %f, %f, %f, %f },\n", particle.center[0], particle.center[1], particle.center[2], particle.scale);
	}
	DevMsg("};\n");
}
#endif

// Note that there is way too many parameters passed.
// TODO: Consider creating a structure to pass all these parameters.
#ifndef USE_PARTICLE_BLOBULATOR
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList, ImpParticleList& particleList )
#else
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList )
#endif
{
#ifndef USE_PARTICLE_BLOBULATOR
	if ( r_paintblob_display_clip_box.GetBool() )
	{
		DisplayClipBox( portalMatrixList );
	}
#endif

#ifdef ENABLE_DEBUG_BLOB
	SetupParticles();
#else
#ifndef USE_PARTICLE_BLOBULATOR
	// Reduce the complexity by first removing particles that won't visibly affect the view
	Paintblob_CullOutOfViewParticles( particleList );
#endif
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

	C_OP_RenderBlobs renderblobs;

	Vector4D vecDiffuseModulation( 1.0f, 1.0f, 1.0f, 1.0f );

	CParticleVisibilityData visData;
	visData.m_bUseVisibility = true;
	visData.m_flAlphaVisibility = 255;
	visData.m_flRadiusVisibility = 64;
	
	// Draw the blobs
	C_OP_RenderBlobs_Access::Render( &renderblobs, pRenderContext, g_pBlobParticles, vecDiffuseModulation, &visData );
	

#ifndef USE_PARTICLE_BLOBULATOR
	bool bUseBlobulator = renderMode == BLOB_RENDER_BLOBULATOR;

	Blobulator::BlobRenderInfo_t blobRenderInfo;
	blobRenderInfo.m_flCubeWidth = flCubeWidth;
	blobRenderInfo.m_flCutoffRadius = r_paintblob_blr_cutoff_radius.GetFloat();
	blobRenderInfo.m_flRenderRadius = r_paintblob_blr_render_radius.GetFloat();
	blobRenderInfo.m_flViewScale = flViewScale;
	blobRenderInfo.m_nViewID = CurrentViewID(); // We assume that the main view is 0 so we can use high res blobs

	Blobulator::RenderBlob( bUseBlobulator, pRenderContext, pMaterial, blobRenderInfo, portalMatrixList.Base(), portalMatrixList.Count(), particleList.Base(), particleList.Count() );
#endif // USE_PARTICLE_BLOBULATOR
}

#ifndef USE_PARTICLE_BLOBULATOR
// This gamesystem provides a location to call higher-level management functions in the blobulator:
class C_Blobulator_AutoGameSystemPerFrame : public CAutoGameSystemPerFrame
{
	virtual void Update( float frametime )
	{
		Blobulator::FrameUpdate( gpGlobals->framecount );
	}
};
C_Blobulator_AutoGameSystemPerFrame g_BlobulatorGameSystem;

#endif // USE_PARTICLE_BLOBULATOR

} // NPaintRenderer
#endif // #if 0
