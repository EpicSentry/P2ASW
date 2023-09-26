
m_nShakeRattleCount <- 0

function PlayShakeRattleSound()
{
	local nSoundNum = 1

	if ( m_nShakeRattleCount == 1 )
	{
		nSoundNum = 2
	}
	else if ( m_nShakeRattleCount == 2 )
	{
		nSoundNum = 3
	}
	else if ( m_nShakeRattleCount == 3 )
	{
		nSoundNum = 4
	}
	else if ( m_nShakeRattleCount > 3 )
	{
		nSoundNum = RandomInt( 1, 4 ) 
	}

	EntFire( "rattles_0" + nSoundNum, "PlaySound", "",			0.0 )

	m_nShakeRattleCount++
}

function StartExploreTimer()
{
	EntFire( "crash-speedmod", "Kill", "",							0 )
	EntFire( "horn_short", "PlaySound", "",						0.9 )
	EntFire( "prop_arms_view", "SetAnimation", "wake_up",			1.5 )

	EntFire( "snd_chair_slide", "PlaySound", "",						6 )

	EntFire( "timer_clock", "Enable", "",				5 )

	EntFire( "horn_short", "PlaySound", "",						5.5 )
	EntFire( "crossingDop", "PlaySound", "",			7.0 )

	//cubegun
	EntFire( "snd_cubegun", "PlaySound", "",				7.0 )
	EntFire( "timer_cubegun_sound", "Enable", "",			7.2 )
	EntFire( "timer_cubegun_sound", "Disable", "",			14.0 )
	EntFire( "timer_cubegun", "Enable", "",					15.0 )

	EntFire( "hint_zoom", "ShowHint", "",			16.0 )
	EntFire( "hint_zoom", "EndHint", "",			28.0 )

	//Radio chatter
	EntFire( "radio_VO_S1_01", "PlaySound", "",					15 )
	EntFire( "radio_VO_S2_01", "PlaySound", "",					19 )
	EntFire( "radio_VO_S1_02", "PlaySound", "",					23 )
	
	// add EntFire's here for happenings and sounds
	local flShutterDelay = 35

	EntFire( "relay_turnoff_swinglight", "Trigger", "",			flShutterDelay )
	EntFire( "particle_sparks_1", "Start", "",					flShutterDelay )
	//EntFire( "snd_hang_light_spark", "PlaySound", "",			flShutterDelay )
	EntFire( "particle_sparks_1", "Stop", "",					flShutterDelay + 0.5 )

	// sound of the shutters opening
	EntFire( "train_shutters", "PlaySound", "",			flShutterDelay )
	EntFire( "prop_dynamic_items", "SetAnimation", "first_hit",			flShutterDelay )
	EntFire( "prop_dynamic_items_gun", "SetAnimation", "first_hit",			flShutterDelay )
	EntFire( "prop_shutter_1", "SetAnimation", "first_hit",			flShutterDelay )
	EntFire( "prop_shutter_2", "SetAnimation", "first_hit",			flShutterDelay + 0.1 )

	EntFire( "bulb_break", "PlaySound", "",			flShutterDelay )
	EntFire( "snd_ding_F", "PlaySound", "",			flShutterDelay + 0.5 )
	EntFire( "snd_ding_B", "PlaySound", "",			flShutterDelay + 0.5 )

	EntFire( "radio_VO_S2_03", "PlaySound", "",					flShutterDelay + 1.6 )
	EntFire( "radio_VO_static3", "PlaySound", "",				flShutterDelay + 2.0 )
	EntFire( "radio_VO_S2_04", "PlaySound", "",					flShutterDelay+ 6.1 )
	EntFire( "radio_VO_S1_03", "PlaySound", "",					flShutterDelay + 6.8 )
	EntFire( "radio_VO_S2_05", "PlaySound", "",					flShutterDelay + 9.8 )

	EntFire( "particle_sparks_1", "Start", "",					flShutterDelay + 2 )
	EntFire( "snd_hang_light_spark", "PlaySound", "",			flShutterDelay + 2 )
	EntFire( "particle_sparks_1", "Stop", "",					flShutterDelay + 2.5 )

	EntFire( "particle_sparks_1", "Start", "",					flShutterDelay + 6 )
	EntFire( "snd_hang_light_spark", "PlaySound", "",			flShutterDelay + 6 )
	EntFire( "particle_sparks_1", "Stop", "",					flShutterDelay + 6.5 )

	EntFire( "timer_cubegun",		"Disable", "",			flShutterDelay + 14.0 )
	EntFire( "timer_cubegun_sound", "Enable", "",			flShutterDelay + 14.2 )
	EntFire( "snd_cubegun", "PlaySound", "",				flShutterDelay + 14.0 )
	EntFire( "timer_cubegun_sound",		"Disable", "",		flShutterDelay + 22.0 )
	EntFire( "timer_cubegun", "Enable", "",					flShutterDelay + 23.0 )

	EntFire( "@script_br_trainride", "RunScriptCode", "StartCrashTimer()", flShutterDelay + 22 )
}


function StartCrashTimer()
{
	EntFire( "radio_VO_S2_02", "PlaySound", "",		0.0 )
	EntFire( "radio_VO_S1_06", "PlaySound", "",		0.9 )
	EntFire( "radio_VO_static1", "PlaySound", "",	1.4 )

	EntFire( "radio_VO_S1_07", "PlaySound", "",		2.0 )
	EntFire( "horn_long", "PlaySound", "",			2.9 )
	EntFire( "radio_VO_S2_08", "PlaySound", "",		3.0 )
	
	//Boy yelling doppler
	EntFire( "dopplerVO_Front", "PlaySound", "",		3.7 )
	EntFire( "dopplerVO_Back", "PlaySound", "",			3.7 )

	EntFire( "radio_VO_static2", "PlaySound", "",	4.0 )

	EntFire( "radio_VO_S2_09", "PlaySound", "",		4.4 )
	EntFire( "horn_short", "PlaySound", "",			4.6 )
	EntFire( "radio_VO_S2_10", "PlaySound", "",		4.9 )

	EntFire( "radio_VO_8a", "PlaySound", "",		5.3 )

	// 5 second delay on everything after this
	local nCrashDelay = 6.0

	EntFire( "particle_train_window_break", "Start", "",		nCrashDelay + 0.0 )
	EntFire( "breakable_window_1", "Break", "",					nCrashDelay + 0.01 )
	
	EntFire( "snd_crash_temp", "PlaySound", "",		nCrashDelay + 0.0 )
	EntFire( "snd_crash_temp2", "PlaySound", "",		nCrashDelay + 0.5 )
	EntFire( "snd_metal_bend", "PlaySound", "",		nCrashDelay + 0.0 )
	
	EntFire( "timer_shake_view_light", "Kill", "",		nCrashDelay + 0.0 )
	
	EntFire( "particle_fire_jet_1", "Start", "",				nCrashDelay + 0.0 )

	EntFire( "particle_paper1", "Start", "",					nCrashDelay + 0.0 )

	EntFire( "viewpunch_forward", "viewpunch", "",				nCrashDelay + 0.0 )
	EntFire( "shake_crash_forward", "StartShake", "",			nCrashDelay + 0.0 )
	EntFire( "push_crash_forward", "Enable", "",				nCrashDelay + 0.0 )
	EntFire( "particle_dust_crash_forward", "Start", "",		nCrashDelay + 0.0 )

	EntFire( "particle_spark_electric_1", "Start", "",			nCrashDelay + 0.0 )

	EntFire( "scape_train_1",					"Disable", "",	nCrashDelay + 0.0 )
	EntFire( "scape_train_knocked_out",			"Enable", "",	nCrashDelay + 0.1 )
	EntFire( "snd_ding_F",						"StopSound", "",	nCrashDelay + 0.5 )
	EntFire( "snd_ding_B",						"StopSound", "",	nCrashDelay + 0.5 )

	// crash back
	EntFire( "relay_view_crash", "Trigger", "",					nCrashDelay + 0.7 )
	EntFire( "fade_crash_forward", "Fade", "",					nCrashDelay + 0.8 )

	EntFire( "timescale", "SetTimescaleBlendTime", "0.5",			nCrashDelay + 0.65 )
	EntFire( "timescale", "SetDesiredTimescale", "0.25",			nCrashDelay + 0.7 )

	local flDelayToBackCrash = 0.6
	//-----------------------------------
	EntFire( "light_fireball", "TurnOn", "",				nCrashDelay + flDelayToBackCrash )
	// kill the telephone poles
	EntFire( "poles", "Kill", "",		nCrashDelay + flDelayToBackCrash  )
	EntFire( "poles1", "Kill", "",		nCrashDelay + flDelayToBackCrash  )
	EntFire( "poles2", "Kill", "",		nCrashDelay + flDelayToBackCrash  )
	EntFire( "rope_train_light_*", "Kill", "",		nCrashDelay + flDelayToBackCrash  )
	
	EntFire( "brush_train_wall_1", "DisableReceivingFlashlight", "",		nCrashDelay + flDelayToBackCrash  )
	EntFire( "brush_train_wall_1", "Disable", "",							nCrashDelay + flDelayToBackCrash + 0.1  )
	EntFire( "brush_trainwall_broken", "Enable", "",						nCrashDelay + flDelayToBackCrash  )
	EntFire( "brush_trainwall_broken", "EnableReceivingFlashlight", "",		nCrashDelay + flDelayToBackCrash + 0.1 )
	
	EntFire( "prop_dynamic_items", "SetAnimation", "crash",			nCrashDelay + flDelayToBackCrash )
	EntFire( "prop_dynamic_items_gun", "SetAnimation", "crash",		nCrashDelay + flDelayToBackCrash )
	EntFire( "prop_shutter_1", "SetAnimation", "crash",				nCrashDelay + flDelayToBackCrash )
	EntFire( "prop_shutter_2", "SetAnimation", "crash",				nCrashDelay + flDelayToBackCrash )
	EntFire( "prop_traincar_ride", "SetAnimation", "crash",			nCrashDelay + flDelayToBackCrash + 0.4  )

	EntFire( "@relay_traincrashing_HDR", "Trigger", "",				nCrashDelay + flDelayToBackCrash )

	EntFire( "train_tree", "StartForward", "",			nCrashDelay + flDelayToBackCrash + 0.2  )

	EntFire( "push_crash_forward", "Disable", "",				nCrashDelay + flDelayToBackCrash )
	EntFire( "light_pole3", "TurnOff", "",						nCrashDelay + flDelayToBackCrash )
	EntFire( "light_pole4", "TurnOff", "",						nCrashDelay + flDelayToBackCrash )
	EntFire( "light_pole5", "TurnOff", "",						nCrashDelay + flDelayToBackCrash )
	EntFire( "light_train_explosion", "TurnOn", "",				nCrashDelay + flDelayToBackCrash + 0.1 )

	EntFire( "particle_glass_bulb_front", "Start", "",			nCrashDelay + flDelayToBackCrash + 0.3 )
	EntFire( "particle_spark_electric_2", "Start", "",			nCrashDelay + flDelayToBackCrash + 0.2 )

	EntFire( "particle_fire_jet_3", "Start", "",				nCrashDelay + flDelayToBackCrash + 0.2 )

	EntFire( "viewpunch_backward", "viewpunch", "",				nCrashDelay + flDelayToBackCrash + 0.2 )
	EntFire( "push_crash_backward", "Enable", "",				nCrashDelay + flDelayToBackCrash + 0.2 )
	EntFire( "particle_sparks_1", "Stop", "",					nCrashDelay + flDelayToBackCrash + 0.2 )
	EntFire( "relay_view_crash_back", "Trigger", "",			nCrashDelay + flDelayToBackCrash + 0.3 )

	EntFire( "particle_sparks_crash_1", "Start", "",			nCrashDelay + flDelayToBackCrash + 0.7 )

	EntFire( "particle_dust_crash_backward", "Start", "",		nCrashDelay + flDelayToBackCrash + 0.7 )
	EntFire( "particle_sparks_1", "Start", "",					nCrashDelay + flDelayToBackCrash + 0.7 )
	EntFire( "particle_paper", "Start", "",						nCrashDelay + flDelayToBackCrash + 0.7 )

	EntFire( "push_crash_backward", "Disable", "",				nCrashDelay + flDelayToBackCrash + 0.7 )

	EntFire( "particle_fire_jet_1", "Stop", "",					nCrashDelay + flDelayToBackCrash + 1.2 )
	EntFire( "particle_fire_jet_1", "Start", "",				nCrashDelay + flDelayToBackCrash + 1.4 )

	// bulb breaks
	EntFire( "socket_light_cage_4", "Break", "",				nCrashDelay + flDelayToBackCrash + 0.9 )
	EntFire( "prop_light_cage_4", "Skin", "0",					nCrashDelay + flDelayToBackCrash + 0.9 )
	EntFire( "spr_light_cage_4", "Kill", "",					nCrashDelay + flDelayToBackCrash + 0.9 )
	EntFire( "particle_glass_bulb_4", "Start", "",				nCrashDelay + flDelayToBackCrash + 0.9 )

	// bulb breaks 2
	EntFire( "socket_light_cage_3", "Break", "",				nCrashDelay + flDelayToBackCrash + 1.2 )
	EntFire( "prop_light_cage_3", "Skin", "0",					nCrashDelay + flDelayToBackCrash + 1.2 )
	EntFire( "spr_light_cage_3", "Kill", "",					nCrashDelay + flDelayToBackCrash + 1.2 )
	EntFire( "particle_glass_bulb_3", "Start", "",				nCrashDelay + flDelayToBackCrash + 1.2 )

	EntFire( "particle_spark_electric_3", "Start", "",			nCrashDelay + flDelayToBackCrash + 1.5 )

	EntFire( "particle_sparks_crash_2", "Start", "",				nCrashDelay + flDelayToBackCrash + 1.7 )

	// player gets knocked out
	// THUMP SOUND GOES HERE
	EntFire( "fade_blackout", "Fade", "",						nCrashDelay + flDelayToBackCrash + 1.8 )

	EntFire( "snd_crash_hit_1", "PlaySound", "",				nCrashDelay + flDelayToBackCrash + 1.4)
	EntFire( "snd_crash_hit_2", "PlaySound", "",				nCrashDelay + flDelayToBackCrash + 2.7)
	EntFire( "snd_crash_hit_3", "PlaySound", "",				nCrashDelay + flDelayToBackCrash + 3.5)
	EntFire( "snd_crash_hit_1", "PlaySound", "",				nCrashDelay + flDelayToBackCrash + 4.4)

	EntFire( "shake_crash_forward", "StopShake", "",			nCrashDelay + flDelayToBackCrash + 2.2 )

	EntFire( "timescale", "SetTimescaleBlendTime", "11",		nCrashDelay + flDelayToBackCrash + 2.4 )
	EntFire( "timescale", "SetDesiredTimescale", "1",			nCrashDelay + flDelayToBackCrash + 2.5  )

	EntFire( "particle_fire_jet_1", "Stop", "",					nCrashDelay + flDelayToBackCrash + 4.4 )

	EntFire( "train_parent", "Stop", "",						nCrashDelay + flDelayToBackCrash + 8 )
	
	EntFire( "timer_clock", "Disable", "",						nCrashDelay + flDelayToBackCrash + 8 )
	EntFire( "timer_cubegun", "Disable", "",					nCrashDelay + flDelayToBackCrash + 8 )
	EntFire( "timer_cubegun_sound", "Disable", "",				nCrashDelay + flDelayToBackCrash + 8 )
	EntFire( "timer_push", "Disable", "",						nCrashDelay + flDelayToBackCrash + 8 )

	// 
	// hand-off to traincrashed map
	EntFire( "@relay_Intro_setup_view2", "Trigger", "",					nCrashDelay + flDelayToBackCrash + 6.75 )
	EntFire( "@relay_traincrashed_HDR", "Trigger", "",					nCrashDelay + flDelayToBackCrash + 6.75 )
}

function ShowEndSteamURL()
{
	if ( ScriptSteamShowURL( "http://www.super8-movie.com/index6.html" ) )
	{
		EntFire( "command", "command", "disconnect",					0.1 );
	}
	else
	{
		ScriptShowHudMessageAll( "www.super8-movie.com",			10 );
		EntFire( "command", "command", "disconnect",					10 );
	}
}

function BRThink()
{
	//printl( "=== DEV " + GetDeveloperLevel() + " ===  BR think" )
}

