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
					cavejohnson
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

// ****************************************************************************************************
// CAVE JOHNSON SCENETABLE - 1950S
// ****************************************************************************************************

	SceneTableLookup[-5050] <- "-1950_01" // Welcome, gentlemen, to Aperture Science. Astronauts, war heroes, Olympians--you're here because we want the best, and you are it. So: Who is ready to make some science?
										// [laughing] Now, you already met one another on the limo ride over, so let me introduce myself: I’m Cave Johnson. I own the place. 
										// That eager voice you heard is the lovely Caroline, my assistant. Rest assured, she has transferred your honorarium to the charitable organization of your choice. Isn’t that right, Caroline? 
										// She's the backbone of this facility. Pretty as a postcard, too. Sorry, fellas. She's married. To science.
	
	SceneTableLookup[-5051] <- "-1951_01"	// There’s a thousand tests performed every day here in our enrichment spheres. I can’t personally oversee every one of them, so these pre-recorded messages’ll cover any questions you might have, and respond to any incidents that may occur in the course of your science adventure.
										// Your test assignment will vary, depending on the manner in which you have bent the world to your will.
										// Those of you helping us test the repulsion gel today, just follow the blue line on the floor. 
										// Those of you who volunteered to be injected with praying mantis DNA, I’ve got some good news and some bad news.
										// Bad news is we’re postponing those tests indefinitely. Good news is we’ve got a much better test for you: fighting an army of mantis men. Pick up a rifle and follow the yellow line. You’ll know when the test starts.
	
	SceneTableLookup[-5052] <- "-1952_01" // They say great science is built on the shoulders of giants. Not here. At Aperture, we do all our science from scratch. No hand holding.

	SceneTableLookup[-5053] <- "-1953_01"	// Alright, let’s get started. This first test involves something the lab boys call "repulsion gel." 
										// You're not part of the control group, by the way. You get the gel. Last poor son of a gun got blue paint. Hahaha. All joking aside, that did happen – broke every bone in his legs. Tragic. But informative. Or so I’m told.
										
	SceneTableLookup[-5054] <- "-1954_01"	// Now, the lab boys were adamant that I do not give you any hints on these tests. To be honest, they think I'm spoiling the results just by talking to you. Hizenstein Uncertainty Principles and so forth. I’ll give ‘em something more practical to be uncertain about. Their next paycheck. 
										// Anyway, overruled. If you think I'm affecting your decisions, in any way, don’t be afraid to speak up. I’m not made of glass. 
										// That reminds me: Caroline, Do we have a wing made out of glass yet?
										// Let's get on that, Caroline.
										
	SceneTableLookup[-5055] <- "-1955_01"	// The lab boys just informed me that I should not have mentioned the control group. They're telling me I oughtta stop making these pre-recorded messages. That gave me an idea: make more pre-recorded messages. I pay the bills here, I can talk about the control group all damn day. 

	SceneTableLookup[-5056] <- "-1956_01" // Oh! In case you got covered in that repulsion gel, here’s some advice the lab boys gave me: Do not get covered in the repulsion gel.
										// We haven’t entirely nailed down what element it is yet, but I’ll tell you this: it’s a lively one, and it does NOT like the human skeleton.

	SceneTableLookup[-5057] <- "-1957_01" // Cave Johnson again. Just a heads-up: this next test’s dangerous. So if you get hurt, there’s a first aid station close by.
										// Oh! Unless you’re participating in the double blind emergency treatment experiment. Then it’s just placebo first aid. 
										// Hold on, lemme check what test this is for… [pages flipping] Uh huh. Uh huh. No sir. I wouldn’t bother with it. 
										// What?	

	SceneTableLookup[-5058] <- "-1958_01"	// Ha! I like your style. You make up your own rules, just like me. 
										// Bean counters said I couldn't fire a man just for being in a wheelchair. Did it anyway. Ramps are expensive. 
										
	SceneTableLookup[-5059] <- "-1959_01"	// The average human male is about sixty percent water. Far as we’re concerned, that’s a little extravagant. So if you feel a bit dehydrated in this next test, that’s normal. We’re gonna hit you with some jet engines, and see if we can’t get you down to twenty or thirty percent.
			
										
	SceneTableLookup[-5060] <- "-1960_01"	// Caroline, do you think anyone’s gonna make it this far? Should I even bother making a message? What? Yes, I know how to turn it off. No, I don't want to turn it off... Alright, I feel like turning it off now.
	
	SceneTableLookup[-5061] <- "-1961_01" // I'm telling 'em, keep your pants on.
 										// Alright, this next test may involve trace amounts of time travel. So, word of advice: If you meet yourself on the testing track, don’t make eye contact. Lab boys tell me that'll wipe out time. Entirely. Forward and backward! Do both of yourselves a favor and just let that handsome devil go about his business. 
	
	SceneTableLookup[-5062] <- "-1962_01"	// If you’re hearing this, it means you’re taking a long time on the catwalks between tests. The lab boys say that might be a fear reaction. 
										// Well, as a wise man once said, “there’s nothing to fear but fear itself and maybe some mild to moderate jellification of bones.” Besides, to borrow from the voodoo sham known as “psychiatry,” it sounds to me like these eggheads are partaking in what they’d call “projection.”
										// I'm no psychiatrist, but coming from a bunch of eggheads who wouldn’t recognize the thrill of danger if it walked up and snapped their little pink bras, that sounds like "projection". 
										// I'm no psychiatrist, but coming from a bunch of eggheads who never did anything more brave than boil some manganese - whatever the hell that accomplishes - that sounds like what they'd call "projection”.
										// I'm no psychiatrist, but coming from a bunch of eggheads who never did anything more brave than read a big book – except for Greg who fought a bear once, but he’s not the point here – that sounds like what they'd call "projection”.
										// THEY didn’t fly into space, storm a beach, or bring back the gold. No sir, we did! It's you and me against the world, son! I like your grit! Hustle could use some work, though. Now let’s solve this thing!
										// Alright. Too much jawing, not enough testing. Let’s get some science done.


	SceneTableLookup[-5063] <- "-1963_01"	// Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired. 
										// Not you, son, you're doing fine.
										// Yes, you. Fired. Box. Your stuff. Out the front door. Parking lot. Car. Goodbye.
										// Caroline, follow him out. Make sure he doesn’t cry all over the carpet.
	
	SceneTableLookup[-5064] <- "-1964_01"	// Congratulations! You made it through. The simple fact that you’re standing here listening to me means you’ve made a glorious contribution to science. You’re a national hero. We're going to take your picture, so stand there and smile.
										// Unless you were part of the Plaque Removing Deuterium Fluoride Laser test, in which case you might want to do us all a favor and keep your mouth closed until the bleeding stops.
										// And don't worry: If you're one of our DNA test subjects, and you've got a hand for a face or whatnot, we’ll photograph you from the neck down.
										// As founder and CEO of Aperture Science, I thank you for your participation and hope we can count on you for another round of tests. 
										// We’re planning some wonderful stuff with time travel. So if you never liked your father, why not come on back and try being him?
										// We’re not gonna release this stuff into the wild until it’s good and damn ready, so as long as you keep yourself in top physical form, there’ll always be a limo waiting for you.
										// Say goodbye, Caroline.
										// She is a gem.
										
// ****************************************************************************************************
// CAVE JOHNSON SCENETABLE - 1970S
// ****************************************************************************************************
	
	SceneTableLookup[-5070] <- "-1970_01"	// Greetings, friend. I'm Cave Johnson, CEO of Aperture Science—you might know us as a vital participant in the 1968 Senate Hearings on missing astronauts. And you've most likely used one of the many products we invented. But that other people have somehow managed to steal from us. Black Mesa can eat my bankrupt--
										// Sir, the testing?									
										// Right. Now, you might be asking yourself, "Cave, just how difficult are these tests? What was in that phone book of a contract I signed? Am I in danger?" Let me answer those questions with a question. Who wants to make sixty dollars? Cash.
										// You can also feel free to relax for up to 20 minutes in the waiting room, which is a damn sight more comfortable than the park benches most of you were sleeping on when we found you.
										// For many of you, I realize 60 dollars is an unprecedented windfall, so don’t go spending it all on… Caroline, what do these people buy? Tattered hats? Beard dirt? 
										// We’ve made some great advances since introducing our repulsion gel in the fifties, but you already know that, unless you’ve been living under a rock or something. Did we find any of these test subjects living under rocks? 
										// Perfect.
										// So anyway. Welcome to Aperture. You’re here because we want the best, and… Caroline, really? This is it?
										// And unfortunately, I guess you’re it. 
										// So anyway. Welcome to Aperture. You’re here because we want the best test subjects sixty dollars can buy, and you’re apparently it. 
										// So. Welcome to Aperture. You’re here because we want the best, and you’re it. Nope. Couldn’t keep a straight face. Anyway, don’t smudge up the glass down there. In fact, why don’t you just go ahead and not touch anything unless it’s test related. 						
					
	SceneTableLookup[-5099] <- "-1970_01a"
	SceneTableLookup[-5071] <- "-1971_01"	// Please help yourselves to the art on the wall. An eyeful. Don’t take it. Don’t take anything. It’s all nailed down. In fact, I take that back. Just get moving. Caroline, is the art nailed to the walls? Nevermind. Put more nails in it. Don’t get one of the lab boys to do it either. I want you to do it so it gets done right.
										
	SceneTableLookup[-5072] <- "-1972_01"	// I know I said you could loiter for up to twenty minutes. That was a possibility, not a suggestion. You can move along any time. And for god’s sake, don’t touch the upholstery.
										
	SceneTableLookup[-5073] <- "-1973_01"	// Well? Get moving. That sixty dollars isn’t hiding in here; it’s waiting for you at the other end of the enrichment sphere.
				
	SceneTableLookup[-5074] <- "-1974_01"	// The testing area’s just up ahead. The quicker you get through, the quicker you’ll get your 60 bucks. Caroline, are the compensation vouchers ready?
	SceneTableLookup[-1992] <- "-1992_01"	// Wait, I know--
		
	SceneTableLookup[-5075] <- "-1975_01"	// So, you might be wondering: why you? Simply: Human test subjects get better scientific results than animals. 
										// Why? For one, your tramps, lunatics, foundlings, what-have-you, can bring problem-solving ability to test environments with a facility that shames lower ruminants. 
										// For another, you have thumbs.
	
	SceneTableLookup[-5076] <- "-1976_01"	// Great job, astronaut, war hero and/or Olympian! With your help, we’re gonna change the world!
										// This on? [thump thump] Hey. Listen up down there. That thing up ahead’s called an elevator. Not a bathroom.
										// [sigh] Caroline, has it really come to this?
										// Damn it, you’re right. Damn good science, too. We’re not out of this race yet, Caroline.
																		
	SceneTableLookup[-5077] <- "-1977_01"	// If you’re interested in an additional sixty dollars, flag down a test associate and let ‘em know. You could walk out of here with a hundred and twenty weighing down your bindle if  you let us take you apart, put some science stuff in you, then put you back together good as new.

	SceneTableLookup[-5078] <- "-1978_01"	// In case you're interested, there're still some positions available for that bonus opportunity I mentioned earlier. Again: all you gotta do is let us disassemble you. We're not banging rocks together here. We know how to put a man back together. We'll take out any diseases we find in there, too, and replace all the organs that aren't tip top. So that's a complete reassembly. New vitals. Spit-shine on the old ones. Plus we're scooping out tumors. Frankly, you oughtta be paying us. Anyway, volunteers should line up outside Enrichment Sphere Archimedes Bravo.

	SceneTableLookup[-5079] <- "-1979_01" // Thank you – [off mic]  I can’t believe I’m thanking these people [on mic] – for staggering your way through Aperture Science’s propulsion gel testing. You’ve made some real contributions to society for a change, and for that, humanity is grateful.
										// If you had any belongings, please pick them up now. We don’t want old newspapers and sticks cluttering up the building.
										// Once you’ve gathered your things, grab a compensation voucher, good for 60 dollars worth of stuff from the Aperture Science gift shop. Please only touch items you plan on purchasing. Thanks to the IRS and our patent-stealing competitors, there are parking meters in Duluth that earn more money than we do lately. So there’s no room in the budget to wash coffee mugs that you pick up and then decide not to buy.
										// Caroline, put some extra security in the gift shop to make sure no one decides to take a nap in the t-shirt displays.
										
	
										
// ****************************************************************************************************
// CAVE JOHNSON SCENETABLE - 1980S
// ****************************************************************************************************

	SceneTableLookup[-5080] <- "-1980_01" // Welcome to the enrichment center. Since making test participation mandatory for all employees, the quality of our test subjects has risen dramatically. Employee retention, however, has not.
										// As a result, you may have heard we’re gonna phase out human testing. There’s still a few things left to wrap up, though. 
										// First up, conversion gel. The bean counters told me we literally could not afford to buy seven dollars worth of moon rocks, much less seventy million. Bought ‘em anyway. Engineers said the moon rocks were too volatile to experiment on. Tested on ‘em anyway. Ground ‘em up, mixed em into a gel. 
										// And guess what? Ground up moon rocks are pure poison. I am deathly ill. Still, it turns out they’re a great portal conductor. So now we’re gonna see if jumping in and out of these new portals can somehow leech the lunar poison out of a man’s bloodstream. When life gives you lemons, make lemonade. Let's all stay positive and do some science.
										// That said, I would really appreciate it if you could test as fast as possible. Caroline, please bring me more pain pills.																											

	/*
	SceneTableLookup[-5081] <- "-1981_01" // When you’re making some great science, it helps to keep a positive attitude. Look at Marie Curie. Invented radiology. Won two Nobel prizes. Dumb as a bag of hammers. Thought the light radiation gave off was pretty, walked around with it in bottles. Buried her with a smile on her face. 
	
	SceneTableLookup[-5082] <- "-1982_01" // Let me tell you about a fella. Lived thousands of years ago. Only wanted folks to be a little nicer to each other. And in the end, he sacrificed himself to save us all. You know who I’m talking about: Hercules.
	
	SceneTableLookup[-5083] <- "-1983_01" // [snore] Hercules…
	*/
	
	SceneTableLookup[-5084] <- "-1984_01" // Allright, I've been thinking. When life gives you lemons? Don't make lemonade. Make life take the lemons back! Get mad! "I don’t want your damn lemons! What am I supposed to do with these?" 
										// Demand to see life's manager! Make life rue the day it thought it could give Cave Johnson lemons! Do you know who I am? I'm the man who's going to burn your house down! With the lemons! I'm going to get my engineers to invent a combustible lemon that burns your house down!
										// [coughing fit]
										// The point is: if we can store music on a compact disc, why can’t we store a man’s intelligence and personality on one? So I have the engineers figuring that out now. 
										// Brain Mapping. Artificial Intelligence. We should have been working on it thirty years ago. But I guess it’s too late for should haves and what ifs. I will say this - and I’m gonna say it on tape so everybody hears it a hundred times a day: if I die before you people can pour me into a computer, I want Caroline to run this place.
										// She’ll argue. She’ll say she can’t. She’s modest like that. But you make her. Treat her just like you’d treat me. 
										// Hell, put her in my computer. I don’t care. Just make sure she’s taken care of.
										// Allright, test’s over. You can head on back to your desk.

	SceneTableLookup[-1995] <- "-1995_01" // Yeah!
	SceneTableLookup[-1996] <- "-1996_01" // Yeah!									
	SceneTableLookup[-1997] <- "-1997_01" // Yeah!	
	SceneTableLookup[-1998] <- "-1998_01" // Yeah, take the lemons...     
	SceneTableLookup[-1999] <- "-1999_01" // BURN HIS HOUSE DOWN! 
	SceneTableLookup[-2000] <- "-2000_01" // Burn it down! Oh, I like this guy. 	
	SceneTableLookup[-2001] <- "-2001_01" // Burning people! He says what we're all thinking!     		
	SceneTableLookup[-2002] <- "-2002_01" // Yeah!	
	
// ****************************************************************************************************
// CAVE JOHNSON SCENETABLE - MISC. TESTS
// ****************************************************************************************************

SceneTableLookup[-8104] <- "-3004_01" // If you’ve cut yourself at all in the course of these tests, you might have noticed that your blood is pure gasoline. That’s normal. We’ve been shooting you with an invisible laser that’s supposed to turn blood into gasoline, so all that means is it’s working.
SceneTableLookup[-8111] <- "-3011_01" // If you need to go to the bathroom after this next series of tests, please let a test associate know, because in all likelihood, whatever comes out of you is going to be coal. Only temporary, so do not worry. If it persists for a week, though, start worrying and come see us, because that’s not supposed to happen.
SceneTableLookup[-8121] <- "-3021_01" // If you’re allergic to peanuts, you might want to tell somebody now, because the lab boys this next test may turn your blood into peanut water for a few minutes. On the bright side, if we can make this happen, they’re gonna have to invent a new type of Nobel Prize to give us, so hang in there.
SceneTableLookup[-8122] <- "-3022_01" // Now, if you’re part of Control Group seven-dee, we implanted a tiny microchip about the size of a postcard into your skull. Most likely you’ve forgotten it’s even there, but if it starts vibrating and beeping during this next test, let us know, because that means it’s about to hit five hundred degrees, so we’re gonna need to go ahead and get that out of you pretty fast.          
SceneTableLookup[-8120] <- "-3020_01" // Heads up: we’re gonna have a superconductor turned up full blast and pointed at you for the duration of this next test. I’ll be honest, we’re throwing science at the wall here to see what sticks. No idea what it’ll do. Probably nothing. Best case scenario, you might get some superpowers. Worst case, some tumors, which we’ll cut out.
SceneTableLookup[-8127] <- "-3027_01" // All right. We’re working on a little teleportation experiment. Now, this doesn’t work with all skin types, so try to remember which skin is yours, and if it doesn’t teleport along with you, we’ll do what we can to sew you right back into it.



/*								
	SceneTableLookup[-8099] <- "-2999_01"  // Once upon a time there was a scorpion and a frog. The scorpion tells the frog “When I say ‘jump’ I don’t want to hear ‘how high?’ That means you’re not jumping!” Then the frog says “It’s my nature!” Well, be that as it may, we have a lot of tests to do and you are taking a long time on these catwalks and, yes, I broke out of the parable somewhere back there. But the point stands that you need to put some muscle back into your hustle, son.

	SceneTableLookup[-8100] <- "-3000_01" // You might have noticed some safety warnings on the walls. Ignore ‘em. I keep telling the Bean Counters danger’s just a natural part of science, but boy they don’t want to hear it. Like telling a paradox to a robot – makes their heads explode.

	SceneTableLookup[-8101] <- "-3001_01" // For this next test, we put nanoparticles in the gel. In layman’s terms, that’s a billion little gizmos that are gonna travel into your bloodstream and pump experimental genes and RNA molecules and so forth into your tumors. 																										
										 // Now, maybe you don’t have any tumors. Well, don’t worry. If you sat on a folding chair in the lobby and weren’t wearing lead underpants, we took care of that too. 

	SceneTableLookup[-8102] <- "-3002_01" // That last test may have made your skeleton magnetic. I don’t know how. But if I were you, I wouldn’t stand next to any sheet metal.

	SceneTableLookup[-8103] <- "-3003_01" // Just a quick heads-up. If you suddenly find yourself able to excrete spider silk out of your fingers or the base of your spine, let one of the test associates know, cause that’s actually important.
	
	SceneTableLookup[-8104] <- "-3004_01" // If you’ve cut yourself at all in the course of these tests, you might have noticed that your blood is pure gasoline. That’s normal. We’ve been shooting you with an invisible laser that’s supposed to turn blood into gasoline, so all that means is it’s working.

	SceneTableLookup[-8105] <- "-3005_01" // Just a heads-up: That coffee we gave you earlier had fluorescent calcium in it so we can track the neuronal activity in your brain. There’s a slight chance the calcium could harden and vitrify your frontal lobe. Anyway, don’t stress yourself thinking about it. I’m serious. Visualizing the scenario while under stress actually triggers the reaction. 																										

	SceneTableLookup[-8106] <- "-3006_01" // All these science spheres are made of asbestos, by the way. Keeps out the rats. Let us know if you feel a shortness of breath, a persistent dry cough or your heart stopping, because that’s not part of the test. That’s asbestos.
										 // Good news is, the lab boys say the symptoms of asbestos poisoning show a median latency of forty-four point six years, so if you’re thirty or older, you’re laughing. Worst case scenario, you miss out on a few rounds of canasta, plus you forwarded the cause of science by three centuries. I punch those numbers into my calculator, it makes a happy face. 

	SceneTableLookup[-8107] <- "-3007_01" // The gel in this next test is probably going to make all of your skin solar-powered. Upside is, you’re not gonna get hungry. Ever. So if you were worried about your waistline, get this: You won’t be able to eat food even if you wanted to.
	
	SceneTableLookup[-8108] <- "-3008_01" // This next test involves molecular dynamics. You’re gonna breathe in a bunch of tiny little robots, and they’re gonna optimize your DNA strands. Exciting stuff. Maybe nothing’ll happen. Or maybe you’ll have a big crab hand by the end of the test, or ten foot legs. It’s up to them. Trust me, I’m as anxious as you are to see how this plays out.

	SceneTableLookup[-8109] <- "-3009_01" // The gel in this next test’s gonna recalibrate your molecular whatsamacallits, blah blah blah. Bottom line is, you won’t need to go to the bathroom anymore. Your body’s just gonna process it up like anything else. Side effect might be that everything you eat from now on tastes like urine, though.																										

	SceneTableLookup[-8110] <- "-3010_01" // If you’re not a fan of the cartilage in your body, you are gonna love what this next test might do for you. Honestly, cartilage is like adenoids, or a second kidney. Body doesn’t want it, you don’t need it. Good riddance.																										

	SceneTableLookup[-8111] <- "-3011_01" // If you need to go to the bathroom after this next series of tests, please let a test associate know, because in all likelihood, whatever comes out of you is going to be coal. Only temporary, so do not worry. If it persists for a week, though, start worrying and come see us, because that’s not supposed to happen.

	SceneTableLookup[-8112] <- "-3012_01" // Ever wondered what a parallel universe you might be like? Well, stop wondering, because you’re just Hitler. Every parallel universe ever. You. Hitler. We can’t figure it out. Anyway, if you meet him in the next test, don’t kill him. I know, I know. But trust me, it’d be bad.
	
	SceneTableLookup[-8113] <- "-3013_01" // The average human male is about sixty percent water. Far as we’re concerned, that’s a little extravagant. So if you feel a bit dehydrated in this next test, that’s normal. We’re gonna hit you with some jet engines, and see if we can’t get you down to twenty or thirty percent.

	SceneTableLookup[-8114] <- "-3014_01" // We’re gonna try and collapse three-dimensional space in this next test. We’re not gonna tell you when. Just give us a holler if everything starts looking flat.
																					
	SceneTableLookup[-8115] <- "-3015_01" // While you’re completing this next test, we’re gonna test out a new device and see if we can read your thoughts. Don’t let it throw you. Just give us a nod if we’re getting close.
										 // You’re thinking about tungsten. Tungsten or zinc. Give us a wave if we nailed it.
										 // You’re thinking about the electrolytic production of hydrogen. How close is that?
										 // You’re thinking about the color green. Greenish blue. 
										 // You might be wondering how I’m guessing what you’re thinking through a pre-recorded message. The answer: volume. I recorded a message for every word in the English language. Also: you are thinking about bib lettuce. Tell me I’m wrong.
										 // You’re still thinking about tungsten. I don’t blame you. It’s fascinating.
										 // Hey, Caroline. I think he’s thinking about you!
										 // Photosynthesis. That or fudge. The bacteria inside fudge. Nod your head if I’m getting close.

	SceneTableLookup[-8116] <- "-3016_01" // We’re gonna bombard you with a few rays in this next test. Nothing to be concerned about. We’re looking for bacterial contamination and inflammatory disease in your bowels. We put enough pathogens in the waiting room coffee to give diarrhea to an aircraft carrier, so if we don’t spot ‘em, I don’t know what to tell you.
	
	SceneTableLookup[-8117] <- "-3017_01" // This next test might sunburn your eyes a little. Ask a test associate for some sunscreen you can put on your retinas if you think your eyes aren’t man enough to handle it.

	SceneTableLookup[-8118] <- "-3018_01" // Bonus test within a test in this next test: we hid a photon somewhere in here. You find it, it’s twenty grand in your pocket. You can keep the photon, too.
	
	SceneTableLookup[-8119] <- "-3019_01" // The human body contains trace amounts of a whole hell of a lot of elements it doesn’t even seem to need. Copper, lead, silicon, cobalt, magnesium, carbon, oxygen. This next test, we’re gonna hit you with some microwaves and boil the worthless elements right out of you. Current hypothesis is it’s not even gonna break your stride. Honestly, what the hell has cobalt ever done for you? Good riddance.
																								
	SceneTableLookup[-8120] <- "-3020_01" // Heads up: we’re gonna have a superconductor turned up full blast and pointed at you for the duration of this next test. I’ll be honest, we’re throwing science at the wall here to see what sticks. No idea what it’ll do. Probably nothing. Best case scenario, you might get some superpowers. Worst case, some tumors, which we’ll cut out.

	SceneTableLookup[-8121] <- "-3021_01" // If you’re allergic to peanuts, you might want to tell somebody now, because the lab boys this next test may turn your blood into peanut water for a few minutes. On the bright side, if we can make this happen, they’re gonna have to invent a new type of Nobel Prize to give us, so hang in there.
	
	SceneTableLookup[-8122] <- "-3022_01" // Now, if you’re part of Control Group seven-dee, we implanted a tiny microchip about the size of a postcard into your skull. Most likely you’ve forgotten it’s even there, but if it starts vibrating and beeping during this next test, let us know, because that means it’s about to hit five hundred degrees, so we’re gonna need to go ahead and get that out of you pretty fast. 

	SceneTableLookup[-8123] <- "-3023_01" // With your help, we might eradicate heart and lung disease with this next test. Full disclosure: we’re going to achieve that by trying as hard as we can to make your heart and lungs stop working, then pump you full of some medicine we’ve been working on. Footnote to the disclosure: You’re incredibly brave and we’re proud of you.
																								
	SceneTableLookup[-8124] <- "-3024_01" // Alright, this next test, I had to fight really hard for. None of the eggheads thought it was safe enough. I keep telling ‘em danger’s the best part of science, but they don’t want to hear it. It’s like telling a paradox to a robot: Makes their heads explode.
	
	SceneTableLookup[-8125] <- "-3025_01" // Alright, this next test, I had to fight really hard for. None of the eggheads thought it was safe enough. Did you? Ha. Caroline, watch this. 
										 // You, stand next to him. You with the big head, over there. That’s right, all in a line against the wall. Okay, here we go. 
										 // You’re fired you’re fired you’re fired you’re fired you’re fired.
	
	SceneTableLookup[-8126] <- "-3026_01" // Don’t eyeball me, son.
										 // Don’t snow me. I know an eyeball when I see one. It’s not good, wholesome eye-looking and I won’t stand for it. Eyeballs shouldn’t judge. They just send the information up for the brain to deal with. That’s the job. Get your eyes fixed, son. 
										 // Caroline. Make appointments with all my engineers for eye surgery.
										 // Sign me up for it, too. From now on, everybody’s doing their eye-looking properly.

	SceneTableLookup[-8127] <- "-3027_01" // All right. We’re working on a little teleportation experiment. Now, this doesn’t work with all skin types, so try to remember which skin is yours, and if it doesn’t teleport along with you, we’ll do what we can to sew you right back into it.
																										
	SceneTableLookup[-8128] <- "-3028_01" // For this next test, we’ll be firing a few isotopes at your eyes to improve your night vision. If you feel a throbbing behind your eyes, that’s normal and should subside within a few hours. If you feel a pecking behind your eyes, that means a small bird got in there. We don’t know how it happens, but we’re working on it.																									

	SceneTableLookup[-8129] <- "-3029_01" // How do you feel about your feet? Like ‘em? Good, because if this next test works, you’ll have five of ‘em.

	SceneTableLookup[-8130] <- "-3030_01" // For this next test, you’ll be helping us solve the problem of male pattern baldness. If it works, you’re gonna start growing lush, new hair instantly. Downside: teeth hair.
	
	SceneTableLookup[-8131] <- "-3031_01" // You want to keep stealing my patents? Fine! This is war. Science war. The sweet science. 
										 // You will cry into your evil black satin pillows about the day you messed with Cave Johnson! 
										 // You hear me? I invented portals! I can put a doorway on the moon and another into your parking lot! Let’s see how many patents you steal when you’re floating around in outer space, you—
										 // What? Right. I know. Felt like having it on. How can… you tell?
										 // Fantastic. Good business. And if I wanted to turn it off—
										 // Good, good. Glad it was on. On purpose. And if I wanted to turn it off—
										 // Bam. Right. Good stuff, I like it. Let’s keep the switch. That’ll be all, Caroline.
										 // [click]
	
	SceneTableLookup[-8132] <- "-3032_01" // Caroline, hold my calls. I need to go see a man about a mule.
										 // What was that?
										 // Alright. Good. Mule. Gonna go see him in the bathroom. Back in ten.

*/

										 

// ****************************************************************************************************
// "Welcome to Aperture," 1950s
// ****************************************************************************************************
	
//SCENE: "Welcome, gentlemen, to Aperture Science..."
	if (curMapName=="sp_a3_03")
	{
		
		//vitrified test chamber #1
		SceneTable ["-3004_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests06.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		//vitrified test chamber #2
		SceneTable ["-3011_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests14.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		//vitrified test chamber #3
		SceneTable ["-3020_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests24.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}



		// Welcome, gentlemen, to Aperture Science. Astronauts, war heroes, Olympians--you're here because we want the best, and you are it. So: Who is ready to make some science?
		SceneTable ["-1950_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_intro01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.4
		next = "-1950_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// I am!
		SceneTable ["-1950_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses25.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1950_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
		// [laughing] Now, you already met one another on the limo ride over, so let me introduce myself.
		SceneTable ["-1950_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_intro03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1950_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// I’m Cave Johnson. I own the place. 
		SceneTable ["-1950_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_intro04.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1950_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// That eager voice you heard is the lovely Caroline, my assistant. Rest assured, she has transferred your honorarium to the charitable organization of your choice. Isn’t that right, Caroline? 
		SceneTable ["-1950_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_intro06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1950_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		//Yes sir, Mister Johnson!
		SceneTable ["-1950_06" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses43.vcd") 
		char = "cave"
		postdelay = 0.1
		predelay = 0.1
		next = "-1950_08"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// She's the backbone of this facility. Pretty as a postcard, too. Sorry, fellas. She's married. To science.
		SceneTable ["-1950_08" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_intro07.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		

		
		// There’s a thousand tests performed every day here in our enrichment spheres. I can’t personally oversee every one of them, so these pre-recorded messages’ll cover any questions you might have, and respond to any incidents that may occur in the course of your science adventure.
		SceneTable ["-1951_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_waiting01.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1951_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Your test assignment will vary, depending on the manner in which you have bent the world to your will.
		SceneTable ["-1951_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_waiting03.vcd") 
		char = "cave"
		postdelay = 0.6
		predelay = 0.0
		next = "-1951_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Those of you helping us test the repulsion gel today, just follow the blue line on the floor. 
		SceneTable ["-1951_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_waiting05.vcd") 
		char = "cave"
		postdelay = 0.6
		predelay = 0.0
		next = "-1951_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Those of you who volunteered to be injected with praying mantis DNA, I’ve got some good news and some bad news.
		SceneTable ["-1951_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_waiting07.vcd") 
		char = "cave"
		postdelay = 0.1
		predelay = 0.0
		next = "-1951_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Bad news is we’re postponing those tests indefinitely. Good news is we’ve got a much better test for you: fighting an army of mantis men. Pick up a rifle and follow the yellow line. You’ll know when the test starts.
		SceneTable ["-1951_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_waiting08.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// They say great science is built on the shoulders of giants. Not here. At Aperture, we do all our science from scratch. No hand holding.
		SceneTable ["-1952_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_elevator_out_a01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]				
		}	
	}

// ****************************************************************************************************
// "Repulsion Gel"
// ****************************************************************************************************
	
	if (curMapName=="sp_a3_jump_intro")
	{
	
		// Alright, let’s get started. This first test involves something the lab boys call "repulsion gel." 
		SceneTable ["-1953_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_repulsion_intro02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1953_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	
		// You're not part of the control group, by the way. You get the gel. Last poor son of a gun got blue paint. Hahaha. All joking aside, that did happen – broke every bone in his legs. Tragic. But informative. Or so I’m told.
		SceneTable ["-1953_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_repulsion_intro03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}	

	
		// The lab boys just informed me that I should not have mentioned the control group. They're telling me I oughtta stop making these pre-recorded messages. That gave me an idea: make more pre-recorded messages. I pay the bills here, I can talk about the control group all damn day. 
		SceneTable ["-1954_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_second_test_intro01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	
	/*
	
		// Now, the lab boys were adamant that I do not give you any hints on these tests. To be honest, they think I'm spoiling the results just by talking to you. Hizenstein Uncertainty Principles and so forth. I’ll give ‘em something more practical to be uncertain about. Their next paycheck. 
		SceneTable ["-1954_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_first_test_exit01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1954_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Anyway, overruled. If you think I'm affecting your decisions, in any way, don’t be afraid to speak up. I’m not made of glass. 
		SceneTable ["-1954_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_first_test_exit02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
	
	
		// That reminds me: Caroline, Do we have a wing made out of glass yet?
		SceneTable ["-1954_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_first_test_exit05.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1954_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Let's get on that, Caroline.
		SceneTable ["-1954_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_first_test_exit06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	*/	

		// For this next test, we put nanoparticles in the gel. In layman’s terms, that’s a billion little gizmos that are gonna travel into your bloodstream and pump experimental genes and RNA molecules and so forth into your tumors. 																										
		SceneTable ["-1955_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests02.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1955_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// Now, maybe you don’t have any tumors. Well, don’t worry. If you sat on a folding chair in the lobby and weren’t wearing lead underpants, we took care of that too. 
		SceneTable ["-1955_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Oh! In case you got covered in that repulsion gel, here’s some advice the lab boys gave me: Do not get covered in the repulsion gel.
		SceneTable ["-1956_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_second_test_complete03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1956_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	
		// We haven’t entirely nailed down what element it is yet, but I’ll tell you this: it’s a lively one, and it does NOT like the human skeleton.
		SceneTable ["-1956_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_second_test_complete02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]				
		}

	
	}

// ****************************************************************************************************
// "Bomb Flings"
// ****************************************************************************************************

if (curMapName=="sp_a3_bomb_flings")
	{
	
		// All these science spheres are made of asbestos, by the way. Keeps out the rats. Let us know if you feel a shortness of breath, a persistent dry cough or your heart stopping, because that's not part of the test. That's asbestos.                          	
		SceneTable ["-1957_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests08.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1957_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// Good news is, the lab boys say the symptoms of asbestos poisoning show a median latency of forty-four point six years, so if you're thirty or older, you're laughing. Worst case scenario, you miss out on a few rounds of canasta, plus you forwarded the   		
		SceneTable ["-1957_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests10.vcd") 
		char = "cave"
		postdelay = 1.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Ha! I like your style. You make up your own rules, just like me. 
		SceneTable ["-1958_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_into_middle_of_test01.vcd") 
		char = "cave"
		postdelay = 0.3
		predelay = 0.0
		next = "-1958_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Bean counters said I couldn't fire a man just for being in a wheelchair. Did it anyway. Ramps are expensive. 
		SceneTable ["-1958_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_into_middle_of_test02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// Just a heads-up: That coffee we gave you earlier had fluorescent calcium in it so we can track the neuronal activity in your brain. There's a slight chance the calcium could harden and vitrify your frontal lobe. Anyway, don't stress yourself thinking   
		SceneTable ["-1959_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests07.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]		
		}
		
	}

// ****************************************************************************************************
// "Crazy Box"
// ****************************************************************************************************


if (curMapName=="sp_a3_crazy_box")
	{	
		
		// Postcard
		SceneTable ["-1960_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests26.vcd") 
		char = "cave"
		postdelay= 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}
		
	
		// I'm telling 'em, keep your pants on.
		SceneTable ["-1961_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fourth_test_complete01.vcd") 
		char = "cave"
		postdelay = 0.1
		predelay = 0.0
		next = "-1961_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Alright, this next test may involve trace amounts of time travel. So, word of advice: If you meet yourself on the testing track, don’t make eye contact. Lab boys tell me that'll wipe out time. Entirely. Forward and backward! Do both of yourselves a favor and just let that handsome devil go about his business. 
		SceneTable ["-1961_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fourth_test_complete02.vcd") 
		char = "cave"
		postdelay = 0.1
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// If you’re hearing this, it means you’re taking a long time on the catwalks between tests. The lab boys say that might be a fear reaction. 
		SceneTable ["-1962_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_intro01.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1962_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Well, as a wise man once said, “there’s nothing to fear but fear itself and maybe some mild to moderate jellification of bones.” Besides, to borrow from the voodoo sham known as “psychiatry,” it sounds to me like these eggheads are partaking in what they’d call “projection.”
		// I'm no psychiatrist, but coming from a bunch of eggheads who never did anything more brave than boil some manganese - whatever the hell that accomplishes - that sounds like what they'd call "projection”.
		// I'm no psychiatrist, but coming from a bunch of eggheads who never did anything more brave than read a big book – except for Greg who fought a bear once, but he’s not the point here – that sounds like what they'd call "projection”.

		// I'm no psychiatrist, but coming from a bunch of eggheads who wouldn’t recognize the thrill of danger if it walked up and snapped their little pink bras, that sounds like "projection". 
		SceneTable ["-1962_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_intro03.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1962_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// THEY didn’t fly into space, storm a beach, or bring back the gold. No sir, we did! It's you and me against the world, son! I like your grit! Hustle could use some work, though. Now let’s solve this thing!
		SceneTable ["-1962_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_intro06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
			
		// Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired. 
		SceneTable ["-1963_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_complete09.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1963_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Not you, test subject, you're doing fine.
		SceneTable ["-1963_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_complete03.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1963_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// Yes, you. Fired. Box. Your stuff. Out the front door. Parking lot. Car. Goodbye.
		SceneTable ["-1963_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_fifth_test_complete08.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]				
		}

	}
	
// ****************************************************************************************************
// "Thanks For Testing!" 1950s
// ****************************************************************************************************
	
if (curMapName=="sp_a3_transition01")
	{
		
		
		//vitrified test chamber #1
		SceneTable ["-3021_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests25.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		//vitrified test chamber #2
		SceneTable ["-3022_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests01.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}

		//vitrified test chamber #3
		SceneTable ["-3027_01" ] <-
		{
			vcd = CreateSceneEntity("scenes/npc/cavejohnson/misc_tests28.vcd") 
			char = "cave"
			postdelay = 0.0
			predelay = 0.0
			next = null
			noDingOff = true
			noDingOn = true
		}


	
		// Congratulations! The simple fact that you’re standing here listening to me means you’ve made a glorious contribution to science.
		SceneTable ["-1964_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_outro02.vcd") 
		char = "cave"
		postdelay = 0.1
		predelay = 0.0
		next = "-1964_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	
		// As founder and CEO of Aperture Science, I thank you for your participation and hope we can count on you for another round of tests. 
		SceneTable ["-1964_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_outro05.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1964_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
		// We’re not gonna release this stuff into the wild until it’s good and damn ready, so as long as you keep yourself in top physical form, there’ll always be a limo waiting for you.
		SceneTable ["-1964_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_outro07.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1964_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
		// Say goodbye, Caroline.
		SceneTable ["-1964_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_outro08.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1964_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
		// Say goodbye, Caroline.
		SceneTable ["-1964_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses31.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1964_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}			
		
			
		//Ha!
		SceneTable ["-1964_06" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/cave_laugh04.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1964_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// She is a gem.
		SceneTable ["-1964_06" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/fifties_outro09.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}

	
// ****************************************************************************************************
// "Welcome to Aperture," 1970s
// ****************************************************************************************************
	
		// Greetings, friend. I'm Cave Johnson, CEO of Aperture Science—you might know us as a vital participant in the 1968 Senate Hearings on missing astronauts. And you've most likely used one of the many products we invented. But that other people have somehow managed to steal from us. Black Mesa can eat my bankrupt--
		SceneTable ["-1970_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro16.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover = true
		fires=
		[
			{entity="@glados",input="runscriptcode",parameter="sp_a3_transition01_70sintropart2()",fireatstart=true,delay=17.8}
		]

		}
		
		// Sir. The testing?
		SceneTable ["-1970_01a" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses36.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1970_02"
		noDingOff = true
		noDingOn = true
		talkover = true
		}
		
		
		// Right. Now, you might be asking yourself, "Cave, just how difficult are these tests? What was in that phone book of a contract I signed? Am I in danger?" 
		SceneTable ["-1970_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1970_02a"
		noDingOff = true
		noDingOn = true
		talkover = true
		}
		
		// Let me answer those questions with a question. Who wants to make sixty dollars? Cash.
		SceneTable ["-1970_02a" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro04.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1970_03"
		noDingOff = true
		noDingOn = true
		talkover = true
		}
		
		// You can also feel free to relax for up to 20 minutes in the waiting room, which is a damn sight more comfortable than the park benches most of you were sleeping on when we found you.
		SceneTable ["-1970_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro06.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1970_04"
		noDingOff = true
		noDingOn = true
		talkover = true
		}
			
		// So. Welcome to Aperture. You’re here because we want the best, and you’re it. Nope. Couldn’t keep a straight face. 
		SceneTable ["-1970_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro14.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1970_05"
		noDingOff = true
		noDingOn = true
		talkover = true
		}
		
		// Anyway, don’t smudge up the glass down there. In fact, why don’t you just go ahead and not touch anything unless it’s test related. 						
		SceneTable ["-1970_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro15.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover = true
		fires=
		[
			{entity="offices_caveaudio_trigger",input="enable",parameter="",delay=0}
		]
		}
		

	}	



// ****************************************************************************************************
// "Speed Ramp"
// ****************************************************************************************************

if (curMapName=="sp_a3_speed_ramp")
	{
	
		// The testing area’s just up ahead. The quicker you get through, the quicker you’ll get your 60 bucks. 
		SceneTable ["-1974_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_intro01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1974_01a"
		noDingOff = true
		noDingOn = true	
		fires=
			[
				{entity="@glados",input="runscriptcode",parameter="PotatosTurnOn()",fireatstart=true,delay=0.0}
			]
		}
		
		// Hold on, who...?
		SceneTable ["-1974_01a" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_speed_ramp_hearcave02.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = "-1974_02"
		noDingOff = true
		noDingOn = true
		}	
		
		// Caroline, are the compensation vouchers ready?
		SceneTable ["-1974_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_intro02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1974_03"
		noDingOff = true
		noDingOn = true
		}
		
		// Yes SIR, Mister Johnson.
		SceneTable ["-1974_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses27.vcd") 
		char = "cave"
		postdelay = -0.02
		predelay = 0.0
		next = "-1974_04"
		noDingOff = true
		noDingOn = true
		}	

		// Yes SIR, Mister Johnson.
		SceneTable ["-1974_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_remembering_caroline03.vcd") 
		char = "glados"
		postdelay = 0.75
		predelay = 0.0
		next = "-1974_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		

		// Why did I just-Who is that? What the HELL is going on h-[shorts out]       
		SceneTable ["-1974_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_speed_ramp_hearcaroline03.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		fires=
			[
				{entity="@glados",input="runscriptcode",parameter="PotatosTurnOff()",fireatstart=true,delay=3.5}
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_speed_flings_zap()",delay=3.5,fireatstart=true},
			]
		}	
         
		// Okay. I guess emotional outbursts require more than one point six volts. Now we know that. We just need to relax. We're still going to find out what the hell's going on here.     
		SceneTable ["-1975_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_sp_a3_speed_ramp_wakeup01.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		fires=
			[
				{entity="@glados",input="runscriptcode",parameter="PotatosTurnOn()",fireatstart=true,delay=0.0}
			]
		}	
 
			
	// Great job, astronaut, war hero and/or Olympian! With your help, we’re gonna change the world!
		SceneTable ["-1976_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_complete06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1976_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// This on? [thump thump] Hey. Listen up down there. That thing’s called an elevator. Not a bathroom.
		SceneTable ["-1976_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_complete05.vcd") 
		char = "cave"
		postdelay = 0.3
		predelay = 0.0
		next = "-1976_03"
		noDingOff = true
		noDingOn = true
		talkover=true
	
		}
		
		// I swear I know him...
		SceneTable ["-1976_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_hearcave_b01.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]			
		}
		
	/*	
		// [sigh] Caroline, has it really come to this?
		SceneTable ["-1975_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_complete03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1975_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
		// [sigh] Caroline, has it really come to this?
		SceneTable ["-1975_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_complete03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1975_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}			
		
		// We're still doing science, sir.
		SceneTable ["-1975_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses46.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1975_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// Damn it, you’re right. Damn good science, too. We’re not out of this race yet, Caroline.
		SceneTable ["-1975_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_a_complete04.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1975_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}		
		
	*/	
		
	}
	

// ****************************************************************************************************
// "Speed Flings"
// ****************************************************************************************************

if (curMapName=="sp_a3_speed_flings")
	{

		// If you’re interested in an additional sixty dollars, flag down a test associate and let ‘em know. You could walk out of here with a hundred and twenty weighing down your bindle if  you let us take you apart, put some science stuff in you, then put you back together good as new.
		SceneTable ["-1977_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_b_complete01.vcd") 
		char = "cave"
		postdelay = 20.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}
	
		/*
		// "Right away, Mister Johnson..."   
		SceneTable["-1977_02"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_remembering_caroline04.vcd"),
			postdelay = 25.0
			next = "-1977_03"
			char = "glados"
			noDingOff = true
			noDingOn = true
		}
		
		//"No one's going to miss a few astronauts, Mister Johnson..."     
		SceneTable["-1977_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_remembering_caroline06.vcd"),
			postdelay = 25.0
			next = null
			char = "glados"
			noDingOff = true
			noDingOn = true
		}
		
		//"Hide the bodies, Mister Johnson..."     
		SceneTable["-1977_03"] <-
		{
			vcd = CreateSceneEntity("scenes/npc/glados/potatos_remembering_caroline05.vcd"),
			postdelay = 0.0
			next = null
			char = "glados"
			noDingOff = true
			noDingOn = true
		}
		*/
	
		// In case you're interested, there're still some positions available for that bonus opportunity I mentioned earlier. Again: all you gotta do is let us disassemble you. We're not banging rocks together here. We know how to put a man back together.
		SceneTable ["-1978_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_b_complete03.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1978_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// So that's a complete reassembly. New vitals. Spit-shine on the old ones. Plus we're scooping out tumors. Frankly, you oughtta be paying us. 
		SceneTable ["-1978_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_test_b_complete04.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@transition_script",input="RunScriptCode",parameter="TransitionReady()",delay=0.0},
			]				
		}
		
	}
	
// ****************************************************************************************************
// "Thanks For Testing!" 1970s
// ****************************************************************************************************
	
if (curMapName=="sp_a3_portal_intro")
	{

		// Thank you – [off mic]  I can’t believe I’m thanking these people [on mic] – for staggering your way through Aperture Science’s propulsion gel testing. You’ve made some real contributions to society for a change, and for that, humanity is grateful.
		SceneTable ["-1979_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_outro01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1979_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}

		// If you had any belongings, please pick them up now. We don’t want old newspapers and sticks cluttering up the building.
		SceneTable ["-1979_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_outro02.vcd") 
		char = "cave"
		postdelay = 0.2
		predelay = 0.0
		next = "-1979_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// For many of you, I realize 60 dollars is an unprecedented windfall, so don’t go spending it all on… Caroline, what do these people buy? Tattered hats? Beard dirt? 
		SceneTable ["-1979_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/seventies_intro07.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}

// ****************************************************************************************************
// "Welcome to Aperture," 1980s
// ****************************************************************************************************
	
		// Welcome to the enrichment center. 
		SceneTable ["-1980_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro01.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1980_01a"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Since making test participation mandatory for all employees, the quality of our test subjects has risen dramatically. Employee retention, however, has not.
		SceneTable ["-1980_01a" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro03.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
	
		// As a result, you may have heard we’re gonna phase out human testing. There’s still a few things left to wrap up, though. 
		SceneTable ["-1980_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro04.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_03"
		noDingOff = true
		noDingOn = true
		talkover=true
		}			
		
		// First up, conversion gel. 
		SceneTable ["-1980_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// The bean counters told me we literally could not afford to buy seven dollars worth of moon rocks, much less seventy million. Bought ‘em anyway. Engineers said the moon rocks were too volatile to experiment on. Tested on ‘em anyway. Ground ‘em up, mixed em into a gel. 
		SceneTable ["-1980_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro07.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}
		
		// And guess what? Ground up moon rocks are pure poison. I am deathly ill. 
		SceneTable ["-1980_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro08.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_04a"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Still, it turns out they’re a great portal conductor. So now we’re gonna see if jumping in and out of these new portals can somehow leech the lunar poison out of a man’s bloodstream. When life gives you lemons, make lemonade. Let's all stay positive and do some science.
		SceneTable ["-1980_04a" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro10.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1980_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// That said, I would really appreciate it if you could test as fast as possible. Caroline, please bring me more pain pills.																											
		SceneTable ["-1980_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_intro12.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1980_06" 
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		/*
		// Yes, Mister Johnson.
		SceneTable ["-1980_06" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/caroline_cave_responses39.vcd") 
		char = "cave"
		postdelay = -0.5
		predelay = 0.0
		next = "-1980_07"
		noDingOff = true
		noDingOn = true
		}		
		
		// Yes, Mister Johnson.
		SceneTable ["-1980_07" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_responses01.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		*/
		
		// When you’re making some great science, it helps to keep a positive attitude. Look at Marie Curie. Invented radiology. Won two Nobel prizes. Dumb as a bag of hammers. Thought the light radiation gave off was pretty, walked around with it in bottles. Buried her with a smile on her face. 
		// Let me tell you about a fella. Lived thousands of years ago. Only wanted folks to be a little nicer to each other. And in the end, he sacrificed himself to save us all. You know who I’m talking about: Hercules.
		// [snore] Hercules…

// ****************************************************************************************************
// Cave's Death Speech -- Potatos Interjections
// ****************************************************************************************************

		// Yeah!
		SceneTable ["-1995_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions08.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}		
		
		// Yeah!
		SceneTable ["-1996_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions06.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}	
		
		// Yeah!
		SceneTable ["-1997_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions07.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}	
		
		// Yeah, take the lemons!
		SceneTable ["-1998_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions02.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}
		
		// BURN HIS HOUSE DOWN!
		SceneTable ["-1999_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions03.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}		
	
		// BURN IT DOWN oh I like this guy.
		SceneTable ["-2000_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions09.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}	

		// Burning people! He says what we're all thinking!
		SceneTable ["-2001_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions10.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}	

		// Yeah!
		SceneTable ["-2002_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cave_deathspeech_reactions05.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.0
		next = null
		noDingOff = true
		noDingOn = true
		}			
		
		
// ****************************************************************************************************
// Cave's Death Speech
// ****************************************************************************************************
	
		// Allright, I've been thinking. When life gives you lemons? Don't make lemonade. Make life take the lemons back! Get mad! "I don’t want your damn lemons! What am I supposed to do with these?" 																									
		SceneTable ["-1984_01" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro09.vcd") 
		char = "cave"
		postdelay = 0.4
		predelay = 0.0
		next = "-1984_02"
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_yeah01()",fireatstart=true, delay=4.7 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_yeah02()",fireatstart=true, delay=6.9 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_yeah03()",fireatstart=true, delay=8.0 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_take_the_lemons()",fireatstart=true, delay=11.2 },			
			]	
		}	
															
		// Demand to see life's manager! Make life rue the day it thought it could give Cave Johnson lemons! Do you know who I am? I'm the man who's going to burn your house down! With the lemons! I'm going to get my engineers to invent a combustible lemon that burns your house down!
		SceneTable ["-1984_02" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro11.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1984_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		fires=
			[
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_yeah04()",fireatstart=true, delay=1.6 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_like_this_guy()",fireatstart=true, delay=12.1 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_burn_house_down()",fireatstart=true, delay=18.0 },
				{entity="@glados",input="RunScriptCode",parameter="sp_a3_portal_intro_thinking()",fireatstart=true, delay=19.9 },			
			]	
		}	
		/*
		// [coughing fit]
		SceneTable ["-1984_03" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro01.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1984_04"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		*/
		// The point is: if we can store music on a compact disc, why can’t we store a man’s intelligence and personality on one? So I have the engineers figuring that out now. 
		SceneTable ["-1984_04" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro02.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 2.4
		next = "-1984_05"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Brain Mapping. Artificial Intelligence. We should have been working on it thirty years ago. But I guess it’s too late for should haves and what ifs. I will say this - and I’m gonna say it on tape so everybody hears it a hundred times a day: if I die before you people can pour me into a computer, I want Caroline to run this place.
		SceneTable ["-1984_05" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro05.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1984_06"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
			
		// She’ll argue. She’ll say she can’t. She’s modest like that. But you make her. Treat her just like you’d treat me. 
		SceneTable ["-1984_06" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro06.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.2
		next = "-1984_07"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
	
		// Hell, put her in my computer. I don’t care. Just make sure she’s taken care of.
		SceneTable ["-1984_07" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro07.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.0
		next = "-1984_08"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
         			
         			
		// Allright, test’s over. You can head on back to your desk.
		SceneTable ["-1984_08" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/cavejohnson/eighties_outro08.vcd") 
		char = "cave"
		postdelay = 0.0
		predelay = 0.5
		next = "-1984_09"
		noDingOff = true
		noDingOn = true
		talkover=true
		}	
		
		// Goodbye, sir.
		SceneTable ["-1984_09" ] <-
		{
		vcd = CreateSceneEntity("scenes/npc/glados/potatos_cavejohnsonmeeting12.vcd") 
		char = "glados"
		postdelay = 0.0
		predelay = 0.4
		next = null
		noDingOff = true
		noDingOn = true
		talkover=true

		}			
                                
	}	
	
// ============================================================================
// Cave functions
// ============================================================================

	
function sp_a3_03_main_liftshaft_area()
{
	GladosPlayVcd( -5050 )
}

function sp_a3_03_waiting_room()
{
	GladosPlayVcd( -5051 )
}

function sp_a3_03_exit()
{
	GladosPlayVcd( -5052 )
}

function sp_a3_jump_intro_entrance()
{
	GladosPlayVcd( -5053 )
}

function sp_a3_jump_intro_interchamber()
{
	GladosPlayVcd( -5054 )
}

function sp_a3_jump_intro_2ndchamber()
{
	GladosPlayVcd( -5055 )
}

function sp_a3_jump_intro_exit()
{
	GladosPlayVcd( -5056 )
}

function sp_a3_bomb_flings_entrance()
{
	GladosPlayVcd( -5057 )
}

function sp_a3_bomb_flings_chamber()
{
	nuke()
	GladosPlayVcd( -5058 )
}

function sp_a3_bomb_flings_exit()
{
	GladosPlayVcd( -5059 )
}

function sp_a3_crazy_box_entrance()
{
	GladosPlayVcd( -5060 )
}

function sp_a3_crazy_box_dummy_chamber()
{
	nuke()
	GladosPlayVcd( -5061 )
}

function sp_a3_crazy_box_2nd_chamber()
{
	GladosPlayVcd( -5062 )
}

function sp_a3_crazy_box_exit()
{
	GladosPlayVcd( -5063 )
}


function sp_a3_transition01_dummy_exit()
{
	GladosPlayVcd( -5064 )
}

function sp_a3_transition01_welcome()
{
	GladosPlayVcd( -5070 )
}

/*
function sp_a3_transition01_waitingroom()
{
	GladosPlayVcd( -5071 )
}
	
//walking into the main entrance of the office building
//function sp_a3_transition01_officenag1()
//{
//	GladosPlayVcd( -5073 )
//}
	

function sp_a3_transition01_officenag2()
{
	GladosPlayVcd( -5072 )
}

function sp_a3_transition01_officenag3()
{
	GladosPlayVcd( -5073 )
}
*/

function sp_a3_speed_ramp_entrance()
{
	GladosPlayVcd( -5074 )
}

function sp_a3_speed_ramp_inter_chamber()
{
	GladosPlayVcd( -5075 )
}

function sp_a3_speed_ramp_exit()
{
	GladosPlayVcd( -5076 )
}

function sp_a3_speed_flings_entrance()
{
	GladosPlayVcd( -5077 )
}

function sp_a3_speed_flings_exit()
{
	GladosPlayVcd( -5078 )
}

function sp_a3_portal_intro_entrance()
{
	GladosPlayVcd( -5079 )
}

function sp_a3_portal_intro_office()
{
	GladosPlayVcd( -5080 )
}

/*
function sp_a3_portal_intro_whitepaint()
{
	GladosPlayVcd( -5081 )
}
*/

function sp_a3_portal_intro_exit()
{
	GladosPlayVcd( -5084 )
}

//Bonus chambers
function sp_a3_03_dummy()
{
	nuke()
	GladosPlayVcd(-8104)
}

function sp_a3_03_dummy2() 
{
	nuke()
	GladosPlayVcd(-8111)
}

function sp_a3_03_dummy3() 
{
	nuke()
	GladosPlayVcd(-8120)
}

function sp_a3_transition01_dummy()
{
	nuke()
	GladosPlayVcd(-8121)
}

function sp_a3_transition01_dummy2()
{
	nuke()
	GladosPlayVcd(-8122)

}

function sp_a3_transition01_dummy3()
{
	nuke()
	GladosPlayVcd(-8127)
}

function sp_a3_speed_ramp_glados_what()
{
//	GladosPlayVcd(-1992)
}

function sp_a3_portal_intro_yeah01()
{
	GladosPlayVcd(-1995)
}

function sp_a3_portal_intro_yeah02()
{
	GladosPlayVcd(-1996)
}

function sp_a3_portal_intro_yeah03()
{
	GladosPlayVcd(-1997)
}

function sp_a3_portal_intro_take_the_lemons()
{
	GladosPlayVcd(-1998)
}

function sp_a3_portal_intro_burn_house_down()
{
	GladosPlayVcd(-1999)
}

function sp_a3_portal_intro_like_this_guy()
{
	GladosPlayVcd(-2000)
}

function sp_a3_portal_intro_thinking()
{
	GladosPlayVcd(-2001)
}

function sp_a3_portal_intro_yeah04()
{
	GladosPlayVcd(-2002)
}

function sp_a3_transition01_back_on_track()
{
//	GladosPlayVcd(-2003)
}

function sp_a3_speed_flings_zap()
{
	self.EmitSound("World.GladosPotatoZap")
}


function sp_a3_transition01_70sintropart2()
{
	if (!sp_a3_transition01_stopCave70sIntro)
	{
		GladosPlayVcd(-5099)
	}
}