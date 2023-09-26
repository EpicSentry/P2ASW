// this checks the "developer" level, will have no effct if changed manually
DBG <- 0

DO_CREDITS <- 0

// blanks are not sent to the server and aren't printed in the screen.  They just produce blank conveyor belt and act as a PAUSE
BLANK		<-	" "
// SPACES are sent to the server to be printed on the screen
SPACE		<-	"     "

// "Flavor" items.  These won't print anything to the screen
TURRET		<-	"(.)"
TURRET_S	<-	":.:"
CUBE		<-	"[=]"
MONSTER		<-	"[(.)]"
BLUE		<-	"( o )"
ORANGE		<-	"|o|"
BIRD		<-	"**B**"
COMPCUBE	<-	"[[COMCUBE]]"
PERSPEHERE	<-	"*@OOOO@*"
TURRETBACK	<-	"((BACK))"
TURRETWIFE	<-	"((WIFE))"
TURRETBOX	<-	"[[BOX]]"

// these items have a special conveyor item associated with them
VOICESTITLE		<-	"[_____________________Voices ___________________]"
GLADOS			<-	"[ GlaDOS / Turrets ]          :      Ellen McLain"
WHEATLEY		<-	"[ Wheatley ]                  :  Stephen Merchant"
CAVE			<-  "[ Cave Johnson ]              :      J.K. Simmons"
ANNOUNCER		<-	"[ Announcer ]                 :      Joe Michaels"
CCORESTUR		<-	"[ Cores / Defective Turrets ] :       Nolan North"
BOTSHUG			<-	"[ Atlas / P-Body ]            : Dee Bradley Baker"
//////////
CHELL			<- 	"[ Face model for Chell ] :        Alesia Glidewell"
STILLALIVE		<-	"[ 'Still Alive' by ]     :        Jonathan Coulton"
GONE			<- 	"[ 'Want You Gone' by ]   :        Jonathan Coulton"
VILIFY			<- 	"[ 'Exile Vilify' by ]    :            The National"
VOICERECORD		<-  "[ Voice Recording ]      : Pure Audio, Seattle, WA"
TRANSLATIONS	<-  "[ Translations ]         :                     SDL"
/////////////////////
SPECIALTHANKS	<-  "[ Special thanks to ]"

// when we hit this, credits will end
ENDING			<-	"<CREDITS END>"
// when we hit this, this is the end of the humans
ENDOFHUMANS			<-	"<ENDOFHUMANS>"

// when we hit this, don't play anymore glados lines to clear her for final line
ENDOFHUMANSPREP			<-	"<ENDOFHUMANSPREP>"


// this is the state of the credits that we start in, it brings the name from the left and scrolls it down
STATE_NAMES		<- "----NAMES"

// this specifies to stop doing the "scanning" name stuff and just do a block of text
// we have a total of 10 lines to show on screen at once
STATE_TEXTBLOCK	<- "----TEXT"

//0=blank
//1=female
//2=male
//3=bluebot
//4=orangebot
//5=turretnormal
//6=terminatorturret
//7=cube
//8=monstercube
//9=bird on cube
//10=companion cube
//11= 3 corrupt cores
//12= turret backwards
//13= turret wife
//14= turret boxes

//16= SPACE
//17= (VOICE) Wheatley
//18= RADIO SONG (still alive)
//19= computer
//20= computer2
//21= chell
//22= (VOICE) Cave portrait
//23= (VOICE) corrupt cores and defective turret
//24= (VOICE) glados
//25= (VOICE) Bots hugging
//26= RADIO SONG2 (gone)
//27= RADIO SONG3 (vilify)

// glados talking stuff
GladosRndStart <-0
GladosRndStart = RandomInt(1,5)
GladosTempCounter <-0
GladosInterval <- 0
GladosCounter <-0
CreditTimer <- 0

NAME_TIME_INTERVAL <- 0.85
BLOCKTEXT_TIME_INTERVAL <- 1.65
// how often the conveyor spawns a new item
TIME_INTERVAL <- NAME_TIME_INTERVAL

m_nLastNameTime <- 0.0
m_nNameIndex <- -1
m_nVGUIIndex <- -1
m_nTotalNameCount <- -1
m_bCreditsFinished <- false

m_nCreditsState <- 0
m_nStateChangeIn <- 0
m_nStateChangeTo <- 0
EndofEmployees <- 0

CreditsList<- [
	BLANK,
	BLANK,
	BLANK,
/////////////////////////////
// TESTING AREA BELOW
////////////////////////////
//	
/////////////////////////////
// TESTING AREA ABOVE
////////////////////////////
 	"Aaron Barber",
 	"Aaron Halifax",
 	"Aaron Kearly",
 	"Aaron Nicholls",
 	"Aaron Seeler",
 	"Adam Foster",
 	"Adrian Finol",
	"Adrian Johnston",
 	TURRET,
 	"Al Farnsworth",
 	"Alden Kroll",
 	"Alex Vlachos",
	"Alexander Mark",
 	"Alfred Reynolds",
 	"Alireza Razmpoosh",
 	"Amanda Beste",
 	"Andrea Wicklund",
 	"Andrew Burke",
	"Andrew Kim",
 	CUBE,
	"Andrew Loomer",
 	"Andrew Wolf",
	"Andy Nisbet",
 	"Anna Sweet",
 	"Antoine Bourdon",
 	"Antonello Maddalena",
 	"Ariel Diaz",
 	"Arsenio N. Navarro II",
 	"Augusta Butlin",
 	"Bay Raitt",
 	ORANGE,
 	"Bill Fletcher",
 	"Bill Van Buren",
 	"Brandon Idol",
 	"Brandon Reinhart",
 	"Brett English",
 	"Brian Jacobson",
 	"Bronwen Grimes",
	"Bruce Dawson",
 	"Burton Johnsey",
	"Caleb Benefiel",
 	"Caroline Müller",
 	TURRET_S,
 	"Cayle George",
 	"Chad Weaver",
 	"Chandler Murch",
 	"Charlie Brown",
 	"Charlie Burgin",
 	"Chet Faliszek",
 	"Chris Carollo",
 	"Chris Chin",
 	"Chris Douglass",
 	"Chris Green",
 	"Chris Grinstead",
 	"Chris Shambaugh",
 	"Chris Welch",
 	"Christen Coomer",
	"Christian Rivers",
 	"Christopher Boyd",
	"Christine Phelan",
 	"Connor Malone",
 	"Corey Peters",
 	"Dan Berger",
 	CUBE,
 	"Danika Wright",
 	"Dario Casali",
 	"Dave Feise",
 	"Dave Riller",
 	"Dave Saunders",
 	"David Kircher",
 	"David Sawyer",
 	"David Speyrer",
	"Derek Zumsteg",
 	"Derrick Birum",
 	"Derrick Gennrich",
 	"Dhabih Eng",
 	"Dina Nelson",
 	"DJ Powers",
 	"Don Holden",
 	BLUE,
	"Doug Church",
 	"Doug Lombardi",
 	"Doug Valente",
 	"Doug Wood",
 	"Elan Ruskin",
	"Elizabeth Andrade",
	"Emily Kent",
 	"Eric Hope",
 	"Eric Kirchmer",
 	"Eric Smith",
 	"Eric Strand",
 	"Eric Tams",
 	"Eric Wanless",
 	"Erik Johnson",
 	"Erik Robson",
 	"Erik Wolpaw",
	"Fletcher Dunn",
	"Florent Schiffer",
 	PERSPEHERE,
 	"Gabe Newell",
 	"Gabe Van Engel",
 	"Garret Rickey",
 	"Gary McTaggart",
 	"Gautam Babbar",
 	"Gray Horsfield",
 	"Greg Coomer",
	"Greg Towner",
 	"Gregory Sedgwick",
 	"Grégoire Cherlin",
 	"Hamish McKenzie",
	"Heather Campbell",
	"Henry Goffin",
 	"Ido Magal",
	TURRETBACK,
 	"Iestyn Bleasdale-Shepherd",
 	"Iikka Keränen",
 	"Isabelle Lemay",
 	"Ivan Simoncini",
 	"Jacob J. Smith",
 	"Jakob Jungels",
	"James Shin",
 	"Jane Lo",
	"Jared Christen",
 	"Jason Brashill",
 	"Jason Holtman",
 	"Jason Mitchell",
 	"Jason Ruymen",
 	"Jay Pinkerton",
 	"Jay Stelly",
 	"Jeep Barnett",
 	"Jeff Ballinger",
 	"Jeff Brown",
	"Jeff Hameluck",
 	"Jeff Lane",
 	"Jeff Lind",
 	"Jeff Sorensen",
 	"Jeff Unay",
 	"Jenny Stendahl",
 	"Jeremy Bennett",
 	"Jess Cliffe",
 	TURRET_S,
 	"Jim Hughes",
 	"Jim Murray",
 	"Jinwoo Lee",
 	"Joe Demers",
 	"Joe Han",
 	"Joe Ludwig",
 	"Joe Rohde",
 	"Joel Shoop",
 	"John Bartkiw",
 	"John Cook",
 	COMPCUBE,
 	"John Guthrie",
 	"John McCaskey",
 	"John Morello II",
 	"Jon Lippincott",
 	"Jonathan Sutton",
 	"Joshua Weier",
 	"Justin Lesamiz",
	"Justin Skinner",
 	"Karen Prell",
 	"Karl Whinnie",
 	"Keith Huggins",
 	"Keith Lango",
 	"Kelly Thornton",
	"Ken Banks",
 	"Ken Birdwell",
 	"Kerry Davis",
 	ORANGE,
 	"Kristopher Katz",
	"Kurtis Chinn",
 	"Kutta Srinivasan",
	"Kyle Davis",
 	"Kyle Monroe",
 	"Lars Jensvold",
 	"Laura Dubuk", 
	"Laure Lacascade",
 	"Liam Lavery",
 	"Lindsay Randall",
 	"Liz Cambridge",
 	"Marc Laidlaw",
 	"Marc Nagel",
 	"Marc Scaparro",
	"Marcus Egan",
 	"Mark Behm",
 	"Mark Richardson",
 	"Martin Otten",
 	"Matt Boone",
 	"Matt Campbell",
 	"Matt Charlesworth",
	"Matt Logue",
 	"Matt Nickerson",
 	"Matt Pritchard",
 	"Matt Rhoten",
 	"Matt T. Wood",
 	"Matt Wright",
	"Matthew An",
 	"Matthew Russell",
 	TURRETWIFE,
 	"Matthew Scott",
 	"Max Aristov",
 	"Mellissa Stanfield",
	"Michael Abrash",
 	"Michael Avon Oeming",
 	"Michael Booth",
 	"Michael Coupland",
	"Michael Marcus",
	"Michael Sartain",
 	"Mike Ambinder",
 	"Mike Belzer",
 	"Mike Blaszczak",
 	"Mike Dunkle",
 	"Mike Durand",
 	"Mike Morasky",
 	"Mike Stevens",
 	"Miles Estes",
 	"Milton Ngan",
 	MONSTER,
 	"Nate Heller",
 	"Neil Kaethler",
 	"Nick Maggiore",
 	"Noel McGinn",
 	"Olivier Nallet",
 	"Patrick McClard",
 	"Paul Graham",
 	"Paul G. Thuriot",
 	"Peter Konig",
 	"Phil Co",
 	"Phillip Saltzman",
 	"Pieter Wycoff",
 	BLUE,
 	"Pongthep Bank Charnchaichujit",
 	"Quintin Doroquez",
 	"Randy Lundeen",
	"Ray Ueno",
 	"Realm Lovejoy",
 	"Reuben Fries",
 	"Ricardo Ariza",
 	"Rich Geldreich",
 	"Rich Kaethler",
 	"Richard Lord",
 	"Rick Johnson",
 	"Robin Walker",
	"Roger Lundeen",
 	"Romy Hatfield",
 	"Ryan Thorlakson",
 	"Scott Dalton",
 	"Scott Ludwig",
 	TURRETBOX,
 	"Scott Lynch",
 	"Sergiy Migdalskiy",
 	"Shanon Drone",
 	"Shawn Zabecki",
 	"Stefan Landvogt",
	"Stephane Gaudette",
 	"Steve Bond",
 	"Steve Kalning",
 	"Taylor Sherman",
 	"Ted Backman",
 	"Ted Kosmatka",
 	"Ted Rivera",
 	"Tejeev Kohli",
 	PERSPEHERE,
 	"Thad Wharton",
 	"Thorsten Scheuermann",
 	"Tim Larkin",
 	"Tobin Buttram",
 	"Tom Bui",
 	"Tom Leonard",
 	"Torsten Zabka",
 	"Tristan Reidford",
	ENDOFHUMANSPREP, 	
	"Viktoria Dillon",
 	"Vitaliy Genkin",
 	"Wade Schin",
 	"William Bacon",
 	TURRET,
 	"Yahn Bernier",
 	"Yasser Malaika",
 	"Zach Eller",
 	"Zachary Franks",
 	"Zoid Kirsch",
 	ENDOFHUMANS,
	BLANK,
	//////////////////////////////////////////////////
	BLANK,
	BIRD,
	STATE_TEXTBLOCK,  // this switches modes to use a block of text from top to botom
 	VOICESTITLE,
	GLADOS,
	WHEATLEY,
	CAVE,
	ANNOUNCER,
	CCORESTUR,
	BOTSHUG,
	BLANK,
	BLANK,
	BLANK,
	STATE_TEXTBLOCK,  // this switches modes to use a block of text from top to botom
	MONSTER,
	SPACE,
	CHELL,
	VILIFY,
 	STILLALIVE,
	GONE, 
	VOICERECORD,
	TRANSLATIONS,
	BLANK,
	BLANK,
	BLANK,
	STATE_TEXTBLOCK,
	SPECIALTHANKS,
	"Alienware"  ,
	"ATI"  ,
	"Dell"  ,
	"Falcon Northwest"  ,
	"Havok",
	"Sam Gray",
	"Jamie Hunsdale", 
	"Photo credit of Earth: NASA",
	BLANK,
	BLANK,
	BLANK,
	STATE_TEXTBLOCK,
	SPACE,
	"[ THANK YOU         ]", 
	"[ FOR PARTICIPATING ]",
	"[ IN THIS           ]",
	"[ ENRICHMENT CENTER ]",
	"[ ACTIVITY!!        ]",
	BLANK,
	ENDING,
]

function CreditsThink()
{
	DBG = GetDeveloperLevel()

	if ( DO_CREDITS == 0 || m_bCreditsFinished == true )
	{	
		return
	}

	if ( m_nNameIndex >= m_nTotalNameCount )
	{	
		// the credits are over!!
		EndCredits()
		return
	}
	
	if ( m_nLastNameTime == 0 || m_nLastNameTime+TIME_INTERVAL < Time() )
	{
		m_nNameIndex++	
		m_nLastNameTime = Time()
	
		local nType = 0
		if ( m_nNameIndex <= m_nTotalNameCount )
		{
			if ( CreditsList[m_nNameIndex] == ENDING )
			{
				// the credits are over!!
				EndCredits()
				return
			}
			else if ( CreditsList[m_nNameIndex] == STATE_NAMES )
			{
				m_nStateChangeTo = 0
				UpdateVGUICreditsStateCommit()
			}
			else if ( CreditsList[m_nNameIndex] == STATE_TEXTBLOCK )
			{
				m_nStateChangeTo = 1
				UpdateVGUICreditsStateCommit()
			}	
			else if ( CreditsList[m_nNameIndex] == BLANK || CreditsList[m_nNameIndex] == ENDING ) 
			{
				// spawn a blank
			}
			else if ( CreditsList[m_nNameIndex] == ENDOFHUMANS ) 
			{
				// spawn a blank
				EntFireByHandle( GladosCredit[92], "Start", "", 0.00, null, null )
			}
			else if ( CreditsList[m_nNameIndex] == ENDOFHUMANSPREP ) 
			{
					EndofEmployees  = 1
					nType = 10
			}
			else if ( CreditsList[m_nNameIndex] == BLUE )
			{
				EntFire( "relay_monsterbox_walk", "Trigger", nType, 0)
				nType = 3
			}
			else if ( CreditsList[m_nNameIndex] == ORANGE )
			{
				nType = 4
			}
			else if ( CreditsList[m_nNameIndex] == TURRET )
			{
				nType = 5
			}
			else if ( CreditsList[m_nNameIndex] == TURRET_S )
			{
				nType = 6
			}
			else if ( CreditsList[m_nNameIndex] == CUBE )
			{
				nType = 7
			}
			else if ( CreditsList[m_nNameIndex] == MONSTER )
			{
				nType = 8
			}				
			else if ( CreditsList[m_nNameIndex] == BIRD )
			{
				nType = 9
			}	
			else if ( CreditsList[m_nNameIndex] == COMPCUBE )
			{
				nType = 10
			}	
			else if ( CreditsList[m_nNameIndex] == PERSPEHERE )
			{
				nType = 11
			}	
			else if ( CreditsList[m_nNameIndex] == TURRETBACK )
			{
				EntFire( "relay_orange_peek", "Trigger", "", 5)
				nType = 12
			}
			else if ( CreditsList[m_nNameIndex] == TURRETWIFE )
			{
				nType = 13
			}
			else if ( CreditsList[m_nNameIndex] == TURRETBOX )
			{
				nType = 14
			}	
			else if ( CreditsList[m_nNameIndex] == SPACE )
			{
				nType = 16
			}
			else if ( CreditsList[m_nNameIndex] == WHEATLEY )
			{
				nType = 17
			}
			else if ( CreditsList[m_nNameIndex] == STILLALIVE )
			{
				nType = 18
			}
			else if ( CreditsList[m_nNameIndex] == VOICERECORD || CreditsList[m_nNameIndex] == SPECIALTHANKS )
			{
				nType = 19
			}
			else if ( CreditsList[m_nNameIndex] == ANNOUNCER || CreditsList[m_nNameIndex] == TRANSLATIONS || CreditsList[m_nNameIndex] == VOICESTITLE )
			{
				nType = 20
			}
			else if ( CreditsList[m_nNameIndex] == CHELL )
			{
				nType = 21
			}
			else if ( CreditsList[m_nNameIndex] == CAVE )
			{
				nType = 22
			}	
			else if ( CreditsList[m_nNameIndex] == CCORESTUR )
			{
				nType = 23
			}	
			else if ( CreditsList[m_nNameIndex] == GLADOS )
			{
				nType = 24
			}
			else if ( CreditsList[m_nNameIndex] == BOTSHUG )
			{
				nType = 25
			}
			else if ( CreditsList[m_nNameIndex] == GONE )
			{
				nType = 26
			}
			else if ( CreditsList[m_nNameIndex] == VILIFY )
			{
				nType = 27
			}
			else
			{
				//if ( CreditsList[m_nNameIndex].find( ">" ) )

				nType = RandomInt( 1, 2 )
				
				if ( EndofEmployees == 0 )
				{
					// spawn a human
					local curTime = Time()
					if ( CreditTimer == 0 )
					{
						CreditTimer = Time() //tracks overall time in case we want a time limit and then fire the last one
						GladosRndStart = RandomInt( 0,5 )  //seed the first vo
						GladosInterval = Time()
					}
					if ( GladosRndStart < curTime-GladosInterval )
					{
						GladosCounter = GladosCounter + 1
						if ( GladosCounter <= GladosCredit.len() )
						{
							EntFireByHandle( GladosCredit[GladosCounter], "Start", "", 0.00, null, null )
							GladosInterval = Time()
							GladosRndStart = GladosLength[ GladosCounter ]+RandomFloat( 0, 7 )  //takes length of wav that already has .75 seconds added to it and randomally adds 0-7 seconds
						}
					}
				}
			}

			// send off a random job title	
			EntFire( "@command", "Command", "coop_set_credits_jobtitle " + "\"" + JobPrefix[RandomInt( 0, JobPrefix.len()-1 )] + " " +  JobMiddle[RandomInt( 0, JobMiddle.len()-1 )] + " " + JobSuffix[RandomInt( 0, JobSuffix.len()-1 )] + " <<\"", 0) 
		}
		
		if ( nType >= 16 )
		{
			// need to convert these to a string because hammer expects it!!!
			nType = "" + nType
			EntFire( "counter_pick_next_spawn2", "SetValue", nType, 0)
		}	
		else
		{
			// need to convert these to a string because hammer expects it!!!
			nType = "" + nType
			EntFire( "counter_pick_next_spawn", "SetValue", nType, 0)
		}

		if ( DBG )
		{
			printl("[ spawning ] - " + CreditsList[m_nNameIndex])
		}
	}
}

function CreditsGrantGameCompleteAchievement()
{
	local bGameCompleteB = true
	local bGameCompleteO = true
	for ( local i = 0; i < 5; i++ )
	{	
		if ( IsPlayerBranchComplete( 0, i ) == false )
		{
			bGameCompleteB = false
			//printl("!!=== bGameCompleteB = false --- not completed branch " + i )
		}
		if ( IsPlayerBranchComplete( 1, i ) == false )
		{
			bGameCompleteO = false
		}
	}
	
	if ( bGameCompleteB )
	{
		RecordAchievementEvent( "ACH.NEW_BLOOD", GetBluePlayerIndex() )	
		RecordAchievementEvent( "AV_SHIRT1", GetBluePlayerIndex() )	
		//printl("!!!!!!!!=== AWARDING END GAME ACHIEVEMENT!!!" )
	}
	if ( bGameCompleteO )
	{
		RecordAchievementEvent( "ACH.NEW_BLOOD", GetOrangePlayerIndex() )	
		RecordAchievementEvent( "AV_SHIRT1", GetOrangePlayerIndex() )	
	}
}

function DoCredits()
{
	if ( DBG )
	{
		printl("================DUMPING CREDITS")
	}
	
	// award the game complete achievmeent - this was moved from the last map because the toast caused the bink movie's sound to echo on the 360!
	CreditsGrantGameCompleteAchievement()

	// this clears out the list of names and initializes it
	AddCoopCreditsName( "CLEAR ALL" )

	local namecount = 0
	
	foreach( index, name in CreditsList )
	{	
		if ( DBG )
		{
			printl( index + " " + CreditsList[index] )
		}

		// skip over objects which we don't display on the vgui screen and we dont want the server to know about
		if (CreditsList[index] != TURRET && CreditsList[index] != TURRET_S &&
			CreditsList[index] != CUBE && CreditsList[index] != MONSTER &&
			CreditsList[index] != BLUE && CreditsList[index] != ORANGE &&
			CreditsList[index] != BLANK && CreditsList[index] != BIRD &&
			CreditsList[index] != COMPCUBE && CreditsList[index] != PERSPEHERE && 
			CreditsList[index] != STATE_NAMES && CreditsList[index] != STATE_TEXTBLOCK &&
			CreditsList[index] != TURRETBACK && CreditsList[index] != TURRETWIFE &&
			CreditsList[index] != TURRETBOX && CreditsList[index] != ENDING && CreditsList[index] != ENDOFHUMANS && CreditsList[index] != ENDOFHUMANSPREP)
		{
			// send the string to the server to build the list of names
			AddCoopCreditsName( CreditsList[index] )
			namecount++
		}

		m_nTotalNameCount++
	}
	
	if ( DBG )
	{
		printl( namecount + " names total." )
	
		printl("================END DUMP")
	}
	
	DO_CREDITS = 1

	if ( IsLocalSplitScreen() )
	{
		EntFire( "camera_O", "Disable", "", 1.0)
	}
}
 
function EndCredits()
{
	EntFire( "relay_end_confetti", "Trigger", "", 0)
	// disable the camera so the splitscreen player isn't stuck on spawning into the hub
	EntFire( "camera*", "Disable", "", 9.0)
	if ( GetNumPlayersConnected() > 1 )
	{
		EntFire( "playmovie_level_transition_to_hub", "PlayLevelTransitionMovie", "", 6.5)
		EntFire( "@command", "command", "changelevel mp_coop_lobby_2", 10)
	}
	else
	{
		EntFire( "fade_end_ss", "Fade", "", 6.5)
		EntFire( "@command", "command", "disconnect", 10)
	}
	m_bCreditsFinished = true
}

function AdvanceVGUIIndex( nType )
{
	// everything in the slot 16 and above are displayed on screen
	// slots 1 and 2 are humans
	if ( nType == 1 || nType == 2 || nType >= 16 )
	{
		// if we're human increment the index
		m_nVGUIIndex++
		if ( DBG )
		{
			printl( "[" + Time() + "] @@@@@@@@@ INCREMENTING INDEX TO: " + m_nVGUIIndex)
		}
	}
	
	EntFire( "@command", "Command", "coop_set_credits_index " + m_nVGUIIndex + " " + nType, 0)
}

function UpdateVGUICreditsStateNotify( nState )
{
	m_nStateChangeIn = 1
	m_nStateChangeTo = nState	
}

function UpdateVGUICreditsStateCommit()
{
	m_nCreditsState	= m_nStateChangeTo
	if ( DBG )
	{
		printl( "[" + Time() + "] @@@@@@@@@ UPDATE STATE TO: " + m_nCreditsState + " : Index: " + m_nVGUIIndex)
	}
	EntFire( "@command", "Command", "coop_set_credits_state " + m_nCreditsState, 0)
	if ( m_nCreditsState == 1 )
	{
		// slow thigns down
		TIME_INTERVAL <- BLOCKTEXT_TIME_INTERVAL
		EntFire( "train_track_1", "EnableAlternatePath", "", 0)
	}
	else
	{
		// slow thigns down
		TIME_INTERVAL <- NAME_TIME_INTERVAL
		EntFire( "train_track_1", "DisableAlternatePath", "", 0)
	}
}

function ScanItem( nType )
{
	AdvanceVGUIIndex( nType )

	// dont scan for the big items like glados, cave portrait, etc
	// everything in the slot 16 and above should not be scanned
	if ( nType >= 16 )
	{
		return
	}
		
	// need to convert these to a string because hammer expects it!!!
	nType = "" + nType
	
	if ( m_nCreditsState == 1 )
	{		
		EntFire( "relay_scan", "Trigger", "", 0)
	}
	else
	{
		EntFire( "relay_scan_fast", "Trigger", "", 0)
	}
}

JobPrefix <- [
"Lead",
"Senior",
"Direct",
"Dynamic",
"Future",
"National",
"Regional",
"Central",
"Global",
"Dynamic",
"International",
"Legacy",
"Forward",
"Internal",
"Chief",
"Principal",
"Postdoctoral",
"Regulatory",
]

JobMiddle <- [
"Human",
"Environmental",
"Aerospace",
"Space",
"Deep Sea",
"Atmospheric",
"Cardiovascular",
"Electrical",
"Computer",
"Emergency",
"Mining",
"Nuclear",
"Safety",
"Histology",
"Forensic",
]

JobSuffix <- [
"Surgeon",
"Scientist",
"Engineer",
"Technologist",
"Neurosurgeon",
"Pilot",
"Astronaut",
"Archeologist",
"Aviator",
"Specialist",
"Psychologist",
"Composer",
"Fighter",
"Professional",
"Geographer",
"Architect",
"Astronomer",
"Cytogeneticist",
"Dentist",
"Interpreter",
"Phlebotomist",
"Physician",
"Meteorologist",
"Philosopher",
"Garbologist",
]

GladosCredit <- {}
GladosCredit[1] <- CreateSceneEntity("scenes/npc/glados/mp_credits01.vcd")
GladosCredit[2] <- CreateSceneEntity("scenes/npc/glados/mp_credits03.vcd")
GladosCredit[3] <- CreateSceneEntity("scenes/npc/glados/mp_credits04.vcd")
GladosCredit[4] <- CreateSceneEntity("scenes/npc/glados/mp_credits05.vcd")
GladosCredit[5] <- CreateSceneEntity("scenes/npc/glados/mp_credits06.vcd")
GladosCredit[6] <- CreateSceneEntity("scenes/npc/glados/mp_credits07.vcd")
GladosCredit[7] <- CreateSceneEntity("scenes/npc/glados/mp_credits08.vcd")
GladosCredit[8] <- CreateSceneEntity("scenes/npc/glados/mp_credits09.vcd")
GladosCredit[9] <- CreateSceneEntity("scenes/npc/glados/mp_credits10.vcd")
GladosCredit[10] <- CreateSceneEntity("scenes/npc/glados/mp_credits11.vcd")
GladosCredit[11] <- CreateSceneEntity("scenes/npc/glados/mp_credits12.vcd")
GladosCredit[12] <- CreateSceneEntity("scenes/npc/glados/mp_credits13.vcd")
GladosCredit[13] <- CreateSceneEntity("scenes/npc/glados/mp_credits14.vcd")
GladosCredit[14] <- CreateSceneEntity("scenes/npc/glados/mp_credits15.vcd")
GladosCredit[15] <- CreateSceneEntity("scenes/npc/glados/mp_credits17.vcd")
GladosCredit[16] <- CreateSceneEntity("scenes/npc/glados/mp_credits16.vcd")
GladosCredit[17] <- CreateSceneEntity("scenes/npc/glados/mp_credits18.vcd")
GladosCredit[18] <- CreateSceneEntity("scenes/npc/glados/mp_credits19.vcd")
GladosCredit[19] <- CreateSceneEntity("scenes/npc/glados/mp_credits20.vcd")
GladosCredit[20] <- CreateSceneEntity("scenes/npc/glados/mp_credits21.vcd")
GladosCredit[21] <- CreateSceneEntity("scenes/npc/glados/mp_credits22.vcd")
GladosCredit[22] <- CreateSceneEntity("scenes/npc/glados/mp_credits23.vcd")
GladosCredit[23] <- CreateSceneEntity("scenes/npc/glados/mp_credits24.vcd")
GladosCredit[24] <- CreateSceneEntity("scenes/npc/glados/mp_credits26.vcd")
GladosCredit[25] <- CreateSceneEntity("scenes/npc/glados/mp_credits25.vcd")
GladosCredit[26] <- CreateSceneEntity("scenes/npc/glados/mp_credits27.vcd")
GladosCredit[27] <- CreateSceneEntity("scenes/npc/glados/mp_credits28.vcd")
GladosCredit[28] <- CreateSceneEntity("scenes/npc/glados/mp_credits29.vcd")
GladosCredit[29] <- CreateSceneEntity("scenes/npc/glados/mp_credits30.vcd")
GladosCredit[30] <- CreateSceneEntity("scenes/npc/glados/mp_credits31.vcd")
GladosCredit[31] <- CreateSceneEntity("scenes/npc/glados/mp_credits32.vcd")
GladosCredit[32] <- CreateSceneEntity("scenes/npc/glados/mp_credits33.vcd")
GladosCredit[33] <- CreateSceneEntity("scenes/npc/glados/mp_credits34.vcd")
GladosCredit[34] <- CreateSceneEntity("scenes/npc/glados/mp_credits35.vcd")
GladosCredit[35] <- CreateSceneEntity("scenes/npc/glados/mp_credits36.vcd")
GladosCredit[36] <- CreateSceneEntity("scenes/npc/glados/mp_credits37.vcd")
GladosCredit[37] <- CreateSceneEntity("scenes/npc/glados/mp_credits38.vcd")
GladosCredit[38] <- CreateSceneEntity("scenes/npc/glados/mp_credits39.vcd")
GladosCredit[39] <- CreateSceneEntity("scenes/npc/glados/mp_credits40.vcd")
GladosCredit[40] <- CreateSceneEntity("scenes/npc/glados/mp_credits41.vcd")
GladosCredit[41] <- CreateSceneEntity("scenes/npc/glados/mp_credits42.vcd")
GladosCredit[42] <- CreateSceneEntity("scenes/npc/glados/mp_credits43.vcd")
GladosCredit[43] <- CreateSceneEntity("scenes/npc/glados/mp_credits44.vcd")
GladosCredit[44] <- CreateSceneEntity("scenes/npc/glados/mp_credits45.vcd")
GladosCredit[45] <- CreateSceneEntity("scenes/npc/glados/mp_credits46.vcd")
GladosCredit[46] <- CreateSceneEntity("scenes/npc/glados/mp_credits47.vcd")
GladosCredit[47] <- CreateSceneEntity("scenes/npc/glados/mp_credits48.vcd")
GladosCredit[48] <- CreateSceneEntity("scenes/npc/glados/mp_credits49.vcd")
GladosCredit[49] <- CreateSceneEntity("scenes/npc/glados/mp_credits50.vcd")
GladosCredit[50] <- CreateSceneEntity("scenes/npc/glados/mp_credits51.vcd")
GladosCredit[51] <- CreateSceneEntity("scenes/npc/glados/mp_credits52.vcd")
GladosCredit[52] <- CreateSceneEntity("scenes/npc/glados/mp_credits53.vcd")
GladosCredit[53] <- CreateSceneEntity("scenes/npc/glados/mp_credits54.vcd")
GladosCredit[54] <- CreateSceneEntity("scenes/npc/glados/mp_credits55.vcd")
GladosCredit[55] <- CreateSceneEntity("scenes/npc/glados/mp_credits56.vcd")
GladosCredit[56] <- CreateSceneEntity("scenes/npc/glados/mp_credits57.vcd")
GladosCredit[57] <- CreateSceneEntity("scenes/npc/glados/mp_credits58.vcd")
GladosCredit[58] <- CreateSceneEntity("scenes/npc/glados/mp_credits59.vcd")
GladosCredit[59] <- CreateSceneEntity("scenes/npc/glados/mp_credits60.vcd")
GladosCredit[60] <- CreateSceneEntity("scenes/npc/glados/mp_credits61.vcd")
GladosCredit[61] <- CreateSceneEntity("scenes/npc/glados/mp_credits62.vcd")
GladosCredit[62] <- CreateSceneEntity("scenes/npc/glados/mp_credits63.vcd")
GladosCredit[63] <- CreateSceneEntity("scenes/npc/glados/mp_credits64.vcd")
GladosCredit[64] <- CreateSceneEntity("scenes/npc/glados/mp_credits65.vcd")
GladosCredit[65] <- CreateSceneEntity("scenes/npc/glados/mp_credits66.vcd")
GladosCredit[66] <- CreateSceneEntity("scenes/npc/glados/mp_credits67.vcd")
GladosCredit[67] <- CreateSceneEntity("scenes/npc/glados/mp_credits68.vcd")
GladosCredit[68] <- CreateSceneEntity("scenes/npc/glados/mp_credits69.vcd")
GladosCredit[69] <- CreateSceneEntity("scenes/npc/glados/mp_credits70.vcd")
GladosCredit[70] <- CreateSceneEntity("scenes/npc/glados/mp_credits71.vcd")
GladosCredit[71] <- CreateSceneEntity("scenes/npc/glados/mp_credits72.vcd")
GladosCredit[72] <- CreateSceneEntity("scenes/npc/glados/mp_credits73.vcd")
GladosCredit[73] <- CreateSceneEntity("scenes/npc/glados/mp_credits74.vcd")
GladosCredit[74] <- CreateSceneEntity("scenes/npc/glados/mp_credits75.vcd")
GladosCredit[75] <- CreateSceneEntity("scenes/npc/glados/mp_credits76.vcd")
GladosCredit[76] <- CreateSceneEntity("scenes/npc/glados/mp_credits77.vcd")
GladosCredit[77] <- CreateSceneEntity("scenes/npc/glados/mp_credits79.vcd")
GladosCredit[78] <- CreateSceneEntity("scenes/npc/glados/mp_credits80.vcd")
GladosCredit[79] <- CreateSceneEntity("scenes/npc/glados/mp_credits81.vcd")
GladosCredit[80] <- CreateSceneEntity("scenes/npc/glados/mp_credits82.vcd")
GladosCredit[81] <- CreateSceneEntity("scenes/npc/glados/mp_credits83.vcd")
GladosCredit[82] <- CreateSceneEntity("scenes/npc/glados/mp_credits84.vcd")
GladosCredit[83] <- CreateSceneEntity("scenes/npc/glados/mp_credits85.vcd")
GladosCredit[84] <- CreateSceneEntity("scenes/npc/glados/mp_credits86.vcd")
GladosCredit[85] <- CreateSceneEntity("scenes/npc/glados/mp_credits87.vcd")
GladosCredit[86] <- CreateSceneEntity("scenes/npc/glados/mp_credits88.vcd")
GladosCredit[87] <- CreateSceneEntity("scenes/npc/glados/mp_credits89.vcd")
GladosCredit[88] <- CreateSceneEntity("scenes/npc/glados/mp_credits90.vcd")
GladosCredit[89] <- CreateSceneEntity("scenes/npc/glados/mp_credits91.vcd")
GladosCredit[90] <- CreateSceneEntity("scenes/npc/glados/mp_credits92.vcd")
GladosCredit[91] <- CreateSceneEntity("scenes/npc/glados/mp_credits93.vcd")
GladosCredit[92] <- CreateSceneEntity("scenes/npc/glados/mp_credits94.vcd")
GladosLength <- {}
GladosLength[1] <- 2.238
GladosLength[2] <- 2.011
GladosLength[3] <- 2.546
GladosLength[4] <- 2.310
GladosLength[5] <- 2.112
GladosLength[6] <- 2.191
GladosLength[7] <- 2.083
GladosLength[8] <- 2.350
GladosLength[9] <- 2.774
GladosLength[10] <- 2.132
GladosLength[11] <- 3.697
GladosLength[12] <- 1.850
GladosLength[13] <- 2.743
GladosLength[14] <- 2.528
GladosLength[15] <- 1.550
GladosLength[16] <- 2.182
GladosLength[17] <- 2.364
GladosLength[18] <- 1.642
GladosLength[19] <- 1.442
GladosLength[20] <- 2.404
GladosLength[21] <- 2.421
GladosLength[22] <- 1.814
GladosLength[23] <- 3.402
GladosLength[24] <- 1.374
GladosLength[25] <- 2.602
GladosLength[26] <- 2.330
GladosLength[27] <- 4.095
GladosLength[28] <- 2.436
GladosLength[29] <- 1.678
GladosLength[30] <- 2.200
GladosLength[31] <- 1.534
GladosLength[32] <- 3.850
GladosLength[33] <- 3.357
GladosLength[34] <- 1.930
GladosLength[35] <- 1.876
GladosLength[36] <- 2.566
GladosLength[37] <- 6.999
GladosLength[38] <- 2.428
GladosLength[39] <- 2.355
GladosLength[40] <- 3.151
GladosLength[41] <- 1.776
GladosLength[42] <- 2.647
GladosLength[43] <- 1.813
GladosLength[44] <- 2.496
GladosLength[45] <- 6.536
GladosLength[46] <- 5.198
GladosLength[47] <- 4.806
GladosLength[48] <- 4.462
GladosLength[49] <- 1.453
GladosLength[50] <- 2.433
GladosLength[51] <- 1.625
GladosLength[52] <- 4.666
GladosLength[53] <- 1.918
GladosLength[54] <- 8.409
GladosLength[55] <- 4.853
GladosLength[56] <- 2.266
GladosLength[57] <- 2.915
GladosLength[58] <- 2.296
GladosLength[59] <- 1.561
GladosLength[60] <- 3.807
GladosLength[61] <- 2.570
GladosLength[62] <- 6.204
GladosLength[63] <- 3.022
GladosLength[64] <- 2.707
GladosLength[65] <- 2.452
GladosLength[66] <- 2.673
GladosLength[67] <- 2.105
GladosLength[68] <- 3.021
GladosLength[69] <- 3.291
GladosLength[70] <- 4.186
GladosLength[71] <- 2.292
GladosLength[72] <- 2.081
GladosLength[73] <- 2.326
GladosLength[74] <- 2.696
GladosLength[75] <- 1.462
GladosLength[76] <- 1.892
GladosLength[77] <- 1.724
GladosLength[78] <- 2.634
GladosLength[79] <- 2.163
GladosLength[80] <- 2.238
GladosLength[81] <- 1.918
GladosLength[82] <- 1.861
GladosLength[83] <- 1.938
GladosLength[84] <- 2.025
GladosLength[85] <- 2.223
GladosLength[86] <- 2.311
GladosLength[87] <- 3.865
GladosLength[88] <- 2.348
GladosLength[89] <- 2.597
GladosLength[90] <- 5.989
GladosLength[91] <- 2.921
GladosLength[92] <- 5.939
