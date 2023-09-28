//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#include "cbase.h"
#include "model_types.h"
#include "vcollide.h"
#include "vcollide_parse.h"
#include "solidsetdefaults.h"
#include "bone_setup.h"
#include "engine/ivmodelinfo.h"
#include "physics.h"
#include "c_breakableprop.h"
#include "view.h"
#include "portal_grabcontroller_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_BreakableProp, DT_BreakableProp, CBreakableProp)
	RecvPropQAngles( RECVINFO( m_qPreferredPlayerCarryAngles ) ),
	RecvPropBool( RECVINFO( m_bClientPhysics ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BreakableProp::C_BreakableProp( void )
{
	m_takedamage = DAMAGE_YES;
}


//-----------------------------------------------------------------------------
// Copy fade from another breakable prop
//-----------------------------------------------------------------------------
void C_BreakableProp::CopyFadeFrom( C_BreakableProp *pSource )
{
	SetGlobalFadeScale( pSource->GetGlobalFadeScale() );
	SetDistanceFade( pSource->GetMinFadeDist(), pSource->GetMaxFadeDist() );
}

void C_BreakableProp::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );
	if ( m_bClientPhysics )
	{
		bool bCreate = (type == DATA_UPDATE_CREATED) ? true : false;
		VPhysicsShadowDataChanged(bCreate, this);
	}
}

//IPlayerPickupVPhysics
bool C_BreakableProp::HasPreferredCarryAnglesForPlayer( CBasePlayer *pPlayer )
{
	return (m_qPreferredPlayerCarryAngles.x < FLT_MAX);
}

QAngle C_BreakableProp::PreferredCarryAngles( void )
{
	return (m_qPreferredPlayerCarryAngles.x < FLT_MAX) ? m_qPreferredPlayerCarryAngles : vec3_angle;
}


bool C_BreakableProp::ShouldPredict( void )
{
#ifdef PORTAL
	C_BasePlayer *pPredOwner = GetPlayerHoldingEntity( this );
	return (pPredOwner && pPredOwner->IsLocalPlayer()) ? true : BaseClass::ShouldPredict();
#else
	return false;
#endif
}

C_BasePlayer *C_BreakableProp::GetPredictionOwner( void )
{
#ifdef PORTAL
	return GetPlayerHoldingEntity( this );
#else
	return NULL;
#endif
}