NextNagTime <- -1
NextNagLine <- -1

NextSpeakTime <- -1
NextSpeakLine <- -1

NagLine1 <- -1
NagLine2 <- -1

Dialog <- {}
Dialog[0] <- { one = "The neurotoxin is housed in this wing of the labs",  two = "I can't disable it but I've got a plan for taking it out", nextLine = 1, nextLineDelay = 5 }
Dialog[1] <- { one = "Walk forward slowly!",  two = "There's a gap ahead" }
Dialog[2] <- { one = "Let me light this jump for you" } 

Dialog[3] <- { one = "Nice Jump!" } 
Dialog[4] <- { one = "Watch your step" } 

Dialog[5] <- { one = "Wait - did you hear something?", nextLine = 6, nextLineDelay = 5 }

Dialog[6] <- { one = "Oh sorry", two = "I forgot you can't see in the dark" }

Dialog[7] <- { one = "It sure is dark in here" }
Dialog[8] <- { one = "You may not be much of a conversationalist" two = "but at least you are a champion jumper!" }
Dialog[9] <- { one = "I'll join you on the other side of that wall" } 
Dialog[10] <- { one = "Coming! I'm coming!", two = "Sorry for leaving you in the dark", nextLine = 11, nextLineDelay = 7 }

Dialog[11] <- { one = "These Platforms look unstable", two = "You can make it though" }
Dialog[12] <- { one = "You need to get up onto the walkway", nagDelay = 5 }

Dialog[13] <- { one = "That's the spirit!" }

Dialog[14] <- { one = "Onto the panel assembly" }
Dialog[15] <- { one = "Watch your head" }
Dialog[16] <- { one = "Now that's what I call a tight squeeze!" }
Dialog[17] <- { one = "Try to get onto the walkway", two = "The manual override for the panel assembly is up here" }

Dialog[18] <- { one = "Don't fall off!" }

Dialog[19] <- { one = "We need to find across over that tube down there" two = "Maybe you can use the electro-magnet crane to cross the gap" nagDelay = 30 }
Dialog[31] <- { one = "You should be able to use that panel to get across!" nagDelay = 15 }
Dialog[20] <- { one = "" }
Dialog[21] <- { one = "Nice" }

Dialog[22] <- { one = "You see that red machine down there? That's the nerotoxin pump", two = "We need to destroy that so Glados can't gas you."}

Dialog[23] <- { one = "That tube there contains energy cores" two = "Each core contains a very small amount of animatter", nextLine = 24, nextLineDelay = 5 }
Dialog[24] <- { one = "Matter plus Antimatter equals a big explosion" two = "What I need you to do is blow up the tube so we can get a core", nextLine = 25, nextLineDelay = 5 }
Dialog[25] <- { one = "Reverse the tube direction then run!" }

Dialog[26] <- { one = "Crap - We are locked out.", two = "New Plan - we are going to get physical with the tube.", nextLine = 27, nextLineDelay = 6 }
Dialog[27] <- { one = "Up here", two = "I bet we can make use of this crane." }

Dialog[28] <- { one = "Good idea! - Maybe you can bash the tube with that" }
Dialog[29] <- { one = "That doesn't appear to be working" two = "Maybe we can use this crane some other way" }

Dialog[30] <- { one = "Well - there goes that plan" two = "Using the magnet might work out better for us anyway", nextLine = 41, nextLineDelay = 10 }
// 31 is taken above

Dialog[32] <- { one = "Good idea!", two = "maybe now we can use that magnet to get a core", nextLine = 41, nextLineDelay = 10 }

Dialog[33] <- { one = "That's the ticket", two = "Let me try to route something metal through there.", nextLine = 34, nextLineDelay = 5 }
Dialog[34] <- { one = "Stand back", two = "This is going to get messy!", nextLine = 35, nextLineDelay = 10 }

Dialog[35] <- { one = "Let's make a plan before you grab one of those cores", two = "If you set it down... well... boom" nextLine = 36, nextLineDelay = 5 }
Dialog[36] <- { one = "The core should be able to destroy the neurotoxin pump", two = "It looks like it's too far away for you to throw it", nextLine = 42, nextLineDelay = 10 }

Dialog[37] <- { one = "Bam! - Haha!", two = "Good job" nextLine = 38, nextLineDelay = 6 }
Dialog[38] <- { one = "Not to alarm you but you should probably get out of here", two = "That is neurotoxin leaking out of that thing afterall." nextLine = 39, nextLineDelay = 8 }
Dialog[39] <- { one = "I'll catch up with you later", two = "If anyone asks just pretend nothing happened." }

Dialog[40] <- { one = "Cruuuunch!" }
Dialog[41] <- { one = "Maybe we can use that magnet to get an energy core", nagDelay = 20 }
Dialog[42] <- { one = "Whatever you do - don't put the core down", two = "Pitch it over the railing if you need to drop it", nextLine = 36, nextLineDelay = 10 }

Dialog[43] <- { one = "It's good to finally meet you", two = "You can call me Pendleton", nextLine = 44, nextLineDelay = 5 }
Dialog[44] <- { one = "Glados used to terrorize us AI task cores", two = "I've been able to avoid detection for now but she's bound to find me", nextLine = 45, nextLineDelay = 5 }
Dialog[45] <- { one = "She's unstable so it's critical that we take her out", two = "We got to strike before she can fully rebuild", nextLine = 46, nextLineDelay = 5 }
Dialog[46] <- { one = "I hope I can count on you" two = "Let's get going" }

function SpeakLine( line )
{
	NextNagTime = -1
	NextSpeakTime = -1

	if ("one" in Dialog[line] )
	{
		EntFire( "sphere_text_1", "SetText", Dialog[line].one, 0 )
		EntFire( "sphere_text_1", "Display", "", 0 )
		
		EntFire( "sphere_text_2", "SetText", "", 0 )
		EntFire( "sphere_text_2", "Display", "", 0 )
	}

	if ("two" in Dialog[line] )
	{
		EntFire( "sphere_text_2", "SetText", Dialog[line].two, 0 )
		EntFire( "sphere_text_2", "Display", "", 0.75 )		
	}
	
	if ("nextLine" in Dialog[line] )
	{
		if ("nextLineDelay" in Dialog[line] )
		{
			NextSpeakTime = Time() + Dialog[line].nextLineDelay
		}
		else
		{
			NextSpeakTime = Time() + 5
		}
		
		NextSpeakLine = Dialog[line].nextLine
	}
	else
	{
		NextSpeakTime = -1
		NextSpeakLine = -1
	}
	
	if ("nagDelay" in Dialog[line] )
	{
		if ("nextLine" in Dialog[line] )
		{
			printl("Hey Dummy! How are you going to nag and speak another line? Well - I'm waiting?!?")
			printl("Hey Dummy! How are you going to nag and speak another line? Well - I'm waiting?!?")
			printl("Hey Dummy! How are you going to nag and speak another line? Well - I'm waiting?!?")
			printl("Hey Dummy! How are you going to nag and speak another line? Well - I'm waiting?!?")
			printl("Hey Dummy! How are you going to nag and speak another line? Well - I'm waiting?!?")
			
			NextSpeakTime = -1
			NextSpeakLine = -1
		}

		NextNagLine = line
		NextNagTime = Time() + Dialog[line].nagDelay
	}
}

function Think()
{
	if( NextSpeakTime > -1 && NextSpeakLine > -1 && Time() > NextSpeakTime )
	{
		SpeakLine( NextSpeakLine ) // this might set new next lines to speak.
	}
	
	else if( NextNagTime > -1 && NextNagLine > -1 && Time() > NextNagTime )
	{
		SpeakLine( NextNagLine )
	}
}