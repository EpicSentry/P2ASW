//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//
#ifndef TRIGGER_PORTAL_CLEANSER_H
#define TRIGGER_PORTAL_CLEANSER_H

#include "cbase.h"
#include "c_triggers.h"
#include "c_portal_player.h"
#include "c_weapon_portalgun.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
//#include "portal/c_weapon_physcannon.h"
#include "model_types.h"
#include "rumble_shared.h"
#include "c_triggers.h"
#include "proxyentity.h"

#define CTriggerPortalCleanser C_TriggerPortalCleanser

static char *g_pszPortalNonCleansable[] = 
{ 
	"func_door", 
	"func_door_rotating", 
	"prop_door_rotating",
	"func_tracktrain",
	"env_ghostanimating",
	"physicsshadowclone",
	"prop_energy_ball",
	NULL,
};

class ClientFizzlerMultiOriginSoundPlayer : public C_BaseEntity
{
	DECLARE_CLASS( ClientFizzlerMultiOriginSoundPlayer, C_BaseEntity );
	DECLARE_CLIENTCLASS();
public:
	ClientFizzlerMultiOriginSoundPlayer();
	~ClientFizzlerMultiOriginSoundPlayer();
	
	//bool GetSoundSpatialization(SpatializationInfo_t& info);
   
private:
	//bool GetSoundSpatializationFromFizzler(struct SpatializationInfo_t & ,class C_TriggerPortalCleanser * );
		
};

class C_FizzlerVortexProxy : public CEntityMaterialProxy
{
public:
    C_FizzlerVortexProxy();
    ~C_FizzlerVortexProxy();
	
	virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	virtual void OnBind( C_BaseEntity *pC_BaseEntity );
	virtual IMaterial  *GetMaterial() { return m_pMaterial; }
    
private:
    IMaterial  *m_pMaterial;
    IMaterialVar  *m_pPosition1Var;
    IMaterialVar  *m_pVortex1Var;
    IMaterialVar  *m_pPosition2Var;
    IMaterialVar  *m_pVortex2Var;
    IMaterialVar  *m_pIntensityVar;
    IMaterialVar  *m_pPowerUpVar;
};

DECLARE_AUTO_LIST( ITriggerPortalCleanserAutoList )

//-----------------------------------------------------------------------------
// Purpose: Removes anything that touches it. If the trigger has a targetname,
//			firing it will toggle state.
//-----------------------------------------------------------------------------
class C_TriggerPortalCleanser : public C_BaseTrigger, public ITriggerPortalCleanserAutoList
{
	DECLARE_CLASS( C_TriggerPortalCleanser, C_BaseTrigger );
	DECLARE_CLIENTCLASS();
public:
    	
	C_TriggerPortalCleanser();
    ~C_TriggerPortalCleanser();
    
	void OnDataChanged( DataUpdateType_t updateType );
    
	bool IsEnabled() { return !m_bDisabled; }
	bool UsesScanline() { return m_bUseScanline; }
    void GetCurrentState( float & ,float & );
	bool GetSoundSpatialization( SpatializationInfo_t& info );
    void UpdateParticles();
    void DestroyParticles();
    
	void Touch( C_BaseEntity *pOther );
    void UpdatePartitionListEntry();
	
    bool m_bObject1InRange;
    bool m_bObject2InRange;
	
	CNetworkHandle( C_BaseEntity, m_hObject1 );
	CNetworkHandle( C_BaseEntity, m_hObject2 );
	
    C_BaseEntity *GetEntity() { return this; }
	
private:

    bool m_bDisabled;
    bool m_bVisible;
    bool m_bUseScanline;
    bool m_bPlayersPassTriggerFilters;
	
    float m_flPowerUpTimer;
    float m_flPortalShotTime;
    float m_flLastShotTime;
    float m_flShotPulseTimer;
    float m_flLastUpdateTime;
	
    CUtlReference<CNewParticleEffect> m_hCleanserFX;
};
#endif