"Resource/UI/Quickplay.res"
{
	"QuickPlay"
	{
		"ControlName"			"Frame"
		"fieldName"				"QuickPlay"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"10"
		"tall"					"5"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
	
	"LblQuickPlayDescription"
	{
		"ControlName"					"Label"
		"fieldName"						"LblQuickPlayDescription"
		"font"							"ConfirmationText"
		"xpos"							"258"	
		"ypos"							"158"
		"zpos"							"4"
		"wide"							"234"
		"tall"							"40"
		"autoResize"					"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"					"north-west"
		"wrap"							"1"
		"fgcolor_override"				"201 211 207 255"
		"labelText"					""
	}

	"EnumerationTypeList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"EnumerationTypeList"
		"xpos"							"0"
		"ypos"							"25"
		"zpos"							"1"
		"wide"							"250"
		"tall"							"200"	[$GAMECONSOLE]
		"tall"							"200"	[!$GAMECONSOLE]
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}
	
	"FilterIcon"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"FilterIcon"
		"xpos"							"300"
		"ypos"							"8"
		"wide"							"150"
		"tall"							"150"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"quick_recent"
	}	
}
