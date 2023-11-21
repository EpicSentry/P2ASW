#ifndef PORTAL_TURRET_FLOOR_H
#define PORTAL_TURRET_FLOOR_H

#include "cbase.h"
#include "npc_turret_floor.h"
#include "portal_player.h"
#include "portal_grabcontroller_shared.h"
#include "basehlcombatweapon_shared.h"
#include "ammodef.h"
#include "ai_senses.h"
#include "ai_memory.h"
#include "prop_portal_shared.h"
#include "sprite.h"
#include "paint/player_pickup_paint_power_user.h"


#define SF_FLOOR_TURRET_AUTOACTIVATE		0x00000020
#define SF_FLOOR_TURRET_STARTINACTIVE		0x00000040
#define SF_FLOOR_TURRET_OUT_OF_AMMO			0x00000100

//Turret types
#define	FLOOR_TURRET_PORTAL_MODEL	"models/npcs/turret/turret.mdl"
#define DEFECTIVE_TURRET_MODEL		"models/npcs/turret/turret_skeleton.mdl"
#define BOX_TURRET_MODEL			"models/npcs/turret/turret_boxed.mdl"
#define BACKWARDS_TURRET_MODEL		"models/npcs/turret/turret_backwards.mdl"

#define FLOOR_TURRET_GLOW_SPRITE	"sprites/glow1.vmt"
#define FLOOR_TURRET_BC_YAW			"aim_yaw"
#define FLOOR_TURRET_BC_PITCH		"aim_pitch"
#define	PORTAL_FLOOR_TURRET_RANGE	1500
#define	PORTAL_FLOOR_TURRET_MAX_SHOT_DELAY	2.5f
#define	FLOOR_TURRET_MAX_WAIT		5
#define FLOOR_TURRET_SHORT_WAIT		2.0f		// Used for FAST_RETIRE spawnflag

#define SF_FLOOR_TURRET_FASTRETIRE			0x00000080

#define TURRET_FLOOR_DAMAGE_MULTIPLIER 3.0f
#define TURRET_FLOOR_BULLET_FORCE_MULTIPLIER 0.4f
#define TURRET_FLOOR_PHYSICAL_FORCE_MULTIPLIER 135.0f

//Turret states
enum portalTurretState_e
{
	PORTAL_TURRET_DISABLED = TURRET_STATE_TOTAL,
	PORTAL_TURRET_COLLIDE,
	PORTAL_TURRET_PICKUP,
	PORTAL_TURRET_SHOTAT,
	PORTAL_TURRET_DISSOLVED,
	PORTAL_TURRET_FLUNG,
	PORTAL_TURRET_STARTBURNING,
	PORTAL_TURRET_BURNED,

	PORTAL_TURRET_STATE_TOTAL
};

//Turret types
enum portalTurretType
{
	Normal = 0, //Standard model
	Unused = 1, //The user will define their own model
	Box = 2, //Box turret, deals no damage at all
	Backwards = 3, //Turret fires backwards
	Skeleton = 4 //Defective
};

//Debug visualization
extern ConVar	g_debug_turret;

//Activities
extern int ACT_FLOOR_TURRET_OPEN;
extern int ACT_FLOOR_TURRET_CLOSE;
extern int ACT_FLOOR_TURRET_OPEN_IDLE;
extern int ACT_FLOOR_TURRET_CLOSED_IDLE;
extern int ACT_FLOOR_TURRET_FIRE;
extern int ACT_FLOOR_TURRET_FIRE2;
extern int ACT_FLOOR_TURRET_DIE;
extern int ACT_FLOOR_TURRET_DIE_IDLE;




class CNPC_Portal_FloorTurret : public PlayerPickupPaintPowerUser< CNPC_FloorTurret >
{
	DECLARE_CLASS( CNPC_Portal_FloorTurret, PlayerPickupPaintPowerUser< CNPC_FloorTurret > );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

public:

	CNPC_Portal_FloorTurret( void );

	virtual void	Precache( void );
	virtual void	Spawn( void );
	virtual void	Activate( void );
	virtual void	OnRestore( void );
	virtual void	UpdateOnRemove( void );
	virtual int		OnTakeDamage( const CTakeDamageInfo &info );

	virtual bool	ShouldAttractAutoAim( CBaseEntity *pAimingEnt );
	virtual float	GetAutoAimRadius();
	virtual Vector	GetAutoAimCenter();

	virtual void	OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );

	virtual void	NotifySystemEvent( CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params );

	virtual bool	PreThink( turretState_e state );
	virtual void	Shoot( const Vector &vecSrc, const Vector &vecDirToEnemy, bool bStrict = false );
	virtual void	SetEyeState( eyeState_t state );

	virtual void    TryEmitSound(const char* soundname);
	virtual void	SetTurretType(int nType);
	virtual bool	OnSide( void );
	void			EnableTipController(bool bEnabled);

	virtual float	GetAttackDamageScale( CBaseEntity *pVictim );
	virtual Vector	GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget );
	virtual bool	HasPreferredCarryAnglesForPlayer(CBasePlayer* pPlayer) { return true; }
	virtual QAngle	PreferredCarryAngles() { return QAngle(-45.0f,0.0f,0.0f); }
	
	// Think functions
	virtual void	Retire( void );
	virtual void	Deploy( void );
	virtual void	ActiveThink( void );
	virtual void	SearchThink( void );
	virtual void	AutoSearchThink( void );
	virtual void	TippedThink( void );
	virtual void	HeldThink( void );
	virtual void	InactiveThink( void );
	virtual void	DieThink( void );
	virtual void	SuppressThink( void );
	virtual void	DisabledThink( void );
	virtual void	HackFindEnemy( void );
	virtual void	BurnThink(void);
	virtual void	BreakThink(void);

#ifndef NO_TRACTOR_BEAM

	void	OnExitedTractorBeam(void) {}
	void	OnEnteredTractorBeam(void) {}

#endif

	virtual void	StartTouch( CBaseEntity *pOther );

	bool	IsLaserOn( void ) { return m_bLaserOn; }
	void	LaserOff( void );
	void	LaserOn( void );

	void	FireBullet( const char *pTargetName );

	// Inputs
	void	InputFireBullet( inputdata_t &inputdata );
	void	InputEnableGagging(inputdata_t& inputdata);
	void	InputDisableGagging(inputdata_t& inputdata);
	void	InputEnablePickup(inputdata_t& inputdata);
	void	InputDisablePickup(inputdata_t& inputdata);
	void	InputSelfDestructImmediately(inputdata_t& inputdata);
	void	InputSetModel(inputdata_t& inputdata);

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

protected:
	float GetFireConeZTolerance();

private:
	bool IsMovingSuddenly();
	bool IsEnemyBehindGlass(CPortal_Base2D* pPortal, CBaseEntity* pEnemy, Vector& vecMuzzle, Vector& vecDirToEnemy, float flDistToEnemy);
	void MakeSolid();
	bool AllowedToIgnite() { return true; }

	CNetworkVar( bool, m_bOutOfAmmo );
	CNetworkVar( bool, m_bLaserOn );
	CNetworkVar( bool, m_bIsFiring );
	CNetworkVar( int, m_sLaserHaloSprite );

	bool	m_bIsDead;
	int		m_iBarrelAttachments[ 4 ];
	int		m_iNextShootingBarrel;
	bool	m_bShootWithBottomBarrels;
	bool	m_bDamageForce;
	bool	m_bPickupEnabled;
	bool	m_bDisableMotion;
	int		m_nCollisionType;
	float	m_flTurretRange;
	bool	m_bLoadAlternativeModels;

	float	m_fSearchSpeed;
	float	m_fMovingTargetThreashold;
	float	m_flDistToEnemy;
	float	m_flPreviousVelocity;
	float	m_flBurnExplodeTime;

	turretState_e	m_iLastState;
	float			m_fNextTalk;
	bool			m_bDelayTippedTalk;
	bool			m_bUsedAsActor;
	bool			m_bGagged;
	int				m_nModelIndex;
	bool			m_bUseSuperDamageScale;
};
#endif // PORTAL_TURRET_FLOOR_H
