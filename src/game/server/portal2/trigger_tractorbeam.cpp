#include "cbase.h"
#include "trigger_tractorbeam_shared.h"
#include "prop_weightedcube.h"
#include "npc_portal_turret_floor.h"
#include "particle_parse.h"
#include "prop_tractorbeam.h"

BEGIN_DATADESC( CProjectedTractorBeamEntity )

	DEFINE_FIELD( m_hTractorBeamTrigger, FIELD_EHANDLE ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CProjectedTractorBeamEntity, DT_ProjectedTractorBeamEntity )

	SendPropEHandle( SENDINFO(m_hTractorBeamTrigger) ),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( projected_tractor_beam_entity, CProjectedTractorBeamEntity )

//-------------------------------------------------//

BEGIN_DATADESC( CTrigger_TractorBeam )
	DEFINE_SOUNDPATCH( m_sndPlayerInBeam ),

	DEFINE_FIELD( m_gravityScale, FIELD_FLOAT ),
	DEFINE_FIELD( m_addAirDensity, FIELD_FLOAT ),
	DEFINE_FIELD( m_linearLimit, FIELD_FLOAT ),
	DEFINE_FIELD( m_linearLimitDelta, FIELD_FLOAT ),
	DEFINE_FIELD( m_linearLimitTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_linearLimitStart, FIELD_TIME ),
	DEFINE_FIELD( m_linearLimitStartTime, FIELD_TIME ),
	DEFINE_FIELD( m_linearScale, FIELD_FLOAT ),
	DEFINE_FIELD( m_angularLimit, FIELD_FLOAT ),
	DEFINE_FIELD( m_angularScale, FIELD_FLOAT ),
	DEFINE_FIELD( m_linearForceAngles, FIELD_VECTOR ),
	DEFINE_FIELD( m_vStart, FIELD_VECTOR ),
	DEFINE_FIELD( m_vEnd, FIELD_VECTOR ),
	DEFINE_FIELD( m_bReversed, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bFromPortal, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bToPortal, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bDisablePlayerMove, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flRadius, FIELD_FLOAT ),
	DEFINE_FIELD( m_hProxyEntity, FIELD_EHANDLE ),

	DEFINE_THINKFUNC( TractorThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CTrigger_TractorBeam, DT_Trigger_TractorBeam )

	SendPropFloat( SENDINFO( m_gravityScale ) ),
	SendPropFloat( SENDINFO( m_addAirDensity ) ),
	SendPropFloat( SENDINFO( m_linearLimit ) ),
	SendPropFloat( SENDINFO( m_linearLimitDelta ) ),
	SendPropFloat( SENDINFO( m_linearLimitTime ) ),
	SendPropFloat( SENDINFO( m_linearLimitStart ) ),
	SendPropFloat( SENDINFO( m_linearLimitStartTime ) ),
	SendPropFloat( SENDINFO( m_linearScale ) ),
	SendPropFloat( SENDINFO( m_angularLimit ) ),
	SendPropFloat( SENDINFO( m_angularScale ) ),
	SendPropFloat( SENDINFO( m_linearForce ) ),
	SendPropFloat( SENDINFO( m_flRadius ) ),
	
	SendPropQAngles( SENDINFO( m_linearForceAngles ) ),
	SendPropVector( SENDINFO( m_vStart ) ),
	SendPropVector( SENDINFO( m_vEnd ) ),
	
	SendPropBool( SENDINFO( m_bDisabled ) ),
	SendPropBool( SENDINFO( m_bReversed ) ),
	SendPropBool( SENDINFO( m_bFromPortal ) ),
	SendPropBool( SENDINFO( m_bToPortal ) ),
	SendPropBool( SENDINFO( m_bDisablePlayerMove ) ),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( trigger_tractorbeam, CTrigger_TractorBeam )


IMPLEMENT_AUTO_LIST( ITriggerTractorBeamAutoList )

CProjectedTractorBeamEntity::CProjectedTractorBeamEntity( void )
{
	//m_flLinearForce = 0.0;
}

CProjectedTractorBeamEntity::~CProjectedTractorBeamEntity( void )
{

}

void CProjectedTractorBeamEntity::Spawn( void )
{
	BaseClass::Spawn();
	m_hTractorBeamTrigger = CTrigger_TractorBeam::CreateTractorBeam( m_vecStartPoint, m_vecEndPoint, this );
	
	SetTransmitState( FL_EDICT_ALWAYS ); // 8
}

void CProjectedTractorBeamEntity::UpdateOnRemove( void )
{
	CheckForSettledReflectorCubes();

	if ( m_hTractorBeamTrigger )
		UTIL_Remove( m_hTractorBeamTrigger );

	BaseClass::UpdateOnRemove();
}

void CProjectedTractorBeamEntity::GetProjectionExtents( Vector &outMins, Vector &outMaxs )
{
	outMins.x = -2.0;
	outMins.y = -2.0;
	outMins.z = 0.0;

	outMaxs.x = 2.0;
	outMaxs.y = 2.0;
	outMaxs.z = 0.0;
}

void CProjectedTractorBeamEntity::OnProjected( void )
{
	BaseClass::OnProjected();
	if ( m_hTractorBeamTrigger )
	{
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
		
		m_hTractorBeamTrigger->UpdateBeam( GetStartPoint(), GetEndPoint(), GetLinearForce() );	
	}
}

void CProjectedTractorBeamEntity::OnPreProjected( void )
{
	if ( m_hTractorBeamTrigger )
	{
		CheckForSettledReflectorCubes();
	}
}


float CProjectedTractorBeamEntity::GetLinearForce( void )
{
	CPropTractorBeamProjector *pPropTractor = assert_cast<CPropTractorBeamProjector*>(m_hOwnerEntity.Get());

	// If we don't have a projector, then wtf?
	Assert( pPropTractor );

	return pPropTractor->GetLinearForce();
}

CProjectedTractorBeamEntity *CProjectedTractorBeamEntity::CreateNewInstance(void)
{
	return (CProjectedTractorBeamEntity*)CreateEntityByName("projected_tractor_beam_entity");
}

CBaseProjectedEntity *CProjectedTractorBeamEntity::CreateNewProjectedEntity()
{
	return CreateNewInstance();
}


CTrigger_TractorBeam::CTrigger_TractorBeam()
{
	m_blobs.Purge();

	m_hProxyEntity = NULL;
	m_sndPlayerInBeam = NULL;
	m_flRadius = 55.0;
}

CTrigger_TractorBeam::~CTrigger_TractorBeam()
{
	RemoveAllBlobsFromBeam();
}

void CTrigger_TractorBeam::Spawn( void )
{
	Precache();
	SetSolid( SOLID_VPHYSICS );
	AddSolidFlags( FSOLID_NOT_SOLID );

	SetMoveType( MOVETYPE_NONE, MOVECOLLIDE_DEFAULT );
	
	CreateVPhysics();
	
	SetTransmitState( FL_EDICT_PVSCHECK );
	SetThink( &CTrigger_TractorBeam::TractorThink );
	SetNextThink( gpGlobals->curtime );
	
	m_bDisablePlayerMove = false;
}

void CTrigger_TractorBeam::Precache( void )
{
	PrecacheMaterial( "effects/tractor_beam" );
	PrecacheMaterial( "effects/tractor_beam2" );
	PrecacheMaterial( "effects/tractor_beam3" );
	PrecacheScriptSound( "VFX.PlayerEnterTbeam" );
	PrecacheScriptSound( "VFX.PlayerEnterTbeam_SS" );
}

void CTrigger_TractorBeam::Activate( void )
{
	BaseClass::Activate();
}

void CTrigger_TractorBeam::OnRestore( void )
{
	BaseClass::OnRestore();

	if ( m_pController )
		m_pController->SetEventHandler( this );

	SetTransmitState( FL_EDICT_PVSCHECK );
}

void CTrigger_TractorBeam::UpdateOnRemove( void )
{
	if ( m_pController )
	{
		int objectcount = m_pController->CountObjects();

		IPhysicsObject *pPhysObjects[1024];
		m_pController->GetObjects( pPhysObjects );
		for ( int i = 0; i < objectcount; ++i )
		{
			if ( pPhysObjects[i] )
				pPhysObjects[i]->Wake();
		}
		
		physenv->DestroyMotionController( m_pController );
		m_pController = NULL;
		
		//free(pPhysObjects);
	}

	StopParticleEffects( this );
	StopLoopingSounds();
	BaseClass::UpdateOnRemove();
}

void CTrigger_TractorBeam::StopLoopingSounds( void )
{
	if ( m_sndPlayerInBeam )
	{
		CSoundEnvelopeController &Controller = CSoundEnvelopeController::GetController();
		Controller.Shutdown( m_sndPlayerInBeam );
		Controller.SoundDestroy( m_sndPlayerInBeam );
		m_sndPlayerInBeam = NULL;
	}
}

bool CTrigger_TractorBeam::CreateVPhysics( void )
{
	m_pController = physenv->CreateMotionController( this );
	return 1;
}

void CTrigger_TractorBeam::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );

	if ( !PassesTriggerFilters( pOther ) )
		return;
	
	if ( pOther && pOther->ClassMatches( "npc_portal_turret_floor") )
	{
		assert_cast<CNPC_Portal_FloorTurret*>( pOther )->OnEnteredTractorBeam();
	}
	else if ( UTIL_IsReflectiveCube( pOther ) && pOther )
	{
		assert_cast<CPropWeightedCube*>( pOther )->OnEnteredTractorBeam();
	}
	
	CPortal_Player *pPlayer = ToPortalPlayer( pOther );
	if ( pPlayer )
	{
		pPlayer->SetInTractorBeam( this );
		
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		if ( m_sndPlayerInBeam )
		{
			controller.Shutdown( m_sndPlayerInBeam );
			controller.SoundDestroy( m_sndPlayerInBeam );
			m_sndPlayerInBeam = NULL;
		}

		EmitSound_t ep;
		ep.m_nChannel = CHAN_STATIC;
		if ( gpGlobals->maxClients > 1 && (pPlayer->HasAttachedSplitScreenPlayers() || pPlayer->IsSplitScreenPlayer()) )
			ep.m_pSoundName = "VFX.PlayerEnterTbeam_SS";
		else
			ep.m_pSoundName = "VFX.PlayerEnterTbeam";		

		ep.m_flVolume = 1.0;
		ep.m_SoundLevel = SNDLVL_NORM;

		CPASAttenuationFilter filter( GetAbsOrigin() );
		filter.AddRecipient( pPlayer );

		m_sndPlayerInBeam = controller.SoundCreate( filter, entindex(), ep );
		controller.Play( m_sndPlayerInBeam, 1.0, 100 );
	}
	else
	{
		EntityBeamHistory_t &history = g_TractorBeamManager.GetHistoryFromEnt( pOther );
		if ( history.IsDifferentBeam( this ) )
		{
			triggerevent_t event;
			if ( m_pController && PhysGetTriggerEvent( &event, this ) && event.pObject )
			{
				event.pObject->Wake();
				m_pController->AttachObject( event.pObject, true );
			}
			if ( history.m_beams.Count() )
			{
				ForceDetachEntity( pOther );
			}
			history.UpdateBeam( this );
		}
	}
}

void CTrigger_TractorBeam::ForceAttachEntity( CBaseEntity *pEntity )
{
	IPhysicsObject *m_pPhysicsObject = pEntity->VPhysicsGetObject();
	if (m_pPhysicsObject)
	{
		if ( m_pController )
		{
			m_pPhysicsObject->Wake();
			m_pController->AttachObject( m_pPhysicsObject, true );
		}
	}
}

void CTrigger_TractorBeam::ForceDetachEntity( CBaseEntity *pEntity )
{
	IPhysicsObject *m_pPhysicsObject = pEntity->VPhysicsGetObject();
	if ( m_pPhysicsObject )
	{
		if ( m_pController )
		{
			m_pPhysicsObject->Wake();
			m_pController->DetachObject( m_pPhysicsObject );
		}
	}
}

void CTrigger_TractorBeam::WakeTouchingObjects( void )
{
	// NOTE: This may need to be changed...
	Vector vecRayMins = Vector( -m_flRadius, -m_flRadius, -m_flRadius );
	Vector vecRayMaxs = Vector( m_flRadius, m_flRadius, m_flRadius );

	Ray_t ray;
	ray.Init( m_vStart, m_vEnd, vecRayMins, vecRayMaxs );

	CBaseEntity *pList[1024];
	int count = UTIL_EntitiesAlongRay( pList, 1024, ray, MASK_OPAQUE );
	
	for ( int i = 0; i < count; ++i )
	{
		CBaseEntity *pEntity = pList[i];

		if ( !pEntity )
			continue;
		
		// Cubes in a disabled state don't move, se we force it
		if ( UTIL_IsReflectiveCube( pEntity ) || UTIL_IsSchrodinger( pEntity ) )
			assert_cast<CPropWeightedCube*>( pEntity )->ExitDisabledState();

		IPhysicsObject *pPhys = pEntity->VPhysicsGetObject();

		if (!pPhys)
			continue;
		
		pPhys->Wake();
	}
}

void CTrigger_TractorBeam::TractorThink( void )
{
	if ( m_hProxyEntity )
	{
		m_bFromPortal = m_hProxyEntity->GetSourcePortal() != NULL;
		m_bToPortal = m_hProxyEntity->GetHitPortal() != NULL;
	}
	SetNextThink( gpGlobals->curtime );
}


float CTrigger_TractorBeam::GetLinearForce( void )
{
	return m_linearForce;
}

bool CTrigger_TractorBeam::HasLinearLimit( void )
{
	return m_linearLimit != 0.0;
}

bool CTrigger_TractorBeam::HasLinearScale( void )
{
	return m_linearScale != 1.0;
}

bool CTrigger_TractorBeam::HasAngularScale( void )
{
	return m_angularScale != 1.0;
}

bool CTrigger_TractorBeam::HasAngularLimit( void )
{
	return m_angularLimit != 0.0;
}

bool CTrigger_TractorBeam::HasAirDensity( void )
{
	return m_addAirDensity != 0.0;
}

void CTrigger_TractorBeam::SetLinearLimit( float flLinearLimit )
{
	m_linearLimit = flLinearLimit;
}

void CTrigger_TractorBeam::SetLinearForce( float flLinearForce )
{
	m_linearForce = flLinearForce;
}

void CTrigger_TractorBeam::SetLinearForce( Vector vDir, float flLinearForce )
{
	m_linearForce = flLinearForce;
	
	QAngle qDirection;

	VectorAngles( vDir, qDirection );

	m_linearForceAngles = qDirection;
}

void CTrigger_TractorBeam::SetAsReversed( bool bReversed )
{
	m_bReversed = bReversed;
}

void CTrigger_TractorBeam::SetGravityScale( float flGravityScale )
{
	m_gravityScale = flGravityScale;
}

void CTrigger_TractorBeam::InputSetVelocityLimitTime( inputdata_t &inputdata )
{
	m_linearLimitStart = GetLinearLimit();
	m_linearLimitStartTime = gpGlobals->curtime;
	
	float args[2];
	UTIL_StringToFloatArray( args, 2, inputdata.value.String() );
	
	m_linearLimit = args[0];
	m_linearLimitTime = args[1];
}

CTrigger_TractorBeam *CTrigger_TractorBeam::CreateTractorBeam( const Vector &vStart, const Vector &vEnd, CProjectedTractorBeamEntity *pOwner )
{
	CTrigger_TractorBeam *pBeam = (CTrigger_TractorBeam *)CreateEntityByName( "trigger_tractorbeam" );

	pBeam->Precache();						
	pBeam->UpdateBeam( pOwner->GetStartPoint(), pOwner->GetEndPoint(), pOwner->GetLinearForce() );
	pBeam->SetOwnerEntity( pOwner );
	pBeam->m_hProxyEntity = pOwner;

	DispatchSpawn( pBeam );
	
	return pBeam;
}
