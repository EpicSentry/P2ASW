DBG 		<- 0

DoorOpenSound <- "prop_portal_door.open"
DoorCloseSound <- "prop_portal_door.close"

// Precache sounds we will emit
function Precache()
{
	self.PrecacheSoundScript( DoorOpenSound )
	self.PrecacheSoundScript( DoorCloseSound )
}

function OnPostSpawn()
{
	if (DBG) printl("==========================OnPostSpawn:: connecting outputs")	
	self.ConnectOutput( "OnFullyOpen", "OnFullyOpened" )
	self.ConnectOutput( "OnFullyClosed", "OnFullyClosed" )
}

function OnFullyOpened()
{
	if (DBG) printl("==========================Door Opened!")
	PlayOpenSound()	
}

function OnFullyClosed()
{
	if (DBG) printl("==========================Door Closed!")
	PlayCloseSound()
}

function PlayOpenSound()
{
	if (DBG) printl("==========================Playing door open sound <DING> ")
	self.EmitSound( DoorOpenSound )
}

function PlayCloseSound()
{
	if (DBG) printl("==========================Playing door closed sound <BBZT> ")
	self.EmitSound( DoorCloseSound )
}