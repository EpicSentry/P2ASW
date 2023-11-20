//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef PAINT_DATABASE_H
#define PAINT_DATABASE_H

#include "paint/paint_color_manager.h"

#ifdef PORTAL2
class CWeaponPaintGun;
class CProjectedWallEntity;
#endif


struct PaintLocationData_t
{
	CBaseHandle		hBrushEntity;
	PaintPowerType	type;
	float			flPaintRadius;
	float			flPaintAlphaPercent;
	Vector			location;
	Vector			normal;
};

struct PaintEntityData_t
{
	CBaseHandle		hEnt;
	//CWeaponPaintGun *pPaintGun;
};

typedef PaintLocationData_t* PaintLocationIter;
typedef PaintLocationData_t const* PaintLocationConstIter;

typedef CUtlVector<PaintLocationData_t> PaintLocationVector_t;
typedef CUtlVector<PaintEntityData_t> PaintEntityVector_t;
#ifdef PORTAL2
typedef CUtlVector<CProjectedWallEntity*> ProjectedWallVector_t;
#endif
typedef CUtlVector< uint32 > PaintmapData_t;
struct PaintRestoreData_t
{
	PaintmapData_t m_PaintmapData;
};

class CPaintDatabase : public CAutoGameSystemPerFrame, public CGameEventListener
{
public:
	CPaintDatabase( char const *name );
	~CPaintDatabase();

	//CAutoGameSystem members
	virtual char const *Name() { return "PaintDatabase"; }
	virtual void LevelInitPreEntity();
	virtual void LevelInitPostEntity();
	virtual void LevelShutdownPostEntity();

	//CGameEventListener members
	virtual void FireGameEvent( IGameEvent *event );

	void AddPaint( CBaseEntity* pPaintedEntity, const Vector& vecPaintLocation, const Vector& vecNormal, PaintPowerType powerType, float flPaintRadius, float flAlphaPercent );
	void PaintEntity( CBaseEntity *pPaintedEntity, PaintPowerType powerType, const Vector &vecPosition );

	void RemovePaintedEntity( const CBaseEntity *pPaintedEntity );
	void RemovePaintedEntity( const CBaseEntity *pPaintedEntity, bool bDeleteData );
#ifdef PORTAL2
	void RemovePaintedWall( CProjectedWallEntity *pWall, bool bDeleteData = true );
#endif

	void RemoveAllPaint();

	virtual void PreClientUpdate();

#ifdef PORTAL2
		const ProjectedWallVector_t* GetPaintedProjectedWalls() const { return &m_PaintedProjectedWalls; }
#endif

	void SavePaintmapData( ISave* pSave );
	void RestorePaintmapData( IRestore* pRestore );

	void SendPaintDataTo( CBasePlayer* pPlayer );

private:
#ifdef PORTAL2
		void PaintProjectedWall( CProjectedWallEntity *pWall, PaintPowerType powerType, const Vector &vecPosition );
#endif

	void ClearPaintData();
	void RemovePaintedEntity( int index, bool bDeleteData );
	int FindPaintedEntity( const CBaseEntity *pPaintedEntity ) const;

	void SendPaintDataToEngine();

	PaintLocationVector_t m_PaintThisFrame;
	PaintEntityVector_t m_PaintedEntities;

#ifdef PORTAL2
		ProjectedWallVector_t m_PaintedProjectedWalls;
#endif

	bool m_bCanPaint;

	PaintRestoreData_t m_PaintRestoreData;
};

extern CPaintDatabase PaintDatabase;

extern ConVar sv_paint_surface_sphere_radius;
extern ConVar sv_erase_surface_sphere_radius;
extern ConVar sv_paint_alpha_coat;

#endif //PAINT_DATABASE_H
