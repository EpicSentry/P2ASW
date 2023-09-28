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

//DEFINE_FIELD(m_bIsHittingPortal, FIELD_BOOLEAN),
END_DATADESC()
LINK_ENTITY_TO_CLASS(env_portal_laser, CPortalLaser);
/*
IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_EnvPortalLaser)

SendPropBool(SENDINFO(m_bIsHittingPortal)),
SendPropVector(SENDINFO(v_vHitPos)),
SendPropVector(SENDINFO(vecNetOrigin)),
SendPropVector(SENDINFO(vecNetMuzzleDir)),

END_SEND_TABLE()
*/

CPortalLaser::CPortalLaser()
{
	m_flLastDamageTime = 0.0;
	m_flLastDamageSoundTime = 0.0;
	m_bLaserOnly = false;
	m_bStartOff = false;
	m_flLastSparkTime = false;
	m_bIsLaserExtender = false;

	m_hReflectorCube = NULL;
}

void CPortalLaser::Think()
{
	// Schedule the next think
	SetNextThink(gpGlobals->curtime + 0.015f);

	if (m_bLaserOn)
	{
		UpdateLaser(); // Update the laser position if it's on
	}
}

void CPortalLaser::UpdateLaser()
{
	bool bShouldSpark = true;

	Vector vecOrigin = GetAbsOrigin(); 
	QAngle angMuzzleDir;
	GetAttachment(LASER_ATTACHMENT, vecOrigin, angMuzzleDir);

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
	ray.Init( vecOrigin, vecOrigin + vecMuzzleDir * LASER_RANGE );
	UTIL_Portal_TraceRay( ray, MASK_SHOT, &masterTraceFilter, &normalTrace );

	m_bIsLaserHittingCube = false;

	CBaseEntity *pEntity = normalTrace.m_pEnt;

	if ( pEntity )
	{
		// It's possible for a laser to hit a cube and a catcher at the same time.
		CPropWeightedCube *pCube = dynamic_cast<CPropWeightedCube*>( pEntity );

		if ( pCube && pCube->GetCubeType() == CUBE_REFLECTIVE )
		{
			bShouldSpark = false;

			m_hReflectorCube = pCube;
			m_bIsLaserHittingCube = true;

			if (!pCube->HasLaser())
			{
				CPortalLaser *pNewLaser = (CPortalLaser*)CreateEntityByName("env_portal_laser");
				pNewLaser->m_bLaserOnly = true;
				pNewLaser->m_bIsLaserExtender = true;
				pNewLaser->SetParent( pCube );
				pNewLaser->SetAbsOrigin( pCube->GetAbsOrigin() );
				pNewLaser->SetAbsAngles( pCube->GetAbsAngles() );
				DispatchSpawn( pNewLaser );
				pCube->SetLaser( pNewLaser );
			}			
		}
		else
		{

			bShouldSpark = !FClassnameIs( pEntity, "prop_laser_catcher" );

			if ( m_hReflectorCube.Get() )
			{
				UTIL_Remove( m_hReflectorCube->GetLaser() );
				m_hReflectorCube->SetLaser( NULL );
				m_hReflectorCube = NULL;
			}
		}
	}
	
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
		ray_player.Init( vecOrigin, vecOrigin + vecMuzzleDir * LASER_RANGE );
		UTIL_Portal_TraceRay( ray_player, MASK_SHOT, &playerTraceFilter, &playerTrace );

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
					}
				}
			}

		}

		if ( bPlayer )
		{
			// Player is touching the laser beam, deal damage and push them
			Vector vecPushDir = (pPlayerTraceEnt->GetAbsOrigin() - vecOrigin);
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
		m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(10.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(vecOrigin + vecMuzzleDir * LASER_RANGE, vecOrigin);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(vecOrigin + vecMuzzleDir * LASER_RANGE);
		m_pBeam->SetEndPos(vecOrigin);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecOrigin, vecOrigin + vecMuzzleDir * LASER_RANGE);

	trace_t trace; // Used for determining the portal hit

	// Perform the portal detection trace from the origin to the muzzle direction
	//UTIL_Portal_TraceRay( ray , MASK_SHOT, &masterTraceFilter, &trace);

	enginetrace->TraceRay( rayPath, MASK_SHOT, &masterTraceFilter, &trace );

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &masterTraceFilter, &fEndFraction))
	{
		vEndPoint = vecOrigin + vecMuzzleDir * LASER_RANGE;
		//Msg("Portal Beam End Point: (%f, %f, %f)\n", vEndPoint.x, vEndPoint.y, vEndPoint.z);
	}
	else
	{
		vEndPoint = vecOrigin + vecMuzzleDir * LASER_RANGE * fEndFraction;
		//Msg("Main Trace End Point: (%f, %f, %f)\n", vEndPoint.x, vEndPoint.y, vEndPoint.z);
	}
	
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
	m_pBeam->PointsInit(vEndPoint, vecOrigin);
	
	// Our high thinkrate means we're creating too many sparks, so let's do this
	if (gpGlobals->curtime - m_flLastSparkTime <= 0.1f)
	{
		bShouldSpark = false;
	}

	if ( bShouldSpark )
	{
		// laser_cutter_sparks doesn't work for some reason
#if 0
		QAngle qPlaneNormal;
		VectorAngles( trace.plane.normal, qPlaneNormal );

		m_flLastSparkTime = gpGlobals->curtime;
		DispatchParticleEffect( "laser_cutter_sparks", normalTrace.endpos, qPlaneNormal );
#else
		m_pBeam->PointsInit(vEndPoint, vecOrigin);

		g_pEffects->Sparks(vEndPoint, 2, 2, &vecMuzzleDir);
#endif
	}
	Ray_t targetRay;
	targetRay.Init( normalTrace.startpos, normalTrace.endpos );

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

	DamageAllTargetsInRay( ray );

}


void CPortalLaser::Spawn(void)
{
	Precache();
	SetMoveType( MOVETYPE_NONE );
	
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

	PrecacheParticleSystem( "laser_cutter_sparks" );
}

void CPortalLaser::LaserOff(void)
{
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

	Vector vecOrigin = GetAbsOrigin();
	QAngle angMuzzleDir;
	CBaseAnimating* pBaseAnimating = dynamic_cast<CBaseAnimating*>(this);
	bool bFoundAttachment = false;
	if (pBaseAnimating)
	{
		int lensBone = pBaseAnimating->LookupBone("lens");
		if (lensBone != -1)
		{
			pBaseAnimating->GetBonePosition(lensBone, vecOrigin, angMuzzleDir);
			bFoundAttachment = true;
		}
	}

	if (!bFoundAttachment)
	{
		int laserAttachmentIndex = LookupAttachment("laser_attachment");
		if (laserAttachmentIndex > 0)
		{
			GetAttachmentLocal(laserAttachmentIndex, vecOrigin, angMuzzleDir);
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
		m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 0.2);
		//m_pBeam->SetColor(255, 32, 32);
		m_pBeam->SetBrightness(92);
		m_pBeam->SetNoise(0);
		m_pBeam->SetWidth(24.0f);
		m_pBeam->SetEndWidth(0);
		m_pBeam->SetScrollRate(0);
		m_pBeam->SetFadeLength(0);
		m_pBeam->SetCollisionGroup(COLLISION_GROUP_NONE);
		m_pBeam->PointsInit(vecOrigin + vecMuzzleDir * LASER_RANGE, vecOrigin);
		m_pBeam->SetBeamFlag(FBEAM_REVERSED);
		m_pBeam->SetStartEntity(this);
	}
	else
	{
		m_pBeam->SetStartPos(vecOrigin + vecMuzzleDir * LASER_RANGE);
		m_pBeam->SetEndPos(vecOrigin);
		m_pBeam->RemoveEffects(EF_NODRAW);
	}

	// Trace to find an endpoint
	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecOrigin, vecOrigin + vecMuzzleDir * LASER_RANGE);

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &traceFilter, &fEndFraction))
		vEndPoint = vecOrigin + vecMuzzleDir * LASER_RANGE;
	else
		vEndPoint = vecOrigin + vecMuzzleDir * LASER_RANGE * fEndFraction;

	m_pBeam->PointsInit(vEndPoint, vecOrigin);
	
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
	return m_bIsLaserHittingCube;
}