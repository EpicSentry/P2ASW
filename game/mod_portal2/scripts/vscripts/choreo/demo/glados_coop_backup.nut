BluePingCount <- 0	
BlueBadPingCount <- 0
BlueLastPing <- Time()
BlueInPortalTraining <-0
OrangePingCount <- 0	
OrangeLastPing <- Time()
OrangeBadPingCount <- 0
OrangeInPortalTraining <-0
BlueHasGun <-0
OrangeHasGun <-0
//ispaused()
//IsPlayingBack()
//EstimateLength()

	lastthink <- Time()
	startBlock <- 0

//Next VCD in scene chain info
	waitSceneStart <- 0
	waiting <- 0  //1 means we're waiting for the current vcd to finish so we can play the next vcd in the chain
	waitStartTime <- Time()
	waitLength <- Time()
	waitNext <- null //the SceneTable id for the next scene in the current chain.
	waitNoDingOff <- false
	waitNoDingOn <- false
	waitEndSpecial <- 0
	waitEntityFire <- null
	waitEntityInput <- null
	waitEntityParm <- null
	waitEntityDelay <- null

//PitchShifting stuff
	pitchShifting <- false
	pitchShiftLastThink <- Time()
	pitchShiftInterval <- 1.0
	pitchShiftValue <- 1.0
	pitchOverride <- null

//State Flags
	stateSlowFieldTraining <- false

//debug stuff
	debug <- false	//Set true to enable debug block during think function
	debugInterval <- 2.00
	debugAccrued <- 0.00
	debugMicTest <- false

//Dings
	sceneDingOn  <- CreateSceneEntity("scenes/npc/glados_manual/ding_on.vcd")
	sceneDingOff <- CreateSceneEntity("scenes/npc/glados_manual/ding_off.vcd")
	
	DoIncludeScript( "choreo/glados_coop_scenetable_include", self.GetScriptScope() )
	DoIncludeScript( "choreo/glados_coop_scenetable_include_manual", self.GetScriptScope() )
	

//Players dropping boxes
	badCatchBlue  <- CreateSceneEntity("scenes/npc/glados/coop_misc09.vcd")
	badCatchOrange <- CreateSceneEntity("scenes/npc/glados/coop_misc10.vcd")
	
//Honest compliments
	ComplimentSceneTable <- {}
	ComplimentSceneTable[0] <- CreateSceneEntity("scenes/npc/glados/coop_score_comment01.vcd")
	ComplimentSceneTable[1] <- CreateSceneEntity("scenes/npc/glados/coop_score_comment04.vcd")
	ComplimentSceneTable[2] <- CreateSceneEntity("scenes/npc/glados/coop_score_comment05.vcd")
	ComplimentSceneTable[3] <- CreateSceneEntity("scenes/npc/glados/coop_score_comment06.vcd")
	ComplimentSceneTable[4] <- CreateSceneEntity("scenes/npc/glados/coop_score_comment07.vcd")

//Co-op sepcific stuff
	coopFirstPlayerToGetGun <- 0 //Saves id of first player to pick up portal gun
	coopBlue <- 2 //id of blue player
	coopOrange <- 1 //id of orange player
	coopTalkIDBlue <-1 //Blue id for IsPlayerSpeaking() - There's some weirdness where it may not be the same as coopBlue  
	coopTalkIDOrange <-2 //Orange id for IsPlayerSpeaking() - There's some weirdness where it may not be the same as coopOrange
	coopWaitingForGetGun <- null //Waiting for the second player to pick up the portal gun
	coopRelaxationVaultVoiceMonitor <- false //true = GladosThink() should monitor for player speech
	coopRelaxationVaultVoiceBlueTicks <- 0 //Number of times GladosThink() has detected Blue speaking
	coopRelaxationVaultVoiceOrangeTicks <- 0 //Number of times GladosThink() has detected Blue speaking
	coopRelaxationVaultthreshold <- 20 //number of times in GladosThink that a player has to talk before it is an interruption
	coopSilencethreshold <- 1.0 //number of seconds in GladosThink that both players have to remain silent before it is considered a silence
	coopSilenceTicks <- 0 //Number of consecutive GladosThink() calls during which neither player was speaking
	coopSayOnSilence <- 0 //if >0: When coopSilenceTicks>=coopSilencethreshold, GladosPlayVcd(coopSayOnSilence)
	coopWaitingToExit <- false
//Co-op Ping Training Stuff
	coopPingGameOn <- false
	coopPingPlayerTurn <- 0
	coopPingTurnNumber <- 0
	coopPingNextScene <- null
	coopPingNextSceneDelay <- null
	coopWrongMovesBlue <- 0
	coopWrongMovesOrange <- 0
	coopSaidWrongMovesDialog <- false
	coopPingsOrange <- 0
	coopPingsBlue <- 0
	coopPingNoInterrupt <- false
	coopTimeSinceTurn <- Time()
	coopSilenceBlahsPlayed <- 0
	coopTriggeredElevator <- false
	
//Score Stating Stuff
	coopScoreToState <- null
	 

//BlahBlahBlah vcds
	BlahVcds <- {}
	BlahVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH34.vcd")
	BlahVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH17.vcd")
	BlahVcds[3] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH18.vcd")
	BlahVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH19.vcd")
	BlahVcds[5] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH20.vcd")
	BlahVcds[6] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH21.vcd")
	BlahVcds[7] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH36.vcd")
	BlahVcds[8] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BOTH37.vcd")

//Ping Training Vcds
	PingTrainingVcds <- {}
	//[1]Orange, it’s your turn.
	PingTrainingVcds[1] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt06.vcd")
	//[2]Nice move!
	PingTrainingVcds[2] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt07.vcd")
	//[3]Blue: your turn again.
	PingTrainingVcds[3] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt08.vcd")
	//[4]Phenomenal!
	PingTrainingVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORE_COMMENT04.vcd")
	//[5]Orange, take your turn.
	PingTrainingVcds[5] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt10.vcd")
	//[6]Really? You’re just going to blatantly cheat?
	PingTrainingVcds[6] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt11.vcd")
	//[7]Allright. I’m bringing this farce to a close.
	PingTrainingVcds[7] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt12.vcd")
	//[8]Game over. Back to testing.
	PingTrainingVcds[8] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt13.vcd")
	//[9]Place a ping marker on the board.
	PingTrainingVcds[9] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt14.vcd")
	//[10]No, not a portal. Place a ping marker. They’re different.
	PingTrainingVcds[10] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt15.vcd")
	//[11]The Tic-Tac-Toe-Two turn sequence was designed for your safety. Do not go out of turn.
	PingTrainingVcds[11] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt16.vcd")
	//[12]Allright. I’m going to bring this farce to a close before someone sustains a serious Tic-Tac-Toe-Two injury.
	PingTrainingVcds[12] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt17.vcd")
	//[13]Blue, it’s not your turn.
	PingTrainingVcds[13] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt18.vcd")
	//[14]Orange, it’s not your turn.
	PingTrainingVcds[14] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt19.vcd")
	//[15]Blue, make a move.
	PingTrainingVcds[15] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt20.vcd")
	//[16]Orange, make a move.
	PingTrainingVcds[16] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt21.vcd")
	//[17]We're all waiting, Blue...
	PingTrainingVcds[17] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt22.vcd")
	//[18]We're all waiting, Orange...
	PingTrainingVcds[18] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt23.vcd")
	//[19]Oooh.Okay.
	PingTrainingVcds[19] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt24.vcd")
	//[20]hmmmm. well. Okay. I guess.
	PingTrainingVcds[20] <- CreateSceneEntity("scenes/npc/glados/coop_teamwork_exercise_alt25.vcd")

//Score Prefix Vcds
	ScorePrefixVcds <- {}
	//[1]Blue is penalized
	ScorePrefixVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING01.vcd")
	//[2]Orange is penalized
	ScorePrefixVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING02.vcd")
	//[3]Blue receives
	ScorePrefixVcds[3] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING06.vcd")
	//[4]Orange receives
	ScorePrefixVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING07.vcd")
	//[5]Blue, your score is now
	ScorePrefixVcds[5] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING04.vcd")
	//[6]Orange, your score is now
	ScorePrefixVcds[6] <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING05.vcd")

//PrivateTalk mix vcds
	PrivateMixVcds <- {}
	PrivateMixVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX01.vcd")
	PrivateMixVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX02.vcd")
	PrivateMixVcds[3] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX03.vcd")
	PrivateMixVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX04.vcd")
	PrivateMixVcds[5] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX05.vcd")
	PrivateMixVcds[6] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX06.vcd")
	PrivateMixVcds[7] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX07.vcd")
	PrivateMixVcds[8] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_MIX08.vcd")

//PrivateTalk Both vcds
	PrivateBothVcds <- {}
	PrivateBothVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH01.vcd")
	PrivateBothVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH02.vcd")
	PrivateBothVcds[3] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH03.vcd")
	PrivateBothVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH04.vcd")
	PrivateBothVcds[5] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH05.vcd")
	PrivateBothVcds[6] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH06.vcd")
	PrivateBothVcds[7] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH07.vcd")
	PrivateBothVcds[8] <- CreateSceneEntity("scenes/npc/glados/COOP_PRIVATETALK_BOTH08.vcd")
	
//HelloVcds	
	HelloVcds <- {}
	HelloVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_HELLO01.vcd")
	HelloVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_HELLO02.vcd")


//Numbers Vcds
	NumberVcds <- {}
	//[0]zero
	NumberVcds[0] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS20.vcd")
	//[1]one
	NumberVcds[1] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS11.vcd")
	//[2]two
	NumberVcds[2] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS12.vcd")
	//[3]three
	NumberVcds[3] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS13.vcd")
	//[4]four
	NumberVcds[4] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS14.vcd")
	//[5]five
	NumberVcds[5] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS15.vcd")
	//[6]six
	NumberVcds[6] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS16.vcd")
	//[7]seven
	NumberVcds[7] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS17.vcd")
	//[8]eight
	NumberVcds[8] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS18.vcd")
	//[9]nine
	NumberVcds[9] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS19.vcd")
	//[20]twenty
	NumberVcds[20] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS01.vcd")
	//[30]thirty
	NumberVcds[30] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS02.vcd")
	//[40]forty
	NumberVcds[40] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS03.vcd")
	//[50]fifty
	NumberVcds[50] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS04.vcd")
	//[60]sixty
	NumberVcds[60] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS05.vcd")
	//[70]seventy
	NumberVcds[70] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS06.vcd")
	//[80]eighty
	NumberVcds[80] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS07.vcd")
	//[90]ninety
	NumberVcds[90] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS08.vcd")
	//[100]hundred and
	NumberVcds[100] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS09.vcd")
	//[1000]thousand and
	NumberVcds[1000] <- CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS10.vcd")
	
//Science Collaboration Points
	ScienceCollaborationPointsVcd <- CreateSceneEntity("scenes/npc/glados/COOP_SCORING03.vcd")


function PlayNextScene()
{
	local i = 0
	//printl("===================Scene Done!" + i)
	//Check for any special processing tagged for this block
	if (waitEndSpecial>0)
	{
		switch (waitEndSpecial)
		{
			case 1: //Initial Relaxation Vault Portal Opening
				//printl("===================>Special End PROCESSING!!!!!!!")
				EntFire("player_2_relay_open_portals","Trigger", "", 0.00)
				break
		}
		waitEndSpecial = 0
	}
	if (waitEntityFire != null)
	{
		//printl("===================" + waitEntityFire+ " "+waitEntityInput+" "+waitEntityDelay)
		if (waitEntityParm != null)
			EntFire(waitEntityFire,waitEntityInput,waitEntityParm, waitEntityDelay)
		else
			EntFire(waitEntityFire,waitEntityInput,"", waitEntityDelay)
		waitEntityFire = null
		waitEntityInput = null
		waitEntityDelay = null
		waitEntityParm = null
	}
	if (coopWaitingForGetGun != null)
	{
		if (coopWaitingForGetGun == coopBlue)
		{
			coopWaitingForGetGun = null
			GladosPlayVcd(3)
		}
		else
		{
			coopWaitingForGetGun = null
			GladosPlayVcd(4)
		}
		return
	} 
	
	if (waitNext != null)
	{
		if (waitLength == null)
		{
			i+=1
			//printl("===================Ready to play:" + i)
			GladosPlayVcd(null)
		}	
		else
		{
			waitStartTime = Time()
			waiting = 1
		}	
	}
	else
	{
		waitSceneStart = 0
		if (!waitNoDingOff)
			EntFireByHandle( sceneDingOff, "Start", "", 1.0, null, null )
			//EntFireByHandle( sceneDingOff, "Start", "", 0.2, null, null )
	}
}

// OnPostSpawn
function OnPostSpawn()
{
		//Bind the PlayNextScene function in all our SceneEntities to "this"
		foreach (val in SceneTable)
		{
			val.vcd.ValidateScriptScope()
			val.vcd.GetScriptScope().PlayNextScene <- PlayNextScene.bindenv(this)
			val.vcd.ConnectOutput( "OnCompletion", "PlayNextScene" )
		}
		//Bind the ping training vcds to GLaDOS' handler		
		foreach (val in PingTrainingVcds)
		{
			val.ValidateScriptScope()
			val.GetScriptScope().GladosPingTrainingNextScene <- GladosPingTrainingNextScene.bindenv(this)
			val.ConnectOutput( "OnCompletion", "GladosPingTrainingNextScene" )
		}		
		//Bind the ScorePrefix vcds to GLaDOS' handler		
		foreach (val in ScorePrefixVcds)
		{
			val.ValidateScriptScope()
			val.GetScriptScope().GladosStateScore <- GladosStateScore.bindenv(this)
			val.ConnectOutput( "OnCompletion", "GladosStateScore" )
		}
		//Bind number vcds to handler		
		foreach (val in NumberVcds)
		{
			val.ValidateScriptScope()
			val.GetScriptScope().GladosStateScore <- GladosStateScore.bindenv(this)
			val.ConnectOutput( "OnCompletion", "GladosStateScore" )
		}
}

//Think Function
function GladosThink()
{
	local curTime=Time()
	local thinkInterval = curTime-lastthink
	lastthink = curTime
	//Put debug stuff here!
	if (debug)
	{
		debugAccrued += thinkInterval
		if (debugAccrued>debugInterval)
		{
			debugAccrued = 0
			printl("===================GladosNEWThink-> " + lastthink+" "+thinkInterval)
		}
	}
	//Microphone Test
	if (debugMicTest)
	{
		local OrangeIndex = GetOrangePlayerIndex()
		local BlueIndex = GetBluePlayerIndex()
		if (PlayerVoiceListener.IsPlayerSpeaking(OrangeIndex))
		{
			EntFire("player_1_talking_relay","Trigger", "", 0.00)
		}
		else
		{
			EntFire("player_1_not_talking_relay","Trigger", "", 0.00)
		}
		if (PlayerVoiceListener.IsPlayerSpeaking(BlueIndex))
		{
			EntFire("player_2_talking_relay","Trigger", "", 0.00)
		}
		else
		{
			EntFire("player_2_not_talking_relay","Trigger", "", 0.00)
		}
	}
	//Waiting to exit level
	if (coopWaitingToExit)
	{
		local curscene = self.GetCurrentScene()
		if ( curscene == null && waitNext == null)
		{
			EntFire("gladosendoflevelrelay","Trigger", "", 2.50)  //Trigger the end of level transition
		}
	}
	//Waiting for player to take turn in ping training
	if (coopPingGameOn && ((curTime - coopTimeSinceTurn)>20))
	{
		if (!coopPingNoInterrupt)
		{
			coopTimeSinceTurn = Time()
			if (coopPingPlayerTurn == coopBlue)
			{
				GladosPingTrainingSpeak(17,false,0.00)
			}
			else
			{
				GladosPingTrainingSpeak(18,false,0.00)
			}
		}	
	} 
	//Wait for a break in the player's talking to say a line
	if (coopSayOnSilence)
	{
		local OrangeIndex = GetOrangePlayerIndex()
		local BlueIndex = GetBluePlayerIndex()
		
		local curscene = self.GetCurrentScene()
		if ( curscene == null )
		{
			EntFireByHandle(BlahVcds[RandomInt(1,8)], "Start", "", 0.00, null, null )
			coopSilenceBlahsPlayed += 1
		}
		local lSilence = true
		if (PlayerVoiceListener.IsPlayerSpeaking(OrangeIndex)) lSilence = false
		if (PlayerVoiceListener.IsPlayerSpeaking(BlueIndex)) lSilence = false
		if (lSilence)
			coopSilenceTicks += thinkInterval
		else
			coopSilenceTicks = 0
		if (coopSilenceTicks >= coopSilencethreshold || coopSilenceBlahsPlayed > 5)
		{
			local i = coopSayOnSilence
			coopSayOnSilence = 0
			GladosPlayVcd(i)
		}		
	}
	//Check for players talking over relaxation vault speech.
	if (coopRelaxationVaultVoiceMonitor)
	{
		local OrangeIndex = GetOrangePlayerIndex()
		local BlueIndex = GetBluePlayerIndex()
		if (PlayerVoiceListener.IsPlayerSpeaking(OrangeIndex))
		{
			coopRelaxationVaultVoiceOrangeTicks += 1
			if (coopRelaxationVaultVoiceOrangeTicks > coopRelaxationVaultthreshold)
			{
				coopRelaxationVaultVoiceMonitor = false
				waitNext = null
				waitNoDingOff = true
				local curscene = self.GetCurrentScene()
				if ( curscene != null )
				{
					EntFireByHandle( curscene, "Cancel", "", 0, null, null )
					waitNext = null
					waitNoDingOff = true
				}
				coopSayOnSilence = 6
			}
			//printl("===================ORANGE TALKING")
			//speedDuration = PlayerVoiceListener.GetSpeechDuration( ORANGE_PLAYER );
		}
		else
		{
			if (coopRelaxationVaultVoiceOrangeTicks > 0) coopRelaxationVaultVoiceOrangeTicks -= 1
		}
		if (PlayerVoiceListener.IsPlayerSpeaking(BlueIndex))
		{
			coopRelaxationVaultVoiceBlueTicks += 1
			if (coopRelaxationVaultVoiceBlueTicks > coopRelaxationVaultthreshold)
			{
				coopRelaxationVaultVoiceMonitor = false
				waitNext = null
				waitNoDingOff = true
				local curscene = self.GetCurrentScene()
				if ( curscene != null )
				{
					EntFireByHandle( curscene, "Cancel", "", 0, null, null )
					waitNext = null
					waitNoDingOff = true
				}
				coopSayOnSilence = 5
			}
		}
		else
		{
			if (coopRelaxationVaultVoiceBlueTicks > 0) coopRelaxationVaultVoiceBlueTicks -= 1
		}
	}	
	//Are we PitchShifting?
	if (pitchShifting)
	{
		if (Time()-pitchShiftLastThink>pitchShiftInterval)
		{
			local curscene = self.GetCurrentScene()
			if ( curscene != null )
			{ 
				local shiftAmount = RandomFloat( -0.10, 0.10)
				if (shiftAmount<0.00)
					shiftAmount=shiftAmount*1.5
				pitchShiftValue += shiftAmount
				if ((pitchShiftValue <= 0.0) || (pitchShiftValue >= 1.7) )
					pitchShiftValue -= (shiftAmount*2)
				EntFireByHandle( curscene, "PitchShift", pitchShiftValue.tostring(), 0, null, null )
				pitchShiftInterval = RandomFloat( 0.1, 0.2)
			}
			//Set Lastthink
			pitchShiftLastThink = Time()
		}
	}	
	//Are we waiting to play another vcd?
	if (waiting == 1)
	{
		if (Time()-waitStartTime >= waitLength)
		{
			waiting = 0
			GladosPlayVcd(null)
		}
	}
}

//Play a vcd from the SceneTable, plus set up next line (if any)
function GladosPlayVcd(arg)
//arg==integer -> Start playing new scene (scene being a chain of vcds) from SceneTable[SceneTableLookup[arg]]
//arg==string  -> Start playing new scene (scene being a chain of vcds) from SceneTable[arg]
//arg==null    -> Continue playing current scene with next vcd in current chain
{
	local dingon = false
	if (arg == null)
	{
		arg=waitNext
	}
	else
	{
		//If this is a call from the map, look up the integer arg in the scene lookup table.
		//We need to do this because hammer/the engine can't pass a squirrel script a string, just an integer.
		if (typeof arg == "integer")
		{
			waitSceneStart = arg
			arg = SceneTableLookup[arg]
		}
		else
		{
			waitSceneStart = 0
		}
		//This is a new dialog block, so turn off special processing
		dingon=true
		pitchShifting = false
		startBlock = Time()
		if ("noDingOff" in SceneTable[arg])
			waitNoDingOff = true
		else
			waitNoDingOff = false	
		if ("noDingOn" in SceneTable[arg])
			waitNoDingOn = true
		else
			waitNoDingOn = false	
	}
	local preDelay = 0.00
	if ( arg != null )
	{
		if ("predelay" in SceneTable[arg])
			preDelay = SceneTable[arg].predelay
		//Cancel any vcd that's already playing
		local curscene = self.GetCurrentScene()
		if ( curscene != null )
		{
			EntFireByHandle( curscene, "Cancel", "", 0, null, null )
		}
		
		//Play the initial ding (unless the scene specifically requests no ding)
		if (dingon && !waitNoDingOn)
			EntFireByHandle( sceneDingOn, "Start", "", 0, null, null )

		//Start the new vcd	
		if (dingon && !waitNoDingOn)
			EntFireByHandle( SceneTable[arg].vcd, "Start", "", 0.18, null, null )
		else	
			EntFireByHandle( SceneTable[arg].vcd, "Start", "", preDelay, null, null )

		//Check for EntFire at end of vcd
		if ("fireentity" in SceneTable[arg])
		{
			waitEntityFire = SceneTable[arg].fireentity
			waitEntityInput = SceneTable[arg].fireinput
			waitEntityDelay = SceneTable[arg].firedelay
			waitEntityParm = null
			if ("fireparm" in SceneTable[arg])
				waitEntityParm = SceneTable[arg].fireparm
		}
		else
		{
			waitEntityFire = null
			waitEntityInput = null
			waitEntityDelay = null
			waitEntityParm = null
		}
		//Check for any special end-of-vcd processing
		if ("endspecial" in SceneTable[arg]) 
			waitEndSpecial = SceneTable[arg].endspecial
		else 
			waitEndSpecial = 0

		//Check for any special processing tagged for this block
		if ("special" in SceneTable[arg])
		{
			switch (SceneTable[arg].special)
			{
				case 1: //Block-wide pitch shifting
					pitchShifting = true
					break
				case 2: //Speed up
					if (pitchOverride == null)	
						EntFireByHandle( SceneTable[arg].vcd, "PitchShift", "2.5", 0, null, null )
					break
				case 3: //Slow down a bit	
					EntFireByHandle( SceneTable[arg].vcd, "PitchShift", "0.9", 0, null, null )
					break
			}
		}
		if (pitchOverride!=null)
			EntFireByHandle( SceneTable[arg].vcd, "PitchShift", pitchOverride.tostring(), 0, null, null )

	    //Setup next line (if there is one)
	    if ( ! (SceneTable[arg].next==null) )
	    {
	    	waitLength = SceneTable[arg].postdelay
			waitNext = SceneTable[arg].next 
	    }	
		else
		{
			waitNext = null
			//Uncomment to report total time of chained block of vcds 
			//local tdiff = (Time()-startBlock)+vLen
			//printl("===================Total Block Time-> " + tdiff)
		}
	}	
}

function GladosSetPitch(arg)
{
	pitchOverride = arg
	local curscene = self.GetCurrentScene()
	if ( curscene != null )	
		EntFireByHandle( curscene, "PitchShift", pitchOverride.tostring(), 0, null, null )
}

//Turns off current Glados speech
function GladosStopTalking()
{
	local curscene = self.GetCurrentScene()
	pitchOverride = null
	waiting = 0
	waitNext = null
	waitLength = null
	//if ( curscene != null )	
	//		EntFireByHandle( curscene, "Cancel", "", 0, null, null )
}

//Turns off current Glados speech if the scene # passed as arg is currently playing
function GladosStopScene(arg)
{
	if (waitSceneStart == arg)
	{ 
		local curscene = self.GetCurrentScene()
		pitchOverride = null
		waiting = 0
		waitNext = null
		waitLength = null
		if ( curscene != null )	
			EntFireByHandle( curscene, "Cancel", "", 0, null, null )
	}		
}

//-------------------------------------------------------------------------------------
//Context-specific functions
//-------------------------------------------------------------------------------------
function GladosPlayerGetsGun(arg)
{
	if (coopFirstPlayerToGetGun == 0)
	{
		if (arg == coopBlue)
		{
			coopFirstPlayerToGetGun = coopBlue
			BlueHasGun=1
			GladosPlayVcd(1)
		}
		else
		{
			OrangeHasGun=1
			coopFirstPlayerToGetGun = coopOrange
			GladosPlayVcd(2)
		}
	}
	else	
	{	
		if (self.GetCurrentScene() == null)
		{
			if (arg == coopBlue)
			{
				BlueHasGun=1
				GladosPlayVcd(3)
			}
			else
			{
				OrangeHasGun=1
				GladosPlayVcd(4)
			}
		}	
		else
		{
			 coopWaitingForGetGun = arg
		}
	}
}

// a newly spawned cube or sphere was destroyed 
function GladosDroppedCubeDestroyed(arg)
{
	if ( arg == coopBlue )
	{
		printl ("2323124123412341234123")
		EntFireByHandle( badCatchBlue, "Start", "", 0, null, null )
	}
	else
	{
	  printl ("2323124123412341234123")
		EntFireByHandle( badCatchOrange, "Start", "", 0, null, null )
	}
}

function GladosGiveCompliment(arg)
{
	EntFireByHandle( ComplimentSceneTable[arg], "Start", "", 0, null, null )
}

function GladosCoopTurnOffVoice()
{
	coopRelaxationVaultVoiceMonitor = false
	coopRelaxationVaultVoiceBlueTicks = 0
	coopRelaxationVaultVoiceOrangeTicks = 0
}
function GladosInitRelaxationVault()
{
	//coopRelaxationVaultVoiceMonitor = true
	coopRelaxationVaultVoiceMonitor = false
	coopRelaxationVaultVoiceBlueTicks = 0
	coopRelaxationVaultVoiceOrangeTicks = 0
	GladosPlayVcd(0)
}

function GladosCoopPingPortalTraining()
{
		GladosPlayVcd(35)	
}


function GladosCoopInitiatePlanB()
{
	GladosPlayVcd(60)
}

function GladosCoopEnterRadarRoom()
{
	GladosPlayVcd(65)
}

function GladosCoopStartRadar()
{
	GladosPlayVcd(66)
}

function GladosCoopExplainMakeHuman()
{
	GladosPlayVcd(75)
}

function GladosCoopExplainMakeHumanFail()
{
	GladosPlayVcd(80)
}

//////
function GladosCoopArtifact_1_Enter()
{
	GladosPlayVcd(100)
}

function GladosCoopArtifact_1_Scan()
{
	GladosPlayVcd(102)
}

function GladosCoopArtifact_1_Return()
{
	GladosPlayVcd(103)
}
///////

function GladosCoopReturnHubArtifact_1()
{
	GladosPlayVcd(85)
}

function GladosToggleDebugMode()
{
	debug = !debug
	if (debug) printl("=======================GLaDOS debug mode ON")
	else printl("=======================GLaDOS debug mode OFF")
}

//-------------------------------
// PAX PAX END OF DEMO
//-------------------------------
//ping reminder
function GladosCoopPAXEndDemo()
{
	GladosPlayVcd(153)
}
	
//Plays the final audio for each level
function GladosCoopElevatorEntrance(arg)
{
	local mapname = GetMapName()
	if (debug)
		printl("====================================Player "+arg+" map "+mapname)
	if (coopTriggeredElevator)
	{
		return
	}
	coopTriggeredElevator = true
	switch (mapname)
	{
	// PAX BELOW
//		case "mp_coop_start_pax": //Map 1
//			GladosPlayVcd(150)  //PAX
//			break
//		case "mp_coop_laser_2": //Map 1
//			GladosPlayVcd(151) 
//			break
//		case "mp_coop_wall_intro_train": 
//			GladosPlayVcd(152)
//			break		
//		case "mp_coop_infinifling_train": 
//			EntFire( "ENDDEMO", "Trigger", "", 0.0 )
//			break	
	// PAX ABOVE

		case "mp_coop_start": //Map 1
			GladosPlayVcd(50)
			break	
		case "mp_coop_start_02": //Map 1
			GladosPlayVcd(54)
			break
		case "mp_coop_start_03": //Map 1
			GladosPlayVcd(56)
			break			
		// OLD - not used
		//case "mp_coop_start": //Map 1
		//	if (arg == coopBlue)
		//		GladosPlayVcd(8)
		//	else
		//		GladosPlayVcd(9)
		//	break
//		case "mp_coop_come_along":
//			// this disables the player exit to keep dialogue from getting cut off, 
//			// don't forget to turn it back on using "@relay_enable_exit" when glados is finished talking!
//			EntFire( "@relay_disable_exit", "Trigger", "", 0.0 )
//			GladosPlayVcd(130)
//			break
		case "mp_coop_fling_1": //Map 19
			// this disables the player exit to keep dialogue from getting cut off, 
			// don't forget to turn it back on using "@relay_enable_exit" when glados is finished talking!
			EntFire( "@relay_disable_exit", "Trigger", "", 0.0 )
			GladosPlayVcd(120)
			break
		case "mp_coop_catapult_1":
			GladosPlayVcd(13)
			break
		case "mp_coop_multifling_1": //Map 6
			GladosPlayVcd(14)
			break					
		case "mp_coop_fling_crushers": //Map 6
			GladosPlayVcd(15)
			break
			
		case "mp_coop_wall_intro": 
			GladosPlayVcd(16)
			break
		case "mp_coop_wall_2":
			GladosPlayVcd(17)
			break
		case "mp_coop_catapult_wall_intro": 
			GladosPlayVcd(18)
			break
		case "mp_coop_wall_block": //Map 5
			GladosPlayVcd(19)
			break
		case "mp_coop_catapult_2": //Map 5
			GladosPlayVcd(20)
			break
						
		case "mp_coop_laser_2": //Map 4
			GladosPlayVcd(15)
			break

		case "mp_coop_race_2": //Map 7
			GladosPlayVcd(18)
			break

		case "mp_coop_wall_6": //Map 9
			GladosPlayVcd(20)
			break
		case "mp_coop_wall_catch": //Map 10
			GladosPlayVcd(21)
			break
		case "mp_coop_laser_crusher": //Map 11
			if (arg == coopBlue)
				GladosPlayVcd(23)
			else
				GladosPlayVcd(24)
			break
			//GladosPlayVcd(22)
			//break

		case "mp_coop_wall_straight_jump": //Map 14 
			//GladosPrivateTalk(arg,2)
			GladosPlayVcd(15)
			break
		case "mp_coop_rat_maze": //Map 15 
			//GladosPrivateTalk(arg,3)
			GladosPlayVcd(15)
			break
		case "mp_coop_turret_walls": //Map 16 
			//GladosPrivateTalk(arg,4)
			GladosPlayVcd(15)
			break
		case "mp_coop_wall_5": //Map 18 
			//GladosPrivateTalk(arg,6)
			GladosPlayVcd(15)
			break
		case "mp_coop_race_3": //Map 20 
			//GladosPrivateTalk(arg,8)
			GladosPlayVcd(15)
			break
	}
}

//Fires two different vcds - one for each player's ears only.
function GladosPrivateTalk(player,vndx)
{
	if (player == coopBlue)
	{
		GladosSetBroadcastState( PrivateMixVcds[vndx], "blue" )
		GladosSetBroadcastState( PrivateBothVcds[vndx], "orange" )
		EntFireByHandle(PrivateMixVcds[vndx], "Start", "", 0.00, null, null )
		EntFireByHandle(PrivateBothVcds[vndx], "Start", "", 0.00, null, null )
	}	
	else
	{
		GladosSetBroadcastState( PrivateMixVcds[vndx], "orange" )
		GladosSetBroadcastState( PrivateBothVcds[vndx], "blue" )
		EntFireByHandle(PrivateMixVcds[vndx], "Start", "", 0.0, null, null )
		EntFireByHandle(PrivateBothVcds[vndx], "Start", "", 0.00, null, null )
	}
}

function GladosSayHello(player)
{
	if (player == coopBlue)
	{
		GladosSetBroadcastState( HelloVcds[1], "blue" )
		EntFireByHandle(HelloVcds[1], "Start", "", 0.00, null, null )
	}	
	else
	{
		GladosSetBroadcastState( HelloVcds[2], "orange" )
		EntFireByHandle(HelloVcds[2], "Start", "", 0.00, null, null )
	}
}

function GladosSetBroadcastState(vcd,target )
{
	local ORANGE_PLAYER = 2
	local BLUE_PLAYER = 3
	if ( target == "both" )
	{
		vcd.AddBroadcastTeamTarget( BLUE_PLAYER )
		vcd.AddBroadcastTeamTarget( ORANGE_PLAYER )
	}
	else if ( target == "blue" )
	{
		vcd.AddBroadcastTeamTarget( BLUE_PLAYER )
		vcd.RemoveBroadcastTeamTarget( ORANGE_PLAYER )
	}
	else if ( target == "orange" )
	{
		vcd.RemoveBroadcastTeamTarget( BLUE_PLAYER )
		vcd.AddBroadcastTeamTarget( ORANGE_PLAYER )
	}
}


//Tells GLaDOS that the level wants to end and transition to the next level.
//This gives her a chance to finish up any dialog before transitioning
//Relies on a relay called "gladosendoflevelrelay" in the map that actually switches levels when triggered.
//GLaDOS triggers "gladosendoflevelrelay" from her think function.
function GladosEndLevelRequest()
{
	coopWaitingToExit = true
}

//---------------------------------------------------------
//Score annoucing stuff
//---------------------------------------------------------
	function GladosAnnouncePlayerDemerit(arg,low,hi)
	{
		if (low < 2) low = 2
		if (low>hi)
		{
			local tlow = low
			low = hi
			hi = tlow
		}
		coopScoreToState = RandomInt(low,hi)
		EntFireByHandle( sceneDingOn, "Start", "", 0, null, null )
		if (arg==coopBlue)
		{
			EntFireByHandle(ScorePrefixVcds[1], "Start", "", 0.18, null, null )
		}
		else
		{
			EntFireByHandle(ScorePrefixVcds[2], "Start", "", 0.18, null, null )
		}
	}

	function GladosAnnouncePlayerScore(arg,low,hi)
	{
		if (low < 2) low = 2
		if (low>hi)
		{
			local tlow = low
			low = hi
			hi = tlow
		}
		coopScoreToState = RandomInt(low,hi)
		EntFireByHandle( sceneDingOn, "Start", "", 0, null, null )
		if (arg==coopBlue)
		{
			EntFireByHandle(ScorePrefixVcds[3], "Start", "", 0.18, null, null )
		}
		else
		{
			EntFireByHandle(ScorePrefixVcds[4], "Start", "", 0.18, null, null )
		}
	}

	function GladosStateScore()
	{
		if (coopScoreToState == null)
		{
			return
		}
		if (coopScoreToState <0)
		{
			coopScoreToState =  (-1 * coopScoreToState)
			EntFireByHandle(NumberVcds[100], "Start", "", 0.0, null, null )
			return
		}
		if (coopScoreToState >99)
		{
			local p = coopScoreToState / 100
			coopScoreToState =  (-1 * coopScoreToState % 100)
			if (coopScoreToState == 0) coopScoreToState = 1
			EntFireByHandle(NumberVcds[p], "Start", "", 0.0, null, null )
			return
		}
		if (coopScoreToState >9)
		{
			if (coopScoreToState>= 10 && coopScoreToState <=19) coopScoreToState+=10
			local p = coopScoreToState / 10
			coopScoreToState =  coopScoreToState % 10
			EntFireByHandle(NumberVcds[p*10], "Start", "", 0.0, null, null )
			return
		}
		if (coopScoreToState > 0)
		{
			EntFireByHandle(NumberVcds[coopScoreToState], "Start", "", 0.0, null, null )
			coopScoreToState = 0
			return
		}
		EntFireByHandle(ScienceCollaborationPointsVcd, "Start", "", 0.0, null, null )
		EntFireByHandle( sceneDingOff, "Start", "", 1.8, null, null )
	}

	function GladosStateScienceCollaborationPoints()
	{
		EntFireByHandle(ScienceCollaborationPointsVcd, "Start", "", 0.0, null, null )
	}
//---------------------------------------------------------
//END OF Score annoucing stuff
//---------------------------------------------------------


//-------------------------------
// NEW NEW Ping Training Sequence stuff
//-------------------------------
	//ping reminder
	function GladosCoopPingReminder1()
	{
		GladosPlayVcd(45)
	}
	
//-------------------------------
//Ping Training Sequence stuff
//-------------------------------
	//Play the ping training intro
	function GladosCoopPingTrainingIntro()
	{
		GladosPlayVcd(10)
	}

	//Housecleaning / entity stuff that happens when the ping training is over  
	function GladosCoopPingGameOver()
	{
		EntFire("aperture_door","SetSpeed",35, 0.00)
		EntFire("aperture_door","Close", "", 0.00)
		EntFire("dome_exit_door_blue","Open", "", 3.00)
		EntFire("dome_exit_door_orange","Open", "", 3.00)
		EntFire("platform_2_gate_exit","Open", "", 1.00)
		EntFire("platform_1_gate_exit","Open", "", 1.00)
	}

	//Turns ping training game on
	function GladosCoopPingTrainingGameOn()
	{
		if (debug)
			printl("========================GAME ON!!!!!")
		coopPingGameOn = true  //We are in ping training mode!
		coopPingPlayerTurn = coopBlue  //It's BLUE's turn
		coopPingTurnNumber = 1 //It's turn #1
		coopTimeSinceTurn = Time()  //Turn #1 starts NOW!
		EntFire("PingHint","ShowHint", "!player_blue", 0.00)  //Fire the ping hint to blue player
	}
	
	//Dome ping redirect
	function GladosPingTrainingPingDome(arg)
	{
		GladosPingTrainingPing(arg)
	}

	//Handles player placing a ping
	function GladosPingTrainingPing(arg)
	{
		//Track total # of pings by both players
		if (arg == coopBlue) coopPingsBlue += 1
		if (arg == coopOrange) coopPingsOrange += 1
		//Process ping only if GLaDOS isn't talking
		if ((!coopPingNoInterrupt) && coopPingGameOn)
		{
			if (arg==coopBlue) //Ping belongs to BLUE
			{
				if (coopPingPlayerTurn==coopBlue) //Is it actually BLUE's turn?
				{
					switch (coopPingTurnNumber)
					{
						case 1:  //Turn 1
							coopPingPlayerTurn = coopOrange
							coopPingNextScene = 5 
							coopPingNextSceneDelay = 0.4
							coopTimeSinceTurn = Time()
							GladosPingTrainingSpeak(2,true,0.00)
							EntFire("PingHint","EndHint", "!player_blue", 0.00)  
							EntFire("PingHint","ShowHint", "!player_orange", 3.00)  
							break
						case 2: //Turn 2
							coopPingPlayerTurn = coopOrange
							coopPingNextScene = 16 
							coopPingNextSceneDelay = 0.4
							coopTimeSinceTurn = Time()
							GladosPingTrainingSpeak(4, true, 0.00)
							break
					}
				}
				else //BLUE is pinging out of turn!
				{
					GladosPingTrainingWrongMove(coopBlue)
				}
			}
			if (arg==coopOrange) //Ping belongs to ORANGE
			{
				if (coopPingPlayerTurn==coopOrange) //Is it actually ORANGE's turn?
				{
					switch (coopPingTurnNumber)
					{
						case 1: //TURN 1
							coopPingPlayerTurn = coopBlue
							coopPingNextScene = 3
							coopPingTurnNumber = 2 
							coopPingNextSceneDelay = 0.4
							coopTimeSinceTurn = Time()
							GladosPingTrainingSpeak(19, true, 0.00)
							EntFire("PingHint","EndHint", "!player_orange", 0.00)  
							//GladosPingTrainingSpeak(20, true, 0.00)
							break
						case 2: //TURN 2 (FINAL MOVE OF GAME)
							coopPingGameOn = false
							GladosPlayVcd(11) //Play game over dialog dialog (also closes dome doors)
							break
					}
				}
				else //Orange is pinging out of turn!
				{
					GladosPingTrainingWrongMove(coopOrange)
				}
			}
		}	
	}
	
	//Handles player placing ping when it's not their turn
	function GladosPingTrainingWrongMove(arg)
	{
		//Gave players their last warning, so shut the game down
		if (coopSaidWrongMovesDialog)
		{
			coopPingGameOn = false
			GladosPlayVcd(12)
			return
		} 
		//Give players a final warning
		if (coopPingsBlue>0 && coopPingsOrange>0 && (coopWrongMovesBlue+coopWrongMovesOrange)>0 && !coopSaidWrongMovesDialog)
		{
			coopSaidWrongMovesDialog = true
			GladosPingTrainingSpeak(11, true, 0.00)
			return
		}
		//Tell BLUE not to go out of turn
		if (arg==coopBlue && coopWrongMovesOrange == 0 && coopWrongMovesBlue < 5) //BLUE goes out of turn, but Orange hasn't placed a ping
		{
			GladosPingTrainingSpeak(13, true, 0.00)
			coopWrongMovesBlue +=1
			return
		}
		//Tell ORANGE not to go out of turn
		if (arg==coopOrange && coopWrongMovesBlue == 0 && coopWrongMovesOrange < 5)
		{
			GladosPingTrainingSpeak(14, true, 0.00)
			coopWrongMovesOrange += 1
			return
		}
		//Failsafe for final warning
		if (!coopSaidWrongMovesDialog)
		{
			coopSaidWrongMovesDialog = true
			GladosPingTrainingSpeak(11, true, 0.00)
			return
		} 
	}
	
	function GladosPingTrainingSpeak(arg,setNoInterrupt,d)
	//arg: PingTrainingVcd index
	//setNoInterrupt: true = dialog can't be interrupted by intervening event
	//d: delay (in seconds) before line
	{
			if (setNoInterrupt == null) setNoInterrupt = true
			if (d == null) d = 0.00
			coopPingNoInterrupt = setNoInterrupt
			EntFireByHandle(PingTrainingVcds[arg], "Start", "", d, null, null )
	}
	
	//Automatically called at the end of every PingTrainingVcd. Starts next vcd if one has been set.
	function GladosPingTrainingNextScene()
	{
		local i = coopPingNextScene
		local d = coopPingNextSceneDelay
		if (coopPingNextScene != null)
		{
			coopPingNextScene = null
			coopPingNextSceneDelay = null
			GladosPingTrainingSpeak(i,true,d)
		}
		else coopPingNoInterrupt = false	
	}
//--------------------------------------
//END of ping training sequence stuff
//--------------------------------------

//-------------------------------------------------------------------------------------
//END OF Context-specific functions
//-------------------------------------------------------------------------------------

//----------------------------------------------------
//microphone utilities
//----------------------------------------------------
function GladosStartMicTest()
{
	debugMicTest = true
}
function GladosStopMicTest()
{
	debugMicTest = false
}

//----------------------------------------------------
//END of microphone utilities
//----------------------------------------------------

//----------------------------------------------------
//Catches for old method of firing directly into speakvcd
//----------------------------------------------------
//Glados placing portals for player using ping tool.
SceneTableLookup[35] <- "mp_coop_startcoop_portal_ping_intro00"

function GladosPingIntroDone()
{
	printl ("12312312")
	BlueInPortalTraining = 1
	OrangeInPortalTraining = 1
}

function CoopStartTwoPortals(){
		GladosPlayVcd(1002)
}


function CoopPingTool(player,surface) 
{
	local curTime = Time()
	local mapname = GetMapName()
	printl (BlueInPortalTraining)
	printl (BlueHasGun)
	printl (mapname)
	if (player == coopBlue){		
		if (BlueInPortalTraining == 1 && BlueHasGun == 0 && mapname == "mp_coop_start"){
			printl ("in")
			local BluePingInterval = curTime-BlueLastPing
			BlueLastPing = curTime
			if (BluePingInterval<3 && surface==1){
				BlueBadPingCount++	
				printl(BluePingInterval)
			}
			if (BlueBadPingCount==5){
				GladosPlayVcd(1000)
			}
			if (BlueBadPingCount==20){
				GladosPlayVcd(1001)
			}		
		}
	}

}



