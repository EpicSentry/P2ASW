//========= Copyright 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_PAINTBLOB_RENDER_H
#define C_PAINTBLOB_RENDER_H

#if defined( _WIN32 )
#pragma once
#endif

#ifdef USE_BLOBULATOR
#include "../../../common/blobulator/implicit/impdefines.h"
#include "../../../common/blobulator/implicit/imprenderer.h"
#include "../../../common/blobulator/implicit/imptiler.h"
#include "../../../common/blobulator/implicit/userfunctions.h"
#endif

#include "particles/particles.h"


typedef CUtlVector< matrix3x4_t > PortalMatrixList_t;

//class C_PaintblobRenderer
namespace NPaintRenderer
{

#if defined ( USE_BLOBULATOR ) 
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList, ImpParticleList& particleList );
#elif defined ( USE_PARTICLE_BLOBULATOR )
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList );
#endif


}

// Recreated from CSGO leak and with the use of IDA
// This is needed so we can hack our way to creating a blobulator effect
class C_OP_RenderBlobs : public CParticleRenderOperatorInstance
{
	DECLARE_PARTICLE_OPERATOR( C_OP_RenderBlobs );

	virtual uint64 GetReadControlPointMask() const;

	size_t GetRequiredContextBytes( void ) const;

	virtual void InitializeContextData( CParticleCollection *pParticles, void *pContext ) const;

	uint32 GetWrittenAttributes( void ) const;

	uint32 GetReadAttributes( void ) const;

	virtual void Render( IMatRenderContext *pRenderContext, CParticleCollection *pParticles, const Vector4D &vecDiffuseModulation, void *pContext ) const;
	
	virtual bool IsBatchable() const;

	friend class C_OP_RenderBlobs_Access;
};

#endif // C_PAINTBLOB_RENDER_H
