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


// ======================================
// sp_a4_tb_trust_drop
// ======================================

SceneTableLookup[-4400] <- "-4400_01"	// Made this test myself. Out of some smaller tests I found. Jammed ‘em together. Buttons. Funnels. Bottomless pits. It’s got it all.
										// I was hoping to shoot rock salt at you while you tested. See if that helped. You’ll know I cracked it when… well, when rock salt hits you, I imagine.
SceneTableLookup[-4401] <- "-4401_01"	// Alright, this is taking too long. I’ll just tell you how to solve the test. You see that button up there? You just need to… need to… HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!
										// (gasping for breath, spitting) Solve it yourself. Take your time. 
										// [gasping for breath, spitting] Right, new plan. You solve the tests. I’ll watch you quietly. Vomiting. Quietly vomiting.
SceneTableLookup[-4402] <- "-4402_01"	// [gasping for breath, spitting] Ugh. Okay. Okay, good.
										// GLADOS talks to EVIL WHEATLEY about a plate of rancid clams.
										// HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!							

// ======================================
// sp_a4_tb_wall_button
// ======================================

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

// ======================================
// sp_a4_tb_polarity
// ======================================

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

// ======================================
// sp_a4_laser_catapult
// ======================================

										
SceneTableLookup[-4417] <- "-4417_01"	// Had a brainwave. I’m going to tape you solving these, and then watch ten at once—get a more concentrated burst of science. Oh, on a related note: I’m going to need you to solve these ten times as fast.
										// Anyway, just give me a wave before you solve this one, alright? Don’t want to spoil the ending for when I watch it later.
SceneTableLookup[-4418] <- "-4418_01"	// You just solved it, didn’t you? I—told you to tell me before you… NNNNGH! Why are making this so HARD for me?
										
// ======================================
// sp_a4_laser_platform
// ======================================

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
	
// ======================================
// sp_a4_jump_polarity
// ======================================
	
SceneTableLookup[-4420] <- "-4420_01" 	// Sorry! Sorry. My fault. 
SceneTableLookup[-4421] <- "-4421_01" 	// Ohhh, you solved it. Oh. Good. Good for you… 

// ======================================
// sp_a4_speed_tb_catch
// ======================================
	
SceneTableLookup[-4448] <- "-4448_01" 	// Oh! Good. You're alive. Just getting a test ready... For you. Who else would I be doing it for? No one.
SceneTableLookup[-4449] <- "-4449_01" 	// After you told me to turn the beam off, I thought I'd lost you. Went poking around for test subjects. No luck there. Still all dead. 
 	
// ****************************************************************************************************
// Finale scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4800] <- "-4800_02" // Alright. There's only two more chambers until your BIG SURPRISE.
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
	SceneTableLookup[-4852] <- "-4852_01" 	// Oh! Oh! I'll take that as a no.Not choosing the death trap. Not dying.
	SceneTableLookup[-4853] <- "-4853_01" 	// Oh! Good! I did not think that was going to work.


// ****************************************************************************************************
// Boss Battle scenetable								  
// ****************************************************************************************************

SceneTableLookup[-4900] <- "-4900_01" // These are corrupted cores

SceneTableLookup[-4901] <- "-4901_01" // Plug me on to this socket

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
// Wheatley Recapture
// ****************************************************************************************************
	
	if (curMapName=="sp_a3_end")
	{

	// ====================================== Paradox Inception
	
		// Wait. I know how we can BEAT him.
		SceneTable["-4000_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradoxinception02.vcd") 
			char = "glados"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4000_02"
			noDingOff = true
			noDingOn = true
		}
		
		// Paradoxes. No AI can resist thinking about them. 
		SceneTable["-4000_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradoxinception03.vcd") 
			char = "glados"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4000_03"
			noDingOff = true
			noDingOn = true
		}
	
		// If you can get me in FRONT of him, I’ll hit him with every paradox I know.
		SceneTable["-4000_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradoxinception04.vcd") 
			char = "glados"
			postdelay = 0.5 
			predelay = 0.0
			next = "-4000_04"
			noDingOff = true
			noDingOn = true
		}
	
		// As long as I don’t listen to what I’m saying, I SHOULD be okay.
		SceneTable["-4000_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradoxinception06.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4000_05"
			noDingOff = true
			noDingOn = true
		}
		
		// Probably.
		SceneTable["-4000_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradoxinception07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
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
	
		// Oh. He left. 
		SceneTable["-4005_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_recaptureleadin04.vcd")  
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4005_02"
			noDingOff = true
			noDingOn = true
		}
	
	
		// Maybe he'll come back if we solve his puzzle for him.
		SceneTable["-4005_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_recaptureleadin05.vcd") 
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
		}	
	
		// Hey! Moron!
		SceneTable["-4007_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradox01.vcd") 
			char = "glados"
			postdelay = 0.4 
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
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox03.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4007_05"
			noDingOff = true
			noDingOn = true
		}	
	
		// "This sentence... is false."
		SceneTable["-4007_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradox02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_06"
			noDingOff = true
			noDingOn = true
			//talkover = true
		}	
		
		// Don't think about it don't think about it don't think about it...
		SceneTable["-4007_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_paradox03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_07"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="bot_selfdestruct_relay",input="Trigger",parameter="",delay=0.0,fireatstart=true},
			]			
		}	
	
		// Hm. "TRUE".
		SceneTable["-4007_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox04.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4007_07a"
			noDingOff = true
			noDingOn = true
		}
		
		// That was easy.
		SceneTable["-4007_07a" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox05.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4007_07b"
			noDingOff = true
			noDingOn = true
		}
		
		// I'll be honest, though: I might have heard that before. Sort of cheating.
		SceneTable["-4007_07b" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox06.vcd") 
			char = "wheatley"
			postdelay = -2.0 
			predelay = 0.0
			next = "-4007_09"
			noDingOff = true
			noDingOn = true
		}
		
		
	/*	// Alright. That DIDN'T work. Let me try reasoning with him.
		SceneTable["-4007_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_intro01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.8
			next = "-4007_09"
			noDingOff = true
			noDingOn = true
		}
	*/
	
		// Let me BACK in my BODY before you BLOW US ALL UP, you MORON!
		SceneTable["-4007_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_recapture01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.4
			next = "-4007_10"
			noDingOff = true
			noDingOn = true
		}
	
		// Ah. FALSE.
		SceneTable["-4007_10" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.4
			next = "-4007_11"
			noDingOff = true
			noDingOn = true
		}
		
		// WARNING
		SceneTable["-4007_11" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/recapture01.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_12"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_warning_relay",input="Trigger",parameter="",delay=0.0 },
			]
		}
		
		// Hold on! Ugh, I thought I fixed that.
		SceneTable["-4007_12" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_13"
			noDingOff = true
			noDingOn = true
		}
		
		// WARNING
		SceneTable["-4007_13" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/recapture02.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_14"
			noDingOff = true
			noDingOn = true
		}
		
		// There. Fixed.
		SceneTable["-4007_14" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox09.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4007_15"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_close_relay",input="Trigger",parameter="",delay=1.0 },
				{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=3.0 },
			]
		}
		
		// Wow, it is GREAT seeing you guys again. It turns out I'm a little short on test subjects right now. So this works out PERFECT.
		SceneTable["-4007_15" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox10.vcd") 
			char = "wheatley"
			postdelay = 0.8 
			predelay = 0.6
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="floor_gate_close_relay",input="Trigger",parameter="",delay=0.0 },
//				{entity="@glados",input="RunScriptCode",parameter="RecaptureProceed()",delay=3.8 },
				{entity="wheatley_proceed_relay",input="Trigger",parameter="",delay=3.8 },
			]
		}
	
		// ====================================== Proceed to Next Chamber Nag
		
        // Alright, get moving.
         SceneTable["-4008_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_paradox11.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=10.0,idlemaxsecs=20.0,idlegroup="proceednag",idleorderingroup=1}
         SceneTable["-4008_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_leave_nags01.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=2}
         SceneTable["-4008_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_leave_nags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=3}
         SceneTable["-4008_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_leave_nags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="proceednag",idleorderingroup=4}

		
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
	
		// Ah yes. That takes me back.
		SceneTable["-4009_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_scratchingtheitch03.vcd") 
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
			postdelay = -4.0 
			predelay = 0.0
			next = "-4009_07"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="wheatley_close_relay",input="Trigger",parameter="",delay=1.0,fireatstart=true},
				{entity="wheatley_wide_relay",input="Trigger",parameter="",delay=4.0,fireatstart=true},
			]			
		}	
		
		// Core overheating.
		SceneTable["-4009_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/announcer/recapture03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = "-4009_08"
			noDingOff = true
			noDingOn = true
			talkover = true
			fires=
			[
				{entity="wheatley_leave_relay",input="Trigger",parameter="",delay=0.0,fireatstart=true},
				{entity="wheatley_return_relay",input="Trigger",parameter="",delay=4.0,fireatstart=true},
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
		}	
	
		// I think we're in trouble.
		SceneTable["-4009_09" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_scratchingtheitch04.vcd") 
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
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="test1_end_counter",input="Add",parameter="1",delay=0.0},
			]	
		}	

		// ====================================== First Test Start Nags
		
        // Go on.
         SceneTable["-4021_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_moat04.vcd"), postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="test1hints",idleorderingroup=1}
		 SceneTable["-4021_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test_solve_nags02.vcd"), postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="test1hints",idleorderingroup=2}
		 SceneTable["-4021_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test_solve_nags03.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=3}
         SceneTable["-4021_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_first_test_solve_nags04.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=4}
         SceneTable["-4021_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags06.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=5}
		 SceneTable["-4021_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags07.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=6}
         SceneTable["-4021_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags08.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=7}
		 SceneTable["-4021_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags09.vcd"),postdelay=1.4,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=8}
		 SceneTable["-4021_09"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags10.vcd"),postdelay=0.0,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1hints",idleorderingroup=9}
		 
		// ====================================== First Test Finish Nags
		 
		 // Hold on.  You've still got to finish the test
		 SceneTable["-4022_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_notdone01.vcd"), postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=8.0,idlemaxsecs=12.0,idlegroup="test1finishhints",idleorderingroup=1}		 
         SceneTable["-4022_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_intro_notdone05.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=2}
 		 SceneTable["-4022_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags06.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=3}
		 SceneTable["-4022_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags07.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test1finishhints",idleorderingroup=4}
		 
		// ====================================== First Test Redo
		
	
		// Here's an idea, since making tests is difficult--why don't you just keep solving THIS test. And I can just... watch you solve it. Yes. That sounds much easier.
		SceneTable["-4012_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_big_idea02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	

		// ====================================== Second Test Nags
		
        // Here we go.  Now do it again.
        SceneTable["-4019_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="test2hints",idleorderingroup=1}
        SceneTable["-4019_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags01.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=2}
        SceneTable["-4019_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=3}
		SceneTable["-4019_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=4}
		SceneTable["-4019_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=5}
		SceneTable["-4019_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve_nags05.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=6}
		SceneTable["-4019_07"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags01.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=7}
		SceneTable["-4019_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=8}
		SceneTable["-4019_09"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=9}
		SceneTable["-4019_10"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_misc_solve_nags04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="test2hints",idleorderingroup=10}
			 
				 
		// ====================================== Second Test Done
		
		// Andddd heerrrrrre ittttt comes.... No. Nothing.  
		SceneTable["-4013_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_2nd_first_test_solve01.vcd") 
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
		
		// I think we’re in luck. Judging by the tests he’s made so far, we shouldn’t have any trouble staying alive long enough to stop him.
		SceneTable["-4023_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4023_02"
			noDingOff = true
			noDingOn = true
		}
		
		// All we have to do is find out where we are, and I can take us right to him.
		SceneTable["-4023_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests03.vcd") 
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
// TBeam Tests
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_tb_intro")
	{
	
		// ====================================== Wheatley Uses Glados Test Chambers
		
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
		
		// Okay. So the bad news is the tests are my tests now, which means they can kill us. The GOOD news is, I know where we ARE now.  
		SceneTable["-4017_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests06.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4017_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// If we stay on this test track, I can get us out of bounds in ten chambers.  
		SceneTable["-4017_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_newtests10.vcd") 
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
		
	}
	
	if (curMapName=="sp_a4_tb_trust_drop")
	{
	
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
			next = "-4400_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// I was hoping to shoot rock salt at you while you tested. See if that helped.		
		SceneTable["-4400_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_intro07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4400_04"
			noDingOff = true
			noDingOn = true	
		}

		// You’ll know I cracked it when… well, when rock salt hits you, I imagine.		
		SceneTable["-4400_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_intro08.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}

		// Alright, this is taking too long. I’ll just tell you how to solve the test. 
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
		
		// You see that button up there? You just need to… need to… 
		SceneTable["-4401_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4401_03"
			noDingOff = true
			noDingOn = true	
		}	
		
		// HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!
		SceneTable["-4401_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4401_04"
			noDingOff = true
			noDingOn = true	
		}	
		
		// You just -- HRRRRAUUUUGGHHH! HRRRRAUUUUGGHHH!
		SceneTable["-4401_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient04.vcd") 
			char = "wheatley"
			postdelay = -1.8 
			predelay = 0.0
			next = "-4401_05"
			noDingOff = true
			noDingOn = true	
		}	
		
		// This is why we don't talk during the tests.
		SceneTable["-4401_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4401_06"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
				
		//(gasping for breath, spitting) Solve it yourself. Take your time. 
		SceneTable["-4401_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_impatient06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}			

		// [gasping for breath, spitting] Ugh. Okay. Okay, good.
		SceneTable["-4402_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4402_02"
			noDingOff = true
			noDingOn = true	
		}

		// Clams...
		SceneTable["-4402_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_trust_drop02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4402_03"
			noDingOff = true
			noDingOn = true	
		}	

		// vomiting
		SceneTable["-4402_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_trust_drop_solve03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
		
		
	/*
		// ====================================== Glados compares herself to Wheatley
		
		// I wasn't as bad as HE is, was I? 
		SceneTable["-4014_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleytests01.vcd") 
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
	*/
	
	}
	
	if (curMapName=="demo_tbeam")
	{
	
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
			postdelay = 0.2 
			predelay = 0.0
			next = "-4403_03"
			noDingOff = true
			noDingOn = true	
		}
			
		// Anyway, let’s just… mash them together and see if you can’t solve it.
		SceneTable["-4403_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Anddddd…THERE we go.
		SceneTable["-4404_01" ] <-
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
	
		// Ohhhh, here we go… here it comes… test gets solved in three… two… 
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
	
	/*
	
		// Coming! Coming! Don't start yet! Don't start yet!
		SceneTable["-4100_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			postdelay = 0.0 
			predelay = 0.0
			next = "-4100_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// You're not gonna believe this. I found a sealed off wing back here. Hundreds of perfectly good test chambers. Just sitting there. Filled with skeletons. Shook 'em out. Good as new!
		SceneTable["-4100_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4100_03"
			noDingOff = true
			noDingOn = true	
		}

		// Anyway, let’s just… mash ‘em together and see if you can’t solve it.	
		SceneTable["-4100_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_wall_button_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// ====================================== Glados compares herself to Wheatley 2
		
		// You know... I'm starting to think he doesn't even CARE about the test results. 
		SceneTable["-4027_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleytests02.vcd") 
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
	*/	
	}

	if (curMapName=="sp_a4_tb_polarity")
	{

		// Alright. So that last test was… disappointing. Apparently being civil isn’t motivating you.  So let’s try things her way… fatty.  Adopted fatty.
		SceneTable["-4410_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro01.vcd") 
			char = "wheatley"
			postdelay = -14.0 
			predelay = 0.0
			next = "-4410_02"
			noDingOff = true
			noDingOn = true	
		}

		// What, exactly, is wrong with being adopted?
		SceneTable["-4410_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity01.vcd") 
			char = "glados"
			postdelay = 0.0 
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
			postdelay = 0.4 
			predelay = 0.0
			next = "-4410_04"
			noDingOff = true
			noDingOn = true	
		}
		
		// bumblings
		SceneTable["-4410_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro06.vcd") 
			char = "wheatley"
			postdelay = -1.6
			predelay = 0.0
			next = "-4410_05"
			noDingOff = true
			noDingOn = true	
			
		}
		
		// Work with me...
		SceneTable["-4410_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity02.vcd") 
			char = "glados"
			postdelay = 7.8 
			predelay = 0.0
			next = "-4410_06"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
		// Not fat...
		SceneTable["-4410_06" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity03.vcd") 
			char = "glados"
			postdelay = -3.2 
			predelay = 0.0
			next = "-4410_07"
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
		// not a moron
		SceneTable["-4410_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro09.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = "-4410_08"
			noDingOff = true
			noDingOn = true	
			talkover = true
			
		}
		
		// Just test!
		SceneTable["-4410_08" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_intro10.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			
		}

		// [flip flip flip] Oh. I’m sorry. I hope that didn’t disturb you too much during the test. That was just the sound of books. Being read. By me. You know what it’s like when the urge - and ability - to read takes you. Ahhh. Literature. Reading it.
		SceneTable["-4411_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.1
			predelay = 0.0
			next = "-4411_02"
			noDingOff = true
			noDingOn = true	
		}
		
		//  You know what it's like when the urge – and ability – to read takes you. Ahhh. Literature.
		SceneTable["-4411_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4411_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// So that's what I was doing. I was just reading. Books.	
		SceneTable["-4411_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_polarity_solve03.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		
		// gone too far...
		SceneTable["-4447_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_polarity04.vcd") 
			char = "glados"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
			talkover = true
		}
		
	}
	
	if (curMapName=="sp_a4_tb_catch")
	{
		
		// Oh! I didn’t hear you come in.  I was just polishing my… book museum, when it occurred to me, I’ve read all of the books. Tragic. Speaking as someone who can literally read. Not bragging.
		SceneTable["-4412_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4412_02"
			noDingOff = true
			noDingOn = true	
		}	

		// Speaking as someone who can literally read. Not bragging, not trying to show off.
		SceneTable["-4412_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro02.vcd") 
			char = "wheatley"
			postdelay = 0.2 
			predelay = 0.0
			next = "-4412_03"
			noDingOff = true
			noDingOn = true	
		}			
	
		// But I can read. So... not a moron.
		SceneTable["-4412_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4412_04"
			noDingOff = true
			noDingOn = true	
		}	
		
		// Anyway, all done with them now. Someone should really make a book called "How to Make Books."		
		SceneTable["-4412_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_intro07.vcd") 
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
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_tb_catch_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}		
	
		// He's building up a resistance to the tests. I did too. I just didn't care, because I was doing it for the science.
		SceneTable["-4028_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_catch01.vcd") 
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
	
		// resistance
		SceneTable["-4446_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_tb_catch02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// Don’t mind me. Just moving the test chamber a little closer to me. Proximity to the test solving. Might get stronger results.
		SceneTable["-4414_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_intro01.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4414_02"
			noDingOff = true
			noDingOn = true	
		}	

		// It won't.
		SceneTable["-4414_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_stop_the_box01.vcd") 
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
			postdelay = 0.6 
			predelay = 0.0
			next = "-4414_04"
			noDingOff = true
			noDingOn = true	
		}		

		// Nothing.
		SceneTable["-4414_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_stop_the_box02.vcd") 
			char = "glados"
			postdelay = 0.4
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
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
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
			postdelay = 0.0 
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
			next = "-4416_04"
			noDingOff = true
			noDingOn = true	
		}	

		// Get into the lift, onto the next test...
		SceneTable["-4416_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_stop_the_box_solve05.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
		
		/*
		// ====================================== Graduation Intro
		
		// THESE tests were designed to be solved by invincible titanium test-bots piloted by a super computer. But we never got around to inventing them. So we tried it with humans. It… didn’t end well.
		SceneTable["-4025_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_miscsupertests01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// ====================================== Graduation Solved
		
		// You are GOOD. Too good. But I guess you’ll be leaving. You ARE leaving, aren’t you?
		SceneTable["-4026_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_miscsupertests03.vcd") 
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
		*/	
	
	}

	if (curMapName=="sp_a4_laser_catapult")
	{
	
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
		}	
	
	
	}
	
	if (curMapName=="sp_a4_laser_platform")
	{
		
		// This place is self-destructing, you idiot!
		SceneTable["-4419_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform07.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
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
					{entity="@force_rumble",input="Trigger",parameter="",delay=2,fireatstart=true}
				]			
		}	

		// Programmed in one last tremor, for old time’s sake. 
		SceneTable["-4419_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_intro03.vcd") 
			char = "wheatley"
			postdelay = 0.6 
			predelay = 0.0
			next = "-4419_04"
			noDingOff = true
			noDingOn = true	
			fires=
				[
					{entity="@force_rumble"input="Trigger",parameter="",delay=2,fireatstart=true}
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
			vcd = CreateSceneEntity("scenes/npc/announcer/recapture03.vcd") 
			char = "announcer"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4419_06"
			noDingOff = true
			noDingOn = true	
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
		}	
		
		// Sorry about the lift. It's, uh... temporarily out of service. It... melted.
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
		
		// Sorry about the lift. It's, uh... temporarily out of service. It... melted.
		SceneTable["-4130_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform_tbeam01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}

		/*
		// Not a problem, though. Not much to see upstairs now, anyway. Also melted.
		SceneTable["-4131_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_laser_platform09.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4131_03"
			noDingOff = true
			noDingOn = true	
		}
		*/

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
			predelay = 0.0
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
		
		/*
		
		// This place is self-destructing...
		SceneTable["-4132_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform03.wav                               .vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	*/
	}
	
/*	

// ****************************************************************************************************
// Act 4 General
// ****************************************************************************************************
	
			
		// ====================================== Glados Chell Deal
	
	if (curMapName=="sp_a4_laser_platform")
	{
		
		
		
		// You need to put me back in my body. I know this sounds like a trick. In fact, I can unconditionally guarantee you that at any other time it would be a trick. But if you don’t let me stop the damage your friend is doing, this facility is going to explode. Soon.
		SceneTable["-4015_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform03.vcd") 
			char = "glados"
			postdelay = 4.5 
			predelay = 0.0
			next = "-4015_02"
			noDingOff = true
			noDingOn = true
		}			
		
		// Let’s make a deal. If you get me back in my body and help me stop him… I’ll… let… you… go.
		SceneTable["-4015_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform01.vcd") 
			char = "glados"
			postdelay = 0.8 
			predelay = 0.0
			next = "-4015_03"
			noDingOff = true
			noDingOn = true
		}
		
		// I want to make this clear: I’m not promising to stop testing humans. I’m just promising to stop testing on you. So long as you leave and never. Come. Back. 
		SceneTable["-4015_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_laser_platform04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}
		
	}
	
*/
	
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
		}	
		
		// Just getting a test ready... For you. Who else would I be doing it for? No one.                                                                                                                                                                                                                                                                                                                                                                       
		SceneTable["-4448_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_speed_tb_catch02.vcd") 
			char = "wheatley"
			postdelay = 0.4 
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
			fires=
				[
					{entity="start_chamber_destruction",input="Trigger",parameter="",delay=2,fireatstart=true}
				]				
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
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
						
		// After you told me to turn the beam off, I thought I'd lost you. Went poking around for test subjects. No luck there. Still all dead.
		SceneTable["-4449_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_speed_tb_catch_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.1 
			predelay = 0.0
			next = "-4449_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// Oh! But I did find something. Reminds me: I've got a big surprise for you two. Look forward to it.
		SceneTable["-4449_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_speed_tb_catch_solve02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}			
		
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
		
	}

	if (curMapName=="sp_a4_jump_polarity")
	{
		
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
		
		// Carry on!
		SceneTable["-4420_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro04.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
	
		// You are gonna love this surprise.
		SceneTable["-4444_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death03.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4444_02"
			noDingOff = true
			noDingOn = true	
		}	
		
		// I don't know whether you're getting that...
		SceneTable["-4444_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death02.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}	
		/*	
		// Yes, now I'm convinced.
		SceneTable["-4444_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleysurprise02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 1.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		*/	
		
		// Oh, you solved it. Good.
		SceneTable["-4421_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_sp_a4_jump_polarity_intro_solve01.vcd") 
			char = "wheatley"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
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
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death06.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "null"
			noDingOff = true
			noDingOn = true
		}

		// I'll bet you're DYING to know what your big surprise is. There's only two more chambers.
		SceneTable["-4800_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_love_to_death06.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
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
			next = "null"
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
		
		/*
		// Good instincts on that, by the way.
		SceneTable["-4801_05"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap03.vcd") 
			char = "glados"
			postdelay = 1 
			predelay = 0.0
			next = "-4801_07"
			noDingOff = true
			noDingOn = true
		}
		*/
		
		// It wasn't anything personal. Just... you know. You did kill me. Fair's fair.
		SceneTable["-4801_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap05.vcd") 
			char = "glados"
			postdelay = 2.5 
			predelay = 0.0
			next = "-4801_07"
			noDingOff = true
			noDingOn = true
		}
	

		// Anyway, this is the part where he kills us.
		SceneTable["-4801_07" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_wheatleys_trap06.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.6
			next = null
			noDingOff = true
			noDingOn = true
		}

		// Hello! This is the part where I kill you.
		SceneTable["-4802_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "null"
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StartWheatleyDeathtrapNag()",delay=1.0}
			] 
		}
		
		// ======================================
		// Wheatley deathtrap nag
		// ======================================
		
		SceneTable["-4803_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_spike_smash01.vcd"), predelay=0.0, postdelay=0.0,next="-4803_02",char="wheatley",noDingOff=true,noDingOn=true,idle=true, talkover=true,idlegroup="deathtrap",idleorderingroup=1}
		SceneTable["-4803_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_spike_smash02.vcd"),postdelay=0.0,next="-4803_03",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=2}
		SceneTable["-4803_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_finale01a01.vcd"),postdelay=0.0,next="-4803_04",char="glados",noDingOff=true,noDingOn=true,talkover=true,idlegroup="deathtrap",idleorderingroup=3}
		SceneTable["-4803_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou01.vcd"),postdelay=0.0,next="-4803_05",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=2}
		SceneTable["-4803_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_finale01a02.vcd"),postdelay=0.0,next="-4803_06",char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=5}
		SceneTable["-4803_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou09.vcd"),postdelay=0.0,next="-4803_07",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=2}
		SceneTable["-4803_07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_finale01a03.vcd"),postdelay=0.0,next="-4803_08",char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=7}
		SceneTable["-4803_08"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou11.vcd"),postdelay=0.0,next="-4803_09",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=2}
		SceneTable["-4803_09"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_trap_solve05.vcd"),postdelay=0.0,next="-4804_01",char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=9}
		/*SceneTable["-4803_10"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale01_killyou13.vcd"),postdelay=0.0,next="-4803_11",char="wheatley",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=10}
		SceneTable["-4803_11"] <- {vcd=CreateSceneEntity("scenes/npc/glados/potatos_trap_solve06.wav.vcd"),postdelay=0.0,next="-4804_01",char="glados",noDingOff=true,noDingOn=true, talkover=true,idlegroup="deathtrap",idleorderingroup=11}*/
			
		function StartWheatleyDeathtrapNag()
		{
			printl("==Starting wheatley Nag")
			GladosPlayVcd( -4803 )
		}
		
		function StopWheatleyDeathtrapNag()
		{
			printl("==Stopping wheatley Nag")
			GladosStopNag()
			GladosCharacterStopScene("wheatley")  // this will make wheatley stop talking if he is mid sentence
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
				{entity="@glados",input="RunScriptCode",parameter="StopWheatleyDeathtrapNag()",delay=0.0, fireatstart=true}			
				{entity="relay_crush_prep",input="Trigger",parameter="",delay=5.0 ,fireatstart=true },
				{entity="relay_wheatley_leave",input="Trigger",parameter="",delay=0.0 ,fireatstart=true },

			]
		}
			
		/*		// Awwww. Keys. Keys. What have I done with my keys?                                                                          
		SceneTable["-4804_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_nags05.vcd") 
			char = "wheatley"
			postdelay = 0.4 
			predelay = 0.0
			next = "-4804_03"
			noDingOff = true
			noDingOn = true
		}
		                                                                                                           

		// Ah! Here they are.                                                                        
		SceneTable["-4804_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_nags07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			fires=
			[
				{entity="relay_crush_prep",input="Trigger",parameter="",delay=0.0 ,fireatstart=true },
			]
		}
		*/

	
		// Hey! Come back! Come back!
		SceneTable["-4805_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_death_trap_escape01.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
			queue = true
			queuecharacter = "glados"
			fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="StopWheatleyDeathtrapNag()",delay=0.0, fireatstart=true}			
			]
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
		// So-yes. It's a trap. The stupidest, most obvious trap I've ever seen.
		SceneTable["-4821_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale02_catwalk_finished01.vcd") 
			char = "glados"
			postdelay = 1.0 
			predelay = 0.0
			next = "-4821_02"
			noDingOff = true
			noDingOn = true
		}
		// Look. Let's just fall for it. He'll screw it up somehow. And it'll be quicker than trying to avoid it.
		SceneTable["-4821_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a4_finale02_catwalk_finished02.vcd") 
			char = "glados"
			postdelay = 1.0 
			predelay = 0.0
			next = "null"
			noDingOff = true
			noDingOn = true
		}

		// Ha! Death trap!
		SceneTable["-4822_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_enter01.vcd") 
			char = "wheatley"
			postdelay = 1.6 
			predelay = 0.0
			next = "-4822_02"
			noDingOff = true
			noDingOn = true
		}
		// Are they killing you? Say no if they aren't.
		SceneTable["-4822_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_turrettrap_nags01.vcd") 
			char = "wheatley"
			postdelay = 1.6
			predelay = 0.0
			next = "-4822_03"
			noDingOff = true
			noDingOn = true
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
			next = "null"
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
			next = "null"
			noDingOff = true
			noDingOn = true
		}




		// Ha! Spinny-blade-wall! Machiavellian.
		SceneTable["-4825_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_beamtrap_spinnyblades02.vcd") 
			char = "wheatley"
			postdelay = 1.0 
			predelay = 0.0
			next = "null"
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
			next = "null"
			noDingOff = true
			noDingOn = true
		}



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
		// Holmes versus Moriarty.
		SceneTable["-4828_01"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro05.vcd") 
			char = "wheatley"
			postdelay = 0.3 
			predelay = 0.0
			next = "-4828_02"
			noDingOff = true
			noDingOn = true
		}
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
		// Aristotle versus Mashy-spike-plate!
		SceneTable["-4828_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale02_mashplate_intro07.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = "null"
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
			next = "null"
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
			next = "null"
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
			next = "null"
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
			next = "null"
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
		SceneTable["-4851_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_escape01.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}

		// Oh! Oh! I'll take that as a no.Not choosing the death trap. Not dying.
		SceneTable["-4852_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_a4_finale03_escape02.vcd") 
			char = "wheatley"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
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
		}
	
	}

	
// ****************************************************************************************************
// Boss Battle
// ****************************************************************************************************
	
	if (curMapName=="sp_a4_finale4")
	{

		// ====================================== Corrupted Core Intro
		
		// These are corrupted cores
		SceneTable["-4900_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_corrupted_cores01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4900_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// We can attach these to Wheatley to make him more corrupt than I am
		SceneTable["-4900_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_corrupted_cores02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4900_03"
			noDingOff = true
			noDingOn = true	
		}
		
		// You will need to disable Wheatley somehow
		SceneTable["-4900_03" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_corrupted_cores03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4900_04"
			noDingOff = true
			noDingOn = true	
		}
		
		// So you can attach the cores to him
		SceneTable["-4900_04" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_corrupted_cores04.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4900_05"
			noDingOff = true
			noDingOn = true	
		}
		
		// It might not be easy for me to deliver them to you
		SceneTable["-4900_05" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_corrupted_cores05.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		// ====================================== Breaker Shaft
		
		// Plug me on to this socket
		SceneTable["-4901_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_lift_socket01.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4901_02"
			noDingOff = true
			noDingOn = true	
		}
		
		// I will go back down and send you the corrupted cores
		SceneTable["-4901_02" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_lift_socket02.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true	
		}
		
		
		// ====================================== Chamber Intro
		
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
		
		// ====================================== Pre-Pipe Destruction
		
		// Where are you going? Don't run. Don't run. The harder you breathe, the more neurotoxin you'll inhale. It's bloody clever. Devilish.
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

		// ====================================== Post-Pipe Destruction
		
		// No! Um... That's an impression of you. Because you just fell into my trap. There. Just now. Ha. I wanted you to trick me into bursting that pipe. Gives you false hope. Leads to overconfidence. Mistakes. Fatal missteps. All part of my plan.
		
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

		// ====================================== Post-Pipe Post Portal Destruction
		
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

		// ====================================== Core Delivery
		
		// Here's the first corrupted core
		SceneTable["-4915_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores01.vcd") 
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
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores02.vcd") 
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
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores03.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4916_02"
			noDingOff = true
			noDingOn = true	
		}
		
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
		
		// Here's the third corrupted core
		SceneTable["-4917_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_deliver_cores06.vcd") 
			char = "glados"
			postdelay = 0.0 
			predelay = 0.0
			next = "-4917_02"
			noDingOff = true
			noDingOn = true	
		}
		
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

		// ====================================== Core Transfer

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
		
		// ====================================== Stalemate Button Nags
		
        // Don't press the button!
        SceneTable["-4940_01"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idle=true,idleminsecs=6.0,idlemaxsecs=8.0,idlegroup="stalemate_buttonnags",idleorderingroup=1}
        SceneTable["-4940_02"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags02.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=2}
        SceneTable["-4940_03"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags03.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=3}
		SceneTable["-4940_04"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags04.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=4}
		SceneTable["-4940_05"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags05.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=5}
		SceneTable["-4940_06"] <- {vcd=CreateSceneEntity("scenes/npc/sphere03/bw_bb_buttonnags06.vcd"),postdelay=0.1,next=null,char="wheatley",noDingOff=true,noDingOn=true,idlegroup="stalemate_buttonnags",idleorderingroup=6}
		
		// ====================================== Button Pressed
		
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

		//Oh yes, brilliant. Take on elast look at your precious human moon. Because it cannot help you now!
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
		
		// AHHHHHHH!
		SceneTable["-4950_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/sphere03/bw_bb_moonportal01.vcd") 
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
			predelay = 2.0
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
//		GladosCharacterStopScene("glados")
//		GladosCharacterStopScene("wheatley")
		GladosPlayVcd ( -4804 )
	}

	function WheatleyEscape01()
	{
		GladosPlayVcd ( -4805 )
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
		GladosPlayVcd( -4012 )
	}
	
	function RecaptureTestTwoDone()
	{
		GladosPlayVcd( -4013 )
	}
	
	function GladosCompare1()
	{
	//	GladosPlayVcd( -4014 )
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
         printl("==Starting Wheatley Nag")
         GladosPlayVcd( -4021 )
    }
	
	function StartWheatleyTest1FinishNag()
    {
         printl("==Starting Wheatley Nag")
         GladosPlayVcd( -4022 )
    }
	
	function StartWheatleyTest2Nag()
    {
         printl("==Starting Wheatley Nag")
         GladosPlayVcd( -4019 )
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
	

// ============================================================================
// Functions for tb_trust_drop / tb_wall_button / tb_polarity / tb_catch / stop_the_box / laser_catapult / laser_platform / jump_polarity
// ============================================================================


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

	function tb_wall_button_start_yet()
	{
		GladosPlayVcd( -4403 )
	}

	function tb_wall_button_crush_end()
	{
		GladosPlayVcd( -4404 )	
	}	
	
	function tb_wall_button_test_solve_1()
	{
		GladosPlayVcd( -4405 )	
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
		GladosPlayVcd( -4409 )	
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
	
	function tb_catch_test_start()
	{
		GladosPlayVcd( -4412 )	
	}		

	function tb_catch_test_end()
	{
		GladosPlayVcd( -4413 )	
	}

	function stop_the_box_glados_start()
	{
		GladosPlayVcd( -4446 )	
	}	
	
	function stop_the_box_test_start()
	{
		GladosPlayVcd( -4414 )	
	}		

	function stop_the_box_missed()
	{
		GladosPlayVcd( -4415 )	
	}
	
	function stop_the_box_test_end()
	{
		GladosPlayVcd( -4416 )	
	}

	function laser_catapult_test_start()
	{
		GladosPlayVcd( -4417 )	
	}

	function laser_catapult_test_end()
	{
		GladosPlayVcd( -4418 )	
	}
	
	function laser_platform_test_start()
	{
		GladosPlayVcd( -4419 )	
	}

	function jump_polarity_sorry()
	{
		GladosPlayVcd( -4420 )	
	}
	
	function jump_polarity_test_end()
	{
		GladosPlayVcd( -4421 )	
	}
	
	function speed_tb_catch_intro_test_start()
	{
		GladosPlayVcd( -4448 )	
	}
	
	function speed_tb_catch_intro_test_complete() 
	{
		GladosPlayVcd( -4449 )	
	}
	
	// ============================================================================
	// Functions for Boss Battle
	// ============================================================================
	
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
		GladosPlayVcd( -4910 )
	}
	
	function BBPrePipeDest()
	{
		GladosPlayVcd( -4911 )
	}
	
	function BBPostPipeDest()
	{
		GladosPlayVcd( -4912 )
	}
	
	function BBPostPipePortal()
	{
		GladosPlayVcd( -4913 )
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
	
	function BBWakeUpClueless1()
	{
		GladosPlayVcd( -4920 )
	}
	
	function BBWakeUpClueless2()
	{
		GladosPlayVcd( -4921 )
	}
	
	function BBWakeUpClueless3()
	{
		GladosPlayVcd( -4922 )
	}
	
	function BBWakeUpStill1()
	{
		GladosPlayVcd( -4925 )
	}
	
	function BBWakeUpStill2()
	{
		GladosPlayVcd( -4926 )
	}
	
	function BBWakeUpStill3()
	{
		GladosPlayVcd( -4927 )
	}
	
	function BBWakeUpStill4()
	{
		GladosPlayVcd( -4928 )
	}
	
	function BBCoreTransfer()
	{
		GladosPlayVcd( -4930 )
	}
	
	function BBButtonNags()
	{
		GladosPlayVcd( -4940 )
	}
	
	function BBButtonPressed()
	{
		GladosPlayVcd( -4941 )
	}
	
	function BBPortalPlaced()
	{
		GladosPlayVcd( -4950 )
	}
	
// ****************************************************************************************************
// Personality Cores scenetable								  
// ****************************************************************************************************

SceneTableLookup[-7100] <- "-7100_01" // [nervous laugh] Space.
	// [nervous laugh] Space. Space. Wanna go to space.
	// Space. Space. Space. Space. Comets. Stars. Galaxies. Orion. [nervous laugh]
	// Are we in space yet? What's the hold-up? Gotta go to space. Gotta go to SPACE.
	// What's your favorite thing about space? Mine is space.
	// [nervous laugh] Going to space.
	// [nervous laugh] I'm going. Going to space.
	// [nervous laugh] Love space. Need to go to space.
	// [nervous laugh] Didn't kill anyone. Oh! Oh! Let's go to space.
	// Space space space. Going. Going there. Okay. [nervous laugh] I love you, space.
	// Space going to space can't wait.
	// [nervous laugh] Space.
	// [nervous laugh] Stars. Planets. Asteroids. Suns. Going to space.
	// Space...
	// Einstein. E equals me in space.
	// Space. Trial. Puttin' the system on trial. Guilty. Of being in space. Going to space jail.
	// You are the farthest ever in space. [normal voice] Why me, space? [low-pitched 'space' voice] Because you are the best. The best at space.
	// [nervous laugh] So much space. Need to see it all.
	// Dad! I'm in space! [low-pitched 'space' voice] I'm proud of you, son. [normal voice] Dad, are you space? [low-pitched 'space' voice] Yes. Now we are a family again.
	// Space space wanna go to space yes please space. Space space. Go to space.
	// Space space wanna go to space
	// Space space going to space oh boy
	// [nervous laugh] Stars. Planets. Asteroids. Suns. Space zoos. Asteroid zoos. Space ships. Planet space suns. Oh boy.
	// Ba! Ba! Ba ba ba! Space! Ba! Ba! Ba ba ba!
	// Space invaders. Ohhhh. No. No. Don't do it. Don't invade space.
	// Guess what I am. Give up? I'm in space.
	// I'm go to space. I'm gonna go to space twenty-seven times! I'm going to win! At space!
	// Oh. Play it cool. Play it cool. Here come the space cops.
	// Help me, space cops. Space cops, help.
	// Ha ha ha ha! Halley's comet! Gonna see it twice!
	// Going to space going there can't wait gotta go. Space. Going.
	// Space Court. For people in space. Judge space sun presiding. Bam. Guilty. Of being in space. I'm in space.
	// Better buy a telescope. Wanna see me. Buy a telescope. Gonna be in space.
	// Running out of space. NOOO. Wait. Space is infinite. Nevermind. Close call.

SceneTableLookup[-7200] <- "-7200_01" // Dental floss has superb tensile strength.
	// The square root of rope is string.
	// While the submarine is vastly superior to the boat in every way, over 97% of people still use boats for aquatic transportation.
	// Cellular phones will not give you cancer. Only hepatitis.
	// Pants were invented by sailors in the sixteenth century to avoid Poseidon's wrath. It was believed that the sight of naked sailors angered the sea god.
	// The atomic weight of Germanium is seven two point six four.
	// 89% of magic tricks are not magic. Technically, they are sorcery.
	// An ostrich's eye is bigger than its brain.
	// In Greek myth, the craftsman Daedalus invented human flight so a group of Minotaurs would stop teasing him about it.
	// Humans can survive underwater. But not for very long.
	// Raseph, the semitic god of war and plague, had a gazelle growing out of his forehead.
	// The plural of surgeon general is surgeons general. The past tense of surgeons general is surgeonsed general.
	// Polymerase I polypeptide A is a human gene.
	// Rats cannot throw up.
	// Iguanas can stay underwater for twenty-eight point seven minutes.
	// Human tapeworms can grow up to twenty-two point nine meters.
	// The Schrodinger's cat paradox outlines a situation in which a cat in a box must be considered, for all intents and purposes, simultaneously alive and dead. Schrodinger created this paradox as a justification for killing cats.
	// Every square inch of the human body has 32 million bacteria on it.
	// The Sun is 330,330 times larger than Earth.
	// The average life expectancy of a rhinoceros in captivity is 15 years.
	// Volcanologists are experts in the study of volcanoes.
	// Avocados have the highest fiber and calories of any fruit.
	// Avocados have the highest fiber and calories of any fruit. They are found in Australians.
	// The moon orbits the Earth every 27.32 days.
	// The billionth digit of Pi is 9.
	// If you have trouble with simple counting, use the following mnemonic device: one comes before two comes before 60 comes after 12 comes before six trillion comes after 504. This will make your earlier counting difficulties seem like no big deal.
	// A gallon of water weighs 8.34 pounds
	// Hot water freezes quicker than cold water.
	// Honey does not spoil.
	// The average adult body contains half a pound of salt.
	// A nanosecond lasts one billionth of a second.
	// According to Norse legend, thunder god Thor's chariot was pulled across the sky by two goats.
	// China produces the world's second largest crop of soybeans.
	// Tungsten has the highest melting point of any metal, at 3,410 degrees Celsius.
	// Gently cleaning the tongue twice a day is the most effective way to fight bad breath.
	// Roman toothpaste was made with human urine. Urine as an ingredient in toothpaste continued to be used up until the 18th century.
	// The Tariff Act of 1789, established to protect domestic manufacture, was the second statute ever enacted by the United States government.
	// The value of Pi is the ratio of any circle's circumference to its diameter in Euclidean space.
	// The Mexican-American War ended in 1848 with the signing of the Treaty of Guadalupe Hidalgo.
	// In 1879, Sandford Fleming first proposed the adoption of worldwide standardized time zones at the Royal Canadian Institute.
	// Marie Curie invented the theory of radioactivity, the treatment of radioactivity, and dying of radioactivity.
	// At the end of The Seagull by Anton Chekhov, Konstantin kills himself.
	// Contrary to popular belief, the Eskimo does not have a hundred different words for snow. They do, however, have two hundred and thirty-four words for fudge.
	// In Victorian England, a commoner was not allowed to look directly at the Queen, due to a belief at the time that the poor had the ability to steal thoughts. Science now believes that less than 4% of poor people are able to do this.
	// In 1862, Abraham Lincoln signed the Emancipation Proclamation, freeing the slaves. Like everything he did, Lincoln freed the slaves while drunk, and later had no memory of the event.
	// In 1862, Abraham Lincoln signed the Emancipation Proclamation, freeing the slaves. Like everything he did, Lincoln freed the slaves while sleepwalking, and later had no memory of the event.
	// In 1948, at the request of a dying boy, baseball legend Babe Ruth ate seventy-five hot dogs, then died of hot dog poisoning.
	// William Shakespeare did not exist. His plays were masterminded in 1589 by Francis Bacon, who used a Ouija board to enslave play-writing ghosts.
	// It is incorrectly noted that Thomas Edison invented "push-ups" in 1878. Nikolai Tesla had in fact patented the activity three years earlier, under the name "Tesla-cize."
	// Whales are twice as intelligent, and three times as delicious, as humans.
	// The automobile brake was not invented until 1895. Before this, someone had to remain in the car at all times, driving in circles until passengers returned from  their errands.
	// Edmund Hillary, the first person to climb Mount Everest, did so accidentally while chasing a bird.
	// Diamonds are made when coal is put under intense pressure. Diamonds put under intense pressure become pellets of foam pellets, commonly used today as packing material.
	// The most poisonous fish in the world is the orange ruffy. Everything but its eyes are made of deadly poison. The ruffy's eyes are composed of a less harmful, deadly poison.
	// The occupation of court jester was invented accidentally, when a vassal's epilepsy was mistaken for capering.
	// Halley's Comet can be viewed orbiting Earth every seventy-six years. For the other seventy-five, it retreats to the heart of the sun, where it hibernates undisturbed.
	// The first commercial airline flight took to the air in 1914. Everyone involved screamed the entire way.
	// In Greek myth, Prometheus stole fire from the Gods and gave it to humankind. The jewelry he kept for himself.
	// The first person to prove that cow's milk is drinkable was very, very thirsty.
	// Before the Wright Brothers invented the airplane, anyone wanting to fly anywhere was required to eat 200 pounds of helium.
	// Before the invention of scrambled eggs in 1912, the typical breakfast was either whole eggs still in the shell or scrambled rocks.
	// During the Great Depression, the Tennessee Valley Authority outlawed pet rabbits, forcing many to hot glue gun long ears onto their pet mice.
	// At some point in their lives 1 in 6 children will be abducted by the Dutch.
	// According to most advanced algorithms, the world's best name is Craig.
	// To make a photocopier, simply photocopy a mirror.
	// Dreams are the subconscious minds way of reminding people to go to school naked and have their teeth fall out.

SceneTableLookup[-7300] <- "-7300_01" // QUICK: WHAT'S THE SITUATION? Oh, hey, hi pretty lady. My name's Rick. So, you out having a little adventure?
	// QUICK: WHAT'S THE SITUATION? Oh, hello angel. I guess I must have died and gone to heaven. My name's Rick. So, you just out having an adventure?
	// What, are you fighting that guy? You got that under control? You know, because, looks like there's a lot of stuff on fire...
	// Hey, a countdown clock! Man, that is trouble. Situation's looking pretty ugly. For such a beautiful woman. If you don't mind me saying.
	// I don't want to scare you, but, I'm an Adventure Sphere. Designed for danger. So, why don't you go ahead and have yourself a little lady break, and I'll just take it from here.
	// Here, stand behind me. Yeah, just like that. Just like you're doing. Things are about to get real messy.
	// Going for it yourself, huh? All right, angel. I'll do what I can to cover you.
	// Doesn't bother me. I gotta say, the view's mighty nice from right here.
	// Man, that clock is moving fast. And you are beautiful. Always time to compliment a pretty lady. Allright, back to work. Let's do this.
	// I'll tell ya, it's times like this I wish I had a waist so I could wear all my black belts. Yeah, I'm a black belt. In pretty much everything. Karate. Larate. Jiu Jitsu. Kick punching. Belt making. Tae Kwan Do... Bedroom
	// I am a coiled spring right now. Tension and power. Just... I'm a muscle. Like a big arm muscle, punching through a brick wall, and it's hitting the wall so hard the arm is catching on fire. Oh yeah.
	// I probably wouldn't have let things get this far, but you go ahead and do things your way.
	// Tell ya what, why don't you put me down and I'll make a distraction.
	// Allright. You create a distraction then, and I'll distract him from the distraction you're creating!
	// Allright, your funeral. Your beautiful-lady-corpse open casket funeral.
	// Do you have a gun? Because I should really have a gun. What is that thing you're holding?
	// How about a knife, then? You keep the gun, I'll use a knife.
	// No knife? That's fine. I know all about pressure points.
	// So, when you kill that guy, do you have a cool line? You know, prepared? Tell you what: Lemme help you with that while you run around.
	// Okay, let's see. Cool line... He's... big.  He's... just hanging there. Okay. Yeah, allright, here we go: "Hang around." That might be too easy.
	// "Hang ten?" That might work if there were ten of him. Do you think there might be nine more of this guy somewhere?
	// You know what, it's gonna be best if you can get him to say something first. It's just better if I have a set-up.
	// Here's the plan: Get him to say, "You two have been a thorn in my side long enough." Then tell your pretty ears to stand back, because I am going to zing him into the stone age.
	// Here's the plan: Get him to say, "You two have been a thorn in my side long enough." Then tell your pretty ears to stand back, because I am going to zing him into space.
	// Don't forget! Thorn! Side!
	// 'Well, this thorn... is about to take you down.' Man, that sounded a whole lot better in my head.
	// 'Well, this thorn... is about to take you down.' Oh yeah!
	// Okay, have it your way. What, are you fighting that guy? You got that under control? You know, because, looks like there's a lot of stuff on fire...
	// Did you hear that? I think something just exploded. Man, we are in a lot of danger. This is like Christmas. It's better than Christmas. This should be its own holiday. Explosion Day.
	// Happy Explosion Day, gorgeous.


	
		// ****************************************************************************************************
		// Personality Cores
		// ****************************************************************************************************
	
	if (curMapName=="sp_a4_finale4" || curMapName=="core_test1")
	{

		// ====================================== "Space" Core01
		
		SceneTable["-7100_01"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idle=true,idleminsecs=1.0,idlemaxsecs=2.0,idlegroup="core01_babble",idleorderingroup=1}
        SceneTable["-7100_02"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble02.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=2}
        SceneTable["-7100_03"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble03.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=3}
		SceneTable["-7100_04"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble04.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=4}
		SceneTable["-7100_05"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble05.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=5}
		SceneTable["-7100_06"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble06.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=6}
		SceneTable["-7100_07"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble07.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=7}
		SceneTable["-7100_08"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble08.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=8}
		SceneTable["-7100_09"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble09.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=9}
		SceneTable["-7100_10"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble10.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=10}
		SceneTable["-7100_11"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble11.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=11}
		SceneTable["-7100_12"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble12.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=12}
        SceneTable["-7100_13"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble13.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=13}
		SceneTable["-7100_14"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble14.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=14}
		SceneTable["-7100_15"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble15.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=15}
		SceneTable["-7100_16"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble16.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=16}
		SceneTable["-7100_17"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble17.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=17}
		SceneTable["-7100_18"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble18.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=18}
		SceneTable["-7100_19"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble19.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=19}
		SceneTable["-7100_20"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble20.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=20}
		SceneTable["-7100_21"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble21.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=21}
		SceneTable["-7100_22"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble22.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=22}
        SceneTable["-7100_23"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble23.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=23}
		SceneTable["-7100_24"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble24.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=24}
		SceneTable["-7100_25"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble25.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=25}
		SceneTable["-7100_26"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble26.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=26}
		SceneTable["-7100_27"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble27.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=27}
		SceneTable["-7100_28"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble28.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=28}
		SceneTable["-7100_29"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble29.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=29}
		SceneTable["-7100_30"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble30.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=30}
		SceneTable["-7100_31"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble31.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=31}
		SceneTable["-7100_32"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble32.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=32}
        SceneTable["-7100_33"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble33.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=33}
		SceneTable["-7100_34"] <- {vcd=CreateSceneEntity("scenes/npc/core01/babble34.vcd"),postdelay=0.1,next=null,char="core01",noDingOff=true,noDingOn=true,idlegroup="core01_babble",idleorderingroup=34}

		
		// ====================================== "Fact" Core02
		
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
	
	
		// ====================================== "Adventure" Core03
		
		SceneTable["-7300_01"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble01.vcd"),idlerepeat=true, postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idle=true,idleminsecs=1.0,idlemaxsecs=2.0,idlegroup="core03_babble",idleorderingroup=1}
        SceneTable["-7300_02"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble02.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=2}
        SceneTable["-7300_03"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble03.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=3}
		SceneTable["-7300_04"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble04.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=4}
		SceneTable["-7300_05"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble05.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=5}
		SceneTable["-7300_06"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble06.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=6}
		SceneTable["-7300_07"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble07.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=7}
		SceneTable["-7300_08"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble08.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=8}
		SceneTable["-7300_09"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble09.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=9}
		SceneTable["-7300_10"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble10.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=10}
		SceneTable["-7300_11"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble11.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=11}
		SceneTable["-7300_12"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble12.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=12}
        SceneTable["-7300_13"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble13.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=13}
		SceneTable["-7300_14"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble14.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=14}
		SceneTable["-7300_15"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble15.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=15}
		SceneTable["-7300_16"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble16.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=16}
		SceneTable["-7300_17"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble17.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=17}
		SceneTable["-7300_18"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble18.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=18}
		SceneTable["-7300_19"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble19.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=19}
		SceneTable["-7300_20"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble20.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=20}
		SceneTable["-7300_21"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble21.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=21}
		SceneTable["-7300_22"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble22.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=22}
        SceneTable["-7300_23"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble23.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=23}
		SceneTable["-7300_24"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble24.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=24}
		SceneTable["-7300_25"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble25.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=25}
		SceneTable["-7300_26"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble26.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=26}
		SceneTable["-7300_27"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble27.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=27}
		SceneTable["-7300_28"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble28.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=28}
		SceneTable["-7300_29"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble29.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=29}
		SceneTable["-7300_30"] <- {vcd=CreateSceneEntity("scenes/npc/core03/babble30.vcd"),postdelay=0.1,next=null,char="core03",noDingOff=true,noDingOn=true,idlegroup="core03_babble",idleorderingroup=30}
		
	}
	
	// ============================================================================
	// Functions for Personality Cores
	// ============================================================================

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
		GladosPlayVcd( -7200 )
	}
	
	function StopCore02Nag()
    {
        printl("==Stopping core02 Nag")
        Core02StopNag()
        GladosCharacterStopScene("core02")  // This will make Core02 stop talking if he is mid sentence.
    }
	
	function Core03Babble1()
	{
		GladosPlayVcd( -7300 )
	}
	
	function StopCore03Nag()
    {
        printl("==Stopping core03 Nag")
        Core03StopNag()
        GladosCharacterStopScene("core03")  // This will make Core03 stop talking if he is mid sentence.
    }
	

	// ============================================================================
	// Functions for Act 4 Test Chambers
	// ============================================================================
	
	function GladosGraduationIntro()
	{
		GladosPlayVcd( -4025 )
	}
	
	function GladosGraduationGood()
	{
		GladosPlayVcd( -4026 )
	}
	
	function GladosCompare2()
	{
		GladosPlayVcd( -4027 )
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
		GladosPlayVcd( -4110 )
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
		GladosPlayVcd( -4851 )
	}

	function finale3_monitor_break()
	{
		GladosPlayVcd( -4852 )
	}
	
	function finale3_mashed()
	{
		GladosPlayVcd( -4853 )
	}