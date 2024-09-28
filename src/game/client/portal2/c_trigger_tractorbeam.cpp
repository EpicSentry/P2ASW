#include "cbase.h"
#include "trigger_tractorbeam_shared.h"
#include "c_portal_player.h"
#include "soundinfo.h"

#undef CProjectedTractorBeamEntity // Just in case

IMPLEMENT_CLIENTCLASS_DT( C_ProjectedTractorBeamEntity, DT_ProjectedTractorBeamEntity, CProjectedTractorBeamEntity )
	RecvPropEHandle( RECVINFO(m_hTractorBeamTrigger) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_ProjectedTractorBeamEntity )

	DEFINE_FIELD( m_hTractorBeamTrigger, FIELD_EHANDLE ),

END_PREDICTION_DATA()

C_ProjectedTractorBeamEntity::C_ProjectedTractorBeamEntity()
{

}

C_ProjectedTractorBeamEntity::~C_ProjectedTractorBeamEntity()
{

}

void C_ProjectedTractorBeamEntity::GetProjectionExtents( Vector &outMins, Vector &outMaxs )
{
	outMins.x = -2.0;
	outMins.y = -2.0;
	outMins.z = 0.0;
	outMaxs.x = 2.0;
	outMaxs.y = 2.0;
	outMaxs.z = 0.0;
}

void C_ProjectedTractorBeamEntity::OnProjected( void )
{
	BaseClass::OnProjected();
	if ( m_hTractorBeamTrigger )
	{
		m_hTractorBeamTrigger->SetPredictionEligible( GetPredictionEligible() );

		if ( IsPlayerSimulated() )
		{
			if ( GetSimulatingPlayer() )
				m_hTractorBeamTrigger->SetPlayerSimulated( GetSimulatingPlayer() );
			else
				m_hTractorBeamTrigger->SetPlayerSimulated( NULL );
		}
		else
		{
			m_hTractorBeamTrigger->UnsetPlayerSimulated();
		}
		
		m_hTractorBeamTrigger->UpdateBeam( GetStartPoint(), GetEndPoint(), m_hTractorBeamTrigger->GetLinearForce() );
	}
}

#undef CTrigger_TractorBeam

IMPLEMENT_CLIENTCLASS_DT( C_Trigger_TractorBeam, DT_Trigger_TractorBeam, CTrigger_TractorBeam )

	RecvPropFloat( RECVINFO( m_gravityScale ) ),
	RecvPropFloat( RECVINFO( m_addAirDensity ) ),
	RecvPropFloat( RECVINFO( m_linearLimit ) ),
	RecvPropFloat( RECVINFO( m_linearLimitDelta ) ),
	RecvPropFloat( RECVINFO( m_linearLimitTime ) ),
	RecvPropFloat( RECVINFO( m_linearLimitStart ) ),
	RecvPropFloat( RECVINFO( m_linearLimitStartTime ) ),
	RecvPropFloat( RECVINFO( m_linearScale ) ),
	RecvPropFloat( RECVINFO( m_angularLimit ) ),
	RecvPropFloat( RECVINFO( m_angularScale ) ),
	RecvPropFloat( RECVINFO( m_linearForce ) ),
	RecvPropFloat( RECVINFO( m_flRadius ) ),
	
	RecvPropQAngles( RECVINFO( m_linearForceAngles ) ),
	RecvPropVector( RECVINFO( m_vStart ) ),
	RecvPropVector( RECVINFO( m_vEnd ) ),
	
	RecvPropBool( RECVINFO( m_bDisabled ) ),
	RecvPropBool( RECVINFO( m_bReversed ) ),
	RecvPropBool( RECVINFO( m_bFromPortal ) ),
	RecvPropBool( RECVINFO( m_bToPortal ) ),
	RecvPropBool( RECVINFO( m_bDisablePlayerMove ) ),

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( trigger_tractorbeam, C_Trigger_TractorBeam )


BEGIN_PREDICTION_DATA( C_Trigger_TractorBeam )

	DEFINE_PRED_FIELD( m_linearForceAngles, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vStart, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_vEnd, FIELD_VECTOR, FTYPEDESC_INSENDTABLE ),

END_PREDICTION_DATA()


IMPLEMENT_AUTO_LIST( ITriggerTractorBeamAutoList )

C_Trigger_TractorBeam::C_Trigger_TractorBeam()
{
	m_blobs.Purge();
	m_hProxyEntity = NULL;
	m_hCoreEffect = NULL;
	m_bDisabled = false;
}

C_Trigger_TractorBeam::~C_Trigger_TractorBeam()
{
	RemoveAllBlobsFromBeam();
	m_hCoreEffect();
}

void C_Trigger_TractorBeam::Spawn( void )
{
	BaseClass::Spawn();
	if (!m_pMaterial1)
		m_pMaterial1 = materials->FindMaterial( "effects/tractor_beam", NULL, false );
	if (!m_pMaterial2)
		m_pMaterial2 = materials->FindMaterial( "effects/tractor_beam2", NULL, false );
	if (!m_pMaterial3)
		m_pMaterial3 = materials->FindMaterial( "effects/tractor_beam3", NULL, false );

	SetNextClientThink( CLIENT_THINK_ALWAYS );

}

void C_Trigger_TractorBeam::UpdateOnRemove( void )
{
	if (m_pController)
	{
		IPhysicsObject *pList[1024];
		int objectcount = m_pController->CountObjects();
		m_pController->GetObjects( pList );
		for ( int i = 0; i < objectcount; ++i)
		{
			if ( pList[i] )
				pList[i]->Wake();
		}

		physenv->DestroyMotionController( m_pController );
		m_pController = NULL;
	}

	if (m_hCoreEffect)
	{
		ParticleProp()->StopEmission( m_hCoreEffect, false, false, false, true );
	}

	for ( int i = 1; i <= MAX_PLAYERS; ++i)
	{
		C_Portal_Player* pPlayer = (C_Portal_Player *)UTIL_PlayerByIndex( i );
		
		if ( !pPlayer )
			continue;

		C_Trigger_TractorBeam *pTrigger = pPlayer->GetPortalPlayerLocalData().m_hTractorBeam;

		if ( pTrigger == this )
			pPlayer->SetLeaveTractorBeam( this, false );
	}
	BaseClass::UpdateOnRemove();
}

void C_Trigger_TractorBeam::StartTouch( C_BaseEntity *pOther )
{
	C_Portal_Player *pPlayer = ToPortalPlayer( pOther );

	if ( pPlayer )
	{
		pPlayer->SetInTractorBeam( this );
	}
	else
	{
		if ( m_pController && m_pPhysicsObject )
		{
			 m_pController->AttachObject( m_pPhysicsObject, true );
		}
	}

	BaseClass::StartTouch( pOther );
}

int C_Trigger_TractorBeam::DrawModel( int flags, const RenderableInstance_t &instance )
{
	ConVarRef gpu_level("gpu_level");
	
	Vector vDir = m_vEnd - m_vStart;

	bool bDrawOuterColumn = !(( gpu_level.GetInt() - 1 < 0 ) || gpu_level.GetInt() == 1);

	float flLength = VectorNormalize(vDir);

	if ((gpGlobals->curtime - m_flStartTime) < 0.5)
	{
		float flMod1;

		// This code doesn't make any fucking sense
		if (m_flStartTime == (m_flStartTime + 0.5))
		{
			if ( gpGlobals->curtime < ( m_flStartTime + 0.5 ) )
				flMod1 = 0.0;
			else
				flMod1 = 1.0;
		}
		else
		{
			float flMod2;
			float flRemainingTime = (gpGlobals->curtime - m_flStartTime) / ( ( m_flStartTime + 0.5 ) - m_flStartTime );
			if (flRemainingTime >= 0.0)
			{
				if (flRemainingTime <= 1.0)
					flMod2 = flRemainingTime;
				else
					flMod2 = 1.0;
			}
			else
			{
				flMod2 = 0.0;
			}
			flMod1 = ( (flMod2 * flMod2) * 3.0 ) - ( ( (flMod2 * flMod2) * 2.0 ) * flMod2 );
		}
		flLength = flMod1 * flLength;
	}

	matrix3x4_t xform;
	QAngle angles;

	VectorAngles( vDir, angles );
	AngleMatrix( angles, m_vStart, xform );

	Vector xAxis;
	Vector yAxis;
	MatrixGetColumn( xform, 2, xAxis);
	MatrixGetColumn( xform, 1, yAxis);

	C_Trigger_TractorBeam::DrawColumn( m_pMaterial1, m_vStart, vDir, flLength, xAxis, yAxis, 58.0, 1.0, m_bFromPortal, m_bToPortal, 0.0 );

	if (bDrawOuterColumn)
		C_Trigger_TractorBeam::DrawColumn( m_pMaterial2, m_vStart, vDir, flLength, xAxis, yAxis, 60.0, 1.0, m_bFromPortal, m_bToPortal, 0.0 );
	return 1;
}

void C_Trigger_TractorBeam::DrawColumn( IMaterial *pMaterial, Vector &vecStart, Vector vDir, float flLength,
	Vector &vecXAxis, Vector &vecYAxis, float flRadius, float flAlpha, bool bPinchIn, bool bPinchOut, float flTextureOffset )
{
#if 1
	CMatRenderContextPtr pRenderContext( materials );
	IMesh* pMesh = pRenderContext->GetDynamicMesh( false, NULL, NULL, pMaterial );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 256 );
	
	DrawColumnSegment( meshBuilder, vecStart, vDir, flLength, vecXAxis, vecYAxis, flRadius, flAlpha, flTextureOffset, pMaterial->GetVertexFormat() );
	
	meshBuilder.End();
	
	pMesh->Draw();
#endif
}

void C_Trigger_TractorBeam::DrawColumnSegment( CMeshBuilder &meshBuilder, Vector &vecStart, Vector &vDir, float flLength, Vector &vecXAxis,
										Vector &vecYAxis, float flRadius, float flAlpha, float flTextureOffset, VertexFormat_t vertexFormat )
{
	bool bSetTangentS = (vertexFormat & VERTEX_TANGENT_S) != 0;
	bool bSetTangentT = (vertexFormat & VERTEX_TANGENT_T) != 0;

	float color[3];
	if ( m_bReversed )
	{
		color[0] = 1.0;
		color[1] = 0.125;
	}
	else
	{
		color[0] = 0.0390625;
		color[1] = 1.0;
	}
	color[2] = 0;
	
	float flLastV = 0.0;
	float flU = flLength * 0.00390625;

	Vector vecPosition = vecStart + (vecXAxis *flRadius);

	int i = 1;
	while (1)
	{
		Vector vecLastPosition = vecPosition;
		float fl = i * 0.098174773; // Unnamed
		float flCos = cos( fl );
		float flSin = sin( fl );
		
		float flV = i * 0.015625; 
		vecPosition = (vecStart + ( vecXAxis * flCos ) * flRadius) + ( ( vecYAxis * flSin ) * flRadius );
		Vector normal = vecStart - vecPosition;

		Vector tangents = vDir;
		VectorNormalize( tangents );
		Vector tangentt = (tangents * normal) - (tangents * normal);
		VectorNormalize( tangentt );
		VectorNormalize( normal );
		
		// Vert 1
		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, flV, 0 );
		meshBuilder.Position3fv( vecPosition.Base() );

		if ( vertexFormat & VERTEX_NORMAL )
			meshBuilder.Normal3fv( normal.Base() );

		if ( bSetTangentS )
			meshBuilder.TangentS3fv( tangents.Base() );
		if ( bSetTangentT )
			meshBuilder.TangentT3fv( tangentt.Base() );

		meshBuilder.AdvanceVertex();
		// Vert 2
		Vector vert = vDir * flLength + vecPosition;
		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, flV, flU );
		meshBuilder.Position3fv( vert.Base() );
		
		if ( vertexFormat & VERTEX_NORMAL )
			meshBuilder.Normal3fv( normal.Base() );

		if ( bSetTangentS )
			meshBuilder.TangentS3fv( tangents.Base() );
		if ( bSetTangentT )
			meshBuilder.TangentT3fv( tangentt.Base() );

		meshBuilder.AdvanceVertex();
		// Vert 3
		Vector vStart = vDir * flLength + vecLastPosition;
		normal = vecStart - vecLastPosition;
		VectorNormalize( normal );
		tangentt = (tangents * normal) - (tangents * normal);
		VectorNormalize( tangentt );
		
		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, flLastV, flU );
		meshBuilder.Position3fv( vert.Base() );
		
		if ( vertexFormat & VERTEX_NORMAL )
			meshBuilder.Normal3fv( normal.Base() );

		if ( bSetTangentS )
			meshBuilder.TangentS3fv( tangents.Base() );
		if ( bSetTangentT )
			meshBuilder.TangentT3fv( tangentt.Base() );

		meshBuilder.AdvanceVertex();
		// Vert 4
		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, flLastV, 0.0 );
		meshBuilder.Position3fv( vecLastPosition.Base() );
		
		if ( vertexFormat & VERTEX_NORMAL )
			meshBuilder.Normal3fv( normal.Base() );

		if ( bSetTangentS )
			meshBuilder.TangentS3fv( tangents.Base() );
		if ( bSetTangentT )
			meshBuilder.TangentT3fv( tangentt.Base() );

		meshBuilder.AdvanceVertex();

		if (++i > 64)
			break;

		flLastV = flV;
	}

	vecStart = vecStart + (vDir * flLength);
}

bool C_Trigger_TractorBeam::GetSoundSpatialization( SpatializationInfo_t& info )
{
	bool bResult = false;
	float outT[4]; // [esp+2Ch] [ebp-1Ch] BYREF
		
	if ( IsDormant() )
		return 0;

	if (info.pOrigin)
	{
		CalcClosestPointOnLine( info.info.vListenerOrigin, m_vStart, m_vEnd, *info.pOrigin, outT );
		if (outT[0] >= 0.0)
		{
			if (outT[0] > 1.0)
			{
				bResult = true;
				*info.pOrigin = m_vEnd;
				if (!info.pAngles)
					return bResult;
				goto LABEL_6;
			}
		}
		else
		{
			*info.pOrigin = m_vStart;
		}
	}
	bResult = true;
	if (info.pAngles)
	{
	LABEL_6:
			
		QAngle qAng = CollisionProp()->GetCollisionAngles();

		*info.pAngles = qAng;
	}
	
	return bResult;
}

void C_Trigger_TractorBeam::CreateParticles( void )
{
	if (m_hCoreEffect)
	{
		ParticleProp()->StopEmission( m_hCoreEffect, false, false, false, true );
		m_hCoreEffect = NULL;
	}
	m_hCoreEffect = ParticleProp()->Create( "tractor_beam_core", PATTACH_CUSTOMORIGIN );


	if (m_hCoreEffect)
	{
		ParticleProp()->AddControlPoint( m_hCoreEffect, 1, this, PATTACH_CUSTOMORIGIN );
		ParticleProp()->AddControlPoint( m_hCoreEffect, 2, this, PATTACH_CUSTOMORIGIN );
		ParticleProp()->AddControlPoint( m_hCoreEffect, 3, this, PATTACH_CUSTOMORIGIN );

		Vector vDir;
		vDir = m_vEnd - m_vStart;
		VectorNormalize( vDir );

		Vector vRight;
		Vector vUp;
		VectorVectors( vDir, vRight, vUp );

		m_hCoreEffect->SetControlPoint( 0, m_vStart );
		m_hCoreEffect->SetControlPointOrientation( 0, vDir, vRight, vUp );
		m_hCoreEffect->SetControlPoint( 1, m_vEnd );

		Vector forward;
		forward = -vDir;

		m_hCoreEffect->SetControlPointOrientation( 1, forward, vRight, vUp );

		matrix3x4_t matWorldTransform = EntityToWorldTransform();

		Vector vVelocity;
		vVelocity.x = matWorldTransform.m_flMatVal[0][0] * m_linearForce;
		vVelocity.y = matWorldTransform.m_flMatVal[1][0] * m_linearForce;
		vVelocity.z = matWorldTransform.m_flMatVal[2][0] * m_linearForce;
		
		m_hCoreEffect->SetControlPoint( 2, vVelocity );

		Vector color;
		if (m_bReversed)
		{
			color.x = 255;
			color.y = 56;
			color.z = 8;
		}
		else
		{
			color.x = 0.0;
			color.y = 49;
			color.z = 189;
		}
		m_hCoreEffect->SetControlPoint( 3, color );
	}
}

void C_Trigger_TractorBeam::PhysicsSimulate( void )
{
	BaseClass::PhysicsSimulate();
}

void C_Trigger_TractorBeam::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	if ( updateType == DATA_UPDATE_DATATABLE_CHANGED )
	{
		if ( !m_bRecreateParticles )
			return;
		m_bRecreateParticles = false;
	}
	else
	{
		AddEffects( EF_NOFLASHLIGHT );
		if ( !m_pController )
		{
			m_pController = physenv->CreateMotionController( this );

			if (m_bReversed)
				m_linearForce = -m_linearForce;

			UpdateBeam( GetStartPoint(), GetEndPoint(), m_linearForce );
		}
	}
	CreateParticles();
}

C_BasePlayer *C_Trigger_TractorBeam::GetPredictionOwner( void )
{
	return GetSimulatingPlayer();
}

bool C_Trigger_TractorBeam::ShouldPredict( void )
{
	return GetSimulatingPlayer() == C_BasePlayer::GetLocalPlayer();
}

void C_Trigger_TractorBeam::OnNewParticleEffect( const char *pszParticleName, CNewParticleEffect *pNewParticleEffect )
{
	if ( !V_stricmp(pszParticleName, "tractor_beam_src") )
		pNewParticleEffect->SetControlPoint( 2, m_vEnd );
}

RenderableTranslucencyType_t C_Trigger_TractorBeam::ComputeTranslucencyType( void )
{
	return RENDERABLE_IS_TRANSLUCENT;
}

void C_Trigger_TractorBeam::GetToolRecordingState( KeyValues *msg )
{
	//No IFM for Swarm!
#if 0
	const char *HandlerIDKeyString; // eax

	BaseClass::GetToolRecordingState(msg);
	KeyValues *pKeyValues = CIFM_EntityKeyValuesHandler_AutoRegister::FindOrCreateNonConformantKeyValues(msg);
	HandlerIDKeyString = CIFM_EntityKeyValuesHandler_AutoRegister::GetHandlerIDKeyString();
	pKeyValues->SetString( HandlerIDKeyString, "C_Trigger_TractorBeam");

	pKeyValues->SetInt( "entIndex", entindex());
	pKeyValues->SetFloat( "starttime", m_flStartTime);
	pKeyValues->SetInt( "reversed", m_bReversed);
	pKeyValues->SetFloat( "force", m_linearForce);

	pKeyValues->SetFloat( "sp_x", m_vStart.x);
	pKeyValues->SetFloat( "sp_y", m_vStart.y);
	pKeyValues->SetFloat( "sp_z", m_vStart.z);

	pKeyValues->SetFloat( "ep_x", m_vEnd.x);
	pKeyValues->SetFloat( "ep_y", m_vEnd.y);
	pKeyValues->SetFloat( "ep_z", m_vEnd.z);
#endif
}

void C_Trigger_TractorBeam::RestoreToToolRecordedState( KeyValues *pKV )
{
#if 0
	_DWORD vecMin[2]; // [esp+Ch] [ebp-18h] BYREF
	unsigned __int64 vecMin_8; // [esp+14h] [ebp-10h] BYREF

	m_flStartTime = pKV->GetFloat("starttime", 0.0);
	m_bReversed = pKV->GetInt("reversed", 0) == 1;
	m_linearForce = pKV->GetFloat("force", 0.0);
	m_vStart.x = pKV->GetFloat("sp_x", 0.0);
	m_vStart.y = pKV->GetFloat("sp_y", 0.0);
	m_vStart.z = pKV->GetFloat("sp_z", 0.0);
	m_vEnd.x = pKV->GetFloat("ep_x", 0.0);
	m_vEnd.y = pKV->GetFloat("ep_y", 0.0);
	m_vEnd.z = pKV->GetFloat("ep_z", 0.0);
	float v5 = 16384.0;
	float v6 = 16384.0;
	*(float *)vecMin = 16384.0;
	*(float *)&vecMin[1] = 16384.0;
	vecMin_8 = __PAIR64__(LODWORD(16384.0), LODWORD(16384.0));
	SetSize( (const Vector *)vecMin, (const Vector *)((char *)&vecMin_8 + 4));
#endif
	m_pMaterial1 = materials->FindMaterial("effects/tractor_beam", 0, 0, 0);
	m_pMaterial2 = materials->FindMaterial("effects/tractor_beam2", 0, 0, 0);
	m_pMaterial3 = materials->FindMaterial("effects/tractor_beam3", 0, 0, 0);
}

// NOTE: This was copied from CPSCollisionEntity::UpdatePartitionListEntry, but this should work.
void C_Trigger_TractorBeam::UpdatePartitionListEntry()
{
	partition->RemoveAndInsert( 
		PARTITION_CLIENT_RESPONSIVE_EDICTS | PARTITION_CLIENT_NON_STATIC_EDICTS | PARTITION_CLIENT_TRIGGER_ENTITIES | PARTITION_CLIENT_IK_ATTACHMENT,  // remove
		PARTITION_CLIENT_SOLID_EDICTS | PARTITION_CLIENT_STATIC_PROPS,  // add
		CollisionProp()->GetPartitionHandle() );
}

bool C_Trigger_TractorBeam::ShouldDraw( void )
{
	return true;
}

float C_Trigger_TractorBeam::GetLinearForce( void )
{
	return m_linearForce;
}

bool C_Trigger_TractorBeam::HasLinearLimit( void )
{
	return m_linearLimit != 0.0;
}

bool C_Trigger_TractorBeam::HasLinearScale( void )
{
	return m_linearScale != 1.0;
}

bool C_Trigger_TractorBeam::HasAngularScale( void )
{
	return m_angularScale != 1.0;
}

bool C_Trigger_TractorBeam::HasAngularLimit( void )
{
	return m_angularLimit != 0.0;
}

bool C_Trigger_TractorBeam::HasAirDensity( void )
{
	return m_addAirDensity != 0.0;
}