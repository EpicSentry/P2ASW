#include "cbase.h"
#include "prop_tractorbeam.h"
#include "soundenvelope.h"
#include "trigger_tractorbeam_shared.h"


IMPLEMENT_SERVERCLASS_ST( ProjectedEntityAmbientSoundProxy, DT_ProjectedEntityAmbientSoundProxy )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( projected_entity_ambient_sound_proxy, ProjectedEntityAmbientSoundProxy )

ProjectedEntityAmbientSoundProxy::ProjectedEntityAmbientSoundProxy()
{

}

ProjectedEntityAmbientSoundProxy::~ProjectedEntityAmbientSoundProxy()
{

}

int ProjectedEntityAmbientSoundProxy::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_DONTSEND );
}

ProjectedEntityAmbientSoundProxy *ProjectedEntityAmbientSoundProxy::Create( CBaseEntity *pAttachTo )
{
	ProjectedEntityAmbientSoundProxy *pSoundProxy = (ProjectedEntityAmbientSoundProxy *)CreateEntityByName( "projected_entity_ambient_sound_proxy" );
	Assert( pSoundProxy );
	pSoundProxy->AttachToEntity( pAttachTo );

	return pSoundProxy;
}

void ProjectedEntityAmbientSoundProxy::AttachToEntity( CBaseEntity *pAttachTo )
{
	SetParent( pAttachTo );
	SetLocalOrigin( vec3_origin );
	SetLocalAngles( vec3_angle );
}

BEGIN_DATADESC( CPropTractorBeamProjector )

	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetLinearForce", InputSetLinearForce ),
	
	DEFINE_KEYFIELD( m_flLinearForce, FIELD_FLOAT, "linearForce" ),
	DEFINE_KEYFIELD( m_bNoEmitterParticles, FIELD_BOOLEAN, "noemitterparticles" ),
	DEFINE_KEYFIELD( m_bUse128Model, FIELD_BOOLEAN, "use128model" ),

	DEFINE_SOUNDPATCH( m_sndMechanical ),
	DEFINE_SOUNDPATCH( m_sndAmbientSound ),
	DEFINE_SOUNDPATCH( m_sndAmbientMusic ),

	DEFINE_FIELD( m_hAmbientSoundProxy, FIELD_EHANDLE ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( prop_tractor_beam, CPropTractorBeamProjector )

IMPLEMENT_SERVERCLASS_ST( CPropTractorBeamProjector, DT_PropTractorBeamProjector )

	SendPropFloat( SENDINFO( m_flLinearForce ) ),
	SendPropBool( SENDINFO( m_bNoEmitterParticles ) ),
	//SendPropBool( SENDINFO( bDisableAutoReprojection ) ),

	SendPropVector( SENDINFO( m_vEndPos ) ),

END_SEND_TABLE()

CPropTractorBeamProjector::CPropTractorBeamProjector()
{
	m_bUse128Model = false;
	m_sndMechanical = NULL;
	m_sndAmbientMusic = NULL;
	m_hAmbientSoundProxy = NULL;
}

CPropTractorBeamProjector::~CPropTractorBeamProjector()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_sndMechanical )
	{
		controller.Shutdown( m_sndMechanical );
		controller.SoundDestroy( m_sndMechanical );
	}
	
	if ( m_sndAmbientMusic )
	{
		controller.Shutdown( m_sndAmbientMusic );
		controller.SoundDestroy( m_sndAmbientMusic );
	}
	
	UTIL_Remove( m_hAmbientSoundProxy );
}

void CPropTractorBeamProjector::Spawn()
{
	BaseClass::Spawn();
	Precache();
	AddEffects( EF_NOFLASHLIGHT );
	
	if ( m_bUse128Model )
		SetModel( "models/props_ingame/tractor_beam_128.mdl" );
	else
		SetModel( "models/props/tractor_beam_emitter.mdl" );

	SetSolid( SOLID_VPHYSICS );
	ResetSequence( 2 );
	UseClientSideAnimation();

	m_hAmbientSoundProxy = ProjectedEntityAmbientSoundProxy::Create( this );

	SetFadeDistance( -1.0, 0.0 );
	SetGlobalFadeScale( 0.0 );
	AddEffects( EF_NOSHADOW );
}

void CPropTractorBeamProjector::Precache()
{
	if ( m_bUse128Model )
		PrecacheModel("models/props_ingame/tractor_beam_128.mdl");
	else
		PrecacheModel("models/props/tractor_beam_emitter.mdl");

	PrecacheParticleSystem("tractor_beam_arm");
	PrecacheParticleSystem("tractor_beam_core");
	PrecacheScriptSound("VFX.TbeamEmitterSpinLp");
	PrecacheScriptSound("VFX.TBeamPosPolarity");
	PrecacheScriptSound("VFX.TBeamNegPolarity");


	V_snprintf( s_AmbientBeamMusic, sizeof( s_AmbientBeamMusic ), "music.%s_tbout", gpGlobals->mapname.ToCStr() );
	if ( PrecacheScriptSound( s_AmbientBeamMusic ) == -1 )
	{
		V_snprintf( s_AmbientBeamMusic, 64, "music.sp_all_maps_tbout");
		PrecacheScriptSound( s_AmbientBeamMusic );
	}
	UTIL_PrecacheOther("trigger_tractorbeam");
}

void CPropTractorBeamProjector::Project( void )
{
	BaseClass::Project();

	m_vEndPos = m_hFirstChild->GetEndPoint();

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	if ( !m_sndMechanical )
	{
		EmitSound_t ep;
		ep.m_nSpeakerEntity = -1;
		ep.m_hSoundScriptHandle = -1; // Was m_hSoundScriptHash, but Swarm uses m_hSoundScriptHandle instead.
		ep.m_nFlags = 0;
		ep.m_nPitch = 100;
		//memset(&ep.m_pOrigin, 0, 12);
		ep.m_bEmitCloseCaption = 1;
		ep.m_bWarnOnDirectWaveReference = 0;
		//memset(&ep.m_UtlVecSoundOrigin, 0, sizeof(ep.m_UtlVecSoundOrigin));
		//ep.m_nSoundEntryVersion = 1;
		ep.m_nChannel = 6;
		ep.m_pSoundName = "VFX.TbeamEmitterSpinLp";
		ep.m_flVolume = 1.0;
		ep.m_SoundLevel = SNDLVL_NORM;
		ep.m_pOrigin = &GetAbsOrigin();

		CReliableBroadcastRecipientFilter filter;
		filter.AddRecipientsByPAS( GetAbsOrigin() );

		m_sndMechanical = controller.SoundCreate( filter, entindex(), ep );
		
		//CUtlVector<ITriggerTractorBeamAutoList *, CUtlMemory<ITriggerTractorBeamAutoList *, int>>::~CUtlVector<ITriggerTractorBeamAutoList *, CUtlMemory<ITriggerTractorBeamAutoList *, int>>((CUtlVector<__m128, CUtlMemory<__m128, int> > *)&ep.m_UtlVecSoundOrigin);
	}
	controller.Play( m_sndMechanical, 0.1, 100.0 );
	controller.SoundChangeVolume( m_sndMechanical, 1.0, 0.75 );
	if ( !m_sndAmbientMusic )
	{
		CReliableBroadcastRecipientFilter filter;
		filter.AddAllPlayers();
		filter.MakeReliable();
		
		m_sndAmbientMusic = controller.SoundCreate( filter, m_hAmbientSoundProxy->entindex(), s_AmbientBeamMusic );
	}
	controller.Play( m_sndAmbientMusic, 1.0, 100.0 );

	const char *soundName = IsReversed() ? "VFX.TBeamNegPolarity" : "VFX.TBeamPosPolarity";

	bool bIsSameSound = false;
	if ( m_sndAmbientSound )
	{
		const char *pszAmbientSoundName = controller.SoundGetScriptName( m_sndAmbientSound ).ToCStr();
		if (!pszAmbientSoundName)
			pszAmbientSoundName = "";
	
		bIsSameSound = V_strcmp(pszAmbientSoundName, soundName) != 0;
	}
	if ( !m_sndAmbientSound || bIsSameSound )
	{
		if ( !m_sndAmbientSound )
		{
			controller.Shutdown( m_sndAmbientSound );
			controller.SoundDestroy( m_sndAmbientSound );
			m_sndAmbientSound = NULL;
		}

		CReliableBroadcastRecipientFilter filter;
		filter.AddAllPlayers();
		filter.MakeReliable();

		m_sndAmbientSound = controller.SoundCreate( filter, m_hAmbientSoundProxy->entindex(), soundName );

		controller.Play( m_sndAmbientSound, 1.0, 100.0, 0 );
	}
}

void CPropTractorBeamProjector::Shutdown( void )
{
	BaseClass::Shutdown();
	
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_sndMechanical )
		controller.SoundFadeOut( m_sndMechanical, 1.5 );

	if ( m_sndAmbientMusic )
	{
		controller.Shutdown( m_sndAmbientMusic );
		controller.SoundDestroy( m_sndAmbientMusic );
		m_sndAmbientMusic = NULL;
	}
	if ( m_sndAmbientSound )
	{
		controller.Shutdown( m_sndAmbientSound );
		controller.SoundDestroy( m_sndAmbientSound );
		m_sndAmbientSound = NULL;
	}
}

bool CPropTractorBeamProjector::IsReversed( void )
{
	return m_flLinearForce < 0.0;
}


CBaseProjectedEntity *CPropTractorBeamProjector::CreateNewProjectedEntity( void )
{
	return CProjectedTractorBeamEntity::CreateNewInstance();
}

void CPropTractorBeamProjector::InputSetLinearForce( inputdata_t &inputdata )
{
	m_flLinearForce = inputdata.value.Float();
	
	if ( m_flLinearForce == 0.0)
	{
		EnableProjection( false );
		m_vEndPos = vec3_origin;
	}
	else if ( m_bEnabled )
	{
		Project();
	}
}
