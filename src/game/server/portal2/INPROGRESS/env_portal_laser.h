#ifndef ENV_PORTAL_LASER_H
#define ENV_PORTAL_LASER_H

#include "cbase.h"
#include "beam_shared.h"
#include "baseanimating.h"
#include "info_placement_helper.h"

class CPropWeightedCube;

DECLARE_AUTO_LIST( IPortalLaserAutoList );

class CPortalLaser : public CBaseAnimating, public IPortalLaserAutoList
{
public:

	DECLARE_CLASS( CPortalLaser, CBaseAnimating );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	
	CBaseEntity *GetEntity() { return this; }
	CPortalLaser( const CPortalLaser & );
	CPortalLaser();
    ~CPortalLaser();
    void Spawn();
    void Precache();
    void Activate();
    void UpdateOnRemove();
    bool CreateVPhysics();
    int UpdateTransmitState();
    Vector ClosestPointOnLineSegment( Vector &vPos );
    void InputTurnOn( inputdata_t &inputdata );
    void InputTurnOff( inputdata_t &inputdata );
    void InputToggle( inputdata_t &inputdata );
    CPortalLaser *m_pParentLaser;
    CPortalLaser *m_pChildLaser;
	
    struct PortalLaserInfo_t
    {
        Vector vecStart;
        Vector vecEnd;
        CUtlVector<CBaseEntity *,CUtlMemory<CBaseEntity *,int> > sortedEntList;
        //PortalLaserInfo_t(struct CPortalLaser::PortalLaserInfo_t & );
		PortalLaserInfo_t()
		{
			vecStart.Init();
			vecEnd.Init();
		}
		~PortalLaserInfo_t() {}
        
	};
	
	typedef CUtlVector<CPortalLaser::PortalLaserInfo_t, CUtlMemory<CPortalLaser::PortalLaserInfo_t, int> > PortalLaserInfoList_t;

private:
	
	CNetworkHandle( CPropWeightedCube, m_hReflector );
	
	CNetworkVector( m_vStartPoint );
	CNetworkVector( m_vEndPoint );
	
	CNetworkVar( bool, m_bLaserOn );
	CNetworkVar( bool, m_bIsLethal );
	CNetworkVar( bool, m_bIsAutoAiming );
	CNetworkVar( bool, m_bShouldSpark );
	CNetworkVar( bool, m_bUseParentDir );
	CNetworkQAngle( m_angParentAngles );
	
    QAngle m_angPortalExitAngles;
	
   //CBaseEntity *TraceLaser(bool ,class Vector & ,class Vector & ,float & ,class CGameTrace & ,class CUtlVector<CPortalLaser::PortalLaserInfo_t,CUtlMemory<CPortalLaser::PortalLaserInfo_t,int> > & ,class Vector * );
	CBaseEntity *TraceLaser( bool bIsFirstTrace, Vector &vecStart, Vector &vecDirection, float &flTotalBeamLength, trace_t &tr, PortalLaserInfoList_t &infoList, Vector *pVecAutoAimOffset );

    CBaseEntity *GetEntitiesAlongLaser(class Vector & ,class Vector & ,class Vector * ,class CUtlVector<CPortalLaser::PortalLaserInfo_t,CUtlMemory<CPortalLaser::PortalLaserInfo_t,int> > & ,bool );
    void DamageEntitiesAlongLaser( const PortalLaserInfoList_t &infoList , bool bAutoAim );
	Vector m_vecNearestSoundSource[MAX_PLAYERS];
    CBaseEntity *m_pSoundProxy[MAX_PLAYERS];
	CSoundPatch *m_pAmbientSound[MAX_PLAYERS];
    CInfoPlacementHelper * m_pPlacementHelper;
    int m_iLaserAttachment;
    string_t m_ModelName;
    bool m_bStartOff;
    bool m_bFromReflectedCube;
    bool m_bGlowInitialized;
    bool m_bAutoAimEnabled;
    bool m_bNoPlacementHelper;
	
    void RemoveChildLaser();
    void UpdateNextLaser( Vector &vecStart, Vector &vecDirection, CPropWeightedCube *pReflector );
    void FireLaser( Vector &vecStart, Vector &vecDirection, CPropWeightedCube *pReflector );
    void CreateHelperEntities();
    void CreateSoundProxies();
    void DamageEntity(class CBaseEntity * ,float );
    void TurnOn();
    void TurnOff();
    void TurnOnGlow();
    void TurnOffGlow();
    void TurnOffLaserSound();
    void StrikeThink();
    void BeamDamage( trace_t &tr );
    void UpdateSoundPosition( Vector &vecStart, Vector &vecEnd );
    void SetFromReflectedCube( bool bReflect );
	
    bool StrikeEntitiesAlongLaser(class Vector & ,class Vector & ,class Vector * );
    bool ShouldAutoAim( CBaseEntity *pTarget );
    bool IsOn();
    bool ReflectLaserFromEntity( CBaseEntity *pTarget );
    bool InPVS();
};

#endif // ENV_PORTAL_LASER_H
