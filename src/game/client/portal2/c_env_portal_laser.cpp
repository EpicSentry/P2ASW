#include "cbase.h"
#include "portalsimulation.h"
#include "c_portal_beam_helper.h"
#include "portal_util_shared.h"
#include "util_shared.h"
#include "c_prop_weightedcube.h"

#define	MASK_PORTAL_LASER (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_DEBRIS|CONTENTS_HITBOX)
const char *LASER_SPARK_EFFECT_NAME = "discouragement_beam_sparks";

class C_PortalLaser : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_PortalLaser, C_BaseAnimating );	
	DECLARE_CLIENTCLASS();
    
    C_PortalLaser();
    void Precache();
    void Spawn();
    void UpdateOnRemove();
    void OnDataChanged( DataUpdateType_t updateType );
    void ClientThink();
    ~C_PortalLaser();
	
private:
	
    void CreateSparkEffect( void );
    void UpdateSparkEffect( const Vector &vSparkPosition, const Vector &vSparkNormal );
    C_PortalBeamHelper m_beamHelper;
    
	CHandle<C_PropWeightedCube> m_hReflector;

    CUtlReference<CNewParticleEffect> m_pSparkEffect;
    Vector m_vStartPoint;
    Vector m_vEndPoint;
    bool m_bLaserOn;
    bool m_bIsLethal;
    bool m_bIsAutoAiming;
    bool m_bShouldSpark;
    bool m_bUseParentDir;
    QAngle m_angParentAngles;
	
};

IMPLEMENT_CLIENTCLASS_DT( C_PortalLaser, DT_PortalLaser, CPortalLaser )

RecvPropBool( RECVINFO( m_bLaserOn ) ),
RecvPropBool( RECVINFO( m_bShouldSpark ) ),
RecvPropBool( RECVINFO( m_bIsAutoAiming ) ),
RecvPropBool( RECVINFO( m_bIsLethal ) ),
RecvPropBool( RECVINFO( m_bUseParentDir ) ),
RecvPropEHandle( RECVINFO( m_hReflector ) ),
RecvPropVector( RECVINFO( m_vStartPoint ) ),
RecvPropVector( RECVINFO( m_vEndPoint ) ),
RecvPropQAngles( RECVINFO( m_angParentAngles ) ),

END_RECV_TABLE();

LINK_ENTITY_TO_CLASS( env_portal_laser, C_PortalLaser );

C_PortalLaser::C_PortalLaser( void )
{
	m_bLaserOn = false;

	m_pSparkEffect = NULL;

	m_vStartPoint = vec3_origin;
	m_vEndPoint = vec3_origin;

	m_bShouldSpark = false;
}

C_PortalLaser::~C_PortalLaser( void )
{
	m_pSparkEffect = NULL;
}
void C_PortalLaser::Precache( void )
{
	 PrecacheParticleSystem( LASER_SPARK_EFFECT_NAME );
	 BaseClass::Precache();
}

void C_PortalLaser::Spawn( void )
{
	C_Beam *pNewBeam; // eax

	Precache();
	if ( m_bIsLethal )
	{
		pNewBeam = C_Beam::BeamCreate("sprites/laserbeam.vmt", 2.0);
		pNewBeam->SetRenderColor( 100, 255, 100 );
	}
	else
	{
		pNewBeam = C_Beam::BeamCreate("sprites/purplelaser1.vmt", 32.0);
	}
	
	m_beamHelper.Init( pNewBeam );
	BaseClass::Spawn();
}

void C_PortalLaser::UpdateOnRemove( void )
{
	if (m_pSparkEffect)
	{
		ParticleProp()->StopEmission( m_pSparkEffect );
		m_pSparkEffect = NULL;
	}

	BaseClass::UpdateOnRemove();
}

void C_PortalLaser::OnDataChanged( DataUpdateType_t updateType )
{
	if ( m_bShouldSpark )
	{
		if ( !m_pSparkEffect )
			CreateSparkEffect();
	}
	else if ( m_pSparkEffect )
	{
		ParticleProp()->StopEmission( m_pSparkEffect.m_pObject );
		m_pSparkEffect = NULL;
	}

	if (m_bLaserOn)
	{
		SetNextClientThink(CLIENT_THINK_ALWAYS);
		m_beamHelper.TurnOn();
	}
	else
	{
		SetNextClientThink(CLIENT_THINK_NEVER);
		m_beamHelper.TurnOff();
	}

	BaseClass::OnDataChanged( updateType );
}

void C_PortalLaser::CreateSparkEffect( void )
{
	m_pSparkEffect = ParticleProp()->Create( LASER_SPARK_EFFECT_NAME, PATTACH_CUSTOMORIGIN );
}

void C_PortalLaser::UpdateSparkEffect( const Vector &vSparkPosition, const Vector &vSparkNormal )
{
	Vector vecRight; // [esp+38h] [ebp-30h] BYREF
	Vector vecUp; // [esp+44h] [ebp-24h] BYREF

	if ( m_pSparkEffect )
	{
		m_pSparkEffect->SetControlPoint( 0, vSparkPosition);		
		VectorVectors( vSparkNormal, vecRight, vecUp );				
		m_pSparkEffect->SetControlPointOrientation( 0, vSparkNormal, vecRight, vecUp );
	}
}

void C_PortalLaser::ClientThink( void )
{
	trace_t tr;
	Vector vDir; 
	Vector vStart;
	
	CTraceFilterSimpleClassnameList traceFilter( m_hReflector.Get(), COLLISION_GROUP_NONE );
	traceFilter.AddClassnameToIgnore( "projected_wall_entity" );
	traceFilter.AddClassnameToIgnore( "player" );
	traceFilter.AddClassnameToIgnore( "point_laser_target" );

	C_BaseEntity *pReflector = m_hReflector.Get();

	if ( pReflector )
	{
		if (m_bUseParentDir)
		{
			AngleVectors( m_angParentAngles, &vDir);
		}
		else
		{
			AngleVectors( pReflector->GetAbsAngles(), &vDir );
		}
		
		vStart = (vDir * 22.0) + pReflector->WorldSpaceCenter();

		CPortalSimulator *pSimulator = CPortalSimulator::GetSimulatorThatOwnsEntity( pReflector );
		if (pSimulator)
		{
			if (pSimulator->EntityIsInPortalHole( pReflector )
				&& (DotProduct( vStart, pSimulator->GetInternalData().Placement.PortalPlane.m_Normal ) -
				pSimulator->GetInternalData().Placement.PortalPlane.m_Dist) < 0.0)
			{
				UTIL_Portal_PointTransform( pSimulator->GetInternalData().Placement.matThisToLinked, vStart, vStart );
				vDir = pSimulator->GetInternalData().Placement.matThisToLinked * vDir;
			}
		}

		if (m_bIsAutoAiming)
		{
			vDir = (m_vEndPoint - vStart);
			
			VectorNormalize( vDir );
		}
		else
		{
			UTIL_Portal_Laser_Prevent_Tilting(vDir);
		}

		m_beamHelper.UpdatePointDirection( vStart, vDir, MASK_PORTAL_LASER, &traceFilter, &tr );
	}
	else
	{
		m_beamHelper.UpdatePoints( m_vStartPoint, m_vEndPoint, MASK_PORTAL_LASER, &traceFilter, &tr );
	}

	if (m_bShouldSpark)
		UpdateSparkEffect( tr.endpos, tr.plane.normal );
}
