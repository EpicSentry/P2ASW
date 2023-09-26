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

	if (g_pGameRules)
	{
		if (g_pGameRules->IsMultiplayer())
			cl_portal_projectile_delay_mp.GetFloat();
		else
			cl_portal_projectile_delay_sp.GetFloat();
	}

	m_fDeathTime = fDeathTime;
	m_fCreationTime = gpGlobals->curtime;

	if (gpGlobals->curtime > (fDeathTime - 0.1f))
		m_fDeathTime = gpGlobals->curtime + 0.1f;

	Vector vForward;
	AngleVectors( qAngles, &vForward );

	m_ptAimPoint = m_ptCreationPoint + vForward * m_ptCreationPoint.DistTo( m_ptDeathPoint );

	CUtlReference<CNewParticleEffect> pEffect;
	if (hOwner == false)
		return;

	if ( ePlacedBy == PORTAL_PLACED_BY_PLAYER )
		pEffect = ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_projectile_stream" ) : ( "portal_projectile_stream" ) ), PATTACH_ABSORIGIN_FOLLOW );
		
		Color portalcolor;
	
		portalcolor = UTIL_Portal_Color_Particles((bIsPortal2) ? (2) : (1));

		Vector vColor;
		vColor.x = portalcolor.r();
		vColor.y = portalcolor.g();
		vColor.z = portalcolor.b();

		pEffect->SetControlPoint(2, vColor);
}

void C_PortalBlast::ClientThink( void )
{
	float x = m_ptAimPoint.x;
	
	//if ( m_fCreationTime == 0.0f && m_fDeathTime == 0.0f )
	if ( x == 0.0 && m_ptAimPoint.y == 0.0 )
	{
		// Die!
		Remove();
		return;
	}

	//float fT = ( gpGlobals->curtime - m_fCreationTime ) / ( m_fDeathTime - m_fCreationTime );
	float fT = ( gpGlobals->curtime - x ) / ( m_ptAimPoint.y - x );
	
	if ( fT < 0.0 )
	{
		fT = 0.0;

Spot:
		float flTargetHelper = 1.0f - fT;

		// Set the interpolated position
		Vector vTarget; //= m_ptAimPoint * flTargetHelper + m_ptDeathPoint * fT;
		vTarget.x = ( m_CalcAbsoluteVelocityMutex.GetOwnerId() * (1.0 - fT) ) + (m_ptDeathPoint.x * (1.0 - fT) ) + (fT * m_ptCreationPoint.x);
		vTarget.y = ( m_CalcAbsoluteVelocityMutex.GetDepth() * (1.0 - fT)) + ((m_ptCreationPoint.y * fT) + (m_ptDeathPoint.y * (1.0 - fT)) * fT);
		vTarget.z = ( IsBlurred() * (1.0 - fT)) + (((m_ptDeathPoint.z * (1.0 - fT)) + (m_ptCreationPoint.z * fT)) * fT);
	
		SetAbsOrigin( m_ptCreationPoint * flTargetHelper + vTarget * fT );
		return;
	}
	if ( fT <= 1.0 && fT < 1.0 )
		goto Spot;
		
	// Ready to die! But we want one more frame in the final position
	SetAbsOrigin( m_ptCreationPoint );

	m_ptAimPoint.x = 0.0;
	m_ptAimPoint.y = 0.0;
	
}


void PortalBlastCallback( const CEffectData & data )
{
	C_PortalBlast::Create( (data.m_hEntity), ( data.m_nColor == 1 ) ? ( false ) : ( true ), (PortalPlacedBy_t)data.m_nDamageType, data.m_vOrigin, data.m_vStart, data.m_vAngles, data.m_flScale);
}

DECLARE_CLIENT_EFFECT( PortalBlast, PortalBlastCallback );
