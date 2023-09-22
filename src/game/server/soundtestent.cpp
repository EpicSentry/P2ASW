#include "cbase.h"
#include "entitylist.h"
#include "gameinterface.h"
#include "soundent.h"

// Replace "your_sound_file.wav" with the path to your WAV file
const char* g_szSoundFile = "vo/glados/a2_triple_laser01.mp3";

class CSoundTestEnt : public CBaseEntity
{
	DECLARE_CLASS(CSoundTestEnt, CBaseEntity);
public:
	void Spawn() override;
	void Precache() override;
	void Think() override;
};

LINK_ENTITY_TO_CLASS(sound_test_ent, CSoundTestEnt);

void CSoundTestEnt::Spawn()
{
	Precache();
	SetThink(&CSoundTestEnt::Think);
	SetNextThink(gpGlobals->curtime + 5.0f);
}

void CSoundTestEnt::Precache()
{
	PrecacheScriptSound(g_szSoundFile);
}

void CSoundTestEnt::Think()
{
	// Play the sound directly using EmitSound with a high volume
	EmitSound(g_szSoundFile, ATTN_TO_SNDLVL(100.0f));

	// Print a message to the console
	Msg("Sound test played: %s\n", g_szSoundFile);

	// Schedule the next think
	SetNextThink(gpGlobals->curtime + 5.0f);
}
