////////////////////////////////////  CONSTANTS //////////////////////////////////////////

///////////////////////////////////  FUNCTION DEFINITIONS ////////////////////////////////  Functions need to be defined before they are called.  That's why CODE section is at the bottom.

function RegisterProp()
{
	if (!( self.GetName() in ::SP.Props ))  // the first time we've added this prop type
	{
		SP.Props[self.GetName()] <- { items = [], min = Vector( WORLD_MAX, WORLD_MAX, 16 ), max = Vector( -WORLD_MAX, -WORLD_MAX, 40 ) }
	}

	SP.Props[self.GetName()].items.append( this.weakref() )
	if(DBG) printl("Registered prop: " + self.GetName() + " " + SP.Props[self.GetName()].items.len() )
}

////////////////////////////////////////// CODE ///////////////////////////////////////////// this code gets run when the entity activates

RegisterProp()

