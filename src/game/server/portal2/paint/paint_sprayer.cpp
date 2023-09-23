//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"

#include "paint_sprayer.h"
#include "soundenvelope.h"
#include "paint/paint_stream_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar max_noisy_blobs_per_second;
extern ConVar blobs_paused;

extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );
extern void SendProxy_Angles( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );

IMPLEMENT_SERVERCLASS_ST( CPaintSprayer, DT_PaintSprayer )

	SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_COORD | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropEHandle(SENDINFO_NAME( m_hMoveParent, moveparent ) ),
	SendPropQAngles( SENDINFO( m_angRotation ), 13, SPROP_CHANGES_OFTEN, SendProxy_Angles ),

	SendPropBool( SENDINFO( m_bActive ) ),
	SendPropBool( SENDINFO( m_bSilent ) ),
	SendPropBool( SENDINFO( m_bDrawOnly ) ),

	SendPropInt( SENDINFO( m_nBlobRandomSeed ) ),
	SendPropInt( SENDINFO( m_PaintPowerType ) ),
	SendPropInt( SENDINFO( m_nMaxBlobCount ), 0, SPROP_UNSIGNED ),

	SendPropFloat( SENDINFO( m_flBlobsPerSecond ) ),
	SendPropFloat( SENDINFO( m_flMinSpeed ) ),
	SendPropFloat( SENDINFO( m_flMaxSpeed ) ),
	SendPropFloat( SENDINFO( m_flBlobSpreadRadius ) ),
	SendPropFloat( SENDINFO( m_flBlobSpreadAngle ) ),

	SendPropFloat( SENDINFO( m_flStreakPercentage ) ),
	SendPropFloat( SENDINFO( m_flMinStreakTime ) ),
	SendPropFloat( SENDINFO( m_flMaxStreakTime ) ),
	SendPropFloat( SENDINFO( m_flMinStreakSpeedDampen ) ),
	SendPropFloat( SENDINFO( m_flMaxStreakSpeedDampen ) ),

	SendPropFloat( SENDINFO( m_flStartTime ) ),

	SendPropEHandle( SENDINFO( m_hPaintStream ) ),

	SendPropFloat( SENDINFO( m_flNoisyBlobPercentage ) ),
	SendPropFloat( SENDINFO( m_flPercentageSinceLastNoisyBlob ) )

END_SEND_TABLE()


BEGIN_DATADESC( CPaintSprayer )
	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_hPaintStream, FIELD_EHANDLE ),

	DEFINE_FIELD( m_nBlobRandomSeed, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_bSilent, FIELD_BOOLEAN, "Silent" ),
	DEFINE_KEYFIELD( m_bStartActive, FIELD_BOOLEAN, "start_active" ),
	DEFINE_KEYFIELD( m_bDrawOnly, FIELD_BOOLEAN, "DrawOnly" ),

	DEFINE_KEYFIELD( m_PaintPowerType, FIELD_INTEGER, "PaintType" ),
	DEFINE_KEYFIELD( m_nMaxBlobCount, FIELD_INTEGER, "maxblobcount" ),

	DEFINE_KEYFIELD( m_flBlobsPerSecond, FIELD_FLOAT, "blobs_per_second" ),
	DEFINE_KEYFIELD( m_flMinSpeed, FIELD_FLOAT, "min_speed" ),
	DEFINE_KEYFIELD( m_flMaxSpeed, FIELD_FLOAT, "max_speed" ),
	DEFINE_KEYFIELD( m_flBlobSpreadRadius, FIELD_FLOAT, "blob_spread_radius" ),
	DEFINE_KEYFIELD( m_flBlobSpreadAngle, FIELD_FLOAT, "blob_spread_angle" ),

	DEFINE_KEYFIELD( m_flStreakPercentage, FIELD_FLOAT, "blob_streak_percentage" ),
	DEFINE_KEYFIELD( m_flMinStreakTime, FIELD_FLOAT, "min_streak_time" ),
	DEFINE_KEYFIELD( m_flMaxStreakTime, FIELD_FLOAT, "max_streak_time" ),
	DEFINE_KEYFIELD( m_flMinStreakSpeedDampen, FIELD_FLOAT, "min_streak_speed_dampen" ),
	DEFINE_KEYFIELD( m_flMaxStreakSpeedDampen, FIELD_FLOAT, "max_streak_speed_dampen" ),

	DEFINE_KEYFIELD( m_nRenderMode, FIELD_INTEGER, "RenderMode" ),

	DEFINE_KEYFIELD( m_nAmbientSound, FIELD_INTEGER, "AmbientSound" ),

	DEFINE_KEYFIELD( m_strLightPositionName, FIELD_STRING, "light_position_name" ),
	DEFINE_FIELD( m_hLightPositionEntity, FIELD_EHANDLE ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Start", InputStart ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Stop", InputStop ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "ChangePaintType", InputChangePaintType ),

	DEFINE_THINKFUNC( PaintSprayerThink ),

	DEFINE_FIELD( m_flNoisyBlobPercentage, FIELD_FLOAT ),
	DEFINE_FIELD( m_flPercentageSinceLastNoisyBlob, FIELD_FLOAT ),

	DEFINE_SOUNDPATCH( m_pFlowSoundLoop )

END_DATADESC()

LINK_ENTITY_TO_CLASS( info_paint_sprayer, CPaintSprayer );

CPaintSprayer::CPaintSprayer( void )
		: m_flAccumulatedTime( 0.0f ),
		  m_bStartActive( false ),
		  m_bActive( false ),
		  m_flLastThinkTime( 0.0f ),
		  m_flStartTime( 0.0f ),
		  m_nAmbientSound( PAINT_SPRAYER_SOUND_NONE ),
		  m_nMaxBlobCount( 0 ),
		  m_pFlowSoundLoop( NULL ),
		  m_bDrawOnly( false ),
		  m_PaintPowerType( BOUNCE_POWER ), // bounce is default power in portal2.fgd
		  m_nRenderMode( BLOB_RENDER_BLOBULATOR )
{
}


CPaintSprayer::~CPaintSprayer( void )
{
	if( m_pFlowSoundLoop )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		controller.Shutdown( m_pFlowSoundLoop );
		controller.SoundDestroy( m_pFlowSoundLoop );
	}
}


void CPaintSprayer::Precache( void )
{
	// FIXME: Bring back for DLC2
	//PrecacheParticleSystem( "paint_splat_reflect_01" );
	PrecacheParticleSystem("paint_splat_bounce_01");
	PrecacheParticleSystem("paint_splat_stick_01");
	PrecacheParticleSystem("paint_splat_speed_01");
	PrecacheParticleSystem("paint_splat_erase_01");
	PrecacheParticleSystem("paint_splat_stick_01");

	PrecacheScriptSound( "Paintblob.Inception" );
	PrecacheScriptSound( "Paintblob.Impact" );
	PrecacheScriptSound( "Paintblob.ImpactDrip" );
	PrecacheScriptSound( "Paint.Drip" );
	PrecacheScriptSound( "Paint.MediumFlow" );
	PrecacheScriptSound( "Paint.HeavyFlow" );

	BaseClass::Precache();
}


void CPaintSprayer::Spawn( void )
{
	BaseClass::Spawn();

	Precache();

	SetThink( &CPaintSprayer::PaintSprayerThink );
	SetNextThink( TICK_NEVER_THINK );

	m_nBlobRandomSeed = RandomInt( -9999, 9999 );

	if( m_bStartActive )
	{
		StartPaintSpray();
	}

	if ( m_nMaxBlobCount == 0 )
	{
		// It if was zero, this may be the default value, the corresponding may have not been rebuilt. Let's put a reasonable value.
		m_nMaxBlobCount = 250;
	}

	m_hLightPositionEntity = gEntList.FindEntityByName( NULL, m_strLightPositionName );
	if ( m_hPaintStream == NULL )
	{
		CPaintStream *pPaintStream = (CPaintStream*)CreateEntityByName( "paint_stream" );
		if ( pPaintStream  )
		{
			Vector vLightPosition = ( m_hLightPositionEntity.Get() == NULL ) ? vec3_origin : m_hLightPositionEntity->GetAbsOrigin();
			pPaintStream->Init( vLightPosition, m_PaintPowerType, m_nRenderMode, m_nMaxBlobCount );

			DispatchSpawn( pPaintStream );

			m_hPaintStream = pPaintStream;
		}
	}

	m_flNoisyBlobPercentage = max_noisy_blobs_per_second.GetFloat() / m_flBlobsPerSecond;
}


void CPaintSprayer::Activate( void )
{
	BaseClass::Activate();

	// when you restore the level, curtime could be big, so we hit the max blobs allocation
	m_flLastThinkTime = gpGlobals->curtime;

	if( m_bStartActive )
	{
		m_bStartActive = false;
		StartPaintSpray();
	}
}


void CPaintSprayer::UpdateOnRemove( void )
{
	UTIL_Remove( m_hPaintStream );

	BaseClass::UpdateOnRemove();
}


int CPaintSprayer::Save( ISave &save )
{
	if ( !BaseClass::Save( save ) )
		return 0;

	save.StartBlock();
	{
		int count = m_hPaintStream->GetBlobsCount();
		save.WriteInt( &count );
		for ( int i=0; i<count; ++i )
		{
			CPaintBlob *pBlob = m_hPaintStream->GetBlob( i );
			save.WriteVector( pBlob->GetPosition() );
			save.WriteVector( pBlob->GetVelocity() );
			float flMaxStreakTime = pBlob->GetStreakTime();
			save.WriteFloat( &flMaxStreakTime );
			float flStreakSpeedDampenRate = pBlob->GetStreakSpeedDampenRate();
			save.WriteFloat( &flStreakSpeedDampenRate );
		}
	}
	save.EndBlock();

	return 1;
}


int CPaintSprayer::Restore( IRestore &restore )
{
	if ( !BaseClass::Restore( restore ) )
		return 0;

	Assert( m_hPaintStream != NULL );

	// temporary unpause so we can restore blobs
	blobs_paused.SetValue( false );

	restore.StartBlock();
	{
		int count = restore.ReadInt();
		for ( int i=0; i<count; ++i )
		{
			Vector vPosition;
			restore.ReadVector( &vPosition, 1 );
			Vector vVelocity;
			restore.ReadVector( &vVelocity, 1 );
			float flStreakTime;
			restore.ReadFloat( &flStreakTime );
			float flStreakSpeedDampen;
			restore.ReadFloat( &flStreakSpeedDampen );

			CPaintBlob *pBlob = PaintStreamManager.AllocatePaintBlob();
			if ( pBlob )
			{
				pBlob->Init( vPosition, vVelocity, m_PaintPowerType, flStreakTime, flStreakSpeedDampen, m_hPaintStream, m_bSilent, m_bDrawOnly );
				m_hPaintStream->AddPaintBlob( pBlob );
			}
		}
	}
	restore.EndBlock();

	blobs_paused.SetValue( true );

	return 1;
}


void CPaintSprayer::SetPaintPowerType( PaintPowerType paintType )
{
	m_PaintPowerType = paintType;
}


int CPaintSprayer::UpdateTransmitState( void )
{
	if( m_bSilent )
	{
		return SetTransmitState( FL_EDICT_DONTSEND );
	}

	return SetTransmitState( FL_EDICT_ALWAYS );
}

void CPaintSprayer::PaintSprayerThink( void )
{
	if( m_bActive )
	{
		float flDeltaTime = gpGlobals->curtime - m_flLastThinkTime;
		m_flLastThinkTime = gpGlobals->curtime;
		SprayPaint( flDeltaTime );

		SetNextThink( gpGlobals->curtime );
	}
}


void CPaintSprayer::InputStart( inputdata_t &inputData )
{
	StartPaintSpray();
}


void CPaintSprayer::InputStop( inputdata_t &inputData )
{
	StopPaintSpray();
}


void CPaintSprayer::InputChangePaintType( inputdata_t &inputData )
{
	SetPaintPowerType( static_cast<PaintPowerType>( inputData.value.Int() ) );
}


void CPaintSprayer::StartPaintSpray( void )
{
	if( m_bActive == false )
	{
		m_bActive = true;

		m_flAccumulatedTime = 1.0f / m_flBlobsPerSecond;

		if( m_bSilent == false )
		{
			char const* soundName = NULL;
			switch( m_nAmbientSound )
			{
				case PAINT_SPRAYER_SOUND_MEDIUM_FLOW:
					soundName = "Paint.MediumFlow";
					break;

				case PAINT_SPRAYER_SOUND_HEAVY_FLOW:
					soundName = "Paint.HeavyFlow";
					break;
			}

			CSoundParameters soundParams;
			if( soundName != NULL && GetParametersForSound( soundName, soundParams, NULL ) )
			{
				CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

				if( m_pFlowSoundLoop )
				{
					controller.Shutdown( m_pFlowSoundLoop );
					controller.SoundDestroy( m_pFlowSoundLoop );
				}

				CBroadcastRecipientFilter filter;
				EmitSound_t emitSoundParams( soundParams );
				emitSoundParams.m_pSoundName = soundName;
				emitSoundParams.m_pOrigin = &GetAbsOrigin();
				m_pFlowSoundLoop = controller.SoundCreate( filter, entindex(), emitSoundParams );
				controller.Play( m_pFlowSoundLoop, emitSoundParams.m_flVolume, emitSoundParams.m_nPitch );
			}
		}

		m_flStartTime = gpGlobals->curtime;
		m_flLastThinkTime = gpGlobals->curtime;
		SetNextThink( gpGlobals->curtime );
	}
}


void CPaintSprayer::StopPaintSpray( void )
{
	m_bActive = false;

	if( m_pFlowSoundLoop != NULL )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		controller.Shutdown( m_pFlowSoundLoop );
		controller.SoundDestroy( m_pFlowSoundLoop );
		m_pFlowSoundLoop = NULL;
	}

	SetNextThink( TICK_NEVER_THINK );
}
