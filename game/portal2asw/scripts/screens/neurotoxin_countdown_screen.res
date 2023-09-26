"neurotoxin_countdown_screen.res"
{	

// SCREENANIM

// BACKGROUND
	
	"BackgroundImage"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"BackgroundImage"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"-3"
		"wide"			"256"
		"tall"			"128"

		"visible"		"1"
		"material"		"vgui/screens/screen"
	}
	
// TEXT ENTRIES
	
	"NumberDisplay"
	{
		"ControlName"		"Label"
		"fieldName"			"NumberDisplay"
		"xpos"				"22" [!$X360]
		"xpos"				"24" [$X360]
		"ypos"				"0"
		"zpos"				"-2"
		"wide"				"256"
		"tall"				"128"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"		
		"textAlignment"		"left"
		"dulltext"			"0"
		"paintBackground" 	"0"
		"labelText"			"*"
		"font"				"NeurotoxinCountdownFont"
	}
	
// MONITOR OVERLAY

	"Overlay"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Overlay"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"-1"
		"wide"			"256"
		"tall"			"300"

		"visible"		"1"
		"material"		"vgui/screens/screenanim"
	}

	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"screens/screen"
	}

	"Precache02"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache02"
		"visible"		"0"
		
		"image"		"screens/screenanim"
	}
}
