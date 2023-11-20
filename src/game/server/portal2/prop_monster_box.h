#include "cbase.h"
#include "props.h"

class CPropMonsterBox : public CPhysicsProp
{
public:
	DECLARE_CLASS( CPropMonsterBox, CPhysicsProp );
	DECLARE_DATADESC();
	
	CPropMonsterBox();
	~CPropMonsterBox();

	void Spawn();
	void Precache();
	void OnFizzled();
	void BecomeBox( bool bForce );
	void BecomeMonster( bool bForce );
	void BecomeShortcircuit();
	void AnimateThink();
	void HandleAnimEvent( animevent_t *pEvent );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );
	void OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason );
	int ObjectCaps();
	
	
	void InputBecomeBox( inputdata_t &inputdata );
	void InputBecomeMonster( inputdata_t &inputdata );
	void InputBecomeShortcircuit( inputdata_t &inputdata );
	void InputDissolve( inputdata_t &inputdata );
	void InputSilentDissolve( inputdata_t &inputdata );
	
	bool HasPreferredCarryAnglesForPlayer( CBasePlayer *pPlayer ) { return true; }
	QAngle PreferredCarryAngles( void ) { return QAngle( 180, 0, 180 ); }
	
	COutputEvent m_OnFizzled;
    
private:
	
	bool m_bIsShortcircuit;
	bool m_bHeld;
	bool m_bIsABox;
	bool m_bForcedAsBox;
	bool m_bIsFlying;
	
	float m_flBoxSwitchSpeed;
	float m_flPushStrength;
    
	int m_nBodyGroups;
	int m_nDeferredTransform;
    
	bool m_bAllowSilentDissolve;
    	
};