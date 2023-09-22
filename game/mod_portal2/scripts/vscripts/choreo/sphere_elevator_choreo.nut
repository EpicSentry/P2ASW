DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )

//Jailbreak
Dialog[100] <- { speaker = WHEATLEY, one = "There you are!", two = "I was worried you had gotten yourself murdered" nextLine = 101, nextLineDelay = 3 }
Dialog[101] <- { speaker = WHEATLEY, one = "I needed to get myself on the maintainance crew to see you" nextLine = 102, nextLineDelay = 2 }
Dialog[102] <- { speaker = WHEATLEY, one = "These nanobots workers are pushovers" two = "I'm already the only one authorized to weld girders?" }

Dialog[103] <- { speaker = WHEATLEY, one = "OUCH!" nextLine = 104, nextLineDelay = 2 }
Dialog[104] <- { speaker = WHEATLEY, one = "What moron put that girder there?!?" }

Dialog[105] <- { speaker = WHEATLEY, one = "I think I've figured out how to bust you out" nextLine = 106, nextLineDelay = 3 }
Dialog[106] <- { speaker = WHEATLEY, one = "see you in a bit" }

function ElevatorThereYouAre()
{
//	SpeakLine( 100 )
	//EntFire("@glados","RunScriptCode","GladosPlayVcd(-2)", 0)
	EntFire("@glados","RunScriptCode","Wheatley_Elevator_Scene_Start()", 0)
	//EntFire("@sphere","EnableFlashlight","",0)
	
	EntFire("departure_elevator-spherebot_train_1_chassis_1","movetopathnode","departure_elevator-spherebot_train_1_path_2",1.0)
	EntFire("departure_elevator-spherebot_train_1_chassis_1","setspeedreal",50,1.01)
	EntFire("departure_elevator-elevator_1","setspeedreal","50",1.01)
}

function ElevatorOuch()
{
//	SpeakLine( 103 )
	EntFire("@glados","RunScriptCode","Wheatley_Elevator_Scene_Ow()", 0)
	EntFire("departure_elevator-spherebot_train_1_chassis_1","movetopathnode","spherebot_train_1_path_4",1.0)
	EntFire("departure_elevator-spherebot_train_1_chassis_1","setspeedreal",40,1.01)
	EntFire("departure_elevator-elevator_1","setspeedreal","40",1.01)
}
