#include "cbase.h"
#include "c_ai_basenpc.h"
#include "tier0/memdbgon.h"

class C_NPC_Personality_Core : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Personality_Core, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();
private:
	bool m_bFlashlightEnabled;
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Personality_Core, DT_NPC_Personality_Core, CNPC_PersonalityCore)
RecvPropBool( RECVINFO ( m_bFlashlightEnabled) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS(npc_personality_core, C_NPC_Personality_Core);
