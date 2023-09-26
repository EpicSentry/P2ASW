////////////////////////////////////  CONSTANTS //////////////////////////////////////////

///////////////////////////////////  FUNCTION DEFINITIONS ////////////////////////////////  Functions need to be defined before they are called.  That's why CODE section is at the bottom.

function Spawn( x, y, z )
{
	local v = Vector()
	
	v.x = x
	v.y = y
	v.z = z
	
	self.SetOrigin( v )

	EntFire(self.GetName(),"forcespawn")
	
}

function RegisterSpawner()
{
	SP.PropSpawners.append( this.weakref() )
	if(DBG) printl("Registered spawner: " + SP.PropSpawners[SP.PropSpawners.len()-1])
}

////////////////////////////////////////// CODE ///////////////////////////////////////////// this code gets run when the entity activates

RegisterSpawner()
