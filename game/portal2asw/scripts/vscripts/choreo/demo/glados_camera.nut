//pitchShifting <- false

function StartPincerSequence()
{
//	EntFire("camera_ghostAnim_2","SetParent", "ghostAnim", 45.00)
//	EntFire("camera_ghostAnim_2","SetParentAttachment", "attach_1", 45.10)	
	EntFire("relay_wakeup","Trigger", "", 44.40)

	EntFire("claw_clang_sound","PlaySound", "", 46.60)

	EntFire("camera_1","Enable", "", 46.60)

	EntFire("trigger_weapon_strip", "Enable", "", 47.0)

	EntFire("camera_1","Disable", "", 47.80)
	EntFire("camera_ghostAnim_2","Enable", "", 47.80)

	
	EntFire("gun_shooter", "Shoot", "", 50.40)
	
	EntFire("relay_incinerator_open","Trigger", "", 73.0)
	
	EntFire("incinerator_pincer_teleport_to_end_relay","Trigger", "", 75.5)
	EntFire("incinerator_pincer_raise","Trigger", "", 76.0)
}

function OldPincerSequence()
{
// First we start looking at glados
	EntFire("incinerator_pincer_lower","Trigger", "", 46.10)
	EntFire("incinerator_pincer_lower_2","Trigger", "", 46.10)	
	
	EntFire("camera_1","Enable", "", 48.50)
	EntFire("@command","Command", "crosshair 0", 48.5)
	EntFire("camera_1","SetTarget", "pincer_1-claw_01", 49.60)
	EntFire("camera_1","SetTarget", "@sphere", 51.00)

//	EntFire("incinerator_pincer_grab","Trigger", "", 51.90)
//
//	EntFire("camera_1","SetTrackSpeed", "60", 50.6)
//	EntFire("camera_1","SetTarget", "pincer_1-claw_01", 52.6)

//	EntFire("incinerator_pincer_move_to_player","Trigger", "", 53.0)
	EntFire("incinerator_pincer_grab","Trigger", "", 51.40)

	EntFire("incinerator_pincer_grab_2","Trigger", "", 52.00)
	EntFire("pincer_2-incinerator_pincer_train_2", "startforward", "", 52.30)
	
//	EntFire("pincer_2-incinerator_pincer_train_2","Kill", "", 53.0)
	
//	EntFire("incinerator_pincer_grab","Trigger", "", 53.40)

	EntFire("@sphere","SetParent", "pincer_1-claw_01", 52.4 ) 

	EntFire("incinerator_pincer_raise","Trigger", "", 52.45)
	
	EntFire("@command","Command", "scene_playvcd npc/sphere01/PAINCRITICAL01", 56.1 )
	EntFire("@sphere","Explode", "", 57.1 )
	EntFire("@sphere","setparent", "", 57.15 )
	EntFire("@sphere","enablemotion", "", 57.2 )
	EntFire("incinerator_pincer_grab","Trigger", "", 57.25)

	EntFire("camera_1","SetTarget", "picked_up_look_target", 57.2)	



	EntFire("camera_1","SetPath", "new_path", 52.0)

	
	////////////////////
	
	EntFire("camera_1","SetTrackSpeed", "250", 62.20)
	
	EntFire("camera_1","SetTarget", "incinerator_target", 61.0)
	
	EntFire("need_gun_text","Display", "", 65)
	EntFire("jump_in_text","Display", "", 70)

	EntFire("relay_incinerator_open","Trigger", "", 73.0)

	EntFire("teleport_to_incinerator","Teleport", "", 74.45)
	EntFire("camera_1","ReturnToEyes", "", 74.5 )

	EntFire("camera_1","Disable", "", 75.5 )
	
	EntFire("@command","Command", "crosshair 1", 75.5)
	
//	EntFire("incinerator_blocker","Enable", "", 76.0)
	
	EntFire("incinerator_pincer_teleport_to_end_relay","Trigger", "", 75.5)
	EntFire("incinerator_pincer_raise","Trigger", "", 76.0)
}
