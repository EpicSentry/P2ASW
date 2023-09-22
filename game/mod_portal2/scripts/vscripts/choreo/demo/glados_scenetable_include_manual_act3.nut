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
					cave_body
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

SceneTableLookup[-3000] <- "-3000_01" // potatos falling with player down the bottomless pit

SceneTableLookup[-3001] <- "-3001_01" // find potatos for first time

SceneTableLookup[-3002] <- "-3002_01" // pick up potatos for first time

SceneTableLookup[-3003] <- "-3003_01" // discover 'Caroline' in portrait in office

/*
SceneTableLookup[-3004] <- "-3004_01" // discover the Cave Johnson cube

SceneTableLookup[-3005] <- "-3005_01" // kill the Cave Johnson cube

SceneTableLookup[-3006] <- "-3006_01" // step on the Cave Johnson cube and linger

SceneTableLookup[-3007] <- "-3007_01" // a short time after the Cave 

SceneTableLookup[-3008] <- "-3008_01" // Cave says thanks for testing in transition01

SceneTableLookup[-3009] <- "-3009_01" // Potatos realises she didn't design speed_ramp

SceneTableLookup[-3010] <- "-3010_01" // Potatos says you'd be a corpse if she designed that puzzle

SceneTableLookup[-3011] <- "-3011_01" // Potatos says she would have put neurotixin into the cube you just found

SceneTableLookup[-3012] <- "-3012_01" // Potatos says her tests are good and these are garbage

SceneTableLookup[-3013] <- "-3013_01" // Potatos comments on the subtleties of human literature

SceneTableLookup[-3014] <- "-3014_01" // Potatos completes her thoughts on literature

SceneTableLookup[-3015] <- "-3015_01" // Potatos comments on the white paint

SceneTableLookup[-3016] <- "-3016_01" // Potatos knows about white paint while Wheatley does not

SceneTableLookup[-3017] <- "-3017_01" // Potatos comments on the repercussions of Wheatley being in charge

SceneTableLookup[-3018] <- "-3018_01" // Potatos tells you the plan of putting her back in charge

SceneTableLookup[-3019] <- "-3019_01" // Potatos tells you she'll let you go when she's back in charge
*/

	//=================================================================
        //Called when PotatOS is falling down bottomless pit
	//=================================================================
	if (curMapName=="sp_a3_00")
	{
		//Glados: Oh hi.
		SceneTable["-3000_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech01.vcd"),
			postdelay=4.0,
			next="-3000_02",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: So, how are you holding up?
		SceneTable["-3000_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech02.vcd"),
			postdelay=1.0,
			next="-3000_03",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: BECAUSE I'M A POTATO.
		SceneTable["-3000_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech03.vcd"),
			postdelay=1.0,
			next="-3000_04",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: I don't know if you've noticed, but I have a weakness for neurotoxin.
		SceneTable["-3000_04"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech13.vcd"),
			postdelay=1.0,
			next="-3000_05",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: I'll be honest, it's a problem the engineers really wrestled with. They tried everything. They hid the neurotoxin; they printed signs that said "Don't neurotoxin anyone" and hung them where I couldn't miss them.
		SceneTable["-3000_05"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech14.vcd"),
			postdelay=1.0,
			next="-3000_06",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Once, they even installed an Intelligence Dampening Sphere. It was designed – by the greatest minds of a generation – for one purpose: to generate an endless stream of dumb ideas. I always wondered what happened to it. Now I know: You just put it in
		SceneTable["-3000_06"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech15.vcd"),
			postdelay=1.0,
			next="-3000_07",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: (slow clap) That's a slow clap sound.
		SceneTable["-3000_07"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech08.vcd"),
			postdelay=3.0,
			next="-3000_08",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Hey, just in case this pit isn't actually bottomless, do you think maybe you could unstrap one of your longfall boots and, you know... shove me into it? You just have to remember to land on one foot–
		SceneTable["-3000_08"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_longfall_speech10.vcd"),
			postdelay=0.0,
			next=null,
			char="glados",
			noDingOff=true,
			noDingOn=true,
			fires=
			[
				{entity="@shaft_potatos_ledge_start",input="Trigger",parameter="",delay=10,fireatstart=true},
				{entity="@shaft_crash_landing_start",input="Trigger",parameter="",delay=18,fireatstart=true}
			]
		}
	}

	//=================================================================
        //Called when player enters room Potatos is sitting in
	//=================================================================
	if (curMapName=="sp_a3_transition01")
	{
		//Glados: Oh. It's you. Go away.
		SceneTable["-3001_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup02.vcd"),
			postdelay=4.0,
			next="-3001_02",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}	
		//Glados: Come to gloat?
		SceneTable["-3001_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup03.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_03",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Go on. Get a goooood lonnnnng look.
		SceneTable["-3001_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup04.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_04",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Go on. Get a big fat eyeful. With your big fat eyes.
		SceneTable["-3001_04"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup05.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_05",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: That's right. A potato just called your eyes fat.
		SceneTable["-3001_05"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup06.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_06",
			char="glados",
			noDingOff=true,
			noDingOn=true,
			idlegroup="sp_a3_transition01_find_potatos_nag",
			idleorderingroup=4
		}
		//Glados: Now your fat eyes have seen everything.
		SceneTable["-3001_06"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup07.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_07",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: In case you were wondering: Yes. I'm still a potato. Go away.
		SceneTable["-3001_07"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup08.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_08",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Wait. Why DID you trundle over here? You're not HUNGRY, are you? It's hard to see. What do you have in your hand? Knowing you it's a deep fryer.
		SceneTable["-3001_08"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup10.vcd"),
			postdelay=[0.8,1.4],
			next="-3001_09",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
		//Glados: Stay back.
		SceneTable["-3001_09"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_meetup11.vcd"),
			postdelay=0.0,
			next=null,
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
	
		//=================================================================
			//Called when player picks up Potatos for the first time
		//=================================================================

		//Glados: What are you doing? Put me back this instant.
		SceneTable["-3002_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_postpickup05.vcd"),
			char="glados"
			postdelay=1.0,
			predelay = 0.3
			next = "-3002_02"
			noDingOff = true
			noDingOn = true
		}
		//Glados: I was getting SO lonely down here. It's good to finally hear someone else's voice. I'm kidding, of course. God, I hate you.
		SceneTable["-3002_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_transition_lonely01.vcd"),
			char="glados"
			postdelay=3.0,
			predelay = 0.0
			next = "-3002_03"
		}
		//Glados: I was so bored, I actually read the entire literary canon of the human race. Ugh. I hope YOU didn't write any of them.
		SceneTable["-3002_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_lonely02.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = null
		}
	}

	//=================================================================
        //Called when discovering 'Caroline' in portrait in office
	//=================================================================
	if (curMapName=="sp_a3_speed_ramp")
	{
		//Caroline ... Her name was Caroline. I remember her. Why do I remember her?
		SceneTable["-3003_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_familiarportrait01.vcd"),
			char="glados"
			postdelay=10.0,
			predelay = 0.3
			next = "-3003_02"
			noDingOff = true
			noDingOn = true
		}
		//Caroline...
		SceneTable["-3003_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_familiarportrait02.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = null
		}
	}

/*	
	
	//=================================================================
        //Called when discovering the Cave Johnson cube
	//=================================================================
	if (curMapName=="sp_a3_portal_intro")
	{
		//Cave: Greetings, friend. It's Cave Johnson, CEO of Aperture Science.
		SceneTable["-3004_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_intro01.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.0
			next = "-3004_02"
			noDingOff = true
			noDingOn = true
		}
		//Cave: Down here! [pause] On the floor.
		SceneTable["-3004_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_intro02.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.8
			next = "-3004_03"
			noDingOff = true
			noDingOn = true
		}
		//Cave: That's right! It's really me. My entire living consciousness, for all eternity, inside a machine.
		SceneTable["-3004_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_intro05.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.6
			next = "-3004_04"
			noDingOff = true
			noDingOn = true
		}

		//Cave: Alone. On a dirty floor. In an abandoned room. At the bottom of a pit.
		SceneTable["-3004_04"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_intro06.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 1.0
			next = "-3004_05"
			noDingOff = true
			noDingOn = true
		}

		//Cave: My life is torture, please kill me.
		SceneTable["-3004_05"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_intro09.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 1.1
			next = "-3004_06"
			noDingOff = true
			noDingOn = true
		}

		//Glados: We don't have time for this.
		SceneTable["-3004_06"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting02.vcd"),
			char="glados"
			postdelay= 0.0,
			predelay = 0.4
			next = "-3004_07"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Hold on. Is that you, Caroline?	
		SceneTable["-3004_07"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup01.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.0
			next = "-3004_08"
			noDingOff = true
			noDingOn = true
		}
		
		//Yes SIR, Mister Johnson! I'll have that report on your desk by four–thirty! [normal voice, horrified] What. In the hell. Was THAT.	
		SceneTable["-3004_08"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting03.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = "-3004_09"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: You were my assistant! The heart and soul of Aperture Science! You don't remember?
		SceneTable["-3004_09"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup02.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.1
			next = "-3004_10"
			noDingOff = true
			noDingOn = true
		}
		
		//Glados: No, Mister Johnson. I DON'T.
		SceneTable["-3004_10"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting04.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.1
			next = "-3004_11"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: See, the science boys invented me a machine to house my consciousness in. But that sounded DANGEROUS, so I volunteered you to go first.
		//		Like a food taster, except with your soul! [chuckling] Guess they must've found a use for you after all. Oh! Which reminds me. I do need you both to kill me.
		SceneTable["-3004_11"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup03.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.1
			next = "-3004_12"
			noDingOff = true
			noDingOn = true
			
			fires=
			[
				{entity="@allow_cave_pickup_rl",input="Trigger",parameter="",delay=0}
			]
		}

		//Cave: Come on, be a sport and kill me. All you gotta do is pick me up.
		SceneTable["-3004_12"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup11.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 2.0
			next = "-3004_13"
			noDingOff = true
			noDingOn = true
		}

		//Glados: Sure
		SceneTable["-3004_13"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting06.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.1
			next = "-3004_14"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Plug's in the back of me. Give me a good pull, it should pop right out.	
		SceneTable["-3004_14"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup12.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.2
			next = "-3004_15"
			noDingOff = true
			noDingOn = true
		}
	
		//Glados: Okay.
		SceneTable["-3004_15"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting05.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = "-3004_16"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Now, before you say no, I want you to remember that I've lived a full life. Also, if this helps seal the deal, livin' in a computer this long's made me crazy. That's right: I am insane.
		SceneTable["-3004_16"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup06.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.3
			next = "-3004_17"
			noDingOff = true
			noDingOn = true
		}
		
		//Glados: I said we'll do it.	
		SceneTable["-3004_17"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting07.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = "-3004_18"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Wait. I suppose tellin' you I'm not in my right mind could sway you to not unplugging me. Let me round back on the important parts: in a computer. Ceaseless torture. Monster in the eyes of god. So why don't you get on over here and unplug ol' Cave.
		SceneTable["-3004_18"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Meetup07.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.2
			next = "-3004_19"
			noDingOff = true
			noDingOn = true
		}
		
		//Glados: If you don't unplug him, I will.
		SceneTable["-3004_19"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting09.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		//=================================================================
			//Called when the player kills the Cave cube
		//=================================================================

		//Cave: Ho ho! I can feel myself shuttin' down. Man, this is excitin'.	
		SceneTable["-3005_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown01.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.2
			next = "-3005_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="GladosCharacterStopScene(\"glados\")",delay=0.0, fireatstart=true }
			]
		}
		
		//Glados: Maybe we can stand on him to climb up.	
		SceneTable["-3005_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting10.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.8
			next = "-3005_03"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Oh! Room's gettin' dark. That's a good sign.	
		SceneTable["-3005_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown02.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.0
			next = "-3005_04"
			noDingOff = true
			noDingOn = true
		}

		//Cave: I'm comin' for you, Caroline!	
		SceneTable["-3005_04"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown03.vcd"),
			char="cave_body"
			postdelay=0.0,
			predelay = 0.1
			next = "-3005_05"
			noDingOff = true
			noDingOn = true
		}

		//Glados: Mister Johnson? You need to shut up.
		SceneTable["-3005_05"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting11.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = "-3005_06"
			noDingOff = true
			noDingOn = true
		}

		//Cave: Ten–four!
		SceneTable["-3005_06"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown04.vcd"),
			char="cave_body"
			postdelay= 0.0,
			predelay = 0.2
			next = "-3005_07"
			noDingOff = true
			noDingOn = true
		}		
		
		//Cave: Here I go! The great beyond! Valhalla, home of Hercules! I can hear them winged chariots thunderin' over now!	
		SceneTable["-3005_07"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown07.vcd"),
			char="cave_body"
			postdelay= 0.0,
			predelay = 3.0
			next = null
			noDingOff = true
			noDingOn = true
		}
	
		//==================================================================
			//Called if player lingers after using the Cave corpse to escape
		//==================================================================

		//Glados: Goodbye, sir. May whatever tests await you on the other side either support or disprove your hypotheses.
		SceneTable["-3006_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting13.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.0
			next = "-3006_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="GladosCharacterStopScene(\"cave_body\")",delay=0.0, fireatstart=true }
			]
		}
	
		//Cave: Thank you, Caroline.
		SceneTable["-3006_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown05.vcd"),
			char="cave_cube"
			postdelay=0.0,
			predelay = 0.2
			next = "-3006_03"
			noDingOff = true
			noDingOn = true
		}
		
		//Cave: Alright! Too much jawin', not enough dyin'. Here I go! Ah.
		SceneTable["-3006_03"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/CaveFind_Shutdown06.vcd"),
			char="cave_cube"
			postdelay=0.0,
			predelay = 0.2
			next = "-3007_01"
		}
		
		//==================================================================
			//Called if player lingers after using the Cave corpse to escape
		//==================================================================
		
		//Glados: I'd... appreciate it... if we never... EVER talked  about that... ever again.	
		SceneTable["-3007_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_postcave01.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 4.0
			next = null
			noDingOff = true
			noDingOn = true
		}
	}

	//=================================================================
        //Called at the fake exit of transition01
	//=================================================================
	if (curMapName=="sp_a3_transition01")
	{
		//Cave Johnson here! Thanks again, from me and Aperture Science, fo taking part in these tests.
		SceneTable["-3008_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/cavejohnson/Cave_Exit02.vcd"),
			char="cave_body"
			postdelay=10.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}
	}
	//=================================================================
    //Called when Potatos enters the test chamber for the first time
	//=================================================================
	if (curMapName=="sp_a3_speed_ramp")
	{
		//Wait. This isn't one of MY tests. It must... PRE–DATE me. So that means... they were doing testing without me.
		SceneTable["-3009_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_undergroundtests02.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	//=================================================================
    //Called when player flings to exit platform
	//=================================================================
		//Well done. Of course, if I'D built that test area, you'd still be IN there. Because you'd be a corpse.
		SceneTable["-3010_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_painttests02.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	}
	//=================================================================
    //Called when player reaches cube platform
	//=================================================================
	if (curMapName=="sp_a3_speed_flings")
	{
		//I would NEVER have put that cube there. Or I WOULD have put it there, but then filled it with neurotoxin! [remembering half heartedly] Oh. Good work solving it, though.
		SceneTable["-3011_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_painttests03.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	//=================================================================
    //Called when player reaches exit platform
	//=================================================================
		//If I'd built that test, you would have never solved it. I'm not bragging. It's an objective fact. My tests are good and these are stupid garbage.
		SceneTable["-3012_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_painttests05.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	}
	//=================================================================
    //Called when player enters offices
	//=================================================================
	if (curMapName=="sp_a3_portal_intro")
	{
		//Maybe I didn't appreciate the subtleties of human literature the first time. Let me try again. [beep] No. It didn't get any better.
		SceneTable["-3013_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_lonely03.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	//=================================================================
    //Called when player exits offices
	//=================================================================
		//I just realized why I don't like human literature. Not enough omniscient AI characters administering neurotoxin. [beep] There, I rewrote them. They're all good now.
		SceneTable["-3014_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_lonely04.vcd"),
			char="glados"
			postdelay=1.6,
			predelay = 0.3
			next = "-3014_02"
			noDingOff = true
			noDingOn = true
		}	
		//I made you a character in Hamlet. You're the court jester that gets hit by a neurotoxin truck in Act One. All the other characters laugh. So you're famous now.
		SceneTable["-3014_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_lonely05.vcd"),
			char="glados"
			postdelay=0.8,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	//=================================================================
    //Called when white paint turns on
	//=================================================================
		//Wait. I HEARD about this. We discontinued it after all the test subjects kept escaping.
		SceneTable["-3015_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_whitepaint01.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	//=================================================================
    //Called when solved white paint puzzle
	//=================================================================
		//So...WE know about this. He doesn't.
		SceneTable["-3016_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_whitepaint02.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = "-3016_02"
			noDingOff = true
			noDingOn = true
		}	
		//Oh, I am good. [chuckling evilly] Sorry. It's hard to just turn that off.
		SceneTable["-3016_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_whitepaint03.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	}
	//=================================================================
    //Called when the lift lowers in the pump room
	//=================================================================
	if (curMapName=="sp_a3_end")
    {
		//You need to put me back in my body. I know this sounds like a trick. In fact, I can unconditionally guarantee you that at any other time it would be a trick. But if you don't let me stop the damage your friend is doing, this facility is going to	explode. [distant rumble] Soon.
		SceneTable["-3018_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_act4setup05.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			fires = 
			[
				{entity="rumble_relay",input="Trigger",parameter="",delay=16,fireatstart=true}
			]
			noDingOff = true
			noDingOn = true
		}	
		//=================================================================
		//Called when you ride the lift up out of the pump room
		//=================================================================
		//Let's make a deal. If you get me back in my body... I'll... let... you... go.
		SceneTable["-3019_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_act4setup06.vcd"),
			char="glados"
			postdelay=2.5,
			predelay = 0.3
			next = "-3019_02"
			noDingOff = true
			noDingOn = true
		}	
		//I want to make this clear: I'm not promising to stop testing humans. I'm just promising to stop testing on you. So long as you leave and never. Come. Back.
		SceneTable["-3019_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_act4setup08.vcd"),
			char="glados"
			postdelay=0.0,
			predelay = 0.3
			next = null
			noDingOff = true
			noDingOn = true
		}	
	}
	
*/	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function sp_a3_01_falling_potatos()
{
	GladosPlayVcd( -3000 )
}
function sp_a3_transition01_find_potatos()
{
	GladosPlayVcd( -3001 )
}
function sp_a3_transition01_pickup_potatos()
{
	GladosPlayVcd( -3002 )
}
function sp_a3_speed_ramp_caroline()
{
	GladosPlayVcd( -3003 )
}
/*
function sp_a3_portal_intro_find_cave()
{
	GladosPlayVcd( -3004 )
}
function sp_a3_portal_intro_kill_cave()
{
	GladosAllCharactersStopScene()
	GladosPlayVcd( -3005 )
}
function sp_a3_portal_intro_stand_on_cave_linger()
{
	GladosPlayVcd( -3006 )
}
function sp_a3_portal_intro_potatos_post_cave()
{
	// NOTE: this has been commented out and instead chained to the previous VCD that plays
	//GladosPlayVcd( -3007 )
}
function sp_a3_transition01_cave_exit_greeting()
{
	GladosPlayVcd( -3008 )
}
function sp_a3_speed_ramp_testing_without_me()
{
	GladosPlayVcd( -3009 )
}function sp_a3_speed_ramp_corpse()
{
	GladosPlayVcd( -3010 )
}
function sp_a3_speed_flings_cube()
{
	GladosPlayVcd( -3011 )
}
function sp_a3_speed_flings_exit()
{
	GladosPlayVcd( -3012 )
}
function sp_a3_portal_intro_office_literature()
{
	GladosPlayVcd( -3013 )
}
function sp_a3_portal_intro_office_exit()
{
	GladosPlayVcd( -3014 )
}
function sp_a3_portal_intro_white_paint()
{
	GladosPlayVcd( -3015 )
}
function sp_a3_portal_intro_we_know_about_white()
{
	GladosPlayVcd( -3016 )
}
function sp_a3_end_that_idiot_in_charge()
{
	GladosPlayVcd( -3017 )
}
function sp_a3_end_put_me_back_in_my_body()
{
	GladosPlayVcd( -3018 )
}
function sp_a3_end_make_a_deal()
{
	GladosPlayVcd( -3019 )
}
*/