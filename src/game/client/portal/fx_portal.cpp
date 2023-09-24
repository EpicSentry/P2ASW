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


class C_PortalBlast : public C_BaseEntity
{
	DECLARE_CLASS( C_PortalBlast, C_BaseAnimating );

public:

	static void		Create( bool bIsPortal2, PortalPlacedByType ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles );

	void			Init( bool bIsPortal2, PortalPlacedByType ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles );

	virtual void	ClientThink( void );

private:

	Vector	m_ptCreationPoint;
	Vector	m_ptDeathPoint;
	Vector	m_ptAimPoint;

	float	m_fCreationTime;
	bool	m_bRemove;
};


void C_PortalBlast::Create( bool bIsPortal2, PortalPlacedByType ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles )
{
	C_PortalBlast *pPortalBlast = new C_PortalBlast;
	pPortalBlast->Init( bIsPortal2, ePlacedBy, vStart, vEnd, qAngles );
}


void C_PortalBlast::Init( bool bIsPortal2, PortalPlacedByType ePlacedBy, const Vector &vStart, const Vector &vEnd, const QAngle &qAngles )
{
	ClientEntityList().AddNonNetworkableEntity( this );
	ClientThinkList()->SetNextClientThink( GetClientHandle(), CLIENT_THINK_ALWAYS );

	AddToLeafSystem( false );

	SetThink( &C_PortalBlast::ClientThink );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	m_ptCreationPoint = vStart;
	m_ptDeathPoint = vEnd;

	SetAbsOrigin( m_ptCreationPoint );

	m_fCreationTime = gpGlobals->curtime;

	Vector vForward;
	AngleVectors( qAngles, &vForward );

	m_ptAimPoint = m_ptCreationPoint + vForward * m_ptCreationPoint.DistTo( m_ptDeathPoint );
#if 0
	if ( ePlacedBy == PORTAL_PLACED_BY_PLAYER )
		ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_2_projectile_stream" ) : ( "portal_1_projectile_stream" ) ), PATTACH_ABSORIGIN_FOLLOW );
	else
		ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_2_projectile_stream_pedestal" ) : ( "portal_1_projectile_stream_pedestal" ) ), PATTACH_ABSORIGIN_FOLLOW );
#else
		ParticleProp()->Create( ( ( bIsPortal2 ) ? ( "portal_projectile_stream" ) : ( "portal_projectile_stream" ) ), PATTACH_ABSORIGIN_FOLLOW );
#endif
}

void C_PortalBlast::ClientThink( void )
{
	float fT = m_ptAimPoint.x;
	
	//if ( m_fCreationTime == 0.0f && m_fDeathTime == 0.0f )
	if ( fT == 0.0 && m_ptAimPoint.y == 0.0 )
	{
		// Die!
		Remove();
		return;
	}

	//fT = ( gpGlobals->curtime - m_fCreationTime ) / ( m_fDeathTime - m_fCreationTime );
	fT = ( gpGlobals->curtime - fT ) / ( m_ptAimPoint.y - fT );
	
	if ( 0.0 <= fT )
	{
		if ( fT >= 1.0f )
		{
			// Ready to die! But we want one more frame in the final position
			SetAbsOrigin( m_ptCreationPoint );

			m_ptAimPoint.x = 0.0;
			m_ptAimPoint.y = 0.0;
			return;
		}
	}
	else
	{
		fT = 0.0;
	}

	float flTargetHelper = 1.0f - fT;

	// Set the interpolated position
	Vector vTarget; //= m_ptAimPoint * flTargetHelper + m_ptDeathPoint * fT;
	vTarget.x = m_ptAimPoint.x * flTargetHelper + m_ptDeathPoint.x + flTargetHelper + fT * m_ptCreationPoint.x * fT;
	vTarget.y = m_ptAimPoint.y * flTargetHelper + m_ptCreationPoint.y * fT + (m_ptDeathPoint.y * flTargetHelper) * fT;
	vTarget.z = m_ptAimPoint.z * flTargetHelper + m_ptDeathPoint.z * flTargetHelper + (m_ptCreationPoint.z * fT) * fT;
	
	SetAbsOrigin( m_ptCreationPoint * flTargetHelper + vTarget * fT );
	
}


void PortalBlastCallback( const CEffectData & data )
{
	C_PortalBlast::Create( ( data.m_nColor == 1 ) ? ( false ) : ( true ), (PortalPlacedByType)data.m_nDamageType, data.m_vOrigin, data.m_vStart, data.m_vAngles );
}

DECLARE_CLIENT_EFFECT( PortalBlast, PortalBlastCallback );