#include "cbase.h"
#include "c_ai_basenpc.h"
#include "dlight.h"
#include "tier0/memdbgon.h"

class C_NPC_Personality_Core : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Personality_Core, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	void ClientThink();
private:
	bool m_bFlashlightEnabled;
	CUtlReference<CNewParticleEffect> m_pFlashlightEffect;
	dlight_t* m_pELight;
};

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
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Wheatley_Boss, DT_NPC_Wheatley_Boss, CNPC_Wheatley_Boss)
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS(npc_wheatley_boss, C_NPC_Wheatley_Boss);

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
