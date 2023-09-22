//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//
//=============================================================================//
#ifndef C_PAINT_SPRAYER_H
#define C_PAINT_SPRAYER_H

#include "c_paint_stream.h"

class C_PaintSprayer : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_PaintSprayer, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	C_PaintSprayer( void );
	~C_PaintSprayer( void );

	virtual void Precache( void );
	virtual void Spawn( void );

	virtual void PreDataUpdate( DataUpdateType_t updateType );
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void ClientThink( void );

private:

	void SprayPaint( float flDeltaTime );

	bool m_bOldActive;
	float m_flLastThinkTime;
	float m_flAccumulatedTime;
	int m_nBlobRandomSeed;

	//Server controlled variables
	bool m_bActive;
	bool m_bSilent;
	bool m_bDrawOnly;

	unsigned int m_nMaxBlobCount;
	int m_PaintPowerType;

	float m_flBlobsPerSecond;
	float m_flBlobSpreadRadius;
	float m_flBlobSpreadAngle;
	float m_flMinSpeed;
	float m_flMaxSpeed;

	float m_flNoisyBlobPercentage;
	float m_flPercentageSinceLastNoisyBlob;

	float m_flStreakPercentage;
	float m_flMinStreakTime;
	float m_flMaxStreakTime;
	float m_flMinStreakSpeedDampen;
	float m_flMaxStreakSpeedDampen;

	float m_flStartTime;

	CNetworkHandle( C_PaintStream, m_hPaintStream );
};

typedef C_PaintSprayer CPaintSprayer;

#endif //PAINT_SPRAYER_H
