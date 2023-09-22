DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )

// Set wheatley idle on map spawn
EntFire( "!self", "SetIdleSequence", "sphere_damaged_idle_concerned", 0 )


//GladosPlayVcd indexes for original text
DialogVcd <- {}
//DialogVcd[200] <- 384
//DialogVcd[202] <- 385
//DialogVcd[203] <- 400
//DialogVcd[204] <- 386
//DialogVcd[205] <- 387


//Jailbreak
Dialog[200] <- { speaker = WHEATLEY, one = "Good to see you made it" two = "What are you doing all the way over there?" nextLine = 201, nextLineDelay = 4}
Dialog[201] <- { speaker = WHEATLEY, one = "We need to figure out a way to destroy the toxin vaporators" two = "Make your way over to me" }

Dialog[202] <- { speaker = WHEATLEY, one = "That laser should do the trick" two = "Use it to destroy the vaporator!", nagDelay = 30 }

Dialog[203] <- { speaker = WHEATLEY, one = "You disabled the toxin!" two = "Now break that tube!" }

Dialog[204] <- { speaker = WHEATLEY, one = "We want to leave through that tube!" two = "But you must destroy the toxin first" }

Dialog[205] <- { speaker = WHEATLEY, one = "Jump in the tube! It should lead us straight to Glados.", nagDelay = 10 }

Dialog[206] <- { speaker = WHEATLEY, one = "I'll stay here and check the flow." two = "You take out the neurotoxin." }

Dialog[207] <- { speaker = WHEATLEY, one = "Neurotoxin supply at 100 percent..." two = "but I hear explosions - that's a good sign right?" }

Dialog[208] <- { speaker = WHEATLEY, one = "Neurotoxin supply at 75 percent" }

Dialog[209] <- { speaker = WHEATLEY, one = "Neurotoxin supply at 50 percent" two = "Warning neurotoxin at dangerously unleathal levels" }

Dialog[210] <- { speaker = WHEATLEY, one = "Neurotoxin supply at 25 percent" }

Dialog[211] <- { speaker = WHEATLEY, one = "Neurotoxin supply at 0 percent" two = "Presure falling to 150 PSI" nextLine = 212, nextLineDelay = 3 }

Dialog[212] <- { speaker = WHEATLEY, one = "Presure at 100 PSI" two = "Presure at 50 PSI" nextLine = 213, nextLineDelay = 3 }

Dialog[213] <- { speaker = WHEATLEY, one = "Presure at 0 PSI" two = "Implosion Emminent..." nextLine = 214, nextLineDelay = 3 }

Dialog[214] <- { speaker = WHEATLEY, one = "What is an implosion anyway?" }

function SpeakLineVcd( arg )
{
	if (arg in DialogVcd)
	{
		EntFire("@glados","RunScriptCode","GladosPlayVcd("+DialogVcd[arg]+")", 0.00)
	}
	else
	{
		SpeakLine( arg )
	}
}


function ToxinWheatleyGoodToSeeYou()
{
	SpeakLineVcd( 200 )
}

function ToxinWheatleyGetTheLaser()
{
	SpeakLineVcd( 202 )
}

function ToxinWheatleyToxinIsOff()
{
	SpeakLineVcd( 203 )
}

function ToxinWheatleyTurnToxinOff()
{
	SpeakLineVcd( 204 )
}

function ToxinWheatleyJumpInTheTube()
{
	SpeakLineVcd( 205 )
}

function ToxinIllWatchTheMonitor() 
{
	SpeakLineVcd( 206 )
}

function ToxinAt100Percent()
{
	SpeakLineVcd( 207 )
}

function ToxinAt75Percent()
{
	SpeakLineVcd( 208 )
}

function ToxinAt50Percent()
{
	SpeakLineVcd( 209 )
}

function ToxinAt25Percent()
{
	SpeakLineVcd( 210 )
}

function ToxinAt0Percent()
{
	SpeakLineVcd( 211 )
}

function ToxinPressureFalling()
{
	SpeakLineVcd( 212 )
}

function ToxinImplosionEmminent()
{
	SpeakLineVcd( 213 )
}

function ToxinWhatIsAnImplosion()
{
	SpeakLineVcd( 214 )
}