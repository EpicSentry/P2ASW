/*
Properties:
	The following properties are evaluated for EVERY vcd in  a scene:
		[required] vcd 
				This is the actual scene entity. Must take the form
				 	vcd = CreateSceneEntity("my_fully_pathed_scene.vcd")
		[required] next 
				SceneTable index of next vcd in scene or null for last vcd in scene.
		[required] char
				Character who is speaking in this vcd. Valid characters:
					glados
					wheatley
					announcer
					turret
					cave
		[optional] postdelay
					Number of seconds to wait after finishing this vcd before firing the next VCD in the scene.
					If postdelay < 0 the next vcd starts abs(postdelay) seconds after this vcd STARTS. This lets one character talk over another character.
					Postdelay can take the format postdelay=[x,y], in which case postdelay evaluates to RandomFloat(x,y)
		[optional] predelay
					Number of seconds to wait before starting this vcd.
					Predelay can take the format predelay=[x,y], in which case predelay evaluates to RandomFloat(x,y)
		[optional] talkover
					If talkover exists, this vcd will not silence and currently playing vcds. This should always be set for postdelay<0.

		[optional] fires
					Each vcd in a scene can trigger an arbitrary number of ent_fires. The fires array defines these ent_fires for each vcd.
					By default, these fire at the end of the associated vcd. If fireatstart exists, however, the entry will fire at the START
					of the vcd, and delay defines the number of seconds to wait from the START of the vcd.
					ex:
						fires=
						[
							{entity="entity1",input="Trigger",parameter="",delay=2,fireatstart=true},
							{entity="entity2",input="RunScriptCode",parameter="DoMyThing(1)",delay=1.6}
						]	
						
						2 seconds after the START of the vcd, entity1 is sent "Trigger".
						1.6 seconds after the END of the vcd, entity2 is sent "RunScriptCode" with a parameter of DoMyThing(1)


	The following properties are evaluated only for the FIRST vcd in a scene:

		[optional] noDingOn
				By default, a "ding" noise is played at the beginning and end of a scene (this is the default because glados.nut was originally
				designed just for glados vo. If noDingOn exists, the starting ding is suppressed.
		[optional] noDingOff
				By default, a "ding" noise is played at the beginning and end of a scene (this is the default because glados.nut was originally
				designed just for glados vo. If noDingOff exists, the ending ding is suppressed.
		[optional] queue
				If queue exists, this scene will queue up if a scene is currently playing.
		[optional] queuetimeout
				A scene will be removed from the queue if it waits in the queue > queuetimeout seconds. This is only evaluated if queue exists.
		[optional] queueforcesecs
				If a scene has waited in the queue > queueforcesecs, it will stop whatever scene is playing and start. This is only evaluated if queue exists.
		[optional] skipifbusy
				The scene will only play if no other scene is playing.
				
	Nag/Idle tags:
		-Every vcd in a nag/idle block should have "next" set to null (unless they are part of a "nag chain" - see idleindex/idleunder below)
		-You can stop a nag cycle by calling @glados.GladosStopNag() or @glados.WheatleyStopNag()
		
		The *FIRST* vcd in a nag block should have:
			idle=true
			idlerandom=true 					[optional] if you want the vcds to play in random order, otherwise they play in order by "idleorderingroup"
			idlerepeat=true 					[optional] if you want the list to repeat after exhausting the list once
			idlerandomonrepeat=true 	[optional] if you want the list to play in order the first time and then randomize on repeats, mutually exclusive with idlerandom & idlerepeat
			idleminsecs=0.300 				minimum # of seconds between nags
			idlemaxsecs=1.500					maximum # of seconds between nags		
			
		*Every* vcd should have:
			idlegroup 				- a string that is the same for all vcds in the nag cycle block		
			idleorderingroup	- an integer ordering this vcd in the block		
			idlemaxplays			- [optional] once a vcd plays this many times, retire it. [default is never retire]
			idlerarity				- [optional] Every time the nag list is rebuilt (happens the first time the list is built, plus every time the list is exhausted) this vcd has idlerarity% chance to be included
			idleindex					- [optional] a unique id within the nag block used to group a set of vcds that should be played together as one "nag". This should be set only on the first vcd of the group
			idleunder					- [optional] points to idleindex for members of a nag group other that the first one
			
		//Example nag/idle cycle

		//TestIdle
		//Oops.
			//SceneTable["sp_intro_03TestIdle01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer03.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idle=true,idlerandom=true,idlerepeat=true,idleminsecs=0.300,idlemaxsecs=1.500,idlegroup="sp_intro_03testidle",idleorderingroup=1}
		//That's funny, I don't feel corrupt. In fact, I feel pretty good.
			//SceneTable["sp_intro_03TestIdle02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer04.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=2}
		//Core transfer?
			//SceneTable["sp_intro_03TestIdle03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer05.vcd"),postdelay=0.1,next="sp_intro_03TestIdle0301",char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=3,idleindex=1}
		//NAG GROUP: Oh, you are kidding me.
			//SceneTable["sp_intro_03TestIdle0301"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer06.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=4,idleunder=1}
		//No!
			//SceneTable["sp_intro_03TestIdle05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer07.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=5}
		//Yes! You little worm!
			//SceneTable["sp_intro_03TestIdle06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer10.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=6}
		//Don't do it.
			//SceneTable["sp_intro_03TestIdle07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer12.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=7}
		//Not so fast!
			//SceneTable["sp_intro_03TestIdle08"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer14.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idlegroup="sp_intro_03testidle",idleorderingroup=8}
*/



//SceneTableLookup

//MAKE SURE THE INDEXES ON THIS ARRAY ARE NEGATIVE!!!!!
	SceneTableLookup[-1] <- "-1_01" //Test Scene
									//Wheatly Elevator Scene - Pre Crash
									//Wheatly Elevator Scene - Post Crash
	


// ****************************************************************************************************
// GLADOS BATTLE scenetable
// ****************************************************************************************************

	SceneTableLookup[-50] <- "-20_01" 	// I honestly didn't think you would fall for that
										// In fact, I devised a much more elaborate trap ahead, for when you got through this easy one.
										// If I had known it would be this easy, I would have just dangled a turkey leg
	SceneTableLookup[-53] <- "-21_01"	// But anyway. Here we are.  Again.
										// I hope you brought something more powerful than the portal gun this time
										// It was nice catching up. Lets get to business.
										// You remember my turrets don't you... oh wait, that's you in 5 seconds. goodbye.
	
	SceneTableLookup[-700] <- "-700_01" // turret conversation here
	SceneTableLookup[-750] <- "-750_01" // wheatley neurotoxin ride nags here
	
	
	SceneTableLookup[-57] <- "-22_01"	// My turrets!
										// Oh, you were busy back there
										// //I suppose we could just sit in this room and glare at each other until somebody drops dead, but I have a better idea.
										// Your old friend, deadly neurotoxin.  If i were you i'd take a deep breath and hold it.
										
	SceneTableLookup[-61] <- "-23_01"	// Hello!
										// I hate you so much
										// Warning: Central core is 80 percent corrupted. 
										// That's funny, I don't feel corrupt. In fact, I feel pretty good.
										// Alternate core detected.
										// That's ME they're talking about!
										// To initiate a core transfer, please deposit substitute core in receptacle.
										// Core transfer?
										// Oh, you are kidding me.
										
	SceneTableLookup[-760] <- "-760_01"// Wheatley socket scene start
	SceneTableLookup[-761] <- "-760_01d"// Wheatley socket nag start

	SceneTableLookup[-71] <- "-24_01" 	//Substitute core accepted.
										//Substitute core, are you ready to start the procedure?
										//Yes.
										//Corrupted core, are you ready to start the procedure?
										//No!
										//Nonononononono!
										//Stalemate detected. Transfer procedure cannot continue.
										//Yes! You little worm!
										//...unless a stalemate associate is present to press the stalemate resolution button.
										//Leave me in! Leave me in! Go press it!
										//Don't do it.
										//Don't press that button. You don't know what you're doing.
	
	SceneTableLookup[-755] <- "-755_01"//Button press nags

	SceneTableLookup[-84] <- "-25_01"	//Not so fast!
										//Think about this.
										//You need to be a trained stalemate associate to press that button. You're unqualified. I know this sounds like an obvious ploy, but I'm really not joking.
										//Impersonating a stalemate associate. I just added that to the list. It's a list I made of all the things you've done. Well, it's a list that I AM making, because you're still doing things right now, even though I'm telling you to stop. Stop, by the way.

										
	SceneTableLookup[-88] <- "-26_01"	// Ahhhh!
										// Stalemate Resolved. Core Transfer Initiated.


	SceneTableLookup[-4] <- "-4_01" 	// Wheatley: Here I go!
										// Wheatley: Wait, what if this hurts?
										// Wheatley: Wait, what if it really hurts? Ohh, I didn't think of that
										// Wheatley: Oh no!  Oh no oh no --
										// Wheatley: Ahhhhh!
	
	SceneTableLookup[-89] <- "-27_00"  // GLaDOS: GET YOUR HANDS OFF ME! NO! NO! STOP!
	
	SceneTableLookup[-90] <- "-27_01"	// CHELL! STOP THIS! I AM YOUR MOTHER!
										// ALRIGHT, FINE! THAT'S NOT TRUE!
	SceneTableLookup[-91] <- "-27_03" // BUT IT COULD BEEEEEAAHHH!
	
	SceneTableLookup[-93] <- "-28_01"	// Glados GIBBERING
	
	SceneTableLookup[-666] <- "-666_01"	// Wheatley Elevator Nag
	
	
	SceneTableLookup[-9] <- "-5_01"	// Wheatley: Wow! Check ME out, Partner!
										// We did it! I can't believe we did it! I'm in control of the whole facility now!
										// Oh! Right, the escape elevator! I'll call it now.
										// Elevator --called. You get in. I'll tell it to take you to the surface.										
										
	SceneTableLookup[-13] <- "-14_01"	// GLaDOS: Don't do this.. Don't do this...
										// GLaDOS: Oh.  it's you.
										// Wheatley: I knew it was gonna be cool being in charge of everything, but... WOW, is this cool! This body is amazing!
										// Glados: No.
										// Wheatley: And check this out! I'm brilliant now! [spanish] I don't know what I just said, but I can find out.
										// Wheatley: Oh! The elevator. Sorry. [ elevator moves ]
										// Wheatley: Wow, look how small you are! I can barely see you down there! But I'm HUGE. [evil laugh] Where did THAT come from? 
										// Glados: So here's a couple of facts. Absolute power corrupts. Absolutely.
										// Wheatley: Actually... hold on. 
										// Wheatley: Why do we have to leave right this minute? [elevator descends]
										// Glados: And if you combine absolute power with an absolute moron, inside the world's most powerful supercomputer... 
										// Wheatley: You be quiet! I was smart enough to take control of the whole building away from you, wasn't I? 
										// Glados: You didn't do anything. SHE did all the work.
										// Wheatley: Oh really? Well, maybe it's time I DID something, then. 
										// Glados: What are you doing?
										// Glados: No!
	
	
// - potatos
									 
	SceneTableLookup[-33] <- "-10_01"	// I'm on to you too, lady.  .. who's the boss?  Little old Wheatley!
	
	
	SceneTableLookup[-34] <- "-11_01" 	// Ah...!
										// THAT is a potato battery.  Now you live in it.
										// Just... kill me.
	
	SceneTableLookup[-37] <- "-12_01"	// There! See? I'm huge, you're trapped and she's a potato!  who's a moron now?
										// Still you.
										// How about now? now who's a moron?
										// Still you.  You're still a moron.
										// NOW WHO'S A MORON?
										// If you weren't such a moron, you'd know who.
										// I am not a moron! 
	
	SceneTableLookup[-44] <- "-13_01" 	// How about if I PUNCHED.YOU.INTO.THIS.PIT.  Huh?  Who's a moron now?
										// Uh oh.																														

// ****************************************************************************************************
// SCIENCE FAIR	scenetable								  
// ****************************************************************************************************

SceneTableLookup[-100] <- "-100_01" // Alright we took out her turrets. Now lets go take out her neurotoxin as well.

SceneTableLookup[-101] <- "-101_01" // Bring your daughter to work day.  That did not end well.
									//  And 40 potato batteries.  Embarassing.

SceneTableLookup[-102] <- "-102_01" // Pretty sure we're going the right way. Just to reassure you.

SceneTableLookup[-103] <- "-103_01" // Don't worry i'm absolutely guaranteeing you it is this way.  Ah, It's not this way.

SceneTableLookup[-104] <- "-104_01" // Definitely sure it's this way.  Not entirely sure.  I don't know where it is.


// ****************************************************************************************************
// SP_A2_PIT_FLINGS	companion cube fizzle scenetable					  
// ****************************************************************************************************

SceneTableLookup[-200] <- "-200_01" // Oh. Did I accidentally fizzle that before you could complete the test? I'm sorry.

SceneTableLookup[-201] <- "-201_01"// Go ahead and grab another one so that it won't also fizzle and you won't look stupid again.

SceneTableLookup[-202] <- "-202_01"// Go ahead. I PROMISE not to fizzle it this time.

SceneTableLookup[-203] <- "-203_01"// Oh. No. I fizzled that one too.

SceneTableLookup[-204] <- "-204_01"// Oh well. We have warehouses FULL of the things. Absolutely worthless. I'm happy to get rid of them.

SceneTableLookup[-205] <- "-205_01"// Go ahead. This time I promise you'll look incrementally less stupid than the previous two times in which you looked incredibly stupid.

SceneTableLookup[-206] <- "-206_01"// I think that one was about to say "I love you." They ARE sentient, of course. We just have a LOT of them.

SceneTableLookup[-207] <- "-207_01"// ** player got stuck dialog here


// ****************************************************************************************************
// SP_A2_TRUST_FLING scenetable					  
// ****************************************************************************************************

SceneTableLookup[-300] <- "-300_01"// Oh, sorry. Some of these test chambers haven't been cleaned in ages.

SceneTableLookup[-301] <- "-301_01"// So sometimes there's still trash in them. Standing around. Smelling, and being useless.

SceneTableLookup[-302] <- "-302_01"// Try to avoid the garbage hurtling towards you.

SceneTableLookup[-303] <- "-303_01"// Don't TEST with the garbage. It's garbage.

SceneTableLookup[-304] <- "-304_01"// Press the button again.

// ****************************************************************************************************
// SP_A2_COLUMN_BLOCKER scenetable			  
// ****************************************************************************************************

SceneTableLookup[-400] <- "-400_01"// Initiating surprise in 3... 2... 1.
									// I made it all up
									// Surprise
									// Your parents are probably dead... Wouldn't want to see you.
								

// ****************************************************************************************************
// SP_A2_BRIDGE_THE_GAP scenetable			  
// ****************************************************************************************************

SceneTableLookup[-500] <- "-500_00" //Perfect, the door's malfunctioning. I guess somebody's going to have to repair that too. [beat] No, don't get up. I'll be right back. Don't touch anything.
SceneTableLookup[-501] <- "-500_01" //Hey! Hey! Up here!
									//I found some bird eggs up here. Just dropped 'em into the door mechanism.  Shut it right down. I--AGH!					
									// BIRD BIRD BIRD BIRD
									//[out of breath] Okay. That's probably the bird, isn't it? That laid the eggs! Livid!
									//Anyway, look, the point is we're gonna break out of here, alright? But we can't do it yet. Look for me fifteen chambers ahead.
									//Here she comes! Just play along! RememberFifteenChambers!
								
								
// ****************************************************************************************************
// SP_A2_FIZZLER_INTRO scenetable					  
// ****************************************************************************************************


SceneTableLookup[-950] <- "-50_01"//Nevermind. I have to go... check something. Test on your own recognizance. I'll be back.


// ****************************************************************************************************
// SP_A2_BTS3 (darkness) scenetable
// ****************************************************************************************************
SceneTableLookup[-2500] <- "-2500_01" // OOh.  Dark down here, isn't it. (ghost story)
SceneTableLookup[-2503] <- "-2503_01" // (smelly humans)
SceneTableLookup[-2504] <- "-2504_01" // humans, love 'em!

// ****************************************************************************************************
// SP_A1_intro7 (hobo turret) scenetable
// ****************************************************************************************************
SceneTableLookup[-2550] <- "-2550_01" // hobo turret scene start.  (Hello?  Hello.....)
	

//SCENE: TestTwo
	if (curMapName=="sp_intro_03")
	{
		//Oh no, don't. Anyway, back to you two imbeciles killing me:
		SceneTable["-1_01"] <- 
		{
			vcd = CreateSceneEntity("scenes/npc/glados/gladosbattle_pre16.vcd")
			char = "glados"
			postdelay = [0.0,5.0]
			predelay = 0.0
			next = "-1_02"
			noDingOff = true
			noDingOn = true
			queue = true
			queuetimeout = 10
		}
		//Wait here. Don't go anywhere. I'll be back.
		SceneTable["-1_02"] <- 
		{	
			vcd = CreateSceneEntity("scenes/npc/glados/gladosbattle_pre18.vcd")
			char = "glados"
			next = null
			fires=
			[
				{entity="entity1",input="Trigger",parameter="",delay=2,fireatstart=true},
				{entity="entity2",input="RunScriptCode",parameter="DoMyThing(1)",delay=1.6}
			]		
		}
	}	


// ****************************************************************************************************
// Wheatley Darkness
// ****************************************************************************************************

if (curMapName=="sp_a1_intro7" || "hobo_test") 
{
	// ====================================== Hobo turret scene
	
	SceneTable ["-2550_01" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube01.vcd") // Hello?
		char = "turret"
		postdelay = 0.0
		predelay = 0.0
		next = "-2550_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	
	SceneTable ["-2550_03" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube04.vcd") // Hello.
		char = "turret"
		postdelay = 0.5
		predelay = 0.0
		next = "-2550_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
		
	SceneTable ["-2550_04" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube06.vcd") // Excuse me?
		char = "turret"
		postdelay = 0.7
		predelay = 0.0
		next = "-2550_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	SceneTable ["-2550_06" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube08.vcd") // Hello... 
		char = "turret"
		postdelay = 1.0
		predelay = 0.0
		next = "-2550_07"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	SceneTable ["-2550_07" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintubegoodbye01.vcd") // Thanks anyway.
		char = "turret"
		postdelay = 0.9
		predelay = 0.0
		next = "-2550_08"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	SceneTable ["-2550_08" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube02.vcd") // Hello...
		char = "turret"
		postdelay = 1.5
		predelay = 0.0
		next = "-2550_09"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	SceneTable ["-2550_09" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube01.vcd") // Hello.
		char = "turret"
		postdelay = 1.5
		predelay = 0.0
		next = "-2550_10"
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
	
	SceneTable ["-2550_10" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/turret/turretstuckintube09.vcd") // I'm different.
		char = "turret"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		settarget1="hobo_turret"
	}
}

function sp_a1_intro7_HoboTurretScene()
{
	GladosPlayVcd(-2550)
	GladosPlayVcd(578)
}

// ****************************************************************************************************
// Wheatley Darkness
// ****************************************************************************************************

if (curMapName=="sp_a2_bts3") 
{
// ====================================== Ghost Story scene
	
	SceneTable ["-2500_01" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour07.vcd") // Ooh.  Dark down here, isn't it.
		char = "wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-2500_02"
		noDingOff = true
		noDingOn = true
	}
	
	SceneTable ["-2500_02" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour57.vcd") // They say that the old caretaker of this place...
		char = "wheatley"
		postdelay = 0.0 
		predelay = 1.0
		next = null
		noDingOff = true
		noDingOn = true
	}	
	
// ====================================== Smelly Humans scene
	SceneTable ["-2503_01" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour58.vcd") // Here's an interesting story.. (smelly humans story)
		char = "wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-2503_02"
		noDingOff = true
		noDingOn = true
	}
		SceneTable ["-2503_02" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour59.vcd") // the um.. sorry, that's.. just tending to the humans.
		char = "wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-2503_03"
		noDingOff = true
		noDingOn = true
	}
	SceneTable ["-2503_03" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour60.vcd") // Sorry about that.  That just slipped out.
		char = "wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
	}
	
	SceneTable ["-2504_01" ] <-
	{
		vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour62.vcd") // Ahh. Humans!  Love 'em!
		char = "wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
	}
}

// ============================================================================
// Ghost story
// ============================================================================
function sp_a2_bts3_ghoststory()
{
	GladosPlayVcd(-2500)
}

// ============================================================================
// Smelly Humans
// ============================================================================
function sp_a2_bts3_smellyhumanstart()
{
	GladosPlayVcd(-2503)
}

// ============================================================================
// Ah, humans! love 'em!
// ============================================================================
function sp_a2_bts3_smellyhumanlovethem()
{
	GladosPlayVcd(-2504)
}
	

	
// ****************************************************************************************************
// GLaDOS Battle
// ****************************************************************************************************
		
if (curMapName=="sp_a2_core") // TODO:  CHANGE THIS TO CORRECT MAP NAME
{
// ====================================== Glados trap scene
		SceneTable ["-20_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbrvtrap02.vcd") // I honestly didn't think you would fall for that
			char = "glados"
			postdelay = 1.3 
			predelay = 1.0
			next = "-20_02"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-20_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbrvtrap03.vcd") // In fact, I devised a much more elaborate trap ahead, for when you got through this easy one.
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-20_03"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-20_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbrvtrap05.vcd") // If I had known it would be this easy, I would have just dangled a turkey leg
			char = "glados"
			postdelay = 2.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="rv_start_moving_trigger",input="enable",parameter="",delay=0.5}
			]
			
			
		}
		
// ======================================  Plays when player lands in moving vault
		SceneTable ["-21_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbturrets01.vcd") // Well, it was nice catching up. Lets get to business.
			char = "glados"
			postdelay = 0.8
			predelay = 1.0
			next = "-21_02"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-21_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgb_trap01.vcd") // I hope you brought something more powerful than the portal gun this time
			char = "glados"
			postdelay = 0.4
			predelay = 0.0
			next = "-21_03"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="glados_chamber_body",input="setanimation",parameter="gladosbattle_firstpart",delay=0,fireatstart=true}, // ANIM
				{entity="glados_chamber_body",input="setdefaultanimation",parameter="glados_idle_rightside",delay=0,fireatstart=true}
			]
		}
		
		SceneTable ["-21_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgb_trap02.vcd") // Otherwise you're about to become the past president of the being alive club, ha ha. 
			char = "glados"
			postdelay = 0.6
			predelay = 0.0
			next = "-21_03b"
			noDingOff = true
			noDingOn = true
			talkover=true
			fires=
			[
				{entity="deploy_turrets_relay",input="Trigger",parameter="",delay=4,fireatstart=true}	
			]
		}
	
		SceneTable ["-21_03b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgb_trap03.vcd") // Seriously, though--goodbye.
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover=true	
		}	
		
		
	/*	SceneTable ["-21_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbturrets02.vcd") // You remember my turrets don't you... oh wait, that's you in 5 seconds. goodbye.
			char = "glados"
			postdelay = -9.0
			predelay = 0.0
			next = "-700_01"
			noDingOff = true
			noDingOn = true
			talkover=true	
			fires=
			[
				{entity="deploy_turrets_relay",input="Trigger",parameter="",delay=4,fireatstart=true},		
			]
		}
	*/	
// ====================================== Turrets deployed scene

		SceneTable ["-700_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive18.vcd") // [muffled turret talking]
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_02"
			noDingOff = true
			noDingOn = true
			talkover=true			
			settarget1="turret_02-chamber_npc_turret"
		}
		
		SceneTable ["-700_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire05.vcd") // dry fire
			char = "turret"
			postdelay = -0.5
			predelay = 0.0
			next = "-700_03"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_02-chamber_npc_turret"
			fires=
			[
				{entity="box_turret_push_trigger",input="enable",parameter="",delay=0.0,fireatstart=true}	// turret falls over
			]
		}
		
		SceneTable ["-700_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive01.vcd") // it's my big chance!
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_04"
			noDingOff = true
			noDingOn = true
			talkover=true				
			settarget1="turret_01-chamber_npc_turret"
			fires=
			[
				{entity="turret_01-chamber_npc_turret_guns_out_ss",input="beginsequence",parameter="",delay=0, fireatstart=true}, // pop out guns
				{entity="turret_01-chamber_npc_turret",input="ignite",parameter="",delay=0},
			]
		}
		
		SceneTable ["-700_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire07.vcd") // dry fire
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_05"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_01-chamber_npc_turret"
		}
		
		SceneTable ["-700_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive15.vcd") //  This is trouble
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_06"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_01-chamber_npc_turret"
			fires=
			[
				{entity="turret_01-chamber_npc_turret",input="selfdestructimmediately",parameter="",delay=0.0},		
				{entity="turret_02-chamber_npc_turret",input="ignite",parameter="",delay=0.1}
			]
		}
		
		SceneTable ["-700_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive17.vcd") // [muffled turret talking while on fire]
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_07"
			noDingOff = true
			noDingOn = true
			talkover=true			
			settarget1="turret_02-chamber_npc_turret"
			fires=
			[
				{entity="turret_02-chamber_npc_turret",input="selfdestructimmediately",parameter="",delay=0.0}
			]
		}
		
		SceneTable ["-700_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive12.vcd") // ahhh not again!
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_08"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_03-chamber_npc_turret"
			fires=
			[
				{entity="turret_03-chamber_npc_turret",input="selfdestruct",parameter="",delay=0,fireatstart=true},
				{entity="turret_03-chamber_npc_turret",input="selfdestructimmediately",parameter="",delay=0.5}
			]
		}
		
		SceneTable ["-700_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive07.vcd") // Here it comes, pal!
			char = "turret"
			postdelay = 0.5
			predelay = 0.0
			next = "-700_09"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_04-chamber_npc_turret"
			fires=
			[
				{entity="turret_04-chamber_npc_turret_guns_out_ss",input="beginsequence",parameter="",delay=0, fireatstart=true}, // pop out guns
			]
		}
		
		
		SceneTable ["-700_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive08.vcd") // Locked and loaded!
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_10"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_05-chamber_npc_turret"
			fires=
			[
				{entity="turret_05 -chamber_npc_turret_guns_out_ss",input="beginsequence",parameter="",delay=0, fireatstart=true}, // pop out guns
			]
		}
		
		SceneTable ["-700_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire01.vcd") // dry fire
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = "-700_11"
			noDingOff = true
			noDingOn = true
			talkover=true
			settarget1="turret_04-chamber_npc_turret"
			fires=
			[
				{entity="turret_04-chamber_npc_turret",input="ignite",parameter="",delay=0,fireatstart=true},
				{entity="turret_04-chamber_npc_turret",input="selfdestructimmediately",parameter="",delay=1.0}
				{entity="turret_05-chamber_npc_turret",input="ignite",parameter="",delay=0},
			]
			
			// explodes, cracks glass
			// sets turret 5 on fire
		}
		
		
		SceneTable ["-700_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/glados_battle_defect_arrive16.vcd") // Oh this aint' good
			char = "turret"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true		
			settarget1="turret_05-chamber_npc_turret"
			fires=
			[
				{entity="turret_05-chamber_npc_turret",input="selfdestructimmediately",parameter="",delay=0.0},
				{entity="@glados",input="RunScriptCode",parameter="TurretDeathReactionDialog()",delay=0.3}
			]
		}
		
		/*
		
		// ====================================== Turret Death Reaction
		SceneTable["-22_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/fgbturrets04.vcd") // My turrets!
			char="glados"
			postdelay=0.1
			predelay = 0.0
			next="-22_02"
			noDingOff=true
			noDingOn=true
		}
		*/
		
		SceneTable["-22_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/fgb_trap05.vcd") // Ohhhhh. You were busy back there.
			char="glados"
			postdelay=0.8
			predelay = 0.0
			next="-22_03"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-22_03"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/fgb_trap06.vcd") //I suppose we could just sit in this room and glare at each other until somebody drops dead, but I have a better idea.
			char="glados"
			postdelay=0.1
			predelay = 0.0
			next="-22_05"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="deploy_neurotoxin_tube_relay",input="Trigger",parameter="",delay=5.00, fireatstart=true}
			]
		}
		
		/*(SceneTable["-22_04"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_pre07.vcd") // Your old friend, deadly neurotoxin.
			char="glados"
			postdelay=0.3
			predelay = 0.0
			next="-22_05"
			noDingOff=true
			noDingOn=true
		}*/
		
		SceneTable["-22_05"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/fgb_trap08.vcd") //Your old friend, deadly neurotoxin. If I were you, I'd take a deep breath right now and hold it.
			char="glados"
			postdelay=0.1
			predelay = 0.0
			next= null
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="deploy_wheatley_relay",input="Trigger",parameter="",delay=0.00},
				//{entity="glados_watch_wheatley_deploy_relay",input="Trigger",parameter="",delay=0.4} // makes glados spin to watch wheatley
				{entity="@glados",input="runscriptcode",parameter="StartWheatleyNeurotoxinRideNag()",delay=0.00}
			]
		}
		
		// ======================================
		// Wheatley neurotoxin ride
		// ======================================
		SceneTable["-750_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows05.vcd"),idlerandom=true, idlerepeat=true, postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idle=true, talkover=true, idleminsecs=0.0,idlemaxsecs=0.0,idlegroup="wheatleybouncepain",idleorderingroup=1}
		SceneTable["-750_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows06.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="wheatleybouncepain",idleorderingroup=2}
		SceneTable["-750_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows01.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=3}
		SceneTable["-750_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows02.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=4}
		SceneTable["-750_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows12.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=5}
		SceneTable["-750_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows15.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=6}
		SceneTable["-750_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows10.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=7}
		SceneTable["-750_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_wheatley_ows18.vcd"),postdelay=0.0,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="wheatleybouncepain",idleorderingroup=8}
		
		
		function StartWheatleyNeurotoxinRideNag()
		{
			printl("==Starting wheatley neurotoxin ride Nag")
			GladosPlayVcd( -750 )
		}
		
		function StopWheatleyNeurotoxinRideNag()
		{
			printl("==Stopping wheatley neurotoxin ride Nag")
			GladosStopNag()
			GladosCharacterStopScene("wheatley")  // this will make wheatley stop talking if he is currently playing a vcd
		}
		
		
		
		// ======================================  Wheatley Lands In Chamber		
		SceneTable["-23_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_hello01.vcd") // Hello!
			char="wheatley"
			postdelay= 1.1
			predelay = 0.0
			next="-23_02"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="shatter_vault_glass_relay",input="Trigger",parameter="",delay=0.4 },
			]
			
		}
		
		SceneTable["-23_02"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_pre09.vcd") //I hate you so much.
			char="glados"
			postdelay=0.0
			predelay = 0.8
			next= "-23_03"
			noDingOff=true
			noDingOn=true
		}
		
		//CoreDetected
		SceneTable["-23_03"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle02.vcd")	 // Warning: Central core is 80 percent corrupted.
			char="announcer"
			postdelay=0.0
			predelay = 0.0
			next= "-23_04"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-23_04"] <- 
		{			
		vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer04.vcd") 	// That's funny, I don't feel corrupt. In fact, I feel pretty good.
			char="glados"
			postdelay=0.0
			predelay = 0.0
			next= "-23_05"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body",input="setanimation",parameter="gladosbattle_xfer04",delay=0,fireatstart=true} // ANIM
				{entity="glados_chamber_body",input="setdefaultanimation",parameter="glados_idle_agitated_more",delay=0,fireatstart=true}
			]
		}
		
		SceneTable["-23_05"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle03.vcd")	 // Alternate core detected.
			char="announcer"
			postdelay=0.0
			predelay = 0.0
			next= "-23_06"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-23_06"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_that_is_me02.vcd") 	// That's ME they're talking about!
			char="wheatley"
			postdelay=0.0
			predelay = 0.0
			next= "-23_07"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-23_07"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle04.vcd") 	// To initiate a core transfer, please deposit substitute core in receptacle.
			char="announcer"
			postdelay=-5.0
			predelay = 0.0
			next= "-23_08"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="deploy_core_receptacle_relay",input="Trigger",parameter="",delay=0.3,fireatstart=true},
				{entity="display_socket_instructions_relay",input="Trigger",parameter="",delay=0.3,fireatstart=true}
			]
		}
		
		SceneTable["-23_08"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer05.vcd") // Core transfer?
			char="glados"
			postdelay=0.0
			predelay = 0.0
			next= "-23_09"
			noDingOff=true
			noDingOn=true
			talkover=true
		}
		
		SceneTable["-23_09"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer06.vcd") // Oh, you are kidding me.
			char="glados"
			postdelay=0.0
			predelay = 0.0 
			next= null
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StartWheatleyPluginScene()",delay=0.0}
			]
		}
		
		// ======================================
		// Wheatley socket plug in start
		// ======================================
		SceneTable["-760_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags01.vcd") // I've got an idea!  Do what it says, plug me in.
			char="wheatley"
			predelay = 0.0
			postdelay=0.1
			next="-760_01a"
			noDingOff=true
			noDingOn=true
			talkover=true
		}
		

		SceneTable["-760_01a"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/sp_a2_core02.vcd") // do not plug that little idiot into my mainframe
			postdelay=0.1
			predelay=0
			next="-760_01b"
			char="glados"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="glados_chamber_body",input="setanimation",parameter="sp_a2_core02",delay=0,fireatstart=true} // ANIM
				{entity="glados_chamber_body",input="setdefaultanimation",parameter="glados_idle_agitated_more",delay=0,fireatstart=true}
			]
		}
		
		// no you should plug that little idiot into the mainframe
		SceneTable["-760_01b"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_doit_nags08.vcd")
			postdelay=0.0
			predelay=0.0
			next="-760_01c"
			char="wheatley"
			noDingOff=true
			noDingOn=true
			talkover=true
		}
		
		// don't you dare plug him in
		SceneTable["-760_01c"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/sp_a2_core01.vcd")
			postdelay=0.0
			predelay=0.0
			next=null
			char="glados"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="glados_chamber_body",input="setanimation",parameter="sp_a2_core01",delay=0,fireatstart=true} // ANIM
				{entity="@glados",input="RunScriptCode",parameter="StartWheatleyPluginNag()",delay=0} // start the nag
			]
		}
		
		// ======================================
		// Wheatley socket plugin nag
		// ======================================
		
		SceneTable["-760_01d"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_doit_nags09.vcd"),idlerandomonrepeat=true, idlerepeat=true, idleminsecs=1.5, idlemaxsecs=3.0, postdelay=0.1,next=null,char="wheatley", noDingOff=true, noDingOn=true, idle=true, talkover=true, idlegroup="socketwheatleynag", idleorderingroup=1, idleunder=1}
		
		SceneTable["-760_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags06.vcd"),postdelay=0.1,next="-760_02a",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=2, idleindex=2}
		SceneTable["-760_02a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer13.vcd"),postdelay=0.1,next="-760_02b",char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=2, idleunder=2}
		SceneTable["-760_02b"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_doit_nags06.vcd"),postdelay=0.1,next="-760_02c",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=2, idleunder=2}
		SceneTable["-760_02c"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_doit_nags03.vcd"),postdelay=1.0,next=null,char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=2, idleunder=2}
		
		SceneTable["-760_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags08.vcd"),postdelay=0.1,next="-760_03a",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="socketwheatleynag",idleorderingroup=3, idleindex=3}
		SceneTable["-760_03a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_a2_core03.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=3, idleunder=3}
		
		SceneTable["-760_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags07.vcd"),postdelay=0.1,next="-760_04a",char="wheatley",noDingOff=true,noDingOn=true,talkover=true,idlegroup="socketwheatleynag",idleorderingroup=4, idleindex=4}
		SceneTable["-760_04a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_a2_core04.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=4, idleunder=4}
		
		SceneTable["-760_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags05.vcd"),postdelay=0.1,next="-760_05a",char="wheatley",noDingOff=true,noDingOn=true,talkover=true,idlegroup="socketwheatleynag",idleorderingroup=5, idleindex=5}
		SceneTable["-760_05a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer13.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=5, idleunder=5}
		
		SceneTable["-760_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags09.vcd"),postdelay=0.1,next="-760_06a",char="wheatley",noDingOff=true,noDingOn=true,talkover=true,idlegroup="socketwheatleynag",idleorderingroup=6, idleindex=6}
		SceneTable["-760_06a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_a2_core04.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=6, idleunder=6}
		
		SceneTable["-760_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_plugin_nags11.vcd"),postdelay=0.1,next="-760_07a",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="socketwheatleynag",idleorderingroup=7, idleindex=7}
		SceneTable["-760_07a"] <- {vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer12.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="socketwheatleynag",idleorderingroup=7, idleunder=7}
		
		function StartWheatleyPluginScene()
		{
			GladosPlayVcd( -760 )
		}
		
		function StartWheatleyPluginNag()
		{
			printl("==Starting wheatley plugin Nag")
			GladosPlayVcd( -761 )
		}
		
		function StopWheatleyPluginNag()
		{
			printl("==Stopping wheatley plugin Nag")			
			nuke() // stop every scene
		}
		
		
		
		// ======================================  Wheatley Plugged in
		SceneTable["-24_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle05.vcd") //Substitute core accepted. 
			char="announcer"
			postdelay=0.1
			predelay = 0.0
			next="-24_02"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="core_transfer_nag_relay",input="kill",parameter="",delay=0.0, fireatstart=true}
				{entity="glados_lookat_wheatley",input="trigger",parameter="",delay=0.0, fireatstart=true}
			]
		}
		
		SceneTable["-24_02"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle06.vcd") //Substitute core, are you ready to start the procedure?
			char="announcer"
			postdelay=0.0
			predelay = 0.0
			next= "-24_03"
			noDingOff=true
			noDingOn=true
			talkover=true
		}
		
		SceneTable["-24_03"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_ready_glados01.vcd")	// Yes!
			char="wheatley"
			postdelay=0.0
			predelay = 0.0
			next= "-24_04"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-24_04"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle07.vcd") //Corrupted core, are you ready to start the procedure?
			char="announcer"
			postdelay=0.0
			predelay = 0.0
			next= "-24_05"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="@glados",input="runscriptcode",parameter="sp_sabotage_glados_specials(1)",delay=0.00}
				{entity="glados_chamber_body",input="setanimation",parameter="gladosbattle_xfer07",delay=0,fireatstart=true} // ANIM
				{entity="glados_chamber_body",input="setdefaultanimation",parameter="glados_idle_agitated_more",delay=0,fireatstart=true}
			]
		}
		
		SceneTable["-24_05"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer07.vcd") //No!
			char="glados"
			postdelay=0.4
			predelay = 0.0
			next= "-24_06"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-24_06"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer08.vcd") 	// Nonononono!
			char="glados"
			postdelay=0.0
			predelay = 0.0
			next= "-24_07"
			noDingOff=true
			noDingOn=true
			talkover=true
		}
		
		SceneTable["-24_07"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle08.vcd")	//Stalemate detected. Transfer procedure cannot continue.
			char="announcer"
			postdelay=0.0
			predelay = 0.0
			next= "-24_08"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-24_08"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer10.vcd") //Yes! 
			char="glados"
			postdelay=0.0
			predelay = 0.0
			next= "-24_09"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="@glados",input="runscriptcode",parameter="sp_sabotage_glados_specials(2)",delay=0.3,fireatstart=true}
			]
		}
		
		SceneTable["-24_09"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle09.vcd") //...unless a stalemate associate is present to press the stalemate resolution button.
			char="announcer"
			postdelay=0.0
			predelay = 0.0 
			next= "-24_10"
			noDingOff=true
			noDingOn=true
			talkover=true
			fires=
			[
				{entity="open_stalemate_room_doors_relay",input="trigger",parameter="",delay=0.0},
				{entity="display_button_press_instructions_relay",input="Trigger",parameter="",delay=0.0}
			]
		}
		
		SceneTable["-24_10"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_leave_me_in01.vcd") //Leave me in! Leave me in! Go press it!
			char="wheatley"
			postdelay=0.0
			predelay = 0.4 
			next= "-24_11"
			noDingOff=true
			noDingOn=true
		}
		
		SceneTable["-24_11"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer13.vcd") //Don't do it.
			char="glados"
			postdelay=0.0
			predelay = 0.0 
			next= "-24_12"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="glados_lookat_player",input="trigger",parameter="",delay=0.0, fireatstart=true}
			]
		}
		
		SceneTable["-24_12"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_doit_nags01.vcd") //Yes, do do it!
			char="wheatley"
			postdelay=0.0
			predelay = 0.0 
			next= "-24_13"
			noDingOff=true
			noDingOn=true
		}
						
		SceneTable["-24_13"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer11.vcd") //Don't press that button. You don't know what you're doing.
			char="glados"
			postdelay=0.0
			predelay = 0.0 
			next= "-24_14"
			noDingOff=true
			noDingOn=true
		}

		SceneTable["-24_14"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags05.vcd") //I think she's lying.
			char="wheatley"
			postdelay=0.0
			predelay = 0.0 
			next= null
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StartStalemateButtonPressNag()",delay=0}
			]
		}
		
				
				
		// ======================================
		// Stalemate button press nags
		// ======================================
		SceneTable["-755_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags06.vcd"), predelay=[7,12], postdelay=0.1, next="-755_02",char="wheatley",noDingOff=true,noDingOn=true,idle=true, talkover=true, idleminsecs=7.0,idlemaxsecs=12.0,idlegroup="pressthebutton",idleorderingroup=1}
		
		SceneTable["-755_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags07.vcd"),postdelay=3.1,next="-755_03",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="pressthebutton",idleorderingroup=2}
		
		SceneTable["-755_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags08.vcd"),postdelay=3.1,next="-755_04",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="pressthebutton",idleorderingroup=3 }
		
		SceneTable["-755_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags09.vcd"),postdelay=3.1,next="-755_05",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="pressthebutton",idleorderingroup=4 }

		SceneTable["-755_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags10.vcd"),postdelay=3.1,next=null,char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="pressthebutton",idleorderingroup=5 }

		
// JAYP ADD IN BUTTON COUGHS		
		
		function StartStalemateButtonPressNag()
		{
			printl("==Starting stalemate button Nag")
			GladosPlayVcd( -755 )
		}
		
		function StopStalemateButtonPressNag()
		{
			printl("==Stopping stalemate button Nag")
			nuke() // stop every scene
		}
		
		// ======================================  //ButtonDenied
		SceneTable["-25_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer14.vcd") //Not so fast!
			postdelay=0.3
			next="-25_02"
			char="glados"
			noDingOff=true
			noDingOn=true
			talkover=false
			fires=
			[
				{entity="glados_chamber_body",input="setanimation",parameter="gladosbattle_xfer14",delay=0,fireatstart=true} //ANIM
			]
		}
		
		SceneTable["-25_02"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer15.vcd") //Think about this.
			postdelay=0.2
			next="-25_03"
			char="glados"
			noDingOff=true
			noDingOn=true
			talkover=false 
		} 
		
		SceneTable["-25_03"] <- 
		{
		vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer16.vcd") //You need to be a trained stalemate associate to press that button. You're unqualified. I know this sounds like an obvious ploy, but I'm really not joking.
		postdelay=0.2
		next="-25_04"
		char="glados"
		noDingOff=true
		noDingOn=true
		talkover=false
		}
				
		//Don't listen to her! It IS true that you don't have the qualifications. But you've got something more important than that. A finger, with which to press that button, so that she won't kill us.                                                             
		SceneTable["-25_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags06.vcd"),postdelay=0.0,next="-25_05",char="wheatley",noDingOff=true,noDingOn=true, talkover=false}
		
		//Impersonating a stalemate associate. I just added that to the list. It's a list I made of all the things you've done. Well, it's a list that I AM making, because you're still doing things right now, even though I'm telling you to stop. Stop, by the way.
		SceneTable["-25_05"] <-
		{
		vcd=CreateSceneEntity("scenes/npc/glados/gladosbattle_xfer17.vcd")
		postdelay=0.2
		next="-25_06"
		char="glados"
		noDingOff=true
		noDingOn=true
		talkover=false
		}
		
		//I... that's probably correct. But where it is incorrect is while I've been stalling you WE JUST PRESSED THE BUTTON! USE THE MOMENT OF CONFUSION I'VE JUST CREATED TO PRESS THE BUTTON!                                                                       
		SceneTable["-25_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags12.vcd"),postdelay=3.0,next="-25_07",char="wheatley",noDingOff=true,noDingOn=true, talkover=false}
			//Have I ever told you the qualities I love most in you? In order: Number one: resolving things, disputes highest among them. Number one, tied: Button-pushing.                                                                                                                                                                  
		SceneTable["-25_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags08.vcd"),postdelay=3.0,next="-25_08",char="wheatley",noDingOff=true,noDingOn=true, talkover=false}
			//First thing I thought about you...                                                                     
		SceneTable["-25_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags07.vcd"),postdelay=3.0,next="-25_08a",char="wheatley",noDingOff=true,noDingOn=true, talkover=false}
		//Don't want to rush you...                                                                    
		SceneTable["-25_08a"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags09.vcd"),postdelay=3.0,next="-25_09",char="wheatley",noDingOff=true,noDingOn=true, talkover=false}
		//Here's a good idea...     
		SceneTable["-25_09"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/sphere03/fgb_stalemate_nags04.vcd")
			postdelay=0.1
			next= null
			char="wheatley"
			noDingOff=true
			noDingOn=true
			talkover=false,
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StartStalemateButtonPressNag()",delay=0 }
			]
		}

		
		// ====================================== //Stalemate button pressed
		SceneTable["-26_01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_dropped01.vcd") // ahhh!
			char="glados"
			postdelay=0.5
			predelay = 0.0 
			next= "-26_02"
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StopStalemateButtonPressNag()",delay=0.0, fireatstart=true}
				{entity="glados_chamber_body",input="setanimation",parameter="Sp_sabotage_glados_dropped",delay=0,fireatstart=true} // ANIM
				{entity="glados_chamber_body",input="setdefaultanimation",parameter="glados_pit_eyegrab",delay=0,fireatstart=true}
			]
		}
		
		SceneTable["-26_02"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/gladosbattle10.vcd") // Stalemate Resolved. Core Transfer Initiated.
			char="announcer"
			postdelay=0.5
			predelay = 0.0 
			next= null
			noDingOff=true
			noDingOn=true
		}

		
		// ====================================== Wheatley transfer scene
		SceneTable ["-4_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/fgb_xfer_start03.vcd") // Here I go!
			char = "wheatley"
			postdelay = 0.6 
			predelay = 0.0
			next = "-4_02"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-4_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/fgb_xfer_start04.vcd") // Wait, what if this hurts? What if this really hurts?
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4_03"
			noDingOff = true
			noDingOn = true
		}
				
		SceneTable ["-4_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fgbwheatleytransfer03.vcd") // Oh it will.  Believe me, it will.
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = "-4_04"
			noDingOff = true
			noDingOn = true		
		}
		
		SceneTable ["-4_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/fgb_transfer_pain01.vcd") // Are you just saying that, or is it really going to hurt? You're just saying that, no, you're not aren't you, it's really going to hurt, isn't it? Exactly how painful is it AGHHHHHH!                                                                        
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4_05"
			noDingOff = true
			noDingOn = true
		}
		
		
			SceneTable ["-4_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sp_a2_core_goingtohurt01.vcd") // Exactly how much pain are we tAGGGHHH! 			
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover=true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="OpenMaintenancePitDoor()",delay=3.0, fireatstart=true},
				{entity="@glados",input="RunScriptCode",parameter="PitHandsGrabGladosHead()", delay=3.0, fireatstart=true}
				{entity="glados_pit_player_clip_relay",input="trigger",parameter="",delay=3.1, fireatstart=true} // pushes player off of maintenance pit
			]				
		}

	/*	
		SceneTable ["-4_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/fgb_xfer_start08.vcd")  // Oh no! Oh no! Nonono!
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
					
		}
	*/	
		
		// ====================================== Glados initial grab by small pit arms
		SceneTable["-27_00"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/fgbgladostransfer15.vcd") // Get your hands off me!
			char="glados"
			postdelay=0.1
			predelay = 0.0 
			next= null // stand alone scene, next is triggered by player look
			noDingOff=true
			noDingOn=true
			fires=
			[
				{entity="pitgrab_player_looking_at_glados_aisc",input="enable",parameter="",delay=0 } 
			]
		}
		
		// ====================================== Glados grabbed by big pit arms

		SceneTable ["-27_01" ] <-
		{
			// No no no!
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_core_drag_to_hell01.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="glados_head_removal_spark_timer",input="enable",parameter="",delay=1, fireatstart=true } 	// start the head cutting sparks
				{entity="glados_head_removal_spark_timer",input="disable",parameter="",delay=3, fireatstart=true } 	// start the head cutting sparks
				{entity="@glados",input="runscriptcode",parameter="CoreRoomWiltAnim()",delay=0.0 }		//show glados being disconnected by wilting the panels
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="EjectGladosHead()",delay=0.0 },		// eject glados head
				{entity="begin_wheatley_emergence_relay",input="trigger",parameter="",delay=2.0}, 						// wheatley appears
				{entity="@glados",input="RunScriptCode",parameter="CoreTransferCompleted()",delay=2.5 },				 // start wheatley dialog
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="RevealWheatley()",delay=2.0 },		// reveal wheatley

				//{entity="@glados",input="RunScriptCode",parameter="StartGladosGibberishNag()",delay=4.0 }				 // start gibberish nag	
//JAYP turned off gibberish until it gets ducked because it's SO LOUD AND ANNOYING!				
			]		
		}
		
		// ======================================
		// Glados gibberish nag
		// ======================================
		SceneTable["-28_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish01.vcd"),idlerandom=true, idlerepeat=true, postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,idle=true, talkover=true, idleminsecs=0.0,idlemaxsecs=0.0,idlegroup="gibberish",idleorderingroup=1}
		SceneTable["-28_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish03.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true, idlegroup="gibberish",idleorderingroup=2}
		SceneTable["-28_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish04.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="gibberish",idleorderingroup=3}
		SceneTable["-28_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish05.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="gibberish",idleorderingroup=4}
		SceneTable["-28_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish06.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="gibberish",idleorderingroup=5}
		SceneTable["-28_06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_gibberish07.vcd"),postdelay=0.1,next=null,char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="gibberish",idleorderingroup=6}
		
		
		function StartGladosGibberishNag()
		{
			printl("==Starting Glados Nag")
			GladosPlayVcd( -93 )
		}
		
		function StopGladosGibberishNag()
		{
			printl("==Stopping Glados Nag")
			GladosStopNag()
			GladosCharacterStopScene("glados")  // this will make glados stop talking if she is mid sentence
		}
		
		
		
		// ====================================== Wheatley swap completed scene
		
		SceneTable ["-5_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro01.vcd") // Wow, check me out!
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-5_01a"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_body",input="setanimation",parameter="glados_wheatley_body_intro01",delay=0,fireatstart=true} // ANIM
				{entity="@glados",input="runscriptcode",parameter="CoreRoomUnWiltAnim()",delay=0,fireatstart=true}
			]
		}

			SceneTable ["-5_01a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro04.vcd") // We did it! I can't believe we did it! I'm in control of the whole facility now!
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = "-5_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="CoreRoomHappyCounterClockwiseAnim()",delay=1,fireatstart=true} // ANIM
			]
		}
		
		SceneTable ["-5_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro03.vcd") // Ha ha ha! Look at me! Not too bad, eh? Giant robot. Massive!  
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-5_03"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro05.vcd") // Oh! Right, the escape lift! I'll call it now.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-5_04"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="deploy_exit_elevator_relay",input="Trigger",parameter="",delay=0.0,fireatstart=true},
			] 
		}
		
		SceneTable ["-5_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift01.vcd") // lift called, in you go
			char = "wheatley"
			postdelay = 1.0 
			predelay = 1.0
			next = "-5_05"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro12.vcd") 
			char = "wheatley"
			postdelay = 0.7 
			predelay = 0.0
			next = "-5_07"
			noDingOff = true
			noDingOn = true
		}
			
		SceneTable ["-5_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags01.vcd") 
			char = "wheatley"
			postdelay = 0.6 
			predelay = 0.0
			next = "-5_08"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags02.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-5_09"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags03.vcd") 
			char = "wheatley"
			postdelay = 1.8
			predelay = 0.0
			next = "-5_10"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags04.vcd") 
			char = "wheatley"
			postdelay = 0.3
			predelay = 0.0
			next = "-5_11"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-5_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags05.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StartWheatleyElevatorNag()",delay=3.0}
			]
		}
		

		// ======================================
		// Wheatley elevator nag
		// ======================================
		
		SceneTable["-666_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags06.vcd"), idleminsecs=5.0, idlemaxsecs=8.0, next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,talkover=true,idlegroup="escapeelevator",idleorderingroup=1}
		SceneTable["-666_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags07.vcd"), postdelay=5.0,next="-666_03",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="escapeelevator",idleorderingroup=2}
		SceneTable["-666_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags08.vcd"), postdelay=5.0,next="-666_04",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="escapeelevator",idleorderingroup=3}		
		SceneTable["-666_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags09.vcd"), postdelay=5.0,next="-666_05",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="escapeelevator",idleorderingroup=4}	
		SceneTable["-666_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags10.vcd"), postdelay=5.0,next="-666_06",char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="escapeelevator",idleorderingroup=5}				
		SceneTable["-666_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_lift_nags11.vcd"), postdelay=0.0,next=null,char="wheatley",noDingOff=true,noDingOn=true, talkover=true, idlegroup="escapeelevator",idleorderingroup=6}	

		function StartWheatleyElevatorNag()
		{
			printl("==Starting wheatley Nag")
			GladosPlayVcd( -666 )
		}
		
		function StopWheatleyElevatorNag()
		{
			printl("==Stopping wheatley Nag")
			GladosStopNag()
			GladosCharacterStopScene("wheatley")  // this will make wheatley stop talking if he is mid sentence
		}
		
		SceneTable ["-14_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro14.vcd") // Wheatley: I knew it was gonna be cool being in charge of everything, but... WOW, is this cool! This body is amazing!
			char = "wheatley"
			postdelay = 0.7
			predelay = 0.0
			next = "-14_05"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StopWheatleyElevatorNag()",delay=0.0, fireatstart=true}			
				{entity="start_juggling_relay",input="Trigger",parameter="",delay=3, fireatstart=true }
				{entity="stop_juggling_relay",input="Trigger",parameter="",delay=3}
				{entity="@glados",input="RunScriptCode",parameter="CoreRoomHappyFlappingAnim()",delay=3, fireatstart=true}
			]
		}
		
		SceneTable ["-14_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro15.vcd") // Wheatley: And check this out! I'm brilliant now! 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-14_05a"
			noDingOff = true
			noDingOn = true
	
		}
		
			SceneTable ["-14_05a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_spanish01.vcd") // [spanish]
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-14_05b"
			noDingOff = true
			noDingOn = true
	
		}
		
			SceneTable ["-14_05b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro16.vcd") // I don't know what I just said, but I can find out. 
			char = "wheatley"
			postdelay = 0.6
			predelay = 0.0
			next = "-14_06"
			noDingOff = true
			noDingOn = true
	
		}
		
		SceneTable ["-14_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro17.vcd") // Oh! The elevator. Sorry. [ elevator moves ]
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-14_07"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="move_exit_elevator_to_escape_vista_relay",input="Trigger",parameter="",delay=0.2}
			]
		}
		
		SceneTable ["-14_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro21.vcd") // Seriously, body amazing.
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-14_08"
			noDingOff = true
			noDingOn = true
	
		}
		
		SceneTable ["-14_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_body_intro19.vcd") // Wheatley: Wow, look how small you are! 
			char = "wheatley"
			postdelay = 0.1
			predelay = 0.0
			next = "-14_09"
			noDingOff = true
			noDingOn = true
	
		}
		
				SceneTable ["-14_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_actually01.vcd") // But I'm HUGE. [evil laugh] Where did THAT come from? 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-14_10"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="CoreRoomEvilLaughPanelAnim()",delay=1.5, fireatstart=true}
			]
	
		}
		
		SceneTable ["-14_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_actually05.vcd") // Wheatley: Actually... hold on. 
			char = "wheatley"
			postdelay = 0.6
			predelay = 0.0
			next = "-14_11"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="move_exit_elevator_to_smash_start",input="Trigger",parameter="",delay=2.1, fireatstart=true}
				{entity="@glados",input="RunScriptCode",parameter="CoreRoomPanelLookAnim()",delay=0, fireatstart=true}
			]
		}
		
		SceneTable ["-14_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_actually06.vcd") // Wheatley: Why do we have to leave right this minute? [elevator descends]
			char = "wheatley"
			postdelay = 0.4
			predelay = 0.0
			next = "-14_13"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-14_13" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_heelturn06.vcd") // I did that! Me!
			char = "wheatley"
			postdelay = 0.4
			predelay = 0.0
			next = "-14_14"
			noDingOff = true
			noDingOn = true
	
		}
		
		SceneTable ["-14_14" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation07.vcd") // Glados: You didn't do anything. SHE did all the work.
			char = "glados"
			postdelay = 0.4
			predelay = 0.0
			next = "-14_15"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="CoreRoomPanelReturnToNormalAnim()",delay=0, fireatstart=true}
			]
	
		}
		
		SceneTable ["-14_15" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation08.vcd") // Glados: You didn't do anything. SHE did all the work.
			char = "glados"
			postdelay = 0.6
			predelay = 0.0
			next = "-14_16"
			noDingOff = true
			noDingOn = true
	
		}
		
		// ====================================== Wheatley grabs GLaDOS to make her PotatOS.
		
		SceneTable ["-14_16" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_heelturn08.vcd") // Wheatley: Oh really? 
			char = "wheatley"
			postdelay = 0.3
			predelay = 0.0
			next = "-14_17"
			noDingOff = true
			noDingOn = true
			
	
		}	
	
		SceneTable ["-14_17" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_heelturn09.vcd") // Well, maybe it's time I DID something, then.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-14_18"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="MakePotatos()",delay=1.3, fireatstart=true}
			]
	
		}
		
		SceneTable ["-14_18" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_confused04.vcd") // Glados: What are you doing?
			char = "glados"
			postdelay = 0.2
			predelay = 0.0
			next = "-14_20"
			noDingOff = true
			noDingOn = true
	
		}
		
		SceneTable ["-14_20" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_emotion_no01.vcd") // Glados: No!
			char = "glados"
			postdelay = 0.1
			predelay = 0.0
			next = "-14_21"
			noDingOff = true
			noDingOn = true
		
	
		}
		
		SceneTable ["-14_21" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_emotion_no02.vcd") // Glados: No!
			char = "glados"
			postdelay = 0.1
			predelay = 0.0
			next = "-14_22"
			noDingOff = true
			noDingOn = true
		
			
		}
		
		SceneTable ["-14_22" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_emotion_no03.vcd") // Glados: No!
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="DialogDuringPotatosManufacture()",delay=0.5 },
			]
		}
		
		
		// ====================================== Wheatley talks to player while PotatOS is being made
			
		SceneTable ["-10_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_heel_turn07.vcd") //   I'm on to you too lady.. Now who's the boss?
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="wheatley_body",input="setanimation",parameter="glados_wheatley_heel_turn07",delay=0,fireatstart=true} // ANIM
				{entity="@glados",input="RunScriptCode",parameter="PotatosPresentation()",delay=2 },
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PresentPotatos()",delay=0}
				{entity="potato_factory_effects_start_relay",input="trigger",parameter="",delay=0, fireatstart=true}
				{entity="potato_factory_effects_stop_relay",input="trigger",parameter="",delay=0}
				{entity="glados_detached_head",input="kill",parameter="",delay=3.5, fireatstart=true}							 // deletes glados's head
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="DeletePitArms()",delay=5, fireatstart=true}  // deletes all but the small pit arms

			]
		}
		

		
		// ====================================== Wheatley displays potato battery for first time
		
		SceneTable ["-11_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_heel_turn10.vcd") //   Ah!
			char = "wheatley"
			postdelay = 3.0 
			predelay = 0.0
			next = "-11_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="DeliverPotatos()",delay=1 }
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="EnablePincer()",delay=0, fireatstart=true}  // enables visibility on the pincer arm
			]
		}
		
		SceneTable ["-11_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_potato01.vcd") // See that?
			char = "wheatley"
			postdelay = 0.5
			predelay = 0.0
			next = "-11_03"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="DeleteSmallArms()",delay=0, fireatstart=true }
			]
		}
		
		SceneTable ["-11_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_potato02.vcd") // THAT is a potato battery.  Now you live in it.
			char = "wheatley"
			postdelay = 0.5
			predelay = 0.0
			next = "-11_04"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="PotatosPresentation()",delay=0, fireatstart=true },
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="WigglePotatosMultiple()",delay=0.0, fireatstart=true }
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="TapGladosGentlyOnGlass()",delay=1.4, fireatstart=true }
			]
		}
		
		SceneTable ["-11_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_potato04.vcd") // [chuckles]
			char = "wheatley"
			postdelay = -0.2
			predelay = 0.0
			next = "-11_05"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-11_05" ] <-
		{
			// I know you.
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation05.vcd") 			
			char = "glados"
			postdelay = 0.4
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover = true
			fires =
			[
				{entity="begin_potatos_moron_scene_relay",input="Trigger",parameter="",delay=0.5 },
			]
		}
		
// ====================================== Elevator moron scene	
			
		SceneTable ["-12_01" ] <-
		{
			// What?
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_history_response01.vcd") 
			char = "wheatley"
			postdelay = 0.3
			predelay = 0.0
			next = "-12_07"
			noDingOff = true
			noDingOn = true

		}
		
		SceneTable ["-12_07" ] <-
		{
			// The engineers tried everything to make me... behave. To slow me down.                                                                                                                                                                                        
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation11.vcd") 			
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-12_08"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-12_08" ] <-
		{
			// Once, they even attached an Intelligence Dampening Sphere on me. It clung to my brain like a tumor, generating an endless stream of terrible ideas.                                                                                                                                                                                                                                                                                                  
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation12.vcd") 			
			char = "glados"
			postdelay = -9.5
			predelay = 0.0
			next = "-12_09"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-12_09" ] <-
		{
			// No! I'm not listening! I'm not listening!                                                                                                                                                                                                                    
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_history_response13.vcd") 
			char = "wheatley"
			postdelay = -1.3
			predelay = 0.0
			next = "-12_10"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		SceneTable ["-12_10" ] <-
		{
			// It was your voice.  			
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation16.vcd") 			
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = "-12_11"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		SceneTable ["-12_11" ] <-
		{
			// No! No! You're LYING! You're LYING!
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_history_response12.vcd") 
			char = "wheatley"
			postdelay = -1.5
			predelay = 0.0
			next = "-12_12"
			noDingOff = true
			noDingOn = true	

		}
		
		SceneTable ["-12_12" ] <-
		{
			// Yes. You're the tumor.	
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation17.vcd") 			
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-12_13"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
		SceneTable ["-12_13" ] <-
		{
			// I am smart! I'm not a moron!
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_history_response05.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-12_14"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="TapGladosStronglyOnGlass()",delay=0.6, fireatstart=true }
			]
		}
		
		SceneTable ["-12_14" ] <-
		{
			// YES YOU ARE! YOU'RE THE MORON THEY BUILT TO MAKE ME AN IDIOT! 	
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgb_confrontation19.vcd") 			
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = "-12_15"
			noDingOff = true
			noDingOn = true			
			talkover = true
		}
		
		SceneTable ["-12_15" ] <-		
		{
		// How about NOW? Now who's a moron?
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_heel_turn15.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PunchGladosThroughGlass()",delay=0.5, fireatstart=true }
				{entity="begin_elevator_conclusion_relay",input="Trigger",parameter="",delay=0.0 },

			] 
		}
		
		/*SceneTable ["-12_16" ] <-
		{
			// I hate to tell you this... 
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_fgbprefallelevator04.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
			] 			
		}*/
		
		// ====================================== Elevator conclusion
		SceneTable ["-13_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a2_core_pitpunch02.vcd") // Could a moron punch you into this pit?
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-13_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PoundElevatorOne()",delay=1.5, fireatstart=true },
				{entity="smash_elevator_relay",input="Trigger",parameter="",delay=2.6, fireatstart=true },
				
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PoundElevatorTwo()",delay=2.57, fireatstart=true },
				{entity="smash_elevator_relay",input="Trigger",parameter="",delay=3.57, fireatstart=true },
			
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PoundElevatorThree()",delay=3.5, fireatstart=true },
				{entity="smash_elevator_relay",input="Trigger",parameter="",delay=4.5, fireatstart=true },
			
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PoundElevatorFour()",delay=4.6, fireatstart=true },
				{entity="smash_elevator_relay",input="Trigger",parameter="",delay=5.6, fireatstart=true },
			
				{entity="maintenance_pit_script",input="RunScriptCode",parameter="PoundElevatorFive()",delay=5.43, fireatstart=true }
				{entity="smash_elevator_relay",input="Trigger",parameter="",delay=6.43, fireatstart=true }
			
			]
		}
		
		SceneTable ["-13_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sp_sabotage_glados_postxfer09.vcd") // uh oh.
			char = "wheatley"
			postdelay = 0.0
			predelay = 1.5
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="begin_elevator_collapse_relay",input="Trigger",parameter="",delay=0.0 },
				{entity="exit_elevator_groan_sound",input="playsound",parameter="",delay=0.0, fireatstart=true },
			] 
		}
	 
}

	
// ****************************************************************************************************
// SCIENCE FAIR
// ****************************************************************************************************

if (curMapName=="sp_a2_bts4" || "sabotage_offices" )
{
		SceneTable ["-100_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sp_sabotage_panel_sneak01.vcd") // Alright, we took out her turrets. Now lets take out her toxin.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-101_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour41.vcd") // Bring your daughter to work day.  That did not end well.
			char = "wheatley"
			postdelay = 1.0
			predelay = 0.0
			next = "-101_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="runscriptcode",parameter="ScienceFairBusy()",delay=0.0, fireatstart=true }
			] 
		}
		
		SceneTable ["-101_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour42.vcd") // And 40 potato batteries.  Embarassing.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="runscriptcode",parameter="ScienceFairNotBusy()",delay=0.0 },
				{entity="lookat_potato_exhibits_relay",input="trigger",parameter="",delay=0.2, fireatstart=true },
				{entity="wheatley_exhibit_move_triggers",input="enable",parameter="",delay=4.0, fireatstart=true },
				{entity="lookat_player_rl",input="trigger",parameter="",delay=3, fireatstart=true }, 
			] 
		}
		
		
		SceneTable ["-102_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour44.vcd") // Pretty sure we're going the right way. Just to reassure you.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			queue = true
			queuepredelay=1.0
		}
		
		SceneTable ["-103_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour50.vcd") //  Don't worry i'm absolutely guaranteeing you it is this way.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="lookat_deadend_relay",input="trigger",parameter="",delay=2.0, fireatstart=true },			// aim light down deadend
				{entity="move_wheatley_to_deadend_relay",input="trigger",parameter="",delay=3.0, fireatstart=true },	// move towards deadend
				{entity="lookat_deadend_entry_relay",input="trigger",parameter="",delay=3.7, fireatstart=true },		// aim light back
				{entity="move_wheatley_to_exit_relay",input="trigger",parameter="",delay=4.0, fireatstart=true },		// move out of deadend
			]
			
		}
		
		SceneTable ["-104_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/sphere_flashlight_tour51.vcd") //  Definitely sure it's this way.  I dont know where it is.
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
	}
	
// ============================================================================
// Called at end of factory where wheatley isn't quite sure of the way
// ============================================================================
function sp_a2_bts4_end_dialog()
{
	GladosPlayVcd( -104 )
}

// ============================================================================
// Called from sp_sabotage_factory_hackdoor08.vcd during door hack scene in bts4 factory
// Causes sparks to shoot out of computer
// ============================================================================
function bts4_computer_hack_spark()
{
	EntFire("computer_hack_spark_relay", "trigger", 0, 0 )
}

// ****************************************************************************************************
// ****************************************************************************************************

	if (curMapName=="sp_column_blocker")
	{
		//Hey, partner. I knew you'd be comin' through this shaft, so I talked my way onto this nanobot work crew over here that's rebuildin' it. They're REALLY small, so they got tiny little brains. But there's a billion of 'em, so it's only a matter of time
		SceneTable["-2_01"] <- 
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotInto01.vcd")
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-2_02"
			noDingOff = true
			noDingOn = true
		}
		//Hold on... I'm on BREAK, Jerry.
		SceneTable["-2_02"] <- 
		{	
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotOnBreak01.vcd")
			char = "wheatley"
			next = "-2_03"
		}
		//Anyway, look, we're real close to bustin' out. So just hang in there for five more chambers.
		SceneTable["-2_03"] <- 
		{	
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotRealClose01.vcd")
			char = "wheatley"
			next = null
		}
		
		//Ow!
		SceneTable["-3_01"] <- 
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotOw01.vcd")
			char = "wheatley"
			postdelay = 1.0
			predelay = 0.0
			next = "-3_02"
			noDingOff = true
			noDingOn = true
		}
		//What? You can't fire me!
		SceneTable["-3_02"] <- 
		{	
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotFired01.vcd")
			char = "wheatley"
			next = "-3_03"
		}
		//Well, JERRY -- maybe your prejudiced worksite could have accommodated a nanobot of my size! You'll be hearing from my lawyer! Thanks for the HATE CRIME, Jerry!
		SceneTable["-3_03"] <- 
		{	
			vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotLawyers03.vcd")
			postdelay = 0.5
			char = "wheatley"
			next=null
			//next = "-3_04"
		}
		//We're not actually going to sue them, I just don't want them to report this. I don't even HAVE a lawyer. In fact, if I EVER retain counsel, I will DIE. Oh, I gotta go. I'll see you soon.
		//SceneTable["-3_04"] <- 
		//{	
		//	vcd = CreateSceneEntity("scenes/npc/sphere03/NanobotLawyers02.vcd")
		//	char = "wheatley"
		//	next = "-3_05"
		//}



//		NanobotOw01.wav	Ow!
		
//		NanobotFired01.wav	What? You can't fire me!
//		NanobotLawyers01.wav	Well, JERRY -- maybe your prejudiced worksite could have accommodated a nanobot of my size! You'll be hearing from my lawyer! Thanks for the HATE CRIME, Jerry!
//		NanobotLawyers02.wav	We're not actually going to sue them, I just don't want them to report this. I don't even HAVE a lawyer. In fact, if I EVER retain counsel, I will DIE. Oh, I gotta go. I'll see you soon.

	}	

// ****************************************************************************************************
// SP_A2_PIT_FLINGS	companion cube fizzle
// ****************************************************************************************************
if (curMapName=="sp_a2_pit_flings")
	{
		// triggered when player picks up cube for the first time
		SceneTable ["-200_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fizzlecube01.vcd") // Oh. Did I accidentally fizzle that before you could complete the test? I'm sorry.
			char = "glados"
			postdelay = 1.0 
			predelay = 0.0
			next = "-201_01"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-201_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fizzlecube03.vcd") // Go ahead and grab another one so that it won't also fizzle and you won't look stupid again.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="drop_new_box_relay",input="trigger",parameter="",delay=1.0, fireatstart=true },
			]
		}
		
		// triggered when player picks up the second box
		SceneTable ["-203_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fizzlecube05.vcd") // Oh. No. I fizzled that one too.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-204_01"
			noDingOff = true
			noDingOn = true
			queue = true
			fires=
			[
				{entity="drop_new_box_relay",input="trigger",parameter="",delay=0.0 },
				{entity="companion_cube_trigger",input="kill",parameter="",delay=0.0, fireatstart=true },
			]
		}
		
		SceneTable ["-204_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fizzlecube06.vcd") // Oh well. We have warehouses FULL of the things. Absolutely worthless. I'm happy to get rid of them.
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// This line plays from the PuzzleCompleted call
		SceneTable ["sp_a2_pit_flingsCubeSmuggleEnding01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_pit_flings03.vcd")  // Every test chamber is equiped with a fizzler. This one is broken.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "sp_a2_pit_flingsCubeSmuggleEnding02"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["sp_a2_pit_flingsCubeSmuggleEnding02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_pit_flings02.vcd")  // Don't take anything with you.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="smuggled_cube_fizzle_trigger",input="enable",parameter="",delay=0.0},
				{entity="glados_summon_elevator_relay",input="trigger",parameter="",delay=0.5, fireatstart=true }
			]	
		}

		// called when smuggled cube fizzles
		SceneTable ["-206_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_pit_flings06.vcd") // I think that one was about to say "I love you." They ARE sentient, of course. We just have a LOT of them.
			char = "glados"
			postdelay = 0.0 
			predelay = 2
			next = null
			noDingOff = true
			noDingOn = true
			fires=[{entity="@transition_script",input="runscriptcode",parameter="TransitionReady()",delay=0.0}]	
		}
		
		// Player got stuck, glados taunts then spawns another cube
		SceneTable ["-207_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/fizzlecube02.vcd") // go ahead and grab another one
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="drop_new_box_relay",input="trigger",parameter="",delay=0.0}
			]	
		}
		
	}

	
// ============================================================================
// Triggered when the player gets into an unwinnable state
// ============================================================================
function sp_a2_pit_flings_player_stuck()
{
		GladosPlayVcd( -207 )
}

// ============================================================================
// Drop a companion cube out of a dropper in sp_a2_pit_flings
// if the player gets into an unwinnable situation
// NOTE: This is called EVERY TIME the cube is lost.
//		 The very first time a cube is dropped it will be a companion cube
// ============================================================================

if (curMapName=="sp_a2_pit_flings")
{
	boxDissolveCount <- 1
}

function sp_laser_lift_pit_flings_cube_lost()
{
	
		// play different dialog for the various boxes that drop
		switch ( boxDissolveCount )
		{
			case 1:
				GladosPlayVcd( -200 )
				break
			case 2:
				GladosPlayVcd( -203 )
				break
			default:
				printl("====== Cube lost! spawning a new one...")
				EntFire( "drop_new_box_relay", "trigger", 0, 0, 0 )
				break
		}
		boxDissolveCount+=1
}


// ============================================================================
// Called when sp_laser_lift_pit_flings companion cube is picked up
// ============================================================================
function sp_laser_lift_pit_flings_companion_cube_dissolved()
{
		printl("***DISSOLVING cube_dropper_box!")
		
		// spawn a new box whenever ready
		sp_laser_lift_pit_flings_cube_lost();
}


// ============================================================================
// Called if player tries to smuggle cube out of level
// ============================================================================
function sp_a2_pit_flings_smuggled_cube_fizzle()
{
	GladosPlayVcd( -206 )
}



// ****************************************************************************************************
// SP_A2_TRUST_FLING
// ****************************************************************************************************
 if (curMapName=="sp_a2_trust_fling")
	{
		SceneTable ["-300_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_trust_fling01.vcd") // Oh, sorry. Some of these test chambers haven't been cleaned in ages.
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = "-301_01"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-301_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_trust_fling02.vcd") // So sometimes there's still trash in them. Standing around. Smelling, and being useless.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-302_01"
			noDingOff = true
			noDingOn = true
		}
		

		// triggered when second box hits the ground
		SceneTable ["-302_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_trust_fling03.vcd") // Try to avoid the garbage hurtling towards you.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.5
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="garbage_pickup_relay",input="enable",parameter="",delay=0.5 },
			]
		}
		
		
		// triggered when player picks up the second box
		SceneTable ["-303_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_trust_fling04.vcd") // Don't TEST with the garbage. It's garbage.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="button_press_dialog_relay",input="trigger",parameter="",delay=0.5 },
			]
		}
		
		SceneTable ["-304_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/faithplategarbage06.vcd") // Press the button again.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
	}


// ============================================================================
// Called when trust fling dropper button is first pressed
// ============================================================================
function sp_a2_trust_fling_garbage_spawn()
{
	GladosPlayVcd( -300 )
}

// ============================================================================
// Called when trust fling garbage is picked up
// ============================================================================
function sp_a2_trust_fling_garbage_pickup()
{
	GladosPlayVcd( -303 )
}


// ============================================================================
// This VCD is chained from the previous one through a map relay and will only
// play if the button has not already been pressed
// ============================================================================
function sp_a2_trust_fling_button_press_reminder()
{
	GladosPlayVcd( -304 )
}

// ============================================================================
// This VCD is chained from the previous one through a map relay and will only
// play if the button has not already been pressed
// ============================================================================
function sp_a2_trust_fling_elevator_stop()
{
	printl("I should be triggering sp_laser_over_gooElevatorStop01")
	printl("The current function that does this is sp_a2_laser_over_goo_elevator_stoppage() in glados.nut")
	GladosPlayVcd(465)
}


// ****************************************************************************************************
// SP_A2_COLUMN_BLOCKER	(birthday surprise scene)		  
// ****************************************************************************************************
if (curMapName=="sp_a2_column_blocker")
{
		SceneTable ["-400_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/testchambermisc34.vcd") // initiating surprise in 3...2...1.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-401_01"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="surprise_room_lights_on",input="Trigger",parameter="",delay=0.2},
			]
		}
		
		SceneTable ["-401_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/testchambermisc35.vcd") // i made it all up
			char = "glados"
			postdelay = 2.5 
			predelay = 2.0
			next = "-402_01"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="surprise_room_effects_relay",input="Trigger",parameter="",delay=0.6},
				{entity="surprise_room_party_horn_sound",input="playsound",parameter="",delay=0.3}
			]
		}
		
		SceneTable ["-402_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/testchambermisc41.vcd") // surprise.
			char = "glados"
			postdelay = 2.0 
			predelay = 0.0
			next = "-403_01"
			noDingOff = true
			noDingOn = true
		}
		
		SceneTable ["-403_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_a2_column_blocker01.vcd") // your parents are probably dead. ...i doubt they'd want to see you.
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="surprise_room_door_relay",input="trigger",parameter="",delay=1.5, fireatstart=true}
			]
		}
}


// ****************************************************************************************************
// SP_A2_BRIDGE_THE_GAP - BIRD!
// ****************************************************************************************************

if (curMapName=="sp_a2_bridge_the_gap")
{
		//Perfect, the door's malfunctioning. I guess somebody's going to have to repair that too. [beat] No, don't get up. I'll be right back. Don't touch anything.
		SceneTable["-500_00"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/sp_trust_fling_sphereinterrupt01.vcd")
			postdelay=0.000
			next=null
			char="glados"
			fires=
			[
				{entity="start_wheatley_window_scene_relay",input="trigger",parameter="",delay=3.0}
			]
			predelay=0.2
			queue=1 
		}

	//Hey! Hey! Up here!
	SceneTable["-500_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_trust_fling01.vcd"),postdelay=0.2,next="-500_02",char="wheatley",noDingOff=true,noDingOn=true}

	//I found some bird eggs up here. Just dropped 'em into the door mechanism.  Shut it right down. I--AGH!
	SceneTable["-500_02"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/sp_trust_flingAlt07.vcd")
		postdelay=0.1
		next="-500_03"
		char="wheatley"
		noDingOff=true
		noDingOn=true
		fires=
		[
			{entity="bird_attack_start_relay",input="Trigger",parameter="",delay=3.6, fireatstart=true},
		]
	}

	//BIRD BIRD BIRD BIRD
	SceneTable["-500_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_trust_flingAlt02.vcd"),postdelay=2.5,next="-500_04",char="wheatley",noDingOff=true,noDingOn=true}

	//[out of breath] Okay. That's probably the bird, isn't it? That laid the eggs! Livid!
	SceneTable["-500_04"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/sp_trust_flingAlt08.vcd")
		postdelay=0.4
		next="-500_05"
		char="wheatley"
		noDingOff=true
		noDingOn=true
		fires=
		[
			{entity="bird_attack_end_relay",input="Trigger",parameter="",delay=0.0, fireatstart=true} 
		]
	}

	//Okay, look, the point is, we’re gonna break out of here! Very soon, I promise! 
	SceneTable["-500_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_bridge_the_gap_expo01.vcd"),postdelay=0.1,next="-500_06",char="wheatley",noDingOff=true,noDingOn=true}

	//I just have to figure out how.
	SceneTable["-500_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_bridge_the_gap_expo03.vcd"),postdelay=0.4,next="-500_07",char="wheatley",noDingOff=true,noDingOn=true}

	//Here she comes! Just keep testing! Remember, you never saw me!
	SceneTable["-500_07"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/sp_a2_bridge_the_gap_expo06.vcd")
		postdelay=1
		next=null
		char="wheatley"
		fires=
		[
			{entity="trick_door_open_relay",input="Trigger",parameter="",delay=3.3,fireatstart=true},
			{entity="wheatley_depart_scene_relay",input="Trigger",parameter="",delay=3,fireatstart=true}
		]
		noDingOff=true
		noDingOn=true
	}
}

// ============================================================================
// Called when door malfunction begins
// ============================================================================
function sp_a2_bridge_the_gap_brokendoor_scene()
{
	GladosPlayVcd( -500 )
}

// ============================================================================
// Starts the wheatley window / bird scene
// ============================================================================
function sp_a2_bridge_the_gap_window_scene()
{
	printl("=====================***************************************CALLING WINDOW SCENE")
	GladosPlayVcd( -501 )
}



if (curMapName=="sp_a2_fizzler_intro")
{
	//Nevermind. I have to go... check something. Test on your own recognizance. I'll be back.
	SceneTable["-50_01"] <- 
	{
		vcd = CreateSceneEntity("scenes/npc/glados/sp_trust_fling_sphereinterrupt03.vcd")
		char = "glados"
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
	}
}

// ============================================================================
// Called after the distant explosion
// ============================================================================
function sp_a2_fizzler_training_distant_explosion()
{
	GladosPlayVcd( -950 )
}

// ****************************************************************************************************
// SP_A1_INTRO1 scenetable			  
// ****************************************************************************************************

SceneTableLookup[-600] <- "-600_01" // start of the open the door string of dialog

SceneTableLookup[-601] <- "-601_01" // Nags if player waited through the entire opening sequence

SceneTableLookup[-602] <- "-602_01" // First Sequence when door is opened

SceneTableLookup[-603] <- "-603_01" // Nag telling the player to say apple

SceneTableLookup[-604] <- "-604_01" // Jumping is close enough to saying apple

SceneTableLookup[-605] <- "-605_01" // First Impact with the wall above dock

SceneTableLookup[-606] <- "-606_01" // When the container first starts it's ride

SceneTableLookup[-607] <- "-607_01" // When wheatley re-enters and lets the player know that they have brain damage

// ****************************************************************************************************
// SP_A1_INTRO1 (container ride scene)		  
// ****************************************************************************************************
if (curMapName=="demo_intro" || curMapName=="sp_a1_intro1_no_sound" )
{
	//Hello?
	SceneTable["-600_01"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello01.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 0.0
		next = "-600_02"
		noDingOff = true
		noDingOn = true
//		queue = true
//		queuetimeout = 10
		fires=
		[
			{entity="container_door_knock_2_relay",input="trigger",parameter="",delay=0.0}
		]		
	}
	//Helloooo? 
	SceneTable["-600_02"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello12.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_03"
	}
	//Are you going to open the door? At any time?
	SceneTable["-600_03"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello13.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_05"
	}
	//Hello? Can y--no?
	SceneTable["-600_05"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello15.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_07"
	}
	//Are you going to open this door? Because it's fairly urgent.
	SceneTable["-600_07"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello17.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_08"
	}
	//Oh, just open the door! [to self] That's too aggressive. [loud again] Hello, friend! Why not open the door?
	SceneTable["-600_08"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello18.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_09"
	}
	//[to self] Hm. Could be Spanish, could be Spanish. [loud again] Hola, amigo! Abre la puerta! Donde esta--no. Um...
	SceneTable["-600_09"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello19.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_11"
	}
	//Fine! No, absolutely fine. It's not like I don't have, you know, ten thousand other test subjects begging me to help them escape. You know, it's not like this place is about to EXPLODE.
	SceneTable["-600_11"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello21.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-600_12"
		fires=
		[
			{entity="container_door_knock_3_relay",input="trigger",parameter="",delay=1.0}
		]
	}
	//Alright, look, okay, I'll be honest. You're the LAST test subject left. And if you DON'T help me, we're both going to die. Alright? I didn't want to say it, you dragged it out of me. Alright? Dead. Dos Muerte.
	SceneTable["-600_12"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHello22.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 2.0
		next = null
		fires=
		[
			{entity="@glados",input="RunScriptCode",parameter="sp_a1_intro1_open_door_nags()",delay=1.0}
		]
	}
	
	///////////////////////////////////////////////
	///////////////////////////////////////////////
	
	//Hello!
	SceneTable["-601_01"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHelloNag01.vcd"),
		postdelay=0.1,
		next=null,
		char="wheatley",
		noDingOff=true,
		noDingOn=true,
		idle=true,
		idlerandom=true,
		idlerepeat=true,
		idleminsecs=1.00,
		idlemaxsecs=2.600,
		idlegroup="sp_a1_intro1_open_door_nag",
		idleorderingroup=1
	}	
	//Helloooooooooooo!
	SceneTable["-601_02"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHelloNag02.vcd"),
		postdelay=0.1,
		next=null,
		char="wheatley",
		noDingOff=true,
		noDingOn=true,
		idlegroup="sp_a1_intro1_open_door_nag",
		idleorderingroup=2
	}
	//Come on!
	SceneTable["-601_03"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHelloNag03.vcd"),
		postdelay=0.1,
		next=null,
		char="wheatley",
		noDingOff=true,
		noDingOn=true,
		idlegroup="sp_a1_intro1_open_door_nag",
		idleorderingroup=3
	}
	//Open the door!
	SceneTable["-601_04"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHelloNag04.vcd"),
		postdelay=0.1,
		next=null,
		char="wheatley",
		noDingOff=true,
		noDingOn=true,
		idlegroup="sp_a1_intro1_open_door_nag",
		idleorderingroup=4
	}
	//Hello!
	SceneTable["-601_05"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningHelloNag05.vcd"),
		postdelay=0.1,
		next=null,
		char="wheatley",
		noDingOff=true,
		noDingOn=true,
		idlegroup="sp_a1_intro1_open_door_nag",
		idleorderingroup=5
	}
	
	///////////////////////////////////////////////
	///////////////////////////////////////////////

	//YES! I KNEW someone was alive in here!
	SceneTable["-602_01"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningDoorOpened01.vcd"),
		char="wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-602_02"
		noDingOff = true
		noDingOn = true
		fires=
		[
//			{entity="are_you_alright_vcd",input="Start",parameter="",delay=0.6}
			{entity="spherebot_train_1_chassis_1",input="MoveToPathNode",parameter="spherebot_path_inside_hallway",delay=3.5}
			{entity="@rl_container_ride_wheatley_enter",input="Trigger",parameter="",delay=3.5}
		]
	}
	
	//AGH! You look TERRIB--you look good. Looking good, actually. If I'm honest.
	SceneTable["-602_02"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningFirstLook01.vcd"),
		char="wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-602_03"
		noDingOff = true
		noDingOn = true
	}
	
	//How are you? How you feeling, you okay? Wait. Don't answer that. Too much deep relaxation, what it does is it relaxes the gums. And the vibrations from talking 
	SceneTable["-602_03"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningPanicSituation01.vcd"),
		char="wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = "-602_04"
		noDingOff = true
		noDingOn = true
	}
	
	//So, word of advice: there's PLENTY of time to recover. Just take it slow.
	SceneTable["-602_04"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningPanicSituation03.vcd"),
		char="wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		fires=
		[
			{entity="emergency_evacuation_vcd",input="Start",parameter="",delay=0.0}
		]
	}
	
	///////////////////////////////////////////////
	///////////////////////////////////////////////

	//Simple word. 'Apple'.
	SceneTable["-603_01"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamageAppleNag01.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.5
		next = "-603_02"
		noDingOff = true
		noDingOn = true
//		queue = true
//		queuetimeout = 10
	}
	//Just say 'Apple'. Classic. Very simple.
	SceneTable["-603_02"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamageAppleNag02.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-603_03"
	}
	//Ay. Double Pee-Ell-Ee.
	SceneTable["-603_03"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamageAppleNag03.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-603_04"
	}
	//Just say 'Apple'. Easy word, isn't it? 'Apple'.
	SceneTable["-603_04"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamageAppleNag04.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = "-603_05"
	}
	//How would you use it in a sentence? "Mmm, this apple's crunchy," you might say. And I'm not even asking you for the whole sentence. Just the word 'Apple'.
	SceneTable["-603_05"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamageAppleNag05.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 1.0
		next = null
		fires=
		[
			{entity="sphere_player_has_pressed_space_second",input="Trigger",parameter="",delay=1.5}
		]
	}
	
		///////////////////////////////////////////////
	///////////////////////////////////////////////
	
	// Let me explain. Most test subjects do experience some cognitive deterioration after a few months in suspension.
	SceneTable["-607_01"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamage01.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 0.0
		next = "-607_02"
		noDingOff = true
		noDingOn = true
		queue = true
		queuetimeout = 10
	}

	//Straight away you're thinking, "Oo, that doesn't sound good. But don't be alarmed, alright? Because ah... well, actually, if you DO feel alarmed, hold onto that.
	SceneTable["-607_02"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamage03.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 0.0
		next = "-607_03"
	}
	
	//Do you understand what I'm saying? Does any of this make sense? Just tell me 'Yes'.
	SceneTable["-607_03"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningBrainDamage04.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 0.0
		next = null
		fires=
		[
			{entity="hint_press_spacebar_to_talk",input="ShowHint",parameter="",delay=0.0}
			{entity="sphere_player_has_pressed_space_first",input="Enable",parameter="",delay=0.0}
		]
	}
	
	///////////////////////////////////////////////
	///////////////////////////////////////////////

	//Okay, you know what? That's close enough. Just hold tight.
	SceneTable["-604_01"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningCloseEnough01.vcd"),
		char="wheatley"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		fires=
		[
			{entity="core_meltdown_rl",input="Trigger",parameter="",delay=0.0}
		]
	}
}

if (curMapName=="sp_a1_intro11" )
{	
	///////////////////////////////////////////////
	///////////////////////////////////////////////

	//Ohhhhhh! - removed
	SceneTable["-605_02"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningWallHitOne01.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 3.5
		next = "-605_03"
		noDingOff = true
		noDingOn = true
		queue = true
		queuetimeout = 10
	}
	
	//Good news: that is NOT a docking station. So there's one mystery solved. I'm going to attempt a manual override on this wall. Could get a bit technical! Hold on!
	SceneTable["-605_01"] <-
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningWallHitOne02.vcd"),
		char="wheatley"
		postdelay=0.0,
		predelay = 3.5
		next = null
		noDingOff = true
		noDingOn = true
		queue = true
		queuetimeout = 10
		
		fires=
		[
			{entity="ram_exit_wall_relay",input="Trigger",parameter="",delay=0.0}
			{entity="almost_there_vcd",input="Start",parameter="",delay=2.6}
			{entity="@modify_collision_2_rl",input="Trigger",parameter="",delay=8.0}
		]
	}
	
	///////////////////////////////////////////////
	///////////////////////////////////////////////
	
	// Alright, I wasn't going to mention this to you, but I am in PRETTY HOT WATER here.
	SceneTable["-606_01"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningOutside01.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 0.0
		next = "-606_02"
		noDingOff = true
		noDingOn = true
		queue = true
		queuetimeout = 10
	}


	//How you doing down there? You still holding on?
	SceneTable["-606_02"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningNavigate17.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_03"
	}

	// The reserve power ran out, so of course the whole relaxation center stops waking up the bloody test subjects.
	SceneTable["-606_03"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningOutside02.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_04"
	}
	
	//Hold on! This is a bit tricky!
	SceneTable["-606_04"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningNavigate05.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_05"
				
		fires=
		[
			{entity="@modify_collision_rl",input="Trigger",parameter="",delay=3.0}
		]		

	}


	//And of course nobody tells ME anything. No.
    SceneTable["-606_05"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningOutside03.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_06"
	}


	//Anyway, why should I be kept informed about the life functions of the ten thousand bloody test subjects I'm supposed to be in charge of? Why would you mention it to me?
	SceneTable["-606_06"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningOutside04.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_07"
	}

	//Oi, it's close... can you see? Am I gonna make it through? Have I got enough space?
	SceneTable["-606_07"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningNavigate12.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_08"
	}
	//Agh, just... I just gotta get it through here...
	SceneTable["-606_08"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningNavigate06.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_09"
	}
	//Okay, I've just gotta concentrate!
	SceneTable["-606_09"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningNavigate03.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.0
		next = "-606_10"
	}

	//And whose fault do you think it's going to be when the management comes down here and finds ten thousand flipping vegetables?
	SceneTable["-606_10"] <- 
	{
		vcd=CreateSceneEntity("scenes/npc/sphere03/OpeningOutside05.vcd"),
		char="wheatley"
		postdelay = 0.0,
		predelay = 1.3
		next = null
	}
}

// ============================================================================
// Called when wheatley wants you to open the door
// ============================================================================
function sp_a1_intro1_open_door_sequence()
{
	GladosPlayVcd( -600 )
}

function sp_a1_intro1_open_door_nags() 
{
	GladosPlayVcd( -601 )
}

function sp_a1_intro1_knew_someone_alive()
{
	GladosCharacterStopScene("wheatley")
	GladosPlayVcd( -602 )
}

function sp_a1_intro1_say_apple_nag()
{
	GladosPlayVcd( -603 )
}

function sp_a1_intro1_jumping_close_enough()
{
	GladosCharacterStopScene("wheatley")
	GladosPlayVcd( -604 )
}

function sp_a1_intro1_first_wall_impact()
{
	GladosPlayVcd( -605 )
}

function sp_a1_intro1_container_start_moving()
{
	GladosPlayVcd( -606 )
}

function sp_a1_intro1_explain_brain_damage()
{
	GladosPlayVcd( -607 )
}


// ****************************************************************************************************
// SP_A2_CORE FUNCTIONS
// ****************************************************************************************************

// one time setup only for the core map
// this will set up a table of all the panels in the map and assign a random
// wilt animation for the glados powerdown scene.  on powerup this will allow us
// to play the corresponding unwilt animation

if (curMapName=="sp_a2_core" )
{

	::GlobalPanelAnimationList <- []
		
	
	for ( local i=1; i < 25; i++ )
	{
		entry <-[]
	
		local random_anim = RandomInt(1,4)
		printl("=== random anim is: " + random_anim )
		
		entry <- 
		{ 
			panel_name = "chamber_arm_" + i
			wilt_animation = "core_arms_WILT_0" + random_anim
			unwilt_animation = "core_arms_UNWILT_0" + random_anim
			
			lookup_small_animation = "core_arms_LOOKUP_SMALL_0" + random_anim
			lookup_animation = "core_arms_LOOKUP_0" + random_anim
			lookup_idle_animation = "core_arms_LOOKUP_IDLE_0" + random_anim
			lookup_return_animation = "core_arms_LOOKUP_RETURN_0" + random_anim
			
		}

		GlobalPanelAnimationList.append( entry )
	
		//printl("====== Adding " + entry.panel_name + " " + entry.wilt_animation + " " + entry.unwilt_animation + " to table. Table Length = " + GlobalPanelAnimationList.len() )
	}
}



// ============================================================================
// Causes the room to ripple all at once
// ============================================================================
function CoreRoomRippleAnim()
{
	EntFire( "chamber_arm_*", "setanimation", "core_ripple", 0 )
}

// ============================================================================
// Causes the room to wilt all at once
// ============================================================================
function CoreRoomWiltAnim()
{
	// ensure that the panels have been swapped (this may have occured already after stalemate, this is just a required failsafe)
	EntFire("swap_stalemate_panels_rl", "trigger", 0, 0 )
	
	foreach( index, val in GlobalPanelAnimationList )
	{
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetAnimation", GlobalPanelAnimationList[index].wilt_animation, 0 )
	}
	
	// blink the skin from off to blue
	EntFire( "blink_panel_error_lights_timer", "enable", 0, 0 )
}

// ============================================================================
// Unwilt the room!
// ============================================================================
function CoreRoomUnWiltAnim()
{
	// stop the blinking
	EntFire( "blink_panel_error_lights_timer", "disable", 0, 0 )
	
	EntFire("@glados", "RunScriptCode", "SetArmSkinOn()", 1.1 )
	EntFire("@glados", "RunScriptCode", "SetArmSkinOff()", 1.2 )
	
	EntFire("@glados", "RunScriptCode", "SetArmSkinOn()", 1.3 )
	EntFire("@glados", "RunScriptCode", "SetArmSkinOff()", 1.4 )
	
	EntFire("@glados", "RunScriptCode", "SetArmSkinOn()", 1.5 )
	
	// ripple the room once the light blinking stops
	EntFire("@glados", "RunScriptCode", "CoreRoomRippleAnim()", 1.5 )
	
	foreach( index, val in GlobalPanelAnimationList )
	{
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetAnimation", GlobalPanelAnimationList[index].unwilt_animation, 0 )
	}	
}


// ============================================================================
// Happy animation -- panel wave around the room while wheatley spins
// ============================================================================
function CoreRoomHappyCounterClockwiseAnim()
{
	delay <- 0
	// play wave counter clockwise, starting at the middle
	for( local i=12; i>=0; i-- )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_arms_WIGGLE_01", delay )
		delay += 0.1
	}
	
	// play wave counter clockwise, picking up from start
	for( local i=24; i>=17; i-- )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_arms_WIGGLE_01", delay )
		delay += 0.1
	}
	
	// **TODO** this needs to play a wiggle animation going the other way
	
	// now go the other way!
	for( local i=18; i<=24; i++ )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_arms_WIGGLE_02", delay )
		delay += 0.1
	}
	
	// now go the other way!
	for( local i=1; i<=12; i++ )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_arms_WIGGLE_02", delay )
		delay += 0.1
	}
		
}

// ============================================================================
// Happy flappin' -- flap the panels with random animations
// ============================================================================
function CoreRoomHappyFlappingAnim()
{
	// flap all the panels
	for( local i=1; i<=24; i++ )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_arms_HAPPY_" + RandomInt(1,12) , 0 )
	}
}

// ============================================================================
// Plays the panel animations for the wheatley evil chuckle scene
// ============================================================================
function CoreRoomEvilLaughPanelAnim()
{
	foreach( index, val in GlobalPanelAnimationList )
	{
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetAnimation", GlobalPanelAnimationList[index].lookup_small_animation, 0 )
	}
}



// ============================================================================
// Makes the panels look at the player
// ============================================================================
function CoreRoomPanelLookAnim()
{
	foreach( index, val in GlobalPanelAnimationList )
	{
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetAnimation", GlobalPanelAnimationList[index].lookup_animation, 0 )
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetDefaultAnimation", GlobalPanelAnimationList[index].lookup_idle_animation, 0.1 )
	}
}


// ============================================================================
// Makes the panels return to their normal position
// ============================================================================
function CoreRoomPanelReturnToNormalAnim()
{
	foreach( index, val in GlobalPanelAnimationList )
	{
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetAnimation", GlobalPanelAnimationList[index].lookup_return_animation, 0 )
		EntFire( GlobalPanelAnimationList[index].panel_name, "SetDefaultAnimation", "", 0.1 )
	}
}



// ============================================================================
// Causes the room to ripple in a pattern starting from the left and right
// of the stalemate room and ending at the back of the room
// ============================================================================
function CoreRoomLeftRightWaveAnim()
{
	delay <- 0
	
	// ripple right side of room
	for( local i=1; i<12; i++ )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_ripple", delay )
		delay += 0.2
	}
	
	delay = 0
	
	// ripple left side of room
	for( local i=24; i>=12; i-- )
	{
		EntFire( "chamber_arm_" + i, "setanimation", "core_ripple", delay )
		delay += 0.2
	}
	
}


// ============================================================================
// Turn off arm skins
// ============================================================================
function SetArmSkinOff()
{
	EntFire( "chamber_arm_*", "skin", "2", 0 )
}

// ============================================================================
// Turn on arm skins
// ============================================================================
function SetArmSkinOn()
{
	EntFire( "chamber_arm_*", "skin", "0", 0 )
}


// track stalemate room light state
StalemateRoomLeadLightsOn <- 0

// ============================================================================
// ============================================================================
function StartStalemateRoomLeadLights()
{
	SetArmSkinOff()
	
	StalemateRoomLeadLightsOn = 1
	
	EntFire("@glados", "RunScriptCode", "StalemateRoomLeadLights()", 0 )
}

// ============================================================================
// ============================================================================
function StopStalemateRoomLeadLights()
{	
	StalemateRoomLeadLightsOn = 0
	
	EntFire("stalemate_runway_lights_relay", "cancelpending", "", 0 )
	EntFire("@glados", "RunScriptCode", "SetArmSkinOn()", 2.7 )
}

// ============================================================================
// Causes arm lights to animate towards stalemate room
// ============================================================================
function StalemateRoomLeadLights()
{
	if ( StalemateRoomLeadLightsOn )
	{
		delay <- 0
		
		// blink right side of room
		for( local i=12; i>2; i-- )
		{
			EntFire( "chamber_arm_" + i, "skin", "1", delay )
			EntFire( "chamber_arm_" + i, "skin", "2", delay + 0.1 )
			delay += 0.2
		}
		
		delay = 0
		
		// blink left side of room
		for( local i=14; i<=23; i++ )
		{
			EntFire( "chamber_arm_" + i, "skin", "1", delay )
			EntFire( "chamber_arm_" + i, "skin", "2", delay + 0.1 )
			delay += 0.2
		}
		
		EntFire("stalemate_runway_lights_relay", "trigger", "", 2 )
	}
	
}

// ****************************************************************************************************
// SP_A2_BTS5 FUNCTIONS
// ****************************************************************************************************

if (curMapName=="sp_a2_bts5")
{
	bts5_cuts <- 0
}


function ToxinWheatleyGreetsYou() // this tank is big - let's get to the observation room
{
	GladosPlayVcd(561)
}

function ToxinTurretDestructionOurHandiwork() // when the players see the good turrets into the crusher
{
	GladosPlayVcd(562)
}

function ToxinTheDoorIsLocked() // As the player gets to the top of the lift
{
	GladosPlayVcd(563)
}

function ToxinDoorIsNowOpenMovingToMonitor() // When the player disables the panels
{
	GladosPlayVcd(564)
}

function ToxinCutOffFrontOfRoom() // when the player chops of the front of the room
{
	nuke()
	GladosPlayVcd(565)
}

function ToxinPipeCut() // called when *each* pipe is cut
{
	bts5_cuts+=1

	switch (bts5_cuts)
	{
		case 1:
			GladosPlayVcd(566)
			break
		case 4:
			GladosPlayVcd(567)
			break
		case 5:
			GladosPlayVcd(568)
			break
		case 8:
			GladosPlayVcd(569)
			break
		default:
			break
	}
}

// we will delay the glass breaking until the destruction is played out.
function ToxinGettingPulledIntoPipe()
{
	nuke()
	GladosPlayVcd(570)
}


// ****************************************************************************************************
// A2 CORE SPECIALS
// ****************************************************************************************************
function sp_sabotage_glados_specials(arg)
{
	switch (arg)
	{
		case 1: 
			SendToConsole( "scene_playvcd npc/sphere03/fgb_ready_glados06")
			break
		case 2: 
			//SendToConsole( "scene_playvcd npc/sphere03/gladosbattle_pre09")
			//pullmeout pullmeout
			SendToConsole( "scene_playvcd npc/sphere03/sp_a2_core_pullmeout01")
			break
	}
}


// ****************************************************************************************************
// SP_A1_WAKEUP FUNCTIONS
// ****************************************************************************************************
function sp_a1_wakeup_ElevatorBreakerRideFinished()
{
	//printl("sp_a1_wakeup_ElevatorBreakerRideFinished()")
	
	GladosPlayVcd(589)
	
	// This started with the anouncer "Powerup Initiated"	
	
	// when we are finished with all of the dialog then call this
	// the time in here is when the sequence normaly would end
	//EntFire( "relay_start_claw_pickup", "Trigger", "", 44.40 )
}

function sp_a1_wakeup_playbuzzer()
{
	self.EmitSound("World.HackBuzzer")
}	

function sp_a1_wakeup_WheatleyGettingGrabbed()
{
	EntFire("lcs_wheatley_sphere_crush","Start", "", 0)
	
	//printl("sp_a1_wakeup_WheatleyGettingGrabbed()")

	// This will get automatically called from the claw animation sequence
}

function sp_a1_wakeup_DroppedInIncinerator()
{
	//printl("sp_a1_wakeup_DroppedInIncinerator()")

	// Called when the pincer releases you
}

