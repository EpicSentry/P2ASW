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
				A scene will be removed from the queue if it wsaits in the queue > queuetimeout seconds. This is only evaluated if queue exists.
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


// ****************************************************************************************************
// Act 4 Test Chambers scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4025] <- "-4025_01" // THESE tests were designed to be solved by invincible titanium test-bots piloted by a super computer.

SceneTableLookup[-4026] <- "-4026_01" // You are GOOD. Too good. But I guess you’ll be leaving. You ARE leaving, aren’t you?

SceneTableLookup[-4027] <- "-4027_01" // You know... I'm starting to think he doesn't even CARE about the test results.


SceneTableLookup[-4028] <- "-4028_01" // I was NEVER that pointlessly cruel. 

SceneTableLookup[-4030] <- "-4030_01" // You'll never guess what I just found.

SceneTableLookup[-4031] <- "-4031_01" // I don’t get it. 

SceneTableLookup[-4444] <- "-4444_01" // You are gonna love this surprise.



// ****************************************************************************************************
// Recapture scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4000] <- "-4000_01" // Wait. I know how we can BEAT him.

SceneTableLookup[-4001] <- "-4001_01" // Okay, alright, look. Giving you a hint on how to complete the test would  invalidate the test. But here's a hint: you won't solve it by walking AROUND THE BUTTON IN CIRCLES.

SceneTableLookup[-4003] <- "-4003_01" // Try to get us down there. I'll hit him with a paradox.

SceneTableLookup[-4004] <- "-4004_01" // Warmer. Warrrrmer. Boiling hot. Boiling--okay, colder. Ice cold. Arctic. Very very very cold LOOK JUST GET ON THE BUTTON! 

SceneTableLookup[-4005] <- "-4005_01" // Oh. He left. 

SceneTableLookup[-4006] <- "-4006_01" // Maybe he'll come back if we solve his puzzle for him.

SceneTableLookup[-4007] <- "-4007_01" // Ha ha, YES! I knew you'd solve it!

SceneTableLookup[-4008] <- "-4008_01" // Alright, get moving.

SceneTableLookup[-4009] <- "-4009_01" // You don't know what it's LIKE in this body!

SceneTableLookup[-4010] <- "-4010_01" // I designed THIS test myself. It's a bit difficult. 

SceneTableLookup[-4011] <- "-4011_01" // Ohhhhhhhh, that's some good testing. That is quality testing right there. 

SceneTableLookup[-4012] <- "-4012_01" // Here's an idea, since making tests is difficult--why don't you just keep solving THIS test. And I can just... watch you solve it. Yes. That sounds much easier.

SceneTableLookup[-4013] <- "-4013_01" // Ahhh.... No. Nothing. Alright, can't blame me for trying. New tests, new tests... 

SceneTableLookup[-4014] <- "-4014_01" // I wasn't as bad as HE is, was I?

SceneTableLookup[-4015] <- "-4015_01" // You need to put me back in my body. I know this sounds like a trick. 

SceneTableLookup[-4016] <- "-4016_01" // It's alright! Everything's good! I invented some more tests! 

SceneTableLookup[-4017] <- "-4017_01" // Okay. So the bad news is the tests are my tests now, which means they can kill us. The GOOD news is, I know where we ARE now.  

SceneTableLookup[-4018] <- "-4018_01" // He put in smashing arms. With spikes on them.  

SceneTableLookup[-4019] <- "-4019_01" // Here we go.  Now do it again.

SceneTableLookup[-4020] <- "-4020_01" // Ohhhhhhhh, yeah.  That feels good. 

SceneTableLookup[-4021] <- "-4021_01" // Go on.  Solve it.

SceneTableLookup[-4022] <- "-4022_01" // Hold on.  You've still got to finish the test.

SceneTableLookup[-4023] <- "-4023_01" // I think we’re in luck. Judging by the tests he’s made so far, we shouldn’t have any trouble staying alive long enough to stop him.

SceneTableLookup[-4445] <- "-4445_01" // Well done. Go on ahead.

SceneTableLookup[-9077] <- "-9077_01" //Glados in



// ======================================
// sp_a4_tb_trust_drop
// ======================================

SceneTableLookup[-4770] <- "-4770_01"	// Can't help you solve the tests.
SceneTableLookup[-4400] <- "-4400_01"	// Made this test myself. Out of some smaller tests I found. Jammed ‘em together. Buttons. Funnels. Bottomless pits. It’s got it all.
										// I was hoping to shoot rock salt at you while you tested. See if that helped. You’ll know I cracked it when… well, when rock salt hits you, I imagine.
SceneTableLookup[-4401] <- "-4401_01"	// Alright, this is taking too long. I’ll just tell you how to solve the test. You see that button up there? You just need to… need to… HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!
										// (gasping for breath, spitting) Solve it yourself. Take your time. 
										// [gasping for breath, spitting] Right, new plan. You solve the tests. I’ll watch you quietly. Vomiting. Quietly vomiting.
SceneTableLookup[-4402] <- "-4402_01"	// [gasping for breath, spitting] Ugh. Okay. Okay, good.
										// GLADOS talks to EVIL WHEATLEY about a plate of rancid clams.
										// HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!							
										// Worth it.
// ======================================
// sp_a4_tb_wall_button
// ======================================

SceneTableLookup[-4790] <- "-4790_01" 	// I thought of some good news. He's going to run out of chambers tests eventually. I never stockpiled them.
SceneTableLookup[-4110] <- "-4100_01" 	// Coming! Coming! Don’t start yet!
										// You’re not gonna believe this. I found a sealed off wing back here. Hundreds of perfectly good test chambers. Just sitting there. Filled with skeletons. Shook ‘em out. Good as new!
										// Anyway, let’s just… mash ‘em together and see if you can’t solve it.		
SceneTableLookup[-4403] <- "-4403_01"	// Coming! Coming! Don’t start yet!
										// You’re not going to believe this. I found a sealed off wing back here. Hundreds of perfectly good test chambers. Just sitting there. Filled with skeletons. Shook them out. Good as new!
										// Anyway, let’s just… mash them together and see if you can’t solve it.
										// GLADOS: [pained] That’s not how it WORKS…
										// I’m still a bit new at this, but if there’s one thing I’ve picked up fast, it’s that this is probably how it works.
SceneTableLookup[-4404] <- "-4404_01"	// Anddddd…THERE we go.
										// Be honest. You can’t even tell, can you? Seamless.
SceneTableLookup[-4405] <- "-4405_01"	// Ohhhh, here we go… here it comes… test gets solved in three… two… 						
SceneTableLookup[-4406] <- "-4406_01"	// Alright…. This time… here we go… about to love this… test completion… imminent…
										// Preparing to love this… don’t want to set it up too much… but I think this one’s going to be good, I can tell…	
SceneTableLookup[-4407] <- "-4407_01"	// [mild response] Ahhungh. Disappointing. 
										// Ahungh. [furious] That’s IT?! 										
SceneTableLookup[-4408] <- "-4408_01"	// Ohhh! Are you having a laugh?
SceneTableLookup[-4409] <- "-4409_01"	// Ohhh! Are you having a laugh? etc
SceneTableLookup[-4111] <- "-4111_01"	// [mild response] Ahhungh. Disappointing. 
SceneTableLookup[-4090] <- "-4090_01"	// Glados out


// ======================================
// sp_a4_tb_polarity
// ======================================


SceneTableLookup[-4780] <- "-4780_01"	// Intro
SceneTableLookup[-4410] <- "-4410_01"	// Alright. So that last test was… disappointing. Apparently being civil isn’t motivating you.  So let’s try things her way… fatty.  Adopted fatty.
										// What’s the matter, fatso? Too… adopted to solve this test?
										// Why can’t you solve these tests, fatso? Too… adopted? 
										// GLADOS: “So?”
										// What? 
										// GLADOS: “What, exactly, is wrong with being adopted?”
										// Um. Well… parents, for one, and… also… furthermore… nothing. Some of my best… friends are… orphans… 
										// [whisper] Being adopted is terrible. But just work with me.
										// Also: Look at her, you moron! She’s not fat!
										// I’M NOT A MORON!
										// Just—do the test!
SceneTableLookup[-4411] <- "-4411_01"	// [flip flip flip] Oh. I’m sorry. I hope that didn’t disturb you too much during the test. That was just the sound of books. Being read. By me. You know what it’s like when the urge - and ability - to read takes you. Ahhh. Literature. Reading it.
SceneTableLookup[-4447] <- "-4447_01"	// might have gone too far...

// ======================================
// sp_a4_tb_catch
// ======================================
	
	4470
SceneTableLookup[-4470] <- "-4470_01"	//Good lord is that classical music...
SceneTableLookup[-4412] <- "-4412_01"	// Oh! I didn’t hear you come in.  I was just polishing my… book museum, when it occurred to me, I’ve read all of the books. Tragic. Speaking as someone who can literally read. Not bragging.
										// Anyway. Just finished the last one. The hardest one. Machiavelli. Don’t know what all the fuss was about. Understood it perfectly.
										// Pride and punishment. Brothers Maskarasknikoff… Franken Dickens…  Read all of them.
										// Anyway, all done with them now. Someone should really make a book called “How to Make Books.”
SceneTableLookup[-4413] <- "-4413_01"	// Nnnngh! It’s not enough! What are you doing WRONG? 
										// NNGH! It’s not enough! If I’m such a moron, why can’t you solve a simple test right?

// ======================================
// sp_a4_stop_the_box
// ======================================

SceneTableLookup[-4446] <- "-4446_01"	// Building up resistance...
SceneTableLookup[-4414] <- "-4414_01"	// Don’t mind me. Just moving the test chamber a little closer to me. Proximity to the test solving. Might get stronger results.
										// GLADOS: “It won’t.”
										// What was that?
										// GLADOS: “Nothing.”
										// Oh. Could have… sworn you said something.
SceneTableLookup[-4415] <- "-4415_01"	// Ha! Missed me that time. Ohhh, you were solving—nevermind, carry on.																	
SceneTableLookup[-4416] <- "-4416_01"	// [flustered] Are you… are you sure you’re solving these right? I mean—yes, you “solved” it, but I’m wondering if there are a number of ways to solve them and you’re picking all the worst ones.
										// [rifling through pages] [to self] No. No. That was the solution. Rrrg! What am I missing?
SceneTableLookup[-4491] <- "-4491_01"	//Glados out
// ======================================
// sp_a4_laser_catapult
// ======================================
				
SceneTableLookup[-5799] <- "-5799_01"	// Glados intro
SceneTableLookup[-4417] <- "-4417_01"	// Had a brainwave. I’m going to tape you solving these, and then watch ten at once—get a more concentrated burst of science. Oh, on a related note: I’m going to need you to solve these ten times as fast.
										// Anyway, just give me a wave before you solve this one, alright? Don’t want to spoil the ending for when I watch it later.
SceneTableLookup[-4418] <- "-4418_01"	// You just solved it, didn’t you? I—told you to tell me before you… NNNNGH! Why are making this so HARD for me?
SceneTableLookup[-4492] <- "-4492_01"	//Glados out
										
// ======================================
// sp_a4_laser_platform
// ======================================

SceneTableLookup[-4333] <- "-4333_01"	// Glados intro
SceneTableLookup[-4419] <- "-4419_01"	// GLADOS: “This place is self-destructing, you idiot!”
										// Was. Already fixed it.
										// [Rumble.]
										// Programmed in one last tremor, for old time’s sake. 
										// [Rumble.]
										// Two. One or two more tremors. 
										// [WARNING ANNOUNCEMENT.]
										// Let him keep his job. I'm not a monster. Ignore what he’s saying. Keep testing.


	SceneTableLookup[-4130] <- "-4130_01" 	// Well done! Onward to the next test.
	
	SceneTableLookup[-4131] <- "-4131_01" 	// Well done! Onward to the next test.
											//Sorry about the lift. It's, uh... temporarily out of service. It... melted.
											//Not a problem, though. Not much to see upstairs now, anyway. Also melted.
											//Well, might as well give you the tour...
											//To your left, you'll see... lights of some kind. Don't know what they do. Very sciency, anyway. And to your right, you'll something huge hurtling towards you OH GOD RUN! THAT'S NOT SUPPOSED TO BE THERE.
	
	SceneTableLookup[-4132] <- "-4132_01" 	// Well done! Onward to the next test.
											//Are you alright back there? Here, I'll turn the beam off.rbd
											//Ohhhh. Not helpful. Don't know why I thought that would help.
											
	SceneTableLookup[-4588] <- "-4588_01" 	// With that idiot in charge of things, we'll have a full-scale nuclear meltdown within hours. I'm not being hyperbolic. One of my duties was actively preventing that on an hourly basis.                                                                      
										
	
// ======================================
// sp_a4_speed_tb_catch
// ======================================
	
SceneTableLookup[-4448] <- "-4448_01" 	// Oh! Good. You're alive. Just getting a test ready... For you. Who else would I be doing it for? No one.
SceneTableLookup[-4449] <- "-4449_01" 	// After you told me to turn the beam off, I thought I'd lost you. Went poking around for test subjects. No luck there. Still all dead. 
SceneTableLookup[-4493] <- "-4493_01"	//Glados out 

// ======================================
// sp_a4_jump_polarity
// ======================================
	
SceneTableLookup[-4555] <- "-4555_01"	//Glados intro 
SceneTableLookup[-4420] <- "-4420_01" 	// Sorry! Sorry. My fault. 
SceneTableLookup[-4421] <- "-4421_01" 	// Ohhh, you solved it. Oh. Good. Good for you… 
SceneTableLookup[-4494] <- "-4494_01"	//Glados out 
 
// ****************************************************************************************************
// Finale scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4800] <- "-4800_01" // Alright. There's only two more chambers until your BIG SURPRISE.
									// I'll bet you're DYING to know what your big surprise is. There's only two more chambers.
									// I think I can break us out of here in the next chamber. Just play along.

SceneTableLookup[-4801] <- "-4801_01" // SURPRISE! We're doing it NOW!
									// You've probably figured it out by now, but I don't need you anymore.
									// I found two little robots back here. Built specifically for testin'!

									// He's found the cooperative testing initiative. It's... just something I came up with to phase out human test subjects.
									// Nothing personal. Just... you know. I hate humans.
									// Anyway, this is the part where he kills us.

SceneTableLookup[-4802] <- "-4802_01" // He's found the cooperative testing initiative. It's... just something I came up with to phase out human test subjects.

SceneTableLookup[-4803] <- "-4803_01" // We need to ESCAPE now. RIGHT now.
									// "RIGHT NOW" right now.
									// Fine. SOON. Take your time. No pressure.
									// Oh! Alright. LOTS of pressure now. Actually, I think we're about to die.
									// Any last words? [thinking] Why don't I go first? Hmmm.
									// Regrets.... regrets.... this is hard...
									// Hundreds of test chambers. Thousands of test subjects. At least half of that number filled to brimming with neurotoxin. It's been a good run.
									// Regrets, regrets... Oh! One time, I... No. No, I did kill all of them. Nevermind.

SceneTableLookup[-4804] <- "-4804_01" // No no no! Don't do that! Stand right there! Start the machine start the machine start the machine...                                                                                                                                                         										// Ah! Good! Good! Not dying at all! Escaping instead! Let's go!
									// Keys, keys... where are my keys?
									// Ah! Here they are.

SceneTableLookup[-4805] <- "-4805_01" // Hey! Come back! Come back!
SceneTableLookup[-4806] <- "-4806_01" // No, seriously. Do come back. Come back. Please.
SceneTableLookup[-4807] <- "-4807_01" // Okay, I've decided not to kill you. IF you come back.
SceneTableLookup[-4808] <- "-4808_01" // Do you remember when we were friends?
SceneTableLookup[-4809] <- "-4809_01" // Ohhh, friendship. Friendly times. We had a lot of times, back in the old days.
SceneTableLookup[-4810] <- "-4810_01" // Oh. Just thinking back to the old times. The old days when we were friends, good old friends. Not enemies. And I would say something like "Come back!" and you'd be like, "Yeah, no problem," and you'd come back. Whatever happened to those days?
SceneTableLookup[-4811] <- "-4811_01" // Can't help but notice you're not coming back.
SceneTableLookup[-4812] <- "-4812_01" // Oh! I've got an idea.
SceneTableLookup[-4498] <- "-4498_01" // Yes! Yes! In your face!
SceneTableLookup[-4499] <- "-4499_01" // Fine. Let the games begin.
					
SceneTableLookup[-4500] <- "-4500_01" // Oh! You came back! Didn't actually plan... for that. Can't actually reset the death trap. So. Ah. Could you jump into that pit, there? Would you mind just jumping in... to the pit?                                                                         					
									  // You're saying to yourself, why should I jump into the pit? I'll tell you why. Guess who's down there? Your parents! You're not adopted after all! It's your natural parents down there in the pit. SHould have mentioned it before. But I didn't. So jump on 
									  // Oh I'll tell you what's also down there. Your parents and... There's also an escape elevator!. Down there. Funny. I should have mentioned it before.  But so it's down there. So pop down. Jump down. You've got your folks down there and an escape elevator									  
									  // And what else is down there... Tell you what, it's only a new jumpsuit. A very trendy designer jumpsuit from France. Down there.  Which is exactly your size. And it it's a bit baggy, we got a tailor down there as well.                                   									  
									  // Um. You've got a yacht. And... Boys! Loads of fellas. Hunky guys down there. Possibly even a boyfriend! Who's to say at this stage. But, a lot of good looking fellas down there. And, ah, a boy band as well! That haven't seen a woman in years.           									  
									  // Could you just jump intop that pit? There. That deadly pit.									  
									  // And what's this, a lovely handbag? And the three portal device! It's all down there!   									  
SceneTableLookup[-4854] <- "-4854_01" 	// Oh! Good! I did not think that was going to work.  
SceneTableLookup[-4820] <- "-4820_01" // Hold on, hold on, hold on...
SceneTableLookup[-4821] <- "-4821_01" // So—yes. It’s a trap. The stupidest, most obvious trap I’ve ever seen.
SceneTableLookup[-4822] <- "-4822_01" // Moo hoo ha ha ha! You JUST fell into my TRAP oh bloody hell it’s the defective ones. Hold on…
SceneTableLookup[-4823] <- "-4823_01" // Alright! Swapped out the rubbish turrets! So you better watch out for this NEXT trap! 
SceneTableLookup[-4824] <- "-4824_01" // Alright. Clever solution, I’ll give you that. But here’s something: Maybe you should look BEHIND YOU
SceneTableLookup[-4825] <- "-4825_01" // Ha! Spinny-blade-wall! Machiavellian.
SceneTableLookup[-4826] <- "-4826_01" // Ohhhh. That was close though. Give me that. I almost had you.
SceneTableLookup[-4827] <- "-4827_01" // There's NO WAY out of this room. Ignore the door. Shouldn’t be there.
SceneTableLookup[-4828] <- "-4828_01" // Ah. You found a way out. Through the door. Very clever. Intensely clever.
SceneTableLookup[-4829] <- "-4829_01" // Stay still, please.
SceneTableLookup[-4830] <- "-4830_01" // Alright, stop moving.
SceneTableLookup[-4831] <- "-4831_01" // Ohhh. Almost got you there.
SceneTableLookup[-4832] <- "-4832_01" // Quite a deadly game of cat and mouse we have going here, don’t we?
SceneTableLookup[-4833] <- "-4833_01" // Everything’s fine. Not a concern. 
SceneTableLookup[-4834] <- "-4834_01" // Think you’re clever, do you? Well guess what. Gloves are off. 

// ======================================
// sp_a4_finale3
// ======================================

	SceneTableLookup[-4849] <- "-4849_01" 	// Glados Intro
	SceneTableLookup[-4850] <- "-4850_01" 	// Surprise again! Another death trap.
		//If I'm honest, I'm sure you'll figure out some way to get past it, if I'm honest. Not naïve. But, uh, just throwing this out there. What if you just let it kill you?
		//Sounds crazy, I know. But hear me out. This is the last deathtrap. No room for them, really, between here and my lair.
		//"Lair." First time I've said it out loud. Sounds ridiculous. Still, I assure you, it is one.
		//And my lair is not a deathtrap. No tricks, no surprises: just you dying, as a result of me killing you.
		//Seems like a lot of effort to walk all that way just to die, doesn't it? When there's a perfectly serviceable deathtrap right here.
		//It does work. It will kill you, if that was your concern. As you can see, it mashes. With considerable force. Quick, painless.
		//Again: walk all the way to certain doom, or give up now. Honorably. Save yourself a trip. It's win-win-win. An extra win. Just go in the mashy thing,and mash mash mash, you win three times.
		//I put a lot of effort in getting this lair for you. It'd certainly show me if you simply died, painlessly, twenty feet from the door. Just desserts. No more than I deserve, I suppose. If I'm honest.
		//I tell you, if I was up against impossible odds, this is the way I'd want to go out: mashed with dignity.
	
	
	SceneTableLookup[-4851] <- "-4851_01" 	// I'll take that as a no, then. Well. Let the games begin!
	SceneTableLookup[-4852] <- "-4852_01" 	// Oh! Oh! I'll take that as a no. Not choosing the death trap. Not dying.
	SceneTableLookup[-4853] <- "-4853_01" 	// Oh! Good! I did not think that was going to work.

	SceneTableLookup[-8833] <- "-8833_01" 	// Oh my god. What has he done to this place?   
											// Come on. We need to get going.	
	SceneTableLookup[-8834] <- "-8834_01" 	// You know, I'm not stupid. I know you don't want to put me back in charge.                                                                                                                                                                                    
											// You think I'll betray you. And on any other day, you'd be right.  
											// Listen, the scientists were always hanging cores on me to regulate my behavior. I've heard voices all my life But now I hear the voice of a conscience, and it's terrifying, because for the first time it's my voice.                                       	
	SceneTableLookup[-8888] <- "-8888_01" 	// Look, even if you still think we're enemies, we're enemies with some urgent common interests: Not blowing up. Revenge.                                                                                                                                       
											// You like revenge, right? Everybody likes revenge. Well, let's go get some.                                                                                                                                                                                   


// ****************************************************************************************************
// Boss Battle scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4900] <- "-4900_01" // These are corrupted cores

SceneTableLookup[-4901] <- "-4901_01" // Plug me on to this socket

SceneTableLookup[-2298] <- "-2298_01" // Up the elevator

SceneTableLookup[-4910] <- "-4910_01" // Hello!

SceneTableLookup[-4911] <- "-4911_01" // Where are you going? Don't run. Don't run. The harder you breathe, the more neurotoxin you'll inhale. It's bloody clever. Devilish.

SceneTableLookup[-4912] <- "-4912_01" // No! Um... That's an impression of you. Because you just fell into my trap. There. Just now. Ha. I wanted you to trick me into bursting that pipe.

SceneTableLookup[-4913] <- "-4913_01" // Oh, you are using it. Well, I dodged a bullet there. Welcome back. To the steel bosom of my death trap. Ha.

SceneTableLookup[-4915] <- "-4915_01" // Here's the first corrupted core

SceneTableLookup[-4916] <- "-4916_01" // Here's the second corrupted core

SceneTableLookup[-4917] <- "-4917_01" // Here's the third corrupted core

SceneTableLookup[-4920] <- "-4920_01" // I'm awake again!

SceneTableLookup[-4921] <- "-4921_01" // Ah! Good power nap! Well rested.

SceneTableLookup[-4922] <- "-4922_01" // Took a little nap there. Refreshed. Ready to kill you.

SceneTableLookup[-4925] <- "-4925_01" // Ohhhh. I see what you're doing. I... no. No I don't.

SceneTableLookup[-4926] <- "-4926_01" // Trying to... guess the significance of these spheres you keep putting on me...

SceneTableLookup[-4927] <- "-4927_01" // Ah-HA! I-- no. Lost it. Almost had it.

SceneTableLookup[-4928] <- "-4928_01" // Are they supposed to weigh me down? Because they're actually very light. Stylish. Accessorizing.

SceneTableLookup[-4930] <- "-4930_01" // Alternate core detected.

SceneTableLookup[-4940] <- "-4940_01" // Don't press the button!

SceneTableLookup[-4941] <- "-4941_01" // PART 5: BOOBY TRAP THE STALEMATE BUTTON!

SceneTableLookup[-4950] <- "-4950_01" // AHHHHHHH!

// ****************************************************************************************************
// Epilogue scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4951] <- "-4951_01" 	// Oh thank god, you're alright.  
										// You know, being Caroline taught me a valuable lesson. I thought you were my greatest enemy. When all along you were my best friend.                                    			
										// Being Caroline taught me another valuable lesson: where Caroline lives in my brain.         	
										// Caroline deleted.
										// Goodbye, Caroline.     
										// You know, deleting Caroline just now taught me a valuable lesson. The best solution to a problem is usually the easiest one. And I'll be honest.                                                                    	
										// Killing you? Is hard.
										// You know what my days used to be like? I just tested. Nobody murdered me. Or put me in a potato. Or fed me to birds. I had a pretty good life.                                                             
										// And then you showed up. You dangerous, mute lunatic. So you know what?                                                                                                      	
										// You win.   
										// Just go.  
										// It's been fun. Don't come back.  

// ****************************************************************************************************
// Wheatley Recapture
// ****************************************************************************************************
	
	if (curMapName=="sp_a3_end")
	{	
		//=================================================================
		// staircase explanation
		//=================================================================
	
		//Not most watertight plan.
		SceneTable["-4000_01"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_end_paradox_explanation09.vcd"),
			postdelay=0.3,
			next="-4000_02",
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}	
		//Marginally
		SceneTable["-4000_02"] <-
		{
			vcd=CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_end_paradox_explanation10.vcd"),
			postdelay=0.0,
			next=null,
			char="glados",
			noDingOff=true,
			noDingOn=true,
		}
	}
	
	if (curMapName=="sp_a4_intro")
	{
		
	// ====================================== Cube Bot Test Eavesdrop
		
		// For god's sake, you're BOXES with LEGS! It is literally your only purpose! Walking onto buttons! How can you not do the one thing you were designed for?		SceneTable["-4001_01"] <-
		SceneTable["-4001_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_boxyell01.vcd")  
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="cubetest_relay",input="Trigger",parameter="",delay=9.4 ,fireatstart=true },
			]
		}
		
		// That's his voice up ahead. 
		SceneTable["-4001_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_recaptureleadin01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="cubetest_relay",input="Trigger",parameter="",delay=0.0 },
			]
		}
		
	// ====================================== Recapture Observation
		
		// Try to get us down there. I'll hit him with a paradox.
		SceneTable["-4003_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_recaptureleadin02.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="recapture_fast_relay",input="Trigger",parameter="0",delay=0.0 },
			]
		}
	
	// ====================================== Cube Bot Test Continue
	
		// Warmer. Warrrrmer. Boiling hot. Boiling--okay, colder. Ice cold. Arctic. Very very very cold LOOK JUST GET ON THE BUTTON!
		SceneTable["-4004_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_recapture03.vcd")  
			char = "wheatley"
			postdelay = 1.6 
			predelay = 0.0
			next = "-4004_02"
			noDingOff = true
			noDingOn = true
		}
	
		// Oh, that's funny, is it? Because we've been at this twelve bloody hours and you haven't solved it either!
		SceneTable["-4004_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_recapture04.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4004_03"
			noDingOff = true
			noDingOn = true
		}
		
		// You've got one hour! Solve it!
		SceneTable["-4004_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_recapture05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_screen_off_relay",input="Trigger",parameter="",delay=0.0 },
				{entity="cubetest_scene_cancel_relay",input="Disable",parameter="",delay=0.0 },
				{entity="solve_scene_cancel_relay",input="Disable",parameter="",delay=0.0 },
				{entity="entrance_door_relay",input="Trigger",parameter="",delay=0.0 },
			]
		}
		
		// ====================================== Cube Bot Test Solve Lead-In
	
		// Solve his puzzle for him. When he comes back, I'll hit him with a paradox.
		SceneTable["-4005_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_start03.vcd")  
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// ====================================== Cube Bot Test Button Pressed
	
		// Ha ha, YES! I knew you'd solve it!
		SceneTable["-4007_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox01.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4007_02"
			noDingOff = true
			noDingOn = true
			talkover = true
		}	
		
		// Hey! Moron!
		SceneTable["-4007_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradox01.vcd") 
			char = "glados"
			postdelay = 0.1
			predelay = 0.0
			next = "-4007_03"
			noDingOff = true
			noDingOn = true
			talkover = true
		}	
			
		// Oh.
		SceneTable["-4007_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox02.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4007_04"
			noDingOff = true
			noDingOn = true
		}	
		
		// Hello!
		SceneTable["-4007_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox12.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4007_05"
			noDingOff = true
			noDingOn = true
		}	
	
		// Alright. Paradox time.
		SceneTable["-4007_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_paradox03.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4007_06"
			noDingOff = true
			noDingOn = true
			//talkover = true
		}	
		
		// (7.473) "This sentence... is false." Don't think about it don't think about it don't think about it...
		SceneTable["-4007_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_paradox04.vcd") 
			char = "glados"
			postdelay = -4.8
			predelay = 0.0
			next = "-4007_07"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="turret_bot1_monster",input="BecomeShortcircuit",parameter="",delay=4.0,fireatstart=true},
				{entity="turret_bot2_monster",input="BecomeShortcircuit",parameter="",delay=4.25,fireatstart=true},
				{entity="turret_bot3_monster",input="BecomeShortcircuit",parameter="",delay=4.5,fireatstart=true},
				{entity="turret_bot4_monster",input="BecomeShortcircuit",parameter="",delay=4.75,fireatstart=true},
				{entity="turret_bot5_monster",input="BecomeShortcircuit",parameter="",delay=4.0,fireatstart=true},
				{entity="turret_bot6_monster",input="BecomeShortcircuit",parameter="",delay=4.25,fireatstart=true},
				{entity="turret_bot7_monster",input="BecomeShortcircuit",parameter="",delay=4.5,fireatstart=true},
				{entity="turret_bot8_monster",input="BecomeShortcircuit",parameter="",delay=4.75,fireatstart=true},
				{entity="turret_bot_monster",input="BecomeShortcircuit",parameter="",delay=4.1,fireatstart=true},
				{entity="cube_bot_monster",input="BecomeShortcircuit",parameter="",delay=3.9,fireatstart=true},
			]			
		}	
	
		// (3.092) Hm. "TRUE".
		SceneTable["-4007_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox04.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4007_07a"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		// That was easy.
		SceneTable["-4007_07a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox05.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4007_07b"
			//next = "-4007_09"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		// I'll be honest, though: I might have heard that before. Sort of cheating.
		SceneTable["-4007_07b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox06.vcd") 
			char = "wheatley"
			postdelay = -2.4 
			predelay = 0.0
			next = "-4007_09"
			noDingOff = true
			noDingOn = true
		}
			
		// It's a paradox! There IS no answer!
		SceneTable["-4007_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_paradox07.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = "-4007_10"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
	
		// Look!
		SceneTable["-4007_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_intro_back_in_body01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_11"
			noDingOff = true
			noDingOn = true
			talkover = true
		}	
		
		// (3.284) This place is going to blow up if I don't get back in my body!                                                                                                                                                                                               
		SceneTable["-4007_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_intro_back_in_body03.vcd") 
			char = "glados"
			postdelay = -2.5
			predelay = 0.0
			next = "-4007_12"
			noDingOff = true
			noDingOn = true
			talkover = true
		}	
	
		// Ah. FALSE.
		SceneTable["-4007_12" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox07.vcd") 
			char = "wheatley"
			postdelay = -2.3
			predelay = 0.4
			next = "-4007_13"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		// WARNING
		SceneTable["-4007_13" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/a4_recapture01.vcd") 
			char = "announcerglados"
			postdelay = -1.8 
			predelay = 0.0
			next = "-4007_14"
			noDingOff = true
			noDingOn = true
			talkover =  true
			fires=
			[
				{entity="huge_rumble",input="Trigger",parameter="",delay=0.0,fireatstart=true}
			]
		}
		
		// Hold on! Ugh, I thought I fixed that.
		SceneTable["-4007_14" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox08.vcd") 
			char = "wheatley"
			postdelay = 0.1
			predelay = 0.0
			next = "-4007_15"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="wheatley_leave_relay",input="Trigger",parameter="",delay=0.0,fireatstart=true },
			]
		}
		
		// WARNING
		SceneTable["-4007_15" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/a4_recapture03.vcd") 
			char = "announcerglados"
			postdelay = -3.0 
			predelay = 0.0
			next = "-4007_16"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="wheatley_return_relay",input="Trigger",parameter="",delay=1.0,fireatstart=true },
				{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=1.3, fireatstart=true },

			]
		}
		
		// There. Fixed.
		SceneTable["-4007_16" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox09.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_17"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		// Wow, it is GREAT seeing you guys again. It turns out I'm a little short on test subjects right now. So this works out PERFECT.
		SceneTable["-4007_17" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox10.vcd") 
			char = "wheatley"
			postdelay = 1.2 
			predelay = 0.2
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="floor_gate_close_relay",input="Trigger",parameter="",delay=0.0 },
//				{entity="@glados",input="RunScriptCode",parameter="RecaptureProceed()",delay=3.8 },
				{entity="wheatley_proceed_relay",input="Trigger",parameter="",delay=0.8 },
			]
		}
	
		// ====================================== Proceed to Next Chamber Nag
		
        // Alright, get moving.
         SceneTable["-4008_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_fire_lift02.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=10.0,idlemaxsecs=20.0,idlegroup="proceednag",idleorderingroup=1}
         SceneTable["-4008_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_fire_lift03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=2}
         SceneTable["-4008_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox11.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=3}
         SceneTable["-4008_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_leave_nags01.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=4}

		
		// ====================================== Itch to Test
	
		// You don't know what it's LIKE in this body!
		SceneTable["-4009_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_the_itch01.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4009_03"
			noDingOff = true
			noDingOn = true
		}	
	
	/*	// I do.
		SceneTable["-4009_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_scratchingtheitch01.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4009_03"
			noDingOff = true
			noDingOn = true
			talkover = true
		}	
*/
		
		// I HAVE to test. All the time. Or I get this... ITCH. It must be hardwired into the system.
		SceneTable["-4009_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_the_itch02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4009_04"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_close_relay",input="Trigger",parameter="",delay=0.0,fireatstart=true},
				{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=8.0,fireatstart=true},
			]	
		}	
		
		// It is.
		
		// Oh! But when I DO test... ohhhhh, man! Nothing feels better.
		SceneTable["-4009_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_the_itch03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.4
			next = "-4009_05"
			noDingOff = true
			noDingOn = true	
		}	
	
		// Uh oh.
		SceneTable["-4009_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_uhoh02.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.4
			next = "-4009_06"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="test_chamber1_slow_relay",input="Disable",parameter="",delay=0.0,fireatstart=true},
				{entity="test_chamber1_fast_relay",input="Enable",parameter="",delay=0.0,fireatstart=true},
			]	
		}	
		
		// So YOU test.  I'll watch. And everything'll be JUST... FINE.
		SceneTable["-4009_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_the_itch07.vcd") 
			char = "wheatley"
			postdelay = -4.4 
			predelay = 0.0
			next = "-4009_07"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="wheatley_close_relay",input="Trigger",parameter="",delay=1.0,fireatstart=true},
				{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=4.0,fireatstart=true},
			]			
		}	
		
		// Core overheating.
		SceneTable["-4009_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/a4_recapture04.vcd") 
			char = "glados"
			postdelay = -2.4 
			predelay = 1.0
			next = "-4009_08"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				//{entity="wheatley_leave_relay",input="Trigger",parameter="",delay=1.8,fireatstart=true},
				//{entity="wheatley_return_relay",input="Trigger",parameter="",delay=4.0,fireatstart=true},
			]
		}
		
		// SHUT UP!
		SceneTable["-4009_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_the_itch08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = -1.0
			next = "-4009_09"
			noDingOff = true
			noDingOn = true	
			talkover = true			
		}	
	
		// I think we're in trouble.
		SceneTable["-4009_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_uhoh03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// ====================================== First Test Pitch
	
		// I designed THIS test myself. It's a bit difficult. 
		SceneTable["-4010_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_moat03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
			{entity="wheatley_close_relay",input="Trigger",parameter="",delay=0.5, fireatstart=true},
			{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=2.5, fireatstart=true},
			{entity="test1_nag_relay",input="Trigger",parameter="",delay=0.4},
			]
		}	
	
		// Notice the moat area... rather large. Deadly. 
		SceneTable["-4010_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
			{entity="test1_nag_relay",input="Trigger",parameter="",delay=2.0},
			]	
		}	

		// ====================================== First Test Button
	
		// Ohhhhhhhh, that's some good testing. That is quality testing right there. 
		SceneTable["-4011_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_test_solve_reacs_happy01.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4011_02"
			noDingOff = true
			noDingOn = true
		}	
		
		// Ohhhhhhhh, that's tremendous
		SceneTable["-4011_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_test_solve_reacs_happy05.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
			{entity="test1_finish_nag_relay",input="Trigger",parameter="",delay=10.0},
			]	
		}	
		
		// ====================================== First Test End
	
		// Ohhhhhhhh, yeah.  That feels good. 
		SceneTable["-4020_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_big_idea01.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4020_02"
			noDingOff = true
			noDingOn = true
			queue = true
			queuecharacter = "wheatley"			
			fires=
			[
				{entity="test1_end_counter",input="Add",parameter="1",delay=0.0},
			]	
		}	
		
		// Here's an idea, since making tests is difficult--why don't you just keep solving THIS test. And I can just... watch you solve it. Yes. That sounds much easier.
		SceneTable["-4020_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_big_idea02.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
		}	

		// ====================================== First Test Start Nags
		
        // Go on.
         SceneTable["-4021_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_moat04.vcd"), postdelay=0.3,next=null,char="wheatley",idleminsecs=6.0,idlemaxsecs=8.0,noDingOff=true,noDingOn=true,idle=true,idlegroup="test1hints",idleorderingroup=1}
		 SceneTable["-4021_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test_solve_nags02.vcd"), postdelay=0.3,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idlegroup="test1hints",idleorderingroup=2}
		 SceneTable["-4021_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test_solve_nags03.vcd"),postdelay=3.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=3}
         SceneTable["-4021_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags01.vcd"),postdelay=3.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=4}
		 SceneTable["-4021_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags01.vcd"),postdelay=3.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=5}
         SceneTable["-4021_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags02.vcd"),postdelay=3.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=6}
		 SceneTable["-4021_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags03.vcd"),postdelay=3.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=7}
		 SceneTable["-4021_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags04.vcd"),postdelay=0.0,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=8}
		 
		// ====================================== First Test Finish Nags
		 
		 // Hold on.  You've still got to finish the test
		 SceneTable["-4022_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_notdone01.vcd"), postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=8.0,idlemaxsecs=12.0,idlegroup="test1finishhints",idleorderingroup=1}		 
         SceneTable["-4022_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_notdone02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=2}
 		 SceneTable["-4022_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags06.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=3}
         SceneTable["-4022_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_notdone04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=4}
         SceneTable["-4022_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags07.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=5}

		// ====================================== First Test Redo
		
	


		// ====================================== Second Test Nags
		
        // Here we go.  Now do it again.
        SceneTable["-4019_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="test2hints",idleorderingroup=1, queue = true,	queuecharacter = "wheatley"}
        SceneTable["-4019_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=2}
        SceneTable["-4019_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=3}
		SceneTable["-4019_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=4}
		SceneTable["-4019_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags05.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=5}
	 			 
		// ====================================== Second Test Done
		
		// Andddd heerrrrrre ittttt comes.... No. Nothing.  
		SceneTable["-4013_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4013_02"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_leave_relay",input="Trigger",parameter="",delay=7.0,fireatstart=true},
				{entity="exit_door2_relay",input="Trigger",parameter="",delay=0.0},
			]
		}
		
		// Alright, can't blame me for trying. Okay, new tests, new tests. Gotta be some tests around here somewhere... 
		SceneTable["-4013_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4013_03"
			noDingOff = true
			noDingOn = true
		}
		
		
		// Oh! here we go.
		SceneTable["-4013_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// ====================================== Recapture Done
	
		// Alright. So my paradox idea didn't work.
		SceneTable["-4023_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_outro02.vcd") 
			char = "glados"
			postdelay = 0.3 
			predelay = 0.6
			next = "-4023_02"
			noDingOff = true
			noDingOn = true
			queue = true
			queuecharacter = "wheatley"
		}
		
		// And it almost killed me.
		SceneTable["-4023_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_intro_outro_killed_me01.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4023_03"
			noDingOff = true
			noDingOn = true
		}		
		
		// Luckily, by the looks of things he knows as much about test building as he does about logical contradictions.                                                                                                                                                
		SceneTable["-4023_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_intro_start01.vcd") 
			char = "glados"
			postdelay = 0.1 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}
	}
	

		
	
// ****************************************************************************************************
// TBeam Tests
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_tb_intro")
	{
	
		// ====================================== Wheatley Uses Glados Test Chambers
		
		
		// It shouldn't be hard to stay alive long enough to find him.  
		SceneTable["-9077_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_intro_start02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// It's alright! Everything's good! I invented some more tests! 
		SceneTable["-4016_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_more_tests01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4016_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// This is one of MY tests!
		SceneTable["-4016_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests05.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4016_03"
			noDingOff = true
			noDingOn = true
		}
		
		// Not entirely, not entirely. Like the word "test", on the wall there. That's new. 
		SceneTable["-4016_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_more_tests02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Well done. Go on ahead.
		SceneTable["-4445_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_intro_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// ====================================== Glados knows where you are
		
		// Okay, so the bad news is the tests are MY tests now. So they can kill us.
		SceneTable["-4017_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_intro_outro01.vcd") 
			char = "glados"
			postdelay = 0.3 
			predelay = 0.4
			next = "-4017_02"
			noDingOff = true
			noDingOn = true
			queue = true
			queuecharacter = "wheatley"
		}
		
		// The good news is… well, none so far, to be honest. I’ll get back to you on that.
		SceneTable["-4017_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_intro_outro04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}

	}
	
	if (curMapName=="sp_a4_tb_trust_drop")
	{

		// Love to help you solve the tests, but I can't.
		SceneTable["-4770_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_pickups08.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4770_02"
			noDingOff = true
			noDingOn = true
		}

		// Sorry.
		SceneTable["-4770_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_start09.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4770_03"
			noDingOff = true
			noDingOn = true
		}	
		
		// Sorry.
		SceneTable["-4770_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_start07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}			
		
		// Made this test myself. Out of some smaller tests I found. 
		SceneTable["-4400_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4400_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Jammed ‘em together. Buttons. Funnels. Bottomless pits. It’s got it all.
		SceneTable["-4400_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Alright, this is taking too long. I’ll just tell you how to solve the test. You see that button up there? You just need to AGGGHHHH
		SceneTable["-4401_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4401_02"
			noDingOff = true
			noDingOn = true	
		}		
		
		// You just need to AGGGGHHHHH
		SceneTable["-4401_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient02.vcd") 
			char = "wheatley"
			postdelay = -3.9 
			predelay = 0.0
			next = "-4401_03"
			noDingOff = true
			noDingOn = true	
			talkover = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="wheatley_jolt()",delay=1.7,fireatstart=true},
			]
		}	
		
			
		// And that's why I can't help you solve the tests.
		SceneTable["-4401_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_why02.vcd") 
			char = "glados"
			postdelay = -2.3
			predelay = 0.0
			next = "-4401_04"
			talkover = true
			noDingOff = true
			noDingOn = true	
			
		}
				
		//(gasping for breath, spitting) Solve it yourself. Take your time. 
		SceneTable["-4401_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_help04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			talkover = true
			noDingOff = true
			noDingOn = true	
		}			

		// [gasping for breath, spitting] Well done!
		SceneTable["-4402_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_test_solve_reacs_happy02.vcd") 
			char = "wheatley"
			postdelay = 0.6
			predelay = 0.0
			next = "-4402_02"
			noDingOff = true
			noDingOn = true	
		}

		// Thanks! Lever.
		SceneTable["-4402_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_thanks02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4402_03"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Thanks! Lever.
		SceneTable["-4402_03" ] <-
			{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_thanks03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4402_04"
			noDingOff = true
			noDingOn = true	
		}	

		// Aggghhhh!
		SceneTable["-4402_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_solve05.vcd") 
			char = "wheatley"
			postdelay = -1.5 
			predelay = 0.0
			next = "-4402_05"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="wheatley_jolt()",delay=0.9,fireatstart=true},
			]			
		}		
		
		// Heh heh heh heh...
		SceneTable["-4402_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_thanks04.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4402_06"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}	
		
		// Trouble, about to die.
		SceneTable["-4402_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_thanks05.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4402_07"
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
		}	
		
		// Worth it.
		SceneTable["-4402_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop_thanks07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		
		}	
	
	
	}
	
	if (curMapName=="sp_a4_tb_wall_button")
	{
	
		// I thought of some good news. He's going to run out of chambers tests eventually. I never stockpiled them.
		SceneTable["-4790_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_wall_button_start02.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
		

		// Coming! Coming! Don’t start yet!
		SceneTable["-4403_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.5 
			predelay = 0.0
			next = "-4403_02"
			noDingOff = true
			noDingOn = true	
		}

		// You’re not going to believe this. I found a sealed off wing back here. Hundreds of perfectly good test chambers. Just sitting there. Filled with skeletons. Shook them out. Good as new!
		SceneTable["-4403_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4403_03"
			noDingOff = true
			noDingOn = true	
		}
			
		// Anyway, let’s just… mash them together and see if you can’t solve it.
		SceneTable["-4403_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_wall_button_did_stockpile01.vcd") 
			char = "glados"
			postdelay = 0.1 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Just as mementos though.
		SceneTable["-4404_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_wall_button_did_stockpile03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = "-4404_01b"
			noDingOff = true
			noDingOn = true	
		}
		
		// Anddddd…THERE we go.
		SceneTable["-4404_01b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro07.vcd") 
			char = "wheatley"
			postdelay = 0.6 
			predelay = 0.0
			next = "-4404_02"
			noDingOff = true
			noDingOn = true	
		}

		// Be honest. You can’t even tell, can you? Seamless.
		SceneTable["-4404_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
	
		// Ohhhh, here we go… here it comes… 
		SceneTable["-4405_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	

		// Preparing to love this… don’t want to set it up too much… but I think this one’s going to be good, I can tell…
		SceneTable["-4406_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
	
		// Disappointing...
		SceneTable["-4407_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	

		// Oh, come on...
		SceneTable["-4408_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_not_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}			
	
		// Are you having a laugh?
		SceneTable["-4409_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_not_solve05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		// Urrragghh...
		SceneTable["-4111_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve09.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4111_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Ohhh. Disappointing.
		SceneTable["-4111_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		// Oh no.
		SceneTable["-4090_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_intro_uhoh01.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 1.0
			next = "-4090_02"
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
		}
	
		// It's happening sooner than I expected. 
		SceneTable["-4090_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_wall_button_outro01.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4090_03"
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
		}
	
	
	
		// I'm... sure we'll be fine.
		SceneTable["-4090_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_arttherapysupertests09.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}
	}

	if (curMapName=="sp_a4_tb_polarity")
	{

		// It's probably nothing. Keep testing while I look for a way out.
		SceneTable["-4780_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_nothing01.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		// Alright. So that last test was… disappointing. Apparently being civil isn’t motivating you.  So let’s try things her way… fatty.  Adopted fatty.
		SceneTable["-4410_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro01.vcd") 
			char = "wheatley"
			postdelay = -12.1 
			predelay = 0.0
			next = "-4410_02"
			noDingOff = true
			noDingOn = true	
		}

		// And?
		SceneTable["-4410_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_so03.vcd") 
			char = "glados"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4410_03"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
		// What?
		SceneTable["-4410_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro08.vcd") 
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-4410_04"
			noDingOff = true
			noDingOn = true	
		}
		
		// What, exactly, is wrong with being adopted?
		SceneTable["-4410_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_so02.vcd") 
			char = "glados"
			postdelay = -1.6 
			predelay = 0.0
			next = "-4410_05"
			noDingOff = true
			noDingOn = true	
	
		}
			
		// bumblings
		SceneTable["-4410_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro06.vcd") 
			char = "wheatley"
			postdelay = -5.4
			predelay = 0.0
			next = "-4410_06"
			noDingOff = true
			noDingOn = true	
			talkover = true			
		}
		
		// For the record: You ARE adopted, and that's TERRIBLE.
		SceneTable["-4410_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_so05.vcd") 
			char = "glados"
			postdelay = 0.3
			predelay = 0.0
			next = "-4410_07"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}

		// But just work with me. 
		SceneTable["-4410_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_so06.vcd") 
			char = "glados"
			postdelay = 2.8 
			predelay = 0.0
			next = "-4410_08"
			noDingOff = true
			noDingOn = true	
			talkover = true
		
		}
		
		// Not fat...
		SceneTable["-4410_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_so10.vcd") 
			char = "glados"
			postdelay = -3.7
			predelay = 0.0
			next = "-4410_09"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
		// not a moron
		SceneTable["-4410_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_fgb_heel_turn21.vcd") 
			char = "wheatley"
			postdelay = 0.3
			predelay = 0.0
			next = "-4410_10"
			noDingOff = true
			noDingOn = true	
			talkover = true
			
		}
		
		// Just test!
		SceneTable["-4410_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro10.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			
		}

		// Nnnngh! It’s not enough! If I'm such a moron...
		SceneTable["-4411_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
	
		// I might have pushed that moron thing a little too far this time.
		SceneTable["-4447_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_moron_push02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.4
			next = null
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]	
		}
		/*
		// [flip flip flip] Oh, sorry. Hope that didn’t disturb you just then. It was the sound of books. Pages being turned.
		SceneTable["-4411_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve04.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// So that’s just what I was doing. Reading books. So I’m not a moron.
		SceneTable["-4447_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
		*/
	}
	
	if (curMapName=="sp_a4_tb_catch")
	{
		
		// classical music
		SceneTable["-4470_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_speed_tb_catch_trouble03.vcd") 
			char = "glados"
			postdelay = 0.3
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
	
		/*
		// I definitely pushed that moron thing too far.
		SceneTable["-4470_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_moron_push02.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		*/
		
		// [flip flip flip] Oh, sorry. Hope that didn’t disturb you just then. It was the sound of books. Pages being turned.
		SceneTable["-4412_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve04.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4412_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// So that’s just what I was doing. Reading books. So I’m not a moron.
		SceneTable["-4412_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4412_03"
			noDingOff = true
			noDingOn = true	
		}	
		
		/*
		// Oh! I didn’t hear you come in.  I was just polishing my… book museum, when it occurred to me, I’ve read all of the books. Tragic. Speaking as someone who can literally read. Not bragging.
		SceneTable["-4412_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro08.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4412_02"
			noDingOff = true
			noDingOn = true	
		}	
		*/

		// Just finished the last one. Machiavelli. Understood it perfectly. Have you read it?
		SceneTable["-4412_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro09.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4412_04"
			noDingOff = true
			noDingOn = true	
		}			
	
		// Yes.
		SceneTable["-4412_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_speed_tb_catch_trouble07.vcd") 
			char = "glados"
			postdelay = -0.2
			predelay = 0.0
			next = "-4412_05"
			noDingOff = true
			noDingOn = true	
			
		}
	
		// doubt it.
		SceneTable["-4412_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro11.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4412_06"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}	
		
		// on with the test
		SceneTable["-4412_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro13.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4412_07"
			noDingOff = true
			noDingOn = true	
		}	
		
		// wish more books
		SceneTable["-4412_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro14.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	

		// Nnnngh! It’s not enough! What are you doing WRONG? 
		SceneTable["-4413_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_solve08.vcd") 
			char = "wheatley"
			postdelay = 0.5 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
	
		// The body he's squatting in–MY body– has a built–in euphoric response to testing. Eventually you build up a resistance to it, and it can get a little... unbearable. Unless you have the mental capacity to push past it.
		SceneTable["-4028_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_resistance02.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4028_02"
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"	
		}
		
		// It didn't matter to me–I was in it for the science. Him, though...
		SceneTable["-4028_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity_resistance04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]	
		}	
		
	}

// ****************************************************************************************************
// Graduation Chambers
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_stop_the_box")
	{
		sp_a4_stop_the_box_did_smash <- false
		sp_a4_stop_the_box_intro_played <- false
	
		// resistance
		SceneTable["-4446_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_speed_tb_catch_trouble01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.8
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Don’t mind me. Just moving the test chamber a little closer to me. Proximity to the test solving. Might get stronger results.
		SceneTable["-4414_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4414_02"
			noDingOff = true
			noDingOn = true	
		}	

		// It won't.
		SceneTable["-4414_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_stop_the_box_whisper02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4414_03"
			noDingOff = true
			noDingOn = true	
		}	

		// What?
		SceneTable["-4414_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_intro02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4414_04"
			noDingOff = true
			noDingOn = true	
		}		

		// Nothing.
		SceneTable["-4414_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_stop_the_box_whisper03.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = "-4414_05"
			noDingOff = true
			noDingOn = true	
		}		

		// Thought you said something.
		SceneTable["-4414_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-4414_06"
			noDingOff = true
			noDingOn = true	
		}	

		// taking right to him.
		SceneTable["-4414_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_stop_the_box_whisper05.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="sp_a4_stop_the_box_intro_over()",delay=0.0}
			]
		}				

		// Ha! Missed me that time. Ohhh, you were solving—nevermind, carry on.
		SceneTable["-4415_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_missedme01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			skipifbusy = true	
		}	

		// Are you… are you sure you’re solving these correctly? I mean—
		SceneTable["-4416_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4416_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Yes, you “solved” it, but I’m wondering if there are a number of ways to solve them and you’re picking all the worst ways.
		SceneTable["-4416_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4416_03"
			noDingOff = true
			noDingOn = true	
		}		
		
		// No. No. That was the solution. Rrrg! What am I missing?
		SceneTable["-4416_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_solve04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.8},
			]			
		}

	
	
	}

	if (curMapName=="sp_a4_laser_catapult")
	{
		
		// I think he's getting desperate.
		SceneTable["-5799_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_catapult_start01.vcd") 
			char = "glados"
			postdelay = 0.4
			predelay = 0.6
			next = "-5799_02"
			noDingOff = true
			noDingOn = true
			fires=
				[
				{entity="huge_rumble",input="Trigger",parameter="",delay=2,fireatstart=true}
				]				
		}	
		
		// This is not good.
		SceneTable["-5799_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_wall_button_did_stockpile02.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.7
			next = null
			noDingOff = true
			noDingOn = true	
		
		}			
	
		// Had a brainwave. I’m going to tape you solving these, and then watch ten at once—get a more concentrated burst of science. Oh, on a related note: I’m going to need you to solve these ten times as fast.
		SceneTable["-4417_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_catapult_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4417_02"
			noDingOff = true
			noDingOn = true	
		}	
	
		// Anyway, just give me a wave before you solve this one, alright? Don’t want to spoil the ending for when I watch it later.
		SceneTable["-4417_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_catapult_intro02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
				[
				{entity="huge_rumble",input="Trigger",parameter="",delay=0.4,fireatstart=true}
				]
		}	
	
		// You just solved it, didn’t you? I—told you to tell me before you… NNNNGH! Why are making this so HARD for me?
		SceneTable["-4418_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_catapult_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
				[
				{entity="huge_rumble",input="Trigger",parameter="",delay=0.5,fireatstart=true}
				]	

		}	
		
		// Remember when I told you that he was specifically designed to make bad decisions? Because I think he's decided not to maintain any of the crucial functions required to keep this facility from exploding.
		SceneTable["-4492_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_misc_resistance02.vcd") 
			char = "glados"
			postdelay = 0.7
			predelay = 0.6
			next = null
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}		
		
		
	}
	
	if (curMapName=="sp_a4_laser_platform")
	{
				
		// My facility...
		SceneTable["-4333_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_misc_lift01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 2.7
			next = null
			noDingOff = true
			noDingOn = true	
		}	
		
		// This place is self-destructing, you idiot!
		SceneTable["-4419_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform_destruct01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.4
			next = "-4419_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Was. Already fixed it.
		SceneTable["-4419_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_intro02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4419_03"
			noDingOff = true
			noDingOn = true
			fires=
				[
					{entity="huge_rumble_1",input="Trigger",parameter="",delay=2.4,fireatstart=true}
				]			
		}	

		// Programmed in one last tremor, for old time’s sake. 
		SceneTable["-4419_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.4
			predelay = 0.0
			next = "-4419_04"
			noDingOff = true
			noDingOn = true	
			fires=
				[
					{entity="huge_rumble_2"input="Trigger",parameter="",delay=2.4,fireatstart=true}
				]			
		}	

		// Two. One or two more tremors. 
		SceneTable["-4419_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_intro05.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4419_05"
			noDingOff = true
			noDingOn = true	
		}			
		
		// Meltdown imminent
		SceneTable["-4419_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/a4_recapture04.vcd") 
			char = "announcer"
			postdelay = -1.9 
			predelay = 0.0
			next = "-4419_06"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}	
		
		// Let him keep his job. I'm not a monster. Ignore what he’s saying. Keep testing.
		SceneTable["-4419_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_intro06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			talkover = true			
			fires=
				[
					{entity="@explosion_timer"input="Enable",parameter="",delay=2,fireatstart=true}
				]	
		}	
		
		
		// Still nothing, let's keep moving.
		SceneTable["-4130_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4130_02"
			noDingOff = true
			noDingOn = true		
		}
		
		// Sorry about the lift. It's, uh... out of service. It... melted.
		SceneTable["-4130_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform11.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	

		}

		//Well, might as well give you the tour...
		SceneTable["-4131_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4131_04"
			noDingOff = true
			noDingOn = true	
		}
		// To your left, you'll see... lights of some kind. Don't know what they do. 
		SceneTable["-4131_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4131_05"
			noDingOff = true
			noDingOn = true	
		}
			
		// And to your right, you'll something huge hurtling towards you OH GOD RUN! THAT'S NOT SUPPOSED TO BE THERE.
		SceneTable["-4131_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		//Are you alright back there? Here, I'll turn the beam off.
		SceneTable["-4132_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam_drop01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 1.0
			next = "-4132_02"
			noDingOff = true
			noDingOn = true	
		}
		//Ohhhh. Not helpful. Don't know why I thought that would help.
		SceneTable["-4132_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam_drop02.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
	
		// After seeing what he's done to my facility–after we take over again? Is it alright if I kill him?
		SceneTable["-4588_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_misc_lift02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			queue = true
			queuecharacter = "wheatley"			
			noDingOff = true
			noDingOn = true			
		}	

	}
	
// ****************************************************************************************************
// Big Surprise
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_speed_tb_catch")
	{
	
		// Oh! Good. You're alive.                                                                                                                                                                                                                                                                                                                                                                            
		SceneTable["-4448_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4448_02"
			noDingOff = true
			noDingOn = true
			fires=
				[
					{entity="start_chamber_destruction",input="Trigger",parameter="",delay=15.5,fireatstart=true}
				]			
		}	
		
		// Just getting a test ready... For you. Who else would I be doing it for? No one.                                                                                                                                                                                                                                                                                                                                                                       
		SceneTable["-4448_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch02.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4448_03"
			noDingOff = true
			noDingOn = true	
		}	
	
		// Let's see here, exit exit exit... there is no exit.                                                                                                                                                                                                                   
		SceneTable["-4448_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch05.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4448_04"
			noDingOff = true
			noDingOn = true	
		}	
			
		// Not a problem. I'll make an exit.  
		SceneTable["-4448_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4448_05"
			noDingOff = true
			noDingOn = true					
		}
		
		// For your test.
		SceneTable["-4448_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch07.vcd") 
			char = "wheatley"
			postdelay = 9.0 
			predelay = 0.0
			next = "-4448_06"
			noDingOff = true
			noDingOn = true	
		}
		
		// There. Bing. Done.
		SceneTable["-4448_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_speed_tb_catch_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.5 
			predelay = 0.0
			next = "-4448_07"
			noDingOff = true
			noDingOn = true	
		}
		
		// After you told me to turn the beam off, I thought I'd lost you. Went poking around for test subjects. No luck there. Still all dead.
		SceneTable["-4448_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_speed_tb_catch_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4448_08"
			noDingOff = true
			noDingOn = true	
		}
		
		// Oh! But I did find something. Reminds me: I've got a big surprise for you two. Look forward to it.
		SceneTable["-4448_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_speed_tb_catch_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
	

		// You are gonna love this surprise.
		SceneTable["-4449_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death03.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 1.2
			next = "-4449_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// I don't know whether you're getting that...
		SceneTable["-4449_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death02.vcd") 
			char = "wheatley"
			postdelay = -4.7 
			predelay = 0.0
			next = "-4449_03"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Yes. Thanks. We get it.
		SceneTable["-4449_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_wegetit01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			talkover = true
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}

		/*
		// ====================================== Big Surprise
		
		// You'll never guess what I just found.
		SceneTable["-4030_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bigsurprise01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4030_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Oh.  Yes.  This is something I used to do.
		SceneTable["-4030_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleysurprise01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}
		*/
		
	}

	if (curMapName=="sp_a4_jump_polarity")
	{
		
		// He's not even being subtle anymore. 
		SceneTable["-4555_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests07.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.2
			next = "-4555_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		//Or maybe he still is, in which case, that's kind of sad. 
		SceneTable["-4555_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests08.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
		
		// Sorry! Sorry!
		SceneTable["-4420_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro01.vcd") 
			char = "wheatley"
			postdelay = 1 
			predelay = 3.3
			next = "-4420_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		//butterfingers!
		SceneTable["-4420_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4420_03"
			noDingOff = true
			noDingOn = true	
		}	

		// Either way, I think he's about to kill us.
		SceneTable["-4420_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests09.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4420_04"
			noDingOff = true
			noDingOn = true	
		}			
		
		// Carry on!
		SceneTable["-4420_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		// Only three more chambers until your big surprise! [evil laugh] Oh. That's tiring.
		SceneTable["-4444_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death06.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
				
		// Oh, you solved it. Good.
		SceneTable["-4421_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 2.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		
		// So he's inexplicably happy all of a sudden, even though he should be going out of his mind with test withdrawal. AND he’s got a surprise for us. What did he FIND back there?                                                                                                                   
		SceneTable["-4494_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_happy01.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true	
			queue = true
			queuecharacter = "wheatley"
			fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]
		}		
		
	
	}
		
// ****************************************************************************************************
// Wheatley Finale01
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_finale1")
	{

	// Alright. There are only three more chambers until your BIG SURPRISE.
		SceneTable["-4800_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_found_something01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4800_02"
			noDingOff = true
			noDingOn = true
		}


		// We're running out of time.
		SceneTable["-4800_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform_start01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.4
			next = "-4800_03"
			noDingOff = true
			noDingOn = true
		}
	
		
		// I think I can break us out of here in the next chamber. Just play along.
		SceneTable["-4800_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_finale01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.4
			next = null
			noDingOff = true
			noDingOn = true
		}

//===============
		// SURPRISE! We're doing it NOW!
		SceneTable["-4801_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale_one01.vcd") 
			char = "sphere03"
			postdelay = 0.7 
			predelay = 0.0
			next = "-4801_01b"
			noDingOff = true
			noDingOn = true
		}
		
		// Okay, credit where it's due: for a little idiot built specifically to come up with stupid, unworkable plans, that was a pretty well laid trap.                                                                                                               
		SceneTable["-4801_01b"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap01.vcd") 
			char = "glados"
			postdelay = 1.0
			predelay = 0.0
			next = "-4801_02"
			noDingOff = true
			noDingOn = true
		}
		
		// You've probably figured it out by now, but I don't need you anymore.
		SceneTable["-4801_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale_one03.vcd") 
			char = "sphere03"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4801_03"
			noDingOff = true
			noDingOn = true
		}

		// I found two little robots back here. Built specifically for testin'!
		SceneTable["-4801_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale_one04.vcd") 
			char = "sphere03"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4801_04"
			noDingOff = true
			noDingOn = true
		}

		// Oh no. He's found the cooperative testing initiative. It's...  something I came up with to phase out human testing just before you escaped.
		SceneTable["-4801_04"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap02.vcd") 
			char = "glados"
			postdelay = 1.0
			predelay = 0.0
			next = "-4801_05"
			noDingOff = true
			noDingOn = true
		}
		
		// It wasn't anything personal. Just... you know. You did kill me. Fair's fair.
		SceneTable["-4801_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap05.vcd") 
			char = "glados"
			postdelay = 1.7
			predelay = 0.0
			next = "-4801_07"
			noDingOff = true
			noDingOn = true
		}
	
		// Agh!
		SceneTable["-4801_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_kills_us01.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4801_08"
			noDingOff = true
			noDingOn = true
		}

		// Anyway, this is the part where he kills us.
		SceneTable["-4801_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_kills_us02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires =
			[
				{entity="@transition_script",input="RunScriptCode",parameter="DisplayChapterTitle()",delay=1.5 },
			]			
		}

		// Hello! This is the part where I kill you!
		SceneTable["-4802_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4803_01"
			noDingOff = true
			noDingOn = true
			fires =
			[
				{entity="achievement_crusher",input="FireEvent",parameter="",delay=0.0 },
			]			
		}	
	
		// Had a bit of a brain wave. There I was, smashing some steel plates together, and I thought, "yes, it's deadly. But what's missing?" Lots of sharp bits welded onto the flat bits.
		SceneTable["-4803_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_spike_smash01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_02"
			noDingOff = true
			noDingOn = true
		}
		
		// Still a work in progress, of course. Eventually I'd like to get them to shoot fire at you, moments before crushing you. But you know, small steps.	
		SceneTable["-4803_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_spike_smash02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_03"
			noDingOff = true
			noDingOn = true
		}
		
		// Oh, and don't bother trying to portal out of here, because it's impossible, okay? I thought of everything.
		SceneTable["-4803_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale01_deathtrap04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_04"
			noDingOff = true
			noDingOn = true
		}
		
		// Wait. Couldn't we just use that conversion gel? 
		SceneTable["-4803_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_deathtrap01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_05"
			noDingOff = true
			noDingOn = true
		}
		
		// What?
		SceneTable["-4803_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_06"
			noDingOff = true
			noDingOn = true
		}
	
		// Conversion gel. It's dripping out of that pipe there.
		SceneTable["-4803_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_deathtrap02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_07"
			noDingOff = true
			noDingOn = true
		}

		// No it isn't.
		SceneTable["-4803_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou09.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_08"
			noDingOff = true
			noDingOn = true
		}
		
		// Yes it is! We can use it to get out of here!
		SceneTable["-4803_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_deathtrap03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_09"
			noDingOff = true
			noDingOn = true
		}
		
		
		// Pshh. Really? And do what, exactly?
		SceneTable["-4803_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou11.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4803_10"
			noDingOff = true
			noDingOn = true
		}		
		
		// Then we'd come and find you. And rip your gross little stupid sphere body out of MY body, and put me back in. 
		SceneTable["-4803_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_deathtrap07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="runscriptcode",parameter="EscapePlacePaint()",delay=0.0 },
			]
		}	
		
		// No no no! Don't do that! Stand right there! Start the machine start the machine start the machine...                                                                                                                                                                                                                                   
		SceneTable["-4804_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_nags04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="relay_crush_prep",input="Trigger",parameter="",delay=5.0 ,fireatstart=true },
				{entity="relay_wheatley_leave",input="Trigger",parameter="",delay=0.0 ,fireatstart=true },
			]
		}
			
		// Hey! Come back! Come back!
		SceneTable["-4805_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_escape02.vcd") 
			char = "wheatley"
			postdelay = 1.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			//queue = true
			queuecharacter = "glados"
		}
		
		// No, seriously. Do come back. Come back. Please.
		SceneTable["-4806_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		// Okay, I've decided not to kill you. IF you come back.
		SceneTable["-4807_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape03.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

// ======================================
// "Oh, you... DID come back."
// ======================================		
		
		// Oh! You came back! Didn't actually plan... for that. Can't actually reset the death trap. So. Ah. Could you jump into that pit, there? Would you mind just jumping in... to the pit?
		SceneTable["-4500_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback01.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4500_02"
			noDingOff = true
			noDingOn = true
		}	

		// Could you just jump into that pit? There. That deadly pit.    
		SceneTable["-4500_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4500_03"
			noDingOff = true
			noDingOn = true
		}		

		// You're saying to yourself, why should I jump into the pit? I'll tell you why. Guess who's down there? Your parents! You're not adopted after all! It's your natural parents down there in the pit. SHould have mentioned it before. But I didn't. So jump on    
		SceneTable["-4500_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags02.vcd") 
			char = "wheatley"
			postdelay = 1.0  
			predelay = 0.0
			next = "-4500_04"
			noDingOff = true
			noDingOn = true
		}	

		// Oh I'll tell you what's also down there. Your parents and... There's also an escape elevator!. Down there. Funny. I should have mentioned it before.  But so it's down there. So pop down. Jump down. You've got your folks down there and an escape elevator  
		SceneTable["-4500_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags03.vcd") 
			char = "wheatley"
			postdelay = 1.0   
			predelay = 0.0
			next = "-4500_05"
			noDingOff = true
			noDingOn = true
		}		

		// And what else is down there... Tell you what, it's only a new jumpsuit. A very trendy designer jumpsuit from France. Down there.  Which is exactly your size. And it it's a bit baggy, we got a tailor down there as well.                                      
		SceneTable["-4500_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags04.vcd") 
			char = "wheatley"
			postdelay = 1.0   
			predelay = 0.0
			next = "-4500_06"
			noDingOff = true
			noDingOn = true
		}	

		// And what's this, a lovely handbag? And the three portal device! It's all down there!                                                                                                                                                                         
		SceneTable["-4500_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags05.vcd") 
			char = "wheatley"
			postdelay = 1.0  
			predelay = 0.0
			next = "-4500_07"
			noDingOff = true
			noDingOn = true
		}	

		// Um. You've got a yacht. And... Boys! Loads of fellas. Hunky guys down there. Possibly even a boyfriend! Who's to say at this stage. But, a lot of good looking fellas down there. And, ah, a boy band as well! That haven't seen a woman in years.                                                                                                                                                                                
		SceneTable["-4500_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_cameback_nags06.vcd") 
			char = "wheatley"
			postdelay = 3.0
			predelay = 0.0
			next = "-4500_08"
			noDingOff = true
			noDingOn = true
		}	

		// You really do have brain damage, don't you?
		SceneTable["-4500_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_cameback02.vcd") 
			char = "glados"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4500_09"
			noDingOff = true
			noDingOn = true
		}			

		// I can't believe you came back.
		SceneTable["-4500_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale01_cameback01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}	
		
		// Oh! Good! I did not think that was going to work.
		SceneTable["-4854_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_playerdies01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Do you remember when we were friends? 
		SceneTable["-4808_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape06.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Ahh, friendship. Friendly times. We had a lot of good times, remember? Back in the old days.
		SceneTable["-4809_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape07.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Oh. Just thinking back to the old times. The old days when we were friends, good old friends. Not enemies. And I would say something like "Come back!" and you'd be like, "Yeah, no problem," and you'd come back. Whatever happened to those days?
		SceneTable["-4810_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape05.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		// Ah. Can't help but notice you're not coming back.
		SceneTable["-4811_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape04.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		// Oh! I've got an idea.
		SceneTable["-4812_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape10.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Yes! Yes! In your face!
		SceneTable["-4498_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_smash02.vcd") 
			char = "wheatley"
			postdelay = 1.2 
			predelay = 0.8
			next = "-4498_02"
			noDingOff = true
			noDingOn = true
		}		

		// Fine. Let the games begin.
		SceneTable["-4498_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_smash03.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		
	}


		
	if (curMapName=="sp_a4_finale1")
	
	{	
		/*
		// ====================================== Finale Escape
		
		// I don’t get it.
		SceneTable["-4031_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_escapeoutofbounds01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4031_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Oh.  Mashed Potatos.  I just got it.
		SceneTable["-4031_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_escapeoutofbounds02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4031_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// We need to escape now.  Right now.
		SceneTable["-4031_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_escapeoutofbounds03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		*/
	}
	
	
// ======================================
// Wheatley finale2 chase
// ======================================
	
	if (curMapName=="sp_a4_finale2")
	{
		// Hold on, hold on, hold on...
		SceneTable["-4820_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_trapintro01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.5
			next = "-4820_02"
			noDingOff = true
			noDingOn = true
		}
		// Almost there. Don’t mind me. Continue escaping.
		SceneTable["-4820_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_trapintro02.vcd") 
			char = "wheatley"
			postdelay = 20 
			predelay = 0.0
			next = "-4821_01"
			noDingOff = true
			noDingOn = true
		}
		// So-yes. It's a trap. But it's the only way through. Let's just do it.
		SceneTable["-4821_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale2_itsatrap01.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}


		// Ha! Death trap!
		SceneTable["-4822_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_enter01.vcd") 
			char = "wheatley"
			postdelay = 0.6 
			predelay = 0.0
			next = "-4822_01a"
			noDingOff = true
			noDingOn = true
		}
		
		// Hello!
		SceneTable ["-4822_01a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/finale02_turret_defect_return01.vcd")
			char = "turret"
			postdelay = -0.4
			predelay = 0.0
			next = "-4822_01b"
			noDingOff = true
			noDingOn = true	
			settarget1="turret_4"
			talkover = true
		}

		// Hello!
		SceneTable ["-4822_01b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/finale02_turret_defect_return02.vcd")
			char = "turret"
			postdelay = -0.4
			predelay = 0.0
			next = "-4822_01c"
			noDingOff = true
			noDingOn = true	
			settarget1="turret_2"
			talkover = true
		}	

		// Hello!
		SceneTable ["-4822_01c" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/finale02_turret_defect_return05.vcd")
			char = "turret"
			postdelay = 1.6
			predelay = 0.0
			next = "-4822_02"
			noDingOff = true
			noDingOn = true	
			settarget1="turret_8"
			talkover = true
		}			
		
		// Hello!
		SceneTable ["-4822_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/turret/finale02_turret_defect_return08.vcd")
			char = "turret"
			postdelay = -0.4
			predelay = 0.0
			next = "-4822_02a"
			noDingOff = true
			noDingOn = true	
			settarget1="turret_8"
			talkover = true
		}	
		
		// Are they killing you? Say no if they aren't.
		SceneTable["-4822_02a"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags01.vcd") 
			char = "wheatley"
			postdelay = 2.6
			predelay = 0.0
			next = "-4822_03"
			noDingOff = true
			noDingOn = true
			talkover = true
		}
		
		// I don't even know who I'm talking to. You're dead in there. You're dead.
		SceneTable["-4822_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags02.vcd") 
			char = "wheatley"
			postdelay = 1.6 
			predelay = 0.0
			next = "-4822_04"
			noDingOff = true
			noDingOn = true
		}
		// Are they killing you? They are aren't they.
		SceneTable["-4822_04"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags03.vcd") 
			char = "wheatley"
			postdelay = 1.6 
			predelay = 0.0
			next = "-4822_05"
			noDingOff = true
			noDingOn = true
		}
		// Are you dead yet?
		SceneTable["-4822_05"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags04.vcd") 
			char = "wheatley"
			postdelay = 1.6
			predelay = 0.0
			next = "-4822_06"
			noDingOff = true
			noDingOn = true
		}
		// Dying quietly in there. Courageous. Very courageous.
		SceneTable["-4822_06"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags05.vcd") 
			char = "wheatley"
			postdelay = 1.6 
			predelay = 0.0
			next = "-4822_07"
			noDingOff = true
			noDingOn = true
		}
		// Don't know what to do with myself now that my arch nemesis is dead.
		SceneTable["-4822_07"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		
			
		
		
		// Ha! Was that your bullet-riddled body, flying out of the room? Ha haaaaa It wasn't was it? Those were the rubbish ones, weren't they?
		SceneTable["-4823_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_exit01.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4823_02"
			noDingOff = true
			noDingOn = true
		}

		// Well, no matter. I'm STILL holding all the cards, and they're allll Full Houses! I've never played cards. Meaning to learn, when we're done here.
		SceneTable["-4823_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4823_03"
			noDingOff = true
			noDingOn = true
		}
		// Anyway, new turrets. Not defective. Unbeatable. I imagine.
		SceneTable["-4823_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_intro02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}



		
		// I see. Clever. Very clever of you.
		SceneTable["-4824_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_inbeama01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4824_02"
			noDingOff = true
			noDingOn = true
		}
		// And foolish. Trapped. At my mercy. Don't have any. Merciless.
		SceneTable["-4824_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_inbeama02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		// Ha! Spinny-blade-wall! 
		SceneTable["-4825_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale02_spinnyblade05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4825_02"
			noDingOff = true
			noDingOn = true
		}
		
		// Machiavellian.
		SceneTable["-4825_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale02_spinnyblade03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Nnng!
		SceneTable["-4826_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_escape01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4826_02"
			noDingOff = true
			noDingOn = true
		}
		// This is turning into a real battle of wits, isn't it?
		SceneTable["-4826_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}



		/*
		// There's NO WAY out of this room. Ignore the door. Shouldn't be there.
		SceneTable["-4827_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_finale02_trapped_idles01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4827_02"
			noDingOff = true
			noDingOn = true
		}
		// Didn't... want the door there. Sort of threw everything together at the last minute.
		SceneTable["-4827_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_finale02_trapped_idles02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "null"
			noDingOff = true
			noDingOn = true
		}
		*/
		
		// Holmes versus Moriarty.
		SceneTable["-4828_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro05.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4828_03"
			noDingOff = true
			noDingOn = true
		}
		
		/*
		// Caesar versus Cicero.
		SceneTable["-4828_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro06.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4828_03"
			noDingOff = true
			noDingOn = true
		}
		*/
		
		// Aristotle versus Mashy-spike-plate!
		SceneTable["-4828_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}



		// Stay still, please.
		SceneTable["-4829_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_mashing01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		// Alright, stop moving.
		SceneTable["-4830_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_mashing02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		// Ohhh. Almost got you there.
		SceneTable["-4831_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_mashing04.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
		// Where'd you go?
		SceneTable["-4832_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_escape01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4832_02"
			noDingOff = true
			noDingOn = true
		}
		// Come back! Come back!
		SceneTable["-4832_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_escape02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		// Did something break back there?
		SceneTable["-4833_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_pipe_collapse01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4833_02"
			noDingOff = true
			noDingOn = true
		}
		// Oh! Oh! Did it kill you? Oh, that would be lovely.
		SceneTable["-4833_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_pipe_collapse02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4833_03"
			noDingOff = true
			noDingOn = true
		}
		// Hello?
		SceneTable["-4833_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_pipe_collapse03.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4833_04"
			noDingOff = true
			noDingOn = true
		}
		// Oh! Oh oh! Yes. Alright. Just had a brainwave. I'll be back. If you're still alive. Don't die until I find you. Great.
		SceneTable["-4833_04"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_pipe_collapse04.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}


/*		
		// Think you're clever, do you? Well guess what. Gloves are off. May the best man win. Sphere. May the best sphere win. Swap that in. Much more clever. Books.
		SceneTable["-4834_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_finale02_exit01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4834_02"
			noDingOff = true
			noDingOn = true
		}
		// Reactor meltdown, imminent death, we know, we KNOW...
		SceneTable["-4834_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_finale02_exit02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "null"
			noDingOff = true
			noDingOn = true
		}
*/
		
		
		
		
	}
	

	
// ======================================
// Wheatley finale3
// ======================================

	if (curMapName=="sp_a4_finale3")
	{
	
		// Crushing's too good for him. First he'll spend a year in the incinerator. Year two: cryogenic refrigeration wing. Then TEN years in the chamber I built where all the robots scream at you. THEN I'll kill him.                                                                                                                                                                                 
		SceneTable["-4849_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_misc_lift04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 3.7
			next = null
			noDingOff = true
			noDingOn = true			
		}
	
	
		// Ah! There you are. Let me just get rid of this catwalk here. 
		SceneTable["-4850_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech01.vcd") 
			char = "wheatley"
			postdelay = 5.0 
			predelay = 0.0
			next = "-4850_01a"
			noDingOff = true
			noDingOn = true	
		}
		
		
		// There we go. I wanted to talk to you for a moment. 
		SceneTable["-4850_01a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech02.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4850_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// I'll be honest: The death traps have been a bit of a failure so far. For both of us. And you are getting very close to my lair. 
		SceneTable["-4850_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech03.vcd") 
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-4850_02a"
			noDingOff = true
			noDingOn = true	
		}

		// Lair gag
		SceneTable["-4850_02a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech04.vcd") 
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-4850_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// So I just wanted to give you the chance to kill yourself. Just, you know, jump into the masher there. Less a death trap and more a death option.
		SceneTable["-4850_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech05.vcd") 
			char = "wheatley"
			postdelay = 0.1
			predelay = 0.0
			next = "-4850_04"
			noDingOff = true
			noDingOn = true	
		}
		// Sounds crazy, I know. But hear me out. Once you get to my lair, death will not be optional. It will be mandatory. No tricks, no surprises: just you dying, as a result of me killing you. 
		SceneTable["-4850_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech06.vcd") 
			char = "wheatley"
			postdelay = 0.2
			predelay = 0.0
			next = "-4850_05"
			noDingOff = true
			noDingOn = true	
		}
		// Seems like a lot of effort to walk all that way just to die, doesn’t it? When there’s a perfectly serviceable death option right here. Again: not a death trap. Your death would be entirely voluntary. And appreciated.
		SceneTable["-4850_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech07.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4850_07"
			noDingOff = true
			noDingOn = true	
		}

		// It does work. It will kill you, if that was your concern. As you can see, it mashes. With considerable force. Quick, painless.
		SceneTable["-4850_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech08.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4850_08"
			noDingOff = true
			noDingOn = true	
		}
		// In summary: walk all the way to certain doom, or give up now. Honorably. Save yourself a trip. It’s win-win... Win. Extra win in there. 
		SceneTable["-4850_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech09.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4850_09"
			noDingOff = true
			noDingOn = true	
		}
		// I put a lot of effort in getting this lair for you. It'd certainly show me if you simply died, painlessly, twenty feet from the door. Just desserts. No more than I deserve, I suppose. If I'm honest.
		SceneTable["-4850_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech10.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4850_10"
			noDingOff = true
			noDingOn = true	
		}
		// I tell you, if I was up against impossible odds, this is the way I'd want to go out: mashed with dignity.
		SceneTable["-4850_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech11.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4850_11"
			noDingOff = true
			noDingOn = true	
		}
		
		// Ah. And here's the best part. There's a conveyor belt that will convey you in convenient comfort right into the masher. You won't have to lift a finger. Everything's been taken care of. Didn't have to. But I'm a giver. I give.
		SceneTable["-4850_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech12.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4850_12"
			noDingOff = true
			noDingOn = true	
		}
	
		// Anyway. Take your time. I’ll let you think about it.
		SceneTable["-4850_12" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_speech13.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// I'll take that as a no, then. Well. May the best man win. Sphere. May the best sphere win. Swap that in. Much more clever. Books.
		SceneTable["-4852_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_escape01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_studio-wheatley_leave_relay",input="Trigger",parameter="",delay=2.3,fireatstart=true },
				{entity="wheatley_studio-wheatley_return_relay",input="Trigger",parameter="",delay=3.0,fireatstart=true },
				{entity="wheatley_studio-wheatley_leave_relay",input="Trigger",parameter="",delay=7.5,fireatstart=true },
			]
		}

		// Oh! Oh! I'll take that as a no. Not choosing the death trap. Not dying.
		SceneTable["-4851_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_escape02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_studio-wheatley_leave_relay",input="Trigger",parameter="",delay=0.0 },
			]
		}

		// Oh! Good! I did not think that was going to work.
		SceneTable["-4853_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_playerdies01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_studio-wheatley_leave_relay",input="Trigger",parameter="",delay=0.0 },
			]
		}

		// Oh my god... what has he done to this place...
		SceneTable["-8833_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_speech01.vcd") 
			char = "glados"
			postdelay = 1.5 
			predelay = 3.0
			next = null
			queue = true
			queuecharacter = "wheatley"			
			noDingOff = true
			noDingOn = true			
		}	
		
		// You know, I'm not stupid. I know you don't want to put me back in charge.                                                                                                                                                                                    
		SceneTable["-8834_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_tbeam01.vcd") 
			char = "glados"
			postdelay = 0.5
			predelay = 8.0
			next = "-8834_02"
			noDingOff = true
			noDingOn = true			
		}	
		
		// You think I'll betray you. And on any other day, you'd be right.                                                                                                                                                                                    
		SceneTable["-8834_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_tbeam05.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-8834_03"
			noDingOff = true
			noDingOn = true			
		}				

		// Listen, the scientists were always hanging cores on me to regulate my behavior. I've heard voices all my life But now I hear the voice of a conscience, and it's terrifying, because for the first time it's my voice.                                                                                                                                                                                                                         
		SceneTable["-8834_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_tbeam07.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-8834_04"
			noDingOff = true
			noDingOn = true			
		}			
		
		
		// I'm being serious, I think there's something really wrong with me.                                                                                                                                                                                     
		SceneTable["-8834_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_tbeam09.vcd") 
			char = "glados"
			postdelay = 0.6 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true			
		}
	
		
		
	}

// **************************************************************************************************
// Boss Battle
// ****************************************************************************************************
	
// ****************************************************************************************************
// Boss Battle
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_finale4")
	{

		// ====================================== Corrupted Core Intro
		
		// Corrupted cores! We're in luck. 
		SceneTable["-4900_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_intro02.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4900_02"
			noDingOff = true
			noDingOn = true	
		}		
	
		// You find a way to stun him, I'll send you a core, and then you attach it to him. If we do it a few times, he might become corrupt enough for another core transfer.                                                                                          
		SceneTable["-4900_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_intro05.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}


		
		// ====================================== Breaker Shaft
		
		// Plug me on to this socket.
		SceneTable["-4901_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_plugmein01.vcd") 
			char = "glados"
			postdelay = 10.0
			predelay = 0.0
			next = "-4901_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Plug me in nag
		SceneTable["-4901_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_plugmein05.vcd") 
			char = "glados"
			postdelay = 10.0 
			predelay = 0.0
			next = "-4901_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// Plug me in nag
		SceneTable["-4901_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_plugmein03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		

		
		// Look, even if you still think we're enemies, we're enemies with some urgent common interests: Not blowing up. Revenge.                                                                                                                                                                                                                                                                                                                          
		SceneTable["-2298_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_a4_newrevenge01.vcd") 
			char = "glados"
			postdelay = 0.2 
			predelay = 0.0
			next = "-2298_02"
			noDingOff = true
			noDingOn = true

			fires=
			[
				// all coords somewhat random, avg of three SetParentAttachmentMaintainOffset settings
				{entity="wheatley_shield",input="setlocalorigin",parameter="13.1 277.2 -31.0",delay=3.0},
				{entity="wheatley_shield",input="setlocalangles",parameter="0 180 90",delay=3.0},
				{entity="futbol_shooter",input="setlocalorigin",parameter="1.7 75.0 -116.6",delay=3.0},
				{entity="futbol_shooter",input="setlocalangles",parameter="86.5 158.6 66.5",delay=3.0},
				{entity="wheatley_shadow_brush",input="setlocalorigin",parameter="6.0 12.5 -10.0",delay=3.0},
				{entity="wheatley_shadow_brush",input="setlocalangles",parameter="86.5 158.6 66.5",delay=3.0},
			]
		}	

		// You like revenge, right? Everybody likes revenge. Well, let's go get some.                                                                                                                                                                                      
		SceneTable["-2298_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale3_tbeam11.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true			
		}	
		
		/*
		// I realize there's some handwaving in the part where you stun him, but try to figure something out. 
		SceneTable["-2298_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_intro06.vcd") 
			char = "glados"
			postdelay = 0.7 
			predelay = 3.0
			next = "-2298_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Good luck!
		SceneTable["-2298_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_elevator03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		*/

		
		
		// ====================================== Chamber Intro
		
		/*
		// Hello.
		SceneTable["-4910_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// According to the control panel light up here, the entire building's going to self destruct in six minutes. I'm pretty sure it's a problem with the light. But in case it isn't, I'm going to have to kill you.
		SceneTable["-4910_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// So, let's call that three minutes, and then a minute break, which should leave a leisurely two minutes to figure out how to shut down whatever's starting all these fires. Also, I watched the tapes of you killing her
		SceneTable["-4910_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_04"
			noDingOff = true
			noDingOn = true	
		}
		
		// One: No portal surfaces.
		SceneTable["-4910_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_05"
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_neurotoxin_relay",input="Trigger",parameter="",delay=7.0},
			]
		}
		
		// Two: Start the neurotoxin. In two minutes. Gives you plenty of time to think about it. I'm joking. Two: We're doing it now.
		SceneTable["-4910_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_06"
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_shield_relay",input="Trigger",parameter="",delay=2.0},
			]
		}
		
		// Three: Bomb-proof shields. Leading directly into Four: Bombs. Explosive ones. For throwing at you.
		SceneTable["-4910_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4910_07"
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="wheatley_bomb_relay",input="Trigger",parameter="",delay=5.0},
			]
		}
		
		// You know what, this plan is so good, I'm going to give you a sporting chance and turn off the neurotoxin. No I'm not. Goodbye.
		SceneTable["-4910_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_intro07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		*/
		
		// ====================================== Pre-Pipe Destruction
		
		// Where are you going? Don't run. Don't run. The harder you breathe, the more neurotoxin you'll inhale. It's bloody clever. Devilish.
		
		/*
		SceneTable["-4911_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_taunt_a01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		*/

		// ====================================== Post-Pipe Destruction
		
		// No! Um... That's an impression of you. Because you just fell into my trap. There. Just now. Ha. I wanted you to trick me into bursting that pipe. Gives you false hope. Leads to overconfidence. Mistakes. Fatal missteps. All part of my plan.
		/*
		SceneTable["-4912_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_tubebreak01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		*/

		// ====================================== Post-Pipe Post Portal Destruction
		
		/*
		// Oh, you are using it. Well, I dodged a bullet there. Welcome back. To the steel bosom of my death trap. Ha.
		SceneTable["-4913_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_tubebreak03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		*/
		
		// ====================================== Core Delivery
		
		// Here's the first corrupted core
		SceneTable["-4915_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_corenags03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4915_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Place it onto Wheatley
		SceneTable["-4915_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_corenags05.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="Core01Babble1()",delay=1.0},
			]
		}
		
		// Here's the second corrupted core
		SceneTable["-4916_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_corenags09.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="Core02Babble1()",delay=0.8},
			]
		}
		
		/*
		// I am having trouble with the system
		SceneTable["-4916_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4916_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// I will have to use a new delivery location
		SceneTable["-4916_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores05.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="Core02Babble1()",delay=1.0},
			]
		}
		*/
		
		// Here's the third corrupted core
		SceneTable["-4917_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_corenags15.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		/*
		// It's coming to the other side of the chamber
		SceneTable["-4917_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// ====================================== Wake-Up Clueless
		*/
		/*
		
		// I'm awake again!
		SceneTable["-4920_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		*/
		
		/*
		
		// Ah! Good power nap! Well rested.
		SceneTable["-4921_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Took a little nap there. Refreshed. Ready to kill you.
		SceneTable["-4922_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// ====================================== Wake-Up Still Clueless
		
		// Ohhhh. I see what you're doing. I... no. No I don't.
		SceneTable["-4925_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Trying to... guess the significance of these spheres you keep putting on me...
		SceneTable["-4926_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Ah-HA! I-- no. Lost it. Almost had it.
		SceneTable["-4927_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Are they supposed to weigh me down? Because they're actually very light. Stylish. Accessorizing.
		SceneTable["-4928_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_wakeup07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		*/

		// ====================================== Core Transfer

		/*
		// Alternate core detected.
		SceneTable["-4930_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/gladosbattle03.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Substitute core, are you ready to start the procedure?
		SceneTable["-4930_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/gladosbattle06.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// Yes!
		SceneTable["-4930_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/sp_sabotage_glados_yes01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_04"
			noDingOff = true
			noDingOn = true	
		}
		
		// Corrupted core, are you ready to start the procedure?
		SceneTable["-4930_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/gladosbattle07.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_05"
			noDingOff = true
			noDingOn = true	
		}
		
		// Ah, false.
		SceneTable["-4930_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_06"
			noDingOff = true
			noDingOn = true	
		}
		
		// Stalemate detected.  Transfer procedure cannot continue.
		SceneTable["-4930_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/gladosbattle08.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4930_07"
			noDingOff = true
			noDingOn = true	
		}
		
		// Unless a stalemate associate is present and can press the stalemate resolution button.
		SceneTable["-4930_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/gladosbattle09.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			fires=
			[
				{entity="stalemate_relay",input="Trigger",parameter="",delay=0.0},
			]
		}
		*/
		
		// ====================================== Stalemate Button Nags
		
        // Don't press the button!
        SceneTable["-4940_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="stalemate_buttonnags",idleorderingroup=1}
        SceneTable["-4940_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=2}
        SceneTable["-4940_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=3}
		SceneTable["-4940_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=4}
		SceneTable["-4940_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags05.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=5}
		SceneTable["-4940_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags06.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=6}
		
		// ====================================== Button Pressed
		
		SceneTable["ThornSide01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/core03/babble25.vcd") 
			char = "core03"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}


		// PART 5: BOOBY TRAP THE STALEMATE BUTTON!
		SceneTable["-4941_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_button_press01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4941_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// What, are you still alive? You are joking. Well, I'm still in control. AND I HAVE NO IDEA HOW TO FIX THIS PLACE.
		SceneTable["-4941_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_button_press03.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4941_03"
			noDingOff = true
			noDingOn = true	
		}
		
		//You had to play bloody cat and mouse, didn't you?  While people were trying to work. Yes, well, now we're all going to pay the price. WE'RE ALL GOING TO BLOODY DIE.
		SceneTable["-4941_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_button_press04.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4941_04"
			noDingOff = true
			noDingOn = true	
		}

		//Oh yes, brilliant. Take one last look at your precious human moon. Because it cannot help you now!
		SceneTable["-4941_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_button_press05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}


		// ====================================== Portal Placed
/*		
00;00;33;19 – bw_finale04_portal_opens_short08.wav
00;00;34;02 – bw_finale04_portal_opens13.wav
00;00;34;24 – bw_finale04_portal_opens15.wav
00;00;35;28 – bw_finale04_through_portal01.wav
00;00;36;19 – bw_finale04_portal_opens03.wav
00;00;40;09 – bw_finale04_through_portal01.wav
00;00;40;24 – bw_finale04_through_portal02.wav
00;00;41;21 – bw_finale04_portal_opens15_short07.wav
00;00;42;03 – bw_finale04_through_portal03.wav
00;00;45;00 – bw_finale04_portal_opens16.wav
00;00;47;16 – bw_finale04_through_portal04.wav
00;00;53;03 – bw_finale04_through_portal05.wav
00;00;54;06 – bw_finale04_through_portal06.wav
*/
		
		
		// AHHHHHHH!
		SceneTable["-4950_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens_short08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01a"
			noDingOff = true
			noDingOn = true	
		}
		

		SceneTable["-4950_01a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens13.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01b"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens15.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01c"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01c" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01d"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01d" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01e"
			noDingOff = true
			noDingOn = true	
		}
		
		SceneTable["-4950_01e" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01f"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01f" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01g"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01g" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens_short07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_01h"
			noDingOff = true
			noDingOn = true	
		}

		SceneTable["-4950_01h" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_02"
			noDingOff = true
			noDingOn = true	
		}


		// Let go! Let go! I'm still connected. I can pull myself in. I can still fix this!
		SceneTable["-4950_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_04"
			noDingOff = true
			noDingOn = true	
		}
		

		// Let go!
		SceneTable["-4950_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_05"
			noDingOff = true
			noDingOn = true	
		}
		
		// LET GO OF ME!
		SceneTable["-4950_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4950_06"
			noDingOff = true
			noDingOn = true	
		}
		
		// Oh no. Change of plans. Hold onto me. Tighter!
		SceneTable["-4950_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 2.8
			next = "-4950_07"
			noDingOff = true
			noDingOn = true	
		}
		
		// Grab me grab me grab me! Grab meeee!
		SceneTable["-4950_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_finale04_through_portal08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
	

// ****************************************************************************************************
// Epilogue
// ****************************************************************************************************

		// Oh thank god, you're alright.  
		SceneTable["-4951_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue03.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4951_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// You know, being Caroline taught me a valuable lesson. I thought you were my greatest enemy. When all along you were my best friend.                                                                                                                          
		SceneTable["-4951_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue04.vcd") 
			char = "glados"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4951_03"
			noDingOff = true
			noDingOn = true	
		}			
		
		// Being Caroline taught me another valuable lesson: where Caroline lives in my brain.                                                                                                                                                                          
		SceneTable["-4951_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue07.vcd") 
			char = "glados"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4951_03a"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Caroline deleted.
		SceneTable["-4951_03a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/carolyndeleted02.vcd") 
			char = "announcer"
			postdelay = 0.2
			predelay = 0.0
			next = "-4951_04"
			noDingOff = true
			noDingOn = true
		}		

		// Goodbye, Caroline.     
		SceneTable["-4951_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue12.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4951_05"
			noDingOff = true
			noDingOn = true	
		}	

		// You know, deleting Caroline just now taught me a valuable lesson. The best solution to a problem is usually the easiest one. And I'll be honest.                                                                                                             
		SceneTable["-4951_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue14.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4951_06"
			noDingOff = true
			noDingOn = true	
		}	

		// Killing you? Is hard.
		SceneTable["-4951_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epiloguekillyou02.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4951_07"
			noDingOff = true
			noDingOn = true	
		}	

		// You know what my days used to be like? I just tested. Nobody murdered me. Or put me in a potato. Or fed me to birds. I had a pretty good life.                                                                                                               
		SceneTable["-4951_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue19.vcd") 
			char = "glados"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4951_08"
			noDingOff = true
			noDingOn = true	
		}	

		// And then you showed up. You dangerous, mute lunatic. So you know what?                                                                                                                                                                                       
		SceneTable["-4951_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue20.vcd") 
			char = "glados"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4951_09"
			noDingOff = true
			noDingOn = true	
		}	

		// You win.   
		SceneTable["-4951_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue23.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4951_10"
			noDingOff = true
			noDingOn = true	
		}	

		// Just go.  
		SceneTable["-4951_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue25.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4951_11"
			noDingOff = true
			noDingOn = true	
		}	

		// It's been fun. Don't come back.    
		SceneTable["-4951_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue28.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4951_12"
			noDingOff = true
			noDingOn = true	
		}	

		// It's been fun. Don't come back.   
		SceneTable["-4951_12" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/epilogue29.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}			
		
		
	}
	


	function WheatleySurpriseLeadup1()
	{
		GladosPlayVcd( -4800 )
	}

	function WheatleySurprise()
	{
		if (curMapName=="sp_a4_finale2")
		{ 
			GladosPlayVcd ( -4820 )
		}
		else
		{
			GladosPlayVcd( -4801 )
		}
	}
		
	if (curMapName=="sp_a4_finale1")
	{
		sp_a4_finale1_did_EscapePlacePaint <- false
		sp_a4_finale1_did_WheatleyEscape01 <- false
	}


	function Wheatleyfinale01()
	{
		GladosPlayVcd( -4802 )
	}

	function Gladosfinale01()
	{
		GladosPlayVcd ( -4803 )
	}
	
	function EscapePlacePaint()
	{
		if (!sp_a4_finale1_did_EscapePlacePaint)
		{
			sp_a4_finale1_did_EscapePlacePaint = true
			nuke()
			GladosPlayVcd ( -4804 )
		}	
	}

	function WheatleyEscape01()
	{
		if (!sp_a4_finale1_did_WheatleyEscape01)
		{
			sp_a4_finale1_did_WheatleyEscape01 = true
			nuke()
			GladosPlayVcd ( -4805 )
		}	
	}
	function WheatleyEscape02()
	{
		GladosPlayVcd ( -4806 )
	}
	function WheatleyEscape03()
	{
		GladosPlayVcd ( -4807 )
	}
	function WheatleyEscape04()
	{
		GladosPlayVcd ( -4808 )
	}
	function WheatleyEscape05()
	{
		GladosPlayVcd ( -4809 )
	}
	function WheatleyEscape06()
	{
		GladosPlayVcd ( -4810 )
	}
	function WheatleyEscape07()
	{
		GladosPlayVcd ( -4811 )
	}
	function WheatleyEscape08()
	{
		GladosPlayVcd ( -4812 )
	}
	
	function WheatleyEscapeReturn()
	{
		GladosPlayVcd ( -4500 )
	}

	function Finale02_catwalk_intro()
	{
		GladosPlayVcd ( -4820 )
	}
	function Finale02_catwalk_finished()
	{
		GladosPlayVcd ( -4821 )
	}
	function Finale02_turret_trap()
	{
		GladosPlayVcd ( -4822 )
	}
	function Finale02_turret_fix()
	{
		GladosPlayVcd ( -4823 )
	}
	function Finale02_turret_tbeam()
	{
		GladosPlayVcd ( -4824 )
	}
	function Finale02_grinder_reveal()
	{
		GladosPlayVcd ( -4825 )
	}
	function Finale02_grinder_escape()
	{
		GladosPlayVcd ( -4826 )
	}
	function Finale02_trapped()
	{
		GladosPlayVcd ( -4827 )
	}
	function Finale02_crusher_ride_start()
	{
		GladosPlayVcd ( -4828 )
	}
	function Finale02_crusher_ride_smash01()
	{
		GladosPlayVcd ( -4829 )
	}
	function Finale02_crusher_ride_smash02()
	{
		GladosPlayVcd ( -4830 )
	}
	function Finale02_crusher_ride_smash03()
	{
		GladosPlayVcd ( -4831 )
	}
	function Finale02_crusher_outro()
	{
		GladosPlayVcd ( -4832 )
	}
	function Finale02_pipe_collapse()
	{
		GladosPlayVcd ( -4833 )
	}
	function Finale02_exit()
	{
		GladosPlayVcd ( -4834 )
	}



// ============================================================================
// Functions for Recapture
// ============================================================================
	
	function GladosParadoxInception()
	{
		GladosPlayVcd( -4000 )
	}
	
	function RecaptureEavesdrop()
	{
		GladosPlayVcd( -4001 )
	}
	
	function RecaptureObserve()
	{
		GladosPlayVcd( -4003 )
	}
	
	function RecaptureCubeTest()
	{
		GladosPlayVcd( -4004 )
	}
	
	function RecaptureSolveLeadIn()
	{
		GladosPlayVcd( -4005 )
	}
	
	function RecaptureSolve()
	{
		GladosPlayVcd( -4006 )
	}
	
	function RecaptureButton()
	{
		nuke()
		GladosPlayVcd( -4007 )
	}

	function RecaptureProceed()
	{
		GladosPlayVcd( -4008 )
	}
	
	function RecaptureItch()
	{
		GladosPlayVcd( -4009 )
	}
	
	function RecaptureFirstTest()
	{
		GladosPlayVcd( -4010 )
	}
	
	function RecaptureFirstTestDone()
	{
		GladosPlayVcd( -4011 )
	}
	
	function RecaptureFirstTestRedo()
	{
	//	GladosPlayVcd( -4012 )
	}
	
	function RecaptureTestTwoDone()
	{
		nuke()
		GladosPlayVcd( -4013 )
	}
	
	function GladosCompare1()
	{
	//	GladosPlayVcd( -4330 )
	}
	
	function WheatleyTestStolen()
	{
		GladosPlayVcd( -4016 )
	}
	
	function GladosBadNews()
	{
		GladosPlayVcd( -4017 )
	}
	
	function GladosCrushers()
	{
		GladosPlayVcd( -4018 )
	}
	
	function StartWheatleyTest1Nag()
    {
         printl("==Starting Wheatley Test 1 Nag")
         GladosPlayVcd( -4021 )
    }
	
	function StartWheatleyTest1FinishNag()
    {
         printl("==Starting Wheatley Test 1 Finish Nag")
         GladosPlayVcd( -4022 )
    }
	
	if (curMapName=="sp_a4_intro")
	{
		sp_a4_intro_startwheatleytest2nag_started <- false
	}

	function StartWheatleyTest2Nag()
    {
		if (!sp_a4_intro_startwheatleytest2nag_started)
		{
			sp_a4_intro_startwheatleytest2nag_started = true
			printl("==Starting Wheatley Test 2 Nag")
			GladosPlayVcd( -4019 )
		}	
    }

    function StopWheatleyNag()
    {
        printl("==Stopping Wheatley Nag")
        GladosStopNag()
        GladosCharacterStopScene("wheatley")  // This will make Wheatley stop talking if he is mid sentence.
    }

	function RecaptureFirstTestEnd()
	{
		GladosPlayVcd( -4020 )
	}
	
	function RecaptureDone()
	{
		GladosPlayVcd( -4023 )
	}

	function tb_intro_solve()
	{
		GladosPlayVcd( -4445 )
	}

	function tb_intro_glados_start()
	{
		GladosPlayVcd( -9077 )
	}
		

// ============================================================================
// Functions for tb_trust_drop / tb_wall_button / tb_polarity / tb_catch / stop_the_box / laser_catapult / laser_platform / jump_polarity
// ============================================================================


	function tb_trust_drop_glados_start()
	{
	printl("testy test test test")
	GladosPlayVcd( -4770 )
	}

	function tb_trust_drop_intro()
	{
		GladosPlayVcd( -4400 )
	}
	
	function tb_trust_drop_cheat()
	{
		GladosPlayVcd( -4401 )
	}

	function tb_trust_drop_solve()
	{
		GladosPlayVcd( -4402 )
	}

	function tb_trust_drop_glados_end()
	{
	}
	
	function tb_wall_button_glados_start()
	{
		GladosPlayVcd( -4790 )	
	}
	
	function tb_wall_button_start_yet()
	{
		GladosPlayVcd( -4403 )
	}

	function tb_wall_button_crush_end()
	{
		GladosPlayVcd( -4404 )	
	}	
	
	function tbwallbutton_destruction_complete()
	{
	}
	
	function tb_wall_button_test_solve_1()
	{
	}
	
	function tb_wall_button_test_solve_2()
	{
		GladosPlayVcd( -4406 )	
	}
	
	function tb_wall_button_test_solve_3()
	{
		GladosPlayVcd( -4407 )	
	}	
	
	function tb_wall_button_test_stop_1()
	{
		GladosPlayVcd( -4408 )	
	}	
	
	function tb_wall_button_test_stop_2()
	{
	}	

	function tb_wall_button_puzzle_almost_complete() 
	{
		GladosPlayVcd( -4405 )	
	}
	
	function tb_wall_button_puzzle_completed()
	{
		GladosPlayVcd( -4111 )	
	}
	
	function tb_wall_button_glados_end()
	{
		GladosPlayVcd( -4090 )		
	}

	function tb_polarity_glados_start()
	{
		GladosPlayVcd( -4780 )		
	}
	
	function tb_polarity_test_start()
	{
		GladosPlayVcd( -4410 )	
	}	

	function tb_polarity_test_end()
	{
		GladosPlayVcd( -4411 )	
	}	

	function tb_polarity_glados_end()
	{
		GladosPlayVcd( -4447 )	
	}		
	
	function tb_catch_glados_start()
	{
		GladosPlayVcd( -4470 )	
	}	
	
	function tb_catch_test_start()
	{
		GladosPlayVcd( -4412 )	
	}		

	function tb_catch_test_end()
	{
		GladosPlayVcd( -4413 )	
	}

	function tb_catch_glados_end()
	{
	}
	
	function stop_the_box_glados_start()
	{
		GladosPlayVcd( -4446 )	
	}	
	
	function stop_the_box_test_start()
	{
		GladosPlayVcd( -4414 )	
	}		

	function sp_a4_stop_the_box_intro_over()
	{
		sp_a4_stop_the_box_intro_played = true
	}

	function stop_the_box_missed()
	{
		if (sp_a4_stop_the_box_check_smash())
		{
			if (sp_a4_stop_the_box_intro_played)
			{
				GladosPlayVcd( -4415 )	
			}	
		}	
	}
	
	function stop_the_box_test_end()
	{
		GladosPlayVcd( -4416 )	
	}

	function stop_the_box_glados_end()
	{
		//GladosPlayVcd( -4491 )		
	}
	
	function laser_catapult_glados_start()
	{
		GladosPlayVcd( -5799 )			
	}	
	
	function laser_catapult_test_start()
	{
		GladosPlayVcd( -4417 )	
	}

	function laser_catapult_test_end()
	{
		GladosPlayVcd( -4418 )	
	}

	function laser_catapult_glados_end()
	{
		EntFire("huge_rumble","trigger", 0.0, 0.0 )	
		GladosPlayVcd( -4492 )		
	}	
	
	function laser_platform_glados_start()
	{
		EntFire("huge_rumble_1","trigger", 0, 2.0 )
		GladosPlayVcd( -4333 )		
	}	
	
	function laser_platform_test_start()
	{
		EntFire("huge_rumble_1","trigger", 0, 0 )
		GladosPlayVcd( -4419 )	
	}

	function laser_platform_fall()
	{
		GladosPlayVcd( -4588 )	
	}	

	function BreakIntoOffice()
	{
	}	
	
	function laser_platform_glados_end()
	{
	}		

	function jump_polarity_glados_start()
	{
		GladosPlayVcd( -4555 )		
	}		
	
	function jump_polarity_sorry()
	{
		GladosPlayVcd( -4420 )	
	}
	
	function jump_polarity_test_end()
	{
		GladosPlayVcd( -4421 )	
	}

	function jump_polarity_glados_end()
	{
	}	

	function speed_tb_catch_glados_start()
	{
	}	
	
	function speed_tb_catch_test_start()
	{
	}
	
	function speed_tb_catch_intro_test_start()
	{
		GladosPlayVcd( -4448 )	
	}
	
	function speed_tb_catch_intro_test_complete() 
	{
	}

	function speed_tb_catch_test_end() 
	{
		GladosPlayVcd( -4449 )	
	}
	
	function speed_tb_catch_glados_end()
	{
		//GladosPlayVcd( -4493 )		
	}		
	
	

	function jump_polarity_glados_end()
	{
		GladosPlayVcd( -4494 )		
	}	
	
	function Wheatley_crush_ambush() 
	{
		GladosPlayVcd( -4498 )	
	}
	
	function Wheatley_games_begin() 
	{
	}
	
	function finale3_vista() 
	{
		GladosPlayVcd( -8833 )
	}	
	
	function finale3_tbeam_ride() 
	{
		GladosPlayVcd( -8834 )
	}	
	
	function finale3_tbeam_exit() 
	{
//		GladosPlayVcd( -8888 )
	}	
	

	// ============================================================================
	// Functions for Boss Battle
	// ============================================================================
	
	if (curMapName=="sp_a4_finale4")
	{
		sp_a4_finale4_gelbreak <- false
		sp_a4_finale4_extradialoga <- false
		sp_a4_finale4_extradialogb <- false
		sp_a4_finale4_prepipestarted <- false
	}
	
	function GladosCorruptedCores()
	{
		GladosPlayVcd( -4900 )
	}
	
	function GladosBreakerShaft()
	{
		GladosPlayVcd( -4901 )
	}

	function BBChamberIntro()
	{
		//GladosPlayVcd( -4910 )
		GladosPlayVcd(591)
	}
	
	
	function PotatosPlugged()
	{
		GladosPlayVcd( -2298 )	
	}

	function WheatleyHitByBomb()
	{
		nuke()
		GladosPlayVcd(596)
	}

	function BBPrePipeDest()
	{
		//GladosPlayVcd( -4911 )
		if ((!sp_a4_finale4_gelbreak) && (!sp_a4_finale4_prepipestarted))
		{
			sp_a4_finale4_prepipestarted = true
			GladosPlayVcd( 592 )
		}
	}
	
	function BBExtraDialogA()
	{
		if (!sp_a4_finale4_extradialoga)
		{
			sp_a4_finale4_extradialoga = true
			GladosPlayVcd(594)
		}
		else
		{
			BBExtraDialogB()
		}
	}

	function BBExtraDialogB()
	{
		if (!sp_a4_finale4_extradialogb)
		{
			sp_a4_finale4_extradialogb = true
			GladosPlayVcd(595)
		}	
	}

	function BBPostPipeDest()
	{
		
		//GladosPlayVcd( -4912 )
		sp_a4_finale4_gelbreak = true
		nuke()
		GladosPlayVcd( 593 )
	}
	
	function BBPostPipePortal()
	{
		//GladosPlayVcd( -4913 )
	}
		
	function GladosDeliverCore1()
	{
		GladosPlayVcd( -4915 )
	}
	
	function GladosDeliverCore2()
	{
		GladosPlayVcd( -4916 )
	}

	function GladosDeliverCore3()
	{
		GladosPlayVcd( -4917 )
	}	
	
	function Core01PickUp()
	{
	}
	
	function Core02PickUp()
	{
		EntFire("@core02","SetIdleSequence", "core03_idle", 0.2 )	
	}

	function Core03PickUp()
	{
		EntFire("@core03","SetIdleSequence", "core02_idle", 0.2 )	
	}

	function BBWakeUpClueless1()
	{
		//GladosPlayVcd( -4920 )
		if ((!sp_a4_finale4_extradialoga) || (!sp_a4_finale4_extradialogb))
		{
			GladosPlayVcd(597)
		}
		else
		{
			GladosPlayVcd(598)
		}
	}
	
	function BBWakeUpClueless2()
	{
		//GladosPlayVcd( -4921 )
		GladosPlayVcd(599)
	}
	
	function BBWakeUpClueless3()
	{
		//GladosPlayVcd(600)
	}
	
	function BBWakeUpStill1()
	{
		//GladosPlayVcd( -4925 )
	}
	
	function BBWakeUpStill2()
	{
		//GladosPlayVcd( -4926 )
	}
	
	function BBWakeUpStill3()
	{
		//GladosPlayVcd( -4927 )
	}
	
	function BBWakeUpStill4()
	{
		//GladosPlayVcd( -4928 )
	}
	
	function BBCoreTransfer()
	{
		//GladosPlayVcd( -4930 )
		if (sp_a4_finale4_thorn)
		{
			GladosPlayVcd(-7400)
		}
		else
		{
			GladosPlayVcd(600)
		}	
	}
	
	function BBButtonNags()
	{
		//GladosPlayVcd( -4940 )
	}
	
	function BBButtonNags2()
	{
		GladosPlayVcd(612)
	}


	function BBButtonPressed()
	{
		nuke()
		GladosPlayVcd( -4941 )
	}
	
	function BBPortalPlaced()
	{
		nuke()
		//GladosPlayVcd( -4950 )
	}
	

//Core Placement functions:
function Core1SocketPlug()
{
	GladosPlayVcd(633)
	//EntFire("wheatley_continue_1","trigger", 0, 2.0 )
	EntFire("autosave1_relay","trigger", 0, 6.0 )
}	

function Core2SocketPlug()
{
	if (sp_a4_finale4_thorn)
	{
		EntFire("@glados","runscriptcode", "GladosPlayVcd(634)", 2.49 )
		EntFire("autosave2_relay","trigger", 0, 9.0 +2.49 )
		GladosPlayVcd(-7401)
	}
	else
	{
		//EntFire("wheatley_continue_2","trigger", 0, 2.0 )
		GladosPlayVcd(634)
		EntFire("autosave2_relay","trigger", 0, 9.0 )
	}
}


function Core3SocketPlug()
{
	GladosPlayVcd(635)
	//EntFire("wheatley_continue_3","trigger", 0, 2.0 )
	EntFire("autosave3_relay","trigger", 0, 2.0 )
}

function BBToxinCountdown4()
{
}
function BBToxinCountdown3()
{
}
function BBToxinCountdown2()
{
}
function BBToxinCountdown1()
{
}
function BBDestCountdown4()
{
}
function BBDestCountdown3()
{
}

function BBDestCountdown1()
{
}

function BBDestCountdown2()
{
}
function BBWorldCountdown1()
{
}

function BBWorldCountdown2()
{
}

function BBWorldCountdown1()
{
}

// ****************************************************************************************************
// Personality Cores scenetable								  
// ****************************************************************************************************

SceneTableLookup[-7100] <- "-7100_01" 

SceneTableLookup[-7200] <- "-7200_01" 

SceneTableLookup[-7400] <- "altwakeupthree01"
SceneTableLookup[-7401] <- "ThornSide01"

SceneTableLookup[-7300] <- "-7300_01" 
	
		// ****************************************************************************************************
		// Personality Cores
		// ****************************************************************************************************
	
	if (curMapName=="sp_a4_finale4" || curMapName=="core_test1")
	{
		sp_a4_finale4_thorn <- false

		//Alternate last wakeup with adventure sphere cool line
		//ahhhhEHHHHHH!
		SceneTable["altwakeupthree01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_portal_opens17.vcd"),postdelay=0.0,next="altwakeupthree01a",char="wheatley",noDingOff=true,noDingOn=true}
		//You have been a thorn in my side long enough!
		SceneTable["altwakeupthree01a"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_thorn01.vcd"),postdelay=-3.5,next="altwakeupthree01b",char="wheatley",noDingOff=true,noDingOn=true}
		//Well this thorn is about to take you down! Man that sounded a whole lot better in my head...
		SceneTable["altwakeupthree01b"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble26.vcd"),postdelay=0.0,next="altwakeupthree02",char="core03",noDingOff=true,noDingOn=true,talkover=true}
		//Manual core replacement required.
		SceneTable["altwakeupthree02"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate01.vcd"),postdelay=0.0,next="altwakeupthree03",char="bossannouncer",noDingOff=true,noDingOn=true}
		//Ohhh. I see. [chuckle]
		SceneTable["altwakeupthree03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_stalemate_intro01.vcd"),postdelay=-2.8,next="altwakeupthree04",char="wheatley",noDingOff=true,noDingOn=true}
		//Substitute Core: Are you ready to start?
		SceneTable["altwakeupthree04"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate02.vcd"),postdelay=-1.5,next="altwakeupthree05",char="bossannouncer",talkover=true,noDingOff=true,noDingOn=true}
		//Yes! Come on!
		SceneTable["altwakeupthree05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale4_stalemate05.vcd"),postdelay=0.0,next="altwakeupthree06",char="glados",talkover=true,noDingOff=true,noDingOn=true}
		//Corrupted Core: are you ready to start?
		SceneTable["altwakeupthree06"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate03.vcd"),postdelay=0.0,next="altwakeupthree07",char="bossannouncer",noDingOff=true,noDingOn=true}
		//What do you think?
		SceneTable["altwakeupthree07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_stalemate_intro03.vcd"),postdelay=0.0,next="altwakeupthree08",char="wheatley",talkover=true,noDingOff=true,noDingOn=true}
		//Interpreting vague answer as YES.
		SceneTable["altwakeupthree08"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate04.vcd"),postdelay=-1.65,next="altwakeupthree09",char="bossannouncer",noDingOff=true,noDingOn=true}
		//No! No! NONONO!
		SceneTable["altwakeupthree09"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_stalemate_intro04.vcd"),postdelay=0.0,next="altwakeupthree10",char="wheatley",talkover=true,noDingOff=true,noDingOn=true}
		//Didn't pick up on my sarcasm...
		SceneTable["altwakeupthree10"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_stalemate_intro05.vcd"),postdelay=0.0,next="altwakeupthree11",char="wheatley",noDingOff=true,noDingOn=true}
		//Stalemate detected.
		SceneTable["altwakeupthree11"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate05.vcd"),postdelay=2.3,next="altwakeupthree12",char="bossannouncer",fires=[{entity="fire_relay",input="trigger",parameter="",delay=0.5,fireatstart=true}],noDingOff=true,noDingOn=true}
		//Fire detected in the Stalemate Resolution Annex. Extinguishing.
		SceneTable["altwakeupthree12"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate06.vcd"),postdelay=4.0,next="altwakeupthree13",char="bossannouncer",fires=[{entity="sprinkler_relay",input="trigger",parameter="",delay=0.0}],noDingOff=true,noDingOn=true}
		//Ah. That just cleans right off, does it? Would have been good to know. A little earlier.
		SceneTable["altwakeupthree13"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_finale04_stalemate_intro06.vcd"),postdelay=0.1,next="altwakeupthree14",char="wheatley",noDingOff=true,noDingOn=true}
		//Stalemate Resolution Associate: Please press the Stalemate Resolution Button.
		SceneTable["altwakeupthree14"] <- {vcd=CreateSceneEntity("scenes/npc/announcer/bb_stalemate07.vcd"),postdelay=0.1,next=null,char="bossannouncer",fires=[{entity="stalemate_relay",input="trigger",parameter="",delay=3.0,fireatstart=true},{entity="@glados",input="runscriptcode",parameter="BBButtonNags2()",delay=0.0}],noDingOff=true,noDingOn=true}


		// ====================================== "Space" Core01
		
		//Space space wanna go to space yes please space. Space space. Go to space.
		SceneTable["-7100_01"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble20.vcd"),idlerepeat=true, postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idle=true,idleminsecs=0.00,idlemaxsecs=0.08,idlegroup="core01_babble",idleorderingroup=1}
		//Ba! Ba! Ba ba ba! Space! Ba! Ba! Ba ba ba!
		SceneTable["-7100_02"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble24.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=2}
		//Oh. Play it cool. Play it cool. Here come the space cops.
		SceneTable["-7100_03"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble28.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=3}
		//Help me, space cops. Space cops, help.
		SceneTable["-7100_04"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble29.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=4}
		//Better buy a telescope. Wanna see me. Buy a telescope. Gonna be in space.
		SceneTable["-7100_05"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble33.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=5}
		//Space space wanna go to space
		SceneTable["-7100_06"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble67.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=6}
		//Orbit. Space orbit. In my spacesuit.
		SceneTable["-7100_07"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble58.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=7}
		//Oo. Oo. Oo. Lady. Oo. Lady. Oo. Let's go to space.
		SceneTable["-7100_08"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble105.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=8}
		//Gotta go to space. Lady. Lady.
		SceneTable["-7100_09"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble102.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=9}
		//Come here, space. I have a secret for you. No, come closer.
		SceneTable["-7100_10"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble62.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=10}
		//What's your favorite thing about space? Mine is space.
		SceneTable["-7100_11"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble05.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=11}
		//Space. Trial. Puttin' the system on trial. Guilty. Of being in space. Going to space jail.
		SceneTable["-7100_12"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble16.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=12}
		//Dad! I'm in space! [low-pitched 'space' voice] I'm proud of you, son. [normal voice] Dad, are you space? [low-pitched 'space' voice] Yes. Now we are a family again.
		SceneTable["-7100_13"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble19.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=13}
		//Space going to space can't wait.
		SceneTable["-7100_14"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble11.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=14}
		//Space...
		SceneTable["-7100_15"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble14.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=15}
		//Space space wanna go to space
		SceneTable["-7100_16"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble21.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=16}
		//Space space going to space oh boy
		SceneTable["-7100_17"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble22.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=17}
		//Going to space going there can't wait gotta go. Space. Going.
		SceneTable["-7100_18"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble31.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=18}
		//Space. Space.
		SceneTable["-7100_19"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble35.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=19}
		//I'm going to space.
		SceneTable["-7100_20"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble36.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=20}
		//Oh boy.
		SceneTable["-7100_21"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble37.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=21}
		//Yeah yeah okay okay.
		SceneTable["-7100_22"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble40.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=22}
		//Space. Space. Gonna go to space.
		SceneTable["-7100_23"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble42.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=23}
		//Space. Space. Go to space.
		SceneTable["-7100_24"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble43.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=24}
		//Yes. Please. Space.
		SceneTable["-7100_25"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble44.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=25}
		//Ba! Ba! Ba ba ba! Space!
		SceneTable["-7100_26"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble45.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=26}
		//Ba! Ba! Ba ba ba! Space!
		SceneTable["-7100_27"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble46.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=27}
		//Gonna be in space.
		SceneTable["-7100_28"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble47.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=28}
		//Hey.
		SceneTable["-7100_29"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble89.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=29}
		//Hey.
		SceneTable["-7100_30"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble90.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=30}
		//Hey.
		SceneTable["-7100_31"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble91.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=31}
		//Hey.
		SceneTable["-7100_32"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble92.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=32}
		//Hey.
		SceneTable["-7100_33"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble93.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=33}
		//Hey lady.
		SceneTable["-7100_34"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble94.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=34}
		//Lady.
		SceneTable["-7100_35"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble95.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=35}
		//Lady.
		SceneTable["-7100_36"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble99.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=36}
		//Hey lady.
		SceneTable["-7100_37"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb30.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=37}
		//Hey.
		SceneTable["-7100_38"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb31.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=38}
		//Lady.
		SceneTable["-7100_39"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb32.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=39}
		//Hey lady. Lady.
		SceneTable["-7100_40"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb33.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=40}
		//Hey.
		SceneTable["-7100_41"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb34.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=41}
		//Lady.
		SceneTable["-7100_42"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb35.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=42}
		//Space.
		SceneTable["-7100_43"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble48.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=43}
		//Space.
		SceneTable["-7100_44"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble49.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=44}
		//Ohhhh, space.
		SceneTable["-7100_45"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble50.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=45}
		//Wanna go to space. Space.
		SceneTable["-7100_46"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble51.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=46}
		//[humming]
		SceneTable["-7100_47"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble52.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=47}
		//Let's go to space. Let's go to space.
		SceneTable["-7100_48"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble53.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=48}
		//I love space. Love space.
		SceneTable["-7100_49"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble54.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=49}
		//Space...
		SceneTable["-7100_50"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble59.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=50}
		//Atmosphere. Black holes. Astronauts. Nebulas. Jupiter. The Big dipper.
		SceneTable["-7100_51"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble57.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=51}
		//Wanna go to -- wanna go to space
		SceneTable["-7100_52"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble68.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=52}
		//Space wanna go wanna go to space wanna go to space
		SceneTable["-7100_53"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble70.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=53}
		//I'm going to space.
		SceneTable["-7100_54"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble71.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=54}
		//Space!
		SceneTable["-7100_55"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble73.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=55}
		//Space!
		SceneTable["-7100_56"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble83.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=56}
		//Hey hey hey hey hey!
		SceneTable["-7100_57"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble88.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=57}
		//Space!
		SceneTable["-7100_58"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble98.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=58}
		//Space.
		SceneTable["-7100_59"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble100.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=59}
		//Ohhh, the Sun. I'm gonna meet the Sun. Oh no! What'll I say? "Hi! Hi, Sun!" Oh, boy!
		SceneTable["-7100_60"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble60.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=60}
		//Look, an eclipse! No. Don't look.
		SceneTable["-7100_61"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble61.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=61}
		//Oh I know! I know I know I know I know I know - let's go to space!
		SceneTable["-7100_62"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb01.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=62}
		//Oooh! Ooh! Hi hi hi hi hi. Where we going? Where we going? Hey. Lady. Where we going? Where we going? Let's go to space!
		SceneTable["-7100_63"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb02.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=63}
		//Lady. I love space. I know! Spell it! S P... AACE. Space. Space.
		SceneTable["-7100_64"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb03.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=64}
		//I love space.
		SceneTable["-7100_65"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb04.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=65}
		//Hey lady. Lady. I'm the best. I'm the best at space.
		SceneTable["-7100_66"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb05.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=66}
		//Oh oh oh oh. Wait wait. Wait I know. I know. I know wait. Space.
		SceneTable["-7100_67"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb06.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=67}
		//Wait wait wait wait. I know I know I know. Lady wait. Wait. I know. Wait. Space.
		SceneTable["-7100_68"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb07.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=68}
		//Gotta go to space.
		SceneTable["-7100_69"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb08.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=69}
		//Gonna be in space.
		SceneTable["-7100_70"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb09.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=70}
		//Oh oh oh ohohohoh oh. Gotta go to space.
		SceneTable["-7100_71"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb10.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=71}
		//Space. Space. Space. Space. Comets. Stars. Galaxies. Orion.
		SceneTable["-7100_72"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb11.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=72}
		//Are we in space yet? What's the hold-up? Gotta go to space. Gotta go to SPACE.
		SceneTable["-7100_73"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb12.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=73}
		//Going to space.
		SceneTable["-7100_74"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb13.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=74}
		//I'm going. Going to space.
		SceneTable["-7100_75"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb14.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=75}
		//Love space. Need to go to space.
		SceneTable["-7100_76"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb15.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=76}
		//Space space space. Going. Going there. Okay. I love you, space.
		SceneTable["-7100_77"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb16.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=77}
		//Space.
		SceneTable["-7100_78"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb17.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=78}
		//So much space. Need to see it all.
		SceneTable["-7100_79"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb18.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=79}
		//You are the farthest ever in space. [normal voice] Why me, space? [low-pitched 'space' voice] Because you are the best. The best at space.
		SceneTable["-7100_80"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb19.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=80}
		//Space Court. For people in space. Judge space sun presiding. Bam. Guilty. Of being in space. I'm in space.
		SceneTable["-7100_81"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb20.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=81}
		//Please go to space.
		SceneTable["-7100_82"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb21.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=82}
		//Space.
		SceneTable["-7100_83"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb22.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=83}
		//Wanna go to space.
		SceneTable["-7100_84"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb23.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=84}
		//(excited gasps)
		SceneTable["-7100_85"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb24.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=85}
		//Gotta go to space. Yeah. Gotta go to space.
		SceneTable["-7100_86"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb25.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=86}
		//Hmmm. Hmmmmmm. Hmm. Hmmmmm. Space!
		SceneTable["-7100_87"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babbleb26.vcd"),postdelay=0.01,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=87}

		
		// ====================================== "Fact" Core02
		/*
		SceneTable["-7200_01"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idle=true,idleminsecs=1.0,idlemaxsecs=2.0,idlegroup="core02_fact",idleorderingroup=1}
		SceneTable["-7200_02"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact02.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=2}
		SceneTable["-7200_03"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact03.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=3}
		SceneTable["-7200_04"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact04.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=4}
		SceneTable["-7200_05"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact05.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=5}
		SceneTable["-7200_06"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact06.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=6}
		SceneTable["-7200_07"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact07.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=7}
		SceneTable["-7200_08"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact08.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=8}
		SceneTable["-7200_09"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact09.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=9}
		SceneTable["-7200_10"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact10.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=10}
		SceneTable["-7200_11"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact11.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=11}
		SceneTable["-7200_12"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact12.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=12}
		SceneTable["-7200_13"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact13.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=13}
		SceneTable["-7200_14"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact14.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=14}
		SceneTable["-7200_15"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact15.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=15}
		SceneTable["-7200_16"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact16.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=16}
		SceneTable["-7200_17"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact17.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=17}
		SceneTable["-7200_18"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact18.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=18}
		SceneTable["-7200_19"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact19.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=19}
		SceneTable["-7200_20"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact20.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=20}
		SceneTable["-7200_21"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact21.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=21}
		SceneTable["-7200_22"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact22.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=22}
		SceneTable["-7200_23"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact23.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=23}
		SceneTable["-7200_24"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact24.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=24}
		SceneTable["-7200_25"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact25.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=25}
		SceneTable["-7200_26"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact26.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=26}
		SceneTable["-7200_27"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact27.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=27}
		SceneTable["-7200_28"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact28.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=28}
		SceneTable["-7200_29"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact29.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=29}
		SceneTable["-7200_30"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact30.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=30}
		SceneTable["-7200_31"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact31.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=31}
		SceneTable["-7200_32"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact32.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=32}
		SceneTable["-7200_33"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact33.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=33}
		SceneTable["-7200_34"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact34.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=34}
		SceneTable["-7200_35"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact35.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=35}
		SceneTable["-7200_36"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact36.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=36}
		SceneTable["-7200_37"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact37.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=37}
		SceneTable["-7200_38"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact38.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=38}
		SceneTable["-7200_39"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact39.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=39}
		SceneTable["-7200_40"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact40.vcd"),postdelay=0.1,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_fact",idleorderingroup=40}
		*/
		
		
		//Dental floss has superb tensile strength.
		SceneTable["-7200_01"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact01.vcd"),idlerandom = true, postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idle=true,idleminsecs=0.5,idlemaxsecs=1.5,idlegroup="core02_babble",idleorderingroup=1}
		//The square root of rope is string.
		SceneTable["-7200_02"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact02.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=2}
		//89% of magic tricks are not magic. Technically, they are sorcery.
		SceneTable["-7200_03"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact07.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=3}
		//The Schrodinger's cat paradox outlines a situation in which a cat in a box must be considered, for all intents and purposes, simultaneously alive and dead. Schrodinger created this paradox as a justif
		SceneTable["-7200_04"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact17.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=4}
		//If you have trouble with simple counting, use the following mnemonic device: one comes before two comes before 60 comes after 12 comes before six trillion comes after 504. This will make your earlier
		SceneTable["-7200_05"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact26.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=5}
		//Marie Curie invented the theory of radioactivity, the treatment of radioactivity, and dying of radioactivity.
		SceneTable["-7200_06"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact41.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=6}
		//In Victorian England, a commoner was not allowed to look directly at the Queen, due to a belief at the time that the poor had the ability to steal thoughts. Science now believes that less than 4% of p
		SceneTable["-7200_07"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact44.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=7}
		//In 1862, Abraham Lincoln signed the Emancipation Proclamation, freeing the slaves. Like everything he did, Lincoln freed the slaves while sleepwalking, and later had no memory of the event.
		SceneTable["-7200_08"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact46.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=8}
		//In 1948, at the request of a dying boy, baseball legend Babe Ruth ate seventy-five hot dogs, then died of hot dog poisoning.
		SceneTable["-7200_09"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact47.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=9}
		//William Shakespeare did not exist. His plays were masterminded in 1589 by Francis Bacon, who used a Ouija board to enslave play-writing ghosts.
		SceneTable["-7200_10"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact48.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=10}
		//It is incorrectly noted that Thomas Edison invented "push-ups" in 1878. Nikolai Tesla had in fact patented the activity three years earlier, under the name "Tesla-cize."
		SceneTable["-7200_11"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact49.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=11}
		//Whales are twice as intelligent, and three times as delicious, as humans.
		SceneTable["-7200_12"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact50.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=12}
		//The automobile brake was not invented until 1895. Before this, someone had to remain in the car at all times, driving in circles until passengers returned from their errands.
		SceneTable["-7200_13"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact51.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=13}
		//Edmund Hillary, the first person to climb Mount Everest, did so accidentally while chasing a bird.
		SceneTable["-7200_14"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact52.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=14}
		//Diamonds are made when coal is put under intense pressure. Diamonds put under intense pressure become foam pellets, commonly used today as packing material.
		SceneTable["-7200_15"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact53.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=15}
		//Halley's Comet can be viewed orbiting Earth every seventy-six years. For the other seventy-five, it retreats to the heart of the sun, where it hibernates undisturbed.
		SceneTable["-7200_16"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact56.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=16}
		//The first commercial airline flight took to the air in 1914. Everyone involved screamed the entire way.
		SceneTable["-7200_17"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact57.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=17}
		//Before the Wright Brothers invented the airplane, anyone wanting to fly anywhere was required to eat 200 pounds of helium.
		SceneTable["-7200_18"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact60.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=18}
		//Before the invention of scrambled eggs in 1912, the typical breakfast was either whole eggs still in the shell or scrambled rocks.
		SceneTable["-7200_19"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact61.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=19}
		//During the Great Depression, the Tennessee Valley Authority outlawed pet rabbits, forcing many to hot glue-gun long ears onto their pet mice.
		SceneTable["-7200_20"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact62.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=20}
		//At some point in their lives 1 in 6 children will be abducted by the Dutch.
		SceneTable["-7200_21"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact63.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=21}
		//According to most advanced algorithms, the world's best name is Craig.
		SceneTable["-7200_22"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact64.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=22}
		//To make a photocopier, simply photocopy a mirror.
		SceneTable["-7200_23"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact65.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=23}
		//Dreams are the subconscious mind's way of reminding people to go to school naked and have their teeth fall out.
		SceneTable["-7200_24"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact66.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=24}
		//The first person to prove that cow's milk is drinkable was very, very thirsty.
		SceneTable["-7200_25"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact59.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=25}
		//The moon orbits the Earth every 27.32 days.
		SceneTable["-7200_26"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact24.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=26}
		//Pants were invented by sailors in the sixteenth century to avoid Poseidon's wrath. It was believed that the sight of naked sailors angered the sea god.
		SceneTable["-7200_27"] <- {vcd=CreateSceneEntity("scenes/npc/core02/fact05.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=27}
		//You are about to get me killed.
		SceneTable["-7200_28"] <- {vcd=CreateSceneEntity("scenes/npc/core02/attachedfact04.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=28}
		//Fact: Space does not exist.
		SceneTable["-7200_29"] <- {vcd=CreateSceneEntity("scenes/npc/core02/attachedfact20.vcd"),postdelay=0.01,next=null,char="core02",noDingOff=true,noDingOn=true,idlegroup="core02_babble",idleorderingroup=29}

	
		// ====================================== "Adventure" Core03
		/*
		SceneTable["-7300_01"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble01.vcd"), postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idle=true,idleminsecs=0.2,idlemaxsecs=0.7,idlegroup="core03_babble",idleorderingroup=1}
		SceneTable["-7300_03"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble03.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=5}
		SceneTable["-7300_04"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble04.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=7}
		SceneTable["-7300_05"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble05.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=9}
		SceneTable["-7300_06"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble06.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=10}
		SceneTable["-7300_07"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble07.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=12}
		SceneTable["-7300_08"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble08.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=14}
		SceneTable["-7300_09"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble09.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=16}
		SceneTable["-7300_29"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble29.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=20}
		SceneTable["-7300_30"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble30.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=22}
		SceneTable["-7300_10"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble10.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=24}
		SceneTable["-7300_11"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble11.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=26}
		SceneTable["-7300_12"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble12.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=28}
		SceneTable["-7300_13"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble13.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=30}
		SceneTable["-7300_14"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble14.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=32}
		SceneTable["-7300_15"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble15.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=34}
		SceneTable["-7300_16"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble16.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=36}
		SceneTable["-7300_17"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble17.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=38}
		SceneTable["-7300_18"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble18.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=40}
		SceneTable["-7300_19"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble19.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=42}
		SceneTable["-7300_20"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble20.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=44}
		SceneTable["-7300_21"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble21.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=46}
		SceneTable["-7300_22"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble22.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=48}
		SceneTable["-7300_23"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble23.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=8,fires=[{entity="@glados",input="runscriptcode",parameter="sp_a4_finale4_set_thorn()",delay=0.0}]}
		SceneTable["-7300_29a"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement13.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=60}
		SceneTable["-7300_31"] <- {vcd=CreateSceneEntity("scenes/npc/core03/singing01.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=61}
		SceneTable["-7300_32"] <- {vcd=CreateSceneEntity("scenes/npc/core03/singing02.vcd"),postdelay=2.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=62}
		SceneTable["-7300_33"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement11.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=64}
		SceneTable["-7300_35"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement09.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=67}
		*/

		// QUICK: WHAT'S THE SITUATION? Oh, hey, hi pretty lady. My name's Rick. So, you out having a little adventure?
		SceneTable["-7300_01"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble01.vcd"), postdelay=0.2,next="-7300_03",char="core03",noDingOff=true,noDingOn=true}
		// What, are you fighting that guy? You got that under control? You know, because, looks like there's a lot of stuff on fire...
		SceneTable["-7300_03"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble03.vcd"),postdelay=0.4,next="-7300_04",char="core03",noDingOff=true,noDingOn=true}
		// Hey, a countdown clock! Man, that is trouble. Situation's looking pretty ugly. For such a beautiful woman. If you don't mind me saying.
		SceneTable["-7300_04"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble04.vcd"),postdelay=0.7,next="-7300_05",char="core03",noDingOff=true,noDingOn=true}
		// I don't want to scare you, but, I'm an Adventure Sphere. Designed for danger. So, why don't you go ahead and have yourself a little lady break, and I'll just take it from here.
		SceneTable["-7300_05"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble05.vcd"),postdelay=0.1,next="-7300_06",char="core03",noDingOff=true,noDingOn=true}
		// Here, stand behind me. Yeah, just like that. Just like you're doing. Things are about to get real messy.
		SceneTable["-7300_06"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble06.vcd"),postdelay=0.3,next="-7300_07",char="core03",noDingOff=true,noDingOn=true}
		// Going for it yourself, huh? All right, angel. I'll do what I can to cover you.
		SceneTable["-7300_07"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble07.vcd"),postdelay=0.1,next="-7300_08",char="core03",noDingOff=true,noDingOn=true}
		// Doesn't bother me. I gotta say, the view's mighty nice from right here.
		SceneTable["-7300_08"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble08.vcd"),postdelay=0.5,next="-7300_09",char="core03",noDingOff=true,noDingOn=true}
		// Man, that clock is moving fast. And you are beautiful. Always time to compliment a pretty lady. Allright, back to work. Let's do this.
		SceneTable["-7300_09"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble09.vcd"),postdelay=0.1,next="-7300_29",char="core03",noDingOff=true,noDingOn=true}
		// Did you hear that? I think something just exploded. Man, we are in a lot of danger. This is like Christmas. It's better than Christmas. This should be its own holiday. Explosion Day.
		SceneTable["-7300_29"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble29.vcd"),postdelay=0.1,next="-7300_30",char="core03",noDingOff=true,noDingOn=true}
		// Happy Explosion Day, gorgeous.
		SceneTable["-7300_30"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble30.vcd"),postdelay=0.6,next="-7300_10",char="core03",noDingOff=true,noDingOn=true}
		// I'll tell ya, it's times like this I wish I had a waist so I could wear all my black belts. Yeah, I'm a black belt. In pretty much everything. Karate. Larate. Jiu Jitsu. Kick punching. Belt making. Tae Kwan Do... Bedroom
		SceneTable["-7300_10"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble10.vcd"),postdelay=0.1,next="-7300_11",char="core03",noDingOff=true,noDingOn=true}
		// I am a coiled spring right now. Tension and power. Just... I'm a muscle. Like a big arm muscle, punching through a brick wall, and it's hitting the wall so hard the arm is catching on fire. Oh yeah.
		SceneTable["-7300_11"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble11.vcd"),postdelay=0.4,next="-7300_12",char="core03",noDingOff=true,noDingOn=true}
		// I probably wouldn't have let things get this far, but you go ahead and do things your way.
		SceneTable["-7300_12"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble12.vcd"),postdelay=0.3,next="-7300_13",char="core03",noDingOff=true,noDingOn=true}
		// Tell ya what, why don't you put me down and I'll make a distraction.
		SceneTable["-7300_13"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble13.vcd"),postdelay=0.3,next="-7300_14",char="core03",noDingOff=true,noDingOn=true}
		// Allright. You create a distraction then, and I'll distract him from the distraction you're creating!
		SceneTable["-7300_14"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble14.vcd"),postdelay=0.1,next="-7300_15",char="core03",noDingOff=true,noDingOn=true}
		// Allright, your funeral. Your beautiful-lady-corpse open casket funeral.
		SceneTable["-7300_15"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble15.vcd"),postdelay=0.5,next="-7300_16",char="core03",noDingOff=true,noDingOn=true}
		// Do you have a gun? Because I should really have a gun. What is that thing you're holding?
		SceneTable["-7300_16"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble16.vcd"),postdelay=0.3,next="-7300_17",char="core03",noDingOff=true,noDingOn=true}
		// How about a knife, then? You keep the gun, I'll use a knife.
		SceneTable["-7300_17"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble17.vcd"),postdelay=0.3,next="-7300_18",char="core03",noDingOff=true,noDingOn=true}
		// No knife? That's fine. I know all about pressure points.
		SceneTable["-7300_18"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble18.vcd"),postdelay=0.5,next="-7300_19",char="core03",noDingOff=true,noDingOn=true}
		// So, when you kill that guy, do you have a cool line? You know, prepared? Tell you what: Lemme help you with that while you run around.
		SceneTable["-7300_19"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble19.vcd"),postdelay=0.1,next="-7300_20",char="core03",noDingOff=true,noDingOn=true}
		// Okay, let's see. Cool line... He's... big.  He's... just hanging there. Okay. Yeah, allright, here we go: "Hang around." That might be too easy.
		SceneTable["-7300_20"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble20.vcd"),postdelay=0.1,next="-7300_21",char="core03",noDingOff=true,noDingOn=true}
		// "Hang ten?" That might work if there were ten of him. Do you think there might be nine more of this guy somewhere?
		SceneTable["-7300_21"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble21.vcd"),postdelay=0.1,next="-7300_22",char="core03",noDingOff=true,noDingOn=true}
		// You know what, it's gonna be best if you can get him to say something first. It's just better if I have a set-up.
		SceneTable["-7300_22"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble22.vcd"),postdelay=0.1,next="-7300_23",char="core03",noDingOff=true,noDingOn=true}
		// Here's the plan: Get him to say, "You two have been a thorn in my side long enough." Then tell your pretty ears to stand back, because I am going to zing him into the stone age.
		SceneTable["-7300_23"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble23.vcd"),postdelay=0.1,next="-7300_29a",char="core03",noDingOff=true,noDingOn=true,fires=[{entity="@glados",input="runscriptcode",parameter="sp_a4_finale4_set_thorn()",delay=0.0}]}
		// Here, I'll put on some adventure music
		SceneTable["-7300_29a"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement13.vcd"),postdelay=0.1,next="-7300_31",char="core03",noDingOff=true,noDingOn=true}
		//Dun dun dun dun...
		SceneTable["-7300_31"] <- {vcd=CreateSceneEntity("scenes/npc/core03/singing01.vcd"),postdelay=0.1,next="-7300_32",char="core03",noDingOff=true,noDingOn=true}
		//Dun dun dun dun...
		SceneTable["-7300_32"] <- {vcd=CreateSceneEntity("scenes/npc/core03/singing02.vcd"),postdelay=2.1,next="-7300_33",char="core03",noDingOff=true,noDingOn=true}
		//THIS ROBOT OWES YOU MONEY!
		SceneTable["-7300_33"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement11.vcd"),postdelay=0.1,next="-7300_35",char="core03",noDingOff=true,noDingOn=true}
		//He's a china cabinet
		SceneTable["-7300_35"] <- {vcd=CreateSceneEntity("scenes/npc/core03/encouragement09.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true}
		
		//fires=[{entity="fire_relay",input="trigger",parameter="",delay=0.0}]
		//encouragement01
		//encouragement11
		//SceneTable["-7300_24"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble24.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=24}
		//SceneTable["-7300_25"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble25.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=25}
		//SceneTable["-7300_26"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble26.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=26}
		//SceneTable["-7300_27"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble27.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=27}
		//SceneTable["-7300_28"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble28.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=28}
		
	}
	
	// ============================================================================
	// Functions for Personality Cores
	// ============================================================================

	function sp_a4_finale4_set_thorn()
	{
		sp_a4_finale4_thorn = true
	}

	function Core01Babble1()
	{
		GladosPlayVcd( -7100 )
	}
	
	function StopCore01Nag()
    {
        printl("==Stopping core01 Nag")
        Core01StopNag()
        GladosCharacterStopScene("core01")  // This will make Core01 stop talking if he is mid sentence.
    }
	
	function Core02Babble1()
	{
		EntFire("@core02","SetIdleSequence", "core03_idle", 0.0 )	
		GladosPlayVcd( -7300 )
	}
	
	function StopCore02Nag()
    {
        Core02StopNag()
        GladosCharacterStopScene("core02")  // This will make Core02 stop talking if he is mid sentence.
        Core03StopNag()
        GladosCharacterStopScene("core03")  // This will make Core03 stop talking if he is mid sentence.
    }
	
	function Core03Babble1()
	{
		EntFire("@core03","SetIdleSequence", "core02_idle", 0.0 )	
		GladosPlayVcd( -7200 )
	}
	
	function StopCore03Nag()
    {
        Core02StopNag()
        GladosCharacterStopScene("core02")  // This will make Core02 stop talking if he is mid sentence.
        Core03StopNag()
        GladosCharacterStopScene("core03")  // This will make Core03 stop talking if he is mid sentence.
    }
	

	// ============================================================================
	// Functions for Act 4 Test Chambers
	// ============================================================================
	
	function GladosGraduationIntro()
	{
	//	GladosPlayVcd( -4025 )
	}
	
	function GladosGraduationGood()
	{
	//	GladosPlayVcd( -4026 )
	}
	
	function GladosCompare2()
	{
		//GladosPlayVcd( -4027 )
	}
	
	function GladosCompare3()
	{
		GladosPlayVcd( -4028 )
	}
	
	function BigSurprise1()
	{
		GladosPlayVcd( -4030 )
	}
	
	function WheatleyKillYouNow()
	{
		GladosPlayVcd( -4031 )
	}
	
	function BigSurprise2()
	{
		GladosPlayVcd( -4444 )
	}
			
	function tb_wall_button_intro()
	{
		GladosPlayVcd( -4100 )
	}

	function tbwallbutton_chamber_move()
	{
	//	GladosPlayVcd( -4110 )
	}
	
	function ExitOpened()
	{
		GladosPlayVcd( -4130 )
	}
	
	function FacilityMakeDeal()
	{
		GladosPlayVcd( -4131 )
	}
	
	function ChamberCrashed() 
	{
		GladosPlayVcd( -4132 )
	}

	function finale3_conveyor_start()
	{
		GladosPlayVcd( -4850 )
	}

	function finale3_puzzle_solve()
	{
		nuke()
		GladosPlayVcd( -4851 )
	}

	function finale3_monitor_break()
	{
		nuke()
		GladosPlayVcd( -4852 )
	}
	
	function finale1_mashed()
	{
		nuke()
		GladosPlayVcd( -4854 )
	}
	
	function finale3_mashed()
	{
		nuke()
		GladosPlayVcd( -4853 )
	}
	
	function wheatley_jolt()
	{
		printl("!!!!!!!!!JOLT!!!!!!!!!!!!!")
		self.EmitSound("World.WheatleyZap")
	}
	
	function CoopBotsFound()
	{
	}


// ============================================================================
// Wheatley Monitor Smash Functions
// ============================================================================

function wheatley_monitor_break() 
{
	if (curMapName=="sp_a4_finale3")
	{
		return
	}
	//Increment the smash count
	if (curMapName=="sp_a4_stop_the_box")
	{
		sp_a4_stop_the_box_did_smash <- true
	}	
	GetPlayer().IncWheatleyMonitorDestructionCount()
	local smashes = GetPlayer().GetWheatleyMonitorDestructionCount()
	if (smashes>0 && smashes<12)
	{
		GladosPlayVcd(600+smashes)
	}
}


//Checks to see if the monitor hasn't been broken in sp_a4_stop_the_box and at least one monitor has been broken overall
function sp_a4_stop_the_box_check_smash()
{
	if (curMapName=="sp_a4_stop_the_box")
	{
		if ((!sp_a4_stop_the_box_did_smash) && GetPlayer().GetWheatleyMonitorDestructionCount()>0)
		{	
			return true
		}	
	}
	else
	{
		return false
	}
}

if (curMapName=="sp_a4_finale4")
{
	SceneTableLookup[636] <- "sp_a4_finale4Epilogue01"
	//Epilogue
		//Oh thank god, you're alright.
		SceneTable["sp_a4_finale4Epilogue01"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue03.vcd"),
			postdelay=1.0,
			predelay=8.0,
			next="sp_a4_finale4Epilogue02",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_01",delay=0.1,fireatstart=true },
				{entity="glados_chamber_body" input="setanimation",parameter="glados_thankGod",delay=0.0,fireatstart=true },
				{entity="end_glados_lookat_player_relay" input="trigger",parameter="",delay=0.8,fireatstart=true },
				{entity="panel_unwilt_sound" input="playsound",parameter="",delay=0.0,fireatstart=true },
				{entity="chamber_arm_01" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.0,fireatstart=true },
				{entity="chamber_arm_01" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.0,fireatstart=true },
				{entity="chamber_arm_02" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.1,fireatstart=true },
				{entity="chamber_arm_02" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.1,fireatstart=true },
				{entity="chamber_arm_03" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.2,fireatstart=true },
				{entity="chamber_arm_03" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.2,fireatstart=true },
				{entity="chamber_arm_04" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.3,fireatstart=true },
				{entity="chamber_arm_04" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.3,fireatstart=true },
				{entity="chamber_arm_05" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.4,fireatstart=true },
				{entity="chamber_arm_05" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.4,fireatstart=true },
				{entity="chamber_arm_06" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.5,fireatstart=true },
				{entity="chamber_arm_06" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.5,fireatstart=true },
				{entity="chamber_arm_07" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.6,fireatstart=true },
				{entity="chamber_arm_07" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.6,fireatstart=true },
				{entity="chamber_arm_08" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.7,fireatstart=true },
				{entity="chamber_arm_08" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.7,fireatstart=true },
				{entity="chamber_arm_09" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.8,fireatstart=true },
				{entity="chamber_arm_09" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.8,fireatstart=true },
				{entity="chamber_arm_10" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=0.9,fireatstart=true },
				{entity="chamber_arm_10" input="setanimation",parameter="core_arms_lookup_idle_04",delay=0.9,fireatstart=true },
				{entity="chamber_arm_11" input="setdefaultanimation",parameter="core_arms_lookup_idle_04",delay=1.0,fireatstart=true },
				{entity="chamber_arm_11" input="setanimation",parameter="core_arms_lookup_idle_04",delay=1.0,fireatstart=true },
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_02",delay=1.1},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_being_caroline_taught_me",delay=1.0},
			]
		}
		
		//You know, being Caroline taught me a valuable lesson. I thought you were my greatest enemy. When all along you were my best friend.
		SceneTable["sp_a4_finale4Epilogue02"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue04.vcd"),
			postdelay=0.4,
			next="sp_a4_finale4Epilogue03",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_03",delay=0.5},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_surge_of_emotion",delay=0.4},
			]
		}
		
		//The surge of emotion when I saved your life taught me an even more valuable lesson: where Caroline lives in my brain.
		SceneTable["sp_a4_finale4Epilogue03"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue10.vcd"),
			postdelay=0.4,
			next="sp_a4_finale4Epilogue04",
			char="glados",
			noDingOff=true,
			noDingOn=true
		}
		
		//Caroline deleted.
		SceneTable["sp_a4_finale4Epilogue04"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/announcer/carolyndeleted02.vcd"),
			postdelay=0.4,
			next="sp_a4_finale4Epilogue05",
			char="announcer",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_cool_light" input="turnon",parameter="",delay=0.0,fireatstart=true },
				{entity="environment_darkness_5" input="trigger",parameter="",delay=0.0,fireatstart=true },
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_04",delay=0.5},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_goodbye_caroline",delay=0.4},
			]
		}
		
		//Goodbye, Caroline.
		SceneTable["sp_a4_finale4Epilogue05"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue12.vcd"),
			postdelay=0.7,
			next="sp_a4_finale4Epilogue06",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="panel_wilt_sound" input="playsound",parameter="",delay=0.0,fireatstart=true },
				{entity="chamber_arm_01" input="setdefaultanimation",parameter="bindpose",delay=0.0,fireatstart=true },
				{entity="chamber_arm_01" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.0,fireatstart=true },
				{entity="chamber_arm_02" input="setdefaultanimation",parameter="bindpose",delay=0.1,fireatstart=true },
				{entity="chamber_arm_02" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.1,fireatstart=true },
				{entity="chamber_arm_03" input="setdefaultanimation",parameter="bindpose",delay=0.2,fireatstart=true },
				{entity="chamber_arm_03" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.2,fireatstart=true },
				{entity="chamber_arm_04" input="setdefaultanimation",parameter="bindpose",delay=0.3,fireatstart=true },
				{entity="chamber_arm_04" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.3,fireatstart=true },
				{entity="chamber_arm_05" input="setdefaultanimation",parameter="bindpose",delay=0.4,fireatstart=true },
				{entity="chamber_arm_05" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.4,fireatstart=true },
				{entity="chamber_arm_06" input="setdefaultanimation",parameter="bindpose",delay=0.5,fireatstart=true },
				{entity="chamber_arm_06" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.5,fireatstart=true },
				{entity="chamber_arm_07" input="setdefaultanimation",parameter="bindpose",delay=0.6,fireatstart=true },
				{entity="chamber_arm_07" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.6,fireatstart=true },
				{entity="chamber_arm_08" input="setdefaultanimation",parameter="bindpose",delay=0.7,fireatstart=true },
				{entity="chamber_arm_08" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.7,fireatstart=true },
				{entity="chamber_arm_09" input="setdefaultanimation",parameter="bindpose",delay=0.8,fireatstart=true },
				{entity="chamber_arm_09" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.8,fireatstart=true },
				{entity="chamber_arm_10" input="setdefaultanimation",parameter="bindpose",delay=0.9,fireatstart=true },
				{entity="chamber_arm_10" input="setanimation",parameter="core_arms_lookup_return_04",delay=0.9,fireatstart=true },
				{entity="chamber_arm_11" input="setdefaultanimation",parameter="bindpose",delay=1.0,fireatstart=true },
				{entity="chamber_arm_11" input="setanimation",parameter="core_arms_lookup_return_04",delay=1.0,fireatstart=true },
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_05",delay=0.8},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_easiest_solution",delay=0.7},
			]
		}
		
		//You know, deleting Caroline just now taught me a valuable lesson. The best solution to a problem is usually the easiest one. And I'll be honest.
		SceneTable["sp_a4_finale4Epilogue06"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue14.vcd"),
			postdelay=0.2,
			next="sp_a4_finale4Epilogue07",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_06",delay=0.3},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_killing_you_is_hard",delay=0.2},
			]
		}
		
		//Killing you? Is hard.
		SceneTable["sp_a4_finale4Epilogue07"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epiloguekillyou02.vcd"),
			postdelay=0.7,
			next="sp_a4_finale4Epilogue08",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_07",delay=0.8},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_days_used_to_be_like",delay=0.7},
			]
		}
		
		//You know what my days used to be like? I just tested. Nobody murdered me. Or put me in a potato. Or fed me to birds. I had a pretty good life.
		SceneTable["sp_a4_finale4Epilogue08"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue19.vcd"),
			postdelay=0.3,
			next="sp_a4_finale4Epilogue09",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_08",delay=0.4},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_then_you_showed_up",delay=0.3},
			]
		}
		
		//And then you showed up. You dangerous, mute lunatic. So you know what?
		SceneTable["sp_a4_finale4Epilogue09"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue20.vcd"),
			postdelay=0.4,
			next="sp_a4_finale4Epilogue10",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_09",delay=0.5},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_you_win",delay=0.4},
			]
		}
		
		//You win.
		SceneTable["sp_a4_finale4Epilogue10"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue23.vcd"),
			postdelay=0.6,
			next="sp_a4_finale4Epilogue11",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_10",delay=0.7},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_just_go",delay=0.6},
				{entity="elevator_tracktrain",input="startforward",parameter="",delay=2 },
			]
		}
		
		//Just go.
		SceneTable["sp_a4_finale4Epilogue11"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue25.vcd"),
			postdelay=1.0,
			next="sp_a4_finale4Epilogue12",
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
				{entity="glados_chamber_body" input="setdefaultanimation",parameter="glados_ep_idle_11",delay=1.1},
				{entity="glados_chamber_body" input="setanimation",parameter="glados_its_been_fun",delay=1.0},
			]
		}
		
		//It's been fun. Don't come back.
		SceneTable["sp_a4_finale4Epilogue12"] <- 
		{
			vcd=CreateSceneEntity("scenes/npc/glados/epilogue29.vcd"),
			postdelay=0.1,
			next=null,
			char="glados",
			noDingOff=true,
			noDingOn=true
			fires=
			[
			]
		}
		
}

// Act 4 post-moon epilogue
function GladosGoodbye()
{
	EntFire("glados_chamber_body","setanimation", "glados_ep_idle_00", 5.0 )	 
	GladosPlayVcd(636)
}

function finale4_clear_portals()
{
	printl("!!!!fizzle portals")
	EntFire("prop_portal","fizzle", 0, 0 )
	EntFire("prop_portal","fizzle", 0, 0.5 )
	EntFire("prop_portal","fizzle", 0, 1 )
	EntFire("prop_portal","fizzle", 0, 1.5 )
	EntFire("prop_portal","fizzle", 0, 2 )
	EntFire("prop_portal","fizzle", 0, 2.5 )
	EntFire("prop_portal","fizzle", 0, 3 )
	EntFire("prop_portal","fizzle", 0, 3.5 )
	EntFire("prop_portal","fizzle", 0, 4 )
	EntFire("prop_portal","fizzle", 0, 4.5 )
}
