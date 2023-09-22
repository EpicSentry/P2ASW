BLUE_enabled <- 0
ORANGE_enabled <- 0

function SelectChoicesSpawn()
{
	for( local i = 1; i <= 9; i++ )
	{
		local TriggerNameA = "pingdet_BLUE_" + i;
		local PanelNameA = "panel_select_BLUE_" + i;
		local TriggerNameB = "pingdet_ORANGE_" + i;
		local PanelNameB = "panel_select_ORANGE_" + i;
		
		EntFire( TriggerNameA, "Disable", "", 0 )
		EntFire( TriggerNameB, "Disable", "", 0 )
		EntFire( PanelNameA, "Open", "", 0 )
		EntFire( PanelNameB, "Open", "", 0 )
	}
}

function SelectChoicesBlueStart()
{
	for( local i = 1; i <= 9; i++ )
	{
		local TriggerNameA = "pingdet_BLUE_" + i;
		local PanelNameA = "panel_select_BLUE_" + i;
		
		EntFire( TriggerNameA, "Enable", "", 1 )
		EntFire( PanelNameA, "Close", "", 0 )
	}
	
	BLUE_enabled = 1
}

function SelectChoiceBlue( nChoice )
{
	if ( BLUE_enabled == 0 )
		return
	
	for( local i = 1; i <= 9; i++ )
	{
		local TriggerName = "pingdet_BLUE_" + i;
		local PanelName = "panel_select_BLUE_" + i;
		
		EntFire( TriggerName, "Kill", "", 0 )
		
		if ( i != nChoice )
		{	
			EntFire( PanelName, "Open", "", 0 )
		}
	}
	
	EntFire( "@glados", "RunScriptCode", "GladosPlayVcd(32)", 0 )
	EntFire( "ping_instructor_hint_A_1", "EndHint", "", 0 )
	EntFire( "ping_instructor_hint_B_1", "EndHint", "", 0 )
}

function SelectChoicesOrangeStart()
{
	for( local i = 1; i <= 9; i++ )
	{
		local TriggerNameA = "pingdet_ORANGE_" + i;
		local PanelNameA = "panel_select_ORANGE_" + i;
		
		EntFire( TriggerNameA, "Enable", "", 1 )
		EntFire( PanelNameA, "Close", "", 0 )
	}
	
	ORANGE_enabled = 1
}

function SelectChoiceOrange( nChoice )
{
	if ( ORANGE_enabled == 0 )
		return
		 
	for( local i = 1; i <= 9; i++ )
	{
		local TriggerName = "pingdet_ORANGE_" + i;
		local PanelName = "panel_select_ORANGE_" + i;
		
		EntFire( TriggerName, "Kill", "", 0 )

		if ( i != nChoice )
		{	
			EntFire( PanelName, "Open", "", 0 )
		}
	}
	
	EntFire( "@glados", "RunScriptCode", "GladosPlayVcd(34)", 0 )
	EntFire( "ping_instructor_hint_A_2", "EndHint", "", 0 )
	EntFire( "ping_instructor_hint_B_2", "EndHint", "", 0 )
}
