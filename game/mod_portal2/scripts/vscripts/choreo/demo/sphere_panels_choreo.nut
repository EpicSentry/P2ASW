DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )


//GladosPlayVcd indexes for original text
DialogVcd <- {}
DialogVcd[1000] <- 388
DialogVcd[1001] <- 389
DialogVcd[1002] <- 390
DialogVcd[1003] <- 391


//Factory
Dialog[1000] <- { speaker = WHEATLEY, one = "We have to find the neurotoxin now", two = "The feeder tube should be in here somewhere" }

Dialog[1001] <- { speaker = WHEATLEY, one = "Drop down here onto this track" }

Dialog[1002] <- { speaker = WHEATLEY, one = "I can see the neurotoxin tube in the next room" }

Dialog[1003] <- { speaker = WHEATLEY, one = "Keep following the neurotoxin tubes" two = "I'll meet you up ahead" }

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


function PanelWheatleyFindToxin()
{
	SpeakLineVcd( 1000 )
}

function PanelWheatleyDropDown()
{
	SpeakLineVcd( 1001 )
}

function PanelWheatleySeeToxin()
{
	SpeakLineVcd( 1002 )
}

function PanelWheatleyFollowToxin()
{
	SpeakLineVcd( 1003 )
}
