"Resource/UI/FoundGameListItem.res"
{
	"FoundGameListItem"
	{
		"ControlName"					"Frame"
		"fieldName"						"FoundGameListItem"
		"wide"							"f8"
		"tall"							"30" [$GAMECONSOLE]
		"tall"							"25" [$WIN32]
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
	
	"ImgPing" [$WIN32]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgPing"
		"xpos"							"2"
		"ypos"							"4"
		"wide"							"16"
		"tall"							"16"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
	}	
	
	"ImgAvatarBG" [$WIN32]
	{
		"ControlName"			"Panel"
		"fieldName"				"ImgAvatarBG"
		"xpos"					"22"
		"ypos"					"4"
		"wide"					"16"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"bgcolor_override"		"80 80 80 255"
	}
	
	"PnlGamerPic" [$GAMECONSOLE]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PnlGamerPic"
		"xpos"							"10"
		"ypos"							"3"
		"wide"							"24"
		"tall"							"24"
		"visible"						"0"
		"bgcolor_override"				"255 255 255 255"
	}
	
	"PnlGamerPic" [$WIN32]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PnlGamerPic"
		"xpos"							"23"
		"ypos"							"5"
		"wide"							"14"
		"tall"							"14"
		"visible"						"1"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
	}
		
	"PnlModPic"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"PnlModPic"
		"xpos"							"22"
		"ypos"							"4"
		"wide"							"16"
		"tall"							"16"
		"visible"						"0"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
	}
		
		
	"LblGamerTag"
	{
		"ControlName"					"Label"
		"fieldName"						"LblGamerTag"
		"xpos"							"40" [$GAMECONSOLE]
		"ypos"							"0"  [$GAMECONSOLE]
		"wide"							"200" [$GAMECONSOLE]
		"tall"							"30" [$GAMECONSOLE]
		"xpos"							"41" [$WIN32]
		"ypos"							"20" [$WIN32]
		"wide"							"123" [$WIN32]
		"tall"							"20" [$WIN32]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"Font"							"DefaultMedium"
		"textAlignment"					"west" [$WIN32]
		"noshortcutsyntax"		"1"
	}
	
	"LblDifficulty"
	{
		"ControlName"					"Label"
		"fieldName"						"LblDifficulty"
		"xpos"							"252" [$GAMECONSOLE && $GAMECONSOLEWIDE]
		"xpos"							"225" [$GAMECONSOLE && !$GAMECONSOLEWIDE]
		"xpos"							"175" [$WIN32]
		"ypos"							"0" [$GAMECONSOLE]
		"ypos"							"2" [$WIN32]
		"wide"							"200"
		"tall"							"30" [$GAMECONSOLE]
		"tall"							"20" [$WIN32]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"Font"							"DefaultMedium"
		"textAlignment"					"west" [$WIN32]
	}
	
	"LblNotJoinable"
	{
		"ControlName"					"Label"
		"fieldName"						"LblNotJoinable"
		"xpos"							"252" [$GAMECONSOLE && $GAMECONSOLEWIDE]
		"xpos"							"225" [$GAMECONSOLE && !$GAMECONSOLEWIDE]
		"xpos"							"175" [$WIN32]
		"ypos"							"0" [$GAMECONSOLE]
		"ypos"							"2" [$WIN32]
		"wide"							"200"
		"tall"							"30" [$GAMECONSOLE]
		"tall"							"20" [$WIN32]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"Font"							"DefaultMedium"
		"textAlignment"					"west" [$WIN32]
	}
	
	"LblNumPlayers"
	{
		"ControlName"					"Label"
		"fieldName"						"LblNumPlayers"
		"xpos"							"400" [$GAMECONSOLE && $GAMECONSOLEWIDE]
		"xpos"							"365" [$GAMECONSOLE && !$GAMECONSOLEWIDE]
		"xpos"							"300" [$WIN32]
		"ypos"							"0" [$GAMECONSOLE]
		"ypos"							"2" [$WIN32]
		"wide"							"200"
		"tall"							"30" [$GAMECONSOLE]
		"tall"							"20" [$WIN32]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"labelText"						""
		"Font"							"DefaultMedium"
		"textAlignment"					"west" [$WIN32]
	}
		
	"LblPing"
	{
		"ControlName"					"Label"
		"fieldName"						"LblPing"
		"xpos"							"5" [$GAMECONSOLE]
		"ypos"							"3" [$GAMECONSOLE]
		"wide"							"24" [$GAMECONSOLE]
		"tall"							"24" [$GAMECONSOLE]
		"xpos"							"340" [$WIN32]
		"ypos"							"4" [$WIN32]
		"wide"							"32" [$WIN32]
		"tall"							"16" [$WIN32]
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"Font"							"GameUIButtons" [$GAMECONSOLE]
		"labelText"						""
		"textAlignment"					"west" [$WIN32]
	}
	
	"ImgPingSmall" [$WIN32]
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgPingSmall"
		"xpos"							"330"
		"ypos"							"9"
		"wide"							"8"
		"tall"							"8"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"bgcolor_override"				"255 255 255 255"
		"scaleImage"					"1"
		"image"							"icon_con_grey"
	}	
}