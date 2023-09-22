"level_placard_screen.res"
{	
	"BackgroundImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"BackgroundImage"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"-3"
		"wide"			"256"
		"tall"			"128"
		"autoResize"	"0"
		"visible"		"0"
		"enabled"		"1"
		"scaleImage"	"0"
		"fillcolor"		"255 255 255 255"
	}
	
// TEXT ENTRIES
	
	"NumberDisplay"
	{
		"ControlName"		"Label"
		"fieldName"			"NumberDisplay"
		"xpos"				"0"
		"ypos"				"0"
		"zpos"				"-2"
		"wide"				"256"
		"tall"				"128"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"		
		"textAlignment"		"center"
		"dulltext"			"0"
		"paintBackground" 	"0"
		"labelText"			"*"
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
		"tall"			"128"

		"visible"		"1"
		"material"		"vgui/screens/vgui_overlay"
	}
	
	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"screens/vgui_coop_progress_board"
	}
	"Precache02"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache02"
		"visible"		"0"
		
		"image"		"screens/vgui_coop_progress_board_numbers"
	}
	"Precache03"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache03"
		"visible"		"0"
		
		"image"		"screens/vgui_coop_progress_board_bar"
	}
	"Precache04"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache04"
		"visible"		"0"
		
		"image"		"screens/p2_lightboard_vgui"
	}
	
	"Precache10"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache10"
		"visible"		"0"
		
		"image"		"elevator_video_overlay1"
	}
	"Precache11"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache11"
		"visible"		"0"
		
		"image"		"elevator_video_overlay2"
	}
	"Precache12"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache12"
		"visible"		"0"
		
		"image"		"elevator_video_overlay3"
	}
}
