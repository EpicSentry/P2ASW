"mp_lobby_screen1.res"
{
	"Background1"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Background1"
		"xpos"			"1"
		"ypos"			"2"
		"zpos"			"-2"
		"wide"			"800"
		"tall"			"320"
		"visible"		"1"
		
		"material"		"vgui/scoreboard/scoreboard001"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard001"
	}
	
	"GladosHeaderLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"GladosHeaderLabel"
		"xpos"			"440"
		"ypos"			"12"
		"wide"			"320"
		"tall"			"40"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Disclaimer1"
		"textAlignment"	"north-east"
		"font"			"CoopHubScreenVerySmallFont"
	}
	
	"GladosHeaderLabel2"
	{
		"ControlName"	"Label"
		"fieldName"		"GladosHeaderLabel2"
		"xpos"			"440"
		"ypos"			"26"
		"wide"			"320"
		"tall"			"40"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Disclaimer2"
		"textAlignment"	"north-east"
		"font"			"CoopHubScreenVerySmallFont"
	}

	"StepsTakenLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"StepsTakenLabel"
		"xpos"			"246"
		"ypos"			"170"
		"wide"			"400"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat1"
		"textAlignment"	"north-west"

		"font"			"CoopHubScreenMediumFont"
	}

	"StepsB"
	{
		"ControlName"	"Label"
		"fieldName"		"StepsB"
		"xpos"			"245"
		"ypos"			"63"
		"wide"			"400"
		"tall"			"160"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-west"
		"font"			"CoopHubScreenNumberFont"
	}

	"StepsO"
	{
		"ControlName"		"Label"
		"fieldName"		"StepsO"
		"xpos"			"242"
		"ypos"			"222"
		"wide"			"400"
		"tall"			"100"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-east"
		"font"			"CoopHubScreenNumberFont"
	}
}
