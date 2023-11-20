#pragma once
// ==== Emulsion Dll Patching Tools - Klaxon#3616 ============
// Use wisely! Can cause crashes if used incorrectly!
// ===========================================================

#include "cbase.h"
#include "dll_offset.h"

#ifdef EMULSION_DLL
#include "paint.h"
#endif

extern void PatchAll();

class CMatSysPatch {
public:
    
    // override the paint colors >:D (going goblin mode)
    void Patch();

protected:

    class MatSysOffsets {
    public:
#ifdef EMULSION_DLL
        Offset g_PaintColors = { "materialsystem.dll", 0x115ad4 };
#endif
    };

    MatSysOffsets m_Offsets = MatSysOffsets();
};
extern CMatSysPatch g_MatSysPatch;

class CEnginePatch {
public:

    void Patch();

protected:

    class EngineOffsets {
    public:

    };

    EngineOffsets m_Offsets = EngineOffsets();
};
extern CEnginePatch g_EnginePatch;