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

#include "c_trigger_portal_cleanser.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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
	// FIXME: Undone
	return false;
}

void C_FizzlerVortexProxy::OnBind( void *pEnt )
{
	//C_TriggerPortalCleanser *pTrigger = static_cast<C_TriggerPortalCleanser*>( pEnt );
	// FIXME: Undone
}


IMPLEMENT_CLIENTCLASS_DT(C_TriggerPortalCleanser, DT_TriggerPortalCleanser, CTriggerPortalCleanser)
	RecvPropBool(RECVINFO(m_bDisabled))
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