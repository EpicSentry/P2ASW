#include "cbase.h"
#include "c_baseprojector.h"

const Vector g_vTractorBeamColorForward = Vector(32.0, 70.0, 255.0);
const Vector g_vTractorBeamColorReverse = Vector(255.0, 60.0, 10.0);

class ClientProjectedEntityAmbientSoundProxy : public C_BaseEntity
{
	DECLARE_CLASS( ClientProjectedEntityAmbientSoundProxy, C_BaseEntity );
	DECLARE_CLIENTCLASS();
public:

	ClientProjectedEntityAmbientSoundProxy();
	~ClientProjectedEntityAmbientSoundProxy();

	virtual bool	GetSoundSpatialization( SpatializationInfo_t& info );
};

ClientProjectedEntityAmbientSoundProxy::ClientProjectedEntityAmbientSoundProxy()
{

}

ClientProjectedEntityAmbientSoundProxy::~ClientProjectedEntityAmbientSoundProxy()
{

}

//NOTE: This is inaccurate!
bool ClientProjectedEntityAmbientSoundProxy::GetSoundSpatialization( SpatializationInfo_t& info )
{
	Assert( GetMoveParent() );
	return GetMoveParent()->GetSoundSpatialization( info );
}

IMPLEMENT_CLIENTCLASS_DT( ClientProjectedEntityAmbientSoundProxy, DT_ProjectedEntityAmbientSoundProxy, ProjectedEntityAmbientSoundProxy )
END_RECV_TABLE()

#define NUM_EMITTER_PARTICLES 3

class C_PropTractorBeamProjector : public C_BaseProjector
{
public:
	
	DECLARE_CLASS( C_PropTractorBeamProjector, C_BaseProjector );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	C_PropTractorBeamProjector::C_PropTractorBeamProjector();
	C_PropTractorBeamProjector::~C_PropTractorBeamProjector();
	
	virtual void Spawn();
	virtual void ClientThink();
	virtual void UpdateOnRemove();
	virtual void OnToolStartRecording();
	
	static void RecvProxy_LinearForce( const CRecvProxyData *pData, void *pStruct, void *pOut );
	static void RecvProxy_Enabled( const CRecvProxyData *pData, void *pStruct, void *pOut );
	
    float CalculateRotationPose();
    float CalculateArmaturePose();
	
protected:
	
    void CreateEffect();
    void UpdateEffect();
    void StopEffect();
	
    float m_flLinearForce;
	
    bool m_bEffectsActive;
    bool m_bActivated;
    bool m_bNoEmitterParticles;
	
    float m_flArmatureStart;
    float m_flArmatureTarget;
    float m_flArmatureDuration;
    float m_flArmatureStartTime;
    float m_flRotationStart;
    float m_flRotationTarget;
    float m_flRotationDuration;
    float m_flRotationStartTime;
	
    Vector m_vEndPos;
	
	CUtlReference<CNewParticleEffect> m_hEmitterEffect[NUM_EMITTER_PARTICLES];
	
};

IMPLEMENT_CLIENTCLASS_DT( C_PropTractorBeamProjector, DT_PropTractorBeamProjector, CPropTractorBeamProjector )

	RecvPropFloat( RECVINFO( m_flLinearForce ), 0, &C_PropTractorBeamProjector::RecvProxy_LinearForce ),
	RecvPropBool( RECVINFO( m_bNoEmitterParticles ) ),
	//RecvPropBool( RECVINFO( m_bNoEmitterParticles ), 0, &C_PropTractorBeamProjector::RecvProxy_Enabled ),
	
	//RecvPropBool( RECVINFO( bDisableAutoReprojection ) ),

	RecvPropVector( RECVINFO( m_vEndPos ) ),

END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_PropTractorBeamProjector )
END_PREDICTION_DATA()

C_PropTractorBeamProjector::C_PropTractorBeamProjector( void )
{
	m_hFirstChild = NULL;
	m_hEmitterEffect[0] = NULL;
	m_hEmitterEffect[1] = NULL;
	m_hEmitterEffect[2] = NULL;
}

C_PropTractorBeamProjector::~C_PropTractorBeamProjector( void )
{

}

void C_PropTractorBeamProjector::Spawn( void )
{
	BaseClass::Spawn();
	SetNextClientThink( CLIENT_THINK_ALWAYS );
	m_bEffectsActive = false;
	m_bActivated = false;
	m_bNoEmitterParticles = true;
}

void C_PropTractorBeamProjector::UpdateOnRemove( void )
{
	StopEffect();
	BaseClass::UpdateOnRemove();
}

void C_PropTractorBeamProjector::ClientThink( void )
{
	if ( m_bEnabled && !m_bEffectsActive && m_flLinearForce != 0.0 && gpGlobals->curtime > 0.5)
	{
		CreateEffect();
		m_bActivated = true;
	}
	
	SetPoseParameter( GetModelPtr(), LookupPoseParameter( GetModelPtr(), "reversal" ), CalculateArmaturePose() );
	m_flPlaybackRate = CalculateRotationPose();
}

float C_PropTractorBeamProjector::CalculateArmaturePose( void )
{

	if (gpGlobals->curtime > (m_flArmatureStartTime + m_flArmatureDuration))
		return m_flArmatureTarget;

	float v5 = m_flArmatureStartTime + m_flArmatureDuration;

	float flArmatureGoal = m_flArmatureStart;

	if (m_flArmatureStartTime == (m_flArmatureStartTime + m_flArmatureDuration))
	{
		if (gpGlobals->curtime < v5)
			flArmatureGoal = m_flArmatureStart;
		else
			flArmatureGoal = m_flArmatureTarget;
	}
	else
	{
		float v7 = (gpGlobals->curtime - m_flArmatureStartTime) / (v5 - m_flArmatureStartTime);
		flArmatureGoal = ((((v7 * v7) * 3.0) - (((v7 * v7) * 2.0) * v7))
			* (m_flArmatureTarget - flArmatureGoal))
			+ flArmatureGoal;
	}
	if (flArmatureGoal < 0.0)
		return 0.0;
	if (flArmatureGoal > 1.0)
		return 1.0;
	else
		return flArmatureGoal;
}

float C_PropTractorBeamProjector::CalculateRotationPose(void)
{		
	if (gpGlobals->curtime > (m_flRotationStartTime + m_flRotationDuration))
		return m_flRotationTarget;

	float flRotationGoal = m_flRotationStart;
	float v5 = m_flRotationStartTime + m_flRotationDuration;
	if (m_flRotationStartTime == (m_flRotationStartTime + m_flRotationDuration))
	{
		if (gpGlobals->curtime < v5)
			flRotationGoal = m_flRotationStart;
		else
			flRotationGoal = m_flRotationTarget;
	}
	else
	{
		flRotationGoal = ((((((gpGlobals->curtime - m_flRotationStartTime)
			/ (v5 - m_flRotationStartTime))
			* ((gpGlobals->curtime - m_flRotationStartTime)
			/ (v5 - m_flRotationStartTime)))
			* 3.0)
			- (((((gpGlobals->curtime - m_flRotationStartTime)
			/ (v5 - m_flRotationStartTime))
			* ((gpGlobals->curtime - m_flRotationStartTime)
			/ (v5 - m_flRotationStartTime)))
			* 2.0)
			* ((gpGlobals->curtime - m_flRotationStartTime)
			/ (v5 - m_flRotationStartTime))))
			* (m_flRotationTarget - flRotationGoal))
			+ flRotationGoal;
	}
	float v7 = m_flLinearForce * 0.0083333338;

	if (v7 != 0.0)
	{
		bool v8;
		if (v7 >= 0.0)
			v8 = flRotationGoal <= v7;
		else
			v8 = v7 <= flRotationGoal;
		if (!v8)
			return (m_flLinearForce * 0.0083333338);
	}
	return flRotationGoal;
}

void C_PropTractorBeamProjector::CreateEffect( void )
{
	StopEffect();
	if (!m_bNoEmitterParticles)
	{
		char *pAttachmentNames[3];
		pAttachmentNames[0] = "emitter1";
		pAttachmentNames[1] = "emitter2";
		pAttachmentNames[2] = "emitter3";

		for (int i = 0; i < NUM_EMITTER_PARTICLES; ++i)
		{
			if (m_hEmitterEffect[i])
			{
				ParticleProp()->StopEmission( m_hEmitterEffect[i], 0, 0, 0, 1 );
				m_hEmitterEffect[i] = NULL;
			}

			m_hEmitterEffect[i] = ParticleProp()->Create( "tractor_beam_arm", PATTACH_POINT_FOLLOW, pAttachmentNames[i] );
			
			if (m_hEmitterEffect[i])
			{
				ParticleProp()->AddControlPoint( m_hEmitterEffect[i], 1, this, PATTACH_ABSORIGIN, 0, vec3_origin, 0 );
				ParticleProp()->AddControlPoint( m_hEmitterEffect[i], 2, this, PATTACH_ABSORIGIN, 0, vec3_origin, 0 );
				
				Vector color = g_vTractorBeamColorReverse;
				if ( m_flLinearForce >= 0.0 )
					color = g_vTractorBeamColorForward;

				m_hEmitterEffect[i]->SetControlPoint( 1, color );
				
				matrix3x4_t matWorldTransform = EntityToWorldTransform();

				Vector vVelocity;
				vVelocity.x = matWorldTransform.m_flMatVal[0][0] * m_flLinearForce;
				vVelocity.y = matWorldTransform.m_flMatVal[1][0] * m_flLinearForce;
				vVelocity.z = matWorldTransform.m_flMatVal[2][0] * m_flLinearForce;
				m_hEmitterEffect[i]->SetControlPoint( 2, vVelocity );
			}
		}
	}
	m_bEffectsActive = true;
}

void C_PropTractorBeamProjector::UpdateEffect( void )
{
	for ( int i = 0; i < NUM_EMITTER_PARTICLES; ++i)
	{
		if (m_hEmitterEffect[i])
		{
			Vector color;
			color = g_vTractorBeamColorForward;
			if ( m_flLinearForce < 0.0 )
				color = g_vTractorBeamColorReverse;

			m_hEmitterEffect[i]->SetControlPoint( 1, color );
			
			matrix3x4_t matWorldSpace = EntityToWorldTransform();
			
			Vector vVelocity;
			vVelocity.x = matWorldSpace.m_flMatVal[0][0] * m_flLinearForce;
			vVelocity.y = matWorldSpace.m_flMatVal[1][0] * m_flLinearForce;
			vVelocity.z = matWorldSpace.m_flMatVal[2][0] * m_flLinearForce;
			m_hEmitterEffect[i]->SetControlPoint( 2, vVelocity );
		}
	}
}

void C_PropTractorBeamProjector::StopEffect( void )
{
	for (int i = 0; i < NUM_EMITTER_PARTICLES; ++i)
	{
		if (m_hEmitterEffect[i])
		{
			ParticleProp()->StopEmission(m_hEmitterEffect[i], 0, 0, 0, 1);
		}
	}

	m_bEffectsActive = false;
}

void C_PropTractorBeamProjector::OnToolStartRecording( void )
{
	if (m_bEffectsActive)
		CreateEffect();
}

void C_PropTractorBeamProjector::RecvProxy_Enabled( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_PropTractorBeamProjector *pTractorBeam = static_cast<C_PropTractorBeamProjector*>( pStruct );

	bool bEnabled = pData->m_Value.m_Int == 1;
	if (pTractorBeam->m_bEnabled == bEnabled)
		goto LABEL_10;
	pTractorBeam->m_bEnabled = pData->m_Value.m_Int == 1;
	if (!bEnabled)
	{
		pTractorBeam->m_flRotationStart = pTractorBeam->CalculateRotationPose();
		pTractorBeam->m_flRotationStartTime = gpGlobals->curtime;
		pTractorBeam->m_flRotationDuration = 1.5;
		pTractorBeam->m_flRotationTarget = 0.0;
		pTractorBeam->m_flArmatureStart = pTractorBeam->CalculateArmaturePose();
		pTractorBeam->m_flArmatureTarget = 0.5;
		pTractorBeam->m_flArmatureDuration = 1.5;
		pTractorBeam->m_flArmatureStartTime = gpGlobals->curtime;
		pTractorBeam->StopEffect();
		goto LABEL_10;
	}
	pTractorBeam->m_flRotationStart = pTractorBeam->CalculateRotationPose();
	pTractorBeam->m_flRotationStartTime = gpGlobals->curtime;
	pTractorBeam->m_flRotationDuration = 0.25;
	pTractorBeam->m_flRotationTarget = pTractorBeam->m_flLinearForce * 0.0083333338;
	pTractorBeam->m_flArmatureStart = pTractorBeam->CalculateArmaturePose();
	pTractorBeam->m_flArmatureTarget = pTractorBeam->m_flLinearForce <= 0.0 ? 0 : 1.0;
	pTractorBeam->m_flArmatureDuration = 0.75;
	pTractorBeam->m_flArmatureStartTime = gpGlobals->curtime;
	if ( !pTractorBeam->m_bEffectsActive )
	{
	LABEL_10:
		if (pTractorBeam->m_bActivated)
			return;
		goto LABEL_11;
	}
	if (pTractorBeam->m_bActivated)
	{
		pTractorBeam->CreateEffect();
		goto LABEL_10;
	}
LABEL_11:
	if (!pTractorBeam->m_bEnabled)
	{
		pTractorBeam->m_flRotationStart = pTractorBeam->CalculateRotationPose();
		pTractorBeam->m_flRotationStartTime = (gpGlobals->curtime);
		pTractorBeam->m_flRotationDuration = 0.0;
		pTractorBeam->m_flRotationTarget = 0.0;
		pTractorBeam->m_flArmatureStart = pTractorBeam->CalculateArmaturePose();
		pTractorBeam->m_flArmatureTarget = 0.5;
		pTractorBeam->m_flArmatureDuration = 0.0;
		pTractorBeam->m_flArmatureStartTime = (gpGlobals->curtime);
	}
}

void C_PropTractorBeamProjector::RecvProxy_LinearForce( const CRecvProxyData *pData, void *pStruct, void *pOut )
{	
	C_PropTractorBeamProjector *pTractorBeam = static_cast<C_PropTractorBeamProjector*>( pStruct );

	float flNewLinearForce = pData->m_Value.m_Float;
	if (pTractorBeam->m_flLinearForce != flNewLinearForce)
	{
		pTractorBeam->m_flLinearForce = flNewLinearForce;
		if ( pTractorBeam->m_bActivated )
		{
			pTractorBeam->m_flRotationStart = pTractorBeam->CalculateRotationPose();
			pTractorBeam->m_flRotationStartTime = gpGlobals->curtime;
			pTractorBeam->m_flRotationDuration = 0.25;
			pTractorBeam->m_flRotationTarget = flNewLinearForce * 0.0083333338;
			pTractorBeam->m_flArmatureStart = pTractorBeam->CalculateArmaturePose();
			pTractorBeam->m_flArmatureTarget = pTractorBeam->m_flLinearForce <= 0.0 ? 0 : 1.0;
			pTractorBeam->m_flArmatureDuration = 0.75;
			pTractorBeam->m_flArmatureStartTime = gpGlobals->curtime;
			pTractorBeam->UpdateEffect();
		}
	}
}