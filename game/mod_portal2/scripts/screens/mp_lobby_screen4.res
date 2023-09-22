"mp_lobby_screen4.res"
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
		
		"material"		"vgui/scoreboard/scoreboard004"
	}
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"scoreboard/scoreboard004"
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

	"HugsLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"HugsLabel"
		"xpos"			"6"
		"ypos"			"63"
		"wide"			"480"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat4"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenNumberFont"
	}
	
	"HugsTextLabelLabel1"
	{
		"ControlName"	"Label"
		"fieldName"		"HugsTextLabelLabel1"
		"xpos"			"170"
		"ypos"			"168"
		"wide"			"160"
		"tall"			"200"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat4_SubLabel_Part1"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenMediumFont"
	}
	
		"HugsTextLabelLabel2"
	{
		"ControlName"	"Label"
		"fieldName"		"HugsTextLabelLabel2"
		"xpos"			"170"
		"ypos"			"304"
		"wide"			"240"
		"tall"			"120"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat4_SubLabel_Part2"
		"textAlignment"	"north-west"
		"wrap"			"1"
		"font"			"CoopHubScreenVerySmallFont"
	}
	
	"NumHugsLabel"
	{
		"ControlName"	"Label"
		"fieldName"		"NumHugsLabel"
		"xpos"			"590"
		"ypos"			"228"
		"wide"			"200"
		"tall"			"40"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"#P2COOP_Scoreboard_Stat4_SubLabel_Part3"
		"textAlignment"	"north-east"

		"font"			"CoopHubScreenVerySmallFont"
	}

	"NumHugs"
	{
		"ControlName"	"Label"
		"fieldName"		"NumHugs"
		"xpos"			"614"
		"ypos"			"223"
		"wide"			"180"
		"tall"			"200"
//		"textcolor"		"White"
		"fgcolor_override"	"242 242 242 255"
		"labelText"		"0"
		"textAlignment"	"north-east"
		"font"			"CoopHubScreenNumberFont"
	}
}
