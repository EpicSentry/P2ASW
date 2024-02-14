#include "cbase.h"
#include "beam_shared.h"
#include "c_ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_NPC_Hover_Turret : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Hover_Turret, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	void Spawn();
	void ClientThink();
	bool IsLaserOn() { return m_pBeam != NULL; }
	void LaserOff();
	void LaserOn();
	float LaserEndPointSize();

private:
	C_Beam* m_pBeam;
	int m_iLastLaserState;
	int m_iLaserState;
	int m_sLaserHaloSprite;
	float m_fPulseOffset;
	float m_flLastUpdateTime;
	float m_bBeamFlickerOff;
	float m_fBeamFlickerTime;
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Hover_Turret, DT_NPC_HoverTurret, CNPC_HoverTurret)

	RecvPropInt(RECVINFO(m_iLaserState)),
	RecvPropInt(RECVINFO(m_sLaserHaloSprite)),

END_RECV_TABLE()

void C_NPC_Hover_Turret::Spawn()
{
	SetThink( &C_NPC_Hover_Turret::ClientThink );
	m_pBeam = NULL;
	m_fPulseOffset = RandomFloat(0.0f, 6.2831855f);
	m_bBeamFlickerOff = 0.0f;
	m_fBeamFlickerTime = 0.0f;
	C_BaseFlex::Spawn();
}

void C_NPC_Hover_Turret::ClientThink()
{
	if (m_iLaserState <= 0)
		LaserOff();
	else
		LaserOn();
}

void C_NPC_Hover_Turret::LaserOff()
{
	if (m_pBeam)
		AddEffects(EF_NODRAW);
}

void C_NPC_Hover_Turret::LaserOn()
{
	if ( !IsBoneAccessAllowed() )
	{
		LaserOff();
		return;
	}

	Vector vecMuzzleDir;
	QAngle angMuzzleDir;
	GetAttachment( 2, vecMuzzleDir, angMuzzleDir );

	Vector vecEye;
	QAngle angEyeDir;
	GetAttachment( 1, vecEye, angEyeDir );
	AngleVectors(angEyeDir, &vecMuzzleDir);

	if (m_pBeam)
	{
		m_pBeam->UpdateVisibility();
	}
	else
	{
		if (m_iLaserState == 1)
		{
			m_pBeam = CBeam::BeamCreate("effects/bluelaser1.vmt", 1.0f);
			m_pBeam->SetColor(100, 100, 255);
			m_pBeam->SetHaloTexture(m_sLaserHaloSprite);
			m_pBeam->SetHaloScale(1.5f);
		}
		else if (m_iLaserState == 2)
		{
			m_pBeam = CBeam::BeamCreate("effects/bluelaser1.vmt", 12.0f);
			m_pBeam->Activate();
			m_pBeam->SetWidth(16.0f);
			m_pBeam->SetEndWidth(16.0f);
		}
	}

	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init(vecMuzzleDir, vecMuzzleDir + vEndPoint * 8192.0f);

	CTraceFilterSkipClassname traceFilter(this, "prop_energy_ball", COLLISION_GROUP_NONE);

	if (UTIL_Portal_TraceRay_Beam(rayPath, MASK_SHOT, &traceFilter, &fEndFraction))
		vEndPoint = (vecMuzzleDir * 8192.0f) + vecMuzzleDir;
	else
		vEndPoint = (vecMuzzleDir * 8192.0f) * fEndFraction + vecMuzzleDir;

	m_pBeam->PointsInit(vEndPoint, vecMuzzleDir);

	m_pBeam->SetHaloScale(LaserEndPointSize());
}

float C_NPC_Hover_Turret::LaserEndPointSize()
{
	return ((MAX(0.0f, sinf(gpGlobals->curtime * M_PI + m_fPulseOffset))) * 4.0f + 3.0f) * (IsX360() ? (3.0f) : (1.5f));
}
