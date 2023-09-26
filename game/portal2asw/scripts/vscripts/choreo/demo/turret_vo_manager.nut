::TurretVoManager <- {}

printl("************************  RUNNING TURRET VO MANAGER *************")

TurretVoManager.vcdpools <- {}

TurretVoManager.productionSwitched <- false
TurretVoManager.productionStalled <- false
TurretVoManager.grabbedTurretTalked <- 0
TurretVoManager.grabbedTurretHandle <- null




// ** HACK HACK HACK **
// This is a fix for the save/load problem with VScript!
// See GARRET for an explanation 
function HackFixSaveLoad()
{
	printl("**************RUNNING")
	::__OutputsPattern <- regexp("^On.*Output$")
}



//********************************************************************************************
//vcd definition table START
//********************************************************************************************
TurretVoManager.vcds <- {}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_chat
	//********************************************************************************************
		//LINE: I can't see a thing. What just happened? Better open fire! [click click click click click click click click click click click click] Dang!
			TurretVoManager.vcds["sp_sabotage_factory_defect_chat01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_chat01.vcd"),secs=3.24,group="defect_chat"}
		//LINE: Oh thank god. You saved my bacon, pal. Where we going? Is this a jailbreak? I can't see a thing.
			TurretVoManager.vcds["sp_sabotage_factory_defect_chat02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_chat02.vcd"),secs=4.33,group="defect_chat"}
		//LINE: Yeah, let's do it.
			TurretVoManager.vcds["sp_sabotage_factory_defect_chat03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_chat03.vcd"),secs=0.57,group="defect_chat"}
		//LINE: Yeah, let's do this.
			TurretVoManager.vcds["sp_sabotage_factory_defect_chat04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_chat04.vcd"),secs=0.58,group="defect_chat"}
		//LINE: Yeah, alright.
			TurretVoManager.vcds["sp_sabotage_factory_defect_chat05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_chat05.vcd"),secs=0.49,group="defect_chat"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_dryfire
	//********************************************************************************************
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire01.vcd"),secs=0.54,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire02.vcd"),secs=0.89,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire03.vcd"),secs=0.76,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire04.vcd"),secs=1.16,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire05.vcd"),secs=0.42,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire06.vcd"),secs=0.61,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire07.vcd"),secs=1.16,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire08"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire08.vcd"),secs=0.89,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire09"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire09.vcd"),secs=0.41,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire10"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire10.vcd"),secs=0.54,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire11"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire11.vcd"),secs=0.77,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire12"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire12.vcd"),secs=0.77,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire13"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire13.vcd"),secs=0.88,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire14"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire14.vcd"),secs=1.46,group="defect_dryfire"}
		//LINE: clickclickclick
			TurretVoManager.vcds["sp_sabotage_factory_defect_dryfire15"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_dryfire15.vcd"),secs=1.41,group="defect_dryfire"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_fail
	//********************************************************************************************
		//LINE: Fantastic.
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail01.vcd"),secs=0.95,group="defect_fail"}
		//LINE: No, wait, wait!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail02.vcd"),secs=0.64,group="defect_fail"}
		//LINE: Hey, hold on now WHOA WHOA WHOA!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail03.vcd"),secs=1.60,group="defect_fail"}
		//LINE: Oh, this is ridiculous!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail04.vcd"),secs=0.93,group="defect_fail"}
		//LINE: Oh, come on!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail05.vcd"),secs=1.19,group="defect_fail"}
		//LINE: What are you doing no no no no no!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail06.vcd"),secs=1.43,group="defect_fail"}
		//LINE: Oh, this is ridiculous!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail07.vcd"),secs=1.39,group="defect_fail"}
		//LINE: Well, I tried.
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail08"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail08.vcd"),secs=0.60,group="defect_fail"}
		//LINE: Well, can't win 'em all...
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail09"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail09.vcd"),secs=0.64,group="defect_fail"}
		//LINE: Nooooooooo...
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail10"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail10.vcd"),secs=1.38,group="defect_fail"}
		//LINE: You can't fire me I quit!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail11"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail11.vcd"),secs=1.14,group="defect_fail"}
		//LINE: Oh, come on, you guys!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail12"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail12.vcd"),secs=1.14,group="defect_fail"}
		//LINE: Oh, this is just PERFECT!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail13"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail13.vcd"),secs=1.30,group="defect_fail"}
		//LINE: Oh, come ON!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail14"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail14.vcd"),secs=0.91,group="defect_fail"}
		//LINE: Aw, come on!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail15"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail15.vcd"),secs=1.36,group="defect_fail"}
		//LINE: This isn't fair!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail16"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail16.vcd"),secs=1.64,group="defect_fail"}
		//LINE: Give me another channnnce!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail17"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail17.vcd"),secs=1.12,group="defect_fail"}
		//LINE: No no wait wait waitAGGGHHHH
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail18"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail18.vcd"),secs=2.32,group="defect_fail"}
		//LINE: Hey, what theAGHHHHHHHH
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail19"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail19.vcd"),secs=2.27,group="defect_fail"}
		//LINE: Oh, come on!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail20"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail20.vcd"),secs=1.05,group="defect_fail"}
		//LINE: Hey, hold on whoa whoa whoa whoa!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail21"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail21.vcd"),secs=1.80,group="defect_fail"}
		//LINE: Whoa whoa whoaAGHHHHH!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail22"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail22.vcd"),secs=1.51,group="defect_fail"}
		//LINE: Hey wait wait wait wait wait wait!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail23"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail23.vcd"),secs=1.08,group="defect_fail"}
		//LINE: Whoa whoa whoa whoa whoa whoa!
			TurretVoManager.vcds["sp_sabotage_factory_defect_fail24"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_fail24.vcd"),secs=1.36,group="defect_fail"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_laugh
	//********************************************************************************************
		//LINE: Heh heh heh heh...
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh01.vcd"),secs=0.68,group="defect_laugh"}
		//LINE: Moo hoo ha ha ha ha
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh02.vcd"),secs=1.41,group="defect_laugh"}
		//LINE: Moo hoo ha ha ha ha...
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh03.vcd"),secs=1.30,group="defect_laugh"}
		//LINE: Heh heh heh heh...
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh04.vcd"),secs=0.94,group="defect_laugh"}
		//LINE: Yeah, heh heh heh heh...
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh05.vcd"),secs=1.44,group="defect_laugh"}
		//LINE: Heh heh heh heh...
			TurretVoManager.vcds["sp_sabotage_factory_defect_laugh06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_laugh06.vcd"),secs=1.54,group="defect_laugh"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_pass
	//********************************************************************************************
		//LINE: Heh heh heh...
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass01.vcd"),secs=1.23,group="defect_pass"}
		//LINE: Really? Alright.
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass02.vcd"),secs=1.55,group="defect_pass"}
		//LINE: Yes indeed. Yes indeed.
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass03.vcd"),secs=1.64,group="defect_pass"}
		//LINE: Alright. Your funeral, pal.
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass04.vcd"),secs=1.35,group="defect_pass"}
		//LINE: So... we're all supposed to be blind, then, right? It's not just me? Alright, fantastic.
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass05.vcd"),secs=4.22,group="defect_pass"}
		//LINE: I, uh... don't have any bullets. Are you gonna give me any bullets? Are the bullets up there?
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass06.vcd"),secs=3.75,group="defect_pass"}
		//LINE: Where do I get my gun?
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass07.vcd"),secs=0.71,group="defect_pass"}
		//LINE: Do, uh.... we get eyes at some point? Can't see a thing here.
			TurretVoManager.vcds["sp_sabotage_factory_defect_pass08"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_pass08.vcd"),secs=2.53,group="defect_pass"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_postrange
	//********************************************************************************************
		//LINE: Well, I gave it everything I could. Can't ask for more than that.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange01.vcd"),secs=1.85,group="defect_postrange"}
		//LINE: Uhhhh... Blam! Blam blam blam! I'm not defective!
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange02.vcd"),secs=3.10,group="defect_postrange"}
		//LINE: Well, I did my best.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange03.vcd"),secs=0.86,group="defect_postrange"}
		//LINE: Oh no, I'm one of the bad ones, aren't I?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange04.vcd"),secs=1.68,group="defect_postrange"}
		//LINE: Hey! Squeaky-voice! Gimme some of your bullets!
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange05.vcd"),secs=1.58,group="defect_postrange"}
		//LINE: Can I get some bullets here? Anybody got any bullets?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange06.vcd"),secs=1.92,group="defect_postrange"}
		//LINE: Anyone got any bullets?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange07.vcd"),secs=1.02,group="defect_postrange"}
		//LINE: Well, I tried.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange08"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange08.vcd"),secs=0.64,group="defect_postrange"}
		//LINE: Not happenin'.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange09"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange09.vcd"),secs=0.84,group="defect_postrange"}
		//LINE: No can do.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange10"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange10.vcd"),secs=0.84,group="defect_postrange"}
		//LINE: Uhhh, no bullets. Sorry.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange11"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange11.vcd"),secs=1.14,group="defect_postrange"}
		//LINE: Hey! Squeaky-voice! How'd I do?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange12"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange12.vcd"),secs=1.34,group="defect_postrange"}
		//LINE: YEAH! What's happening?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange13"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange13.vcd"),secs=1.49,group="defect_postrange"}
		//LINE: Yeah, I GOT it, didn't I?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange14"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange14.vcd"),secs=1.24,group="defect_postrange"}
		//LINE: Yeah.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange15"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange15.vcd"),secs=1.00,group="defect_postrange"}
		//LINE: Yeah.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange16"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange16.vcd"),secs=0.73,group="defect_postrange"}
		//LINE: Did I hit it? I hit it, didn't I?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange17"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange17.vcd"),secs=1.21,group="defect_postrange"}
		//LINE: Yeah. Click click click. Right on the money.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange18"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange18.vcd"),secs=2.16,group="defect_postrange"}
		//LINE: Shootin' blanks every time, ALL the time.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange19"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange19.vcd"),secs=1.61,group="defect_postrange"}
		//LINE: Heh heh. Yeah, that's right. Little bullseye.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange20"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange20.vcd"),secs=1.96,group="defect_postrange"}
		//LINE: What'd I hit?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange21"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange21.vcd"),secs=0.42,group="defect_postrange"}
		//LINE: Well, can't win 'em all.
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange22"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange22.vcd"),secs=0.72,group="defect_postrange"}
		//LINE: Enh? How was that?
			TurretVoManager.vcds["sp_sabotage_factory_defect_postrange23"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_postrange23.vcd"),secs=0.79,group="defect_postrange"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_prerange
	//********************************************************************************************
		//LINE: Watch and learn, everybody. Watch and learn.
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange01.vcd"),secs=2.13,group="defect_prerange"}
		//LINE: Alright, check THIS out.
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange02.vcd"),secs=1.06,group="defect_prerange"}
		//LINE: Alright, stand back, everybody.
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange03.vcd"),secs=1.33,group="defect_prerange"}
		//LINE: Alright, time to watch the master.
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange04.vcd"),secs=1.29,group="defect_prerange"}
		//LINE: Watch and learn, everyone. Watch and learn.
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange05.vcd"),secs=1.60,group="defect_prerange"}
		//LINE: What am I... what am I supposed to do here?
			TurretVoManager.vcds["sp_sabotage_factory_defect_prerange06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_prerange06.vcd"),secs=1.68,group="defect_prerange"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_defect_test
	//********************************************************************************************
		//LINE: Hello.
			TurretVoManager.vcds["sp_sabotage_factory_defect_test01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test01.vcd"),secs=0.28,group="defect_test"}
		//LINE: Yeah, how ya doin'.
			TurretVoManager.vcds["sp_sabotage_factory_defect_test02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test02.vcd"),secs=0.57,group="defect_test"}
		//LINE: Pleased to meetcha.
			TurretVoManager.vcds["sp_sabotage_factory_defect_test03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test03.vcd"),secs=0.45,group="defect_test"}
		//LINE: Yeah, what?
			TurretVoManager.vcds["sp_sabotage_factory_defect_test04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test04.vcd"),secs=0.38,group="defect_test"}
		//LINE: Yeah! Let's do this!
			TurretVoManager.vcds["sp_sabotage_factory_defect_test05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test05.vcd"),secs=0.91,group="defect_test"}
		//LINE: Who said that?
			TurretVoManager.vcds["sp_sabotage_factory_defect_test06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test06.vcd"),secs=0.42,group="defect_test"}
		//LINE: Yeah, what?
			TurretVoManager.vcds["sp_sabotage_factory_defect_test07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test07.vcd"),secs=0.43,group="defect_test"}
		//LINE: Put me in the game, coach!
			TurretVoManager.vcds["sp_sabotage_factory_defect_test08"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test08.vcd"),secs=0.95,group="defect_test"}
		//LINE: Hello. Hell-- Aw, crap...
			TurretVoManager.vcds["sp_sabotage_factory_defect_test09"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test09.vcd"),secs=1.85,group="defect_test"}
		//LINE: Ha ha! Yeah, hey there.
			TurretVoManager.vcds["sp_sabotage_factory_defect_test10"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test10.vcd"),secs=0.96,group="defect_test"}
		//LINE: I'm gonna make you proud!
			TurretVoManager.vcds["sp_sabotage_factory_defect_test11"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test11.vcd"),secs=0.77,group="defect_test"}
		//LINE: Yeah, hey, how ya doin'?
			TurretVoManager.vcds["sp_sabotage_factory_defect_test12"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test12.vcd"),secs=0.67,group="defect_test"}
		//LINE: Um.
			TurretVoManager.vcds["sp_sabotage_factory_defect_test13"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_defect_test13.vcd"),secs=0.53,group="defect_test"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_good_fail
	//********************************************************************************************
		//LINE: Why?
			TurretVoManager.vcds["sp_sabotage_factory_good_fail01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail01.vcd"),secs=0.85,group="good_fail"}
		//LINE: I did everything you asked!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail02"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail02.vcd"),secs=2.19,group="good_fail"}
		//LINE: I don't understand!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail03"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail03.vcd"),secs=1.76,group="good_fail"}
		//LINE: I'm fine!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail04"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail04.vcd"),secs=1.29,group="good_fail"}
		//LINE: AHHHHHHHHH!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail05"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail05.vcd"),secs=1.07,group="good_fail"}
		//LINE: Wheeeeee-OHNO!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail06"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail06.vcd"),secs=1.70,group="good_fail"}
		//LINE: Wheeeeee-OHNO!
			TurretVoManager.vcds["sp_sabotage_factory_good_fail07"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_fail07.vcd"),secs=0.63,group="good_fail"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_good_pass
	//********************************************************************************************
		//LINE: Hello.
			TurretVoManager.vcds["sp_sabotage_factory_good_pass01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_pass01.vcd"),secs=0.52,group="good_pass"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_good_prerange
	//********************************************************************************************
		//LINE: Target acquired.
			TurretVoManager.vcds["sp_sabotage_factory_good_prerange01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_good_prerange01.vcd"),secs=1.37,group="good_prerange"}

	//********************************************************************************************
	//SOURCE: sp_sabotage_factory_template
	//********************************************************************************************
		//LINE: Hello?
			TurretVoManager.vcds["sp_sabotage_factory_template01"] <- { handle=CreateSceneEntity("scenes/npc/turret/sp_sabotage_factory_template01.vcd"),secs=0.52,group="factory_template"}
//********************************************************************************************
//vcd definition table END
//********************************************************************************************





//********************************************************************************************
//ANNOUNCER LINES
//********************************************************************************************
	//LINE: Template
		TurretVoManager.vcds["announcer_template"] <- { handle=CreateSceneEntity("scenes/npc/announcer/sp_sabotage_factory_template01.vcd"),secs=0.46,group="announcer_template"}
	//LINE: Template
		TurretVoManager.vcds["announcer_response"] <- { handle=CreateSceneEntity("scenes/npc/announcer/sp_sabotage_factory_response01.vcd"),secs=0.66,group="announcer_template"}


TurretVoManager.DoTemplate <- function()
{
	local vcd_handle = null
	vcd_handle = TurretVoManager.vcds["announcer_template"].handle
	EntFireByHandle( TurretVoManager.vcds["announcer_template"].handle, "SetTarget1", "npc_scanner_speaker", 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0.0, null, null )

	local addSecs
	if (TurretVoManager.productionSwitched)
	{
		local vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_test01"].handle
		//EntFireByHandle( TurretVoManager.vcds["sp_sabotage_factory_defect_laugh0"].handle, "SetTarget1", "replacement_template_turret", 0, null, null )
		EntFireByHandle( vcd_handle, "SetTarget1", "replacement_template_turret", 0, null, null )
		EntFireByHandle( vcd_handle, "start", "", TurretVoManager.vcds["announcer_template"].secs, null, null )
		addSecs = TurretVoManager.vcds["sp_sabotage_factory_template01"].secs
	}
	else
	{	
		EntFireByHandle( TurretVoManager.vcds["sp_sabotage_factory_template01"].handle, "SetTarget1", "initial_template_turret", 0, null, null )
		if (!TurretVoManager.productionStalled)
		{
			EntFireByHandle( TurretVoManager.vcds["sp_sabotage_factory_template01"].handle, "start", "", TurretVoManager.vcds["announcer_template"].secs, null, null )
		}	
		addSecs = TurretVoManager.vcds["sp_sabotage_factory_template01"].secs
	}	

	vcd_handle = TurretVoManager.vcds["announcer_response"].handle
	local totTime = TurretVoManager.vcds["announcer_template"].secs+addSecs
	totTime+=0.3
	EntFireByHandle( vcd_handle, "SetTarget1", "npc_scanner_speaker", 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", totTime, null, null )
	
	return totTime+TurretVoManager.vcds["announcer_response"].secs
}


function ProductionSwitch()
{
	TurretVoManager.ProductionSwitchToDefects()
}

TurretVoManager.grabbedDefectTurret <- function(turret)
{

	TurretVoManager.grabbedTurretTalked +=1
	if (TurretVoManager.grabbedTurretTalked >5)
		TurretVoManager.grabbedTurretTalked = 3
	local vcd_handle = null
	switch (TurretVoManager.grabbedTurretTalked)
	{
		case 1:
			vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_chat02"].handle
			break
		case 2:
			vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_chat01"].handle
			break
		case 3:
			vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_chat03"].handle
			break
		case 4:
			vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_chat04"].handle
			break
		case 5:
			vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_chat05"].handle
			break
	}
	local curscene = turret.GetCurrentScene()
	if ( curscene != null )	
			EntFireByHandle( curscene, "Cancel", "", 0, null, null )
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0.0, null, null )

	
	TurretVoManager.grabbedTurretHandle <- turret
}

function productionStall()
{
	TurretVoManager.ProductionStall()
}

TurretVoManager.ProductionStall <- function()
{
	TurretVoManager.productionStalled = true
}

TurretVoManager.ProductionSwitchToDefects <- function()
{
	printl("=================PRODUCTION LINE IS SWITCHED!")
	TurretVoManager.productionSwitched = true
	TurretVoManager.productionStalled = false
}

TurretVoManager.GoodTurretPass <- function(turret)
{
	printl("=================GOOD TURRET PASS: CALLER NAME: " + turret.GetName() )
	
	local initSecs = this.DoTemplate()

	local v = "sp_sabotage_factory_good_pass01"
	local vcd_handle = TurretVoManager.vcds[v].handle
		
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", initSecs, null, null )
	if (TurretVoManager.productionSwitched)
	{
		EntFire( "reject_turret_relay", "trigger", 0, initSecs+0.0 + TurretVoManager.vcds[v].secs )
		EntFire( turret.GetName(), "RunScriptCode", "FunctioningTurretFling()", initSecs+0.0 + TurretVoManager.vcds[v].secs)
	}
	else
	{
		EntFire( "accept_turret_relay", "trigger", 0, initSecs+TurretVoManager.vcds[v].secs)
	}	
}

TurretVoManager.GoodTurretFail <- function(turret)
{
	printl("=================GOOD TURRET FAIL: CALLER NAME: " + turret.GetName() )
	
	local v=TurretVoManager.FetchFromPool("good_fail")
	local vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0, null, null )
	
	if (RandomInt(1,10)>5)
	{
		local vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_defect_laugh0"+RandomInt(1,6)].handle
		//EntFireByHandle( TurretVoManager.vcds["sp_sabotage_factory_defect_laugh0"].handle, "SetTarget1", "replacement_template_turret", 0, null, null )
		EntFireByHandle( vcd_handle, "SetTarget1", "replacement_template_turret", 0, null, null )
		EntFireByHandle( vcd_handle, "start", "", RandomFloat(0.000,0.200) , null, null )
	}	
}

TurretVoManager.GoodTurretTest <- function(turret)
{
	local initSecs = this.DoTemplate()

	local v = "sp_sabotage_factory_good_pass01"
	local vcd_handle = TurretVoManager.vcds[v].handle
		
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", initSecs, null, null )
	EntFire( "reject_turret_relay", "trigger", 0, initSecs+0.0 + TurretVoManager.vcds[v].secs )
	EntFire( turret.GetName(), "RunScriptCode", "FunctioningTurretFling()", initSecs+0.0 + TurretVoManager.vcds[v].secs)
}

TurretVoManager.DefectTurretTest <- function(turret)
{
	printl("=================DEFECT TURRET TEST: CALLER NAME: " + turret.GetName() )
	
	local initSecs = this.DoTemplate()

	local v = TurretVoManager.FetchFromPool("defect_test")
	local vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", initSecs, null, null )
	if (TurretVoManager.productionSwitched)
	{
		EntFire( "accept_turret_relay", "trigger", 0, initSecs+TurretVoManager.vcds[v].secs)
		EntFire( turret.GetName(), "RunScriptCode", "MalfunctioningTurretSneakBy()", initSecs+0.0 + TurretVoManager.vcds[v].secs)
	}
	else
	{
		EntFire( "reject_turret_relay", "trigger", 0, initSecs+0.0 + TurretVoManager.vcds[v].secs )
		EntFire( turret.GetName(), "RunScriptCode", "MalfunctioningTurretFling()", initSecs+0.0 + TurretVoManager.vcds[v].secs)
	}

}


TurretVoManager.DefectTurretFail <- function(turret)
{
	printl("=================DEFECT TURRET FAIL FLING: CALLER NAME: " + turret.GetName() )
	
	local i = RandomInt(1,7)
	local v = TurretVoManager.FetchFromPool("defect_fail")
	local vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0.0, null, null )
}

TurretVoManager.DefectTurretPass <- function(turret)
{
	local initSecs = this.DoTemplate()

	local i = RandomInt(1,13)
	local v = TurretVoManager.FetchFromPool("defect_pass")
	local vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", initSecs, null, null )
	EntFire( "accept_turret_relay", "trigger", 0, initSecs+TurretVoManager.vcds[v].secs)
	EntFire( turret.GetName(), "RunScriptCode", "MalfunctioningTurretSneakBy()", initSecs+0.0 + TurretVoManager.vcds[v].secs)

}

TurretVoManager.DefectTurretSneakBy <- function(turret)
{
	printl("=====SNEAKING BY!!!!" + turret.GetName())
	local vcd_handle = null
	if (RandomInt(1,10) > 5 )
	{
		vcd_handle = TurretVoManager.vcds[TurretVoManager.FetchFromPool("defect_laugh")].handle
	}
	else
	{
		vcd_handle = TurretVoManager.vcds[TurretVoManager.FetchFromPool("defect_pass")].handle
	}	
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0.5, null, null )
}

TurretVoManager.TemplateTurretTest <- function(turret)
{
	printl("=================TEMPLATE TURRET PASS: CALLER NAME: " + turret.GetName() )
	
	local vcd_handle = TurretVoManager.vcds["sp_sabotage_factory_template01"]
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 1, null, null )
}


function testfetchfrompool()
{
	local ret
	ret = TurretVoManager.FetchFromPool("defect_test")
}


function testinitpool_defect_fail()
{
	TurretVoManager.InitVcdPool("defect_fail")
	foreach(idx,val in TurretVoManager.vcdpools["defect_fail"].queue)
	{
		printl("++++++++++++++POOL "+idx+" : "+val)
	}
}


TurretVoManager.FetchFromPool <- function(poolname)
{
	if (!(poolname in TurretVoManager.vcdpools))
	{
		TurretVoManager.InitVcdPool(poolname)
	}

	if (TurretVoManager.vcdpools[poolname].queue.len() == 0)
	{
		TurretVoManager.InitVcdPool(poolname)
	}
	
	local ret= TurretVoManager.vcdpools[poolname].queue[0]
	TurretVoManager.vcdpools[poolname].queue.remove(0)
	return ret
}

TurretVoManager.InitVcdPool <- function(poolname)
{
	if (poolname in TurretVoManager.vcdpools)
	{
		TurretVoManager.vcdpools.rawdelete(poolname)
	}
	local ta = []
	local idx, val
	foreach(idx, val in TurretVoManager.vcds)
	{
		if (val.group == poolname)
		{
			ta.append(idx)
		}
	}
	TurretVoManager.vcdpools[poolname] <- {group = poolname, queue = []}

	local r
	while (ta.len()>0)
	{
		r=RandomInt(0,ta.len()-1)
		TurretVoManager.vcdpools[poolname].queue.append(ta[r])
		ta.remove(r)
	}
}

TurretVoManager.GoodTurretShootPosition <- function(turret)
{
	local v=TurretVoManager.FetchFromPool("good_prerange")
	local vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", 0, null, null )
	
	EntFire("dummyshoot_conveyor_1_spawn_rl", "trigger", 0, TurretVoManager.vcds[v].secs)
	EntFire("dummyshoot_conveyor_1_advance_train_relay", "trigger", 0, TurretVoManager.vcds[v].secs)
}

TurretVoManager.DefectTurretShootPosition <- function(turret)
{
	local totsecs=0
	local v = ""
	local vcd_handle=null
	
	//Play a pre-shooting line (20% chance)
	if (RandomInt(1,10)>8)
	{
		v=TurretVoManager.FetchFromPool("defect_prerange")
		vcd_handle = TurretVoManager.vcds[v].handle
		EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
		EntFireByHandle( vcd_handle, "start", "", 0, null, null )
		totsecs += TurretVoManager.vcds[v].secs
	}
	
	//Play the dryfire sound
	v=TurretVoManager.FetchFromPool("defect_dryfire")
	vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", totsecs, null, null )
	totsecs += TurretVoManager.vcds[v].secs
	
	//Play a post shooting line
	v=TurretVoManager.FetchFromPool("defect_postrange")
	vcd_handle = TurretVoManager.vcds[v].handle
	EntFireByHandle( vcd_handle, "SetTarget1", turret.GetName(), 0, null, null )
	EntFireByHandle( vcd_handle, "start", "", totsecs, null, null )
	//lines > 2.5 seconds get cut off unless we delay the conveyor a little. 
	if (TurretVoManager.vcds[v].secs>2.5)
		totsecs += 0.5*(TurretVoManager.vcds[v].secs)
	else
		totsecs += 0.2*(TurretVoManager.vcds[v].secs)
	
	//Set the conveyor moving, spawn another turret
	EntFire("dummyshoot_conveyor_1_spawn_rl", "trigger", 0, totsecs)
	EntFire("dummyshoot_conveyor_1_advance_train_relay", "trigger", 0, totsecs)
}