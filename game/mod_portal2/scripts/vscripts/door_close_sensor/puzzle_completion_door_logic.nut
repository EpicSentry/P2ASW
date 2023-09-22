// enable DBG to display debug spew 
DBG 		<- 0
DoorCloseDist 	<- 125
DidPlayerExitRoom <- false
PlayerDist <- 0

// Precache sounds we will emit
function Precache()
{
	self.PrecacheSoundScript( "Portal.elevator_ding" )
	self.PrecacheSoundScript( "Portal.button_up" )
}

function OnPostSpawn()
{
	if (DBG) printl("==========================OnPostSpawn:: connecting outputs")	
	self.ConnectOutput( "OnFullyOpen", "OnDoorFullyOpened" )
	self.ConnectOutput( "OnFullyClosed", "OnDoorFullyClosed" )
}

function OnDoorFullyOpened()
{
	if (DBG) printl("==========================Door Opened!")
	PlayDoorOpenSound()	
}

function OnDoorFullyClosed()
{
	if (DBG) printl("==========================Door Closed!")
	if ( !DidPlayerExitRoom )
	{
		PlayDoorCloseSound()
	}
}

//---------------------------------------------------------
// fires when player teleports to portal
//---------------------------------------------------------
function OnPlayerTeleportToMeOutput()
{
	if (DBG) printl("==========================TO output fired! Setting false. ")	
	DidPlayerExitRoom = false
}

//---------------------------------------------------------
// fires when player teleports from portal
//---------------------------------------------------------
function OnPlayerTeleportFromMeOutput()
{
	if (DBG) printl("==========================FROM output fired! Setting true. ")
	DidPlayerExitRoom = true
}

function PlayDoorOpenSound()
{
	if (DBG) printl("==========================Playing door open sound <DING> ")
	self.EmitSound( "Portal.elevator_ding" )
}

function PlayDoorCloseSound()
{
	if (DBG) printl("==========================Playing door closed sound <BBZT> ")
	self.EmitSound( "Portal.button_up" )
}
