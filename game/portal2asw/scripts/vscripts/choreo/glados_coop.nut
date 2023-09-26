BluePingCount <- 0	
BlueBadPingCount <- 0
BlueLastPing <- Time()
BlueInPortalTraining <-0
OrangeInPortalTraining <-0
OrangePingCount <- 0	
OrangeLastPing <- Time()
OrangeBadPingCount <- 0
BluePortalTrainingCounter <- 0
OrangePortalTrainingCounter <- 0
BlueHasGun <-0
OrangeHasGun <-0
CoopStartCalCompleteSet <- 0
BlueInMaze <- 0
OrangeInMaze <- 0
BlueInCrusher <-0
OrangeInCrusher <-0
mp_coop_infinifling_traindeath <- 0
mp_coop_infinifling_traindeath01 <- 0
mp_coop_catapult_wall_introdeath <- 0
mp_coop_wall_blockdeath <- 0
bluetrust <-0
orangetrust <-0
BlueTalk <-0
OrangeTalk <-0
BlueNoTalk <-0
OrangeNoTalk <-0
startdeath <-0
mp_coop_turret_ballcount <-0
InHub2 <-0
teambtstaunts <- 0
GladosInsideTauntCam <- 0
GladosInsideTauntCamCounter <- 1
GladosInsideTauntCamInterval <- 0
GladosInsideTauntCamLastInterval <- 0
OrangeTauntCam <-0
BlueTauntCam <-0
BlueTauntFinaleInterval <-0
OrangeTauntFinaleInterval <-0
LastDeathTime <- 0
LastTauntTime <- 0
BluePingStartTimer <-0
OrangePingStartTimer <-0
BluePingStartDone <- 0
OrangePingStartDone <- 0
//Fire Once
CoopStartBButtonSet <-0
CoopStartButtonSet <-0
CoopStartBoxButtonSet <-0
CoopCrushersBlueCrushset <- 0
CoopCrushersOrangeCrushset <- 0
CoopMazeBlueCrushset <- 0
CoopMazeOrangeCrushset <- 0
InHub2Set <- 0
testcnt <-0
BluePingTraining1 <-0
BluePingTraining2 <-0
OrangePingTraining1 <-0
OrangePingTraining2 <-0
mp_coop_wall_2death <-0
HumanResourceDeath1 <-0
HumanResourceDeath2 <-0
HumanResourceDeath3 <-0
HumanResourceDeath4 <-0
HumanResourceDeath5 <-0
HumanResourceDeath6 <-0
HumanResourceDeath7 <-0
EarlyDeath1 <-0
EarlyDeath2 <-0
EarlyDeath3 <-0
EarlyDeath4 <-0
EarlyDeath5 <-0
Tbeam_enddeathturret <-0
Tbeam_enddeathfall <-0
Tbeam_enddeathcounter <-0
Tbeam_enddeathother <- 0
mp_coop_laser_crusherdeath <-0
mp_coop_turret_wallscount <-0
mp_coop_tbeam_laser_1death1 <-0
mp_coop_tbeam_laser_1death2 <-0
mp_coop_tbeam_redirectdeath <-0
mp_coop_tbeam_drilldeathorange <-0
mp_coop_tbeam_drilldeathblue <-0
mp_coop_tbeam_polaritydeathorange <-0
mp_coop_tbeam_polaritydeathblue <-0
mp_coop_tbeam_polarity3deathorange <-0
mp_coop_tbeam_polarity3deathblue <-0
polarity2whisper <- 0
mp_coop_multifling_1cube <- 0
summer_sale_cube_died <- false

//ispaused()
//IsPlayingBack()
//EstimateLength()

//debug stuff
debug <- false	//Set true to enable debug block during think function
debugInterval <- 10.00

lastthink <- Time()
startBlock <- 0

//Queue of scenes started with GladosPlayVcd()
scenequeue <- {}
firedfromqueue <- false

//Map Name
curMapName <- GetMapName()

//PitchShifting stuff
pitchShifting <- false
pitchShiftLastThink <- Time()
pitchShiftInterval <- 1.0
pitchShiftValue <- 1.0
pitchOverride <- null

//State Flags
stateSlowFieldTraining <- false


//jailbreak specific stuff
if (curMapName=="sp_sabotage_jailbreak_01" || curMapName=="sp_sabotage_jailbreak" || curMapName=="sp_a2_bts1")
{
	jailbreakpos <- 0
}


//sp_catapult_fling_sphere_peek specific stuff
if (curMapName=="sp_a2_sphere_peek")
{
	peekctr <- 0
}


//Dings
sceneDingOn  <- CreateSceneEntity("scenes/npc/glados_manual/ding_on.vcd")
sceneDingOff <- CreateSceneEntity("scenes/npc/glados_manual/ding_off.vcd")



DoIncludeScript( "choreo/glados_coop_scenetable_include", self.GetScriptScope() )
DoIncludeScript( "choreo/glados_coop_scenetable_include_manual", self.GetScriptScope() )

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

// Achievement LIMITED_PORTALS
LIMITED_PORTALS_MAP <- "mp_coop_catapult_wall_intro"	// Map the achievement takes place on.
LIMITED_PORTALS_COUNT <- 5								// Max number of portals to make the achievement.

// Achievement STAYING_ALIVE
STAYING_ALIVE_SECTION <- 4								// Section the achievement applies to.

// Achievement SPEED_RUN_COOP
SPEED_RUN_THRESHOLD <- 60								// Number of seconds a run must be finished in to count.
SPEED_RUN_SECTION <- 2									// Section the achievement applies to.
mp_coop_speed_run_time <- 0

function printldebug(arg)
{
	if (debug)
	{
		printl(arg)
	}
}


// OnPostSpawn - this is all the initial setup stuff
function OnPostSpawn()
{
		local i = 0
		//assign a unique id to each scene entity (uses SetTeam because that's the only thing available)
		foreach (val in SceneTable)
		{
			i+=1
			val.vcd.ValidateScriptScope()
			val.vcd.SetTeam(i)
			val.index <- i
		}		
		//Initialize the deferred scene queue
		QueueInitialize()
				
		//Map specific Spawn stuff
		switch (curMapName)
		{
			case "sp_a1_wakeup":
				EntFire("@glados","runscriptcode","sp_a1_wakeup_start_map()",1.0)
				break
		}
}



//Passed the unique team id associated with a scene entity defined in SceneTable, this function returns the correct SceneTable index for that entry
//ex: SceneTableInst = SceneTable[findIndex(team)]
function findIndex(team)
{
		local idx, val
		foreach (idx, val in SceneTable)
		{
			if (val.index == team)
			{
				return idx
			}
		}
		return null
}

//Passed the unique team id associated with a scene entity defined in SceneTable, this function returns the scene instance that originally fired it.
//Unfortunately, if multiple scene instances fire the same vcd, this isn't reliable. Hopefully we can either a) fix that 
//or b) not have multiple scene instances use the same vcd (which is currently the case as of 07/19/2010 - efw)
function FindSceneInstanceByTeam(team)
{
	local idx, val
	local idx2, val2
	local inst = null
	foreach (idx, val in scenequeue)
	{
		foreach (idx2, val2 in val.waitFiredVcds)
		{
			if (val2 == team)
			{
				inst = val
				break
			}
		}
		if (inst != null)
			break
	}
	return inst
}


function SceneCanceled()
{
	printldebug("========SCENE CANCELLED - CALLING ENTITY: "+findIndex(owninginstance.GetTeam()))
}

//If a vcd is tagged to "exit early" (by setting postdelay < 0), this event fires rather than PlayNextScene() when the vcd finishes.
//SkipOnCompletion() has all the functionality of PlayNextScene(), except it doesn't actually play the next scene, because the next scene presumably
//already started playing when the vcd exited early.
//SkipOnCompletion does, however, evaluate the vcd's SceneTable entry to see if any EntFires need to happen.
function SkipOnCompletion()
{
	printldebug("========SKIPONCOMPLETION CALLING ENTITY: "+findIndex(owninginstance.GetTeam())+" : TIME "+Time())
	local team = owninginstance.GetTeam()
	local inst = FindSceneInstanceByTeam(team)
	if (inst != null)
	{
		inst.deleteFiredVcd(team)
		inst.waitVcdCurrent = findIndex(team)
		//Are there any EntFires associated with this vcd?
		if (inst.waitVcdCurrent != null)
		{
			if ("fires" in SceneTable[inst.waitVcdCurrent])
			{
				local idx, val
				foreach (idx, val in SceneTable[inst.waitVcdCurrent].fires)
				{
					if (!("fireatstart" in val))
					{
						printldebug(">>>>>>ENT FIRE AT (SKIPCOMPLETION) END: "+val.entity+":"+val.input)
						EntFire(val.entity,val.input,val.parameter,val.delay)
					}
				}
			}
		}
	}
}	


function PlayNextScene()
{
	printldebug("========PLAYNEXTSCENE CALLING ENTITY: "+findIndex(owninginstance.GetTeam())+" : TIME "+Time())
	local team = owninginstance.GetTeam()
	local inst = FindSceneInstanceByTeam(team)
	if (inst != null)
	{
		inst.deleteFiredVcd(team)
		inst.waitVcdCurrent = findIndex(team)
		PlayNextSceneInternal(inst)
	}	
}	



function PlayNextSceneInternal(inst = null)
//inst = just completed scene
{
	local i = 0
	local tmp = 0
	//printldebug("===================Scene Done!" + i)
	
	//Set the ducking back to the default value
	SendToConsole( "snd_ducktovolume 0.55" )
	
	//Are there any "fire at the end" triggers associated with the just completed?
	if (inst.waitVcdCurrent != null)
	{
		if ("fires" in SceneTable[inst.waitVcdCurrent])
		{
			local idx, val
			foreach (idx, val in SceneTable[inst.waitVcdCurrent].fires)
			{
				if (!("fireatstart" in val))
				{
					printldebug(">>>>>>ENT FIRE AT END: "+val.entity+":"+val.input)
					EntFire(val.entity,val.input,val.parameter,val.delay)
				}
			}
		}
	}
	//Is there another vcd in the scene chain?
	if (inst.waitNext != null)
	{
		printldebug("=====There is a next scene: "+inst.waitNext)
		if (inst.waitLength == null)
		{
			i+=1
			printldebug("===================Ready to play:" + i)
			GladosPlayVcd(inst)
 		}	
		else
		{
			inst.waitStartTime = Time()
			inst.waiting = 1
		}	
	}
	else
	{
		printldebug("=====No next scene!")
		//Remove the instance from the scene list
		scenequeue_DeleteScene(inst.index)
		//The current scene is over. Check to see if there are any queued scenes.
		if (QueueCheck())
			return

		//Do the ding if nothing's queued and the previous scene requires a ding
		if (!inst.waitNoDingOff)
			EntFireByHandle( sceneDingOff, "Start", "", 0.1, null, null )
	}
}


//Think function
function GladosThink()
{

	//Put debug stuff here!
	if (debug)
	{
		if (Time()-lastthink>debugInterval)
		{
			printldebug("===================GladosThink-> " + lastthink)
			lastthink = Time()
			QueueDebug()
		}
	}	
	


	local idx, val
	foreach (idx,val in scenequeue)
	{
		//Check if current vcd is scheduled to exit early
		if (val.waitExitingEarly)
		{
			if (Time()-val.waitExitingEarlyStartTime >= val.waitExitingEarlyThreshold)
			{
				local team
				val.waitExitingEarly=false
				local curscene = characterCurscene(val.currentCharacter)
				if (curscene!=null)
				{
					curscene.ValidateScriptScope()
					curscene.GetScriptScope().SkipOnCompletion <- SkipOnCompletion.bindenv(this)
					curscene.DisconnectOutput("OnCompletion", "PlayNextScene")
					curscene.DisconnectOutput("OnCompletion", "SkipOnCompletion")
					curscene.ConnectOutput( "OnCompletion", "SkipOnCompletion" )
					team = curscene.GetTeam()
					val.waitVcdCurrent = findIndex(team)
				}
				printldebug("====EXITING EARLY!!!!!!!")
				PlayNextSceneInternal(val)
				return
			}
		}
	}
	
	local tmp
	//Check the deferred scene queue
	tmp = QueueThink()
	//Is a queued scene ready to fire?
	if (tmp != null)
	{
			printldebug("===========Forcing a queued Scene!!!!========")
			GladosPlayVcd(tmp,true)
			return
	}

	foreach (idx,val in scenequeue)
	{
		//Are we waiting to play another vcd?
		if (val.waiting == 1)
		{
			if (Time()-val.waitStartTime >= val.waitLength)
			{
				val.waiting = 0
				GladosPlayVcd(val)
			}
		}
	}
	
	if (curMapName=="mp_coop_start" &&  BluePingStartDone==1){
		local curTime=Time()
		local BluePingStartInterval = curTime - BluePingStartTimer
		if (BluePingStartInterval>20){
			BluePingStartTimer=Time()
			GladosPlayVcd(1331)
		}
	}
	if (curMapName=="mp_coop_start" &&  OrangePingStartDone==1){
		local curTime=Time()
		local OrangePingStartInterval = curTime - OrangePingStartTimer
		if (OrangePingStartInterval>20){
			OrangePingStartTimer=Time()
			GladosPlayVcd(1332)
		}		
	}

	if (GladosInsideTauntCam == 1){
		local curTime=Time()
		local tauntreactionfired = 0
		local GladosInsideTauntCamInterval = curTime-GladosInsideTauntCamLastInterval
		printldebug("=================================== CAM INTERVAL:"+GladosInsideTauntCamCounter+" - "+GladosInsideTauntCamInterval+" - "+GladosInsideTauntCam)
		if (GladosInsideTauntCamInterval>8 || GladosInsideTauntCamCounter==1){
			if (BlueTauntCam==1	&& OrangeTauntCam==0){
				if (curTime-BlueTauntFinaleInterval<5){
					tauntreactionfired = 1
					GladosInsideTauntCamLastInterval=Time()-6
					GladosPlayVcd(1194)
				}
			}

			if (BlueTauntCam==0	&& OrangeTauntCam==1){
				if (curTime-OrangeTauntFinaleInterval<5){
					tauntreactionfired = 1
					GladosInsideTauntCamLastInterval=Time()-6
					GladosPlayVcd(1195)
				}
			}

			if (tauntreactionfired == 0){
			switch (GladosInsideTauntCamCounter)
			{
				case 1: 
					GladosPlayVcd(1156)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					// this is where we want to count completing paint_longjump
					EndSpeedRunTimer()
					break

				case 2: 
					GladosPlayVcd(1166)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 3: 
					GladosPlayVcd(1165)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 4: 
					GladosPlayVcd(1167)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 5: 
					GladosPlayVcd(1168)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 6: 
					GladosPlayVcd(1158)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 7: 
					GladosPlayVcd(1157)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 8: 
					GladosPlayVcd(1169)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 9: 
					GladosPlayVcd(1159)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 10: 
					GladosPlayVcd(1160)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 11: 
					GladosPlayVcd(1164)
					GladosInsideTauntCamCounter=GladosInsideTauntCamCounter+1
					GladosInsideTauntCamLastInterval=Time()
					break
				case 12: 
					GladosPlayVcd(1163)
					GladosInsideTauntCamCounter=1
					GladosInsideTauntCamLastInterval=Time()
					break
			}
			}
			
		}

	}


}

function GladosBlowUpBots()
{
	// this calls the function in game code
	CoopGladosBlowUpBots()
}

function GladosEndingTauntCam_Begin(){
	GladosInsideTauntCam =1 
}

function GladosEndingTauntCam_End(){
	GladosInsideTauntCam =0 
}


//Play a vcd from the SceneTable, plus set up next line (if any).
//This is the function that should be used to start a scene from inside a map.
function GladosPlayVcd(arg,IgnoreQueue = null, caller = null)
//arg==instance	-> Continue playing scene defined by scene class instance arg
//arg==integer	-> Start playing new scene (scene being a chain of vcds) from SceneTable[SceneTableLookup[arg]]
//arg==string		-> Start playing new scene (scene being a chain of vcds) from SceneTable[arg]
//arg==null			-> Continue playing current scene with next vcd in current chain
//IgnoreQueue		-> true == don't check for queue status (this is used to force a queued vcd to play)
//caller				-> If passed as an entity, the vcd will have its "target1" set to caller.GetName()
{
	printldebug("=========GladosPlayVcd Called!=========")	
	local dingon = false
	local inst
	local fromqueue = firedfromqueue
	firedfromqueue = false
	if (curMapName=="mp_coop_start" ){	
		if (arg==30){	
			BluePingStartDone=1
			BluePingStartTimer=Time()+15
		}
		if (arg==32){	
			BluePingStartDone=2
			OrangePingStartDone=1
			OrangePingStartTimer=Time()+25
		}
		if (arg==34){	
			OrangePingStartDone=2
		}
	}
	
	
	if (typeof arg == "instance")
	{
		inst = arg
		arg=inst.waitNext
	}
	else
	{
		//If this is a call from the map, look up the integer arg in the scene lookup table.
		//We need to do this because hammer/the engine can't pass a squirrel script a string, just an integer.
		//In other words, from a map, @glados.GladosPlayVcd("MyVcd") crashes the game. GladosPlayVcd(16) doesn't.
		local sceneStart = 0
		if (typeof arg == "integer")
		{
			sceneStart = arg
			printldebug("{}{}{}{}{}{}{}{}{}GladosPlayVcd: "+arg)	
			arg = SceneTableLookup[arg]
		}
		else
		{
			sceneStart = 0
		}
		//if SkipIfBusy is present & we're already playing a scene, skip this new scene
		if ("skipifbusy" in SceneTable[arg])
		{
		  if (!("char" in SceneTable[arg])){
		  	SceneTable[arg].char <- "glados" 
		  }

			if (characterCurscene(SceneTable[arg].char)!=null)
			{
				return
			}
		}
		//if queue is present & we're already playing a scene, add scene to queue
 		if ("queue" in SceneTable[arg])
	 	{
	 		if (IgnoreQueue == null)
			{
				//queue if a specific character is talking 
		 		if ("queuecharacter" in SceneTable[arg])
		 		{
					if (characterCurscene(SceneTable[arg].queuecharacter)!=null)
					{
				 		QueueAdd(arg)
			 			return
			 		}
				}
				//otherwise, queue if the character associated with the vcd is talking
				else
				{
					if (!("char" in SceneTable[arg])){
	  				SceneTable[arg].char <- "glados" 
			  	}

					if (characterCurscene(SceneTable[arg].char)!=null)
					{
				 		QueueAdd(arg)
			 			return
			 		}
			 	}	
			}		
	  }
	  if (!("char" in SceneTable[arg])){
	  	SceneTable[arg].char <- "glados" 
	  }
 		if (scenequeue_AddScene(arg,SceneTable[arg].char) == null)
			return
		inst = scenequeue[arg]
		inst.waitSceneStart = sceneStart
		
		if ("idle" in SceneTable[arg])
		{
			nags_init(inst,arg)
		}

		//This is a new dialog block, so turn off special processing
		dingon=true
		pitchShifting = false
		//startBlock = Time()
		if ("noDingOff" in SceneTable[arg])
			inst.waitNoDingOff = true
		else
			inst.waitNoDingOff = false	
		if ("noDingOn" in SceneTable[arg])
			inst.waitNoDingOn = true
		else
			inst.waitNoDingOn = false	
	}
	
	//If this scene is a nag/idle cycle, grab the next line off the stack
	if (inst.isNag)
	{
		//If we're not in a vcd chain, grab the next vcd from the randomized pool
		if (!inst.naginchain)
		{
			arg = nags_fetch(inst)
		}	
		//if nothing fetched (because the nag has used all the lines and isn't marked as "repeat"), remove this scene
		if (arg == null)
		{
			scenequeue_DeleteScene(inst.index)
			return
		}
	}
	
	//Set ducking volume correctly for booming glados audio
	SendToConsole( "snd_ducktovolume 0.2" )
	
  //SetDucking( "announcerVOLayer", "announcerVO", 0.01 ) 
  //SetDucking( "gladosVOLayer", "gladosVO", 0.1 ) 

	local preDelay = 0.00
	preDelay = EvaluateTimeKey("predelay", SceneTable[arg])
	if (fromqueue && "queuepredelay" in SceneTable[arg])
	{
		preDelay = EvaluateTimeKey("queuepredelay", SceneTable[arg])
	}
		
	if ( arg != null )
	{

		local ltalkover
		ltalkover =  "talkover" in SceneTable[arg]

		//Cancel any vcd that's already playing
		if (!ltalkover)
		{
			GladosAllCharactersStopScene()
		}	
		else
		{
			//characters can't currently talk over themselves
	  	if (!("char" in SceneTable[arg])){
		  	SceneTable[arg].char <- "glados" 
		  }
			GladosCharacterStopScene(SceneTable[arg].char)
		}
		
		//Play the initial ding (unless the scene specifically requests no ding)
		if (dingon && !inst.waitNoDingOn)
			EntFireByHandle( sceneDingOn, "Start", "", preDelay, null, null )

		
		//Start the new vcd	
		printldebug("===================Playing:" + arg)
	  if (!("char" in SceneTable[arg])){
	  	SceneTable[arg].char <- "glados" 
	  }

		inst.currentCharacter = SceneTable[arg].char
		
		//Bind the OnCompletion Event
		SceneTable[arg].vcd.ValidateScriptScope()
		SceneTable[arg].vcd.GetScriptScope().PlayNextScene <- PlayNextScene.bindenv(this)
		SceneTable[arg].vcd.DisconnectOutput( "OnCompletion", "PlayNextScene" )
		SceneTable[arg].vcd.ConnectOutput( "OnCompletion", "PlayNextScene" )
		SceneTable[arg].vcd.ConnectOutput( "OnCanceled", "SceneCanceled" )
		
		//Set the target1 if necessary
		if (caller != null)
		{
			if (typeof caller == "string")
			{
				EntFireByHandle( SceneTable[arg].vcd, "SetTarget1", caller, 0, null, null )
				printldebug("++++++++++++SETTING TARGET: "+caller)
			}	
			else
			{
				EntFireByHandle( SceneTable[arg].vcd, "SetTarget1", caller.GetName(), 0, null, null )
			}
		}	
		if ("settarget1" in SceneTable[arg])
		{
			EntFireByHandle( SceneTable[arg].vcd, "SetTarget1", SceneTable[arg].settarget1 , 0, null, null )
		}

		inst.waitVcdTeam = SceneTable[arg].index
		inst.waitVcdCurrent = arg
		
		inst.addFiredVcd(SceneTable[arg].index)
				
		if (dingon && !inst.waitNoDingOn)
			EntFireByHandle( SceneTable[arg].vcd, "Start", "", preDelay+0.18, null, null )
		else	
			EntFireByHandle( SceneTable[arg].vcd, "Start", "", preDelay, null, null )

		
		//Does this vcd have a "fire into entities" array?
		if ("fires" in SceneTable[arg])
		{
			local idx, val
			foreach (idx, val in SceneTable[arg].fires)
			{
				if ("fireatstart" in val)
				{
					printldebug(">>>>>>ENT FIRE AT START: "+val.entity+":"+val.input)
					EntFire(val.entity,val.input,val.parameter, val.delay)
				}
			}
		}

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
    if (SceneTable[arg].next != null || inst.isNag)
    {
    	local pdelay = EvaluateTimeKey("postdelay",SceneTable[arg])
    	
    	//if this is a nag, use min/max defined in the first entry in the scene
    	if (inst.isNag)
    	{
    		pdelay = RandomFloat(inst.nagminsecs,inst.nagmaxsecs)
    	}

    	if (pdelay<0.00)
    	{
    		if (inst.isNag)
    			//If the "next" key != null, it means we're in a vcd chain
    			if (SceneTable[arg].next != null)
    			{
    				inst.waitNext = SceneTable[arg].next
    				inst.naginchain = true
    			}
    			else
    			{
    				//Otherwise, just slug in the same index (any non-null value would work here, however)
						inst.waitNext = arg
    				inst.naginchain = false
					}
    		else
					inst.waitNext = SceneTable[arg].next 
 				inst.waitExitingEarly=true
 				inst.waitLength=null
 				inst.waitExitingEarlyStartTime=Time()
				//If we're in a nag vcd chain, use the vcds postdelay rather than the nag-wide delay
				//This is because vcd chains generally need to be explicitly timed at the chain level
				//since the vcds are grouped together as a block
 				if (inst.naginchain)
 					pdelay = EvaluateTimeKey("postdelay",SceneTable[arg])
 				inst.waitExitingEarlyThreshold=pdelay*-1
			}
			else
			{
 				inst.waitExitingEarly=false
    		if (inst.isNag)
    		{
    			//If the "next" key != null, it means we're in a vcd chain
    			if (SceneTable[arg].next != null)
    			{
    				inst.waitNext = SceneTable[arg].next
    				inst.naginchain = true
    			}
    			else
    			{
    				//Otherwise, just slug in the same index (any non-null value would work here, however)
						inst.waitNext = arg
    				inst.naginchain = false
					}
				}
    		else
    		{
					inst.waitNext = SceneTable[arg].next 
				}	
				//If we're in a nag vcd chain, use the vcds postdelay rather than the nag-wide delay
				//This is because vcd chains generally need to be explicitly timed at the chain level
				//since the vcds are grouped together as a block
 				if (inst.naginchain)
 					pdelay = EvaluateTimeKey("postdelay",SceneTable[arg])
			}	
   		inst.waitLength = pdelay
    }	
		else
		{
			inst.waitNext = null
			printldebug("===================SCENE END")
		}
	}	
}

function EvaluateTimeKey(keyname, keytable)
{
	local ret = null

 	if (keyname in keytable)
 	{
 		local typ = typeof keytable[keyname]
 		if (typ == "array")
 		{
 			if (keytable[keyname].len() != 2)
 			{
				printldebug("!!!!!!!!!!!!EVALUATE TIME KEY ERROR: "+keyname+" is an array with a length != 2") 		
				return 0.00
 			}
	 		ret = RandomFloat(keytable[keyname][0],keytable[keyname][1])
	 	}
	 	else
	 	{
	 		ret = keytable[keyname]
	 	}	
 	}
 	if (ret == null)
 		ret = 0.00
	printldebug(">>>>>>>>>EVALUATE TIME KEY: "+keyname+" : "+ret) 		
 	return ret
}


function GladosToggleDebug(arg = null)
{
	debug = !debug
	if (debug)
		printl("======================GLaDOS debug ON")
	else
		printl("======================GLaDOS debug OFF")
	if (arg!=null)
		debugInterval = arg
}

function GladosSetPitch(arg)
{
	pitchOverride = arg
	local curscene = self.GetCurrentScene()
	if ( curscene != null )	
		EntFireByHandle( curscene, "PitchShift", pitchOverride.tostring(), 0, null, null )
}


//Stops a scene for all characters
function GladosAllCharactersStopScene()
{
	GladosCharacterStopScene("glados")
	GladosCharacterStopScene("wheatley")
	GladosCharacterStopScene("cave_body")
}

function characterCurscene(arg)
{
	local ret = null, ent = null
	switch (arg)
	{
		case "glados":
		case "@glados":
			ent = Entities.FindByName(ent, "@glados")
			break
		case "@sphere":
		case "wheatley":	
		case "sphere":
			ent = Entities.FindByName(ent, "@sphere")
			break
		case "cave_body":	
		case "cavebody":	
			ent = Entities.FindByName(ent, "@cave_body")
			break
	}
	if (ent != null)
	{
		ret = ent.GetCurrentScene()
	}	
	return ret
}

//Stops a scene for a particular character
function GladosCharacterStopScene(arg)
{
	local ent = null
	local curscene = characterCurscene(arg)
	if ( curscene != null )
	{
		EntFireByHandle( curscene, "Cancel", "", 0, null, null )
	}
}

//Turns off current Glados speech
function GladosStopTalking()
{
	local curscene = self.GetCurrentScene()
	pitchOverride = null
	waitNext = null
	waitLength = null
	if ( curscene != null )	
			EntFireByHandle( curscene, "Cancel", "", 0, null, null )
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

//Slowfield functions
function GladosEndSlowFieldTraining()
{
	stateSlowFieldTraining = false
	GladosStopTalking()
}

function GladosStartSlowFieldTraining()
{
	stateSlowFieldTraining = true
	GladosPlayVcd(40)
}

function GladosSlowFieldOn()
{
	if (stateSlowFieldTraining)
		GladosPlayVcd(41)
}

function GladosSlowFieldOff()
{
	if (stateSlowFieldTraining)
		GladosPlayVcd(40)
}
//End of Slowfield functions

//Special Chamber Processing
function GladosRelaxationVaultPowerUp()
{
	EntFire("open_portal_relay","Trigger","", 0.00)
}


//--------------------------------------------------------------------------------------------------------------
//Queue Functions
//--------------------------------------------------------------------------------------------------------------

Queue <- []

//Initialize the queue
function QueueInitialize()
{
	Queue.clear()
}

//Add a scene to the queue
function QueueAdd(arg)
{
	Queue.append( { item = arg added = Time() })
	if (arg in SceneTable)
	{
 		if ("queueforcesecs" in SceneTable[arg])
 		{
 			Queue[Queue.len()-1].queueforcesecs <- SceneTable[arg].queueforcesecs
 		}
 		if ("queuetimeout" in SceneTable[arg])
 		{
 			Queue[Queue.len()-1].queuetimeout <- SceneTable[arg].queuetimeout
 		}
 		if ("queuepredelay" in SceneTable[arg])
 		{
 			Queue[Queue.len()-1].queuepredelay <- SceneTable[arg].queuepredelay
 		}

	}
}

//Returns number of items in the queue
function QueueLen()
{
	return Queue.len()
}


//Fetch the next scene in the queue
function QueueGetNext()
{
	local ret,l
	ret = null
	l=QueueLen()
	if (l>0)
	{
		ret = Queue[l-1].item
		Queue.remove(l-1)
	}
	return ret
}

function QueueDebug()
{
	printldebug("===================  items in queue-> " + Queue.len())
}

//General stuff called from GladosThink()
function QueueThink()
{
	local ret,t,index
	if (QueueLen()==0)
	{
		return null
	}	
	
	t=Time()
	//Check to see if any queued scenes timed out
	for (index = QueueLen(); index > 0; index -= 1)
	{
 		if ("queuetimeout" in Queue[index-1])
 		{
 			if (t-Queue[index-1].added > Queue[index-1].queuetimeout)
 			{
 				Queue.remove(index-1)
 			}
 		}
	}

	//Check to see if any queued scenes should force fire
	foreach (index, scene in Queue)
	{
 		if ("queueforcesecs" in scene)
 		{
 			if (t-scene.added >scene.queueforcesecs)
 			{
 				ret = scene.item
 				Queue.remove(index)
 				return ret
 			}
 		}
	}
	return null
}

//Debug testbed function
function QueueTest()
{
	local a = []
	for(local i=0;i<10;i+=1)
	{
		a.append(RandomInt(1,100))
		printldebug(">>>>>> " + i + " : " + a[i])
	}
	for (local i = a.len(); i>0; i-=1)
	{
		if (a[i-1]<50)
			a.remove(i-1)
	}
	foreach (index, n in a)
	{
		printldebug(">>>>>> " + index + " : " + a[index])
	}
}

//Delete a single item from the queue
function QueueDeleteItem(item)
{
	if (QueueLen()==0)
	{
		return false
	}	
	foreach (index, scene in Queue)
	{
		if (scene.item == item)
		{
 				Queue.remove(index)
 				return true
 		}
	}
	return false
}


//Check to see if there's a Queued scene ready to go. If so, fire it! (and return true)
function QueueCheck()
{
	local tmp
	if (QueueLen()>0)
	{
		tmp=QueueGetNext()
		if (tmp != null)
		{
			firedfromqueue = true
			GladosPlayVcd(tmp,true)
			//GladosPlayVcd(tmp)
			return true
		}
	}
	return false
}

//--------------------------------------------------------------------------------------------------------------
//End of Queue Functions
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
//sp_sabotage_jailbreak specific functions 
//--------------------------------------------------------------------------------------------------------------

function jailbreak_alert(arg)
{
	jailbreakpos =arg
	printldebug("==========JAILBREAK ALERT "+arg)
}

function jailbreak_whoah_speech()
{
	if (jailbreakpos == 0)
	{
		GladosPlayVcd(303)
	}
	else
	{
		GladosPlayVcd(302)
	}
}


//--------------------------------------------------------------------------------------------------------------
//sp_sabotage_darkness specific functions 
//--------------------------------------------------------------------------------------------------------------


function PlayGhostStory()
{
	GladosPlayVcd(445)
}

function PlaySmellyHumansOne()
{
	GladosPlayVcd(446)
}

function PlaySmellyHumansTwo()
{
	GladosPlayVcd(447)
}
 
function PlaySmellyHumansThree()
{
	GladosPlayVcd(448)
}

function PlaySmellyHumansFour()
{
	GladosPlayVcd(449)
}


//--------------------------------------------------------------------------------------------------------------
//Glados Battle specific functions 
//--------------------------------------------------------------------------------------------------------------

function sp_sabotage_glados_specials(arg)
{
	switch (arg)
	{
		case 1: 
			SendToConsole( "scene_playvcd npc/sphere03/fgb_ready_glados06")
			break
		case 2: 
			//SendToConsole( "scene_playvcd npc/sphere03/gladosbattle_pre09")
			SendToConsole( "scene_playvcd npc/sphere03/fgb_pull_me_out01")
			break
	}
}

glados_gibbering <- false
glados_gibbering_start <- Time()
glados_gibbering_next <- 0.00
glados_gibbering_times <- {}

glados_gibbering_times[1] <- 0.7
glados_gibbering_times[2] <- 1.37
glados_gibbering_times[3] <- 1.22
glados_gibbering_times[4] <- 1.27
glados_gibbering_times[5] <- 5.29
glados_gibbering_times[6] <- 4.15
glados_gibbering_times[7] <- 3.35



function sp_sabotage_glados_start_gibberish()
{
	//gibberish is off for now
	//glados_gibbering = true
	glados_gibbering_start = Time()
	glados_gibbering_next = 0.00
}

function sp_sabotage_glados_stop_gibberish()
{
	glados_gibbering = false
}

function sp_sabotage_glados_gibberish()
{
	local i = RandomInt(1,7)
	if (Time() - glados_gibbering_start > glados_gibbering_next)
	{
		SendToConsole( "scene_playvcd npc/glados/sp_sabotage_glados_gibberish0"+i)
		glados_gibbering_start = Time()
		//glados_gibbering_next = RandomFloat(3.5, 5.0)
		glados_gibbering_next = glados_gibbering_times[i]
	}	
}



// ==============================
// player starts vault trap
// ==============================
function VaultTrapStart()
{
	GladosPlayVcd(-50)
}

// ==============================
// player begins moving in relaxation vault
// ==============================
function VaultTrapStartMoving() 
{
	GladosPlayVcd(-53)
}

// ==============================
// turrets are on the ground and begin their scene
// ==============================
function TurretScene()
{
	GladosPlayVcd(-700)
}
 
// ==============================
// ==============================
function TurretDeathReactionDialog()
{
	GladosPlayVcd(-57)
	//gladosbattle_pre_05 // my turrets!
	//gladosbattle_pre_06 // oh, you were busy back there
	//gladosbattle_pre_17 // i suppose we could just sit here and glare ... but i have a better idea
}



// ==============================
// ==============================
function WheatleyBouncingDownTubeDialog()
{
	// play various "ouch.. oof... ow..." lines here
	// play glados dialog responding to wheatley falling.  "...sigh..."
}


// ==============================
// ==============================
function WheatleyLandsInChamberDialog()
{
	GladosPlayVcd(-61)
	printldebug("==========HELLO!!!!!!!!!!!!!!!!!!!!!")
	//wheatley - gladosbattle_pre01 // Hello!
	//gladosbattle_pre_09 // i hate you so much
}

// ==============================
// ==============================
function CoreDetectedDialog()
{
	//announcer - gladosbattle02 // warning: core corrupted
	//glados - gladosbattle_xfer04 // that's funny i don't feel corrupted. in fact i feel pretty good
	//announcer - gladosbattle03 // alternate core detected
	//wheatley pre05  // ah that's me they're talking about!
	//announcer - gladosbattle03 // to initiate a core transfer deposit core in receptacle
	//ent_fire "deploy_core_receptacle_relay" trigger // deploys the core receptacle
	//glados - gladosbattle_xfer05 // core transfer?
	//glados - gladosbattle_xfer06 // oh you are kidding me
	
	// ********************************************************************************
	// ********************************************************************************
	// TODO - NAG: NEED NAG FOR 'INSERT CORE' TO PLAY UNTIL WheatleyCoreSocketed() GETS CALLED
	// ********************************************************************************
	// ********************************************************************************
}


// ==============================
// ==============================
function WheatleyCoreSocketed()
{
	// ********************************************************************************
	// ********************************************************************************
	// TODO - NAG:  END 'INSERT CORE' NAG HERE
	// ********************************************************************************
	// ********************************************************************************
	
	GladosPlayVcd(-71) // announcer - substitute core accepted.  core, are you willing to start the procedure?
	
	// ********************************************************************************
	// ********************************************************************************
	// TODO - NAG:  BEGIN 'PRESS BUTTON' NAG HERE
	// ********************************************************************************
	// ********************************************************************************
}

// ==============================
//	called when the player reaches a catapult or enters the room to press stalemate button
// ==============================
function StalemateAssociateNotSoFast()
{
	GladosPlayVcd(-84)
}


// ==============================
// ==============================
function CoreTransferInitiated()
{
	// ********************************************************************************
	// ********************************************************************************
	// TODO - NAG:  END 'PRESS BUTTON' NAG HERE
	// ********************************************************************************
	// ********************************************************************************

	GladosPlayVcd(-88) //Stalemate Resolved. Core Transfer Initiated.
}

// ==============================
// ==============================
function WheatleyCoreTransferStart()
{
	GladosPlayVcd(-4) // Here I go!
}

// ==============================
// ==============================
function PitHandsGrabGladosHead()
{
	GladosPlayVcd(-89) // Get your hands off me!
}

// ==============================
// ==============================
function PullGladosIntoPit()
{
	GladosPlayVcd(-90) // CHELL! STOP THIS! I AM YOUR MOTHER!
}

// ==============================
// ==============================
function CoreTransferCompleted()
{
	GladosPlayVcd(-9)  // Wow! Check ME out, Partner! .. Look how small you are!
} 

// ==============================
// ==============================
function PlayerEnteredElevator()
{
	GladosPlayVcd(-13) // Glados: Don't do this...
}


// ==============================
// ==============================
function DialogDuringPotatosManufacture()
{
	GladosPlayVcd(-33)
}

// ==============================
// ==============================
function PotatosPresentation()
{
	GladosPlayVcd(-34)
}

// ==============================
// ==============================
function ElevatorMoronScene()
{
	GladosPlayVcd(-37)
}

// ==============================
// ==============================
function ElevatorConclusion()
{
	GladosPlayVcd(-44)
}


//--------------------------------------------------------------------------------------------------------------
//sp_sabotage_factory functions 
//--------------------------------------------------------------------------------------------------------------

function sabotage_factory_WatchTheLine()
{
	GladosPlayVcd(441)
}

function sabotage_factory_ReachedHackingSpot()
{
	GladosPlayVcd(442)
}

function sabotage_factory_PlayerReachedWheatley()
{
	WheatleyStopNag()
	GladosPlayVcd(443)
}

function sabotage_factory_PlayerReachedExitDoor()
{
	WheatleyStopNag()
	GladosPlayVcd(444)
}	


//--------------------------------------------------------------------------------------------------------------
//sp_sabotage_factory - Science Fair functions 
//--------------------------------------------------------------------------------------------------------------
function ScienceFairGoingTheRightWay()
{
	GladosPlayVcd( -100 )
}

function ScienceFairBringDaughter()
{
	GladosPlayVcd( -101 )
}

function JustToReassureYou()
{
	GladosPlayVcd( -102 )
}

function DefinitelySureThisWay()
{
	GladosPlayVcd( -103 )
}


//--------------------------------------------------------------------------------------------------------------
//sp_catapult_fling_sphere_peek functions 
//--------------------------------------------------------------------------------------------------------------

function sp_catapult_fling_sphere_peek()
{
		switch (peekctr)
		{
			case 0:
				GladosPlayVcd(335)
				break
			case 2:
				GladosPlayVcd(362)
				break
			case 4:
				GladosPlayVcd(363)
				break
		}	
		peekctr+=1
}


//--------------------------------------------------------------------------------------------------------------
//New test functions 
//--------------------------------------------------------------------------------------------------------------
function GladosTest1(arg)
{
	foreach (index, scene in arg)
	{
		if (scene.item)
		{
			arg.remove(index)
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
//Scene Queue Functions START
//--------------------------------------------------------------------------------------------------------------

class scene {
	//constructor
	constructor(a, caller)
	{
		index = a
		owner = caller
		currentCharacter = ""
		waitSceneStart = 0 //1 means we're waiting for the current vcd to finish so we can play the next vcd in the chain
		waiting = 0  
		waitVcdCurrent = null
		waitStartTime = Time()
		waitLength = Time()
		waitNext = null 
		waitExitingEarly = false 
		waitExitingEarlyStartTime = Time() 
		waitExitingEarlyThreshold = 0.00 //How many seconds sould the VCD play before moving on to the next one
		waitNoDingOff = false
		waitNoDingOn = false
		waitVcdTeam = -1
		waitFiredVcds = []
		nagminsecs = 0
		nagmaxsecs = 0
		nags = []
		isNag = false
		nagpool = []
		naglastfetched = null
		nagrandom = false
		nagrandomonrepeat = false
		nagtimeslistcompleted = 0
		nagrepeat = false
		naginchain = false
	}
	
	function nagsClear()
	{
		naglastfetched = null
		nags.clear()
	}
	
	function nagpoolClear()
	{
		nagpool.clear()
	}


	function addFiredVcd(team)
	{
		waitFiredVcds.append(team)
	}

	function deleteFiredVcd(team)
	{
		local idx, val
		local fnd = null
		foreach (idx, val in waitFiredVcds)
		{
			if (val == team)
			{
				fnd = idx
				break
			}
		}	
		if (fnd != null)
		{
			waitFiredVcds.remove(fnd)
		}
	}

	
	//property
	index = 0;
	owner = null;
	currentCharacter = "";
	waitSceneStart = 0; //1 means we're waiting for the current vcd to finish so we can play the next vcd in the chain
	waiting = 0;  
	waitVcdCurrent = null; //SceneTable index of last launched vcd
	waitStartTime = 0;
	waitLength = 0;
	waitNext = null; 
	waitExitingEarly = false; 
	waitExitingEarlyStartTime = 0; 
	waitExitingEarlyThreshold = 0.00; //How many seconds sould the VCD play before moving on to the next one
	waitNoDingOff = false;
	waitNoDingOn = false;
	waitFires = [];
	waitVcdTeam = -1;
	waitFiredVcds = [];
	isNag = false;
	nags = [];
	nagpool = [];
	nagminsecs = 0;
	nagmaxsecs = 0;
	naglastfetched = null;
	nagrandom = false;
	nagrandomonrepeat = false;
	nagtimeslistcompleted = 0;
	nagrepeat = false;
	naginchain = false;
}


function scenequeue_AddScene(arg,char)
{
	local idx, val,delme
	delme=null
	foreach (idx, val in scenequeue)
	{
		if (SceneTable[idx].char==char)
		{
			delme = idx
		}
		if (idx == arg)
		{
			printldebug(">>>>>>>>>>Scene "+arg+" is already in the queue")
			return null
		}
	}
	if (delme != null)
	{
		printldebug(">>>>>>>>>>DELETING SCENE "+delme)
		scenequeue_DeleteScene(delme)
	}
	scenequeue[arg] <- scene(arg, this)
	scenequeue_Dump()
	return scenequeue[arg]
}

function scenequeue_DeleteScene(arg)
{
	local idx, val
	foreach (idx, val in scenequeue)
	{
		if (idx == arg)
		{
			printldebug(">>>>>>>>>>Scene "+arg+" deleted!")
			delete scenequeue[arg]
			return true
		}
	}
	return null
}

function scenequeue_Dump()
{
	
	printldebug(">>>>>>>>>>Scene Dump at "+Time())
	foreach (idx, val in scenequeue)
	{
		printldebug(">>>>>>>>>>Scene "+idx+" ADDED at "+val.waitStartTime+" Type "+ typeof val)
	}	
}

//--------------------------------------------------------------------------------------------------------------
//Scene Queue Functions END
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
//Nag Table Functions START
//--------------------------------------------------------------------------------------------------------------

function nags_init(inst,scenetableentry)
{
	local i = 0
	inst.nagsClear()
	if ("idleminsecs" in SceneTable[scenetableentry])
	{
		inst.nagminsecs = SceneTable[scenetableentry].idleminsecs
		if ("idlemaxsecs" in SceneTable[scenetableentry])
		{
			inst.nagmaxsecs = SceneTable[scenetableentry].idlemaxsecs
		}
		else
		{
			inst.nagmaxsecs = inst.nagminsecs
		}
	}
	
	if ("idlerandomonrepeat" in SceneTable[scenetableentry])
	{
		inst.nagrandomonrepeat = true
	}
	
	if ("idlerepeat" in SceneTable[scenetableentry])
	{
		inst.nagrepeat = true
	}

	if ("idlerandom" in SceneTable[scenetableentry])
	{
		inst.nagrandom = true
	}

	local igroup = SceneTable[scenetableentry].idlegroup
	local idx, val,oig=0
	foreach (idx, val in SceneTable)
	{
		if (!("idlegroup" in val))
			continue
		if (val.idlegroup != igroup)
			continue
		local rar = 101, mnum = 0
		if ("idlerarity" in val)
		{
			rar = val.idlerarity
		}
		if ("idlemaxplays" in val)
		{
			mnum = val.idlemaxplays
		}
		//Skip vcds that are part of a chain (and not the first link in the chain)
		if ("idleunder" in val)
		{
			continue
		}
		if ("idleorderingroup" in val)
		{
			oig=val.idleorderingroup
		}
		else
		{
			oig=0
		}
		inst.nags.append({SceneTableIndex=idx, rarity = rar, maxplays = mnum, totplays = 0,orderingroup = oig})
	}
	inst.nags.sort(nag_array_compare)
	inst.isNag = true
	inst.nagtimeslistcompleted = 0
	nags_createpool(inst)
}

function nags_createpool(inst)
{
	inst.nagpoolClear()
	local idx, val
	local takeit = false
	local tempa = []
	foreach (idx, val in inst.nags)
	{
		takeit=false
		if (val.totplays >= val.maxplays && val.maxplays>0)
		{
			continue
		}
		if (RandomInt(1,100)<val.rarity)
		{
			takeit=true
		}	
		if (takeit)
		{
			tempa.append(val)
		}
	}
	local r
	//The pool can still be empty at this point if only rare lines were available and none of them "made their roll".
	if (tempa.len() == 0)
		return
	
	if (inst.nagrandom || (inst.nagrandomonrepeat && inst.nagtimeslistcompleted > 0))
	{
		//Make sure the first entry in the new list isn't the same as the last vcd played.
		//This ensures no repeats.
		if (tempa.len()>1 && inst.naglastfetched!=null)
		{
			while (true) 
			{
				r=RandomInt(0,tempa.len()-1)
				if (tempa[r].SceneTableIndex != inst.naglastfetched)
				{
					inst.nagpool.append(tempa[r])
					tempa.remove(r)
					break
				}
			}
		}
		//Now build the rest of the pool
		while (tempa.len()>0)
		{
			r=RandomInt(0,tempa.len()-1)
			inst.nagpool.append(tempa[r])
			tempa.remove(r)
		}
	}
	else
	{
		foreach(idx, val in tempa)
		{
			inst.nagpool.append(val)
		}
	}	
}

function nags_nagpooldump(inst)
{
	local idx, val
	foreach (idx, val in inst.nagpool)
		printldebug("*********NAG "+idx+" : "+val.SceneTableIndex)
}

function nags_fetch(inst)
{
	if (inst.nagpool.len() == 0)
	{
		if (inst.nagrepeat)
		{
			inst.nagtimeslistcompleted += 1
			nags_createpool(inst)
			if (inst.nagpool.len() == 0)
				return null
		}
		else
		{
			return null
		}	
	}
	local ret = inst.nagpool[0].SceneTableIndex
	foreach( idx, val in inst.nags)
	{
		if (val.SceneTableIndex == ret)
		{
			val.totplays+=1
			break
		}
	}
	//nags_nagpooldump(inst)
	inst.nagpool.remove(0)
	inst.naglastfetched = ret
	return ret
}

function GladosStopNag(arg = 0)
{
	nag_stop("glados",arg)
}

function WheatleyStopNag(arg = 0)
{
	nag_stop("wheatley",arg)
}

function nag_stop(char, stoptype)
{
	local idx, val
	local todel = null
	foreach (idx, val in scenequeue)
	{
		if (val.isNag && val.currentCharacter == char)
		{
			todel=idx
			break
		}
	}
	if (todel != null)
	{
		scenequeue_DeleteScene(todel)	
	}
}

function nag_array_compare(a,b)
{
	if(a.orderingroup>b.orderingroup) return 1
	else if(a.orderingroup<b.orderingroup) return -1
	return 0;
}


//--------------------------------------------------------------------------------------------------------------
//Nag Table Functions END
//--------------------------------------------------------------------------------------------------------------

function SabotageFactoryRecycledTurretNoticesPlayer()
{
	GladosPlayVcd(439, null, "conveyor_turret")
}

function TrustFlingCatapultTurretNoticesPlayer()
{
	GladosPlayVcd(439, null, "catapulted_turret")
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
}

// a newly spawned cube or sphere was destroyed 
function GladosDroppedCubeDestroyed(arg)
{
	if ( arg == coopBlue )
	{
		EntFireByHandle( badCatchBlue, "Start", "", 0, null, null )
	}
	else
	{
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
	GladosPlayVcd(0,"glados")
}

function GladosCoopPingPortalTraining()
{
		GladosPlayVcd(35)	
}

function CoopStartCalComplete()
{
		if (CoopStartCalCompleteSet == 0){
			CoopStartCalCompleteSet = 1 
			GladosPlayVcd(1007)	
		}
}


function CoopStartBButton(player)
{
	
	if (player == coopBlue && CoopStartBButtonSet == 0){		
		CoopStartBButtonSet = 1
		GladosPlayVcd(1004)	
	}
}

function CoopStartButton(player)
{
	
	if (player == coopOrange && CoopStartButtonSet == 0){		
		CoopStartButtonSet = 1
		GladosPlayVcd(1005)	
	}
}

function CoopStartBoxButton(player)
{
	CoopStartBoxButtonSet++
	if (CoopStartBoxButtonSet == 2){
		GladosPlayVcd(1006)
	}
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
	PrivateMixVcds[9]  <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BLUE06.vcd")	
	PrivateMixVcds[10] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_BLUE03.vcd")
	

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
	PrivateBothVcds[9]  <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_ORANGE06.vcd")		
	PrivateBothVcds[10] <- CreateSceneEntity("scenes/npc/glados/COOP_TEST_CHAMBER_ORANGE03.vcd")

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
		local mapname = GetMapName()
		switch (mapname){		
			case "mp_coop_catapult_1": 
				if (arg==coopBlue){
					break
				}
				else{
					break
				}	
		}
	}

	function GladosAnnouncePlayerScore(arg,low,hi)
	{
		local mapname = GetMapName()
		switch (mapname)
		{
	
			case "mp_coop_doors": 
				if (arg==coopBlue){
					GladosPlayVcd(1012)
					break
				}
				else{
					GladosPlayVcd(1013)
					break
				}	
			case "mp_coop_fling_3":
				if (arg==coopBlue){
//					GladosPlayVcd(1012)
					break
				}
				else{
	//				GladosPlayVcd(1013)  turned off for now, not on right track.
					break
				}	

		}

		printldebug("2222222222222222222222222222222222")
	}

	function GladosStateScore()
	{
		printldebug("333333333333333333333333333333")
	}

	function GladosStateScienceCollaborationPoints()
	{
				printldebug("444444444444444444444444444444")
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
function CoopRaceButtonPress(player){
	if (player==coopBlue)
	{
		GladosPlayVcd(1012)	
	}
	else
	{
		GladosPlayVcd(1013)	
	}
}

function CoopStartHandOff()
{
}

function CoopStartBoxCatch()
{
		GladosPlayVcd(1023)
}


function GladosPingIntroDone()
{
	BlueInPortalTraining = 1
	OrangeInPortalTraining = 1
}

function CoopStartTwoPortals(){
	GladosPlayVcd(1002)
}

function CoopRadarRoom(){
  LastDeathTime=Time()+5000 //extra time added so death responses will not fire
	GladosPlayVcd(1009)
	EndSpeedRunTimer()
}

function CoopBlueprintRoom(){
  LastDeathTime=Time()+5000 //extra time added so death responses will not fire
	GladosPlayVcd(1055) 
	EndSpeedRunTimer()
}
function CoopSecutiryRoom(){
  LastDeathTime=Time()+5000 //extra time added so death responses will not fire
	GladosPlayVcd(1196)
	EndSpeedRunTimer()
}

function CoopTbeamSecurity()
{
	LastDeathTime=Time()+5000 //extra time added so death responses will not fire
	GladosPlayVcd(1094)

	// Award the STAYING_ALIVE achievement here, before GLADOS nukes the players.
	// If no one died...
	local nTotalDeaths = GetPlayerDeathCount( 0 ) + GetPlayerDeathCount( 1 );
	if ( nTotalDeaths == 0 )
	{
		// ...and both players completed every map in the branch during this session.
		if ( CoopGetLevelsCompletedThisBranch() == CoopGetBranchTotalLevelCount(STAYING_ALIVE_SECTION-1)-1 )
		{
			RecordAchievementEvent( "ACH.STAYING_ALIVE", GetBluePlayerIndex() )
			RecordAchievementEvent( "ACH.STAYING_ALIVE", GetOrangePlayerIndex() )
		}
	}

	EndSpeedRunTimer()
}

function RespondToTaunt(tauntnumber){
	printldebug("-------------------------------------------RESPONDTOTAUNT"+tauntnumber)
	local curMapName = GetMapName()
	//taunt 0 given at start
	LastTauntTime=Time()
	switch (curMapName)
	{
		case "mp_coop_start":  
			if (tauntnumber == 1)
			{
				GladosPlayVcd(1064)
			}
			break

		case "mp_coop_laser_2":  
			if (tauntnumber == 2)
			{
				GladosPlayVcd(1065)
			}
			break
		case "mp_coop_rat_maze":  
			if (tauntnumber == 3)
			{
				GladosPlayVcd(1017)
			}
			break
		case "mp_coop_catapult_1":  
			if (tauntnumber == 5)
			{
				GladosPlayVcd(1051)
			}
			break
		case "mp_coop_lobby_2":
			if (tauntnumber == 4){		  			
				//moved ot hub stuff
			}
			break
		case "mp_coop_catapult_wall_intro":
			if (tauntnumber == 7){		  			
				GladosPlayVcd(1097)
			}
			break		
		case "mp_coop_wall_5":
			if (tauntnumber == 8){		  			
				GladosPlayVcd(1098)
			}
			break					
		case "mp_coop_tbeam_laser_1":
			if (tauntnumber == 9){		  			
				GladosPlayVcd(1099)
			}
			break								
	}
}

function CoopHubTrack01(){
	GladosPlayVcd(1020)	
}
function CoopHubTrack02(){
	InHub2=1
	GladosPlayVcd(1095)	
}

function CoopHubTrack03(){
	GladosPlayVcd(1062)	
}

function CoopHubTrack04(){
	GladosPlayVcd(1173)	
	
}

function CoopHubTrack05(){
	GladosPlayVcd(1130)
}



function CoopRaceButton(arg)
{
	if (arg==coopBlue)
	{
		GladosPlayVcd(1026)
	}
	else
	{
		GladosPlayVcd(1027)
	}	
}

function CoopLaserTurretkill()
{
	GladosPlayVcd(1028)
}

function CoopMazeOrangeCrush(arg){
	if (arg == 1){
		OrangeInMaze = 1
	}
	else{
		OrangeInMaze = 0
	}
}

function CoopMazeBlueCrush(arg){
	if (arg == 1){
		BlueInMaze = 1
	}
	else{
		BlueInMaze = 0
	}
}

function CoopCrushersBlueCrush(arg){
	if (arg == 1){
		BlueInCrusher = 1
	}
	else{
		BlueInCrusher = 0
	}
}

function CoopCrushersOrangeCrush(arg){
	if (arg == 1){
		OrangeInCrusher = 1
	}
	else{
		OrangeInCrusher = 0
	}
}

function CoopRedirectAhead(player){
	if (player == coopBlue){
		GladosPlayVcd(1092)
	}
	else{
		GladosPlayVcd(1093)	
	}
	
}


function Fling1Drop4Balls(){
		GladosPlayVcd(1198)	
}





function BotDeath(player,dmgtype)
{
	printldebug("*******************DEATH***************************")
	printldebug(player)
	printldebug(dmgtype)
	printldebug(HumanResourceDeath4)
	printldebug(LastDeathTime)
	printldebug(mp_coop_tbeam_laser_1death1)
	printldebug(mp_coop_tbeam_laser_1death2)
	printldebug("*******************DEATH***************************")
	local curTime=Time()
	if (curTime-LastDeathTime<10){
		return
	}
	LastDeathTime=Time()
		
	if (curMapName == "mp_coop_tbeam_polarity3" && mp_coop_tbeam_polarity3deathblue==1 && player== coopBlue){		
		mp_coop_tbeam_polarity3deathblue=2
		GladosPlayVcd(1322)	
		return
	}
	if (curMapName == "mp_coop_tbeam_polarity3" && mp_coop_tbeam_polarity3deathblue==0 && player== coopBlue){		
		mp_coop_tbeam_polarity3deathblue=1
		GladosPlayVcd(1323)	
		return
	}

	if (curMapName == "mp_coop_tbeam_polarity3" && mp_coop_tbeam_polarity3deathorange==1 && player== coopOrange){		
		mp_coop_tbeam_polarity3deathorange=2
		GladosPlayVcd(1325)	
		return
	}
	if (curMapName == "mp_coop_tbeam_polarity3" && mp_coop_tbeam_polarity3deathorange==0 && player== coopOrange){		
		mp_coop_tbeam_polarity3deathorange=1
		GladosPlayVcd(1324)	
		return
	}


	
	
	
	if (curMapName == "mp_coop_tbeam_drill" && mp_coop_tbeam_drilldeathblue==0 && player== coopBlue){		
		mp_coop_tbeam_drilldeathblue=1
		GladosPlayVcd(1318)	
		return
	}
	if (curMapName == "mp_coop_tbeam_drill" && mp_coop_tbeam_drilldeathorange==0 && player== coopOrange){		
		mp_coop_tbeam_drilldeathorange=1
		GladosPlayVcd(1319)	
		return
	}

	if (curMapName == "mp_coop_tbeam_polarity" && mp_coop_tbeam_polaritydeathblue==0 && player== coopBlue){		
		mp_coop_tbeam_polaritydeathblue=1
		GladosPlayVcd(1320)	
		return
	}
	if (curMapName == "mp_coop_tbeam_polarity" && mp_coop_tbeam_polaritydeathorange==0 && player== coopOrange){		
		mp_coop_tbeam_polaritydeathorange=1
		GladosPlayVcd(1321)	
		return
	}


	if (curMapName == "mp_coop_tbeam_redirect" && mp_coop_tbeam_redirectdeath==0){		
		mp_coop_tbeam_redirectdeath=1
		GladosPlayVcd(1316)	
		return
	}
	if (curMapName == "mp_coop_lobby_2"){		
		local tauntflag3 = GetGladosSpokenFlags( 3 ) //used for deaths
		if ((tauntflag3 & (1 << 1)) == 0){
			tauntflag3 = tauntflag3 + (1 << 1)
			AddGladosSpokenFlags( 3, tauntflag3 )
			GladosPlayVcd(1199)
			return
		}
		if ((tauntflag3 & (1 << 2)) == 0){
			tauntflag3 = tauntflag3 + (1 << 2)
			AddGladosSpokenFlags( 3, tauntflag3 )
			GladosPlayVcd(1200)
			return
		}
		if ((tauntflag3 & (1 << 0)) == 0){
			tauntflag3 = tauntflag3 + (1 << 0)
			AddGladosSpokenFlags( 3, tauntflag3 )
			GladosPlayVcd(1096)
			return
		}		
		if ((tauntflag3 & (1 << 3)) == 0){
			tauntflag3 = tauntflag3 + (1 << 3)
			AddGladosSpokenFlags( 3, tauntflag3 )
			GladosPlayVcd(1201)
			return
		}
		if ((tauntflag3 & (1 << 4)) == 0){
			tauntflag3 = tauntflag3 + (1 << 4)
			AddGladosSpokenFlags( 3, tauntflag3 )
			GladosPlayVcd(1202)
			return
		}
	}
	if (curMapName == "mp_coop_turret_walls" && mp_coop_turret_wallscount==9){		
		GladosPlayVcd(1192)	
	}

	if (curMapName == "mp_coop_turret_walls" && mp_coop_turret_wallscount==4){		
		GladosPlayVcd(1193)	
	}
	
	if (curMapName == "mp_coop_turret_walls" && mp_coop_turret_wallscount==2){		
		GladosPlayVcd(1191)	
	}
	
	if (curMapName == "mp_coop_turret_walls" && mp_coop_turret_wallscount==0){		
		GladosPlayVcd(1153)
	}	
	
	if (curMapName == "mp_coop_turret_walls"){		
		mp_coop_turret_wallscount=mp_coop_turret_wallscount+1
	}
	
	
	if (curMapName == "mp_coop_turret_ball" && mp_coop_turret_ballcount==2){		
		if (player==2){
			GladosPlayVcd(1189)	
		}
		else{
			GladosPlayVcd(1188)	
		}
		mp_coop_turret_ballcount=3
	}
	
	if (curMapName == "mp_coop_turret_ball" && mp_coop_turret_ballcount==1){
		mp_coop_turret_ballcount=2
		GladosPlayVcd(1190)	
	}
	if (curMapName == "mp_coop_turret_ball" && mp_coop_turret_ballcount==0){
		mp_coop_turret_ballcount=1
		GladosPlayVcd(1187)	
	}	
		
	if (curMapName == "mp_coop_tbeam_end" ){
		local fireddeath = 0
		if (dmgtype==2 && Tbeam_enddeathturret==0){
				Tbeam_enddeathturret=1
				fireddeath=1
				GladosPlayVcd(1180)	
		}
		if (dmgtype==32 && Tbeam_enddeathfall==0){
			Tbeam_enddeathfall=1
			fireddeath=1
			GladosPlayVcd(1179)	
		}
		if (Tbeam_enddeathother==2 && fireddeath==0)
		{
			GladosPlayVcd(1181)
			Tbeam_enddeathother=Tbeam_enddeathother+1
		}
		if (Tbeam_enddeathother==1 && fireddeath==0)
		{
			GladosPlayVcd(1182)
			Tbeam_enddeathother=Tbeam_enddeathother+1
		}
		if (Tbeam_enddeathother==0 && fireddeath==0)
		{
			GladosPlayVcd(1183)
			Tbeam_enddeathother=Tbeam_enddeathother+1
		}		
		Tbeam_enddeathcounter=Tbeam_enddeathcounter+1
		
	} 

	
	if (curMapName == "mp_coop_rat_maze" )
	{
		if (player== coopBlue && dmgtype == 1 && BlueInMaze == 1 && CoopMazeBlueCrushset == 2)
		{
				CoopMazeBlueCrushset = 3
				GladosPlayVcd(1047)	
		}
		if (player== coopOrange && dmgtype == 1 && OrangeInMaze == 1  && CoopMazeOrangeCrushset == 2)
		{
				CoopMazeOrangeCrushset = 3
				GladosPlayVcd(1048)	
		}					
		if (player==coopBlue && dmgtype == 1 && BlueInMaze == 1 && CoopMazeBlueCrushset == 1)
		{
				CoopMazeBlueCrushset = 2
				GladosPlayVcd(1045)	
		}
		if (player== coopOrange && dmgtype == 1 && OrangeInMaze == 1  && CoopMazeOrangeCrushset == 1)
		{
				CoopMazeOrangeCrushset = 2
				GladosPlayVcd(1046)	
		}				

		if (player==coopBlue && dmgtype == 1 && BlueInMaze == 1 && CoopMazeBlueCrushset == 0)
		{
				CoopMazeBlueCrushset = 1
				GladosPlayVcd(1043)	
		}
		if (player== coopOrange && dmgtype == 1 && OrangeInMaze == 1  && CoopMazeOrangeCrushset == 0)
		{
				CoopMazeOrangeCrushset = 1
				GladosPlayVcd(1044)	
		}		


	}

	if (curMapName == "mp_coop_laser_crusher")
	{
		mp_coop_laser_crusherdeath=1
		if (player==coopBlue && dmgtype == 1 && BlueInCrusher == 1 && CoopCrushersBlueCrushset == 1)
		{
				CoopCrushersOrangeCrushset = 2
				CoopCrushersBlueCrushset = 2
				GladosPlayVcd(1042)	
		}
		if (player== coopOrange && dmgtype == 1 && OrangeInCrusher == 1  && CoopCrushersOrangeCrushset == 1)
		{
				CoopCrushersOrangeCrushset = 2
				CoopCrushersBlueCrushset = 2
				GladosPlayVcd(1042)	
		}				

		if (player==coopBlue && dmgtype == 1 && BlueInCrusher == 1 && CoopCrushersBlueCrushset == 0)
		{
				CoopCrushersBlueCrushset = 1
				GladosPlayVcd(1040)	
		}
		if (player== coopOrange && dmgtype == 1 && OrangeInCrusher == 1  && CoopCrushersOrangeCrushset == 0)
		{
				CoopCrushersOrangeCrushset = 1
				GladosPlayVcd(1041)	
		}		
		
	}
	if (curMapName == "mp_coop_start")
	{
		if (startdeath==3)
		{
			GladosPlayVcd(1086)
			startdeath=startdeath+1

		}
		if (startdeath==2)
		{
			GladosPlayVcd(1085)
			startdeath=startdeath+1

		}
		if (startdeath==1)
		{
			GladosPlayVcd(1084)
			startdeath=startdeath+1

		}
		if (player==coopBlue && startdeath==0)
		{
			startdeath=startdeath+1
			GladosPlayVcd(1088)

		}
		if (player==coopOrange && startdeath==0)
		{
			startdeath=startdeath+1
			GladosPlayVcd(1087)
		}
	}
	if (curMapName == "mp_coop_lobby_2" && InHub2==1 && InHub2Set==0 )	{
			InHub2Set=1
			GladosPlayVcd(1096)
	}
	if (curMapName == "mp_coop_wall_2")	{
			mp_coop_wall_2death=mp_coop_wall_2death+1
	}
	if (curMapName == "mp_coop_paint_redirect" && HumanResourceDeath1==2 ){
		HumanResourceDeath1=3
		GladosPlayVcd(1294)	
	}

	if (curMapName == "mp_coop_paint_redirect" && HumanResourceDeath1==1 ){
		HumanResourceDeath1=2
		GladosPlayVcd(1293)	
	}

	if (curMapName == "mp_coop_paint_redirect" && HumanResourceDeath1==0 ){
		HumanResourceDeath1=1
		GladosPlayVcd(1145)	
	}
	if (curMapName == "mp_coop_paint_bridge" && HumanResourceDeath2==1){
		HumanResourceDeath2=2
		GladosPlayVcd(1296)	
	}
	if (curMapName == "mp_coop_paint_bridge" && HumanResourceDeath2==0){
		HumanResourceDeath2=1
		GladosPlayVcd(1146)	
	}

	if (curMapName == "mp_coop_paint_walljumps" && HumanResourceDeath3==0){
		HumanResourceDeath3=1
		GladosPlayVcd(1150)	
	}
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==5){
		HumanResourceDeath4=6
		GladosPlayVcd(1186)	
	}	
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==4){
		HumanResourceDeath4=HumanResourceDeath4+1
	}		
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==3){
		HumanResourceDeath4=HumanResourceDeath4+1
	}	
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==2){
		HumanResourceDeath4=3
		GladosPlayVcd(1185)	
	}
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==1){
		HumanResourceDeath4=2
		GladosPlayVcd(1184)	
	}
	if (curMapName == "mp_coop_paint_red_racer" && HumanResourceDeath4==0){
		HumanResourceDeath4=1
		GladosPlayVcd(1148)	
	}
	if (curMapName == "mp_coop_paint_speed_catch" && HumanResourceDeath5==0){
		HumanResourceDeath5=1
		GladosPlayVcd(1149)	
	}
	if (curMapName == "mp_coop_paint_speed_fling" && HumanResourceDeath6==0){
		HumanResourceDeath6=1
		GladosPlayVcd(1299)	
	}
		if (curMapName == "mp_coop_paint_longjump_intro" && HumanResourceDeath7==3){
		HumanResourceDeath7=4
		GladosPlayVcd(1303)	
	}
		if (curMapName == "mp_coop_paint_longjump_intro" && HumanResourceDeath7==2){
		HumanResourceDeath7=3
		GladosPlayVcd(1302)	
	}
	if (curMapName == "mp_coop_paint_longjump_intro" && HumanResourceDeath7==1){
		HumanResourceDeath7=2
		GladosPlayVcd(1301)	
	}
	if (curMapName == "mp_coop_paint_longjump_intro" && HumanResourceDeath7==0){
		HumanResourceDeath7=1
		GladosPlayVcd(1300)	
	}
	
 

	if (curMapName == "mp_coop_race_2"&& EarlyDeath1==0 ){
		EarlyDeath1=1
		GladosPlayVcd(1151)	
	}
	if (curMapName == "mp_coop_multifling_1" &&  EarlyDeath2==0){
		EarlyDeath2=1
		GladosPlayVcd(1152)	
		//add this line to ending if earlydeath2!=1
	}
	if (curMapName == "mp_coop_fling_crushers" && EarlyDeath3==2){
		EarlyDeath3=3
		GladosPlayVcd(1178)	
	}

	if (curMapName == "mp_coop_fling_crushers" && EarlyDeath3==1){
		EarlyDeath3=2
		GladosPlayVcd(1177)	
	}
	if (curMapName == "mp_coop_fling_crushers" && EarlyDeath3==0){
		EarlyDeath3=1
		GladosPlayVcd(1147)	
	}


	if (curMapName == "mp_coop_come_along" && EarlyDeath4==0){
		EarlyDeath4=1
		GladosPlayVcd(1154)	
	}
	if (curMapName == "mp_coop_catapult_1" && EarlyDeath5==0){
		EarlyDeath5=1
		GladosPlayVcd(1155)	
	}
	if (curMapName == "mp_coop_tbeam_laser_1" ){
		if (player==2 && mp_coop_tbeam_laser_1death1==0){
				mp_coop_tbeam_laser_1death1=1
				GladosPlayVcd(1204)	
				return
		}
		if (player==1 && mp_coop_tbeam_laser_1death2==0){
				mp_coop_tbeam_laser_1death2=1
				GladosPlayVcd(1206)	
		}
		if (player==2 && mp_coop_tbeam_laser_1death3==1){
				mp_coop_tbeam_laser_1death1=2
				GladosPlayVcd(1205)	
		}
	}
	if (curMapName == "mp_coop_infinifling_train" && mp_coop_infinifling_traindeath01==0){
		mp_coop_infinifling_traindeath01=1
		GladosPlayVcd(1255)	
	}
	if (curMapName == "mp_coop_infinifling_train" ){
			mp_coop_infinifling_traindeath=mp_coop_infinifling_traindeath+1
	}
	
	if (curMapName == "mp_coop_catapult_wall_intro" && mp_coop_catapult_wall_introdeath==0){
		mp_coop_catapult_wall_introdeath=1
		GladosPlayVcd(1257)	
	}
	if (curMapName == "mp_coop_wall_block" && mp_coop_wall_blockdeath==0){
		mp_coop_wall_blockdeath=1
		GladosPlayVcd(1258)	
	}
}

function CoopCrushersBox(player){
		local x = RandomInt(1,100)
		if (x>50)
		{
			GladosPlayVcd(1012)	
		}
		else
		{
			GladosPlayVcd(1013)	
		}
}


function GladosCoopMapStart(){
	local curMapName = GetMapName()
	switch (curMapName)
	{
///////Calibration Course
			///function handled elsewhere	
///////Team Building Course			
		case "mp_coop_doors":  //introduction
			GladosPlayVcd(54)
			break
		case "mp_coop_race_2":  //human sports - fewest possible points
			GladosPlayVcd(1022)
			break			
		case "mp_coop_laser_2": // potentially lethal
			GladosPlayVcd(1016)
			break
		case "mp_coop_rat_maze": //forged partnership - one lazy
			
			break
		case "mp_coop_laser_crusher":  //not a competition but if it were...
			local x = RandomInt(1,100)
			if (x>50){
				GladosPlayVcd(1018)
			}
			else{
				GladosPlayVcd(1019)
			}
			break
		case "mp_coop_teambts":  //outside the box.
			GladosPlayVcd(1021)
			break
		
///////Confidence Building Course						
		case "mp_coop_fling_3":
			GladosPlayVcd(1029)
			break
		case "mp_coop_infinifling_train":
			GladosPlayVcd(1031)
			break
		case "mp_coop_come_along":
			GladosPlayVcd(1056)
			break		
		case "mp_coop_fling_1":
			GladosPlayVcd(1032)
			break
		case "mp_coop_catapult_1":
			GladosPlayVcd(1057)
			break
		case "mp_coop_multifling_1":
			GladosPlayVcd(1058)
			break
		case "mp_coop_fling_crushers":
			GladosPlayVcd(1059)
			break
		case "mp_coop_fan":
			GladosPlayVcd(1054)
			break

/////////Obstacle  Building Course
		case "mp_coop_wall_intro": 
			GladosPlayVcd(1111)
			break
		case "mp_coop_wall_2":
			GladosPlayVcd(1118)
			break
		case "mp_coop_catapult_wall_intro": 
			GladosPlayVcd(1117)
			break
		case "mp_coop_wall_block": 
			GladosPlayVcd(1119)
			break
		case "mp_coop_catapult_2": 
			GladosPlayVcd(1121)
			break
		case "mp_coop_turret_walls": //check this is in ?
			GladosPlayVcd(1122)
			break			
		case "mp_coop_turret_ball": 
			GladosPlayVcd(1083)
			break			
		case "mp_coop_wall_5": 
			GladosPlayVcd(1060)
			break



	
	///////Subterfuge Building Course
		case "mp_coop_tbeam_redirect":
			GladosPlayVcd(1125)	
			break		
		case "mp_coop_tbeam_drill":
			if (IsLocalSplitScreen()){
				GladosPlayVcd(1306)
			}
			else{
				GladosPrivateTalk(2,10)		
			}
			break
		case "mp_coop_tbeam_catch_grind_1":
			local x = RandomInt(1,100)
			if (x>50)
			{
				GladosPlayVcd(1080)	
				bluetrust=1
			}
			else
			{
				GladosPlayVcd(1081)	
				orangetrust=1
			}
			break
		case "mp_coop_tbeam_laser_1":
			GladosPlayVcd(1073)	
			break
		case "mp_coop_tbeam_polarity":
			local x = RandomInt(1,100)
			if (x>50)
			{
				GladosPlayVcd(1069)	
				bluetrust=1
			}
			else
			{
				GladosPlayVcd(1070)	
				orangetrust=1
			}
			break
		case "mp_coop_tbeam_polarity2":
			local x = RandomInt(1,100)
			if (x>50)
			{
				GladosPlayVcd(1090)	
			}
			else
			{
				GladosPlayVcd(1091)	
			}
			break		
		case "mp_coop_tbeam_polarity3":
			GladosPlayVcd(1261)	

			break		
		case "mp_coop_tbeam_maze":
			GladosPlayVcd(1127)	
			break		
		case "mp_coop_tbeam_end":
				GladosPlayVcd(1129)	
//			GladosPrivateTalk(2,9)		
			break

/////////XXXXXX  Building Course
		case "mp_coop_paint_come_along":
			GladosPlayVcd(1131)	
			break		
		case "mp_coop_paint_redirect":
			GladosPlayVcd(1133)	
			break		
		case "mp_coop_paint_bridge":
			GladosPlayVcd(1135)	
			break		
		case "mp_coop_paint_walljumps":
			GladosPlayVcd(1137)	
			break		
		case "mp_coop_paint_speed_fling":
			GladosPlayVcd(1139)	
			break		
		case "mp_coop_paint_red_racer":
			GladosPlayVcd(1141)	
			summer_sale_cube_died = false;
			break		
		case "mp_coop_paint_speed_catch":
			GladosPlayVcd(1175)	
			break		
		case "mp_coop_paint_longjump_intro":
			GladosPlayVcd(1143)	
			break
	}

	StartSpeedRunTimer()
}


//plays when exit door opens
function GladosCoopOpenExitDoor(player)
{
	//endmap stuff
	local mapname = GetMapName()
	printldebug("****=================================Player "+player+" map NEW CODE"+mapname)
	if (debug)
		printldebug("****=================================Player "+player+" map "+mapname)
	if (coopTriggeredElevator)
	{
		return
	}
	coopTriggeredElevator = true
	EntFire( "@relay_disable_exit", "Trigger", "", 0.0 )
	switch (mapname)
	{
///////Calibration Course
		case "mp_coop_start": //DONE
			EntFire( "@relay_enable_exit", "Trigger", "", 0 )
			//plays at the end of the taunt stuff
			break	

///////Team Building Course
		case "mp_coop_doors": 	//Done
			GladosPlayVcd(1011)
			break	
		case "mp_coop_race_2": 	//Done
			EntFire( "@relay_enable_exit", "Trigger", "", 0 )			
			// plays off putting ball in CoopRacePlaceBox
			break
		case "mp_coop_laser_2": //Done
			GladosPlayVcd(18)
			break
		case "mp_coop_rat_maze": //Done
			local x = RandomInt(1,100)
			if (x>50){
				GladosPlayVcd(1036)
			}
			else{
				GladosPlayVcd(1037)
			}
			break
		case "mp_coop_laser_crusher": // Done
			if (mp_coop_laser_crusherdeath==0){
				if (player == coopBlue){
					GladosPlayVcd(23)
				}
				else{
					GladosPlayVcd(24)
				}
			}
			else{
					GladosPlayVcd(1170)
			}
			break



///////Confidence Building Course			
		case "mp_coop_fling_3":
			GladosPlayVcd(1030)
			break
		case "mp_coop_infinifling_train":
			if (mp_coop_infinifling_traindeath>0){
				GladosPlayVcd(1049)
			}
			else{
				GladosPlayVcd(1050)
			}
			break
		case "mp_coop_come_along":
			if (EarlyDeath4==0){
				GladosPlayVcd(1172)
			}
			else{
				GladosPlayVcd(1171)
			}
//			GladosPlayVcd(1034) - removed
			break		
		case "mp_coop_fling_1":  //deliberatly blank
			EntFire( "@relay_enable_exit", "Trigger", "", 0.0 )
			break
		case "mp_coop_catapult_1":  	
			EntFire( "@relay_enable_exit", "Trigger", "", 0.0 )
			//end speech fired in taunt response

			break
		case "mp_coop_multifling_1": // Done
			if (EarlyDeath2==0){
				GladosPlayVcd(1162)	
			}
			else{
				GladosPlayVcd(1052)
			}
			break					
		case "mp_coop_fling_crushers": //Map 6
			if (EarlyDeath3>0){
					GladosPlayVcd(1176)
			}
			else{
					GladosPlayVcd(1053)
			}
			break
		////mp_coop_fan end with CoopBlueprintRoom function	
			
///////Obstacle Building Course		
		case "mp_coop_wall_intro": 
			GladosPlayVcd(1063)
			break
		case "mp_coop_wall_2":
			if (mp_coop_wall_2death==0){
				GladosPlayVcd(1113)
				break
			}
			if (mp_coop_wall_2death==1){
				GladosPlayVcd(1115)
				break
			}
			if (mp_coop_wall_2death==2){
				GladosPlayVcd(1116)
				break
			}
			GladosPlayVcd(1114)
			break
		case "mp_coop_catapult_wall_intro": 
			GladosPlayVcd(1082)
			break
		case "mp_coop_wall_block": 
			if (mp_coop_wall_blockdeath==1){
				GladosPlayVcd(1259)
			}
			else{
				GladosPlayVcd(1120)
			}
			break
		case "mp_coop_catapult_2": 
				GladosPlayVcd(1144)
				break


		case "mp_coop_turret_walls": 
			if (mp_coop_turret_wallscount==0){
				GladosPlayVcd(1123)
			}
			else{
				GladosPlayVcd(1260)
			}
			break			
		case "mp_coop_turret_ball": 
			GladosPlayVcd(1124)
			break		
		case "mp_coop_wall_5": 
			GladosPlayVcd(15)
			break
///////Subterfuge Buidling Course
		case "mp_coop_tbeam_redirect":
			//moved to normal ending as one in front is rarely end.
			break		
		case "mp_coop_tbeam_drill":
			if (IsLocalSplitScreen()){
				GladosPlayVcd(1307)
			}
			else{
				GladosPrivateTalk(2,4)		
				EntFire( "@relay_enable_exit", "Trigger", "", 15.0 )			
			}

			break		
		case "mp_coop_tbeam_catch_grind_1":
			// chet, I swapped this withtbeam_laser_1 so we can grant the trickfire there -mtw
			GladosPlayVcd(1256)	
			break		
		case "mp_coop_tbeam_laser_1":
			//none here anymore because we gran the trickfire here -mtw			
			break		
		case "mp_coop_tbeam_polarity":
			if (bluetrust==1)
			{
				GladosPlayVcd(1071)	
			}
			else
			{
				GladosPlayVcd(1072)	
			}
			break		
		case "mp_coop_tbeam_polarity2":
			printldebug("-------------------------------->"+polarity2whisper)
			if (polarity2whisper == 2){		
				GladosPlayVcd(1309)	
			}
			else{
				GladosPlayVcd(1308)	
			}
			break		
		case "mp_coop_tbeam_polarity3":
			GladosPlayVcd(1174)	
			break		
		case "mp_coop_tbeam_maze":
				GladosPlayVcd(1128)		
//			GladosPrivateTalk(2,5)		
	//		EntFire( "@relay_enable_exit", "Trigger", "", 10.0 )			
			break		
		case "mp_coop_tbeam_end":
			GladosPlayVcd(15)		
			break

/////////XXXXXX  Building Course
		case "mp_coop_paint_come_along":
			GladosPlayVcd(1132)		
			break		
		case "mp_coop_paint_redirect":
			GladosPlayVcd(1295)		
			break		
		case "mp_coop_paint_bridge":
			if (HumanResourceDeath2>0){
					GladosPlayVcd(1297)		
			}
			else{
					GladosPlayVcd(1298)		
			}
			break		
		case "mp_coop_paint_walljumps":
			GladosPlayVcd(1138)		
			break		
		case "mp_coop_paint_speed_fling":
			GladosPlayVcd(1140)
			break		
		case "mp_coop_paint_red_racer":
			GladosPlayVcd(1142)		
			if ( summer_sale_cube_died == false )
			{
				// Award the achievement here
				RecordAchievementEvent( "ACH.SUMMER_SALE", GetBluePlayerIndex() )
				RecordAchievementEvent( "ACH.SUMMER_SALE", GetOrangePlayerIndex() )
			}		
			break		
		case "mp_coop_paint_speed_catch":
			GladosPlayVcd(1262)		
			break		
		case "mp_coop_paint_longjump_intro":
			GladosPlayVcd(15)		
			break
	}

	EndSpeedRunTimer()

	CheckAchievementsOnExitDoorOpen()
}

function CheckAchievementsOnExitDoorOpen()
{
	// If the player completed the specified map using only a few portals.
	if ( GetMapName() == LIMITED_PORTALS_MAP &&
	     CoopGetNumPortalsPlaced() <= LIMITED_PORTALS_COUNT )
	{
		RecordAchievementEvent( "ACH.LIMITED_PORTALS", GetBluePlayerIndex() )
		RecordAchievementEvent( "ACH.LIMITED_PORTALS", GetOrangePlayerIndex() )
	}
}

function StartSpeedRunTimer()
{
	mp_coop_speed_run_time = Time()
}

function EndSpeedRunTimer()
{
	local run_length = Time() - mp_coop_speed_run_time
	SetMapRunTime( run_length )
}

function SetMapRunTime( flTime )
{
	// cap it at two hours
	if ( flTime > 7200.0 )
	{
		flTime = 7200.0
	}

	CoopSetMapRunTime( flTime )
	 
	if ( flTime < SPEED_RUN_THRESHOLD &&
	     GetCoopSectionIndex() == SPEED_RUN_SECTION )
	{
		NotifySpeedRunSuccess( flTime, GetMapName() )
	}
}

function CoopRacePlaceBox()
{
	local mapname = GetMapName()
	if (mapname == "mp_coop_race_2")
	{
		GladosPlayVcd(19)
	}
}
			


//Plays the final audio for each level
function GladosCoopElevatorEntrance(arg)
{
	printldebug("====================================Player "+arg+" OLD CODE")
	local mapname = GetMapName()
	switch (mapname)
	{
/////////Calibration Course
//		case "mp_coop_start": //DONE
//			EntFire( "@relay_enable_exit", "Trigger", "", 0.0 )
//			//plays at the end of the taunt stuff
//			break	
//
/////////Team Building Course
//		case "mp_coop_doors": 	//Done
//			GladosPlayVcd(1011)
//			break	
//		case "mp_coop_race_2": 	//Done
//			GladosPlayVcd(19)
//			break
//		case "mp_coop_laser_2": //Done
//			GladosPlayVcd(18)
//			break
//		case "mp_coop_rat_maze": //Done
//			local x = RandomInt(1,100)
//			if (x>50){
//				GladosPlayVcd(1036)
//			}
//			else{
//				GladosPlayVcd(1037)
//			}
//			break
//		case "mp_coop_laser_crusher": // Done
//			if (arg == coopBlue)
//				GladosPlayVcd(23)
//			else
//				GladosPlayVcd(24)
//			break
//		//mp_coop_teambts end with GladosCoopEnterRadarRoom function	
//
//
/////////Confidence Building Course			
//		case "mp_coop_fling_3":
//			GladosPlayVcd(1030)
//			break
//		case "mp_coop_infinifling_train":
//			if (mp_coop_infinifling_traindeath>0){
//				GladosPlayVcd(1049)
//			}
//			else{
//				GladosPlayVcd(1050)
//			}
//			break
//		case "mp_coop_come_along":
//			GladosPlayVcd(1034)
//			break		
//		case "mp_coop_fling_1":  //deliberatly blank
//			EntFire( "@relay_enable_exit", "Trigger", "", 0.0 )
//			break
//		case "mp_coop_catapult_1":  	
//			EntFire( "@relay_enable_exit", "Trigger", "", 0.0 )
//			//end speech fired in taunt response
//
//			break
//		case "mp_coop_multifling_1": // Done
//			GladosPlayVcd(1052)
//			break					
//		case "mp_coop_fling_crushers": //Map 6
//			GladosPlayVcd(1053)
//			break
//		////mp_coop_fan end with CoopBlueprintRoom function	
//			
/////////Obstacle Building Course		
//		case "mp_coop_wall_intro": 
//			GladosPlayVcd(1082)
//			break
//		case "mp_coop_wall_2":
//			GladosPlayVcd(15)
//			break
//		case "mp_coop_catapult_wall_intro": 
//			GladosPlayVcd(15)
//			break
//		case "mp_coop_wall_block": 
//			GladosPlayVcd(15)
//			break
//		case "mp_coop_catapult_2": 
////			if (OrangeTalk>500 && BlueTalk==0)
////				GladosPlayVcd(15)
////			break
////			}
////			else{
////				if (BlueTalk>500 && OrangeTalk==0)
////					GladosPlayVcd(15)
////					break
////				}
////
////			}
//				GladosPlayVcd(15)
//				break
//				//didn't hook up sound stuff because quiet person may still  have noisy avatar - need more thinking on this.
//
//
//		case "mp_coop_turret_walls": 
//			GladosPlayVcd(15)
//			break			
//		case "mp_coop_turret_ball": 
//			GladosPlayVcd(15)
//			break		
//		case "mp_coop_wall_5": 
//			GladosPlayVcd(15)
//			break
/////////Subterfuge Buidling Course
		case "mp_coop_tbeam_redirect":
			if (IsLocalSplitScreen()){
				GladosPlayVcd(1305)
			}
			else{
				EntFire( "@relay_disable_exit", "Trigger", "", 0.0 )
				GladosPrivateTalk(1,1)		
				EntFire( "@relay_enable_exit", "Trigger", "", 13.0 )			
				break		
			}
//		case "mp_coop_tbeam_drill":
//			GladosPrivateTalk(2,4)		
//			EntFire( "@relay_enable_exit", "Trigger", "", 15.0 )			
//			break		
//		case "mp_coop_tbeam_catch_grind_1":
//			GladosPrivateTalk(1,2)		
//			EntFire( "@relay_enable_exit", "Trigger", "", 14.0 )			
//			break		
//		case "mp_coop_tbeam_laser_1":
//			GladosPrivateTalk(2,3)		
//			EntFire( "@relay_enable_exit", "Trigger", "", 12.0 )			
//			break		
//		case "mp_coop_tbeam_polarity":
//			if (bluetrust==1)
//			{
//				GladosPlayVcd(1071)	
//			}
//			else
//			{
//				GladosPlayVcd(1072)	
//			}
//			break		
//		case "mp_coop_tbeam_polarity2":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_tbeam_polarity3":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_tbeam_maze":
//			GladosPrivateTalk(2,5)		
//			EntFire( "@relay_enable_exit", "Trigger", "", 10.0 )			
//			break		
//		case "mp_coop_tbeam_end":
//			GladosPlayVcd(15)		
//			break
//
///////////XXXXXX  Building Course
//		case "mp_coop_paint_come_along":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_redirect":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_bridge":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_walljumps":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_speed_fling":
//			GladosPlayVcd(15)
//			break		
//		case "mp_coop_paint_red_racer":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_speed_catch":
//			GladosPlayVcd(15)		
//			break		
//		case "mp_coop_paint_longjump_intro":
//			GladosPlayVcd(15)		
//			break
	}
}




function CoopPingTool(player,surface) 
{
	local curTime = Time()
	local mapname = GetMapName()
	printldebug(surface)
	if (player == coopBlue){		
		if (BlueInPortalTraining == 1 && BlueHasGun == 0 && mapname == "mp_coop_start"){
			local BluePingInterval = curTime-BlueLastPing
			local BluePingInterval2 = curTime-BluePortalTrainingCounter
			BlueLastPing = curTime
			if (surface==1 && BluePortalTrainingCounter==0 && BluePingTraining1==0){
				BluePortalTrainingCounter=Time()
				BluePingInterval2=0
				BluePingTraining1=1
				GladosPlayVcd(1000)

			}
			if (surface==1 && BluePingInterval2>7 && BluePortalTrainingCounter!=0  && BluePingTraining2==0){
				BluePortalTrainingCounter=0
				BluePingTraining2=1
				GladosPlayVcd(1001)
			}		
		}
	}

	if (player == coopOrange){		
		if (OrangeInPortalTraining == 1 && OrangeHasGun == 0 && mapname == "mp_coop_start"){
			local OrangePingInterval = curTime-OrangeLastPing
			local OrangePingInterval2 = curTime-OrangePortalTrainingCounter
			OrangeLastPing = curTime
			if (surface==1 && OrangePortalTrainingCounter==0 && OrangePingTraining1==0){
				OrangePortalTrainingCounter=Time()
				OrangePingInterval2=0
				OrangePingTraining1=1
				GladosPlayVcd(1014)

			}
			if (surface==1 && OrangePingInterval2>7 && OrangePortalTrainingCounter!=0  && OrangePingTraining2==0){
				OrangePortalTrainingCounter=0
				OrangePingTraining2=1
				GladosPlayVcd(1015)
			}		
		}
	}
}

/////// Subterfuge section

function CoopPolarityWhisper(player){
	if (player == coopBlue){		
				GladosPlayVcd(1066)
				polarity2whisper=2
	}
	else{
				GladosPlayVcd(1067)
				polarity2whisper=1
	}
}



function CoopReturnHubTrack01(){
	local tVcd
	tVcd=RandomInt(1263,1266)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)
}

function CoopReturnHubTrack02(){
	local tVcd
	tVcd=RandomInt(1267,1270)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)

}

function CoopReturnHubTrack03(){
	local tVcd
	tVcd=RandomInt(1271,1273)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)
}

function CoopReturnHubTrack04(){
	local tVcd
	tVcd=RandomInt(1274,1276)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)
}

function CoopReturnHubTrack05(){
	local tVcd
	tVcd=RandomInt(1277,1279)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)
}

function CoopReturnHubAllFinished(){
	local tVcd
	tVcd=RandomInt(1280,1286)
	printldebug("THIS IS RANDOM:"+ tVcd)
	GladosPlayVcd(tVcd)

}

function PlayerTauntCamera (player,animation){
	printldebug("===============>CAMERA GESTURE!"+player+" x "+animation)
 	//added to block playing of responses while at vault door.
 	if (curMapName == "mp_coop_paint_longjump_intro"){
 		return
 	}
 	
 	local curTime=Time()
	if (curTime-LastTauntTime<5){
		return
	}
	LastTauntTime=Time()
	local tauntflag0 = GetGladosSpokenFlags( 0 )  //30
	local tauntflag1 = GetGladosSpokenFlags( 1 )  //18
	local tauntflag2 = GetGladosSpokenFlags( 2 )
	local tauntflag3 = GetGladosSpokenFlags( 3 ) //4 used for deaths
	
	if (((tauntflag1 & (1 << 18)) == 0) && (curMapName == "mp_coop_race_2" || curMapName == "mp_coop_laser_2" || curMapName == "mp_coop_fling_3")){
		local TellStory = 0
		local Player2
		while ( Player2 = Entities.FindByName ( Player2, "blue" )  )
		{
    	print ( "Canceling:\t" + Player2.GetName() + "\n" )
    	if (Player2 !=null){
    		break
    	}
		}
		if ( Player2!=null){
			local vecPlayerPos=Player2.GetOrigin()
			local Door
			while ( Door = (Entities.FindByNameNearest( "@exit_door", vecPlayerPos, 600 )) )
			{
    		if	(Door !=null){
    			TellStory = 1
        	break
    		}
			}
		}
		if (TellStory==0){
			tauntflag1 = tauntflag1 + (1 << 18)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1330)
			return
		}  
	}      
	
	if ( animation == "smallWave" ){
		if ((tauntflag0 & (1 << 0)) == 0){
			tauntflag0 = tauntflag0 + (1 << 0)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1227)
			return
		}
		if ((tauntflag0 & (1 << 1)) == 0){
			tauntflag0 = tauntflag0 + (1 << 1)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1228)
			return
		}
		if ((tauntflag0 & (1 << 2)) == 0){
			tauntflag0 = tauntflag0 + (1 << 2)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1229)
			return
		}
		if ((tauntflag0 & (1 << 26)) == 0){
			tauntflag0 = tauntflag0 + (1 << 26)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1234)
			return
		}
		if ((tauntflag0 & (1 << 28)) == 0){
			tauntflag0 = tauntflag0 + (1 << 28)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1236)
			return
		}		
		if ((tauntflag0 & (1 << 29)) == 0){
			tauntflag0 = tauntflag0 + (1 << 29)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1237)
			return
		}		
		if ((tauntflag0 & (1 << 30)) == 0){
			tauntflag0 = tauntflag0 + (1 << 30)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1238)
			return
		}				
	}


	if ( animation == "highFive" ){
		if ((tauntflag0 & (1 << 3)) == 0){
			tauntflag0 = tauntflag0 + (1 << 3)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1207)
			return
		}
		if ((tauntflag0 & (1 << 4)) == 0){
			tauntflag0 = tauntflag0 + (1 << 4)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1208)
			return
		}
		if ((tauntflag0 & (1 << 5)) == 0){
			tauntflag0 = tauntflag0 + (1 << 5)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1209)
			return
		}
		if ((tauntflag1 & (1 << 0)) == 0){
			tauntflag1 = tauntflag1 + (1 << 0)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1248)
			return
		}
		if ((tauntflag1 & (1 << 1)) == 0){
			tauntflag1 = tauntflag1 + (1 << 1)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1249)
			return
		}
		if ((tauntflag1 & (1 << 2)) == 0){
			tauntflag1 = tauntflag1 + (1 << 2)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1251)
			return
		}
		if ((tauntflag1 & (1 << 9)) == 0){
			tauntflag1 = tauntflag1 + (1 << 9)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1289)
			return
		}		

	}
	
	if ( animation == "teamhug" ){
		if ((tauntflag0 & (1 << 6)) == 0){
			tauntflag0 = tauntflag0 + (1 << 6)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1210)
			return
		}
		if ((tauntflag0 & (1 << 7)) == 0){
			tauntflag0 = tauntflag0 + (1 << 7)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1211)
			return
		}
		if ((tauntflag0 & (1 << 8)) == 0){
			tauntflag0 = tauntflag0 + (1 << 8)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1212)
			return
		}
		if ((tauntflag1 & (1 << 3)) == 0){
			tauntflag1 = tauntflag1 + (1 << 3)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1221)
			return
		}		
		if ((tauntflag1 & (1 << 4)) == 0){
			tauntflag1 = tauntflag1 + (1 << 4)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1231)
			return
		}		
		if ((tauntflag1 & (1 << 10)) == 0){
			tauntflag1 = tauntflag1 + (1 << 10)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1290)
			return
		}		

	}

	if ( animation == "TeamEggTease" || animation == "TeamBallTease" ){
		if ((tauntflag0 & (1 << 9)) == 0){
			tauntflag0 = tauntflag0 + (1 << 9)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1213)
			return
		}
		if ((tauntflag0 & (1 << 10)) == 0){
			tauntflag0 = tauntflag0 + (1 << 10)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1214)
			return
		}
		if ((tauntflag0 & (1 << 11)) == 0){
			tauntflag0 = tauntflag0 + (1 << 11)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1215)
			return
		}
		if ((tauntflag0 & (1 << 12)) == 0){
			tauntflag0 = tauntflag0 + (1 << 12)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1216)
			return
		}
		if ((tauntflag1 & (1 << 8)) == 0){
			tauntflag1 = tauntflag1 + (1 << 8)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1288)
			return
		}		

	}


	if ( animation == "laugh" ){
		if ((tauntflag0 & (1 << 13)) == 0){
			tauntflag0 = tauntflag0 + (1 << 13)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1239)
			return
		}
		if ((tauntflag0 & (1 << 14)) == 0){
			tauntflag0 = tauntflag0 + (1 << 14)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1254)
			return
		}

		if ((tauntflag0 & (1 << 15)) == 0){
			tauntflag0 = tauntflag0 + (1 << 15)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1240)
			return
		}
		if ((tauntflag0 & (1 << 16)) == 0){
			tauntflag0 = tauntflag0 + (1 << 16)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1241)
			return
		}
		if ((tauntflag0 & (1 << 17)) == 0){
			tauntflag0 = tauntflag0 + (1 << 17)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1253)
			return
		}
		if ((tauntflag1 & (1 << 11)) == 0){
			tauntflag1 = tauntflag1 + (1 << 11)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1291)
			return
		}		

	}

	if ( animation == "robotDance" ){
		if ((tauntflag0 & (1 << 18)) == 0){
			tauntflag0 = tauntflag0 + (1 << 18)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1242)
			return
		}
		if ((tauntflag0 & (1 << 19)) == 0){
			tauntflag0 = tauntflag0 + (1 << 19)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1243)
			return
		}
		if ((tauntflag1 & (1 << 12)) == 0){
			tauntflag1 = tauntflag1 + (1 << 12)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1292)
			return
		}		

		if ((tauntflag0 & (1 << 20)) == 0){
			tauntflag0 = tauntflag0 + (1 << 20)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1244)
			return
		}
		if ((tauntflag0 & (1 << 21)) == 0){
			tauntflag0 = tauntflag0 + (1 << 21)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1245)
			return
		}
		if ((tauntflag0 & (1 << 22)) == 0){
			tauntflag0 = tauntflag0 + (1 << 22)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1246)
			return
		}
		if ((tauntflag0 & (1 << 23)) == 0){
			tauntflag0 = tauntflag0 + (1 << 23)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1247)
			return
		}
	}

	if ( animation == "rps" ){
		if ((tauntflag0 & (1 << 24)) == 0){
			tauntflag0 = tauntflag0 + (1 << 24)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1217)
			return
		}
		if ((tauntflag0 & (1 << 25)) == 0){
			tauntflag0 = tauntflag0 + (1 << 25)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1218)
			return
		}
		if ((tauntflag0 & (1 << 27)) == 0){
			tauntflag0 = tauntflag0 + (1 << 27)
			AddGladosSpokenFlags( 0, tauntflag0 )
			GladosPlayVcd(1235)
			return
		}		
		if ((tauntflag1 & (1 << 5)) == 0){
			tauntflag1 = tauntflag1 + (1 << 5)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1232)
			return
		}				
		if ((tauntflag1 & (1 << 6)) == 0){
			tauntflag1 = tauntflag1 + (1 << 6)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1233)
			return
		}				
		if ((tauntflag1 & (1 << 13)) == 0){
			tauntflag1 = tauntflag1 + (1 << 13)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1287)
			return
		}				
	}
	if ( animation == "trickfire" ){
		if ((tauntflag1 & (1 << 14)) == 0){
			tauntflag1 = tauntflag1 + (1 << 14)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1252)
			return
		}		

		if ((tauntflag1 & (1 << 15)) == 0){
			tauntflag1 = tauntflag1 + (1 << 15)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1327)
			return
		}		
		if ((tauntflag1 & (1 << 16)) == 0){
			tauntflag1 = tauntflag1 + (1 << 16)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1328)
			return
		}				
		if ((tauntflag1 & (1 << 17)) == 0){
			tauntflag1 = tauntflag1 + (1 << 17)
			AddGladosSpokenFlags( 1, tauntflag1 )
			GladosPlayVcd(1329)
			return
		}				
  }
        
        
        
}

function CoopBotAnimation(player,animation) 
{

	testcnt=testcnt+1
	printldebug("===============>TEST TEST :"+testcnt)
	printldebug("===============>GESTURE!"+player+" x "+animation)

	if (GladosInsideTauntCam == 1 ){
		if (player==1){
			OrangeTauntCam=1	
			OrangeTauntFinaleInterval = Time()
		}
		if (player==2){
			BlueTauntCam=1	
			BlueTauntFinaleInterval = Time()

		}
		if (BlueTauntCam==1 && OrangeTauntCam==1){

			GladosInsideTauntCam = 0 
			printldebug("===============>CAM TEST :"+GladosInsideTauntCam)
			printldebug("===============>CAM TEST :"+GladosInsideTauntCam)
			printldebug("===============>CAM TEST :"+GladosInsideTauntCam)
			printldebug("===============>CAM TEST :"+GladosInsideTauntCam)
			printldebug("===============>CAM TEST :"+GladosInsideTauntCam)
			GladosPlayVcd(1161)

		}
	}
       
	if (curMapName == "mp_coop_teambts"){
 		local curTime=Time()
		if (curTime-LastTauntTime<5){
			return
		}
		LastTauntTime=Time()	
		teambtstaunts=teambtstaunts+1
		switch (teambtstaunts)
		{
			case 1: 
				if (player == coopBlue){		
					GladosPlayVcd(1100)
				}
				else{
					GladosPlayVcd(1101)
				}
				break
			
			case 2: 	
				GladosPlayVcd(1102)		
				break
			
			case 3: 
				if (player == coopBlue){		
					GladosPlayVcd(1103)
				}
				else{
					GladosPlayVcd(1104)
				}
				break				
				
			case 4: 	
				GladosPlayVcd(1105)		
				break
				
			case 5: 	
				GladosPlayVcd(1106)		
				break
		}
	}
}

///bit for killing stuck players in tbeam
function GladosCoopKillTbeamMaze(){
		GladosPlayVcd(1304)	
}

function CoopCubeFizzle(){
	local curMapName = GetMapName()
	if (curMapName=="mp_coop_multifling_1"){
		switch (mp_coop_multifling_1cube)
		{
			case 0: 
				mp_coop_multifling_1cube = 1
				GladosPlayVcd(1310)		
				break
			case 1: 
				mp_coop_multifling_1cube = 2
				GladosPlayVcd(1311)		
				break
			case 2: 
				mp_coop_multifling_1cube = 3
				GladosPlayVcd(1312)		
				break
			case 3: 
				mp_coop_multifling_1cube = 4
				GladosPlayVcd(1313)		
				break
		}
	}
	if (curMapName=="mp_coop_paint_red_racer") {
		summer_sale_cube_died = true;
	}
}

// Announcer Stuf
function CoopHubAllFinished()
{
	local x = RandomInt(1,100)
	if (x>66)
	{
		GladosPlayVcd(1108)	
	}
	else
	{
		if (x>33)
		{
			GladosPlayVcd(1109)	
		}
		else{
			GladosPlayVcd(1110)	
		}
	}
}
