"Resource/UI/SaveLoadGameDialog.res"
{
	"SaveLoadGameDialog"
	{
		"ControlName"			"Frame"
		"fieldName"				"SaveLoadGameDialog"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"9"
		"tall"					"4"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
		
	"SaveGameList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"SaveGameList"
		"xpos"							"0"
		"ypos"							"25"
		"zpos"							"1"
		"wide"							"200"
		"tall"							"175"	[$GAMECONSOLE]
		"tall"							"150"	[!$GAMECONSOLE]
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}

	"WorkingAnim"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"WorkingAnim"
		"xpos"							"274"
		"ypos"							"35"
		"zpos"							"999"
		"wide"							"100"
		"tall"							"100"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}	

	"SaveGameImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"SaveGameImage"
		"xpos"							"212"
		"ypos"							"15"
		"zpos"							"999"
		"wide"							"225"
		"tall"							"152"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							""
	}
	
	"AutoSaveLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"AutoSaveLabel"
		"font"							"NewGameChapterName"
		"xpos"							"210"	
		"ypos"							"9"
		"zpos"							"2"
		"wide"							"223"
		"tall"							"30"
		"autoResize"					"0"
		"visible"						"0"
		"enabled"						"1"
		"textAlignment"					"east"
		"labelText"						"#PORTAL2_AutoSave"
		"fgcolor_override"				"255 255 255 200"
	}

	"CloudSaveLabel"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"CloudSaveLabel"
		"xpos"							"217"	
		"ypos"							"18"
		"zpos"							"2"
		"wide"							"40"
		"tall"							"40"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"resource/icon_cloud_small"
	}

	"SaveGameInfo"
	{
		"ControlName"					"SaveGameInfoLabel"
		"fieldName"						"SaveGameInfo"
		"xpos"							"212"
		"ypos"							"170"
		"zpos"							"2"
		"wide"							"225"
		"tall"							"20"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
	}			
}