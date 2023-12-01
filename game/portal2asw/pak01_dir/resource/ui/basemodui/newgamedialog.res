"Resource/UI/NewGameDialog.res"
{
	"NewGameDialog"
	{
		"ControlName"			"Frame"
		"fieldName"				"NewGameDialog"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"9"
		"tall"					"4"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
		
	"ChapterList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"ChapterList"
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

	"ChapterImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ChapterImage"
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
	
	"ChapterText"
	{
		"ControlName"					"ChapterLabel"
		"fieldName"						"ChapterText"
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