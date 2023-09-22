"mp_lobby_screen3.res"
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
		
		"material"		"vgui/scoreboard/scoreboard003"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard003"
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


	"PortalsTravelledLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"PortalsTravelledLabel"
		"xpos"			"330"
		"ypos"			"85"
		"wide"			"200"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat3"
		"textAlignment"	"south-west"
		"wrap"			"1"
		"font"			"CoopHubScreenMediumFont"
	}

	"NumPortals"
	{
		"ControlName"	"Label"
		"fieldName"		"NumPortals"
		"xpos"			"328"
		"ypos"			"221"
		"wide"			"180"
		"tall"			"100"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-west"
		"font"			"CoopHubScreenNumberFont"
	}
}
