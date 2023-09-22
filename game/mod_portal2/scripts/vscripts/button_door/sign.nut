//=========================================================
// This script is attached to the sign entity.
// When the sign spawn it looks around for nearby doors.
// If the doors are portal doors then the doors increment
// their total buttons.
//=========================================================

// debugging
DBG <- 1

// door search range
const FIND_DOOR_RANGE = 200


my_door <- null
is_sign <- true

//---------------------------------------------------------
// OnPostSpawn
//---------------------------------------------------------
function OnPostSpawn()
{
	local cur_ent = null
	
	do
	{
		cur_ent = Entities.FindByClassnameWithin( cur_ent, "func_door", self.GetOrigin(), FIND_DOOR_RANGE )
		
		if(DBG && cur_ent != null ) printl( self.GetName() + " " +cur_ent.GetName() )

		if ( cur_ent != null && "is_portal_door" in cur_ent.GetScriptScope() && cur_ent.GetScriptScope().is_portal_door )
		{
			my_door <- cur_ent
			if(DBG) printl( self.GetName() + " found a door " + cur_ent.GetName() )
		}
	} while ( cur_ent != null )

	if ( my_door == null )
	{
		if(DBG) printl( self.GetName() + ": Did not find a nearby door." )
		return
	}
	
	my_door.GetScriptScope().IncrementTotalButtons()
}

//---------------------------------------------------------
// fires when OnUser1 is triggered, which is any time the
// button gets pressed
//---------------------------------------------------------
function OnUser1Output()
{
	if(DBG) printl( " attempting to open nearby door with door.nut" )
	my_door.GetScriptScope().CloseButton()
	
	// find nearby info_target that is_sign_target and fire its user1 output	
	local cur_ent = null
				
	// find info_target that sign was spawned on
	do 
	{
		cur_ent = Entities.FindByClassnameWithin( cur_ent, "info_target", self.GetOrigin(), 8 )

		if ( cur_ent != null && "is_sign_target" in cur_ent.GetScriptScope() && cur_ent.GetScriptScope().is_sign_target )
		{
			if(DBG) printl(" found info_target: " + cur_ent )
			EntFire( cur_ent.GetName(), "fireuser1" )
		}
	} while ( cur_ent != null )
}
//---------------------------------------------------------
// fires when OnUser1 is triggered, which is any time the
// button gets unpressed
//---------------------------------------------------------
function OnUser2Output()
{
	if(DBG) printl( " closing door " )	
	my_door.GetScriptScope().OpenButton()
	
		// find nearby info_target that is_sign_target and fire its user1 output	
	local cur_ent = null
				
	// find info_target that sign was spawned on
	do 
	{
		cur_ent = Entities.FindByClassnameWithin( cur_ent, "info_target", self.GetOrigin(), 8 )

		if ( cur_ent != null && "is_sign_target" in cur_ent.GetScriptScope() && cur_ent.GetScriptScope().is_sign_target )
		{
			if(DBG) printl(" found info_target: " + cur_ent )
			EntFire( cur_ent.GetName(), "fireuser2" )
		}
	} while ( cur_ent != null )
}