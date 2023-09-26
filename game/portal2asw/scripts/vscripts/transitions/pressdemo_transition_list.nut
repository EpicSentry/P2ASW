DBG <- 1

FIRST_MAP_WITH_GUN <- "Pressdemo_sphere"
FIRST_MAP_WITH_UPGRADE_GUN <- "Pressdemo_GLaDOS"
LAST_PLAYTEST_MAP <- "Pressdemo_suction"

initialized <- false

// This is the order to play the maps
MapPlayOrder<- [

// ---------------------------------------------------
// 	Intro
// ---------------------------------------------------

"pressdemo_sphere",
"pressdemo_glados",
"pressdemo_reconfigure",
"pressdemo_tbeams",
"pressdemo_laser_stairs",
"pressdemo_paint_speed",
"pressdemo_paint_bounce",
"pressdemo_suction",
]


// --------------------------------------------------------
// Think
// --------------------------------------------------------
function Think()
{	
	if (initialized)
	{
		return
	}
	initialized = true;
	
	local portalGunCommand = ""
	local foundMap = false
	
	foreach (index, map in MapPlayOrder)
	{
		if (MapPlayOrder[index] == FIRST_MAP_WITH_GUN)
		{
			printl( "====Giving regular portal gun" )
			portalGunCommand = "give weapon_portalgun"
			
		}		
		else if (MapPlayOrder[index] == FIRST_MAP_WITH_UPGRADE_GUN)
		{
			//printl( "====Giving UPGRADED portal gun" )
			portalGunCommand = "give weapon_portalgun;upgrade_portalgun"
		}
		
		//printl( "====--------------WEAPON LOOP -----" )
	}
	//printl( "====--------------DONE giving guns!" )
		
	if (foundMap == false )
	{
		printl("================Map not in main track")
		
		printl( "USING ENTRY DEFAULTS" )
		EntFire( "entry_door_1", "SetPartner", "@elevator_entry", 0.0 )
		
		printl( "USING EXIT DEFAULT" )
		EntFire( "exit_door_1", "SetPartner", "@elevator_exit", 0.0 )	
	}

	if (portalGunCommand != "")
	{
		//printl( "=======================Trying to run " + portalGunCommand )
	//	EntFire( "@transition_command", "Command", portalGunCommand, 0.5 )
	}

	if(DBG)
	{
		local mapcount = 0
		
		printl("================DUMPING MAP PLAY ORDER")
		
		foreach( index, map in MapPlayOrder )
		{
			// weed out our transitions
			if( MapPlayOrder[index].find("@") == null )
			{
				if( GetMapName() == MapPlayOrder[index] )
				{
					printl( mapcount + " " + MapPlayOrder[index] + " <--- You Are Here" )
				}
				else
				{
					printl( mapcount + " " + MapPlayOrder[index] )
				}
				mapcount++
			}
			
		}
		printl( mapcount + " maps total." )
		
		printl("================END DUMP")
	}
	
}

// --------------------------------------------------------
// TransitionFromMap
// --------------------------------------------------------
function TransitionFromMap()
{
	local foundMap = false
	
	foreach( index, map in MapPlayOrder )
	{
		if( GetMapName() == MapPlayOrder[index] )
		{
			local foundMap = true
			
			// make good
			local skipIndex = index
			for(local i=0;i<2;i+=1)
			{
				if( skipIndex + 1 < MapPlayOrder.len() )
				{
					if( MapPlayOrder[skipIndex + 1].find("@") != null )
					{
						skipIndex++
					}
					else
					{
						break
					}
				}
			}		
			
			if( skipIndex + 1 >= MapPlayOrder.len() ||
				GetMapName() ==	LAST_PLAYTEST_MAP )
			{
				if(DBG) printl( "Map " + GetMapName() + " is the last map" )
				EntFire( "@end_of_playtest_text", "display", 0 )
			}
			else
			{
				if(DBG) printl( "=============================================================Map " + GetMapName() + " connects to " + MapPlayOrder[ skipIndex + 1 ] )
				EntFire( "@transition_command", "command", "map " + MapPlayOrder[skipIndex + 1], 0.0 )
			}
		}
	}
	
	if( foundMap == false )
	{
		if(DBG) printl( "Map " + GetMapName() + " is the last map" )
		EntFire( "@end_of_pressdemo_text", "display", 0 )
	}
}

