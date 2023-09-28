#include "cbase.h"
#include "c_ai_basenpc.h"
#include "tier0/memdbgon.h"

class C_NPC_Wheatley_Boss : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Wheatley_Boss, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Wheatley_Boss, DT_NPC_Wheatley_Boss, CNPC_Wheatley_Boss)
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS(npc_wheatley_boss, C_NPC_Wheatley_Boss);
