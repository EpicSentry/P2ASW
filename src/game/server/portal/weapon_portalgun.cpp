//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#include "BasePropDoor.h"
#include "portal_player.h"
#include "te_effect_dispatch.h"
#include "GameInterface.h"
#include "prop_combine_ball.h"
#include "portal_shareddefs.h"
#include "triggers.h"
#include "CollisionUtils.h"
#include "cbaseanimatingprojectile.h"
#include "portal_grabcontroller_shared.h"
#include "prop_portal_shared.h"
#include "portal_placement.h"
#include "weapon_portalgun_shared.h"
#include "physicsshadowclone.h"
#include "particle_parse.h"
#include "info_placement_helper.h"
#include "rumble_shared.h"

#define BLAST_SPEED_NON_PLAYER 1000.0f
#define BLAST_SPEED 3000.0f

CON_COMMAND(give_portalgun, "Give a portalgun... Portal 2 script fix.")
{
	CBasePlayer *pPlayer = NULL;
	while ((pPlayer = (CBasePlayer*)gEntList.FindEntityByClassname(pPlayer, "player")) != NULL)
	{
		CWeaponPortalgun* entity = dynamic_cast< CWeaponPortalgun * >(CreateEntityByName("weapon_portalgun"));
		DispatchSpawn(entity);
		Vector playerPos = pPlayer->GetAbsOrigin();
		playerPos.z += 36;
		entity->Teleport(&playerPos, NULL, NULL);
	}
}

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPortalgun, DT_WeaponPortalgun )

BEGIN_NETWORK_TABLE( CWeaponPortalgun, DT_WeaponPortalgun )
	SendPropBool( SENDINFO( m_bCanFirePortal1 ) ),
	SendPropBool( SENDINFO( m_bCanFirePortal2 ) ),
	SendPropInt( SENDINFO( m_iLastFiredPortal ) ),
	SendPropBool( SENDINFO( m_bOpenProngs ) ),
	SendPropFloat( SENDINFO( m_fEffectsMaxSize1 ) ), // HACK HACK! Used to make the gun visually change when going through a cleanser!
	SendPropFloat( SENDINFO( m_fEffectsMaxSize2 ) ),
	SendPropInt( SENDINFO( m_EffectState ) ),
	SendPropEHandle( SENDINFO( m_hPrimaryPortal ) ),
	SendPropEHandle( SENDINFO( m_hSecondaryPortal ) ),
END_NETWORK_TABLE()

BEGIN_DATADESC( CWeaponPortalgun )

	DEFINE_KEYFIELD( m_bCanFirePortal1, FIELD_BOOLEAN, "CanFirePortal1" ),
	DEFINE_KEYFIELD( m_bCanFirePortal2, FIELD_BOOLEAN, "CanFirePortal2" ),
	DEFINE_FIELD( m_iLastFiredPortal, FIELD_INTEGER ),
	DEFINE_FIELD( m_bOpenProngs, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_fEffectsMaxSize1, FIELD_FLOAT ),
	DEFINE_FIELD( m_fEffectsMaxSize2, FIELD_FLOAT ),
	DEFINE_FIELD( m_EffectState, FIELD_INTEGER ),
	DEFINE_FIELD( m_iPortalLinkageGroupID, FIELD_CHARACTER	),

	DEFINE_INPUTFUNC( FIELD_VOID, "ChargePortal1", InputChargePortal1 ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ChargePortal2", InputChargePortal2 ),
	DEFINE_INPUTFUNC( FIELD_VOID, "FirePortal1", InputFirePortal1 ),
	DEFINE_INPUTFUNC( FIELD_VOID, "FirePortal2", InputFirePortal2 ),
	DEFINE_INPUTFUNC( FIELD_VECTOR, "FirePortalDirection1", FirePortalDirection1 ),
	DEFINE_INPUTFUNC( FIELD_VECTOR, "FirePortalDirection2", FirePortalDirection2 ),

	DEFINE_SOUNDPATCH( m_pMiniGravHoldSound ),

	DEFINE_OUTPUT ( m_OnFiredPortal1, "OnFiredPortal1" ),
	DEFINE_OUTPUT ( m_OnFiredPortal2, "OnFiredPortal2" ),

	DEFINE_FUNCTION( Think ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( weapon_portalgun, CWeaponPortalgun );
//PRECACHE_WEAPON_REGISTER(weapon_portalgun);


extern ConVar sv_portal_placement_debug;
extern ConVar sv_portal_placement_never_fail;


void CWeaponPortalgun::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	SetThink( &CWeaponPortalgun::Think );
	SetNextThink( gpGlobals->curtime + 0.1 );

	if( GameRules()->IsMultiplayer() )
	{
		CBaseEntity *pOwner = GetOwner();
		if( pOwner && pOwner->IsPlayer() )
			m_iPortalLinkageGroupID = pOwner->entindex();

		Assert( (m_iPortalLinkageGroupID >= 0) && (m_iPortalLinkageGroupID < 256) );
	}	
}

void CWeaponPortalgun::Activate()
{
	BaseClass::Activate();

	CreateSounds();

	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer )
	{
		CBaseEntity *pHeldObject = GetPlayerHeldEntity( pPlayer );
		OpenProngs( ( pHeldObject ) ? ( false ) : ( true ) );
		OpenProngs( ( pHeldObject ) ? ( true ) : ( false ) );

		if( GameRules()->IsMultiplayer() )
			m_iPortalLinkageGroupID = pPlayer->entindex();

		Assert( (m_iPortalLinkageGroupID >= 0) && (m_iPortalLinkageGroupID < 256) );
	}

	// HACK HACK! Used to make the gun visually change when going through a cleanser!
	m_fEffectsMaxSize1 = 4.0f;
	m_fEffectsMaxSize2 = 4.0f;
}

void CWeaponPortalgun::OnPickedUp( CBaseCombatCharacter *pNewOwner )
{
	if( GameRules()->IsMultiplayer() )
	{
		if( pNewOwner && pNewOwner->IsPlayer() )
			m_iPortalLinkageGroupID = pNewOwner->entindex();

		Assert( (m_iPortalLinkageGroupID >= 0) && (m_iPortalLinkageGroupID < 256) );
	}

	BaseClass::OnPickedUp( pNewOwner );		
}

void CWeaponPortalgun::CreateSounds()
{
	if (!m_pMiniGravHoldSound)
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CPASAttenuationFilter filter( this );

		m_pMiniGravHoldSound = controller.SoundCreate( filter, entindex(), "Weapon_Portalgun.HoldSound" );
		controller.Play( m_pMiniGravHoldSound, 0, 100 );
	}
}

void CWeaponPortalgun::StopLoopingSounds()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	controller.SoundDestroy( m_pMiniGravHoldSound );
	m_pMiniGravHoldSound = NULL;

	BaseClass::StopLoopingSounds();
}

//-----------------------------------------------------------------------------
// Purpose: Allows a generic think function before the others are called
// Input  : state - which state the turret is currently in
//-----------------------------------------------------------------------------
bool CWeaponPortalgun::PreThink( void )
{
	//Animate
	StudioFrameAdvance();

	//Do not interrupt current think function
	return false;
}

void CWeaponPortalgun::Think( void )
{
	//Allow descended classes a chance to do something before the think function
	if ( PreThink() )
		return;

	SetNextThink( gpGlobals->curtime + 0.1f );

	CPortal_Player *pPlayer = ToPortalPlayer( GetOwner() );

	if ( !pPlayer || pPlayer->GetActiveWeapon() != this )
	{
		return;
	}


	// HACK HACK! Used to make the gun visually change when going through a cleanser!
	if ( m_fEffectsMaxSize1 > 4.0f )
	{
		m_fEffectsMaxSize1 -= gpGlobals->frametime * 400.0f;
		if ( m_fEffectsMaxSize1 < 4.0f )
			m_fEffectsMaxSize1 = 4.0f;
	}

	if ( m_fEffectsMaxSize2 > 4.0f )
	{
		m_fEffectsMaxSize2 -= gpGlobals->frametime * 400.0f;
		if ( m_fEffectsMaxSize2 < 4.0f )
			m_fEffectsMaxSize2 = 4.0f;
	}
}

void CWeaponPortalgun::OpenProngs( bool bOpenProngs )
{
	if ( m_bOpenProngs == bOpenProngs )
	{
		return;
	}

	m_bOpenProngs = bOpenProngs;

	DoEffect( ( m_bOpenProngs ) ? ( EFFECT_HOLDING ) : ( EFFECT_READY ) );

	SendWeaponAnim( ( m_bOpenProngs ) ? ( ACT_VM_PICKUP ) : ( ACT_VM_RELEASE ) );
}

void CWeaponPortalgun::InputChargePortal1( inputdata_t &inputdata )
{
	DispatchParticleEffect( "portal_1_charge", PATTACH_POINT_FOLLOW, this, "muzzle" );
}

void CWeaponPortalgun::InputChargePortal2( inputdata_t &inputdata )
{
	DispatchParticleEffect( "portal_2_charge", PATTACH_POINT_FOLLOW, this, "muzzle" );
}

void CWeaponPortalgun::InputFirePortal1( inputdata_t &inputdata )
{
	FirePortal( false );
	m_iLastFiredPortal = 1;

	CBaseCombatCharacter *pOwner = GetOwner();

	if( pOwner && pOwner->IsPlayer() )
	{
		WeaponSound( SINGLE );
	}
	else
	{
		WeaponSound( SINGLE_NPC );
	}
}

void CWeaponPortalgun::InputFirePortal2( inputdata_t &inputdata )
{
	FirePortal( true );
	m_iLastFiredPortal = 2;

	CBaseCombatCharacter *pOwner = GetOwner();

	if( pOwner && pOwner->IsPlayer() )
	{
		WeaponSound( WPN_DOUBLE );
	}
	else
	{
		WeaponSound( DOUBLE_NPC );
	}
}

void CWeaponPortalgun::FirePortalDirection1( inputdata_t &inputdata )
{
	Vector vDirection;
	inputdata.value.Vector3D( vDirection );
	FirePortal( false, &vDirection );
	m_iLastFiredPortal = 1;
	
	CBaseCombatCharacter *pOwner = GetOwner();

	if( pOwner && pOwner->IsPlayer() )
	{
		WeaponSound( SINGLE );
	}
	else
	{
		WeaponSound( SINGLE_NPC );
	}
}

void CWeaponPortalgun::FirePortalDirection2( inputdata_t &inputdata )
{
	Vector vDirection;
	inputdata.value.Vector3D( vDirection );
	FirePortal( true, &vDirection );
	m_iLastFiredPortal = 2;
	
	CBaseCombatCharacter *pOwner = GetOwner();

	if( pOwner && pOwner->IsPlayer() )
	{
		WeaponSound( WPN_DOUBLE );
	}
	else
	{
		WeaponSound( DOUBLE_NPC );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponPortalgun::StartEffects( void )
{
}

void CWeaponPortalgun::DestroyEffects( void )
{
	// Stop everything
	StopEffects();
}

//-----------------------------------------------------------------------------
// Purpose: Ready effects
//-----------------------------------------------------------------------------
void CWeaponPortalgun::DoEffectReady( void )
{
	if ( m_pMiniGravHoldSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundChangeVolume( m_pMiniGravHoldSound, 0.0, 0.1 );
	}
}


//-----------------------------------------------------------------------------
// Holding effects
//-----------------------------------------------------------------------------
void CWeaponPortalgun::DoEffectHolding( void )
{
	if ( m_pMiniGravHoldSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundChangeVolume( m_pMiniGravHoldSound, 1.0, 0.1 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Shutdown for the weapon when it's holstered
//-----------------------------------------------------------------------------
void CWeaponPortalgun::DoEffectNone( void )
{
	if ( m_pMiniGravHoldSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundChangeVolume( m_pMiniGravHoldSound, 0.0, 0.1 );
	}
}

void CC_UpgradePortalGun( void )
{
	CPortal_Player *pPlayer = ToPortalPlayer( UTIL_GetCommandClient() );

	CWeaponPortalgun *pPortalGun = static_cast<CWeaponPortalgun*>(pPlayer->Weapon_OwnsThisType("weapon_portalgun"));
	if (pPortalGun)
	{
		pPortalGun->SetCanFirePortal1();
		pPortalGun->SetCanFirePortal2();
	}
}

static ConCommand upgrade_portal("upgrade_portalgun", CC_UpgradePortalGun, "Equips the player with a single portal portalgun. Use twice for a dual portal portalgun.\n\tArguments:   	none ", FCVAR_CHEAT);

void CC_UpgradePotatoGun( void )
{
	CPortal_Player *pPlayer = ToPortalPlayer( UTIL_GetCommandClient() );

	CWeaponPortalgun *pPortalGun = static_cast<CWeaponPortalgun*>(pPlayer->Weapon_OwnsThisType("weapon_portalgun"));
	if (pPortalGun)
	{
		pPortalGun->SetCanFirePortal1();
		pPortalGun->SetCanFirePortal2();
		pPortalGun->SetPotatosOnPortalgun( true );
	}
}

static ConCommand upgrade_potato("upgrade_potatogun", CC_UpgradePotatoGun, "Upgrades to the portalgun to the dual portalgun with potatos attached\n\tArguments:   	none ", FCVAR_CHEAT);

static void change_portalgun_linkage_id_f( const CCommand &args )
{
	if( sv_cheats->GetBool() == false ) //heavy handed version since setting the concommand with FCVAR_CHEATS isn't working like I thought
		return;

	if( args.ArgC() < 2 )
		return;

	unsigned char iNewID = (unsigned char)atoi( args[1] );

	CPortal_Player *pPlayer = (CPortal_Player *)UTIL_GetCommandClient();

	int iWeaponCount = pPlayer->WeaponCount();
	for( int i = 0; i != iWeaponCount; ++i )
	{
		CBaseCombatWeapon *pWeapon = pPlayer->GetWeapon(i);
		if( pWeapon == NULL )
			continue;

		if( dynamic_cast<CWeaponPortalgun *>(pWeapon) != NULL )
		{
			CWeaponPortalgun *pPortalGun = (CWeaponPortalgun *)pWeapon;
			pPortalGun->m_iPortalLinkageGroupID = iNewID;
			break;
		}
	}
}

ConCommand change_portalgun_linkage_id( "change_portalgun_linkage_id", change_portalgun_linkage_id_f, "Changes the portal linkage ID for the portal gun held by the commanding player.", FCVAR_CHEAT );


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponPortalgun::SetPotatosOnPortalgun( bool bPotatos )
{
	// TODO
}