//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Fizzle effects for portal.
//
//=============================================================================//

#include "cbase.h"
#include "precache_register.h"
#include "fx.h"
#include "FX_Sparks.h"
#include "iefx.h"
#include "c_te_effect_dispatch.h"
#include "particles_ez.h"
#include "decals.h"
#include "engine/IEngineSound.h"
#include "fx_quad.h"
#include "engine/IVDebugOverlay.h"
#include "shareddefs.h"
#include "portal_shareddefs.h"
#include "effect_color_tables.h"
#include "c_weapon_portalgun.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_portal_projectile_delay_mp("cl_portal_projectile_delay_mp", "0.14f", FCVAR_CHEAT);
ConVar cl_portal_projectile_delay_sp("cl_portal_projectile_delay_sp", "0.1f", FCVAR_CHEAT);

class C_PortalBlast : public C_BaseEntity
{
	DECLARE_CLASS( C_PortalBlast, C_BaseAnimating );

public:

	static void		Create(CBaseHandle hOwner, bool bIsPortal2, PortalPlacedBy_t ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles, float fDeathTime);

	void			Init(CBaseHandle hOwner, bool bIsPortal2, PortalPlacedBy_t ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles, float fDeathTime);

	virtual void	ClientThink( void );

private:

	Vector	m_ptCreationPoint;
	Vector	m_ptDeathPoint;
	Vector	m_ptAimPoint;

	float	m_fCreationTime;
	float	m_fDeathTime;
	bool	m_bRemove;
};


void C_PortalBlast::Create(CBaseHandle hOwner, bool bIsPortal2, PortalPlacedBy_t ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles, float fDeathTime)
{
	C_PortalBlast *pPortalBlast = new C_PortalBlast;
	pPortalBlast->Init(hOwner, bIsPortal2, ePlacedBy, vStart, vEnd, qAngles, fDeathTime);
}


void C_PortalBlast::Init(CBaseHandle hOwner, bool bIsPortal2, PortalPlacedBy_t ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles, float fDeathTime)
{
	ClientEntityList().AddNonNetworkableEntity( this );
	ClientThinkList()->SetNextClientThink( GetClientHandle(), CLIENT_THINK_ALWAYS );

	AddToLeafSystem( false );
	
	SetThink( &C_PortalBlast::ClientThink );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	m_ptCreationPoint = vStart;
	m_ptDeathPoint = vEnd;

	SetAbsOrigin( m_ptCreationPoint );

	float flProjectileDelay = cl_portal_projectile_delay_sp.GetBool();

	if ( g_pGameRules->IsMultiplayer() )
		flProjectileDelay = cl_portal_projectile_delay_mp.GetFloat();

	m_fCreationTime = flProjectileDelay + gpGlobals->curtime;
	m_fDeathTime = fDeathTime;

	if ( m_fDeathTime - 0.1f < m_fCreationTime )
	{
		m_fDeathTime = m_fCreationTime + 0.1f;
	}

	Vector vForward, vecRight, vecUp;
	AngleVectors(qAngles, &vForward, &vecRight, &vecUp);

	m_ptAimPoint = m_ptCreationPoint + vForward * m_ptCreationPoint.DistTo( m_ptDeathPoint );

	CNewParticleEffect *pParticle = NULL;

	if ( ePlacedBy == PORTAL_PLACED_BY_PLAYER )
	{
		C_BasePlayer *pPlayer = assert_cast<C_BasePlayer*>( ClientEntityList().GetBaseEntityFromHandle( hOwner ) );

		if ( pPlayer )
		{
			pParticle = ParticleProp()->Create( "portal_projectile_stream", PATTACH_ABSORIGIN_FOLLOW );
			//pParticle = ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_2_projectile_stream" ) : ( "portal_1_projectile_stream" ) ), PATTACH_ABSORIGIN_FOLLOW );
			
			int nTeam = pPlayer->GetTeamNumber();
			Color color = UTIL_Portal_Color_Particles( bIsPortal2 + 1, nTeam );

			Vector vColor;
			vColor.x = color.r();
			vColor.y = color.g();
			vColor.z = color.b();

			pParticle->SetControlPoint( 2, vColor );

		}
	}
	else // ePlacedBy == PORTAL_PLACED_BY_PEDESTAL
	{
		pParticle = ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_2_projectile_stream_pedestal" ) : ( "portal_1_projectile_stream_pedestal" ) ), PATTACH_ABSORIGIN_FOLLOW );
	}

	Assert( pParticle );

	pParticle->SetControlPointOrientation( 1, -vForward, vecRight, vecUp );
}

void C_PortalBlast::ClientThink( void )
{
	if ( m_fCreationTime == 0.0f && m_fDeathTime == 0.0f )
	{
		// Die!
		Remove();
		return;
	}

	float fT = ( gpGlobals->curtime - m_fCreationTime ) / ( m_fDeathTime - m_fCreationTime );

	if ( fT >= 1.0f )
	{
		// Ready to die! But we want one more frame in the final position
		SetAbsOrigin( m_ptDeathPoint );

		m_fCreationTime = 0.0f;
		m_fDeathTime = 0.0f;

		return;
	}

	// Set the interpolated position
	Vector vTarget = m_ptAimPoint * ( 1.0f - fT ) + m_ptDeathPoint * fT;
	SetAbsOrigin( m_ptCreationPoint * ( 1.0f - fT ) + vTarget * fT );
	
}


void PortalBlastCallback( const CEffectData & data )
{
	C_PortalBlast::Create( (data.m_hEntity), ( data.m_nColor == 1 ) ? ( false ) : ( true ), (PortalPlacedBy_t)data.m_nDamageType, data.m_vOrigin, data.m_vStart, data.m_vAngles, data.m_flScale);
}

DECLARE_CLIENT_EFFECT( PortalBlast, PortalBlastCallback );
