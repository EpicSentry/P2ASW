"Resource/UI/pvplobby.res"
{
	"pvplobby"
	{
		"ControlName"					"Frame"
		"fieldName"						"pvplobby"
		"xpos"							"0"
		"ypos"							"0"
		"wide"							"7" [!$PS3]
		"wide"							"9" [$PS3]
		"tall"							"5"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"dialogstyle"					"4"
	}
	
	"ListFriends"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"ListFriends"
		"xpos"							"0"
		"ypos"							"0"
		"zpos"							"1"
		"wide"							"0" [!$PS3]
		"wide"							"300" [$PS3]
		"tall"							"290" [$GAMECONSOLE]
		"tall"							"250" [!$GAMECONSOLE]
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}

	"ListFriendsSteam" [$PS3]
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"ListFriendsSteam"
		"xpos"							"300"
		"ypos"							"0"
		"zpos"							"1"
		"wide"							"150"
		"tall"							"290"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}

	"ListFriendsStatus"
	{
		"ControlName"					"Label"
		"fieldName"						"ListFriendsStatus"
		"xpos"							"12"
		"ypos"							"100"
		"zpos"							"2"
		"wide"							"325"
		"tall"							"150"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"wrap"							"1"
		"textAlignment"					"left"
		"Font"							"ConfirmationText"
		"fgcolor_override"				"0 0 0 255"
	}

	"ListSearchStatus"
	{
		"ControlName"					"Label"
		"fieldName"						"ListSearchStatus"
		"xpos"							"120"
		"ypos"							"0"
		"zpos"							"2"
		"wide"							"205"
		"tall"							"100"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"wrap"							"1"
		"textAlignment"					"left"
		"Font"							"FriendsList"
		"fgcolor_override"				"0 0 0 255"
	}

	"WorkingAnim"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"WorkingAnim"
		"xpos"							"10"
		"ypos"							"0"
		"zpos"							"999"
		"wide"							"100"
		"tall"							"100"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}	
	
	"BtnQuickMatch" [$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnQuickMatch"
		"command"				"BtnQuickMatch"
		"xpos"					"c-240"
		"wide"					"400"
		"ypos"					"400"
		"tall"					"15"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navDown"				"BtnCancel"
		"labelText"				"#Portal2UI_pvp_QuickMatch"
		"allcaps"				"1"
		"style"					"MainMenuSmallButton"
	}
	
	"BtnCancel" [$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnCancel"
		"command"				"Back"
		"xpos"					"c-240"
		"ypos"					"420"
		"zpos"					"1"
		"wide"					"180"
		"tall"					"15"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"labelText"				"#L4D360UI_Back_Caps"
		"tooltiptext"			"#L4D360UI_Tooltip_Back"
		"style"					"MainMenuSmallButton"
		EnabledTextInsetX		"2"
		DisabledTextInsetX		"2"
		FocusTextInsetX			"2"
		OpenTextInsetX			"2"
		"navUp"					"BtnQuickMatch"
	}	



	"ListsSeparator" [$PS3]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ListsSeparator"
		"xpos"							"250"
		"ypos"							"-55"
		"zpos"							"3"
		"wide"							"50"
		"tall"							"400"
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"menu_tiles/UI_tile_128_right_mask"
	}

	"ListsSeparator2" [$PS3]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ListsSeparator2"
		"xpos"							"400"
		"wide"							"50"
		"ypos"							"-55"
		"zpos"							"3"
		"wide"							"50"
		"tall"							"400"
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"menu_tiles/UI_tile_128_right_mask"
	}

	"SteamTitleLogo" [$PS3]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"SteamTitleLogo"
		"xpos"							"300"
		"ypos"							"-50"
		"zpos"							"4"
		"wide"							"150"
		"tall"							"75"
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"steam_logo_wm"
	}

	"PsnTitleLogo" [$PS3]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PsnTitleLogo"
		"xpos"							"-2"
		"ypos"							"-53"
		"zpos"							"4"
		"wide"							"150"
		"tall"							"75"
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"psn_logo"
	}
}