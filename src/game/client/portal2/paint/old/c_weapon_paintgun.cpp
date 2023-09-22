//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: Paint gun for Paint.
//
//=============================================================================//

#include "cbase.h"
#include "c_weapon_paintgun.h"
#include "paint/weapon_paintgun_shared.h"
#include "c_portal_player.h"
#include "paint/paint_color_manager.h"
#include "c_basetempentity.h"
#include "in_buttons.h"
#include "igameevents.h"

const char* const PAINT_PARTICLE_STREAM = "paint_hose";

ConVar cl_erase_stream_color( "cl_erase_stream_color", "255 255 255 0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "Change the appearance of the erase particle stream." );

ConVar draw_paint_particle_stream( "draw_paint_particle_stream", "0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

//extern ConVar sv_limit_paint;
// FIXME: Bring this back for DLC2
#define paintgun_blobs_per_second	40.f//extern ConVar paintgun_blobs_per_second;
#define paintgun_ammo_type 0 //extern ConVar paintgun_ammo_type;


static void ChangePaintColor( PaintPowerType power )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	C_WeaponPaintGun *pPaintGun = dynamic_cast< C_WeaponPaintGun* >( pPlayer->GetActiveWeapon() );
	if ( pPaintGun )
	{
		pPaintGun->SetPaintPower( power );
	}
}

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPaintGun, DT_WeaponPaintGun )
BEGIN_NETWORK_TABLE( C_WeaponPaintGun, DT_WeaponPaintGun )

	RecvPropArray3( RECVINFO_ARRAY( m_bHasPaint ), RecvPropBool( RECVINFO( m_bHasPaint[0] ) ) ),
	RecvPropArray3( RECVINFO_ARRAY( m_hPaintStream ), RecvPropEHandle( RECVINFO( m_hPaintStream[0] ) ) ),
	RecvPropArray3( RECVINFO_ARRAY( m_PaintAmmoPerType ), RecvPropInt( RECVINFO( m_PaintAmmoPerType[0] ) ) ),
	RecvPropInt( RECVINFO(m_nCurrentColor) ),
	RecvPropBool( RECVINFO(m_bFiringPaint) ),
	RecvPropBool( RECVINFO(m_bFiringErase) ),
	RecvPropEHandle( RECVINFO( m_hPaintStream ) ),
	RecvPropInt( RECVINFO(m_nPaintAmmo) ),

END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( C_WeaponPaintGun )
	DEFINE_PRED_FIELD( m_nCurrentColor, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bFiringPaint, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bFiringErase, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_ARRAY( m_bHasPaint, FIELD_BOOLEAN, PAINT_POWER_TYPE_COUNT_PLUS_NO_POWER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_ARRAY( m_PaintAmmoPerType, FIELD_INTEGER, PAINT_POWER_TYPE_COUNT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nPaintAmmo, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_paintgun, C_WeaponPaintGun );
PRECACHE_WEAPON_REGISTER( weapon_paintgun );


C_WeaponPaintGun::C_WeaponPaintGun()
	: m_pStreamEffect(0),
	  m_flBobScale(0.f),
	  m_nCurrentColor( NO_POWER ),
	  m_bFiringPaint( false ),
	  m_bFiringErase( false ),
	  m_nPaintAmmo( 0 ),
	  m_pLastOwner( NULL ),
	  m_flAccumulatedTime( 0.0f ),
	  m_flLastThinkTime( 0.0f )
{}

extern float	g_lateralBob;
extern float	g_verticalBob;

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f


void C_WeaponPaintGun::Precache( void )
{
	PrecacheParticleSystem( "paint_splat_bounce_01" );
	// FIXME: Bring back for DLC2
	//PrecacheParticleSystem( "paint_splat_reflect_01" );
	PrecacheParticleSystem( "paint_splat_speed_01" );
	PrecacheParticleSystem( "paint_splat_erase_01" );

	BaseClass::Precache();
}

void C_WeaponPaintGun::Spawn( void )
{
	BaseClass::Spawn();

	m_flLastThinkTime = gpGlobals->curtime;
	m_flAccumulatedTime = 1.0f/paintgun_blobs_per_second;
	m_nBlobRandomSeed = 0;

	SetNextThink( CLIENT_THINK_ALWAYS );
	SetPaintPower( NO_POWER );

	ResetAmmo();
}


void C_WeaponPaintGun::ClientThink()
{
	float flDeltaTime = gpGlobals->curtime - m_flLastThinkTime;
	m_flLastThinkTime = gpGlobals->curtime;

	C_Portal_Player *pPlayer = ToPortalPlayer( GetOwner() );

	if ( IsCarrierAlive() && ( WeaponState() == WEAPON_IS_ACTIVE ) )
	{
		//if( !C_BasePlayer::IsLocalPlayer( pPlayer ) )
		{
			if( flDeltaTime > 0.0f && ( m_bFiringPaint || m_bFiringErase ) )
			{
				//Fire paint blobs on the client
				PaintPowerType blobType = m_bFiringPaint ? (PaintPowerType)m_nCurrentColor : NO_POWER;

				SprayPaint( flDeltaTime, blobType );
			}
		}
	}

	//Keep track of the last position of the player
	if( pPlayer )
	{
		m_vecOldBlobFirePos = pPlayer->GetPaintGunShootPosition();
	}

	Color color = ( C_BasePlayer::IsLocalPlayer( GetOwner() ) ) ? MapPowerToColor( m_iSubType ) : MapPowerToColor( m_nCurrentColor );
	if ( MapColorToPower( color ) == NO_POWER || !HasAnyPaintPower() )
		color = Color( 255, 255, 255 );

	SetRenderColor( color.r(), color.g(), color.b() );

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}


bool C_WeaponPaintGun::Simulate()
{
	bool result = true;

	if( IsEffectActive( EF_NODRAW ) )
	{
		StopHoseEffect();
		result = false;
	}

	return result;
}


void C_WeaponPaintGun::OnPreDataChanged( DataUpdateType_t type )
{
	BaseClass::OnPreDataChanged( type );

	m_pLastOwner = GetOwner();
	m_pLastEffect = m_pStreamEffect;
}


void C_WeaponPaintGun::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	if( updateType == DATA_UPDATE_CREATED )
	{
		m_iSubType = m_nCurrentColor;

		C_Portal_Player *pOwner = ToPortalPlayer( GetOwner() );

		if( pOwner )
		{
			m_vecOldBlobFirePos = pOwner->GetPaintGunShootPosition();
		}
	}

	if ( IsCarrierAlive() && ( WeaponState() == WEAPON_IS_ACTIVE ) )
	{
		if ( ( m_bFiringPaint && HasAnyPaintPower() ) || m_bFiringErase )
		{
			StartHoseEffect();
		}
		else
		{
			StopHoseEffect();
		}

		ChangeRenderColor();
	}
	else 
	{
		StopHoseEffect();
	}

	// stop old stream particle when player throws weapon
	if ( m_pLastOwner != GetOwner() )
	{
		StopHoseEffect();
		CBaseCombatCharacter *pCurrentOwner = GetOwner();
		SetOwner( m_pLastOwner );
		m_pStreamEffect = m_pLastEffect;
		StopHoseEffect();
		SetOwner( pCurrentOwner );
	}
}


void C_WeaponPaintGun::ChangeRenderColor( bool bForce )
{
	C_BasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	Color color = ( C_BasePlayer::IsLocalPlayer( pPlayer ) ) ? MapPowerToColor( m_iSubType ) : MapPowerToColor( m_nCurrentColor );
	if ( !HasAnyPaintPower() && !bForce )
		color = Color( 255, 255, 255, 255 );

	if( pPlayer && pPlayer->IsLocalPlayer() )
	{
		C_BaseViewModel *pGunModel = GetEffectViewModel();

		if( pGunModel )
		{
			pGunModel->SetRenderColor( color.r(), color.g(), color.b() );
		}
	}
	SetRenderColor( color.r(), color.g(), color.b() );
}


void C_WeaponPaintGun::StartHoseEffect()
{
	if( !draw_paint_particle_stream.GetBool() )
	{
		return;
	}

	// if limit paint, don't start particle for erase
	//if ( sv_limit_paint.GetBool() && !m_bFiringPaint )
	//{
	//	StopHoseEffect();
	//	return;
	//}

	// Choose the correct particle effect.
	const char* particleEffectName = PAINT_PARTICLE_STREAM;

	Color paintColor;

	C_BasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if( C_BasePlayer::IsLocalPlayer( pOwner ) )
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD_ENT( pOwner );

		// If there's already a different stream particle effect, get rid of it.
		if( !m_pStreamEffect.IsValid() )
		{
			m_pStreamEffect = pOwner->GetViewModel()->ParticleProp()->Create( particleEffectName, PATTACH_POINT_FOLLOW, "rtube_muzzle" );
		}

		paintColor = m_bFiringPaint ? MapPowerToVisualColor( m_iSubType ) : MapPowerToVisualColor( NO_POWER );
	}
	else
	{
		if( !m_pStreamEffect.IsValid() )
		{
			m_pStreamEffect = ParticleProp()->Create( particleEffectName, PATTACH_POINT_FOLLOW, "muzzle" );
		}

		paintColor = m_bFiringPaint ? MapPowerToVisualColor( m_nCurrentColor ) : MapPowerToVisualColor( NO_POWER );
	}

	m_pStreamEffect->SetControlPoint( 1, Vector( paintColor.r(), paintColor.g(), paintColor.b() ) );
}


void C_WeaponPaintGun::StopHoseEffect()
{
	if ( m_pStreamEffect.IsValid() )
	{
		// Stop the effect on the viewmodel if our owner is the local player
		C_BasePlayer *pOwner = ToBasePlayer( GetOwner() );
		if ( C_BasePlayer::IsLocalPlayer( pOwner ) )
		{
			ACTIVE_SPLITSCREEN_PLAYER_GUARD_ENT( pOwner );

			if ( pOwner->GetViewModel() )
			{
				pOwner->GetViewModel()->ParticleProp()->StopEmission( m_pStreamEffect );
				m_pStreamEffect = 0;
			}
		}
		else
		{
			ParticleProp()->StopEmission( m_pStreamEffect );
			m_pStreamEffect = 0;
		}
	}
}


C_BaseViewModel *C_WeaponPaintGun::GetEffectViewModel()
{
	// Start the effect on the viewmodel if our owner is the local player
	CBaseViewModel* pViewModel = 0; 
	C_BasePlayer* pOwner = ToBasePlayer( GetOwner() );
	if( C_BasePlayer::IsLocalPlayer( pOwner ) )
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD_ENT( pOwner );
		pViewModel = pOwner->GetViewModel();
	}

	return pViewModel;
}


void C_WeaponPaintGun::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	Vector	forward, right, up;
	AngleVectors( angles, &forward, &right, &up );

	CalcViewmodelBob();

	C_Portal_Player *pPortalPlayer = ToPortalPlayer( GetOwner() );
	if ( !pPortalPlayer )
		return;

	// Apply bob, but scaled down to 40%
	VectorMA( origin, g_verticalBob * 0.1f, forward, origin );

	// Z bob a bit more ...or not
	//origin += g_verticalBob * 0.1f * pPortalPlayer->GetPortalPlayerLocalData().m_Up;

	//move left and right
	VectorMA( origin, g_lateralBob * 0.8f, right, origin );

	//roll, pitch, yaw
	float rollAngle = g_verticalBob * 0.5f;
	VMatrix rotMatrix;
	Vector rotAxis = CrossProduct( right, up ).Normalized();

	MatrixBuildRotationAboutAxis( rotMatrix, rotAxis, rollAngle );
	up = rotMatrix * up;
	forward = rotMatrix * forward;
	right = rotMatrix * right;

	float pitchAngle = -g_verticalBob * 0.4f;
	rotAxis = right;
	MatrixBuildRotationAboutAxis( rotMatrix, rotAxis, pitchAngle );
	up = rotMatrix * up;
	forward = rotMatrix * forward;

	float yawAngle = -g_lateralBob * 0.3f;
	rotAxis = up;
	MatrixBuildRotationAboutAxis( rotMatrix, rotAxis, yawAngle );
	forward = rotMatrix * forward;

	VectorAngles( forward, up, angles );	
}

const float BOB_SCALE_TIME = 1.f;
float C_WeaponPaintGun::CalcViewmodelBob( void )
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer *player = ToBasePlayer( GetOwner() );
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it
	if ( ( !gpGlobals->frametime ) || ( player == NULL ) )
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}


	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length();

	speed = clamp( speed, -player->MaxSpeed(), player->MaxSpeed() );

	float bob_offset = RemapVal( speed, 0, player->MaxSpeed(), 0.0f, 1.0f );

	bobtime += gpGlobals->frametime * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int)(bobtime/HL2_BOB_CYCLE_MAX)*HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;


	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-HL2_BOB_UP)/(1.0 - HL2_BOB_UP);
	}

	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin(cycle);

	g_verticalBob = clamp( g_verticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = bobtime - (int)(bobtime/HL2_BOB_CYCLE_MAX*2)*HL2_BOB_CYCLE_MAX*2;
	cycle /= HL2_BOB_CYCLE_MAX*2;

	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-HL2_BOB_UP)/(1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin(cycle);
	g_lateralBob = clamp( g_lateralBob, -7.0f, 4.0f );

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}


void C_WeaponPaintGun::SetPaintPower( PaintPowerType type )
{
	if( HasPaintPower( type ) )
	{
		SetSubType( type );
		//input->MakeWeaponSelection( this );

		IGameEvent *event = gameeventmanager->CreateEvent( "player_changed_colors" );
		if ( event )
		{

			CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
			if( pPlayer )
			{
				event->SetInt("userid", pPlayer->GetUserID() );

				gameeventmanager->FireEventClientSide( event );
			}
		}
	}
}


static PaintPowerType paintPowersInGunOrder[PAINT_POWER_TYPE_COUNT] = { BOUNCE_POWER, REFLECT_POWER, SPEED_POWER, PORTAL_POWER };

void C_WeaponPaintGun::CyclePaintPower( bool bForward )
{
	//Check if the paint gun has any paint powers
	if( !HasAnyPaintPower() )
	{
		return;
	}

	int nCurrentPowerIndex = -1;
	int nCurrentColor = ( C_BasePlayer::IsLocalPlayer( GetOwner() ) ) ? m_iSubType : m_nCurrentColor;

	//Get the index of the current power using the paint order in the gun
	for( int i = 0; i < PAINT_POWER_TYPE_COUNT; ++i )
	{
		if( paintPowersInGunOrder[i] == nCurrentColor )
		{
			nCurrentPowerIndex = i;
			break;
		}
	}

	int nNextPowerIndex = nCurrentPowerIndex;
	int nCounter = bForward ? 1 : -1;

	//Loop until we find a power that the gun has
	do
	{
		nNextPowerIndex += nCounter;
		if( nNextPowerIndex == PAINT_POWER_TYPE_COUNT )
		{
			nNextPowerIndex = 0;
		}
		else if( nNextPowerIndex < 0 )
		{
			nNextPowerIndex = PAINT_POWER_TYPE_COUNT - 1;
		}

	} while ( !HasPaintPower( paintPowersInGunOrder[nNextPowerIndex] ) );

	SetPaintPower( paintPowersInGunOrder[nNextPowerIndex] );
}

int C_WeaponPaintGun::GetPaintAmmo() const
{
	if( paintgun_ammo_type == 2 && m_nCurrentColor >= 0 && m_nCurrentColor < PAINT_POWER_TYPE_COUNT)
	{
		return m_PaintAmmoPerType[m_nCurrentColor];
	}

	return m_nPaintAmmo;
}

static void NextPaint()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	C_WeaponPaintGun *pPaintGun = dynamic_cast< C_WeaponPaintGun* >( pPlayer->GetActiveWeapon() );
	if ( pPaintGun )
	{
		pPaintGun->CyclePaintPower( true );
	}
}
// FIXME: Bring this back for DLC2
//static ConCommand nextpaint( "nextpaint", NextPaint );


static void PrevPaint()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	C_WeaponPaintGun *pPaintGun = dynamic_cast< C_WeaponPaintGun* >( pPlayer->GetActiveWeapon() );
	if ( pPaintGun )
	{
		pPaintGun->CyclePaintPower( false );
	}
}
// FIXME: Bring this back for DLC2
//static ConCommand prevpaint( "prevpaint", PrevPaint );


static void ChangePaintTo( const CCommand& args )
{
	if ( args.ArgC() != 2 )
	{
		DevMsg("changepaintto bounce,speed,portal");// FIXME: Bring back for DLC2 ,reflect");
		return;
	}

	if ( V_stricmp( args[1], "bounce" ) == 0 )
	{
		ChangePaintColor( BOUNCE_POWER );
	}
	else if ( V_stricmp( args[1], "speed" ) == 0 )
	{
		ChangePaintColor( SPEED_POWER );
	}
	else if ( V_stricmp( args[1], "portal" ) == 0 )
	{
		ChangePaintColor( PORTAL_POWER );
	}
	// FIXME: Bring back for DLC2
	/*else if ( V_stricmp( args[1], "reflect" ) == 0 )
	{
		ChangePaintColor( REFLECT_POWER );
	}*/
}
// FIXME: Bring this back for DLC2
//static ConCommand changepaintto("changepaintto", ChangePaintTo );
