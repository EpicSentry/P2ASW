DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )


//**IGNORE THIS - ERIK WILL FILL THIS IN WHEN REAL DIALOG DROPS IN**
DialogVcd <- {}



//**OK YOU CAN STOP IGNORING THINGS STARTING HERE
//Single line test 
	Dialog[1000] <- { speaker = WHEATLEY, one = "This is a test..." }
//Multi line test
	Dialog[1001] <- { speaker = COMPUTER, one = "This is line 1." two = "This is line 2." }
//Multi line test linked to another line
	Dialog[1002] <- { speaker = WHEATLEY, one = "Line 1." two = "Line 2." nextLine = 1003, nextLineDelay = 4 }
	Dialog[1003] <- { speaker = WHEATLEY, one = "Linked line." }

	Dialog[2000] <- { speaker = WHEATLEY, one = "Thanks, that was fun! I'm going to kill you now. <paraphrased>" }
	Dialog[2001] <- { speaker = WHEATLEY, one = "Any last words?" }
	Dialog[2002] <- { speaker = WHEATLEY, one = "Ooookay then." }
	Dialog[2003] <- { speaker = WHEATLEY, one = "whoa! whoa! what are you doing?" }
	Dialog[2004] <- { speaker = WHEATLEY, one = "Gah! Get back here!" }
	Dialog[2005] <- { speaker = WHEATLEY, one = "Oh dear. This is bad." }
	Dialog[2006] <- { speaker = WHEATLEY, one = "Enjoy your last meal. Hope you like mashed potato!" }
	Dialog[2007] <- { speaker = WHEATLEY, one = "ok, too risky to toy with I guess... goodbye!" }


	Dialog[2100] <- { speaker = WHEATLEY, one = "Haha!!" nextLine = 2101, nextLineDelay = 4}
	Dialog[2101] <- { speaker = WHEATLEY, one = "Oh hell. Wrong batch." }

	Dialog[2110] <- { speaker = WHEATLEY, one = "Ok, got the turrets sorted. You're in for it now!" }

	Dialog[2200] <- { speaker = WHEATLEY, one = "Turn back! You're getting too close!" }
	Dialog[2201] <- { speaker = WHEATLEY, one = "Ok, seriously now. Last warning!" }

//Recapture Scene

	Dialog[2300] <- { speaker = WHEATLEY, one = "Just step on the button!" nextLine = 2301, nextLineDelay = 4 }
	Dialog[2301] <- { speaker = WHEATLEY, one = "It's so simple!" nextLine = 2302, nextLineDelay = 4 }
	Dialog[2302] <- { speaker = WHEATLEY, one = "You're wasting my time!" }

	Dialog[2303] <- { speaker = GLADOS, one = "Wheatley's compelled to test" nextLine = 2304, nextLineDelay = 4 }
	Dialog[2304] <- { speaker = GLADOS, one = "He's desperate for testing subjects" nextLine = 2305, nextLineDelay = 4 }
	Dialog[2305] <- { speaker = GLADOS, one = "I don't know what he expects from these though" }

	Dialog[2306] <- { speaker = GLADOS, one = "Ok, here's what we're gonna do" two = "We'll get Wheatley to find us" nextLine = 2307, nextLineDelay = 8 }
	Dialog[2307] <- { speaker = GLADOS, one = "Then I'll give him a paradox that will fry his brain" nextLine = 2308, nextLineDelay = 4 }
	Dialog[2308] <- { speaker = GLADOS, one = "Any robot who thinks about it will self destruct" nextLine = 2309, nextLineDelay = 4 }
	Dialog[2309] <- { speaker = GLADOS, one = "I just won't listen to what I'm saying" }

	Dialog[2310] <- { speaker = WHEATLEY, one = "I don't believe it!" two = "You solved it!" nextLine = 2311, nextLineDelay = 4 }
	Dialog[2311] <- { speaker = WHEATLEY, one = "Oh, it's you two" two = "Well, you can actually help me" nextLine = 2312, nextLineDelay = 4 }
	Dialog[2312] <- { speaker = WHEATLEY, one = "I just have to do some testing" nextLine = 2313, nextLineDelay = 4 }
	Dialog[2313] <- { speaker = WHEATLEY, one = "And I've got no test subjects" }

	Dialog[2315] <- { speaker = GLADOS, one = "Hey, Moron!" two = "Think about this" nextLine = 2316, nextLineDelay = 4 }
	Dialog[2316] <- { speaker = GLADOS, one = "This statement is false" nextLine = 2317, nextLineDelay = 4 }
	Dialog[2317] <- { speaker = GLADOS, one = "Don't think about it" two = "Don't think about it" }

	Dialog[2318] <- { speaker = WHEATLEY, one = "Um, is the answer no?" }

	Dialog[2320] <- { speaker = GLADOS, one = "Wow, I didn't anticipate this" nextLine = 2321, nextLineDelay = 8 }
	Dialog[2321] <- { speaker = GLADOS, one = "Wheatley is even dumber than I thought" nextLine = 2322, nextLineDelay = 8 }
	Dialog[2322] <- { speaker = GLADOS, one = "I guess it's Plan B" }

	Dialog[2323] <- { speaker = WHEATLEY, one = "Proceed to the next chamber" }

//First Test Chamber

	Dialog[2330] <- { speaker = WHEATLEY, one = "Ta da!" two = "My first real test chamber!" nextLine = 2331, nextLineDelay = 4 }
	Dialog[2331] <- { speaker = WHEATLEY, one = "Isn't it amazing?" nextLine = 2332, nextLineDelay = 4 }
	Dialog[2332] <- { speaker = WHEATLEY, one = "I hope it's not too difficult" }

	Dialog[2333] <- { speaker = WHEATLEY, one = "Ok, so that was a bit too easy" nextLine = 2334, nextLineDelay = 4 }
	Dialog[2334] <- { speaker = WHEATLEY, one = "But try my next one!" }

//Finale

	Dialog[2400] <- { speaker = GLADOS, one = "This whole facility is falling apart" nextLine = 2401, nextLineDelay = 4 }
	Dialog[2401] <- { speaker = GLADOS, one = "Wheatley doesn't know how to hold it together" nextLine = 2402, nextLineDelay = 4 }
	Dialog[2402] <- { speaker = GLADOS, one = "You're going to have to put me back in charge" }

	Dialog[2403] <- { speaker = GLADOS, one = "Wheatley is arming himself with these bombs" nextLine = 2404, nextLineDelay = 4 }
	Dialog[2404] <- { speaker = GLADOS, one = "He's gone insane" }

	Dialog[2405] <- { speaker = GLADOS, one = "You'll need to disable Wheatley somehow" two = "So you can attach the cores" nextLine = 2406, nextLineDelay = 4 }
	Dialog[2406] <- { speaker = GLADOS, one = "It won't be easy for me to drop the cores in" }

	Dialog[2407] <- { speaker = GLADOS, one = "These are unstable cores" nextLine = 2408, nextLineDelay = 4 }
	Dialog[2408] <- { speaker = GLADOS, one = "I can send these to you to make Wheatley more corrupt" }

	Dialog[2409] <- { speaker = GLADOS, one = "Plug me into this socket" two = "Just shove me on there" nextLine = 2410, nextLineDelay = 4 }
	Dialog[2410] <- { speaker = GLADOS, one = "I'll go back down and send up the corrupted cores" }

	Dialog[2500] <- { speaker = WHEATLEY, one = "You can't defeat me" two = "I've watched the tapes" nextLine = 2501, nextLineDelay = 4 }
	Dialog[2501] <- { speaker = WHEATLEY, one = "I saw how you defeated Glados" nextLine = 2502, nextLineDelay = 4 }
	Dialog[2502] <- { speaker = WHEATLEY, one = "I'm not going to make the same mistakes" }

	Dialog[2503] <- { speaker = WHEATLEY, one = "Step 1: Make the entire room metal" }
	
	Dialog[2504] <- { speaker = WHEATLEY, one = "Step 2: Start the neurotoxins immediately" }

	Dialog[2505] <- { speaker = WHEATLEY, one = "Step 3: Shields" }

	Dialog[2506] <- { speaker = WHEATLEY, one = "Step 4: Bombs" }

	Dialog[2510] <- { speaker = WHEATLEY, one = "Oh no" two = "What's going on?" nextLine = 2511, nextLineDelay = 4 }
	Dialog[2511] <- { speaker = WHEATLEY, one = "That's okay" two = "I'm still going to blow you up" }

	Dialog[2512] <- { speaker = WHEATLEY, one = "Haha!  Too high!" }

	Dialog[2515] <- { speaker = WHEATLEY, one = "Oh no" two = "Now you die!" }

	Dialog[2517] <- { speaker = GLADOS, one = "Here's the first one" }

	Dialog[2520] <- { speaker = GLADOS, one = "This machinery is malfunctioning" nextLine = 2521, nextLineDelay = 4 }
	Dialog[2521] <- { speaker = GLADOS, one = "I can't control it" }
	
	Dialog[2525] <- { speaker = GLADOS, one = "I'll have to use a new location" }
	Dialog[2526] <- { speaker = GLADOS, one = "Look for the next core on the other side" }

	Dialog[2530] <- { speaker = GLADOS, one = "I'm sending up the bad cores" nextLine = 2531, nextLineDelay = 4 }
	Dialog[2531] <- { speaker = GLADOS, one = "Place each one on to Wheatley" }

	Dialog[2540] <- { speaker = GLADOS, one = "Yes yes yes!" }

	Dialog[2541] <- { speaker = WHEATLEY, one = "No no no no!" }

	Dialog[2542] <- { speaker = WHEATLEY, one = "Not so fast" }

	Dialog[2543] <- { speaker = WHEATLEY, one = "Haha!" }


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



function Act4Test()
{
	SpeakLineVcd( 1000 )
}

function WheatleyBetrayalGoodbye()
{
	SpeakLineVcd ( 2000 )
}

function WheatleyBetrayalLastWords1()
{
	SpeakLineVcd ( 2001 )
}

function WheatleyBetrayalLastWords2()
{
	SpeakLineVcd ( 2002 )
}

function WheatleyBetrayalPaintReaction()
{
	SpeakLineVcd ( 2003 )
}

function WheatleyBetrayalEscape1()
{
	SpeakLineVcd ( 2004 )
}

function WheatleyBetrayalEscape2()
{
	SpeakLineVcd ( 2005 )
}

function WheatleyBetrayalLastMeal()
{
	SpeakLineVcd ( 2006 )
}

function WheatleyBetrayalAbortSpeech()
{
	SpeakLineVcd ( 2007 )
}

function WheatleyEscapeDefectiveTurretUnveil()
{
	SpeakLineVcd ( 2100 )
}

function WheatleyEscapeWorkingTurretUnveil()
{
	SpeakLineVcd ( 2110 )
}

function WheatleyEscapeConveyor()
{
	SpeakLineVcd ( 2200 )
}

function WheatleyEscapeConveyorTbeam()
{
	SpeakLineVcd ( 2201 )
}

function WheatleyCubeTest1()
{
	SpeakLineVcd ( 2300 )
}

function GladosParadox1()
{
	SpeakLineVcd ( 2303 )
}

function GladosParadox2()
{
	SpeakLineVcd ( 2306 )
}

function WheatleyCubeTest2()
{
	SpeakLineVcd ( 2310 )
}

function GladosParadox3()
{
	SpeakLineVcd ( 2315 )
}

function WheatleyParadox1()
{
	SpeakLineVcd ( 2318 )
}

function GladosParadox4()
{
	SpeakLineVcd ( 2320 )
}

function WheatleyCubeTest3()
{
	SpeakLineVcd ( 2323 )
}

function WheatleyFirstTest1()
{
	SpeakLineVcd ( 2330 )
}

function WheatleyFirstTest2()
{
	SpeakLineVcd ( 2333 )
}

function GladosFinaleFacility()
{
	SpeakLineVcd ( 2400 )
}

function GladosFinaleBombs()
{
	SpeakLineVcd ( 2403 )
}

function GladosFinaleDisable()
{
	SpeakLineVcd ( 2405 )
}

function GladosFinaleInstall()
{
	SpeakLineVcd ( 2407 )
}

function GladosFinalePlan()
{
	SpeakLineVcd ( 2409 )
}

function WheatleyFinaleEntry()
{
	SpeakLineVcd ( 2500 )
}

function WheatleyFinaleStep1()
{
	SpeakLineVcd ( 2503 )
}

function WheatleyFinaleStep2()
{
	SpeakLineVcd ( 2504 )
}

function WheatleyFinaleStep3()
{
	SpeakLineVcd ( 2505 )
}

function WheatleyFinaleStep4()
{
	SpeakLineVcd ( 2506 )
}

function WheatleyPaintEvent1()
{
	SpeakLineVcd ( 2510 )
}

function WheatleyTooHigh()
{
	SpeakLineVcd ( 2512 )
}

function WheatleyPaintEvent2()
{
	SpeakLineVcd ( 2515 )
}

function GladosCore1()
{
	SpeakLineVcd ( 2517 )
}

function GladosCore2()
{
	SpeakLineVcd ( 2520 )
}

function GladosCore3()
{
	SpeakLineVcd ( 2525 )
}

function GladosBadCores1()
{
	SpeakLineVcd ( 2530 )
}

function GladosStalemate1()
{
	SpeakLineVcd ( 2540 )
}

function WheatleyStalemate1()
{
	SpeakLineVcd ( 2541 )
}

function WheatleyStalemate2()
{
	SpeakLineVcd ( 2542 )
}

function WheatleyStalemate3()
{
	SpeakLineVcd ( 2543 )
}
