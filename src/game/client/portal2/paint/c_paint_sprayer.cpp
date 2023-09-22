//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#include "cbase.h"

#include "c_paint_sprayer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_PaintSprayer, DT_PaintSprayer, CPaintSprayer )

	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropInt( RECVINFO_NAME( m_hNetworkMoveParent, moveparent ), 0, RecvProxy_IntToMoveParent ),
	RecvPropQAngles( RECVINFO_NAME( m_angNetworkAngles, m_angRotation ) ),

	RecvPropBool( RECVINFO( m_bActive ) ),
	RecvPropBool( RECVINFO( m_bSilent ) ),
	RecvPropBool( RECVINFO( m_bDrawOnly ) ),
	
	RecvPropInt( RECVINFO( m_nBlobRandomSeed ) ),
	RecvPropInt( RECVINFO( m_PaintPowerType ) ),
	RecvPropInt( RECVINFO( m_nMaxBlobCount ) ),

	RecvPropFloat( RECVINFO( m_flBlobsPerSecond ) ),
	RecvPropFloat( RECVINFO( m_flMinSpeed ) ),
	RecvPropFloat( RECVINFO( m_flMaxSpeed ) ),
	RecvPropFloat( RECVINFO( m_flBlobSpreadRadius ) ),
	RecvPropFloat( RECVINFO( m_flBlobSpreadAngle ) ),

	RecvPropFloat( RECVINFO( m_flStreakPercentage ) ),
	RecvPropFloat( RECVINFO( m_flMinStreakTime ) ),
	RecvPropFloat( RECVINFO( m_flMaxStreakTime ) ),
	RecvPropFloat( RECVINFO( m_flMinStreakSpeedDampen ) ),
	RecvPropFloat( RECVINFO( m_flMaxStreakSpeedDampen ) ),

	RecvPropFloat( RECVINFO( m_flStartTime ) ),

	RecvPropEHandle( RECVINFO( m_hPaintStream ) ),

	RecvPropFloat( RECVINFO( m_flNoisyBlobPercentage ) ),
	RecvPropFloat( RECVINFO( m_flPercentageSinceLastNoisyBlob ) )

END_RECV_TABLE();


C_PaintSprayer::C_PaintSprayer( void )
		: m_flAccumulatedTime( 0.0f ),
		  m_flStartTime( 0.0f )
{
}


C_PaintSprayer::~C_PaintSprayer( void )
{
}


void C_PaintSprayer::Precache( void )
{
	PrecacheParticleSystem( "paint_splat_bounce_01" );
	// FIXME: Turn on for DLC2
	//PrecacheParticleSystem( "paint_splat_reflect_01" );
	PrecacheParticleSystem( "paint_splat_speed_01" );
	PrecacheParticleSystem( "paint_splat_erase_01" );

	BaseClass::Precache();
}


void C_PaintSprayer::Spawn( void )
{
	Precache();

	BaseClass::Spawn();
}

void C_PaintSprayer::PreDataUpdate( DataUpdateType_t updateType )
{
	m_bOldActive = m_bActive;

	BaseClass::PreDataUpdate( updateType );
}


void C_PaintSprayer::PostDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PostDataUpdate( updateType );

	if( updateType == DATA_UPDATE_CREATED )
	{
		//Start spraying if the sprayer is active
		if( m_bActive )
		{
			m_flAccumulatedTime = 1.0f / m_flBlobsPerSecond;

			m_flLastThinkTime = m_flStartTime;
			SetNextClientThink( gpGlobals->curtime );
		}
	}
	else
	{
		//If the active state of the sprayer has changed
		if( m_bOldActive != m_bActive )
		{
			if( m_bActive )
			{
				m_flAccumulatedTime = 1.0f / m_flBlobsPerSecond;

				m_flLastThinkTime = m_flStartTime;
				SetNextClientThink( gpGlobals->curtime );
			}
			else
			{
				SetNextClientThink( CLIENT_THINK_NEVER );
			}
		}
	}
}


void C_PaintSprayer::ClientThink( void )
{
	if( m_bActive )
	{
		float flDeltaTime = gpGlobals->curtime - m_flLastThinkTime;
		m_flLastThinkTime = gpGlobals->curtime;
		SprayPaint( flDeltaTime );

		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}
