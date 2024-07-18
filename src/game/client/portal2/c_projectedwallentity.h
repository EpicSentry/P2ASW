#ifndef C_PROJECTED_WALL_ENTITY_H
#define C_PROJECTED_WALL_ENTITY_H

#include "cbase.h"
#include "c_baseprojectedentity.h"

//#define NO_PROJECTED_WALL
#ifndef NO_PROJECTED_WALL

#define CProjectedWallEntity C_ProjectedWallEntity

DECLARE_AUTO_LIST( IProjectedWallEntityAutoList )

class C_ProjectedWallEntity : public CPaintableEntity<C_BaseProjectedEntity>, public IProjectedWallEntityAutoList
{
public:
	DECLARE_CLASS( C_ProjectedWallEntity, CPaintableEntity<C_BaseProjectedEntity> );

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	C_BaseEntity *GetEntity() { return this; }
	C_ProjectedWallEntity();
	
	virtual void Spawn();
	virtual void ClientThink();	
	virtual void UpdateOnRemove();
	
	virtual void Touch( C_BaseEntity *pOther );
	
	virtual CollideType_t GetCollideType();
	virtual int DrawModel( int flags, const RenderableInstance_t &instance );
	virtual bool ShouldDraw();
	virtual void GetRenderBounds( Vector& mins, Vector& maxs );
	virtual QAngle const& GetRenderAngles( void );
	
	virtual RenderableTranslucencyType_t ComputeTranslucencyType();
	virtual void ComputeWorldSpaceSurroundingBox( Vector *mins, Vector *maxs );
	
	virtual void OnPreDataChanged( DataUpdateType_t datatype );
	virtual void OnDataChanged( DataUpdateType_t datatype );
	virtual void PostDataUpdate( DataUpdateType_t datatype );
	
	virtual bool TestCollision( const Ray_t &ray, unsigned int mask, trace_t& trace );
	virtual bool TestHitboxes( const Ray_t &ray, unsigned int fContentsMask, trace_t& tr );
	
	virtual void OnProjected();
	virtual void GetProjectionExtents( Vector &, Vector & );
	virtual void GetExtents( Vector &outMins, Vector &outMaxs, float flWidthScale = 1.0 );
	virtual void ProjectWall();
	
	virtual PaintPowerType GetPaintPowerAtPoint( const Vector& worldContactPt ) const;
	virtual void Paint( PaintPowerType type, const Vector& worldContactPt );
	virtual void CleansePaint();
	virtual void SetPaintPower(int ,PaintPowerType );
	virtual void GetToolRecordingState( KeyValues *msg );
	virtual void RestoreToToolRecordedState( KeyValues * );
	void DisplaceObstructingEntity( CBaseEntity *pEntity, bool bIgnoreStuck );
private:
	virtual bool InitMaterials();
	void DisplaceObstructingEntity( CBaseEntity *pEntity, const Vector &vOrigin, const Vector &vWallUp, const Vector &vWallRight, bool bIgnoreStuck );
	void DisplaceObstructingEntities();
	void ColorWallByPortal( IMaterial * );
	bool ShouldSpawnParticles( C_Portal_Base2D * );
	void SetupWallParticles();
	void CheckForPlayersOnBridge();
	void PaintWallSideRails( CMeshBuilder & , Vector & , Vector & , Vector & , Vector & ,float ,float );
	
	IMaterial *m_pBodyMaterial;
	IMaterial *m_pPaintMaterialsMid[4];
	IMaterial *m_pPaintMaterialsEnd1[4];
	IMaterial *m_pPaintMaterialsEnd2[4];
	IMaterial *m_pPaintMaterialsSing[4];
	IMaterial *m_pPaintMaterialBounceLSpeed;
	IMaterial *m_pPaintMaterialBounceRSpeed;
	IMaterial *m_pPaintMaterialBounceLRSpeed;
	IMaterial *m_pPaintMaterialRBounceLSpeed;
	IMaterial *m_pPaintMaterialLBounceRSpeed;
	IMaterialVar *m_pPaintColorMid;
	IMaterialVar *m_pPaintColorEnd1;
	IMaterialVar *m_pPaintColorEnd2;
	IMaterialVar *m_pPaintColorSing;
	IMaterial *m_pSideRailMaterial;
	Vector m_vWorldSpace_WallMins;
	Vector m_vWorldSpace_WallMaxs;
	struct WallCollideableAtTime_t
	{
		Vector vStart;
		Vector vEnd;
		Vector vWorldMins;
		Vector vWorldMaxs;
		QAngle qAngles;
		float flTime;
		CPhysCollide *pCollideable;
		WallCollideableAtTime_t();
	};
	CUtlVector<WallCollideableAtTime_t> m_WallCollideables;
	CPhysCollide *m_pActiveCollideable;
	float m_flLength;
	float m_flWidth;
	float m_flHeight;
	bool m_bIsHorizontal;
	CHandle<C_BaseEntity> m_hColorPortal;
	float m_flCurDisplayLength;
	Vector m_vLastStartpos;
	bool SetPaintSurfaceColor(enum PaintPowerType & );
	int m_nNumSegments;
	float m_flSegmentLength;
	CUtlVector<PaintPowerType> m_PaintPowers;
	float m_flParticleUpdateTime;
	float m_flPrevParticleUpdateTime;
	int ComputeSegmentIndex( const Vector& vWorldPositionOnWall ) const;
};

#endif // NO_PROJECTED_WALL

#endif // C_PROJECTED_WALL_ENTITY_H