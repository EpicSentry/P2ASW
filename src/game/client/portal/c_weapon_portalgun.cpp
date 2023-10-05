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

void C_WeaponPortalgun::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	//SetThink( &C_BaseEntity::Think );
	SetNextThink( gpGlobals->curtime + 0.1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::StartEffects( void )
{
#if 1 // Portal 2 Effects

	Vector vColorPortal; // [esp+Ch] [ebp-14h] BYREF

	C_BaseCombatCharacter *pOwner = GetOwner();
	C_BasePlayer *pPlayer = ToBasePlayer(pOwner); // edi
	C_BaseViewModel *vm = NULL; // eax

	if (pPlayer)
		vm = pPlayer->GetViewModel();

	if (!m_hPortalGunEffectFP.IsValid() && vm)
	{
		m_hPortalGunEffectFP = vm->ParticleProp()->Create( "portalgun_top_light_firstperson", PATTACH_POINT_FOLLOW, "Body_light");

		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 2, pOwner, PATTACH_CUSTOMORIGIN, 0, vec3_origin, 0 );
		m_hPortalGunEffectFP->SetControlPointEntity( 2, pOwner );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 3, vm, PATTACH_POINT_FOLLOW, "Beam_point1", vec3_origin, 0 );
		vm->ParticleProp()->AddControlPoint( m_hPortalGunEffectFP, 4, vm, PATTACH_POINT_FOLLOW, "Beam_point5", vec3_origin, 0 );
	}
	if ( m_hPortalGunEffectTP.IsValid() )
	{
	LABEL_28:

		vColorPortal = GetEffectColor( m_iLastFiredPortal.m_Value );
		if ( m_hPortalGunEffectFP.IsValid() )
		{
			m_hPortalGunEffectFP->SetControlPoint( 1, vColorPortal );
			m_hPortalGunEffectFP->SetControlPointEntity( 2, pOwner );
		}

		if ( m_hPortalGunEffectTP.IsValid() )
		{
			m_hPortalGunEffectTP->SetControlPoint( 1, vColorPortal );
			m_hPortalGunEffectTP->SetControlPointEntity( 2, pOwner );
		}
		return;
	}

	m_hPortalGunEffectTP = ParticleProp()->Create( "portalgun_top_light_thirdperson", PATTACH_POINT_FOLLOW, "Body_light" );

	if ( m_hPortalGunEffectTP.IsValid() )
	{
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 2, pOwner, PATTACH_CUSTOMORIGIN, 0, vec3_origin, 0 );

		m_hPortalGunEffectTP->SetControlPointEntity( 2, pOwner );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 3, this, PATTACH_POINT_FOLLOW, "Beam_point1", vec3_origin, 0 );
		ParticleProp()->AddControlPoint( m_hPortalGunEffectTP, 4, this, PATTACH_POINT_FOLLOW, "Beam_point5", vec3_origin, 0 );
		goto LABEL_28;
	}

#else // Portal 1 Effects
	int i;

	CBaseEntity *pModelView = ( ( GetOwner() ) ? ( ToBasePlayer( GetOwner() )->GetViewModel() ) : ( 0 ) );
	CBaseEntity *pModelWorld = this;
	
	if ( !pModelView )
	{
		pModelView = pModelWorld;
	}

	// ------------------------------------------
	// Lights
	// ------------------------------------------

	if ( m_Parameters[PORTALGUN_GRAVLIGHT].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_GRAVLIGHT].GetScale().SetAbsolute( 0.018f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_GRAVLIGHT].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_GRAVLIGHT].SetAttachment( pModelView->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_GRAVLIGHT].SetVisible( false );

		if ( m_Parameters[PORTALGUN_GRAVLIGHT].SetMaterial( PORTALGUN_GRAV_ACTIVE_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	if ( m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].GetScale().SetAbsolute( 0.03f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetAttachment( pModelWorld->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetVisible( false );

		if ( m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetMaterial( PORTALGUN_GRAV_ACTIVE_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	if ( m_Parameters[PORTALGUN_PORTAL1LIGHT].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_PORTAL1LIGHT].GetScale().SetAbsolute( 0.018f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_PORTAL1LIGHT].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_PORTAL1LIGHT].SetAttachment( pModelView->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_PORTAL1LIGHT].SetVisible( false );

		if ( m_Parameters[PORTALGUN_PORTAL1LIGHT].SetMaterial( PORTALGUN_PORTAL1_FIRED_LAST_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	if ( m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].GetScale().SetAbsolute( 0.03f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetAttachment( pModelWorld->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetVisible( false );

		if ( m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetMaterial( PORTALGUN_PORTAL1_FIRED_LAST_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	if ( m_Parameters[PORTALGUN_PORTAL2LIGHT].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_PORTAL2LIGHT].GetScale().SetAbsolute( 0.018f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_PORTAL2LIGHT].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_PORTAL2LIGHT].SetAttachment( pModelView->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_PORTAL2LIGHT].SetVisible( false );

		if ( m_Parameters[PORTALGUN_PORTAL2LIGHT].SetMaterial( PORTALGUN_PORTAL2_FIRED_LAST_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	if ( m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].GetMaterial() == NULL )
	{
		m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].GetScale().SetAbsolute( 0.03f * SPRITE_SCALE );
		m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].SetAttachment( pModelWorld->LookupAttachment( "Body_light" ) );
		m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].SetVisible( false );

		if ( m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].SetMaterial( PORTALGUN_PORTAL2_FIRED_LAST_GLOW ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	// ------------------------------------------
	// Glows
	// ------------------------------------------

	const char *attachNamesGlow[NUM_GLOW_SPRITES] = 
	{
		"Arm1_attach1",
		"Arm1_attach2",
		"Arm2_attach1",
		"Arm2_attach2",
		"Arm3_attach1",
		"Arm3_attach2"
	};

	//Create the view glow sprites
	for ( i = PORTALGUN_GLOW1; i < (PORTALGUN_GLOW1+NUM_GLOW_SPRITES); i++ )
	{
		if ( m_Parameters[i].GetMaterial() != NULL )
			continue;

		m_Parameters[i].GetScale().SetAbsolute( 0.05f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 24.0f );

		// Different for different views
		m_Parameters[i].SetAttachment( pModelView->LookupAttachment( attachNamesGlow[i-PORTALGUN_GLOW1] ) );
		m_Parameters[i].SetColor( Vector( 255, 128, 0 ) );
		m_Parameters[i].SetVisible( false );

		/*if ( m_Parameters[i].SetMaterial( PORTALGUN_GLOW_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}*/
	}

	//Create the world glow sprites
	for ( i = PORTALGUN_GLOW1_WORLD; i < (PORTALGUN_GLOW1_WORLD+NUM_GLOW_SPRITES_WORLD); i++ )
	{
		if ( m_Parameters[i].GetMaterial() != NULL )
			continue;

		m_Parameters[i].GetScale().SetAbsolute( 0.1f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 24.0f );

		// Different for different views
		m_Parameters[i].SetAttachment( pModelWorld->LookupAttachment( attachNamesGlow[i-PORTALGUN_GLOW1_WORLD] ) );
		m_Parameters[i].SetColor( Vector( 255, 128, 0 ) );
		m_Parameters[i].SetVisible( false );
		/*
		if ( m_Parameters[i].SetMaterial( PORTALGUN_GLOW_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}*/
	}

	// ------------------------------------------
	// End caps
	// ------------------------------------------

	const char *attachNamesEndCap[NUM_ENDCAP_SPRITES] = 
	{
		"Arm1_attach3",
		"Arm2_attach3",
		"Arm3_attach3",
	};

	//Create the endcap sprites
	for ( i = PORTALGUN_ENDCAP1; i < (PORTALGUN_ENDCAP1+NUM_ENDCAP_SPRITES); i++ )
	{
		if ( m_Parameters[i].GetMaterial() != NULL )
			continue;

		m_Parameters[i].GetScale().SetAbsolute( 0.02f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[i].SetAttachment( pModelView->LookupAttachment( attachNamesEndCap[i-PORTALGUN_ENDCAP1] ) );
		m_Parameters[i].SetVisible( false );

		if ( m_Parameters[i].SetMaterial( PORTALGUN_ENDCAP_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	//Create the world endcap sprites
	for ( i = PORTALGUN_ENDCAP1_WORLD; i < (PORTALGUN_ENDCAP1_WORLD+NUM_ENDCAP_SPRITES_WORLD); i++ )
	{
		if ( m_Parameters[i].GetMaterial() != NULL )
			continue;

		m_Parameters[i].GetScale().SetAbsolute( 0.04f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 128.0f );
		m_Parameters[i].SetAttachment( pModelWorld->LookupAttachment( attachNamesEndCap[i-PORTALGUN_ENDCAP1_WORLD] ) );
		m_Parameters[i].SetVisible( false );

		if ( m_Parameters[i].SetMaterial( PORTALGUN_ENDCAP_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	// ------------------------------------------
	// Internals
	// ------------------------------------------

	//Create the muzzle glow sprites
	i = PORTALGUN_MUZZLE_GLOW;

	if ( m_Parameters[i].GetMaterial() == NULL )
	{
		m_Parameters[i].GetScale().SetAbsolute( 0.025f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 64.0f );
		m_Parameters[i].SetAttachment( pModelView->LookupAttachment( "Inside_effects" ) );
		m_Parameters[i].SetVisible( false );

		if ( m_Parameters[i].SetMaterial( PORTALGUN_PORTAL_MUZZLE_GLOW_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	//Create the world muzzle glow sprites
	i = PORTALGUN_MUZZLE_GLOW_WORLD;
	
	if ( m_Parameters[i].GetMaterial() == NULL )
	{
		m_Parameters[i].GetScale().SetAbsolute( 0.025f * SPRITE_SCALE );
		m_Parameters[i].GetAlpha().SetAbsolute( 64.0f );
		m_Parameters[i].SetAttachment( pModelWorld->LookupAttachment( "Inside_effects" ) );
		m_Parameters[i].SetVisible( false );

		if ( m_Parameters[i].SetMaterial( PORTALGUN_PORTAL_MUZZLE_GLOW_SPRITE ) == false )
		{
			// This means the texture was not found
			Assert( 0 );
		}
	}

	// ------------------------------------------
	// Tube sprites
	// ------------------------------------------

	const char *attachNamesTubeBeam[NUM_TUBE_BEAM_SPRITES] = 
	{
		"Beam_point1",
		"Beam_point2",
		"Beam_point3",
		"Beam_point4",
		"Beam_point5",
	};

	//Create the tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		if ( m_Parameters[i].GetMaterial() == NULL )
		{
			m_Parameters[i].GetScale().SetAbsolute( cl_portalgun_beam_size.GetFloat() * SPRITE_SCALE );
			m_Parameters[i].GetAlpha().SetAbsolute( 255.0f );
			m_Parameters[i].SetAttachment( pModelView->LookupAttachment( attachNamesTubeBeam[i-PORTALGUN_TUBE_BEAM1] ) );
			m_Parameters[i].SetVisible( false );

			if ( m_Parameters[i].SetMaterial( PORTALGUN_PORTAL_TUBE_BEAM_SPRITE ) == false )
			{
				// This means the texture was not found
				Assert( 0 );
			}
		}
	}

	//Create the world tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1_WORLD; i < (PORTALGUN_TUBE_BEAM1_WORLD+NUM_TUBE_BEAM_SPRITES_WORLD); i++ )
	{
		if ( m_Parameters[i].GetMaterial() == NULL )
		{
			m_Parameters[i].GetScale().SetAbsolute( cl_portalgun_beam_size.GetFloat() * SPRITE_SCALE );
			m_Parameters[i].GetAlpha().SetAbsolute( 255.0f );
			m_Parameters[i].SetAttachment( pModelView->LookupAttachment( attachNamesTubeBeam[i-PORTALGUN_TUBE_BEAM1_WORLD] ) );
			m_Parameters[i].SetVisible( false );

			if ( m_Parameters[i].SetMaterial( PORTALGUN_PORTAL_TUBE_BEAM_SPRITE ) == false )
			{
				// This means the texture was not found
				Assert( 0 );
			}
		}
	}

	// ------------------------------------------
	// Beams
	// ------------------------------------------

	// Setup the beams
	int iBeam = 0;

	if ( pModelView != pModelWorld )
	{
		m_Beams[iBeam++].Init( pModelView->LookupAttachment( "Arm1_attach3" ), pModelView->LookupAttachment( "muzzle" ), pModelView, true );
		m_Beams[iBeam++].Init( pModelView->LookupAttachment( "Arm2_attach3" ), pModelView->LookupAttachment( "muzzle" ), pModelView, true );
		m_Beams[iBeam++].Init( pModelView->LookupAttachment( "Arm3_attach3" ), pModelView->LookupAttachment( "muzzle" ), pModelView, true );
	}
	else
	{
		iBeam += 3;
	}

	m_Beams[iBeam++].Init( pModelWorld->LookupAttachment( "Arm1_attach3" ), pModelWorld->LookupAttachment( "muzzle" ), pModelWorld, false );
	m_Beams[iBeam++].Init( pModelWorld->LookupAttachment( "Arm2_attach3" ), pModelWorld->LookupAttachment( "muzzle" ), pModelWorld, false );
	m_Beams[iBeam++].Init( pModelWorld->LookupAttachment( "Arm3_attach3" ), pModelWorld->LookupAttachment( "muzzle" ), pModelWorld, false );
#endif
}

void C_WeaponPortalgun::DestroyEffects( void )
{
#if 1
#else
	// Free our beams
	for ( int i = 0; i < NUM_PORTALGUN_BEAMS; ++i )
	{
		m_Beams[i].Release();
	}
#endif
	// Stop everything
	StopEffects();
}

//-----------------------------------------------------------------------------
// Purpose: Ready effects
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectReady( void )
{
#if 1

	C_BasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer )
	{
		RumbleEffect( pPlayer->GetUserID(), 0x14u, 0, 0 );
	}

#else
	int i;

	// Turn on the glow sprites
	for ( i = PORTALGUN_GLOW1; i < (PORTALGUN_GLOW1+NUM_GLOW_SPRITES); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( 0.4f * SPRITE_SCALE, 0.2f );
		m_Parameters[i].GetAlpha().InitFromCurrent( 32.0f, 0.2f );
		m_Parameters[i].SetVisibleViewModel();
	}

	for ( i = PORTALGUN_GLOW1_WORLD; i < (PORTALGUN_GLOW1_WORLD+NUM_GLOW_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( 0.8f * SPRITE_SCALE, 0.4f );
		m_Parameters[i].GetAlpha().InitFromCurrent( 32.0f, 0.2f );
		m_Parameters[i].SetVisible3rdPerson();
	}

	// Turn on the endcap sprites
	for ( i = PORTALGUN_ENDCAP1; i < (PORTALGUN_ENDCAP1+NUM_ENDCAP_SPRITES); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	// Turn on the world endcap sprites
	for ( i = PORTALGUN_ENDCAP1_WORLD; i < (PORTALGUN_ENDCAP1_WORLD+NUM_ENDCAP_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	// Turn on the internal sprites
	i = PORTALGUN_MUZZLE_GLOW;
	
	Vector colorMagSprites = GetEffectColor( i );
	m_Parameters[i].SetColor( colorMagSprites );
	m_Parameters[i].SetVisibleViewModel();
	
	// Turn on the world internal sprites
	i = PORTALGUN_MUZZLE_GLOW_WORLD;

	m_Parameters[i].SetColor( colorMagSprites );
	m_Parameters[i].SetVisible3rdPerson();

	// Turn on the tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		m_Parameters[i].SetColor( colorMagSprites );
		m_Parameters[i].SetVisibleViewModel();
	}

	// Turn on the world tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1_WORLD; i < (PORTALGUN_TUBE_BEAM1_WORLD+NUM_TUBE_BEAM_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetColor( colorMagSprites );
		m_Parameters[i].SetVisible3rdPerson();
	}

    // Turn off beams off
	for ( i = 0; i < NUM_PORTALGUN_BEAMS; ++i )
	{
		m_Beams[i].SetVisibleViewModel( false );
		m_Beams[i].SetVisible3rdPerson( false );
	}

	/*CPortal_Player* pPlayer = (CPortal_Player*)GetOwner();
	if ( pPlayer )
	{
		RumbleEffect( RUMBLE_PHYSCANNON_OPEN, 0, RUMBLE_FLAG_STOP );
	}*/
#endif
}


//-----------------------------------------------------------------------------
// Holding effects
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectHolding( void )
{
	Msg("DO EFFECT HOLDING!!!\n");
	Msg("Effect State %i\n", m_EffectState);

#if 1 // Portal 2 Effects:
  
	C_BaseViewModel *vm = NULL; // ebx
	C_BaseCombatCharacter *pOwner = GetOwner();
  
	C_BasePlayer *pPlayer = ToBasePlayer( pOwner ); // edi

	if ( pPlayer )
	{
		vm = pPlayer->GetViewModel();
	}
  
	if ( !m_hPortalGunEffectHoldingFP.IsValid() && vm )
	{

		Msg("CREATE BEAM!!!!\n");

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

	


#else // Portal 1 Effects
	int i;

	// Turn on the glow sprites
	for ( i = PORTALGUN_GLOW1; i < (PORTALGUN_GLOW1+NUM_GLOW_SPRITES); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( 0.5f * SPRITE_SCALE, 0.2f );
		m_Parameters[i].GetAlpha().InitFromCurrent( 64.0f, 0.2f );
		m_Parameters[i].SetVisibleViewModel();
	}

	for ( i = PORTALGUN_GLOW1_WORLD; i < (PORTALGUN_GLOW1_WORLD+NUM_GLOW_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( 1.0f * SPRITE_SCALE, 0.4f );
		m_Parameters[i].GetAlpha().InitFromCurrent( 64.0f, 0.2f );
		m_Parameters[i].SetVisible3rdPerson();
	}

	// Turn on the endcap sprites
	for ( i = PORTALGUN_ENDCAP1; i < (PORTALGUN_ENDCAP1+NUM_ENDCAP_SPRITES); i++ )
	{
		m_Parameters[i].SetVisibleViewModel();
	}

	// Turn on the world endcap sprites
	for ( i = PORTALGUN_ENDCAP1_WORLD; i < (PORTALGUN_ENDCAP1_WORLD+NUM_ENDCAP_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetVisible3rdPerson();
	}

	// Turn on the internal sprites
	i = PORTALGUN_MUZZLE_GLOW;

	Vector colorMagSprites = GetEffectColor( i );
	m_Parameters[i].SetColor( colorMagSprites );
	m_Parameters[i].SetVisibleViewModel();

	// Turn on the world internal sprites
	i = PORTALGUN_MUZZLE_GLOW_WORLD;
	
	m_Parameters[i].SetColor( colorMagSprites );
	m_Parameters[i].SetVisible3rdPerson();

	// Turn on the tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		m_Parameters[i].SetColor( colorMagSprites );
		m_Parameters[i].SetVisibleViewModel();
	}

	// Turn on the world tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		m_Parameters[i].SetColor( colorMagSprites );
		m_Parameters[i].SetVisible3rdPerson();
	}

	// Set beams them visible
	for ( i = 0; i < NUM_PORTALGUN_BEAMS / 2; ++i )
	{
		m_Beams[i].SetVisible3rdPerson( false );
		m_Beams[i].SetVisibleViewModel();
		m_Beams[i].SetBrightness( 128.0f );
	}

	for ( i; i < NUM_PORTALGUN_BEAMS; ++i )
	{
		m_Beams[i].SetVisibleViewModel( false );
		m_Beams[i].SetVisible3rdPerson();
		m_Beams[i].SetBrightness( 128.0f );
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Shutdown for the weapon when it's holstered
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectNone( void )
{
#if 1

	Msg(" DO EFFECT NONE \n");

	
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
#else
	int i;

	//Turn off main glows
	m_Parameters[PORTALGUN_GRAVLIGHT].SetVisible( false );
	m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetVisible( false );
	m_Parameters[PORTALGUN_PORTAL1LIGHT].SetVisible( false );
	m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetVisible( false );
	m_Parameters[PORTALGUN_PORTAL2LIGHT].SetVisible( false );
	m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetVisible( false );

	for ( i = PORTALGUN_GLOW1; i < (PORTALGUN_GLOW1+NUM_GLOW_SPRITES); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	for ( i = PORTALGUN_GLOW1_WORLD; i < (PORTALGUN_GLOW1_WORLD+NUM_GLOW_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	for ( i = PORTALGUN_ENDCAP1; i < (PORTALGUN_ENDCAP1+NUM_ENDCAP_SPRITES); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	for ( i = PORTALGUN_ENDCAP1_WORLD; i < (PORTALGUN_ENDCAP1_WORLD+NUM_ENDCAP_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	m_Parameters[PORTALGUN_MUZZLE_GLOW].SetVisible( false );
	m_Parameters[PORTALGUN_MUZZLE_GLOW_WORLD].SetVisible( false );

	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	for ( i = PORTALGUN_TUBE_BEAM1_WORLD; i < (PORTALGUN_TUBE_BEAM1_WORLD+NUM_TUBE_BEAM_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].SetVisible( false );
	}

	for ( i = 0; i < NUM_PORTALGUN_BEAMS; ++i )
	{
		m_Beams[i].SetVisibleViewModel( false );
		m_Beams[i].SetVisible3rdPerson( false );
	}
#endif
}

void C_WeaponPortalgun::OnPreDataChanged( DataUpdateType_t updateType )
{
	// TODO: Wtf?
	BaseClass::OnPreDataChanged( updateType );
}

void C_WeaponPortalgun::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

#if 1
	
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

#else
	if ( updateType == DATA_UPDATE_CREATED )
	{
		// Start thinking (Baseclass stops it)
		SetNextClientThink( CLIENT_THINK_ALWAYS );

		{
			C_BaseAnimating::AutoAllowBoneAccess boneaccess( true, true );
			StartEffects();
		}

		DoEffect( m_EffectState );
	}
#endif
	// Update effect state when out of parity with the server
	if ( m_nOldEffectState != m_EffectState || m_bOldCanFirePortal1 != m_bCanFirePortal1 || m_bOldCanFirePortal2 != m_bCanFirePortal2 )
	{
		Msg("m_EffectState: %i\n", m_EffectState);
		Msg("DoEffect!!!\n");
		DoEffect( m_EffectState );
		//m_nOldEffectState = m_EffectState;

		m_bOldCanFirePortal1 = m_bCanFirePortal1;
		m_bOldCanFirePortal2 = m_bCanFirePortal2;
	}
}

void C_WeaponPortalgun::ClientThink( void )
{

	CPortal_Player *pPlayer = ToPortalPlayer( GetOwner() );

	if ( pPlayer && dynamic_cast<C_WeaponPortalgun*>( pPlayer->GetActiveWeapon() ) )
	{
		if ( m_EffectState == EFFECT_NONE )
		{
#if 1 // Portal 2

			if( m_hPortalGunEffectTP.IsValid() )
			{
				m_hPortalGunEffectTP->StopEmission();
				m_hPortalGunEffectTP = NULL;
			}

			if ( m_hPortalGunEffectHoldingFP.IsValid() )
			{
				m_hPortalGunEffectHoldingFP->StopEmission();
				m_hPortalGunEffectHoldingFP = NULL;
			}

#else // Portal 1
			//DoEffect( ( GetPlayerHeldEntity( pPlayer ) ) ? ( EFFECT_HOLDING ) : ( EFFECT_READY ) );
#endif
		}
#if 0
		if ( m_EffectState != EFFECT_NONE )
		{
			// Showing a special color for holding is confusing... just use the last fired color -Jeep

			//if ( m_bOpenProngs )
			//{
			//	//Turn on the grav light
			//	m_Parameters[PORTALGUN_GRAVLIGHT].SetVisibleViewModel();
			//	m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetVisible3rdPerson();

			//	m_Parameters[PORTALGUN_PORTAL1LIGHT].SetVisibleViewModel( false );
			//	m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetVisible3rdPerson( false );
			//	m_Parameters[PORTALGUN_PORTAL2LIGHT].SetVisibleViewModel( false );
			//	m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].SetVisible3rdPerson( false );
			//}
			//else
			{
				m_Parameters[PORTALGUN_GRAVLIGHT].SetVisibleViewModel( false );
				m_Parameters[PORTALGUN_GRAVLIGHT_WORLD].SetVisible3rdPerson( false );

				//Turn on and off the correct fired last lights
				m_Parameters[PORTALGUN_PORTAL1LIGHT].SetVisibleViewModel( m_iLastFiredPortal == 1 );
				m_Parameters[PORTALGUN_PORTAL1LIGHT_WORLD].SetVisible3rdPerson( m_iLastFiredPortal == 1 );
				m_Parameters[PORTALGUN_PORTAL2LIGHT].SetVisibleViewModel( m_iLastFiredPortal == 2 );
				m_Parameters[PORTALGUN_PORTAL2LIGHT_WORLD].SetVisible3rdPerson( m_iLastFiredPortal == 2 );
			}
		}
#endif
	}

	// Update our effects
	//DoEffectIdle();
	
	StartEffects();
	
	if ( m_bPulseUp )
	{
		float flNewPulse = gpGlobals->frametime + 16 + m_fPulse;
		m_fPulse = flNewPulse;
		if ( flNewPulse > 1.0 )
		{
		  m_bPulseUp = false;
		  m_fPulse = 1.0;
		}
	}
	else
	{
		float flNewPulse = m_fPulse - gpGlobals->frametime + 16;
		m_fPulse = flNewPulse;
		if ( flNewPulse < 0.0 )
		{
			m_bPulseUp = true;
			m_fPulse = 0.0;
		}
	}

	NetworkStateChanged();
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
#if 0
//-----------------------------------------------------------------------------
// Purpose: Gets the complete list of values needed to render an effect from an
//			effect parameter
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::GetEffectParameters( EffectType_t effectID, color32 &color, float &scale, IMaterial **pMaterial, Vector &vecAttachment, bool b3rdPerson )
{
	const float dt = gpGlobals->curtime;

	// Get alpha
	float alpha = m_Parameters[effectID].GetAlpha().Interp( dt );

	// Get scale
	scale = m_Parameters[effectID].GetScale().Interp( dt );

	// Get material
	*pMaterial = (IMaterial *) m_Parameters[effectID].GetMaterial();

	// Setup the color
	color.r = (int) m_Parameters[effectID].GetColor().x;
	color.g = (int) m_Parameters[effectID].GetColor().y;
	color.b = (int) m_Parameters[effectID].GetColor().z;
	color.a = (int) alpha;

	// Setup the attachment
	int		attachment = m_Parameters[effectID].GetAttachment();
	QAngle	angles;

	// Format for first-person
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner != NULL )
	{
		C_BaseAnimating *pModel;

		if ( b3rdPerson )
		{
			pModel = this;
		}
		else
		{
			pModel = pOwner->GetViewModel();
		}

		pModel->GetAttachment( attachment, vecAttachment, angles );

		if ( !b3rdPerson )
		{
			::FormatViewModelAttachment( ToBasePlayer(GetOwner()), vecAttachment, true );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Whether or not an effect is set to display
//-----------------------------------------------------------------------------
bool C_WeaponPortalgun::IsEffectVisible( EffectType_t effectID, bool b3rdPerson )
{
	if ( b3rdPerson )
	{
		return !!m_Parameters[effectID].IsVisible3rdPerson();
	}
	else
	{
		return !!m_Parameters[effectID].IsVisibleViewModel();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draws the effect sprite, given an effect parameter ID
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DrawEffectSprite( EffectType_t effectID, bool b3rdPerson )
{
	color32 color;
	float scale;
	IMaterial *pMaterial;
	Vector	vecAttachment;

	// Don't draw invisible effects
	if ( !IsEffectVisible( effectID, b3rdPerson ) )
		return;

	// Get all of our parameters
	GetEffectParameters( effectID, color, scale, &pMaterial, vecAttachment, b3rdPerson );

	// Don't render fully translucent objects
	if ( color.a <= 0.0f )
		return;

	// Draw the sprite
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Bind( pMaterial, this );
	DrawSprite( vecAttachment, scale, scale, color );
}

//-----------------------------------------------------------------------------
// Purpose: Render our third-person effects
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DrawEffects( bool b3rdPerson )
{
	for ( int i = 0; i < NUM_PORTALGUN_PARAMETERS; i++ )
	{
		DrawEffectSprite( (EffectType_t) i, b3rdPerson );
	}
}
//-----------------------------------------------------------------------------
// Purpose: Third-person function call to render world model
//-----------------------------------------------------------------------------
int C_WeaponPortalgun::DrawModel( int flags, const RenderableInstance_t& instance )
{
	// Only render these on the transparent pass
	/*if ( flags & STUDIO_TRANSPARENCY )
	{
		DrawEffects( true );
		return 1;
	}*/

	int iRetValue = BaseClass::DrawModel( flags, instance);

	if ( iRetValue )
	{
		DrawEffects( true );
	}

	return iRetValue;
}

//-----------------------------------------------------------------------------
// Purpose: First-person function call after viewmodel has been drawn
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::ViewModelDrawn( int nFlags, C_BaseViewModel *pViewModel )
{
	// Render our effects
	DrawEffects( false );

	// Pass this back up
	BaseClass::ViewModelDrawn( nFlags, pBaseViewModel );
}

#endif

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

#if 0
//-----------------------------------------------------------------------------
// Purpose: Idle effect (pulsing)
//-----------------------------------------------------------------------------
void C_WeaponPortalgun::DoEffectIdle( void )
{
	StartEffects();


	int i;

	// Turn on the glow sprites
	for ( i = PORTALGUN_GLOW1; i < (PORTALGUN_GLOW1+NUM_GLOW_SPRITES); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( RandomFloat( 0.0075f, 0.05f ) * SPRITE_SCALE, 0.1f );
		m_Parameters[i].GetAlpha().SetAbsolute( RandomInt( 10, 24 ) );
	}

	// Turn on the world glow sprites
	for ( i = PORTALGUN_GLOW1_WORLD; i < (PORTALGUN_GLOW1_WORLD+NUM_GLOW_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].GetScale().InitFromCurrent( RandomFloat( 0.015f, 0.1f ) * SPRITE_SCALE, 0.1f );
		m_Parameters[i].GetAlpha().SetAbsolute( RandomInt( 10, 24 ) );
	}

	// Turn on the endcap sprites
	for ( i = PORTALGUN_ENDCAP1; i < (PORTALGUN_ENDCAP1+NUM_ENDCAP_SPRITES); i++ )
	{
		m_Parameters[i].GetScale().SetAbsolute( RandomFloat( 1.0f, 3.0f ) );
		m_Parameters[i].GetAlpha().SetAbsolute( RandomInt( 96, 128 ) );
	}

	// Turn on the world endcap sprites
	for ( i = PORTALGUN_ENDCAP1_WORLD; i < (PORTALGUN_ENDCAP1_WORLD+NUM_ENDCAP_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].GetScale().SetAbsolute( RandomFloat( 2.0f, 6.0f ) );
		m_Parameters[i].GetAlpha().SetAbsolute( RandomInt( 96, 128 ) );
	}

	// Turn on the internal sprites
	i = PORTALGUN_MUZZLE_GLOW;

	if ( m_bPulseUp )
	{
		m_fPulse += gpGlobals->frametime;
		if ( m_fPulse > 1.0f )
		{
			m_fPulse = 1.0f;
			m_bPulseUp = !m_bPulseUp;
		}
	}
	else
	{
		m_fPulse -= gpGlobals->frametime;
		if ( m_fPulse < 0.0f )
		{
			m_fPulse = 0.0f;
			m_bPulseUp = !m_bPulseUp;
		}
	}

	m_Parameters[i].GetScale().SetAbsolute( cl_portalgun_effects_min_size.GetFloat() + ( m_fEffectsMaxSize1 - cl_portalgun_effects_min_size.GetFloat() ) * m_fPulse );
	m_Parameters[i].GetAlpha().SetAbsolute( cl_portalgun_effects_min_alpha.GetInt() + ( cl_portalgun_effects_max_alpha.GetInt() - cl_portalgun_effects_min_alpha.GetInt() ) * m_fPulse );
	Vector colorMagSprites = GetEffectColor( i );
	m_Parameters[i].SetColor( colorMagSprites );

	// Turn on the world internal sprites
	i = PORTALGUN_MUZZLE_GLOW_WORLD;

	m_Parameters[i].GetScale().SetAbsolute( cl_portalgun_effects_min_size.GetFloat() + ( m_fEffectsMaxSize1 - cl_portalgun_effects_min_size.GetFloat() ) * m_fPulse );
	m_Parameters[i].GetAlpha().SetAbsolute( cl_portalgun_effects_min_alpha.GetInt() + ( cl_portalgun_effects_max_alpha.GetInt() - cl_portalgun_effects_min_alpha.GetInt() ) * m_fPulse );
	m_Parameters[i].SetColor( colorMagSprites );

	// Turn on the tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1; i < (PORTALGUN_TUBE_BEAM1+NUM_TUBE_BEAM_SPRITES); i++ )
	{
		m_Parameters[i].GetAlpha().SetAbsolute( cl_portalgun_effects_min_alpha.GetInt() + ( cl_portalgun_effects_max_alpha.GetInt() - cl_portalgun_effects_min_alpha.GetInt() ) * m_fPulse );
		m_Parameters[i].SetColor( colorMagSprites );
	}

	// Turn on the world tube beam sprites
	for ( i = PORTALGUN_TUBE_BEAM1_WORLD; i < (PORTALGUN_TUBE_BEAM1_WORLD+NUM_TUBE_BEAM_SPRITES_WORLD); i++ )
	{
		m_Parameters[i].GetAlpha().SetAbsolute( cl_portalgun_effects_min_alpha.GetInt() + ( cl_portalgun_effects_max_alpha.GetInt() - cl_portalgun_effects_min_alpha.GetInt() ) * m_fPulse );
		m_Parameters[i].SetColor( colorMagSprites );
	}
}
#endif


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