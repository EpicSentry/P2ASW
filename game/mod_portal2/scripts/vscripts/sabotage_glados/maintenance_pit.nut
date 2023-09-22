	// ==============================
// ==============================
function OpenMaintenancePitDoor()
{
	EntFire( "maintenance_pit_model", "setanimation", "opening", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "open_idle", 0.1, 0 )
	
	// enable visibility on arms
	EnableAllArms()
	
	GrabGladosEyeLoop()
} 

// ==============================
// ==============================
function GrabGladosEyeLoop()
{
	EntFire( "glados_chamber_body", "setanimation", "glados_pit_eyegrab", 0, 0 )
	EntFire( "glados_chamber_body", "setdefaultanimation", "glados_pit_eyethrash_loop", 0.1, 0 )

	// set arm animations
	SetAnimation( "eyegrab", "eyethrash_loop" )
}


// ==============================
// ==============================
function PullGladosHeadDownLoop()
{

	EntFire( "glados_chamber_body", "setanimation", "glados_pit_headpull", 0, 0 )
	EntFire( "glados_chamber_body", "setdefaultanimation", "glados_pit_headthrash_loop", 0.1, 0 )
	
	
	// raise shield
	EntFire( "maintenance_pit_model", "setanimation", "sheildsup", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "sheildsup_idle", 0.1, 0 )
	
	
	// set arm animations
	SetAnimation( "headpull", "headthrash_loop" )
}

// ==============================
// ==============================
function EjectGladosHead()
{
	// eject glados head
	EntFire( "glados_detached_head", "setanimation", "head_eject", 0, 0 )
	EntFire( "glados_detached_head", "setdefaultanimation", "glados_floor_idle", 0.1, 0 )
	
	EntFire( "maintenance_pit_model", "setanimation", "head_eject", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "sheildsup_idle", 0.1, 0 )
	
	// move arm with glados head for ejection
	EntFire( "big_pos3", "setanimation", "head_toss", 0 )
	EntFire( "supervisor_pos2", "setanimation", "head_toss", 0 )

	// enable push, player clip logic
	EntFire( "glados_head_clip_relay", "trigger", "", 2.5 )
	
	
	// enable glados head, which starts disabled (and invisible) inside the pit
	EntFire ("glados_detached_head", "enable", 0, 0, 0 )
}

// ==============================
// ==============================
function RevealWheatley()
{
	// lower shield
	EntFire( "maintenance_pit_model", "setanimation", "sheildsdown", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "wheatly_open_idle", 0.1, 0 )

	SetAnimation( "wheatly_reveal", "sleep" )
	
}


// ==============================
// ============================== 
function MakePotatos()
{

	SetAnimation( "make_potato", "sleep" )
	
	// close door for potatos creation
	EntFire( "maintenance_pit_model", "setanimation", "potatos_close", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "potatos_closed_idle", 0.1, 0 )
	
	// retrieve glados head
	EntFire( "glados_detached_head", "setanimation", "make_potatos", 0, 0 )
	EntFire( "glados_detached_head", "setdefaultanimation", "sleep", 0.1, 0 ) 
}


// ==============================
// ==============================
function PresentPotatos()
{
	SetAnimation( "potatos_reveal", "potatos_reveal_idle" )
	
	// spawn the potatos model
	EntFire( "potatos_template", "forcespawn", "", 0, 0 )
	
	// open door for potatos presentation
	EntFire( "maintenance_pit_model", "setanimation", "potatos_opening", 0, 0 )
	EntFire( "maintenance_pit_model", "setdefaultanimation", "potatos_open_idle", 0.1, 0 )
}

// ==============================
// ==============================
function DeliverPotatos()
{
	SetAnimation( "reach", "sleep" ) 
	
	// present potatos with the potatos pincer
	EntFire( "potatos_pincer", "setanimation", "reach1", 0, 0 )
	EntFire( "potatos_pincer", "setdefaultanimation", "reach1_idle", 0.1, 0 )
	
	EntFire( "ghostAnim_potatos", "setanimation", "reach1", 0, 0 )
	EntFire( "ghostAnim_potatos", "setdefaultanimation", "reach1_idle", 0.1, 0 )
}


// ==============================
// ==============================
function WigglePotatosMultiple()
{

}

// ==============================
// ==============================
function TapGladosGentlyOnGlass()
{

}

// ==============================
// ==============================
function TapGladosStronglyOnGlass()
{

}

// ==============================
// ==============================
function PunchGladosThroughGlass()
{

	// need to drop potatos
	//EntFire( "ghostanim_potatos", "setanimation", "punch", 0, 0 )
	//EntFire( "ghostanim_potatos", "setdefaultanimation", "pound_idle", 0.1, 0 )
}

// ==============================
// ==============================
function PoundElevatorOne()
{
}

// ==============================
// ==============================
function PoundElevatorTwo()
{

}

// ==============================
// ==============================
function PoundElevatorThree()
{

}

// ==============================
// ==============================
function PoundElevatorFour()
{

}

// ==============================
// ==============================
function PoundElevatorFive()
{

}

// ==============================
// Enables pit arms. Note: not pincer
// ==============================
function EnableAllArms()
{
	EntFire( "small_pos*", "enable", 0, 0 )
	EntFire( "med_pos*", "enable", 0, 0 )
	EntFire( "big_pos*", "enable", 0.1, 0 )
	EntFire( "supervisor_pos*", "enable", 0.1, 0 )
}

// ==============================
// Deletes all but the small arms
// ==============================
function DeletePitArms()
{
	EntFire( "med_pos*", "kill", 0, 0 )
	EntFire( "big_pos*", "kill", 0, 0 )
	EntFire( "supervisor_pos*", "kill", 0, 0 )
}


// ==============================
// ==============================
function EnablePincer()
{
	EntFire( "potatos_pincer", "enable", 0.2, 0 )
}

// ==============================
// ==============================
function DeleteSmallArms()
{
	EntFire( "small_pos*", "kill", 0, 0 )
}


// ============================================================================
// ============================================================================
function SetAnimation( actionAnimation, idleAnimation )
{
	// iterate the small arms	
	for( local i=1; i<=10; i++ )
	{
		EntFire( "small_pos" + i, "setanimation", actionAnimation + i, 0, 0 )
		EntFire( "small_pos" + i, "setdefaultanimation", idleAnimation + i, 0.1, 0 )
	}
	
	// iterate the medium arms
	for( local i=1; i<=3; i++ )
	{
		EntFire( "med_pos" + i, "setanimation", actionAnimation + i, 0, 0 )
		EntFire( "med_pos" + i, "setdefaultanimation", idleAnimation + i, 0.1, 0 )
	}
	
	// iterate the large arms
	for( local i=1; i<=4; i++ )
	{
		EntFire( "big_pos" + i, "setanimation", actionAnimation + i, 0, 0 )
		EntFire( "big_pos" + i, "setdefaultanimation", idleAnimation + i, 0.1, 0 )
	}
	
		// iterate the supervisor arms
	for( local i=1; i<=5; i++ )
	{
		EntFire( "supervisor_pos" + i, "setanimation", actionAnimation + i, 0, 0 )
		EntFire( "supervisor_pos" + i, "setdefaultanimation", idleAnimation + i, 0.1, 0 )
	}
	
	
} 