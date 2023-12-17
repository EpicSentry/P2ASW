#include "cbase.h"
#include "c_ai_basenpc.h"
#include "dlight.h"
#include "functionproxy.h"
#include "imaterialproxydict.h"
#include "c_portal_player.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


class C_NPC_Personality_Core : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Personality_Core, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	void UpdateOnRemove();
	void ControlMouth(CStudioHdr* pStudioHdr);
	void OnDataChanged(DataUpdateType_t updateType);
	void ClientThink();

	float GetMouthAmount() { return m_flMouthAmount; }

private:
	float m_flMouthAmount;
	float m_flInvDim;
	bool m_bFlashlightEnabled;
	CUtlReference<CNewParticleEffect> m_pFlashlightEffect;
	dlight_t* m_pELight;
};


// FIXME: Setting this at 0.0 temporarily, but what is this true value??
// Setting it to 0.0 because wheatley's dimmer value is set to 0.0 by default.
float s_GLaDOS_flDimmer = 0.0;

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Personality_Core, DT_NPC_Personality_Core, CNPC_PersonalityCore)
RecvPropBool( RECVINFO ( m_bFlashlightEnabled) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS(npc_personality_core, C_NPC_Personality_Core);

// TODO: Find line placements for this class
class C_NPC_Wheatley_Boss : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Wheatley_Boss, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	void ControlMouth(CStudioHdr* pStudioHdr);
	void OnDataChanged(DataUpdateType_t updateType);

	float GetMouthAmount() { return m_flMouthAmount; }

private:
	float m_flMouthAmount;
	float m_flInvDim;
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Wheatley_Boss, DT_NPC_Wheatley_Boss, CNPC_Wheatley_Boss)
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS(npc_wheatley_boss, C_NPC_Wheatley_Boss);

// TODO: Optimize
float MouthDecay(float mouthopen, float& flInvDim, float flBaseLight)
{
	float v3 = 0.0;
	float v4 = 0.015625 * mouthopen;
	if ((float)(0.015625 * mouthopen) < 0.0)
	{
		v4 = 0.0;
		goto LABEL_7;
	}
	if (v4 > 1.0)
	{
		v4 = 1.0;
		v3 = 1.0;
		goto LABEL_7;
	}
	if (v4 < 0.2)
	{
		v4 = 0.0;
	LABEL_7:
		if (v3 <= flInvDim)
			goto LABEL_5;
	LABEL_8:
		flInvDim = v3;
		return fmaxf(v4, flBaseLight * (float)(1.0 - v3));
	}
	v3 = fminf(3.0 * v4, 1.0);
	if (v3 > flInvDim)
		goto LABEL_8;
LABEL_5:
	float v7 = expf(-0.61220264 * gpGlobals->frametime);
	v3 = v7 * flInvDim;
	flInvDim = v7 * flInvDim;
	return fmaxf(v4, flBaseLight * (float)(1.0 - v3));
}

void C_NPC_Personality_Core::UpdateOnRemove()
{
	if (m_pFlashlightEffect)
	{
		ParticleProp()->StopEmissionAndDestroyImmediately(m_pFlashlightEffect);
		m_pFlashlightEffect = NULL;
	}

	if (m_pELight)
	{
		m_pELight->die = gpGlobals->curtime;
		m_pELight = NULL;
	}
	BaseClass::UpdateOnRemove();
}

void C_NPC_Personality_Core::ControlMouth(CStudioHdr* pStudioHdr)
{
	BaseClass::ControlMouth(pStudioHdr);
	if (!MouthInfo().NeedsEnvelope())
		return;
	
	if (!pStudioHdr)
		return;

	m_flMouthAmount = MouthDecay( GetMouth()->mouthopen, m_flInvDim, 0.8f);
}

void C_NPC_Personality_Core::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	if (updateType == DATA_UPDATE_CREATED)
	{
		MouthInfo().ActivateEnvelope();
		m_flMouthAmount = 0.0f;
		m_flInvDim = 0.0f;
	}
}

void C_NPC_Personality_Core::ClientThink()
{
	if (m_bFlashlightEnabled)
	{
		QAngle angles;
		Vector vecOrigin;
		Vector vForward, vRight, vUp;
		Vector vecAbsEnd;
		CGameTrace tr;

		int iAttachmentPoint = LookupAttachment("eyes");
		GetAttachment(iAttachmentPoint, vecOrigin, angles);
		AngleVectors(angles, &vForward);
		
		vecAbsEnd = vecOrigin + vForward * 256.0;

		UTIL_TraceLine(vecOrigin, vecAbsEnd, MASK_SHOT_HULL & (~CONTENTS_GRATE), NULL, &tr);
	
		m_pFlashlightEffect = ParticleProp()->Create("flashlight_thirdperson", PATTACH_POINT_FOLLOW, "flashlightAttachment");
		if (m_pFlashlightEffect)
		{
			ParticleProp()->AddControlPoint(m_pFlashlightEffect, 0, this, PATTACH_CUSTOMORIGIN);
			ParticleProp()->AddControlPoint(m_pFlashlightEffect, 2, this, PATTACH_CUSTOMORIGIN);
			C_BasePlayer* localPlayer = C_BasePlayer::GetLocalPlayer();
			ParticleProp()->AddControlPoint(m_pFlashlightEffect, 3, localPlayer, PATTACH_EYES_FOLLOW);
			
			m_pFlashlightEffect->SetControlPoint(1, vecOrigin);
			m_pFlashlightEffect->SetControlPoint(2, tr.endpos);
			m_pFlashlightEffect->SetControlPointForwardVector(1, vForward);
			
			m_pELight->die = gpGlobals->curtime + 1e10;
			m_pELight->flags = 1;
			m_pELight->radius = 64.0f;
			m_pELight->style = 0;
			m_pELight->m_pExclusiveLightReceiver;
			m_pELight->color.r = 31;
			m_pELight->color.g = 31;
			m_pELight->color.b = 1;
			m_pELight->color.exponent = 0;
			m_pELight->origin = vForward * 2.0 + vecOrigin;
			render->TouchLight(m_pELight);
		}
	}
}

void C_NPC_Wheatley_Boss::ControlMouth(CStudioHdr* pStudioHdr)
{
	BaseClass::ControlMouth(pStudioHdr);
	if (!MouthInfo().NeedsEnvelope())
		return;

	if (!pStudioHdr)
		return;

	m_flMouthAmount = MouthDecay(GetMouth()->mouthopen, m_flInvDim, 0.8f);
}

void C_NPC_Wheatley_Boss::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	if (updateType == DATA_UPDATE_CREATED)
	{
		MouthInfo().ActivateEnvelope();
		m_flMouthAmount = 0.0;
		m_flInvDim = 0.0;
	}
}

extern C_BaseAnimating *GetGLaDOSActor( void );

class CLightedMouthProxy : public CResultProxy
{
public:
	virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
    virtual void OnBind( void *pC_BaseEntity );
};

bool CLightedMouthProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	return CResultProxy::Init( pMaterial, pKeyValues );
}

void CLightedMouthProxy::OnBind( void *pC_BaseEntity )
{
	C_AI_BaseNPC *pActor = NULL;

	float flFlashResult = 1.0;

	if (pC_BaseEntity)
	{
		C_BaseEntity *pEntity = BindArgToEntity( pC_BaseEntity );
		if ( pEntity )
		{
			if ( ( pActor = dynamic_cast<C_NPC_Personality_Core*>( pEntity ) ) != NULL )
			{
				flFlashResult = ((C_NPC_Personality_Core*)pEntity)->GetMouthAmount();
			}
			else if ( ( pActor = dynamic_cast<C_NPC_Wheatley_Boss*>( pEntity ) ) != NULL )
			{
				flFlashResult = ((C_NPC_Wheatley_Boss*)pEntity)->GetMouthAmount();
			}
			else
			{
				C_BaseAnimating *GLaDOSActor = GetGLaDOSActor();
				if ( GLaDOSActor )
				{
					C_BaseAnimating *pAnimating = GLaDOSActor->GetBaseAnimating();
					if ( pAnimating )
					{
						float flBase = 0.0;
						C_Portal_Player *pLocalPlayer = (C_Portal_Player *)C_BasePlayer::GetLocalPlayer();
						if ( pLocalPlayer && pLocalPlayer->IsPotatosOn() )
							flBase = 0.2;

						float flOpenAmount = pAnimating->GetMouth()->mouthopen;
						flFlashResult = MouthDecay( flOpenAmount, s_GLaDOS_flDimmer, flBase );
					}
				}
			}
		}
		SetFloatResult( flFlashResult );
	}
}

EXPOSE_MATERIAL_PROXY( CLightedMouthProxy, LightedMouth );
