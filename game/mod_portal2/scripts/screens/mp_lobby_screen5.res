"mp_lobby_screen5.res"
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
		
		"material"		"vgui/scoreboard/scoreboard005"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard005"
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

	"DisastersAvertedTextLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"DisastersAvertedTextLabel"
		"xpos"			"10"
		"ypos"			"246"
		"wide"			"260"
		"tall"			"60"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat5_Part1"
		"wrap" 			"1"
		"textAlignment"	"north-west"

		"font"			"CoopHubScreenMediumFont"
	}
	
	
	"DisastersAvertedTextLabel2"
	{
		"ControlName"	"Label"
		"fieldName"		"DisastersAvertedTextLabel2"
		"xpos"			"10"
		"ypos"			"280"
		"wide"			"260"
		"tall"			"60"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat5_Part2"
		"wrap" 			"1"
		"textAlignment"	"north-west"

		"font"			"CoopHubScreenMediumFont"
	}

	"Disasters"
	{
		"ControlName"	"Label"
		"fieldName"		"Disasters"
		"xpos"			"81"
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
