"resource/ui/hud_saving.res"
{	
	"HudSaveStatus"
	{
		"ControlName"		"Frame"
		"fieldName"			"HudSaveStatus"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"100"
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"usetitlesafe"		"1"
	}

	"SavingIcon"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"SavingIcon"
		"xpos"					"0"		[$GAMECONSOLE]
		"ypos"					"0"		[$GAMECONSOLE]
		"wide"					"50"	[$GAMECONSOLE]
		"tall"					"50"	[$GAMECONSOLE]
		"xpos"					"10"	[!$GAMECONSOLE]
		"ypos"					"10"	[!$GAMECONSOLE]
		"wide"					"40"	[!$GAMECONSOLE]
		"tall"					"40"	[!$GAMECONSOLE]
		"autoResize"			"0"
		"scaleImage"			"1"
		"visible"				"1"
		"enabled"				"1"
		"image"					"hud/spinner_saving"
	}

	"SavingLabel"
	{
		"ControlName"		"Label"
		"fieldName"			"SavingLabel"
		"font"				"ConfirmationText"
		"xpos"				"55"	
		"ypos"				"4"		[!$GAMECONSOLE]
		"ypos"				"3"		[$X360]
		"ypos"				"-1"	[$PS3]
		"zpos"				"2"
		"wide"				"f0"
		"tall"				"50"
		"autoResize"		"0"
		"visible"			"0"
		"enabled"			"1"
		"textAlignment"		"west"
		"labelText"			""
		"fgcolor_override"	"255 255 255 255"	[$GAMECONSOLE]
		"fgcolor_override"	"220 220 220 255"	[!$GAMECONSOLE]
	}

	"SavedLabel"
	{
		"ControlName"		"Label"
		"fieldName"			"SavedLabel"
		"font"				"ConfirmationText"
		"xpos"				"55"
		"ypos"				"4"		[!$GAMECONSOLE]	
		"ypos"				"3"		[$X360]
		"ypos"				"-1"	[$PS3]
		"zpos"				"2"
		"wide"				"f0"
		"tall"				"50"
		"autoResize"		"0"
		"visible"			"0"
		"enabled"			"1"
		"textAlignment"		"west"
		"labelText"			""
		"fgcolor_override"	"255 255 255 255"	[$GAMECONSOLE]
		"fgcolor_override"	"220 220 220 255"	[!$GAMECONSOLE]
	}
}	
