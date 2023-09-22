////////////////////////////////////  CONSTANTS //////////////////////////////////////////

///////////////////////////////////  FUNCTION DEFINITIONS ////////////////////////////////  Functions need to be defined before they are called.  That's why CODE section is at the bottom.

function SetBounds( x, y, z )
{
	SP.Bounds.append( Vector() )
	
	local n = SP.Bounds.len() - 1
	local temp
	
	SP.Bounds[n].x = x
	SP.Bounds[n].y = y
	SP.Bounds[n].z = z

	if ( SP.Bounds.len() == 2 )
	{
		if ( SP.Bounds[MIN].x > SP.Bounds[MAX].x ) // Workaround: I tried to use a generic Swap function but without passing by reference there's no way to make it stick.
		{
			temp = SP.Bounds[MAX].x
			SP.Bounds[MAX].x = SP.Bounds[MIN].x
			SP.Bounds[MIN].x = temp
		}
		
		if ( SP.Bounds[MIN].y > SP.Bounds[MAX].y )
		{
			temp = parent.Bounds[MAX].y
			SP.Bounds[MAX].y = SP.Bounds[MIN].y
			SP.Bounds[MIN].y = temp
		}
		
		if ( SP.Bounds[MIN].z > SP.Bounds[MAX].z )
		{
			temp = SP.Bounds[MAX].z
			SP.Bounds[MAX].z = SP.Bounds[MIN].z
			SP.Bounds[MIN].z = temp
		}
		
		if(DBG) printl( "min: " + SP.Bounds[MIN].x +" "+ SP.Bounds[MIN].y +" "+ SP.Bounds[MIN].z )
		if(DBG) printl( "max: " + SP.Bounds[MAX].x +" "+ SP.Bounds[MAX].y +" "+ SP.Bounds[MAX].z )
		

	}
}

////////////////////////////////////////// CODE ///////////////////////////////////////////// this code gets run when the entity activates


local v = Vector()
v = self.GetOrigin()

SetBounds( v.x, v.y, v.z )








