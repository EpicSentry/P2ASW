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


//#include "../../../common/blobulator/implicit/impdefines.h"
//#include "../../../common/blobulator/implicit/imprenderer.h"
//#include "../../../common/blobulator/implicit/imptiler.h"
//#include "../../../common/blobulator/implicit/userfunctions.h"

//class C_PaintblobRenderer
namespace NPaintRenderer
{

typedef CUtlVector< matrix3x4_t > PortalMatrixList_t;

void Paintblob_CullOutOfViewParticles(/*ImpParticleList& particleList*/);
void Paintblob_Draw( int renderMode, IMaterial *pMaterial, float flCubeWidth, const PortalMatrixList_t& portalMatrixList/*, ImpParticleList& particleList*/ );

}

#endif // C_PAINTBLOB_RENDER_H
