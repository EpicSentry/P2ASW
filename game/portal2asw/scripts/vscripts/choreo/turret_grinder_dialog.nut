// -------------------------------------------------------------------
// Sound table
// -------------------------------------------------------------------
AlertSound <- "NPC_FloorTurret.Distressed"  // this sound entry contains several wavs that are chosen at random to be played'

// -------------------------------------------------------------------
// Precache sounds
// -------------------------------------------------------------------
function Precache()
{
	self.PrecacheSoundScript( AlertSound )
}

// -------------------------------------------------------------------
// Speak alert sound
// -------------------------------------------------------------------
function SpeakAlertSound()
{
	self.EmitSound( AlertSound )
}
