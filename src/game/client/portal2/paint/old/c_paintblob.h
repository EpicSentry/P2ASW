//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#ifndef C_PAINTBLOB_H
#define C_PAINTBLOB_H

#include "paint/paint_blobs_shared.h"

class C_PaintBlob : public CBasePaintBlob
{
public:
	C_PaintBlob();
	~C_PaintBlob();

	virtual void PaintBlobPaint( const trace_t &tr );
private:
};

typedef C_PaintBlob CPaintBlob;


#endif
