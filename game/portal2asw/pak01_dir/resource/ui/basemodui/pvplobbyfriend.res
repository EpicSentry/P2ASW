"Resource/UI/pvplobbyfriend.res"
{
	"pvplobbyfriend"
	{
		"ControlName"					"Frame"
		"fieldName"						"pvplobbyfriend"
		"wide"							"0"
		"tall"							"50" [$GAMECONSOLE]
		"tall"							"35" [!$GAMECONSOLE]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"paintBorder"					"0"
		"leftFadeWidth"					"100"
		"rightFadeWidth"				"50"
		"selected_color"				"168 26 26 255"
	}
	
	"Cmd_INVITE" [!$GAMECONSOLE]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"Cmd_INVITE"
		"xpos"							"15"		
		"ypos"							"2"		
		"wide"							"31"    
		"tall"							"31"    
		"visible"						"1"
		"scaleImage"					"1"
		"image"							"btn_invite"
	}

	"PnlGamerPicBorder" [$PS3]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PnlGamerPicBorder"
		"xpos"							"5"
		"ypos"							"5"
		"wide"							"40"
		"tall"							"40"
		"visible"						"0"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"steam_avatar_border_psn"
	}

	"PnlGamerPic"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PnlGamerPic"
		"xpos"							"55"	[!$GAMECONSOLE]
		"ypos"							"2"		[!$GAMECONSOLE]
		"wide"							"31"    [!$GAMECONSOLE]
		"tall"							"31"    [!$GAMECONSOLE]
		"ypos"							"8"		[$PS3]
		"xpos"							"8"		[$PS3]
		"wide"							"34"    [$PS3]
		"tall"							"34"    [$PS3]
		"xpos"							"5"		[$X360]
		"wide"							"40"    [$X360]
		"tall"							"40"    [$X360]
		"ypos"							"5"		[$X360]
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"?network_lan"
		{
			"image"				"icon_lobby"
		}
	}
		

	"LblGamerTag"
	{
		"ControlName"					"Label"
		"fieldName"						"LblGamerTag"
		"xpos"							"93"	[!$GAMECONSOLE]
		"xpos"							"55"	[$GAMECONSOLE]
		"ypos"							"5"		[!$GAMECONSOLE]
		"ypos"							"7"		[$GAMECONSOLE]
		"wide"							"400"
		"tall"							"15"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"textAlignment"					"west" [$WIN32]
		"noshortcutsyntax"				"1"
	}
	
	"LblGamerTagStatus"
	{
		"ControlName"					"Label"
		"fieldName"						"LblGamerTagStatus"
		"xpos"							"93"	[!$GAMECONSOLE]
		"xpos"							"55"	[$GAMECONSOLE]
		"ypos"							"16"	[!$GAMECONSOLE]
		"ypos"							"25"	[$GAMECONSOLE]
		"wide"							"400"
		"tall"							"20"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"textAlignment"					"west" [$WIN32]
		"noshortcutsyntax"				"1"
	}
}