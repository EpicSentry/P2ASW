//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side implementation of CBaseCombatWeapon.
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "history_resource.h"
#include "iclientmode.h"
#include "iinput.h"
#include "weapon_selection.h"
#include "hud_crosshair.h"
#include "engine/ivmodelinfo.h"
#include "tier0/vprof.h"
#include "hltvcamera.h"
#include "tier1/KeyValues.h"
#include "toolframework/itoolframework.h"
#include "toolframework_client.h"
#include "c_combatweaponworldclone.h"
#include "clientalphaproperty.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_viewmodelsclonedasworld( "cl_viewmodelsclonedasworld", "1", FCVAR_NONE );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::SetDormant( bool bDormant )
{
	// If I'm going from active to dormant and I'm carried by another player, holster me.
	if ( !IsDormant() && bDormant && !IsCarriedByLocalPlayer() )
	{
		Holster( NULL );
	}

	BaseClass::SetDormant( bDormant );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::NotifyShouldTransmit( ShouldTransmitState_t state )
{
	BaseClass::NotifyShouldTransmit(state);

	if (state == SHOULDTRANSMIT_END)
	{
		if (m_iState == WEAPON_IS_ACTIVE)
		{
			m_iState = WEAPON_IS_CARRIED_BY_PLAYER;
		}
	}
	else if( state == SHOULDTRANSMIT_START )
	{
		if( m_iState == WEAPON_IS_CARRIED_BY_PLAYER )
		{
			if( GetOwner() && GetOwner()->GetActiveWeapon() == this )
			{
				// Restore the Activeness of the weapon if we client-twiddled it off in the first case above.
				m_iState = WEAPON_IS_ACTIVE;
			}
		}
	}
}



static inline bool ShouldDrawLocalPlayer( C_BasePlayer *pl )
{
#if defined( PORTAL )
	return true;
#else
	Assert( pl );
	return pl->ShouldDrawLocalPlayer();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::OnRestore()
{
	BaseClass::OnRestore();

	// if the player is holding this weapon, 
	// mark it as just restored so it won't show as a new pickup
	if ( C_BasePlayer::IsLocalPlayer( GetOwner() ) )
	{
		m_bJustRestored = true;
	}
}

int C_BaseCombatWeapon::GetWorldModelIndex( void )
{
	return m_iWorldModelIndex;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bnewentity - 
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	// If it's being carried by the *local* player, on the first update,
	// find the registered weapon for this ID

	C_BaseCombatCharacter *pOwner = GetOwner();
	C_BasePlayer *pPlayer = ToBasePlayer( pOwner );

	// check if weapon is carried by local player
	bool bIsLocalPlayer = C_BasePlayer::IsLocalPlayer( pPlayer );
	if ( bIsLocalPlayer )
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD( C_BasePlayer::GetSplitScreenSlotForPlayer( pPlayer ) );

		// If I was just picked up, or created & immediately carried, add myself to this client's list of weapons
		if ( ( m_iState != WEAPON_NOT_CARRIED ) && 
			 ( m_iOldState == WEAPON_NOT_CARRIED ) )
		{
			// Tell the HUD this weapon's been picked up
			if ( ShouldDrawPickup() )
			{
				CBaseHudWeaponSelection *pHudSelection = GetHudWeaponSelection();
				if ( pHudSelection )
				{
					pHudSelection->OnWeaponPickup( this );
				}

				pPlayer->EmitSound( "Player.PickupWeapon" );
			}
		}
	}
	
	UpdateVisibility();

	m_iOldState = m_iState;

	m_bJustRestored = false;

	if (!cl_viewmodelsclonedasworld.GetInt()
		|| ( GetWorldModelIndex() == GetModelIndex() )
		|| GetClientAlphaProperty())
	{
		if (!m_pWorldModelClone)
			return;
		
		//(*((void(__cdecl **)(C_CombatWeaponClone *))m_pWorldModelClone->_vptr_IHandleEntity + 11))(this->m_pWorldModelClone);
		m_pWorldModelClone = NULL;
		UpdateVisibility();

		if (!m_pWorldModelClone)
			return;
		goto LABEL_10;
	}
	if (!m_pWorldModelClone)
	{
		m_pWorldModelClone = new C_CombatWeaponClone( this );
		UpdateVisibility();
		
		if (!m_pWorldModelClone)
			return;
	}
LABEL_10:
	m_pWorldModelClone->UpdateClone();
	m_pWorldModelClone->UpdateVisibility();
}

//-----------------------------------------------------------------------------
// Is anyone carrying it?
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::IsBeingCarried() const
{
	return ( m_hOwner.Get() != NULL );
}

//-----------------------------------------------------------------------------
// Is the carrier alive?
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::IsCarrierAlive() const
{
	if ( !m_hOwner.Get() )
		return false;

	return m_hOwner.Get()->GetHealth() > 0;
}

//-----------------------------------------------------------------------------
// Should this object cast shadows?
//-----------------------------------------------------------------------------
ShadowType_t C_BaseCombatWeapon::ShadowCastType()
{
	if ( IsEffectActive( /*EF_NODRAW |*/ EF_NOSHADOW ) )
		return SHADOWS_NONE;

	if (!IsBeingCarried())
		return SHADOWS_RENDER_TO_TEXTURE;

	if (IsCarriedByLocalPlayer())
		return SHADOWS_NONE;

	return (m_iState != WEAPON_IS_CARRIED_BY_PLAYER) ? SHADOWS_RENDER_TO_TEXTURE : SHADOWS_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: This weapon is the active weapon, and it should now draw anything
//			it wants to. This gets called every frame.
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::Redraw()
{
	if ( GetClientMode()->ShouldDrawCrosshair() )
	{
		DrawCrosshair();
	}

	// ammo drawing has been moved into hud_ammo.cpp
}
//-----------------------------------------------------------------------------
// Purpose: Draw the weapon's crosshair
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::DrawCrosshair()
{
#ifndef INFESTED_DLL
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( !player )
		return;

	Color clr = GetHud().m_clrNormal;
/*

	// TEST: if the thing under your crosshair is on a different team, light the crosshair with a different color.
	Vector vShootPos, vShootAngles;
	GetShootPosition( vShootPos, vShootAngles );

	Vector vForward;
	AngleVectors( vShootAngles, &vForward );
	
	
	// Change the color depending on if we're looking at a friend or an enemy.
	CPartitionFilterListMask filter( PARTITION_ALL_CLIENT_EDICTS );	
	trace_t tr;
	traceline->TraceLine( vShootPos, vShootPos + vForward * 10000, COLLISION_GROUP_NONE, MASK_SHOT, &tr, true, ~0, &filter );

	if ( tr.index != 0 && tr.index != INVALID_CLIENTENTITY_HANDLE )
	{
		C_BaseEntity *pEnt = ClientEntityList().GetBaseEntityFromHandle( tr.index );
		if ( pEnt )
		{
			if ( pEnt->GetTeamNumber() != player->GetTeamNumber() )
			{
				g = b = 0;
			}
		}
	}		 
*/

	CHudCrosshair *crosshair = GET_HUDELEMENT( CHudCrosshair );
	if ( !crosshair )
		return;

	// Find out if this weapon's auto-aimed onto a target
	bool bOnTarget = ( m_iState == WEAPON_IS_ACTIVE ) && player->m_fOnTarget;
	
	if ( player->GetFOV() >= 90 )
	{ 
		// normal crosshairs
		if ( bOnTarget && GetWpnData().iconAutoaim )
		{
			clr[3] = 255;

			crosshair->SetCrosshair( GetWpnData().iconAutoaim, clr );
		}
		else if ( GetWpnData().iconCrosshair )
		{
			clr[3] = 255;
			crosshair->SetCrosshair( GetWpnData().iconCrosshair, clr );
		}
		else
		{
			crosshair->ResetCrosshair();
		}
	}
	else
	{ 
		Color white( 255, 255, 255, 255 );

		// zoomed crosshairs
		if (bOnTarget && GetWpnData().iconZoomedAutoaim)
			crosshair->SetCrosshair(GetWpnData().iconZoomedAutoaim, white);
		else if ( GetWpnData().iconZoomedCrosshair )
			crosshair->SetCrosshair( GetWpnData().iconZoomedCrosshair, white );
		else
			crosshair->ResetCrosshair();
	}
	#endif
}
//-----------------------------------------------------------------------------
// Purpose: This weapon is the active weapon, and the viewmodel for it was just drawn.
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::ViewModelDrawn( int nFlags, C_BaseViewModel *pViewModel )
{
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if this client's carrying this weapon
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::IsCarriedByLocalPlayer( void )
{
	if ( !GetOwner() )
		return false;

	return ( C_BasePlayer::IsLocalPlayer( GetOwner() ) );
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if this weapon is the local client's currently wielded weapon
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::IsActiveByLocalPlayer( void )
{
	if ( IsCarriedByLocalPlayer() )
	{
		return (m_iState == WEAPON_IS_ACTIVE);
	}

	return false;
}

bool C_BaseCombatWeapon::GetShootPosition( Vector &vOrigin, QAngle &vAngles )
{
	// Get the entity because the weapon doesn't have the right angles.
	C_BaseCombatCharacter *pEnt = ToBaseCombatCharacter( GetOwner() );
	if ( pEnt )
	{
		if ( pEnt == C_BasePlayer::GetLocalPlayer() )
		{
			vAngles = pEnt->EyeAngles();
		}
		else
		{
			vAngles = pEnt->GetRenderAngles();	
		}
	}
	else
	{
		vAngles.Init();
	}

	C_BasePlayer *player = ToBasePlayer( pEnt );
	bool bUseViewModel = false;
	if ( C_BasePlayer::IsLocalPlayer( pEnt ) )
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD_ENT( pEnt );
		bUseViewModel = !player->ShouldDrawLocalPlayer();
	}

	QAngle vDummy;
	if ( IsActiveByLocalPlayer() && bUseViewModel )
	{
		C_BaseViewModel *vm = player ? player->GetViewModel( 0 ) : NULL;
		if ( vm )
		{
			int iAttachment = vm->LookupAttachment( "muzzle" );
			if ( vm->GetAttachment( iAttachment, vOrigin, vDummy ) )
			{
				return true;
			}
		}
	}
	else
	{
		// Thirdperson
		int iAttachment = LookupAttachment( "muzzle" );
		if ( GetAttachment( iAttachment, vOrigin, vDummy ) )
		{
			return true;
		}
	}

	vOrigin = GetRenderOrigin();
	return false;
}

bool C_BaseCombatWeapon::ShouldSuppressForSplitScreenPlayer( int nSlot )
{
	if ( BaseClass::ShouldSuppressForSplitScreenPlayer( nSlot ) )
	{
		return true;
	}
	
	C_BaseCombatCharacter *pOwner = GetOwner();
	
	// If the owner of this weapon is not allowed to draw in this split screen slot, then don't draw the weapon either.
	if ( pOwner && pOwner->ShouldSuppressForSplitScreenPlayer( nSlot ) )
	{
		return true;
	}
	
	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer( nSlot );

	// Carried by local player?
	// Only draw the weapon if we're in some kind of 3rd person mode because the viewmodel will do that otherwise.
	if ( pLocalPlayer && pOwner == pLocalPlayer && !pLocalPlayer->ShouldDrawLocalPlayer() )
	{
		return true;
	}
	
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::ShouldDraw( void )
{
	if ( m_iWorldModelIndex == 0 )
		return false;

	// FIXME: All weapons with owners are set to transmit in CBaseCombatWeapon::UpdateTransmitState,
	// even if they have EF_NODRAW set, so we have to check this here. Ideally they would never
	// transmit except for the weapons owned by the local player.
	if ( IsEffectActive( EF_NODRAW ) )
		return false;

	C_BaseCombatCharacter *pOwner = GetOwner();

	// weapon has no owner, always draw it
	if ( !pOwner )
		return true;

	bool bIsActive = ( m_iState == WEAPON_IS_ACTIVE );
	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();

	 // carried by local player?
	if ( pOwner == pLocalPlayer )
	{
		// Only ever show the active weapon
		if ( !bIsActive )
			return false;

		// 3rd person mode
		if ( pLocalPlayer->ShouldDrawLocalPlayer() )
			return true;

		// don't draw active weapon if not in some kind of 3rd person mode, the viewmodel will do that
		return false;
	}

	// If it's a player, then only show active weapons
	if ( pOwner->IsPlayer() )
	{
		// Show it if it's active...
		return bIsActive;
	}

	// FIXME: We may want to only show active weapons on NPCs
	// These are carried by AIs; always show them
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Return true if a weapon-pickup icon should be displayed when this weapon is received
//-----------------------------------------------------------------------------
bool C_BaseCombatWeapon::ShouldDrawPickup( void )
{
	if ( GetWeaponFlags() & ITEM_FLAG_NOITEMPICKUP )
		return false;

	if ( m_bJustRestored )
		return false;

	return true;
}

//----------------------------------------------------------------------------
// Hooks into the fast path render system
//----------------------------------------------------------------------------
IClientModelRenderable*	C_BaseCombatWeapon::GetClientModelRenderable()
{
	if ( !m_bReadyToDraw )
		return 0;

	if ( IsFirstPersonSpectated() )
		return NULL;

	if ( !BaseClass::GetClientModelRenderable() )
		return NULL;

	EnsureCorrectRenderingModel();
	return this;
}

void C_BaseCombatWeapon::UpdateOnRemove( void )
{
	if ( m_pWorldModelClone )
		m_pWorldModelClone->Release();

	m_pWorldModelClone = NULL;
	BaseClass::UpdateOnRemove();
}


//-----------------------------------------------------------------------------
// Purpose: Render the weapon. Draw the Viewmodel if the weapon's being carried
//			by this player, otherwise draw the worldmodel.
//-----------------------------------------------------------------------------
int C_BaseCombatWeapon::DrawModel( int flags, const RenderableInstance_t &instance )
{
	VPROF_BUDGET( "C_BaseCombatWeapon::DrawModel", VPROF_BUDGETGROUP_MODEL_RENDERING );
	if ( !m_bReadyToDraw )
		return 0;

	if ( !IsVisible() )
		return 0;
	
	if( IsFirstPersonSpectated() )
		return 0;

	// See comment below
	EnsureCorrectRenderingModel();

	return BaseClass::DrawModel( flags, instance );
}

void C_BaseCombatWeapon::NotifyWorldModelCloneReleased( void )
{
	m_pWorldModelClone = NULL;
}

bool C_BaseCombatWeapon::ShouldDrawThisOrWorldModelClone( void )
{	
	if (!m_iWorldModelIndex|| (GetEffects() & 0x20) != 0)
		return false;

	C_BaseCombatCharacter *pOwner = GetOwner();
	if ( !pOwner )
		return true;

	if ( !pOwner->IsPlayer() )
		return true;

	return m_iState == 2;
}

// If the local player is visible (thirdperson mode, tf2 taunts, etc., then make sure that we are using the 
//  w_ (world) model not the v_ (view) model or else the model can flicker, etc.
// Otherwise, if we're not the local player, always use the world model
void C_BaseCombatWeapon::EnsureCorrectRenderingModel()
{
	C_BasePlayer *localplayer = C_BasePlayer::GetLocalPlayer();
	if ( localplayer && 
		localplayer == GetOwner() &&
		!localplayer->ShouldDrawLocalPlayer() )
	{
		return;
	}

	// BRJ 10/14/02
	// FIXME: Remove when Yahn's client-side prediction is done
	// It's a hacky workaround for the model indices fighting
	// (GetRenderBounds uses the model index, which is for the view model)
	SetModelIndex( GetWorldModelIndex() );

	// Validate our current sequence just in case ( in theory the view and weapon models should have the same sequences for sequences that overlap at least )
	CStudioHdr *pStudioHdr = GetModelPtr();
	if ( pStudioHdr && 
		GetSequence() >= pStudioHdr->GetNumSeq() )
	{
		SetSequence( 0 );
	}
}

//-----------------------------------------------------------------------------
// tool recording
//-----------------------------------------------------------------------------
void C_BaseCombatWeapon::GetToolRecordingState( KeyValues *msg )
{
	if ( !ToolsEnabled() )
		return;

	int nModelIndex = GetModelIndex();
	int nWorldModelIndex = GetWorldModelIndex();
	if ( nModelIndex != nWorldModelIndex )
	{
		SetModelIndex( nWorldModelIndex );
	}

	BaseClass::GetToolRecordingState( msg );

	if ( m_iState == WEAPON_NOT_CARRIED )
	{
		BaseEntityRecordingState_t *pBaseEntity = (BaseEntityRecordingState_t*)msg->GetPtr( "baseentity" );
		pBaseEntity->m_nOwner = -1;
	}
	else
	{
		msg->SetInt( "worldmodel", 1 );
		if ( m_iState == WEAPON_IS_ACTIVE )
		{
			BaseEntityRecordingState_t *pBaseEntity = (BaseEntityRecordingState_t*)msg->GetPtr( "baseentity" );
			pBaseEntity->m_bVisible = true;
		}
	}

	if ( nModelIndex != nWorldModelIndex )
	{
		SetModelIndex( nModelIndex );
	}
}

bool C_BaseCombatWeapon::GetAttachment( int number, Vector &origin )
{
	if (!m_pWorldModelClone)
		return BaseClass::GetAttachment( number, origin );
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetAttachment( number, origin );
}

bool C_BaseCombatWeapon::GetAttachment( int number, Vector &origin, QAngle &angles )
{
	if (!m_pWorldModelClone)
		return BaseClass::GetAttachment( number, origin, angles );
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetAttachment( number, origin, angles );
}

bool C_BaseCombatWeapon::GetAttachment( int number, matrix3x4_t &matrix )
{
	if (!m_pWorldModelClone)
		return BaseClass::GetAttachment( number, matrix );
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetAttachment( number, matrix );
}

bool C_BaseCombatWeapon::GetAttachmentVelocity( int number, Vector &originVel, Quaternion &angleVel )
{
	if (!m_pWorldModelClone)
		return BaseClass::GetAttachmentVelocity( number, originVel, angleVel );
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetAttachmentVelocity( number, originVel, angleVel );
}

void C_BaseCombatWeapon::InvalidateAttachments( void )
{
	if (m_pWorldModelClone)
	{
		m_pWorldModelClone->UpdateClone();
		m_pWorldModelClone->InvalidateAttachments();
	}
	BaseClass::InvalidateAttachments();
}

const Vector &C_BaseCombatWeapon::GetRenderOrigin()
{
	if (!m_pWorldModelClone)
		return BaseClass::GetRenderOrigin();
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetRenderOrigin();
}

const QAngle &C_BaseCombatWeapon::GetRenderAngles()
{
	if (!m_pWorldModelClone)
		return BaseClass::GetRenderAngles();
	m_pWorldModelClone->UpdateClone();
	return m_pWorldModelClone->GetRenderAngles();
}

bool C_BaseCombatWeapon::ComputeStencilState( ShaderStencilState_t *pStencilState )
{
	return BaseClass::ComputeStencilState( pStencilState );
}


bool C_BaseCombatWeapon::IsFirstPersonSpectated( void )
{
	// check if local player chases owner of this weapon in first person
	C_BasePlayer *localplayer = C_BasePlayer::GetLocalPlayer();
	if ( localplayer && localplayer->IsObserver() && GetOwner() )
	{
		// don't draw weapon if chasing this guy as spectator
		// we don't check that in ShouldDraw() since this may change
		// without notification 
		if ( localplayer->GetObserverMode() == OBS_MODE_IN_EYE &&
			localplayer->GetObserverTarget() == GetOwner() )
			return true;
	}

	return false;
}

C_CombatWeaponClone::C_CombatWeaponClone( C_BaseCombatWeapon *pWeaponParent )
{
	InitializeAsClientEntity( pWeaponParent->GetWorldModel(), false );
}
C_CombatWeaponClone::~C_CombatWeaponClone()
{
	m_pWeaponParent->NotifyWorldModelCloneReleased();
}

bool C_CombatWeaponClone::ShouldDraw( void )
{
	C_BaseCombatCharacter *pOwner = m_pWeaponParent->GetOwner();
	return (!pOwner
		|| !pOwner->IsPlayer()
		|| !C_BasePlayer::IsLocalPlayer( pOwner )
		|| pOwner->ShouldDraw()
		&& m_pWeaponParent->ShouldDrawThisOrWorldModelClone()
		&& m_pWeaponParent->GetOwner()
		&& !m_pWeaponParent->ShouldDraw() // Best guess, probably inaccurate
		&& C_BaseEntity::ShouldDraw() );
}

bool C_CombatWeaponClone::ShouldSuppressForSplitScreenPlayer( int nSlot )
{
	C_BaseCombatCharacter *pOwner = m_pWeaponParent->GetOwner();
	return pOwner
		&& pOwner->IsPlayer()
		&& C_BasePlayer::IsLocalPlayer( pOwner )
		&& pOwner->ShouldSuppressForSplitScreenPlayer( nSlot );
}

int C_CombatWeaponClone::DrawModel( int flags, const RenderableInstance_t &instance )
{
	UpdateClone();
	return BaseClass::DrawModel( flags, instance );
}

C_BaseCombatWeapon *C_CombatWeaponClone::MyCombatWeaponPointer( void )
{
	return m_pWeaponParent;
}

bool C_CombatWeaponClone::SetupBones( matrix3x4a_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
{
	UpdateClone();
	int nBoneIndex = m_nWeaponBoneIndex;
	m_nWeaponBoneIndex = -1;
	bool result = BaseClass::SetupBones( pBoneToWorldOut, nMaxBones, boneMask, currentTime );
	m_nWeaponBoneIndex = nBoneIndex;
	return result;
}

IClientModelRenderable *C_CombatWeaponClone::GetClientModelRenderable( void )
{
	if (!m_bReadyToDraw
		|| m_pWeaponParent->IsFirstPersonSpectated()
		|| !m_pWeaponParent->GetClientModelRenderable() )
	{
		return NULL;
	}
	UpdateClone();
	return BaseClass::GetClientModelRenderable();
}

const Vector &C_CombatWeaponClone::GetRenderOrigin( void )
{
	if ( m_nWeaponBoneIndex < 0 )
		return BaseClass::GetRenderOrigin();
	else
		return m_vWeaponBonePosition;
}

const QAngle &C_CombatWeaponClone::GetRenderAngles( void )
{
	if ( m_nWeaponBoneIndex < 0 )
		return BaseClass::GetRenderAngles();
	else
		return m_qWeaponBoneAngle;
}

bool C_CombatWeaponClone::ComputeStencilState( ShaderStencilState_t *pStencilState )
{
	return m_pWeaponParent->ComputeStencilState( pStencilState );
}

void C_CombatWeaponClone::UpdateClone( void )
{
	int iTickCount = gpGlobals->tickcount;
	if (m_nLastUpdatedWorldModelClone != iTickCount)
	{
		m_nLastUpdatedWorldModelClone = iTickCount;
		m_pWeaponParent = m_pWeaponParent;

		SetParent( GetMoveParent(), m_pWeaponParent->GetParentAttachment() );
		SetLocalOrigin( m_pWeaponParent->GetLocalOrigin() );
		SetLocalAngles( m_pWeaponParent->GetLocalAngles() );
		SetCycle( m_pWeaponParent->GetCycle() );
		SetEffects( m_pWeaponParent->GetEffects() );
		m_flAnimTime = m_pWeaponParent->m_flAnimTime;
		
		SetBody( m_pWeaponParent->GetBody() );
		SetSkin( m_pWeaponParent->GetSkin() );
		float flFrozen = 1.0 - m_pWeaponParent->GetFrozenAmount();
		if (flFrozen >= 0.0)
		{
			if (flFrozen > 1.0)
				flFrozen = 1.0;
		}
		else
		{
			flFrozen = 0.0;
		}
		SetPlaybackRate( m_pWeaponParent->GetPlaybackRate() * flFrozen );
		if ( GetModelIndex() != m_pWeaponParent->GetWorldModelIndex() )
		{
			SetModelIndex( m_pWeaponParent->GetWorldModelIndex() );
			m_nWeaponBoneIndex = LookupBone( "weapon_bone" );
		}
#if 0
		if ( GetModelPtr()
			&& GetModelPtr()->GetRenderHdr()
			&& ((m_nSequence = this->m_pWeaponParent->m_nSequence, !m_pStudioHdr->m_pVModel)
			? (numlocalseq = m_pStudioHdr->m_pStudioHdr->numlocalseq)
			: (numlocalseq = CStudioHdr::GetNumSeq_Internal(this->m_pStudioHdr)),
			m_nSequence >= numlocalseq) )
#else
		if ( false )
#endif
		{
			SetSequence( 0 );
		}
		else
		{
			SetSequence( m_pWeaponParent->GetSequence() );
		}

		AlphaProp()->SetAlphaModulation( m_pWeaponParent->AlphaProp()->GetAlphaModulation() );
		AlphaProp()->SetFade( 
			m_pWeaponParent->AlphaProp()->GetGlobalFadeScale(),
			m_pWeaponParent->AlphaProp()->GetMinFadeDist(),
			m_pWeaponParent->AlphaProp()->GetMaxFadeDist() );
		if ( m_nWeaponBoneIndex >= 0 )
			GetBonePosition( m_nWeaponBoneIndex, m_vWeaponBonePosition, m_qWeaponBoneAngle );
	}
}