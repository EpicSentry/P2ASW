//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef PAINT_STREAM_SHARED_H
#define PAINT_STREAM_SHARED_H

#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "paint/c_paint_stream.h"
#else
#include "paint/paint_stream.h"
#endif

#define PAINTBLOB_MAX_RADIUS 1.5f


#endif //PAINT_STREAM_SHARED_H
