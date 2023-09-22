DBG 		<- 0

PowerUpSound <- "Portal.button_down"
PowerDownSound <- "coast.combine_apc_shutdown"

// Precache sounds we will emit
function Precache()
{
	self.PrecacheSoundScript( PowerUpSound )
	self.PrecacheSoundScript( PowerDownSound )
}

function OnPostSpawn()
{
	if (DBG) printl("==========================OnPostSpawn:: connecting outputs")	
	self.ConnectOutput( "OnPowered", "OnPowered" )
	self.ConnectOutput( "OnUnpowered", "OnUnpowered" )
}

function OnPowered()
{
	if (DBG) printl("==========================Laser powered!")
	PlayPoweredSound()	
}

function OnUnpowered()
{
	if (DBG) printl("==========================Laser unpowered!")
	PlayUnpoweredSound()
}

function PlayPoweredSound()
{
	if (DBG) printl("==========================Playing laser powered sound.")
	self.EmitSound( PowerUpSound )
}

function PlayUnpoweredSound()
{
	if (DBG) printl("==========================Playing laser unpowered sound.")
	self.EmitSound( PowerDownSound )
}