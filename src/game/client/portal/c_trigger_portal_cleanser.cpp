//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume which bumps portal placement. Keeps a global list loaded in from the map
//			and provides an interface with which prop_portal can get this list and avoid successfully
//			creating portals partially inside the volume.
//
// $NoKeywords: $
//======================================================================================//

#include "cbase.h"
#include "c_triggers.h"
#include "c_portal_player.h"
#include "c_weapon_portalgun.h"
#include "prop_portal_shared.h"
#include "portal_shareddefs.h"
#include "model_types.h"
#include "rumble_shared.h"
#include "materialsystem/imaterialvar.h"
#include "imaterialproxydict.h"

#include "c_trigger_portal_cleanser.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_portal_cleanser_powerup_time( "cl_portal_cleanser_powerup_time", "1.0f", FCVAR_DEVELOPMENTONLY, "The amount of time the power up sequence takes to complete." );
ConVar cl_portal_cleanser_default_intensity( "cl_portal_cleanser_default_intensity", "1.0f", FCVAR_DEVELOPMENTONLY, "The default intensity of the cleanser field effect." );
ConVar cl_portal_cleanser_shot_pulse_time( "cl_portal_cleanser_shot_pulse_time", "0.1f", FCVAR_DEVELOPMENTONLY, "The amount of time to pulse the cleanser field for when it is shot at." ); 
ConVar cl_portal_cleanser_shot_pulse_intensity( "cl_portal_cleanser_shot_pulse_intensity", "10.0f", FCVAR_DEVELOPMENTONLY, "The intensity of the cleanser field when it gets shot at." );

#undef CTriggerPortalCleanser

IMPLEMENT_CLIENTCLASS_DT( ClientFizzlerMultiOriginSoundPlayer, DT_FizzlerMultiOriginSoundPlayer, FizzlerMultiOriginSoundPlayer )
END_RECV_TABLE()

ClientFizzlerMultiOriginSoundPlayer::ClientFizzlerMultiOriginSoundPlayer()
{

}

ClientFizzlerMultiOriginSoundPlayer::~ClientFizzlerMultiOriginSoundPlayer()
{

}

C_FizzlerVortexProxy::C_FizzlerVortexProxy()
{
	m_pMaterial = NULL;
	m_pPosition1Var = NULL;
	m_pVortex1Var = NULL;
	m_pPosition2Var = NULL;
	m_pVortex2Var = NULL;
	m_pIntensityVar = NULL;
}

C_FizzlerVortexProxy::~C_FizzlerVortexProxy()
{

}

bool C_FizzlerVortexProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	m_pMaterial = pMaterial;
	
	bool bFoundVortex1 = false;
	m_pPosition1Var = m_pMaterial->FindVar( "$FLOW_VORTEX_POS1", &bFoundVortex1, true );
	if ( bFoundVortex1 )
	{
		bool bFoundVortex1 = false;
		m_pVortex1Var = m_pMaterial->FindVar( "$flow_vortex1", &bFoundVortex1, true );
		if ( bFoundVortex1 )
		{
			bool bFoundVortexPos2 = false;
			m_pPosition2Var = m_pMaterial->FindVar( "$FLOW_VORTEX_POS2", &bFoundVortexPos2, true );
			if ( bFoundVortexPos2 )
			{
				bool bFoundVortex2 = false;
				m_pVortex2Var = m_pMaterial->FindVar( "$flow_vortex2", &bFoundVortex2, true );
				if ( bFoundVortex2 )
				{
					bool bFoundColorIntensity = false;
					m_pIntensityVar = m_pMaterial->FindVar( "$flow_color_intensity", &bFoundColorIntensity, true );
					if ( bFoundColorIntensity )
					{
						bool bFoundPowerup = false;
						m_pPowerUpVar = m_pMaterial->FindVar( "$powerup", &bFoundPowerup, true );
						if ( bFoundPowerup )
						{
							return true;
						}
						else
						{
							m_pPowerUpVar = NULL;
							return false;
						}
					}
					else
					{
						this->m_pIntensityVar = NULL;
						return false;
					}
				}
				else
				{
					m_pVortex2Var = NULL;
					return false;
				}
			}
			else
			{
				m_pPosition2Var = NULL;
				return false;
			}
		}
		else
		{
			m_pVortex1Var = NULL;
			return false;
		}
	}
	else
	{
		m_pPosition1Var = NULL;
		return false;
	}
}

void C_FizzlerVortexProxy::OnBind( C_BaseEntity *pC_BaseEntity )
{
	if (pC_BaseEntity)
	{
		C_TriggerPortalCleanser *pCleanser = dynamic_cast<C_TriggerPortalCleanser*>( pC_BaseEntity );

		if (pCleanser)
		{
			if (pCleanser->m_bObject1InRange && pCleanser->m_hObject1)
			{
				m_pVortex1Var->SetIntValue( 1 );
				Vector vecOrigin = pCleanser->m_hObject1->GetAbsOrigin();
				m_pPosition1Var->SetVecValue( vecOrigin.x, vecOrigin.y, vecOrigin.z );
			}
			else
			{
				m_pVortex1Var->SetIntValue( 0 );
			}

			if (pCleanser->m_bObject2InRange && pCleanser->m_hObject2)
			{
				m_pVortex2Var->SetIntValue( 1 );
				Vector vecOrigin = pCleanser->m_hObject2->GetAbsOrigin();
				m_pPosition2Var->SetVecValue( vecOrigin.x, vecOrigin.y, vecOrigin.z );
			}
			else
			{
				m_pVortex2Var->SetIntValue( 0 );
			}

			float flPower;
			float flIntensity;
			pCleanser->GetCurrentState( flIntensity, flPower);
			
			//Msg("flIntensity: %f flPower: %f\n", flIntensity, flPower );
			m_pIntensityVar->SetFloatValue( flIntensity );
			m_pPowerUpVar->SetFloatValue( flPower );
		}
	}
}

EXPOSE_MATERIAL_PROXY( C_FizzlerVortexProxy, FizzlerVortex );

IMPLEMENT_CLIENTCLASS_DT(C_TriggerPortalCleanser, DT_TriggerPortalCleanser, CTriggerPortalCleanser)
	RecvPropBool( RECVINFO( m_bDisabled ) ),
	RecvPropBool( RECVINFO( m_bVisible ) ),
	RecvPropBool( RECVINFO( m_bObject1InRange ) ),
	RecvPropBool( RECVINFO( m_bObject2InRange ) ),
	RecvPropBool( RECVINFO( m_bPlayersPassTriggerFilters ) ),

	RecvPropFloat( RECVINFO( m_flPortalShotTime ) ),
	
	RecvPropEHandle( RECVINFO( m_hObject1 ) ),	
	RecvPropEHandle( RECVINFO( m_hObject2 ) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( trigger_portal_cleanser, C_TriggerPortalCleanser );

IMPLEMENT_AUTO_LIST( ITriggerPortalCleanserAutoList )

C_TriggerPortalCleanser::C_TriggerPortalCleanser()
{

}

C_TriggerPortalCleanser::~C_TriggerPortalCleanser()
{

}


bool C_TriggerPortalCleanser::GetSoundSpatialization( SpatializationInfo_t& info )
{
	// FIXME: This is incomplete, but we need a body for compiling!
	return BaseClass::GetSoundSpatialization( info );
}

void C_TriggerPortalCleanser::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	if (updateType == DATA_UPDATE_CREATED)
		m_flLastUpdateTime = gpGlobals->curtime;
}

void C_TriggerPortalCleanser::GetCurrentState( float &flIntensity, float &flPower )
{	
	float flTime = gpGlobals->curtime - m_flLastUpdateTime;
	
	m_flLastUpdateTime = gpGlobals->curtime;
	flIntensity = cl_portal_cleanser_default_intensity.GetFloat();
	if (m_flPortalShotTime > m_flLastShotTime)
	{
		m_flLastShotTime = m_flPortalShotTime;
		m_flShotPulseTimer = 0.0;
	}
	if (m_flPortalShotTime != 0.0)
	{
		m_flShotPulseTimer = m_flShotPulseTimer;
		if ((cl_portal_cleanser_shot_pulse_time.GetFloat() * 2.0) >= m_flShotPulseTimer)
		{
			float flIntensityMultiplier;
			float v9 = m_flShotPulseTimer + flTime;
			m_flShotPulseTimer = v9;
			if (cl_portal_cleanser_shot_pulse_time.GetFloat() <= v9)
				flIntensityMultiplier = 1.0
				- ((v9 - cl_portal_cleanser_shot_pulse_time.GetFloat())
				/ cl_portal_cleanser_shot_pulse_time.GetFloat());
			else
				flIntensityMultiplier = v9 / cl_portal_cleanser_shot_pulse_time.GetFloat();
			if (flIntensityMultiplier >= 0.0)
			{
				if (flIntensityMultiplier > 1.0)
					flIntensityMultiplier = 1.0;
			}
			else
			{
				flIntensityMultiplier = 0.0;
			}
			flIntensity = ((cl_portal_cleanser_shot_pulse_intensity.GetFloat()
				- cl_portal_cleanser_default_intensity.GetFloat())
				* flIntensityMultiplier)
				+ cl_portal_cleanser_default_intensity.GetFloat();
		}
	}
	float flMaxPower = 1.0;
	if (m_bDisabled)
	{
		if (m_flPowerUpTimer > 0.0)
			m_flPowerUpTimer = m_flPowerUpTimer - flTime;
		if (m_hCleanserFX)
		{
			m_hCleanserFX->StopEmission( false, false, true, true );
			m_hCleanserFX = NULL;
			flMaxPower = 1.0;
		}
	}
	else if (cl_portal_cleanser_powerup_time.GetFloat() > m_flPowerUpTimer)
	{
		m_flPowerUpTimer = m_flPowerUpTimer + flTime;
	}
	float flPowerUpTimer = m_flPowerUpTimer;
	if (flPowerUpTimer >= 0.0)
	{
		if (flPowerUpTimer > cl_portal_cleanser_powerup_time.GetFloat())
			flPowerUpTimer = cl_portal_cleanser_powerup_time.GetFloat();
	}
	else
	{
		flPowerUpTimer = 0.0;
	}
	m_flPowerUpTimer = flPowerUpTimer;

	float flNewPower = flPowerUpTimer / cl_portal_cleanser_powerup_time.GetFloat();
	flPower = flNewPower;
	if (flNewPower >= 0.0)
	{
		if ( flNewPower > flMaxPower )
			flNewPower = flMaxPower;
		flPower = flNewPower;
		UpdateParticles();
	}
	else
	{
		flPower = 0.0;
		UpdateParticles();
	}
}

void C_TriggerPortalCleanser::UpdateParticles( void )
{
	// FIXME: Undone
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TriggerPortalCleanser::Touch( C_BaseEntity *pOther )
{
	if ( !IsEnabled() && !m_bPlayersPassTriggerFilters )
		return;

	if ( pOther->IsPlayer() )
	{
		CPortal_Player *pPlayer = ToPortalPlayer( pOther );

		if ( pPlayer )
		{
			CWeaponPortalgun *pPortalgun = dynamic_cast<CWeaponPortalgun*>( pPlayer->Weapon_OwnsThisType( "weapon_portalgun" ) );

			if ( pPortalgun )
			{
				bool bFizzledPortal1 = false;
				bool bFizzledPortal2 = false;

				if ( pPortalgun->CanFirePortal1() )
				{
					CProp_Portal *pPortal = pPortalgun->GetAssociatedPortal( false );

					if ( pPortal && pPortal->IsActive() )
					{
						pPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, false );
						pPortal->SetActive( false );
						pPortal->EmitSound( "Portal.FizzlerShimmy" );

						bFizzledPortal1 = true;
					}
				}

				if ( pPortalgun->CanFirePortal2() )
				{
					CProp_Portal *pPortal = pPortalgun->GetAssociatedPortal( true );

					if ( pPortal && pPortal->IsActive() )
					{
						pPortal->DoFizzleEffect( PORTAL_FIZZLE_KILLED, false );
						pPortal->SetActive( false );
						pPortal->EmitSound( "Portal.FizzlerShimmy" );

						bFizzledPortal2 = true;
					}
				}

				if ( bFizzledPortal1 || bFizzledPortal2 )
				{
					pPortalgun->SendWeaponAnim( ACT_VM_FIZZLE );
					pPortalgun->DoCleanseEffect( bFizzledPortal1, bFizzledPortal2 );
					pPortalgun->SetLastFiredPortal( 0 );
				}
			}
		}

		return;
	}
}

void C_TriggerPortalCleanser::UpdatePartitionListEntry( void )
{
	partition->RemoveAndInsert( PARTITION_CLIENT_NON_STATIC_EDICTS | PARTITION_CLIENT_STATIC_PROPS , // remove
		PARTITION_CLIENT_TRIGGER_ENTITIES, // Add
		CollisionProp()->GetPartitionHandle());
}