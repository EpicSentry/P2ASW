#ifndef TRIGGER_PORTAL_CLEANSER_H
#define TRIGGER_PORTAL_CLEANSER_H

#include "cbase.h"
#include "triggers.h"

static char *g_pszPortalNonCleansable[] = 
{ 
	"func_door", 
	"func_door_rotating", 
	"prop_door_rotating",
	"func_tracktrain",
	"env_ghostanimating",
	"physicsshadowclone",
	"prop_energy_ball",
	"npc_personality_core",
	NULL,
};

static char *g_pszFizzlingVortexObjects[] =
{
	"prop_weighted_cube",
	"npc_portal_turret_floor",
	"simple_physics_prop",
	"prop_monster_box",
	"hot_potato",
	NULL,
};

struct FizzlerVortexObjectInfo_t
{
	FizzlerVortexObjectInfo_t()
	{
		m_flDistanceSq = 0;
		m_hEnt = NULL;
	}
	
	float m_flDistanceSq;
    EHANDLE m_hEnt;
};

class FizzlerMultiOriginSoundPlayer : public CBaseEntity
{
	DECLARE_CLASS( FizzlerMultiOriginSoundPlayer, CBaseEntity );
	DECLARE_DATADESC();
public:
	DECLARE_SERVERCLASS();
    
    FizzlerMultiOriginSoundPlayer();
	~FizzlerMultiOriginSoundPlayer();

	void Spawn();
    
	static FizzlerMultiOriginSoundPlayer *Create( IRecipientFilter &filter, const char *soundName );
    int UpdateTransmitState();

private:
    CSoundPatch *m_pSound;
    void RemoveThink();	
};

class CleanserVortexTraceEnum : public ICountedPartitionEnumerator
{
public:
	CleanserVortexTraceEnum( CBaseEntity **pList, int listMax, CBaseTrigger *pCleanser );
	IterationRetval_t EnumElement( IHandleEntity *pHandleEntity );
	int GetCount() const { return m_count; }
    bool AddToList( CBaseEntity *pEntity );
private:
	CBaseEntity **m_pList;
    int m_listMax;
    int m_count;
    CBaseTrigger *m_pCleanser;
};

class CFizzlerVortexObjectInfoLess
{
public:
	// FIXME!!
    bool Less( FizzlerVortexObjectInfo_t &vortexObjectInfo1 ,FizzlerVortexObjectInfo_t &vortexObjectInfo2, void *pUnknown );
};

DECLARE_AUTO_LIST( ITriggerPortalCleanserAutoList );

//-----------------------------------------------------------------------------
// Purpose: Removes anything that touches it. If the trigger has a targetname,
//			firing it will toggle state.
//-----------------------------------------------------------------------------
class CTriggerPortalCleanser : public CBaseTrigger, public ITriggerPortalCleanserAutoList
{
public:
	DECLARE_CLASS( CTriggerPortalCleanser, CBaseTrigger );   
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	
    CTriggerPortalCleanser();
    ~CTriggerPortalCleanser();
	
	virtual void Spawn();
	virtual void Precache();
	virtual void Activate();
    virtual void Touch( CBaseEntity *pOther );
	virtual void UpdateOnRemove( void );
	virtual void Enable( void );
	virtual void Disable( void );
    void SearchThink();
    void SetPortalShot();
    void PlayerPassesTriggerFiltersThink();
		
    int UpdateTransmitState();
	
	bool IsEnabled() { return !m_bDisabled; }
	
    CBaseEntity *GetEntity() { return this; }
	
	static void FizzleBaseAnimating( CTriggerPortalCleanser *pFizzler, CBaseAnimating *pBaseAnimating );
    static char *s_szPlayerPassesTriggerFiltersThinkContext;
	
	//CNetworkVar( bool, m_bDisabled );	
	
private:	
	
    bool IsCloserThanExistingObjects( FizzlerVortexObjectInfo_t &info, int &iIndex );
    void ClearVortexObjects();
    void StartAmbientSounds();
    void StopAmbientSounds();
    void FizzleTouchingPortals();
    void InputFizzleTouchingPortals( inputdata_t &inputdata );
    
	COutputEvent m_OnDissolve;
    COutputEvent m_OnFizzle;
    COutputEvent m_OnDissolveBox;
    Vector m_vecSearchBoxMins;
    Vector m_vecSearchBoxMaxs;
    
	FizzlerVortexObjectInfo_t m_VortexObjects[2];	
	
	CNetworkVar( bool, m_bVisible );
	
	CNetworkVar( float, m_flPortalShotTime );
	
	CNetworkVar( bool, m_bObject1InRange );
	CNetworkVar( bool, m_bObject2InRange );
	CNetworkVar( bool, m_bUseScanline );
	CNetworkVar( bool, m_bPlayersPassTriggerFilters );
	
	CNetworkHandle( CBaseEntity, m_hObject1 );
	CNetworkHandle( CBaseEntity, m_hObject2 );
	
    CHandle<FizzlerMultiOriginSoundPlayer> s_FizzlerAmbientSoundPlayer;
};

#endif