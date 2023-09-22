//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef PAINT_SPRAYER_H
#define PAINT_SPRAYER_H

#include "paint_stream.h"
#include "paint/paint_sprayer_shared.h"

enum PaintSprayerAmbientSound
{
	PAINT_SPRAYER_SOUND_NONE,
	PAINT_SPRAYER_SOUND_DRIP,
	PAINT_SPRAYER_SOUND_MEDIUM_FLOW,
	PAINT_SPRAYER_SOUND_HEAVY_FLOW
};

class CPaintSprayer : public CBaseEntity
{
public:
	DECLARE_CLASS( CPaintSprayer, CBaseEntity );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPaintSprayer( void );
	~CPaintSprayer( void );

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void Activate( void );
	virtual int UpdateTransmitState( void );
	virtual void UpdateOnRemove( void );

	virtual int Save( ISave &save );
	virtual int Restore( IRestore &restore );

	void StartPaintSpray( void );
	void StopPaintSpray( void );

	void InputStart( inputdata_t &inputData );
	void InputStop( inputdata_t &inputData );
	void InputChangePaintType( inputdata_t &inputData );

	void PaintSprayerThink( void );

	void SetPaintPowerType( PaintPowerType paintType );

private:

	void SprayPaint( float flDeltaTime );

	bool m_bStartActive;
	float m_flLastThinkTime;
	float m_flAccumulatedTime;
	BlobRenderMode_t m_nRenderMode;
	int m_nAmbientSound;

	CNetworkVar( bool, m_bActive );
	CNetworkVar( bool, m_bSilent );
	CNetworkVar( bool, m_bDrawOnly );

	CNetworkVar( int, m_nBlobRandomSeed );
	CNetworkVar( PaintPowerType, m_PaintPowerType );
	CNetworkVar( unsigned int, m_nMaxBlobCount );

	CNetworkVar( float, m_flBlobsPerSecond );
	CNetworkVar( float, m_flBlobSpreadRadius );
	CNetworkVar( float, m_flBlobSpreadAngle );
	CNetworkVar( float, m_flMinSpeed );
	CNetworkVar( float, m_flMaxSpeed );

	CNetworkVar( float, m_flNoisyBlobPercentage );
	CNetworkVar( float, m_flPercentageSinceLastNoisyBlob );

	//Streaking of the blobs
	CNetworkVar( float, m_flStreakPercentage );
	CNetworkVar( float, m_flMinStreakTime );
	CNetworkVar( float, m_flMaxStreakTime );
	CNetworkVar( float, m_flMinStreakSpeedDampen );
	CNetworkVar( float, m_flMaxStreakSpeedDampen );

	CNetworkVar( float, m_flStartTime );

	CNetworkHandle( CPaintStream, m_hPaintStream );

	CSoundPatch* m_pFlowSoundLoop;

	// light position
	string_t m_strLightPositionName;
	EHANDLE m_hLightPositionEntity;
};

#endif //PAINT_SPRAYER_H

