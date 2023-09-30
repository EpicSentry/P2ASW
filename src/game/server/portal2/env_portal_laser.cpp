#include "cbase.h"
#include "env_portal_laser.h"
#include "prop_portal_shared.h"
#include "portal_util_shared.h"
#include "player.h" 
#include "particle_parse.h"
#include "ieffects.h"
#include "util_shared.h"
#include "prop_weightedcube.h"
#include "point_laser_target.h"
#include "physicsshadowclone.h"


ConVar portal_laser_normal_update( "portal_laser_normal_update", "0.05f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar portal_laser_high_precision_update( "portal_laser_high_precision_update", "0.03f", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_debug_laser( "sv_debug_laser", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar new_portal_laser( "new_portal_laser", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar sv_laser_cube_autoaim( "sv_laser_cube_autoaim", "1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );

// constants
const int CPortalLaser::LASER_EYE_ATTACHMENT = 1;
const float CPortalLaser::LASER_RANGE = 8192;
const char* CPortalLaser::LASER_ATTACHMENT_NAME = "laser_attachment";
const float CPortalLaser::LASER_END_POINT_PULSE_SCALE = 4.0f;
const int CPortalLaser::LASER_ATTACHMENT = 1;

BEGIN_DATADESC(CPortalLaser)
DEFINE_KEYFIELD(m_modelName, FIELD_STRING, "model"),
DEFINE_KEYFIELD(m_bStartOff, FIELD_BOOLEAN, "StartState"),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOn", InputTurnOn),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOff", InputTurnOff),
DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
DEFINE_THINKFUNC( Think ),

DEFINE_FIELD( m_bLaserOn, FIELD_BOOLEAN ),
DEFINE_FIELD( m_hReflector, FIELD_EHANDLE ),
DEFINE_FIELD( m_hTouchingReflector, FIELD_EHANDLE ),
DEFINE_FIELD( m_vStartPoint, FIELD_VECTOR ),
DEFINE_FIELD( m_vEndPoint, FIELD_VECTOR ),


//DEFINE_FIELD(m_bIsHittingPortal, FIELD_BOOLEAN),
END_DATADESC()
LINK_ENTITY_TO_CLASS(env_portal_laser, CPortalLaser);


IMPLEMENT_SERVERCLASS_ST( CPortalLaser, DT_PortalLaser )

SendPropEHandle( SENDINFO( m_hReflector ) ),
SendPropBool( SENDINFO( m_bLaserOn ) ),
SendPropBool( SENDINFO( m_bShouldSpark ) ),
SendPropBool( SENDINFO( m_bUseParentDir ) ),
SendPropVector( SENDINFO( m_vStartPoint ) ),
SendPropVector( SENDINFO( m_vEndPoint ) ),
SendPropQAngles( SENDINFO( m_angParentAngles ) ),

END_SEND_TABLE()


#define	MASK_PORTAL_LASER (CONTENTS_SOLID|CONTENTS_MONSTER)

CPortalLaser::CPortalLaser()
{
	m_flLastDamageTime = 0.0;
	m_flLastDamageSoundTime = 0.0;
	m_bLaserOnly = false;
	m_bStartOff = false;
	m_flLastSparkTime = false;
	m_bIsLaserExtender = false;

	m_hTouchingReflector = NULL;
	m_hReflector = NULL;
}

void CPortalLaser::Think()
{
	float flValue;
	// Schedule the next think
	if ( m_bFromReflectedCube )
		flValue = portal_laser_normal_update.GetFloat();
	else
		flValue = portal_laser_high_precision_update.GetFloat();
		
	SetNextThink( flValue + gpGlobals->curtime, 0);

	if (m_bLaserOn)
	{
		UpdateLaser(); // Update the laser position if it's on
	}
}

void CPortalLaser::UpdateLaser()
{
	bool bShouldSpark = true;

	m_vStartPoint = GetAbsOrigin();
	QAngle angMuzzleDir;

	Vector vTempStart = m_vStartPoint;
	GetAttachment( LASER_ATTACHMENT, vTempStart, angMuzzleDir );
	m_vStartPoint = vTempStart;

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment(LASER_EYE_ATTACHMENT, vecEye, angEyeDir);

	Vector vecMuzzleDir;
	AngleVectors(angEyeDir, &vecMuzzleDir);

	CTraceFilterSimpleClassnameList masterTraceFilter(this, COLLISION_GROUP_NONE);
	masterTraceFilter.AddClassnameToIgnore("info_placement_helper");
	masterTraceFilter.AddClassnameToIgnore("player");
	masterTraceFilter.AddClassnameToIgnore("Player");
	masterTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	
	// Trace from the laser emitter to check if it hits a cube
	trace_t normalTrace;
	Ray_t ray;
	ray.Init( m_vStartPoint, m_vStartPoint + vecMuzzleDir * LASER_RANGE );
	UTIL_Portal_TraceRay( ray, MASK_PORTAL_LASER, &masterTraceFilter, &normalTrace );

	m_bFromReflectedCube = false;

	CBaseEntity *pEntity = normalTrace.m_pEnt;

	if ( pEntity )
	{
		// It's possible for a laser to hit a cube and a catcher at the same time.
		CPropWeightedCube *pCube = dynamic_cast<CPropWeightedCube*>( pEntity );

		if ( pCube && pCube->GetCubeType() == CUBE_REFLECTIVE )
		{
			bShouldSpark = false;

			m_hTouchingReflector = pCube;
			m_bFromReflectedCube = true;

			if (!pCube->HasLaser())
			{
				CPortalLaser *pNewLaser = (CPortalLaser*)CreateEntityByName("env_portal_laser");
				pNewLaser->m_bLaserOnly = true;
				pNewLaser->m_bIsLaserExtender = true;
				pNewLaser->SetParent( pCube );
				pNewLaser->SetAbsOrigin( pCube->GetAbsOrigin() );
				pNewLaser->SetAbsAngles( pCube->GetAbsAngles() );
				pNewLaser->m_hReflector = pCube;
				pNewLaser->m_angParentAngles = pCube->GetAbsAngles();
				pNewLaser->m_bUseParentDir = true;
				pNewLaser->m_bLaserOn = true;

				DispatchSpawn( pNewLaser );

				pCube->SetLaser( pNewLaser );
			}
		}
		else
		{

			bShouldSpark = !FClassnameIs( pEntity, "prop_laser_catcher" );

			if ( m_hTouchingReflector.Get() )
			{
				UTIL_Remove( m_hTouchingReflector->GetLaser() );
				m_hTouchingReflector->SetLaser( NULL );
				m_hTouchingReflector = NULL;
			}
		}
	}

	if ( GetParent() )
		m_angParentAngles = GetParent()->GetAbsAngles();

	// Deal damage to the player if they are touching the laser beam
	if (gpGlobals->curtime - m_flLastDamageTime >= 0.15f) // Damage every 0.15 seconds
	{

		CTraceFilterSimpleClassnameList playerTraceFilter(this, COLLISION_GROUP_NONE);
		playerTraceFilter.AddClassnameToIgnore("info_placement_helper");
		playerTraceFilter.AddClassnameToIgnore("prop_energy_ball");
		playerTraceFilter.AddClassnameToIgnore("prop_portal");

		// Trace from the laser emitter to check if it hits the player
		trace_t playerTrace;
		Ray_t ray_player;
		ray_player.Init( m_vStartPoint, m_vStartPoint + vecMuzzleDir * LASER_RANGE );
		UTIL_Portal_TraceRay( ray_player, MASK_PORTAL_LASER, &playerTraceFilter, &playerTrace );

		// No need to do a cast here since there's no player specific functions to call
		CBaseEntity *pPlayerTraceEnt = playerTrace.m_pEnt;

		bool bPlayer = false;

		if (!pPlayerTraceEnt)
		{
			bPlayer = false;
		}
		else
		{
			bPlayer = pPlayerTraceEnt->IsPlayer();
			
			if (!bPlayer)
			{				
				if( CPhysicsShadowClone::IsShadowClone( pPlayerTraceEnt ) )
				{
					CBaseEntity *pCloned = ((CPhysicsShadowClone *)pPlayerTraceEnt)->GetClonedEntity();
					if ( pCloned )
					{
						bPlayer = pCloned->IsPlayer();

						// Attack the player instead of their shadow clone.
						pPlayerTraceEnt = pCloned;
					}
				}
			}

		}

		if ( bPlayer )
		{
			// Player is touching the laser beam, deal damage and push them
			Vector vecPushDir = (pPlayerTraceEnt->GetAbsOrigin() - m_vStartPoint);
			VectorNormalize(vecPushDir);

			Vector vecLaserDir = vecMuzzleDir;
			VectorNormalize(vecLaserDir);
			Vector vecPushSideways = CrossProduct(vecPushDir, vecLaserDir);
			VectorNormalize(vecPushSideways);

			pPlayerTraceEnt->TakeDamage(CTakeDamageInfo(this, this, 10, DMG_ENERGYBEAM));
			pPlayerTraceEnt->ApplyAbsVelocityImpulse(vecPushSideways * 500.0f); // Adjust the push force as needed
			
			if (gpGlobals->curtime - m_flLastDamageSoundTime >= 0.55f) // Play sound every 0.55 seconds
			{
				m_flLastDamageSoundTime = gpGlobals->curtime;
				pPlayerTraceEnt->EmitSound( "HL2Player.BurnPain" );
			}
			m_flLastDamageTime = gpGlobals->curtime;
		}
	}

	if (!m_pBeam)
	{
		//m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		m_pBeam = CBeam::BeamCreate("", 0.2); // NOTE: We don't want this to render on the client because the client creates its own laser
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(10.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(m_vStartPoint + vecMuzzleDir * LASER_RANGE, m_vStartPoint);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(m_vStartPoint + vecMuzzleDir * LASER_RANGE);
		m_pBeam->SetEndPos(m_vStartPoint);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(m_vStartPoint, m_vStartPoint + vecMuzzleDir * LASER_RANGE);

	trace_t trace; // Used for determining the portal hit

	// Perform the portal detection trace from the origin to the muzzle direction
	//UTIL_Portal_TraceRay( ray , MASK_SHOT, &masterTraceFilter, &trace);

	enginetrace->TraceRay( rayPath, MASK_PORTAL_LASER, &masterTraceFilter, &trace );

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_PORTAL_LASER, &masterTraceFilter, &fEndFraction))
	{
	}

	m_vEndPoint = trace.endpos;
	
	// Note: This gives us an assert, but we need it for turrets
	m_pBeam->BeamDamageInstant( &trace, 0 );

	// Check if the trace hits any portals
	CPortal_Base2D* pLocalPortal = NULL;
	CPortal_Base2D* pRemotePortal = NULL;
	bool bHitPortal = UTIL_DidTraceTouchPortals(rayPath, trace, &pLocalPortal, &pRemotePortal);

	// Handle portal hit, if any
	if (bHitPortal)
	{
		if (pLocalPortal != NULL)
		{
			// Local portal, no need to do anything here
		}

		if (pRemotePortal != NULL)
		{
			//Msg("Remote Portal classname: %s\n", pRemotePortal->GetClassname());
			DoTraceFromPortal(pRemotePortal, trace, vecMuzzleDir); // Do the rest from another void
		}
	}
	else
	{
		// No portal hit
		//Msg("No portal hit\n"); No need to do anything else from here.
	}

	//v_vHitPos = vEndPoint;
	m_pBeam->PointsInit(m_vEndPoint, m_vStartPoint);

	
	m_bShouldSpark = bShouldSpark;

	// Spark effects are handled by the client now.
#if 0
	// Our high thinkrate means we're creating too many sparks, so let's do this
	if (gpGlobals->curtime - m_flLastSparkTime <= 0.1f)
	{
		bShouldSpark = false;
	}


	if ( bShouldSpark )
	{
		// laser_cutter_sparks doesn't work for some reason

		m_pBeam->PointsInit(m_vEndPoint, m_vStartPoint);

		g_pEffects->Sparks(m_vEndPoint, 2, 2, &vecMuzzleDir);

		m_flLastSparkTime = gpGlobals->curtime;
	}
#endif

	Ray_t targetRay;
	targetRay.Init( trace.startpos, trace.endpos );

	DamageAllTargetsInRay( targetRay );

}

void CPortalLaser::DamageAllTargetsInRay( Ray_t &ray )
{
	CBaseEntity *list[1024];

	CFlaggedEntitiesEnum rayEnum( list, 1024, 0 );
	partition->EnumerateElementsAlongRay( PARTITION_ENGINE_NON_STATIC_EDICTS, ray, false, &rayEnum );

	int nCount = rayEnum.GetCount();
	
	// Loop through all entities along the ray between the gun and the surface
	for ( int i = 0; i < nCount; i++ )
	{
		if( dynamic_cast<CPortalLaserTarget*>( list[i] ) != NULL )
		{
			CPortalLaserTarget *pTarget = static_cast<CPortalLaserTarget*>( list[i] );
			
			Assert(pTarget);
			CTakeDamageInfo info;
			info.SetAttacker(this);
			info.SetInflictor(this);
			info.SetDamage(1);
			info.SetDamageType(DMG_ENERGYBEAM);

			pTarget->OnTakeDamage( info );
		}
	}
}

void CPortalLaser::DoTraceFromPortal( CPortal_Base2D* pRemotePortal, trace_t &tr, Vector vecMuzzleDir )
{
	if (!pRemotePortal)
		return;
	if (!pRemotePortal->IsActivedAndLinked())
		return;

	Assert(pRemotePortal->m_hLinkedPortal);

	QAngle angPortalDir = pRemotePortal->GetAbsAngles();
	Vector vecPortalDir;
	AngleVectors(angPortalDir, &vecPortalDir);

	Vector vecRemoteOrigin;
	Vector vecRemoteMuzzleDir;

	VMatrix matThisToLinked = pRemotePortal->m_hLinkedPortal->MatrixThisToLinked();
	
	vecRemoteOrigin = matThisToLinked * tr.endpos;
	//vecRemoteMuzzleDir = matThisToLinked * vecMuzzleDir;
	UTIL_Portal_VectorTransform( matThisToLinked, vecMuzzleDir, vecRemoteMuzzleDir );

	Assert( vecRemoteOrigin != tr.endpos );

	//vecRemoteMuzzleDir += vecPortalDir;

	Vector vecPortalOrigin = pRemotePortal->GetAbsOrigin();


	// Perform the trace from the remote portal's origin
	trace_t remoteTrace;
	CTraceFilterSimpleClassnameList remoteTraceFilter(this, COLLISION_GROUP_NONE);
	remoteTraceFilter.AddClassnameToIgnore("info_placement_helper");
	remoteTraceFilter.AddClassnameToIgnore("player");
	remoteTraceFilter.AddClassnameToIgnore("Player");
	remoteTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	remoteTraceFilter.AddClassnameToIgnore("prop_portal");
	
	Ray_t ray;
	ray.Init( vecRemoteOrigin, vecRemoteOrigin + vecRemoteMuzzleDir * LASER_RANGE );

	enginetrace->TraceRay( ray, MASK_SHOT, &remoteTraceFilter, &remoteTrace );

	//UTIL_TraceLine( vecRemoteOrigin, vecRemoteOrigin + vecRemoteMuzzleDir * LASER_RANGE, MASK_SHOT, &remoteTraceFilter, &remoteTrace );

	// Check if the trace hits a laser catcher
	if (remoteTrace.m_pEnt)
	{
		// Print the class name and position of the hit entity
		DevMsg("Hit entity class name: %s at: %.2f %.2f %.2f\n", remoteTrace.m_pEnt->GetClassname(),
			remoteTrace.m_pEnt->GetAbsOrigin().x, remoteTrace.m_pEnt->GetAbsOrigin().y, remoteTrace.m_pEnt->GetAbsOrigin().z);
	}
	else
	{
		// No entity was hit
		//DevMsg("No entity hit by trace.\n");
	}

	/*
	// Define a new trace and trace filter for the player
	CTraceFilterSimpleClassnameList playerTraceFilter(this, COLLISION_GROUP_NONE);
	playerTraceFilter.AddClassnameToIgnore("info_placement_helper");
	playerTraceFilter.AddClassnameToIgnore("prop_energy_ball");
	playerTraceFilter.AddClassnameToIgnore("prop_portal");
	trace_t playerTrace;
	UTIL_TraceLine(vecRemoteOrigin, vecRemoteOrigin + vecRemoteMuzzleDir * FLOOR_TURRET_PORTAL_LASER_RANGE, MASK_SHOT, &playerTraceFilter, &playerTrace);

	// Deal damage to the player if they are touching the laser beam
	if (gpGlobals->curtime - m_flLastDamageTime >= 2.0f && playerTrace.m_pEnt && FClassnameIs(playerTrace.m_pEnt, "player"))
	{
		// Player is touching the laser beam, deal damage and push them
		Vector vecPushDir = (playerTrace.m_pEnt->GetAbsOrigin() - vecRemoteOrigin);
		VectorNormalize(vecPushDir);

		Vector vecLaserDir = vecRemoteMuzzleDir;
		VectorNormalize(vecLaserDir);
		Vector vecPushSideways = CrossProduct(vecPushDir, vecLaserDir);
		VectorNormalize(vecPushSideways);

		playerTrace.m_pEnt->TakeDamage(CTakeDamageInfo(this, this, 10, DMG_ENERGYBEAM));
		playerTrace.m_pEnt->ApplyAbsVelocityImpulse(vecPushSideways * 500.0f);

		m_flLastDamageTime = gpGlobals->curtime;
	}
	*/
	
	Ray_t targetRay;
	targetRay.Init( remoteTrace.startpos, remoteTrace.endpos );

	DamageAllTargetsInRay( targetRay );

}


void CPortalLaser::Spawn(void)
{
	Precache();
	SetMoveType( MOVETYPE_NONE );
	
	m_angParentAngles = vec3_angle;

	if (!m_bLaserOnly)
	{
		SetSolid( SOLID_BBOX );
	}
	else
	{
		AddEffects( EF_NODRAW );
	}

	// Note: We must have a model for angMuzzleDir
	SetModel("models/props/laser_emitter_center.mdl"); // Default model if it is not defined as a keyvalue
	
	// Check if the entity has a keyvalue for "model" and use that value to set the model path
	if (m_modelName != NULL_STRING)
	{
		const char* pszModelName = STRING(m_modelName);
		if (pszModelName && pszModelName[0] != '\0')
		{
			SetModel(pszModelName);
		}
	}

	if (m_bStartOff)
	{
		LaserOff();
	}
	else
	{
		LaserOn();
	}


	SetThink(&CPortalLaser::Think);
	SetNextThink(gpGlobals->curtime + 0.1f);

	BaseClass::Spawn();
}

void CPortalLaser::UpdateOnRemove( void )
{
	LaserOff();
	UTIL_Remove(m_pBeam);
	BaseClass::UpdateOnRemove();
}

void CPortalLaser::Precache(void)
{
	// Precache the model using the stored model path
	PrecacheModel(STRING(m_modelName));
	PrecacheModel("models/props/laser_emitter_center.mdl");

	PrecacheScriptSound( "HL2Player.BurnPain" );
	if (!m_bIsLaserExtender)
		PrecacheParticleSystem( "laser_start_glow" );
	else		
		PrecacheParticleSystem( "reflector_start_glow" );

}

void CPortalLaser::LaserOff(void)
{
	m_bShouldSpark = false;
	m_bLaserOn = false;
	//Msg("Laser Deactivating\n");
	if (m_pBeam)
	{
		m_pBeam->AddEffects(EF_NODRAW);
	}
	
	StopParticleEffects( this );
	
	CPropWeightedCube *pReflectorCube = dynamic_cast<CPropWeightedCube*>(GetParent());

	if (pReflectorCube)
	{			
		StopParticleEffects( pReflectorCube );
	}
}
//start laseron
void CPortalLaser::LaserOn(void)
{
	m_bLaserOn = true;
	//Msg("Laser Activating\n");

	Vector m_vStartPoint = GetAbsOrigin();
	QAngle angMuzzleDir;
	CBaseAnimating* pBaseAnimating = dynamic_cast<CBaseAnimating*>(this);
	bool bFoundAttachment = false;
	if (pBaseAnimating)
	{
		int lensBone = pBaseAnimating->LookupBone("lens");
		if (lensBone != -1)
		{
			pBaseAnimating->GetBonePosition(lensBone, m_vStartPoint, angMuzzleDir);
			bFoundAttachment = true;
		}
	}

	if (!bFoundAttachment)
	{
		int laserAttachmentIndex = LookupAttachment("laser_attachment");
		if (laserAttachmentIndex > 0)
		{
			GetAttachmentLocal(laserAttachmentIndex, m_vStartPoint, angMuzzleDir);
			bFoundAttachment = true;
		}
	}

	if (!bFoundAttachment)
	{
		LaserOff();
		return;
	}

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment(LASER_EYE_ATTACHMENT, vecEye, angEyeDir);

	Vector vecMuzzleDir;
	AngleVectors(angEyeDir, &vecMuzzleDir);

	if (!m_pBeam)
	{
		//m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		m_pBeam = CBeam::BeamCreate("", 0.2); // NOTE: We don't want this to render on the client because the client creates its own laser
		//m_pBeam->SetColor(255, 32, 32);
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(24.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(m_vStartPoint + vecMuzzleDir * LASER_RANGE, m_vStartPoint);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(m_vStartPoint + vecMuzzleDir * LASER_RANGE);
		m_pBeam->SetEndPos(m_vStartPoint);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	// Trace to find an endpoint
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(m_vStartPoint, m_vStartPoint + vecMuzzleDir * LASER_RANGE);

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_PORTAL_LASER, &traceFilter, &fEndFraction))
		m_vEndPoint = m_vStartPoint + vecMuzzleDir * LASER_RANGE;
	else
		m_vEndPoint = m_vStartPoint + vecMuzzleDir * LASER_RANGE * fEndFraction;

	m_pBeam->PointsInit(m_vEndPoint, m_vStartPoint);
	
	if (!m_bIsLaserExtender)
		DispatchParticleEffect( "laser_start_glow", PATTACH_POINT_FOLLOW, this, "laser_attachment", true, -1 );
	else
	{
		CPropWeightedCube *pReflectorCube = dynamic_cast<CPropWeightedCube*>(GetParent());

		if (pReflectorCube)
		{
			DispatchParticleEffect( "reflector_start_glow", PATTACH_ABSORIGIN_FOLLOW, pReflectorCube );
		}
	}
	
	
	CPropWeightedCube *pCube = assert_cast<CPropWeightedCube*>( m_hTouchingReflector.Get() );	
	if ( pCube )
	{
		CPortalLaser *pLaser = assert_cast<CPortalLaser*>( pCube->GetLaser() );

		if ( pLaser )
		{
			UTIL_Remove( pLaser );
			pCube->SetLaser( NULL );
		}
	}
}

//end laseron
//----------------------------------------------------------------------------
//Hammer entity options
//this section controls the numerous values this entity can have in hammer
//----------------------------------------------------------------------------
void CPortalLaser::InputTurnOn(inputdata_t& inputData)
{
	if (!m_bLaserOn)
		LaserOn();
}

void CPortalLaser::InputTurnOff(inputdata_t& inputData)
{
	if (m_bLaserOn)
		LaserOff();
}

void CPortalLaser::InputToggle(inputdata_t& inputData)
{
	if (m_bLaserOn)
		LaserOff();
	else
		LaserOn();
}

// Misc
float CPortalLaser::LaserEndPointSize(void)
{
	return ((MAX(0.0f, sinf(gpGlobals->curtime * M_PI + m_fPulseOffset))) * LASER_END_POINT_PULSE_SCALE + 3.0f) * 1.5f;
}

bool CPortalLaser::IsLaserHittingCube()
{
	return m_bFromReflectedCube;
}