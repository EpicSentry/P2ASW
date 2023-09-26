WHEATLEY <- 0
GLADOS <- 1

TURRET<- 2
COMPUTER<- 3

NextNagTime <- -1
NextNagLine <- -1

NextSpeakTime <- -1
NextSpeakLine <- -1

NagLine1 <- -1
NagLine2 <- -1

Dialog <- {}

function SpeakLine( line )
{
	NextNagTime = -1
	NextSpeakTime = -1
	
	EntFire( "sphere_text_1", "SetText", "", 0 )	
	EntFire( "sphere_text_1", "Display", "", 0 )	
	EntFire( "sphere_text_2", "SetText", "", 0 )
	EntFire( "sphere_text_2", "Display", "", 0 )
	EntFire( "glados_text_1", "SetText", "", 0 )	
	EntFire( "glados_text_1", "Display", "", 0 )	
	EntFire( "glados_text_2", "SetText", "", 0 )
	EntFire( "glados_text_2", "Display", "", 0 )

	if( "speaker" in Dialog[line] && Dialog[line].speaker == GLADOS )
	{
		if ("one" in Dialog[line] )
		{
			EntFire( "glados_text_1", "SetText", "GLaDOS: " + Dialog[line].one, 0 )
			EntFire( "glados_text_1", "Display", "", 0 )
			
			EntFire( "glados_text_2", "SetText", "", 0 )
			EntFire( "glados_text_2", "Display", "", 0 )
		}
		if ("two" in Dialog[line] )
		{
			EntFire( "glados_text_2", "SetText", "GLaDOS: " + Dialog[line].two, 0 )
			EntFire( "glados_text_2", "Display", "", 0.75 )		
		}
	}
	else if( "speaker" in Dialog[line] && Dialog[line].speaker == WHEATLEY )
	{
		if ("one" in Dialog[line] )
		{
			EntFire( "sphere_text_1", "SetText", "Wheatley: " + Dialog[line].one, 0 )
			EntFire( "sphere_text_1", "Display", "", 0 )
			
			EntFire( "sphere_text_2", "SetText", "", 0 )
			EntFire( "sphere_text_2", "Display", "", 0 )
		}
		if ("two" in Dialog[line] )
		{
			EntFire( "sphere_text_2", "SetText", "Wheatley: " + Dialog[line].two, 0 )
			EntFire( "sphere_text_2", "Display", "", 0.75 )		
		}
	}
	
	else if( "speaker" in Dialog[line] && Dialog[line].speaker == TURRET )
	{
		if ("one" in Dialog[line] )
		{
			EntFire( "glados_text_1", "SetText", "Turret: " + Dialog[line].one, 0 )
			EntFire( "glados_text_1", "Display", "", 0 )
			
			EntFire( "glados_text_2", "SetText", "", 0 )
			EntFire( "glados_text_2", "Display", "", 0 )
		}
		if ("two" in Dialog[line] )
		{
			EntFire( "glados_text_2", "SetText", "Turret: " + Dialog[line].two, 0 )
			EntFire( "glados_text_2", "Display", "", 0.75 )		
		}
	}
	else if( "speaker" in Dialog[line] && Dialog[line].speaker == COMPUTER )
	{
		if ("one" in Dialog[line] )
		{
			EntFire( "glados_text_1", "SetText", "Computer: " + Dialog[line].one, 0 )
			EntFire( "glados_text_1", "Display", "", 0 )
			
			EntFire( "glados_text_2", "SetText", "", 0 )
			EntFire( "glados_text_2", "Display", "", 0 )
		}
		if ("two" in Dialog[line] )
		{
			EntFire( "glados_text_2", "SetText", "Computer: " + Dialog[line].two, 0 )
			EntFire( "glados_text_2", "Display", "", 0.75 )		
		}
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
	
	if( "relay" in Dialog[line] )
	{
		EntFire( Dialog[line].relay, "Trigger", "", Dialog[line].relayDelay )
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

function NextLine()
{
	if( NextSpeakLine > -1 )
	{
		SpeakLine( NextSpeakLine )
	}	
	else if( NextNagLine > -1 )
	{
		SpeakLine( NextNagLine )
	}
}

function ShowHelp()
{
	printl("speaker -  who is speaking, GLADOS or WHEATLEY")
	printl("one -      text displayed at the center of the screen")
	printl("two -      text displayed slightly below one")
	printl("nextLine - Line to play immediately after this one completes")
	printl("nextLineDelay - how long to wait until nextline is played")
	printl("relay -	   relay to fire when this line is spoken")
	printl("relayDelay - how to wait before the relay fires")
	printl("nagDelay - If this line is a nag how long until it repeats")
}


