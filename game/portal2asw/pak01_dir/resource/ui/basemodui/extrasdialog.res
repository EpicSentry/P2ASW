"Resource/UI/ExtrasDialog.res"
{
	"ExtrasDialog"
	{
		"ControlName"			"Frame"
		"fieldName"				"ExtrasDialog"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"9"
		"tall"					"4"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
		
	"InfoList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"InfoList"
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

	"InfoImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"InfoImage"
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
	
	"InfoText"
	{
		"ControlName"					"ChapterLabel"
		"fieldName"						"InfoText"
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