DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )

// Set wheatley idle on map spawn
EntFire( "!self", "SetIdleSequence", "sphere_damaged_idle_concerned", 0 )


//GladosPlayVcd indexes for original text
DialogVcd <- {}

DialogVcd[1000] <- 364
DialogVcd[1001] <- 365
DialogVcd[1002] <- 366
DialogVcd[1003] <- 367
DialogVcd[1004] <- 368
DialogVcd[1005] <- 369
DialogVcd[1101] <- 370
DialogVcd[1104] <- 371
DialogVcd[2000] <- 372
DialogVcd[2015] <- 373
DialogVcd[2020] <- 374
DialogVcd[2025] <- 375
DialogVcd[2030] <- 376
DialogVcd[2035] <- 401
DialogVcd[2040] <- 402
DialogVcd[2045] <- 377
DialogVcd[2050] <- 378
DialogVcd[2055] <- 379
DialogVcd[2060] <- 380
DialogVcd[2070] <- 381
DialogVcd[2075] <- 399
DialogVcd[2095] <- 382
DialogVcd[2105] <- 383


//Factory
Dialog[1000] <- { speaker = WHEATLEY, one = "Hey! You made it through!" }

Dialog[1001] <- { speaker = WHEATLEY, one = "You have to come see what i've found." }

Dialog[1101] <- { speaker = WHEATLEY, one = "You're not gonna believe this..." }

Dialog[1104] <- { speaker = WHEATLEY, one = "Check it out!  It is the  turret quality control room!" }


Dialog[2000] <- { speaker = WHEATLEY, one = "That thing out there is the quality control scanner." two = "It is deciding which turrets to keep and which to toss." nextLine = 2010, nextLineDelay = 4 }
Dialog[2010] <- { speaker = WHEATLEY, one = "We need to change the scanner so it stops working." }

Dialog[2015] <- { speaker = WHEATLEY, one = "Let me see if I can grant us access to the control panel." }

Dialog[2020] <- { speaker = WHEATLEY, one = "There.  Now we can get inside!" }

Dialog[2025] <- { speaker = WHEATLEY, one = "See if you can find a way to unplug the turret Template." }

Dialog[2030] <- { speaker = WHEATLEY, one = "Try to unplug that turret in there." }

Dialog[2035] <- { speaker = COMPUTER, one = "Template disconnected.  Using last stored template." two = "Insert new Template to change turret control settings." }
Dialog[2037] <- { speaker = COMPUTER, one = "Using last stored template." two = "Please insert new Template to change turret settings." }


Dialog[2040] <- { speaker = WHEATLEY, one = "Ahh, the computer must have the old Template in memory." two = "We need to find a new one!" }

Dialog[2045] <- { speaker = WHEATLEY, one = "Grab one of those rejected turrets." two = "Maybe we can use it as a Template!" }

Dialog[2050] <- { speaker = WHEATLEY, one = "We need to get one of those rejected turrets plugged into the computer." }


Dialog[2055] <- { speaker = WHEATLEY, one = "Try Jumping!" }
Dialog[2060] <- { speaker = WHEATLEY, one = "NICE CATCH!" }


Dialog[2060] <- { speaker = TURRET, one = "NICE CATCH!" }


Dialog[2065] <- { speaker = WHEATLEY, one = "Wow, you caught one ugly turret.", nextLine = 2067, nextLineDelay = 3 }
Dialog[2067] <- { speaker = TURRET, one = "I like to party!" }

Dialog[2070] <- { speaker = WHEATLEY, one = "Plug it in!" }

Dialog[2075] <- { speaker = COMPUTER, one = "Template Detected..." two = "Scanning...", nextLine = 2080, nextLineDelay = 2 }
Dialog[2080] <- { speaker = COMPUTER, one = "Template Accepted." two = "Template, are you ready to accept the procedure?", nextLine = 2085, nextLineDelay = 3 }
Dialog[2085] <- { speaker = TURRET, one = "I like turtles!", nextLine = 2090, nextLineDelay = 2 }
Dialog[2090] <- { speaker = COMPUTER, one = "..." two = "New Template Accepted." }


Dialog[2095] <- { speaker = WHEATLEY, one = "All Right!  This is working better than expected." }

Dialog[2100] <- { speaker = WHEATLEY, one = "GLaDOS won't know the turrets are defective until it's too late." }

Dialog[2105] <- { speaker = WHEATLEY, one = "Let me open the door so we can get out of here." }



// exit lines

Dialog[1002] <- { speaker = WHEATLEY, one = "Great! Wait here until all the live turrets are gone" }

Dialog[1003] <- { speaker = WHEATLEY, one = "Drop down here onto this track" }

Dialog[1004] <- { speaker = WHEATLEY, one = "Stop there! I'll show you where to go from here" }

Dialog[1005] <- { speaker = WHEATLEY, one = "Follow me to the neurotoxin" }


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




function FactoryWheatleyHey()
{
	SpeakLineVcd( 1000 )
}

function FactoryWheatleySabotageTurrets()
{
	SpeakLineVcd( 1001 )
}

function FactoryWheatleyNotGonnaBelieveThis()
{
	SpeakLineVcd( 1101 )
}

function FactoryWheatleyTurretControlRoom()
{
	SpeakLineVcd( 1104 )
}

function FactoryWheatleyGoodWait()
{
	SpeakLineVcd( 1002 )
}

function FactoryWheatleyDropDown()
{
	SpeakLineVcd( 1003 )
}

function FactoryWheatleyLetMeLight()
{
	SpeakLineVcd( 1004 )
}

function FactoryWheatleyFollowMe()
{
	SpeakLineVcd( 1005 )
}


// Scanner scene dialog

function FactoryWheatleyControlRoomScannerIntro()
{
	SpeakLineVcd( 2000 )
}


function FactoryWheatleyControlRoomUnlock()
{
	SpeakLineVcd( 2015 )
	
	//Firing VCD in the level directly instead of SpeakLineVCD
	//When this VCD is finished playing it fires an output to the level logic that waits for player to look away
	EntFire( "wheatley_dont_watch_me_hack_vcd", "start", 0, 4.0 )
}

function FactoryWheatleyControlRoomUnlockSuccess()
{
	SpeakLineVcd( 2020 )
}

function FactoryWheatleyControlRoomUnplugNag1()
{
	SpeakLineVcd( 2025 )
}

function FactoryWheatleyControlRoomUnplugNag2()
{
	SpeakLineVcd( 2030 )
}

function FactoryComputerControlRoomInsertNewTurret()
{
	TurretVoManager.ProductionStall()
	EntFire( "@glados","runscriptcode","FactoryFirstTurretTaken()", 0.0 )
}

function FactoryComputerControlRoomInsertNewTurretNag1()
{
	SpeakLineVcd( 2035 )
}
 
function FactoryWheatleyControlRoomFindNewTurret()
{
	SpeakLineVcd( 2040 )
}

function FactoryWheatleyControlRoomFindNewTurretNag1()
{
	SpeakLineVcd( 2045 )
}

function FactoryWheatleyControlRoomFindNewTurretNag2()
{
	SpeakLineVcd( 2050 )
}


function FactoryWheatleyControlRoomFindTurretComment()
{
	SpeakLineVcd( 2055 )
}


function FactoryWheatleyControlRoomFindTurretSuccess()
{
	SpeakLineVcd( 2060 )
}

function FactoryWheatleyMeetTurretComment()
{
	SpeakLineVcd( 2065 )
}

function FactoryTurretMeetWheatleyComment()
{
	SpeakLineVcd( 2067 )
}

function FactoryWheatleyControlRoomInsertTurret()
{
	SpeakLineVcd( 2070)
}

function FactoryComputerNewTurretInserted()
{
	//SpeakLineVcd( 2075 )
	EntFire( "@glados", "runscriptcode", "FactorySuccess()", 0.0 )
	//EntFire( "move_wheatley_to_hacking_spot_relay", "Trigger", 0, 2.0 )
}


function FactoryWheatleyControlRoomDepartingComment()
{
	SpeakLineVcd( 2100 )
}

