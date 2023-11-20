//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#include "c_portal_player.h"
#include "c_te_effect_dispatch.h"
#include "iviewrender_beams.h"
#include "model_types.h"
#include "fx_interpvalue.h"
#include "precache_register.h"
#include "bone_setup.h"
#include "c_rumble.h"
#include "rumble_shared.h"
#include "particles_new.h"

#include "weapon_portalgun_shared.h"


#define	SPRITE_SCALE 128.0f

ConVar cl_portalgun_effects_min_alpha ("cl_portalgun_effects_min_alpha", "96", FCVAR_CLIENTDLL );
ConVar cl_portalgun_effects_max_alpha ("cl_portalgun_effects_max_alpha", "128", FCVAR_CLIENTDLL );

ConVar cl_portalgun_effects_min_size ("cl_portalgun_effects_min_size", "3.0", FCVAR_CLIENTDLL );
ConVar cl_portalgun_beam_size ("cl_portalgun_beam_size", "0.04", FCVAR_CLIENTDLL );

// HACK HACK! Used to make the gun visually change when going through a cleanser!
//ConVar cl_portalgun_effects_max_size ("cl_portalgun_effects_max_size", "2.5", FCVAR_REPLICATED );


//Precahce the effects
PRECACHE_REGISTER_BEGIN( GLOBAL, PrecacheEffectPortalgun )
PRECACHE( MATERIAL, PORTALGUN_BEAM_SPRITE )
PRECACHE( MATERIAL, PORTALGUN_BEAM_SPRITE_NOZ )
//PRECACHE( MATERIAL, PORTALGUN_GLOW_SPRITE )
PRECACHE( MATERIAL, PORTALGUN_ENDCAP_SPRITE )
PRECACHE( MATERIAL, PORTALGUN_GRAV_ACTIVE_GLOW )
PRECACHE( MATERIAL, PORTALGUN_PORTAL1_FIRED_LAST_GLOW )
PRECACHE( MATERIAL, PORTALGUN_PORTAL2_FIRED_LAST_GLOW )
PRECACHE( MATERIAL, PORTALGUN_PORTAL_MUZZLE_GLOW_SPRITE )
PRECACHE( MATERIAL, PORTALGUN_PORTAL_TUBE_BEAM_SPRITE )
PRECACHE_REGISTER_END()


IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPortalgun, DT_WeaponPortalgun )

BEGIN_NETWORK_TABLE( C_WeaponPortalgun, DT_WeaponPortalgun )
	RecvPropBool( RECVINFO( m_bCanFirePortal1 ) ),
	RecvPropBool( RECVINFO( m_bCanFirePortal2 ) ),
	RecvPropInt( RECVINFO( m_iLastFiredPortal ) ),
	RecvPropBool( RECVINFO( m_bOpenProngs ) ),
	RecvPropFloat( RECVINFO( m_fEffectsMaxSize1 ) ), // HACK HACK! Used to make the gun visually change when going through a cleanser!
	RecvPropFloat( RECVINFO( m_fEffectsMaxSize2 ) ),
	RecvPropInt( RECVINFO( m_EffectState ) ),
	RecvPropEHandle( RECVINFO( m_hPrimaryPortal ) ),
	RecvPropEHandle( RECVINFO( m_hSecondaryPortal ) ),
	RecvPropVector( RECVINFO( m_vecBluePortalPos ) ),
	RecvPropVector( RECVINFO( m_vecOrangePortalPos ) ),
	RecvPropInt( RECVINFO( m_iPortalLinkageGroupID ) ),
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( C_WeaponPortalgun )
	DEFINE_PRED_FIELD( m_bCanFirePortal1, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bCanFirePortal2, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iLastFiredPortal, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bOpenProngs, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_EffectState,	FIELD_INTEGER,	FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nSkin, FIELD_INTEGER, FTYPEDESC_NOERRORCHECK | FTYPEDESC_PRIVATE )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_portalgun, C_WeaponPortalgun );
//PRECACHE_WEAPON_REGISTER(weapon_portalgun);

C_WeaponPortalgun::~C_WeaponPortalgun( void )
{

}

void C_WeaponPortalgun::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	//SetThink( &C_BaseEntity::Think );
	SetNextThink( gpGlobals->curtime + 0.1 );
	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::StartEffects( void )
{

	Vector vColorPortal = GetEffectColor( m_iLastFiredPortal );

	C_BaseCombatCharacter *pOwner = GetOwner();
	C_BasePlayer *pPlayer = ToBasePlayer(pOwner); // edi
	C_BaseViewModel *vm = NULL; // eax

	if (pPlayer)
		vm = pPlayer->GetViewModel();
	
	if (!m_hPortalGunEffectFP && vm)
	{
		m_hPortalGunEffectFP = vm->ParticleProp()->Create( "portalgun_top_light_firstperson", PATTACH_POINT_FOLLOW, "Body_light");

		if ( !m_hPortalGunEffectFP )
			return;
		
		m_hPortalGunEffectFP->SetControlPoint( 1, vColorPortal );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 2, pOwner, PATTACH_CUSTOMORIGIN );
		m_hPortalGunEffectFP->SetControlPointEntity( 2, pOwner );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 3, vm, PATTACH_POINT_FOLLOW, "Beam_point1" );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 4, vm, PATTACH_POINT_FOLLOW, "Beam_point5" );
	}


	if ( !m_hPortalGunEffectTP )
	{
		m_hPortalGunEffectTP = ParticleProp()->Create( "portalgun_top_light_thirdperson", PATTACH_POINT_FOLLOW, "Body_light" );
				
		m_hPortalGunEffectTP->SetControlPoint( 1, vColorPortal );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 2, pOwner, PATTACH_CUSTOMORIGIN );
		m_hPortalGunEffectTP->SetControlPointEntity( 2, pOwner );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 3, this, PATTACH_POINT_FOLLOW, "Beam_point1" );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 4, this, PATTACH_POINT_FOLLOW, "Beam_point5" );
	}
}

void C_WeaponPortalgun::DestroyEffects( void )
{
	// Stop everything
	StopEffects();
}

//-----------------------------------------------------------------------------
// Purpose: Ready effects
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectReady( void )
{
	C_BasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer )
	{
		RumbleEffect( pPlayer->GetUserID(), 0x14u, 0, 0 );
	}
}


//-----------------------------------------------------------------------------
// Holding effects
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectHolding( void )
{	
	C_BaseViewModel *vm = NULL;
	C_BaseCombatCharacter *pOwner = GetOwner();
  
	C_BasePlayer *pPlayer = ToBasePlayer( pOwner ); // edi

	if ( pPlayer )
	{
		vm = pPlayer->GetViewModel();
	}
  
	if ( !m_hPortalGunEffectHoldingFP.IsValid() && vm )
	{

		m_hPortalGunEffectHoldingFP = vm->ParticleProp()->Create( "portalgun_beam_holding_FP", PATTACH_POINT_FOLLOW, "muzzle" );

		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingFP, 1, vm, PATTACH_POINT_FOLLOW, "Arm1_attach3" );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingFP, 2, vm, PATTACH_POINT_FOLLOW, "Arm2_attach3" );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingFP, 3, vm, PATTACH_POINT_FOLLOW, "Arm3_attach3" );
		
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingFP, 4, pOwner, PATTACH_CUSTOMORIGIN );
		
		m_hPortalGunEffectHoldingFP->SetControlPointEntity( 4, vm );
		
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingFP, 5, GetPlayerHeldEntity( pPlayer ), PATTACH_ABSORIGIN_FOLLOW );
	}
	if ( !m_hPortalGunEffectHoldingTP )
	{
		m_hPortalGunEffectHoldingTP = ParticleProp()->Create( "portalgun_beam_holding_TP", PATTACH_POINT_FOLLOW, "muzzle");

		ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingTP, 1, this, PATTACH_POINT_FOLLOW, "Arm1_attach3" );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingTP, 2, this, PATTACH_POINT_FOLLOW, "Arm2_attach3" );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingTP, 3, this, PATTACH_POINT_FOLLOW, "Arm3_attach3" );
		
		ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingTP, 4, pOwner, PATTACH_CUSTOMORIGIN );

		m_hPortalGunEffectHoldingTP->SetControlPointEntity( 4, pOwner );
		
		ParticleProp()->AddControlPoint( m_hPortalGunEffectHoldingTP, 5, GetPlayerHeldEntity(pPlayer), PATTACH_ABSORIGIN_FOLLOW, 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Shutdown for the weapon when it's holstered
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectNone( void )
{	
	if ( m_hPortalGunEffectFP.IsValid() )
	{
		m_hPortalGunEffectFP->StopEmission();
		m_hPortalGunEffectFP = NULL;
	}
	
	if ( m_hPortalGunEffectTP.IsValid() )
	{
		m_hPortalGunEffectTP->StopEmission();
		m_hPortalGunEffectTP = NULL;
	}
	
	if ( m_hPortalGunEffectHoldingFP.IsValid() )
	{
		m_hPortalGunEffectHoldingFP->StopEmission();
		m_hPortalGunEffectHoldingFP = NULL;
	}
	
	if ( m_hPortalGunEffectHoldingTP.IsValid() )
	{
		m_hPortalGunEffectHoldingTP->StopEmission();
		m_hPortalGunEffectHoldingTP = NULL;
	}
}

void C_WeaponPortalgun::OnPreDataChanged( DataUpdateType_t updateType )
{
	// TODO: Wtf?
	BaseClass::OnPreDataChanged( updateType );
}

void C_WeaponPortalgun::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
		
	if( updateType == DATA_UPDATE_DATATABLE_CHANGED )
	{
		m_fEffectsMaxSize1.m_Value = m_vecOrangePortalPos.z;
	}
	else
	{
		StartEffects();
		DoEffect( EFFECT_NONE );
	}

	// Disabled for now because it's too buggy
#if 1
	if( g_pGameRules->IsMultiplayer() )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
		if( pPlayer )
		{
			int iSkin = 0;

			if ( pPlayer->GetTeamNumber() == 2 )
				iSkin = 2;
			else
				iSkin = 1;

			SetSkin( iSkin );
			if ( pPlayer->GetViewModel() )
				pPlayer->GetViewModel()->SetSkin( iSkin );

		}
	}
#endif

	// Update effect state when out of parity with the server
	if ( m_nOldEffectState != m_EffectState || m_bOldCanFirePortal1 != m_bCanFirePortal1 || m_bOldCanFirePortal2 != m_bCanFirePortal2 )
	{
		DoEffect( m_EffectState );
		//m_nOldEffectState = m_EffectState;

		m_bOldCanFirePortal1 = m_bCanFirePortal1;
		m_bOldCanFirePortal2 = m_bCanFirePortal2;
	}
}

void C_WeaponPortalgun::ClientThink( void )
{
	C_BaseCombatCharacter *pOwner = GetOwner();

	CPortal_Player *pPlayer = ToPortalPlayer( pOwner );

	if ( pPlayer )
	{	
		C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
		
		// This if statement isn't correct, but it's cheaper than doing a dynamic_cast
		if ( pWeapon == this && m_EffectState != 2 )
		{
			if ( m_hPortalGunEffectHoldingFP )
			{
				m_hPortalGunEffectHoldingFP->StopEmission();
				m_hPortalGunEffectHoldingFP = NULL;
			}
			if ( m_hPortalGunEffectHoldingTP )
			{
				m_hPortalGunEffectHoldingTP->StopEmission();
				m_hPortalGunEffectHoldingTP = NULL;
			}
		}
	}
	DoEffectIdle();
}

void C_WeaponPortalgun::DoEffectIdle( void )
{	
	StartEffects();
	if ( m_bPulseUp )
	{
		float flNewPulse = m_fPulse + gpGlobals->frametime;
		m_fPulse = flNewPulse;
		if ( flNewPulse > 1.0 )
		{
			m_fPulse = 1.0;
			m_bPulseUp = 0;
		}
	}
	else
	{
		float flNewPulse = m_fPulse - gpGlobals->frametime;
		m_fPulse = flNewPulse;
		if ( flNewPulse < 0.0 )
		{
			m_fPulse = 0.0;
			m_bPulseUp = 1;
		}
	}
}

Vector C_WeaponPortalgun::GetEffectColor( int iPalletIndex )
{
	Color color;

	// Showing a special color for holding is confusing... just use the last fired color -Jeep
	/*if ( m_bOpenProngs )
	{
		color = UTIL_Portal_Color( 0 );
	}
	else */if ( m_iLastFiredPortal == 1 )
	{
		color = UTIL_Portal_Color_Particles( 1, GetTeamNumber() );
	}
	else if ( m_iLastFiredPortal == 2 )
	{
		color = UTIL_Portal_Color_Particles( 2, GetTeamNumber() );
	}
	else
	{
		color = Color( 128, 128, 128, 255 );
	}

	Vector vColor;
	vColor.x = color.r();
	vColor.y = color.g();
	vColor.z = color.b();

	return vColor;
}

extern void FormatViewModelAttachment( C_BasePlayer* pPlayer, Vector &vOrigin, bool bInverse );

void UpdatePoseParameter( C_BaseAnimating *pBaseAnimating, int iPose, float fValue )
{
	pBaseAnimating->SetPoseParameter( iPose, fValue );
}

void InterpToward( float *pfCurrent, float fGoal, float fRate )
{
	if ( *pfCurrent < fGoal )
	{
		*pfCurrent += fRate;

		if ( *pfCurrent > fGoal )
		{
			*pfCurrent = fGoal;
		}
	}
	else if ( *pfCurrent > fGoal )
	{
		*pfCurrent -= fRate;

		if ( *pfCurrent < fGoal )
		{
			*pfCurrent = fGoal;
		}
	}
}
void C_WeaponPortalgun::DoCleanseEffect( bool bPortal1Active, bool bPortal2Active )
{
	C_BasePlayer *pLocalPlayer = GetSplitScreenViewPlayer();
	if (pLocalPlayer && pLocalPlayer == GetOwner())
	{
		if (pLocalPlayer->GetViewModel())
		{
			if (bPortal1Active)
			{
				C_BaseViewModel *vm = pLocalPlayer->GetViewModel();
				CNewParticleEffect *pEffect = vm->ParticleProp()->Create( "portal_weapon_cleanser", PATTACH_POINT_FOLLOW, "muzzle" );
				if (pEffect)
				{				
					Color color = UTIL_Portal_Color( 1, GetTeamNumber() );

					Vector vColor;
					vColor.x = color.r();
					vColor.y = color.g();
					vColor.z = color.b();
					pEffect->SetControlPoint( 2, vColor );
				}
			}
			if (bPortal2Active)
			{
				C_BaseViewModel *vm = pLocalPlayer->GetViewModel();
				CNewParticleEffect *pEffect = vm->ParticleProp()->Create( "portal_weapon_cleanser", PATTACH_POINT_FOLLOW, "muzzle" );
				if (pEffect)
				{
					Color color = UTIL_Portal_Color( 2, GetTeamNumber() );
					Vector vColor;
					vColor.x = color.r();
					vColor.y = color.g();
					vColor.z = color.b();
					pEffect->SetControlPoint( 2, vColor );
				}
			}
		}
	}
	else
	{
		ParticleProp()->Create( "portal_weapon_cleanser", PATTACH_POINT_FOLLOW, "muzzle" );
	}
}