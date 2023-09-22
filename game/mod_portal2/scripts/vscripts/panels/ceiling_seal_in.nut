
// --------------------------------------------------------
function CloseCeilingOld()
{	
	local panelDelay = 0.35
	local panelCount = 57
	local exitPanelCount = 3
	local panelOrder = {}

	// seed
	for(local i=0;i<panelCount;i+=1)
	{
		panelOrder[i] <- i
	}

	// shuffle
	for(local i=0;i<panelCount-exitPanelCount;i+=1)
	{
		local temp = panelOrder[i]
		local index = RandomInt(0,panelCount-(exitPanelCount+1))
		panelOrder[i] = panelOrder[index]
		panelOrder[index] = temp		
	}

	for(local i=0;i<panelCount;i+=1)
	{
		local doorNum = 1 + panelOrder[i]
		
//		printl("closing door " + doorNum)
		
		EntFire("door_" + doorNum + "-shutter_door","Open", "", panelDelay*i )
	}
}

// --------------------------------------------------------
//function SealChambers()
//{
//	local panelCount = 56 
//
//	for(local i=0;i<panelCount;i+=1)
//	{
//		local doorNum = 1 + i	
//		EntFire("door_" + doorNum + "-shutter_door","Close", "", 0 )
//	}
//}

armCount <- 61

closeDoor1 <- 28
closeDoor2 <- 29
farDoor1 <- 25
farDoor2 <- 26
finalDoor1 <- 33
finalDoor2 <- 61

function Seal()
{
//	local armOrder = {}
//
//	// seed
//	for(local i=0;i<armCount;i+=1)
//	{
//		local checkDoor = i+1
//		if( checkDoor != closeDoor1 &&
//			checkDoor != closeDoor2 &&
//			checkDoor != farDoor1 &&
//			checkDoor != farDoor2 &&
//			checkDoor != finalDoor1 &&
//			checkDoor != finalDoor2 )
//		{
//			armOrder[armOrder.len()] <- i
//		}
//	}
//	
//	// shuffle
//	for(local i=0;i<armOrder.len();i+=1)
//	{
//		local temp = armOrder[i]
//		local index = RandomInt(0,armOrder.len()-1)
//		armOrder[i] = armOrder[index]
//		armOrder[index] = temp		
//	}
//	
//	armOrder[armOrder.len()] <- closeDoor1
//	armOrder[armOrder.len()] <- closeDoor2
//	armOrder[armOrder.len()] <- farDoor1
//	armOrder[armOrder.len()] <- farDoor2
//	armOrder[armOrder.len()] <- finalDoor1
//	armOrder[armOrder.len()] <- finalDoor2
//	
//	for(local i=0;i<armOrder.len();i+=1)
//	{
//		printl( "Arm " + (armOrder[i] + 1) + " has timer " + timer * 0.35 )
//		
//		SealOneArm( armOrder[i] + 1, i * 0.35 ) // the last arms in the list go last
//	}

	local timer = 0
	for(local i=0;i<armCount;i+=1)
	{	
		if( i == 33 )
		{
			timer = 0
		}
		
		local count = i
		if( count > 32 )
		{ 
			count -= 34
			timer += 0.02*count
		}
		else
			timer += 0.014*count
		
//		printl( "" + (i+1) + " at time " + timer )
			
		SealOneArm( i + 1, timer ) // the last arms in the list go last
	}

//	EntFire( "door_script", "RunScriptCode", "TryCloseFinalDoors()", 0.35*( armOrder.len()+2) )
}

function Setup()
{
	for(local i=0;i<armCount;i+=1)
	{
		local armNum = 1 + i	
		SetArmIdle( armNum )
	}
}

function Cleanup()
{
	for(local i=0;i<armCount;i+=1)
	{
		local armNum = 1 + i	
		CleanUpArm( armNum )
	}
}

function ShowDoors()
{
	for(local i=0;i<armCount;i+=1)
	{
		local armNum = 1 + i	
		EnableArm( armNum, true )
	}
}

function HideDoors()
{
	for(local i=0;i<armCount;i+=1)
	{
		local armNum = 1 + i	
		EnableArm( armNum, false )
	}
}

function CloseFirstDoor()
{
//	printl("Closing first door!")
//	SealOneArm( closeDoor1, 0 )
//	SealOneArm( closeDoor2, 0 )
//	
//	closeDoor1 = -1
//	closeDoor2 = -1
}

function CloseSecondDoor()
{
//	printl("Closing second door!")
//	SealOneArm( farDoor1, 0 )
//	SealOneArm( farDoor2, 0 )	
//
//	farDoor1 = -1
//	farDoor2 = -1
}

function TryCloseFinalDoors()
{
	if( closeDoor1 != -1 )
		SealOneArm( closeDoor1, 0 )

	if( closeDoor2 != -1 )
		SealOneArm( closeDoor2, 0 )

	if( farDoor1 != -1 )
		SealOneArm( farDoor1, 0 )

	if( farDoor2 != -1 )
		SealOneArm( farDoor2, 0 )

	SealOneArm( finalDoor1, 0 )
	SealOneArm( finalDoor2, 0 )
}

function SetArmIdle( arm_number )
{
	local prefabName = "sealin_" + arm_number
	EntFire(prefabName + "-arm_1", "setanimationnoreset", "block_lower01_drop_idle" );
	EntFire(prefabName + "-arm_2", "setanimationnoreset", "block_upper01_drop_idle" );
}

function SealOneArm( arm_number, delay )
{	
	local prefabName = "sealin_" + arm_number
	local bottomArmName = prefabName + "-arm_1"
	local topArmName = prefabName + "-arm_2"
	local bottomPanelName = prefabName + "-panel_1"
	local topPanelName = prefabName + "-panel_2"	
		
	if( arm_number != closeDoor1 &&
		arm_number != closeDoor2 &&
		arm_number != farDoor1 &&
		arm_number != finalDoor1 &&
		(arm_number%3) != 0 )
	{
		EntFire(bottomArmName, "setanimationnoreset", "block_lower01_drop", 0.0 + delay );
		EntFire(bottomArmName, "setanimationnoreset", "block_lower01_grabpanel", 1.0 + delay );

		EntFire(bottomPanelName, "setparent", bottomArmName, 1.5 + delay );
		EntFire(bottomPanelName, "setparentattachment", "panel_attach", 1.5 + delay );
		
		EntFire(bottomArmName, "setanimationnoreset", "block_lower01_pushforward", 2.0 + delay );
		
		delay += RandomFloat( -0.2, 0.2 )

		EntFire(topArmName, "setanimationnoreset", "block_upper01_pushforward", 2.0 + delay );
		
		EntFire(bottomPanelName, "clearparent", "", 3.5 + delay );
	}
	else
	{
		EntFire(topArmName, "setanimationnoreset", "block_upper02_abovestairs", 2.0 + delay );
		EntFire(bottomArmName, "kill", "", 0.0 );
	}
	
	EntFire(topArmName, "setanimationnoreset", "block_upper01_drop", 0.0 + delay );
	EntFire(topArmName, "setanimationnoreset", "block_upper01_grabpanel", 1.0 + delay );

	EntFire(topPanelName, "setparent", topArmName, 1.5 + delay );
	EntFire(topPanelName, "setparentattachment", "panel_attach", 1.5 + delay );
	
	EntFire(topPanelName, "clearparent", "", 3.5 + delay );
}

function CleanUpArm( arm_number, delay )
{	
	local prefabName = "sealin_" + arm_number
	local bottomArmName = prefabName + "-arm_1"
	local topArmName = prefabName + "-arm_2"
	local bottomPanelName = prefabName + "-panel_1"
	local topPanelName = prefabName + "-panel_2"
	
	EntFire(topArmName, "kill", "", 0.0 );
	EntFire(topPanelName, "kill", "", 0.0 );
	EntFire(bottomArmName, "kill", "", 0.0 );
	EntFire(bottomPanelName, "kill", "", 0.0 );
}

function EnableArm( arm_number, enabled )
{	
	local prefabName = "sealin_" + arm_number
	local bottomArmName = prefabName + "-arm_1"
	local topArmName = prefabName + "-arm_2"
	local bottomPanelName = prefabName + "-panel_1"
	local topPanelName = prefabName + "-panel_2"
	
	if( enabled )
	{
		EntFire(topArmName, "Enable", "", 0.0 );
		EntFire(topPanelName, "Enable", "", 0.0 );
		EntFire(bottomArmName, "Enable", "", 0.0 );
		EntFire(bottomPanelName, "Enable", "", 0.0 );
	}
	else
	{
		EntFire(topArmName, "Disable", "", 0.0 );
		EntFire(topPanelName, "Disable", "", 0.0 );
		EntFire(bottomArmName, "Disable", "", 0.0 );
		EntFire(bottomPanelName, "Disable", "", 0.0 );
	}	
}