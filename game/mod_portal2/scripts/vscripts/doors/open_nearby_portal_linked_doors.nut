// Execute this script from the console to open nearby prop_portal_linked_doors.
// Intended to be used to traverse maps in order without noclipping to find the next linked map.

ent <- null
if ( ent = Entities.FindByClassnameWithin( ent, "prop_linked_portal_door", player.GetOrigin(), 256 ) )
{
	EntFire( ent.GetName(), "Open", "", 0.0, null ); 
}

