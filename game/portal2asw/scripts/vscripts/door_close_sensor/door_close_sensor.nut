// enable DBG to display debug spew 
DBG 		<- 0
DoorCloseDist 	<- 125
DoorSensorActive <- false
PlayerDist <- 0

self.ConnectOutput( "OnPlayerTeleportToMe", "OnPlayerTeleportToMeOutput" )
self.ConnectOutput( "OnPlayerTeleportFromMe", "OnPlayerTeleportFromMeOutput" )

//---------------------------------------------------------
// fires when player teleports to portal
//---------------------------------------------------------
function OnPlayerTeleportToMeOutput()
{
	if (DBG) printl("==========================TO output fired! Setting true. ")	
	DoorSensorActive = true
	// poll the distance to the player for the first time
	
	if (DBG) printl("==========================Getting distance to player for the first time. ")
	PlayerDist = GetDistanceToPlayer()
}

//---------------------------------------------------------
// fires when player teleports from portal
//---------------------------------------------------------
function OnPlayerTeleportFromMeOutput()
{
	if (DBG) printl("==========================FROM output fired! Setting false. ")
	DoorSensorActive = false	
}


//---------------------------------------------------------
// THINK function
//---------------------------------------------------------
function Think()
{
	if (DBG) printl("==========================" + DoorSensorActive + " before check. ")	
	if ( DoorSensorActive && IsMultiplayer() == false )
	{		
		if (DBG) printl("==========================GetDistanceToPlayer() returned" + PlayerDist )
		if (DBG) DebugDrawLine( player.GetOrigin(), self.GetOrigin(), 0, 255, 0, true, 1 )
		
		if (DBG) printl("==========================" + DoorSensorActive + " after check. ")	
		if ( PlayerDist > DoorCloseDist )
		{
			if (DBG) printl("==========================Closing door: " + self.GetName() + "================" )
			CloseDoor()
		}
		// poll the new distance to the player 
		PlayerDist = GetDistanceToPlayer()
	}
	else
	{
		if (DBG) printl("==========================FAILED due to false ")	
	}
	
}

function GetDistanceToPlayer()
{	
	// get player origin
	local posPlayer = player.GetOrigin();
	
	// get door (self) origin
	local posDoor = self.GetOrigin();
	
	// calculate distance to player from door
	local dist = ( posDoor - posPlayer ).Length2D();
	
	return dist
}

function CloseDoor()
{
	EntFire(self.GetName(),"close")
	DoorSensorActive = false
	// autosave
	
}