////////////////////////////////////  CONSTANTS //////////////////////////////////////////

///////////////////////////////////  FUNCTION DEFINITIONS ////////////////////////////////  Functions need to be defined before they are called.  That's why CODE section is at the bottom.

function Think()
{
	// Do this the first time through
	if ( !Started )
	{
		EntFire(self.GetName(),"CallScriptFunction", "SpawnPropsInBounds", 1) // delayed by 1 second to let all spawners respond.  We don't know how many spawners there are so we can't be certain ?!
		Started <- true
		StartTime <- Time()
	}
	
	LastThink = Time()
	

	// give everything time to spawn
	if ( Time() < ( StartTime + ( NPROPS * SPAWN_DELAY)  + 0.2 ) )
	{
		return
	}
	
	local AllSorted = 0
	
	foreach ( t, type in SP.Props ) // every model needs its own pile
	{
		
		local minx = WORLD_MAX
		local maxx = -WORLD_MAX
		local miny = WORLD_MAX
		local maxy = -WORLD_MAX
		
		foreach ( prop in type.items )  // every instance of this model needs to be within the pile min and max bounds
		{
			local v = prop.self.GetOrigin()
			
			if ( v.x < minx ) minx = v.x
			if ( v.x > maxx ) maxx = v.x
			
			if ( v.y < miny ) miny = v.y
			if ( v.y > maxy ) maxy = v.y
		}
		
		type.min.x = minx
		type.max.x = maxx
		type.min.y = miny
		type.max.y = maxy
		
		if(DBG)
		{
			//printl ( type.min.x + ", " + type.max.x + ", " + type.min.y + ", " + type.max.y )
			local origin = Vector(0,0,0)		
			DebugDrawBox( origin, type.min, type.max, 255, 255, 0, 1, 0.1 )
		}
		
		if ( ( ( type.max.x - type.min.x ) <= PILE_MAX )  && ( ( type.max.y - type.min.y ) <= PILE_MAX ) )
		{
			printl( t + " sorted" )
			AllSorted += 1
		}
	}
	
	if(DBG) printl("Piles Sorted: " + AllSorted + " / " + SP.Props.len())
	
	if ( AllSorted == SP.Props.len() )
	{
		foreach ( t, i in SP.Props )
		{
			foreach ( t, j in SP.Props )
			{
				if ( IntersectRect( i.min, i.max, j.min, j.max ) )
				{
					return
				}
			}
		}
		
		printl("Success: All piles are sorted and separated!")
	}
}
                  
function SpawnPropsInBounds()
{
	local randx
	local randy
	local randz

	for( local i = 0; i < NPROPS; i++ )
	{
		foreach( j, spawner in SP.PropSpawners )
		{
			randx = RandomFloat( SP.Bounds[MIN].x, SP.Bounds[MAX].x )
			randy = RandomFloat( SP.Bounds[MIN].y, SP.Bounds[MAX].y )
			randz = RandomFloat( SP.Bounds[MIN].z, SP.Bounds[MAX].z )  		
		
			// SP.Prop::SPawners[j].spawn(randx,randy,randz)  Even though this direct call is cleaner, the call goes through entity IO so we can stagger the spawn times.
			
			local code = "Spawn(" + randx +","+ randy +","+ randz +")"
			EntFire( SP.PropSpawners[j].self.GetName() ,"RunScriptCode", code, (i * SPAWN_DELAY ) )
		}
	}
}


/* This function is useless because a and b are passed in as value, not as reference
function Swap( a, b ) 
{
	if(DBG) printl("swapping "+a+" "+b)
	local temp = a
	a = b
	b = temp
}
*/

function IntersectRect( v1min, v1max, v2min, v2max )
{
	if ( ( ( v1min.x > v2min.x ) && ( v1min.x < v2max.x ) ) || ( ( v1max.x > v2min.x ) && ( v1max.x < v2max.x ) ) )
	{
		if ( ( ( v1min.y > v2min.y ) && ( v1min.y < v2max.y ) ) || ( ( v1max.y > v2min.y ) && ( v1max.y < v2max.y ) ) )
		{
			return true
		}
	}
	else if ( ( ( v1min.y > v2min.y ) && ( v1min.y < v2max.y ) ) || ( ( v1max.y > v2min.y ) && ( v1max.y < v2max.y ) ) )
	{
		if ( ( ( v1min.x > v2min.x ) && ( v1min.x < v2max.x ) ) || ( ( v1max.x > v2min.x ) && ( v1max.x < v2max.x ) ) )
		{
			return true
		}
	}
	
	return false
}

////////////////////////////////////////// CODE ///////////////////////////////////////////// this code gets run when the entity activates

LastThink <-Time()
Started <- false







