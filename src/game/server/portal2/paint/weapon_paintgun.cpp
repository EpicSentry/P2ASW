//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: Paint gun for Paint.
//
//=============================================================================//

#include "cbase.h"

#include "weapon_paintgun.h"
#include "paint/weapon_paintgun_shared.h"
#include "paint/paint_color_manager.h"
#include "paint_database.h"
#include "portal_player.h"
#include "paint_sprayer.h"

#include "world.h"
#include "util_shared.h"
#include "te_effect_dispatch.h"
#include "physics_prop_ragdoll.h"
#include "vphysics/friction.h"
#include "in_buttons.h"
#include "igameevents.h"
#include "particle_parse.h"
#include "basetempentity.h"

//#include "decals.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar paintgun_blobs_per_second;
//extern ConVar max_paint;
//extern ConVar sv_limit_paint;

char const *const PAINT_GUN_THINK_CONTEXT = "Paint Gun Think";

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPaintGun, DT_WeaponPaintGun )
BEGIN_NETWORK_TABLE( CWeaponPaintGun, DT_WeaponPaintGun )

	SendPropArray3
	(
		SENDINFO_ARRAY3( m_bHasPaint ),
		SendPropBool( SENDINFO_ARRAY( m_bHasPaint ) )
	),

	SendPropArray3
	(
		SENDINFO_ARRAY3( m_hPaintStream ), 
		SendPropEHandle( SENDINFO_ARRAY( m_hPaintStream ) )
	),

	SendPropArray3
	(
		SENDINFO_ARRAY3( m_PaintAmmoPerType ), 
		SendPropInt( SENDINFO_ARRAY( m_PaintAmmoPerType ) )
	),

	SendPropInt( SENDINFO(m_nCurrentColor) ),
	SendPropInt( SENDINFO(m_nPaintAmmo) ),
	SendPropBool( SENDINFO(m_bFiringPaint) ),
	SendPropBool( SENDINFO(m_bFiringErase) ),

END_NETWORK_TABLE()

BEGIN_DATADESC( CWeaponPaintGun )

	DEFINE_FIELD( m_nCurrentColor, FIELD_INTEGER ),
	DEFINE_FIELD( m_nPaintAmmo, FIELD_INTEGER ),
	DEFINE_FIELD( m_bFiringPaint, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bFiringErase, FIELD_BOOLEAN ),
	DEFINE_ARRAY( m_hPaintStream, FIELD_EHANDLE, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER ),
	DEFINE_ARRAY( m_bHasPaint, FIELD_BOOLEAN, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER ),
	DEFINE_ARRAY( m_PaintAmmoPerType, FIELD_INTEGER, PAINT_POWER_TYPE_COUNT ),
	DEFINE_THINKFUNC( PaintGunThink )

END_DATADESC()

LINK_ENTITY_TO_CLASS( weapon_paintgun, CWeaponPaintGun );
PRECACHE_WEAPON_REGISTER( weapon_paintgun );


CWeaponPaintGun::CWeaponPaintGun()
	: m_nCurrentColor( NO_POWER ),
	  m_bFiringPaint( false ),
	  m_bFiringErase( false ),
	  m_nPaintAmmo( 0 ),
	  m_flAccumulatedTime( 0.0f )
{
	m_bFireOnEmpty = true;
	m_bReloadsSingly = false;

	ResetPaint();
	//memset( m_bHasPaint, 0, sizeof(bool) * PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER );

	//HACK GET AMMO
	ActivatePaint(NO_POWER);

	//ActivatePaint(BOUNCE_POWER);
	//ActivatePaint(SPEED_POWER);
	//ActivatePaint(PORTAL_POWER);
	//ActivatePaint(REFLECT_POWER);
	SetSubType( m_nCurrentColor );
}

CWeaponPaintGun::~CWeaponPaintGun()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_pLiquidLoop )
	{
		controller.SoundDestroy( m_pLiquidLoop );
		m_pLiquidLoop = NULL;
	}

	if( m_pLiquidStart )
	{
		controller.SoundDestroy( m_pLiquidStart );
		m_pLiquidStart = NULL;
	}
}


void CWeaponPaintGun::Activate()
{
	BaseClass::Activate();

	CreatePaintStreams();
}

void CWeaponPaintGun::Spawn()
{
	Precache();

	BaseClass::Spawn();

	//SetNextThink( gpGlobals->curtime );
	SetContextThink( &CWeaponPaintGun::PaintGunThink, gpGlobals->curtime, PAINT_GUN_THINK_CONTEXT );

	m_flAccumulatedTime = 1.0f/paintgun_blobs_per_second.GetFloat();
	m_nBlobRandomSeed = 0;

	CreatePaintStreams();

	ResetAmmo();
}

void CWeaponPaintGun::CreatePaintStreams()
{
	for ( int i=0; i<PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER; ++i )
	{
		if ( m_hPaintStream[i] == NULL )
		{
			CPaintStream *pPaintStream = (CPaintStream*)CreateEntityByName( "paint_stream" );
			if ( pPaintStream  )
			{
				pPaintStream->Init( vec3_origin, (PaintPowerType)i, BLOB_RENDER_BLOBULATOR, 250 );
				DispatchSpawn( pPaintStream );
				m_hPaintStream.Set( i, pPaintStream );
			}
		}
	}
}


void CWeaponPaintGun::UpdateOnRemove()
{
	for ( int i=0; i<PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER; ++i )
	{
		UTIL_Remove( m_hPaintStream[i] );
	}

	BaseClass::UpdateOnRemove();
}


void CWeaponPaintGun::Precache()
{
	PrecacheScriptSound( "Paintgun.FireLoop" );
	PrecacheScriptSound( "NPC_CScanner.DiveBombFlyby" );

	PrecacheParticleSystem( "paint_splat_bounce_01" );
	// FIXME: Bring this back for DLC2
	//PrecacheParticleSystem( "paint_splat_reflect_01" );
	PrecacheParticleSystem( "paint_splat_speed_01" );
	PrecacheParticleSystem( "paint_splat_erase_01" );

	BaseClass::Precache();
}


bool CWeaponPaintGun::SendWeaponAnim( int iActivity )
{
	int newActivity = iActivity;

	if ( newActivity == ACT_VM_PRIMARYATTACK )
	{
		if ( !HasCurrentColor() )
		{
			newActivity = ACT_VM_DRYFIRE;
		}
	}

	//For now, just set the ideal activity and be done with it
	return BaseClass::SendWeaponAnim( newActivity );
}


bool CWeaponPaintGun::HasCurrentColor()
{
	return m_bHasPaint.Get( m_nCurrentColor );
}

void CWeaponPaintGun::StartShootingSound()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_pLiquidLoop != NULL )
	{
		controller.SoundDestroy( m_pLiquidLoop );
		m_pLiquidLoop = NULL;
	}

	if( m_pLiquidLoop == NULL )
	{
		CPASAttenuationFilter filter( this );
		m_pLiquidLoop = controller.SoundCreate( filter, entindex(), "Paintgun.FireLoop" );
		controller.Play( m_pLiquidLoop, 0.0, 100 );
		controller.SoundChangeVolume( m_pLiquidLoop, 1.0, 1.f );
	}
}

void CWeaponPaintGun::StopShootingSound()
{
	if( m_pLiquidLoop )
	{
		CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();

		controller.SoundFadeOut( m_pLiquidLoop, 0.5f, false );
	}
}


void CWeaponPaintGun::ActivatePaint( PaintPowerType nIndex )
{
	m_bHasPaint.Set( nIndex, true );

	IGameEvent *event = gameeventmanager->CreateEvent( "picked_up_paint" );
	if ( event )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
		if( pPlayer )
		{
			event->SetInt("userid", pPlayer->GetUserID() );
			event->SetInt("paintcount", GetPaintCount() );

			gameeventmanager->FireEvent( event );
		}
	}

	SetSubType( nIndex );
}


int CWeaponPaintGun::GetPaintCount( void )
{
	int nPaintCount = 0;
	for( int i = 0; i < PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER; ++i )
	{
		if( m_bHasPaint.Get( i ) )
		{
			nPaintCount++;
		}
	}

	return nPaintCount;
}


void CWeaponPaintGun::ResetPaint()
{
	for( int i = 0; i < PAINT_POWER_TYPE_COUNT; ++i)
	{
		DeactivatePaint( (PaintPowerType)i );
	}

	CBroadcastRecipientFilter filter;
	filter.MakeReliable();

	SetCurrentPaint( NO_POWER );
}


void CWeaponPaintGun::DeactivatePaint( PaintPowerType nIndex )
{
	m_bHasPaint.Set( nIndex, false );
}


void CWeaponPaintGun::CleansePaint()
{
	// Play the lose color sound if we lost any colors
	if( HasAnyPaintPower() )
	{
		EmitSound( "NPC_CScanner.DiveBombFlyby" );
		ResetPaint();	// Clear paint colors
	}
}

void CWeaponPaintGun::SetCurrentPaint( PaintPowerType nIndex )
{
	if( HasPaintPower( nIndex ) )
		m_nCurrentColor = nIndex;

	CBaseEntity* pOwner = GetOwner();
	if( pOwner == NULL )
		return;

	CSingleUserRecipientFilter filter( ToBasePlayer( pOwner ) );
	filter.MakeReliable();

	UserMessageBegin( filter, "ChangePaintColor" );
	WRITE_EHANDLE( this );
	WRITE_BYTE( nIndex );
	MessageEnd();
}

void CWeaponPaintGun::PaintGunThink()
{
	//m_flAttackDeltaTime = gpGlobals->curtime - m_flLastThinkTime;
	//m_flLastThinkTime = gpGlobals->curtime;

	//StudioFrameAdvance();

	//SetNextThink( gpGlobals->curtime );
	SetContextThink( &CWeaponPaintGun::PaintGunThink, gpGlobals->curtime, PAINT_GUN_THINK_CONTEXT );

	CPortal_Player *pPlayer = ToPortalPlayer( GetOwner() );
	if( pPlayer )
	{
		m_vecOldBlobFirePos = pPlayer->GetPaintGunShootPosition();
	}
}


extern void PaintPowerPickup( int colorIndex, CBasePlayer *pPlayer );

static void GiveAllPaintPowers()
{
	CBaseEntity* pFoundEnt = gEntList.FindEntityByClassname( NULL, "weapon_paintgun" );
	while( pFoundEnt )
	{
		CWeaponPaintGun *pPaintGun = dynamic_cast< CWeaponPaintGun* >( pFoundEnt );
		if ( pPaintGun )
		{
			CBasePlayer *pPlayer = ToBasePlayer( pPaintGun->GetOwner() );
			pPaintGun->ActivatePaint(BOUNCE_POWER);
			pPaintGun->ActivatePaint(SPEED_POWER);
			pPaintGun->ActivatePaint(PORTAL_POWER);
			pPaintGun->ActivatePaint(REFLECT_POWER);
			PaintPowerPickup( BOUNCE_POWER, pPlayer );
			PaintPowerPickup( SPEED_POWER, pPlayer );
			PaintPowerPickup( PORTAL_POWER, pPlayer );
			PaintPowerPickup( REFLECT_POWER, pPlayer );
			pPaintGun->SetCurrentPaint( BOUNCE_POWER );
		}

		pFoundEnt = gEntList.FindEntityByClassname( pFoundEnt, "weapon_paintgun" );
	}
}

static ConCommand giveallpaintpowers( "giveallpaintpowers", GiveAllPaintPowers );
