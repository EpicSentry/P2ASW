//=========================================================
// This script is attached to a logic_script entity.  The  
// script should reference info_target entities that are 
// spawn destinations for nuggets.
// 
// EntityGroup[0] should point to the env_maker
// EntityGroup[1] should be the name of the spawn destinations
// EntityGroup[2] should point to the game_text entity
//=========================================================

// debugging
DBG <- 0

// number of available nuggets
number_of_nuggets <- 0

// number of nuggets awarded to player
AwardedNuggetCount <- 0

//---------------------------------------------------------
// OnPostSpawn
//---------------------------------------------------------
function OnPostSpawn()
{
	// array to contain list of spawn destinations for nuggets
	spawn_dest_array <- []
	local cur_ent = null
	
	// collect the number of nuggets to spawn
	do
	{
		// find info_target that is a spawn position for a nugget
		cur_ent = Entities.FindByClassname( cur_ent, "info_target" )	
		if ( cur_ent != null && cur_ent.GetName() == EntityGroup[1].GetName() )
		{
			number_of_nuggets++
			spawn_dest_array.append( cur_ent )
			if(DBG) printl("[VSCRIPT_DEBUG] appended " + number_of_nuggets + " " + cur_ent.GetName() + " to spawn_dest_array" ) 
		}
	} while ( cur_ent != null )
	
		
	if(DBG) printl( "[VSCRIPT_DEBUG] spawning " + number_of_nuggets + " nuggets." )
	
	// spawn the templates at the destination			
	for(local i=0; i<number_of_nuggets; i++)
	{			
		EntityGroup[0].SpawnEntityAtEntityOrigin(spawn_dest_array[i] )
		if(DBG) printl( "[VSCRIPT_DEBUG] spawning " + EntityGroup[0] + " at " + spawn_dest_array[i].GetName()  )
	}
}

//---------------------------------------------------------
// This function gets called when a nugget trigger is touched
//---------------------------------------------------------
function AwardNugget()
{
	AwardedNuggetCount++	
	if(DBG) printl("[VSCRIPT_DEBUG] You got " + AwardedNuggetCount + " of " + number_of_nuggets + " Aperture Incentivizing Nuggets!")
	EntFire( EntityGroup[2].GetName(), "settext", "You got " + AwardedNuggetCount + " of " + number_of_nuggets + " Aperture Incentivizing Nuggets!") 	
	EntFire( EntityGroup[2].GetName(), "display" )
}
