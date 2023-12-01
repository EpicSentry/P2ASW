"Resource/UI/AttractScreen.res"
{
	"AttractScreen"
	{
		"ControlName"			"Frame"
		"fieldName"				"AttractScreen"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"f0"
		"tall"					"f0"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"bgcolor_override"		"0 0 0 0"
	}

	"LblPressStart"
	{
		"ControlName"			"Label"
		"fieldName"				"LblPressStart"
		"ypos"					"c160"	[!($PS3 && ($JAPANESE || $SCHINESE || $TCHINESE || $KOREAN))]
		"ypos"					"c155"	[$PS3 && ($JAPANESE || $SCHINESE || $TCHINESE || $KOREAN)]
		"zpos"					"3"
		"wide"					"f0"
		"tall"					"64"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"labelText"				"#PORTAL2_Attract_PressStart"		
		"textAlignment"			"center"	
		"fgcolor_override"		"255 255 255 255"	
		"Font"					"AttractTitle"
	}	
}