"Resource/UI/DropDownGameAccess.res"
{
	"PnlBackground"
	{
		"ControlName"		"Panel"
		"fieldName"			"PnlBackground"
		"xpos"				"0"
		"ypos"				"0"
		"zpos"				"1"
		"wide"				"146"
		"tall"				"85"	[$GAMECONSOLE]
		"tall"				"45"	[$WIN32]
		"visible"			"1"
		"enabled"			"1"
		"paintbackground"	"1"
		"paintborder"		"1"
	}

	"BtnPrivate"	[$GAMECONSOLE]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnPrivate"
		"xpos"					"0"
		"ypos"					"0"
		"zpos"					"2"
		"wide"					"140"
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"navUp"					"BtnSyslink"
		"navDown"				"BtnFriends"
		"labelText"				"#L4D360UI_Access_Invite"
		"tooltiptext"			"#L4D360UI_Lobby_MakePrivate_Tip"
		"disabled_tooltiptext"	"#L4D360UI_GameSettings_Tooltip_Access_Disabled"
		"style"					"FlyoutMenuButton"
		"command"				"GameAccess_private"
		"EnableCondition" 		"LiveRequired"		
	}	
	
	"BtnFriends"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnFriends"
		"xpos"					"0"
		"ypos"					"20"	[$GAMECONSOLE]
		"ypos"					"0"		[$WIN32]
		"zpos"					"2"
		"wide"					"140"
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"navUp"					"BtnPrivate"	[$GAMECONSOLE]
		"navUp"					"BtnPublic"		[$WIN32]
		"navDown"				"BtnPublic"
		"labelText"				"#L4D360UI_Access_Friends"
		"tooltiptext"			"#L4D360UI_Lobby_MakeFriendOnly_Tip"
		"disabled_tooltiptext"	"#L4D360UI_GameSettings_Tooltip_Access_Disabled"
		"style"					"FlyoutMenuButton"
		"command"				"GameAccess_friends"
		"EnableCondition" 		"LiveRequired"		
	}
	
	"BtnPublic"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnPublic"
		"xpos"					"0"
		"ypos"					"40"	[$GAMECONSOLE]
		"ypos"					"20"	[$WIN32]
		"zpos"					"2"
		"wide"					"140"
		"tall"					"20"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"navUp"					"BtnFriends"
		"navDown"				"BtnSyslink"	[$GAMECONSOLE]
		"navDown"				"BtnFriends"	[$WIN32]
		"labelText"				"#L4D360UI_Access_Public"
		"tooltiptext"			"#L4D360UI_Lobby_OpenToPublic_Tip"
		"disabled_tooltiptext"	"#L4D360UI_GameSettings_Tooltip_Access_Disabled"
		"style"					"FlyoutMenuButton"
		"command"				"GameAccess_public"
		"EnableCondition" 		"LiveRequired"		
	}
	
 	"BtnSyslink"	[$GAMECONSOLE]
 	{
 		"ControlName"			"BaseModHybridButton"
 		"fieldName"				"BtnSyslink"
		"xpos"					"0"
 		"ypos"					"60"
 		"zpos"					"2"
 		"wide"					"140"
 		"tall"					"20"
 		"autoResize"			"1"
 		"pinCorner"				"0"
 		"visible"				"1"
 		"enabled"				"1"
 		"tabPosition"			"0"
 		"wrap"					"1"
 		"navUp"					"BtnPublic"
 		"navDown"				"BtnPrivate"
 		"labelText"				"#L4D360UI_Access_LAN"
 		"tooltiptext"			"#L4D360UI_Access_Tooltip_LAN"
 		"style"					"FlyoutMenuButton"
 		"command"				"GameNetwork_lan"
 	}	
}