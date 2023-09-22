//pitchShifting <- false

function StartClawPickup()  // 45.4
{
	EntFire("@glados","RunScriptCode", "sp_a1_wakeup_WheatleyGettingGrabbed()", 1.2)

	EntFire("glados_cables_hip_03","DisableDraw", "", 1.2)

	EntFire("claw_clang_sound","PlaySound", "", 2.2)

	EntFire("camera_1","Enable", "", 2.2)

	EntFire("trigger_weapon_strip", "Enable", "", 2.6)

	EntFire("camera_1","Disable", "", 3.4)
	EntFire("camera_ghostAnim_2","Enable", "", 3.4)
	
	EntFire("@sphere","SetParent", "ghostAnim", 3.4) // delay this until we aren't looking at wheatley.
	EntFire("@sphere","SetParentAttachment", "attach_2", 3.5) 

	EntFire("gun_shooter", "Shoot", "", 6 )
	
	EntFire("relay_incinerator_open","Trigger", "", 30.6)

	EntFire("achievement_wakeup_glados","FireEvent", "", 37.45)
}