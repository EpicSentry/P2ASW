"mp_lobby_screen6.res"
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
		
		"material"		"vgui/scoreboard/scoreboard006"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard006"
	}

		
	"GladosHeaderLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"GladosHeaderLabel"
		"xpos"			"120"
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
		"xpos"			"120"
		"ypos"			"26"
		"wide"			"320"
		"tall"			"40"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Disclaimer2"
		"textAlignment"	"north-east"
		"font"			"CoopHubScreenVerySmallFont"
	}

	
		"SCPsTextLabel1"
	{
		"ControlName"	"Label"
		"fieldName"		"DisastersAvertedTextLabel1"
		"xpos"			"8"
		"ypos"			"170"
		"wide"			"400"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat6_Part1"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenMediumFont"
	}
	
		"SCPsTextLabel2"
	{
		"ControlName"	"Label"
		"fieldName"		"DisastersAvertedTextLabel2"
		"xpos"			"8"
		"ypos"			"200"
		"wide"			"400"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat6_Part2"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenMediumFont"
	}
	
	"SCPsTextLabel3"
	{
		"ControlName"	"Label"
		"fieldName"		"DisastersAvertedTextLabel3"
		"xpos"			"8`"
		"ypos"			"230"
		"wide"			"400"
		"tall"			"200"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat6_Part3"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenMediumFont"
	}
	
	
	"SCPs"
	{
		"ControlName"	"Label"
		"fieldName"		"SCPs"
		"xpos"			"6"
		"ypos"			"63"
		"wide"			"250"
		"tall"			"160"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-west"

		"font"			"CoopHubScreenNumberFont"
	}
}
