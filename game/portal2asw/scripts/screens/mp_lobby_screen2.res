"mp_lobby_screen2.res"
{
	"Background"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Background1"
		"xpos"			"1"
		"ypos"			"2"
		"zpos"			"-2"
		"wide"			"800"
		"tall"			"320"
		"visible"		"1"
		
		"material"		"vgui/scoreboard/scoreboard002"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard002"
	}

	"GladosHeaderLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"GladosHeaderLabel"
		"xpos"			"570"
		"ypos"			"18"
		"wide"			"320"
		"tall"			"100"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Disclaimer3"
		"textAlignment"	"north-west"
		"wrap"			"1"

		"font"			"CoopHubScreenVerySmallFont"
	}

	"PortalsPlacedLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"PortalsPlacedLabel"
		"xpos"			"246"
		"ypos"			"170"
		"wide"			"160"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat2"
		"textAlignment"	"north-west"
		"font"			"CoopHubScreenMediumFont"
		"wrap"			"1"
	}

	"NumPortals"
	{
		"ControlName"	"Label"
		"fieldName"		"NumPortals"
		"xpos"			"247"
		"ypos"			"63"
		"wide"			"180"
		"tall"			"160"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-west"

		"font"			"CoopHubScreenNumberFont"
	}
}
