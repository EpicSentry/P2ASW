"mp_coop_credits_screen.res"
{
	//"Background"
	//{
	//	"ControlName"	"Label"
	//	"fieldName"		"Background"
	//	"xpos"			"0"
	//	"ypos"			"0"
	//	"zpos"			"-2"
	//	"wide"			"854"
	//	"tall"			"534"
	//	"visible"		"1"
	//	"bgcolor_override"				"0 0 0 255"
	//}

	"BG" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"BG"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"1254"
		"tall"			"534"
		"visible"		"1"
		"scaleImage"	"1"
		"zpos"			-10
		"image"		"screens/vgui_coop_credits_screen_bg"
	}
	
	"ScanningText"
	{
		"ControlName"	"Label"
		"fieldName"		"ScanningText"
		"xpos"			"0"
		"ypos"			"20"
		"wide"			"630"//"430"
		"tall"			"120"
		"fgcolor_override"	"255 135 30 255"
		"labelText"		"#P2COOP_Waiting"
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}

	"JobDescription"
	{
		"ControlName"	"Label"
		"fieldName"		"JobDescription"
		"xpos"			"0"
		"ypos"			"498"
		"wide"			"630"
		"tall"			"30"
		"fgcolor_override"	"255 135 30 255"
		"labelText"		"x"
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}

	"ScanningLine" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"ScanningLine"
		"xpos"			"134"
		"ypos"			"-10"
		"wide"			"334"
		"tall"			"4"
		"visible"		"1"
		"scaleImage"	"1"
		"fgcolor_override"	"251 176 59 255"
		"image"		"white"
	}
	
	"ScrollingDigits1" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"ScrollingDigits"
		"xpos"			"536"
		"ypos"			"220"
		"wide"			"100"
		"tall"			"260"
		"visible"		"1"
		"scaleImage"	"1"
		"image"		"screens/vgui_coop_credits_DNAstrip"
	}

	"Scan_Human" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"Scan_Human"
		"xpos"			"280"
		"ypos"			"220"
		"wide"			"165"
		"tall"			"245"
		"visible"		"1"
		"scaleImage"	"1"
		"image"		"screens/vgui_coop_credits_human_icon"
	}

	"Scan_Unknown" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"Scan_Unknown"
		"xpos"			"280"
		"ypos"			"220"
		"wide"			"165"
		"tall"			"245"
		"visible"		"1"
		"scaleImage"	"1"
		"image"		"screens/vgui_coop_credits_unknown_icon"
	}

	//////////////////////////////////////////////////
	"DataDigits0"
	{
		"ControlName"	"Label"
		"fieldName"		"DataDigits0"
		"xpos"			"0"
		"ypos"			"300"
		"wide"			"140"
		"tall"			"60"
		"fgcolor_override"	"255 135 30 128"
		"labelText"		""
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}
		
	"DataDigits1"
	{
		"ControlName"	"Label"
		"fieldName"		"DataDigits1"
		"xpos"			"0"
		"ypos"			"300"
		"wide"			"140"
		"tall"			"60"
		"fgcolor_override"	"255 135 30 128"
		"labelText"		""
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}

	"DataDigits2"
	{
		"ControlName"	"Label"
		"fieldName"		"DataDigits2"
		"xpos"			"0"
		"ypos"			"300"
		"wide"			"140"
		"tall"			"60"
		"fgcolor_override"	"255 135 30 128"
		"labelText"		""
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}

	"DataDigits3"
	{
		"ControlName"	"Label"
		"fieldName"		"DataDigits3"
		"xpos"			"0"
		"ypos"			"300"
		"wide"			"140"
		"tall"			"60"
		"fgcolor_override"	"255 135 30 128"
		"labelText"		""
		"textAlignment"	"east"
		"font"			"CoopCreditsFont_ExtraSmall"
	}

	"CreditNamesLabelSetup"
	{
		"ControlName"	"Label"
		"fieldName"		"CreditNamesLabelSetup"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"10"
		"tall"			"10"
		"textcolor"		"white"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"0"
		"tabPosition"	"0"
		"labelText"		""
		"textAlignment"	"north-west"

		"font"			"CoopCreditsFont"
	}

	"BlackDataBlocker" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"BlackDataBlocker"
		"xpos"			"0"
		"ypos"			"100"
		"wide"			"1100"
		"tall"			"500"
		"visible"		"1"
		"scaleImage"	"1"
		"image"		"black"
	}
}
