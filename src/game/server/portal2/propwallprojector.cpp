#include "cbase.h"
#include "soundenvelope.h"
#include "propwallprojector.h"
#include "projectedwallentity.h"

#if !defined ( NO_PROJECTED_WALL )

BEGIN_DATADESC( CPropWallProjector )

	DEFINE_SOUNDPATCH( m_pAmbientSound ),
	DEFINE_SOUNDPATCH( m_pAmbientMusic ),
	DEFINE_FIELD( m_hAmbientSoundProxy, FIELD_EHANDLE ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( prop_wall_projector, CPropWallProjector );

CPropWallProjector::CPropWallProjector()
{
	m_pAmbientSound = NULL;
	m_pAmbientMusic = NULL;
	m_hAmbientSoundProxy = NULL;
}

CPropWallProjector::~CPropWallProjector()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	if ( m_pAmbientSound )
	{
		controller.Shutdown( m_pAmbientSound );
		controller.SoundDestroy( m_pAmbientSound );
	}

	if ( m_pAmbientMusic )
	{
		controller.Shutdown( m_pAmbientMusic );
		controller.SoundDestroy( m_pAmbientMusic );
	}

	if ( m_hAmbientSoundProxy )
		UTIL_Remove( m_hAmbientSoundProxy );
}

void CPropWallProjector::Spawn( void )
{
	BaseClass::Spawn();
	Precache();
	SetModel( "models/props/wall_emitter.mdl" );
	SetSolid( SOLID_VPHYSICS );
	if ( !m_hAmbientSoundProxy )
	{
		ProjectedEntityAmbientSoundProxy *pSoundProxy = ProjectedEntityAmbientSoundProxy::Create(this);
		if (pSoundProxy)
		{
			m_hAmbientSoundProxy = pSoundProxy;
			AddEffects( 1024 );
			return;
		}
		m_hAmbientSoundProxy = NULL;
	}
	AddEffects( 1024 );
}

void CPropWallProjector::Precache( void )
{
	PrecacheModel("models/props/wall_emitter.mdl");
	PrecacheScriptSound("VFX.BridgeGlow");

	if (V_stristr( gpGlobals->mapname.ToCStr(), "puzzlemaker" ))
	{
		V_snprintf(s_AmbientBridgeMusic, 64, "music.ctc_lbout");
	}
	else
	{
		V_snprintf( s_AmbientBridgeMusic, 64, "music.%s_lbout", gpGlobals->mapname.ToCStr() );
	}
	if (PrecacheScriptSound(s_AmbientBridgeMusic) == -1)
	{
		V_snprintf(s_AmbientBridgeMusic, 64, "music.sp_all_maps_lbout");
		PrecacheScriptSound(s_AmbientBridgeMusic);
	}
}

void CPropWallProjector::Project( void )
{
	CReliableBroadcastRecipientFilter filter;

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	BaseClass::Project();
	if (!m_pAmbientSound)
	{
		filter.AddAllPlayers();
		filter.MakeReliable();

		m_pAmbientSound = controller.SoundCreate( filter, m_hAmbientSoundProxy->entindex(), "VFX.BridgeGlow" );
		controller.Play( m_pAmbientSound, 1.0, 100.0, 0 );
	}

	if (!m_pAmbientMusic)
	{
		filter.AddAllPlayers();
		filter.MakeReliable();
		
		m_pAmbientMusic = controller.SoundCreate( filter, m_hAmbientSoundProxy->entindex(), CPropWallProjector::s_AmbientBridgeMusic);
		controller.Play( m_pAmbientMusic, 1.0, 100.0 );
	}
}

void CPropWallProjector::Shutdown( void )
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	BaseClass::Shutdown();
	if (m_pAmbientSound)
	{
		controller.Shutdown( m_pAmbientSound );
		controller.SoundDestroy( m_pAmbientSound );
		m_pAmbientSound = NULL;
	}
	if (m_pAmbientMusic)
	{
		controller.Shutdown( m_pAmbientMusic );
		controller.SoundDestroy( m_pAmbientMusic );
		m_pAmbientMusic = NULL;
	}
}

CBaseProjectedEntity *CPropWallProjector::CreateNewProjectedEntity( void )
{
	 return CProjectedWallEntity::CreateNewInstance();
}

#endif // NO_PROJECTED_WALL