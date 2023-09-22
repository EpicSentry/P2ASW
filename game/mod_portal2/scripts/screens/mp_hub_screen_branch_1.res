"mp_select_screen_branch_1.res"
{	
	"BackgroundTop"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"BackgroundTop"
		"xpos"			"96"
		"ypos"			"0"
		"zpos"			"-2"
		"wide"			"384"
		"tall"			"96"
		"visible"		"1"

		"material"		"vgui/white"
	}

	"BackgroundNumber"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Background"
		"xpos"			"-1"
		"ypos"			"-1"
		"zpos"			"-2"
		"wide"			"98"
		"tall"			"98"
		"visible"		"1"

		"material"		"vgui/screens/vgui_signage_blank_bg"
	}
	
	"BackgroundNumber2"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Background"
		"xpos"			"-1"
		"ypos"			"95"
		"zpos"			"-2"
		"wide"			"98"
		"tall"			"98"
		"visible"		"1"

		"material"		"vgui/screens/vgui_signage_blank_bg"
	}
	
	"BackgroundNumber3"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"Background"
		"xpos"			"96"
		"ypos"			"-1"
		"zpos"			"-2"
		"wide"			"392"
		"tall"			"98"
		"visible"		"1"

		"material"		"vgui/screens/vgui_signage_blank_bg_wide"
	}
		
	"LockIcon" 
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"LockIcon"
		"xpos"			"-2"
		"ypos"			"-2"
		"wide"			"98"
		"tall"			"98"
		"visible"		"1"
		"scaleImage"	"1"
		"image"		"screens/vgui_levelselect_icon_lock"
	}
		
	"SplitBarVertical"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"SplitBar"
		"xpos"			"94"
		"ypos"			"0"
		"zpos"			"0"
		"wide"			"2"
		"tall"			"96"
		"visible"		"1"

		"material"		"vgui/black"
	}
	
	"SplitBarHorizontal"
	{
		"ControlName"	"MaterialImage"
		"fieldName"		"SplitBar"
		"xpos"			"0"
		"ypos"			"94"
		"zpos"			"0"
		"wide"			"480"
		"tall"			"2"
		"visible"		"1"

		"material"		"vgui/black"
	}
		
	"BranchTitle"
	{
		"ControlName"	"Label"
		"fieldName"		"BranchTitle"
		"xpos"			"108"
		"ypos"			"109"
		"wide"			"480"
		"tall"			"64"
		"textcolor"		"White"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		"TEMP TITLE"
		"textAlignment"	"north-west"

		"font"			"CoopHubTrackScreenFont"
	}

	"CourseNumberTitle"
	{
		"ControlName"	"Label"
		"fieldName"		"CourseNumberTitle"
		"xpos"			"10"
		"ypos"			"104"
		"wide"			"88"
		"tall"			"88" [!$OSX]
		"tall"			"110" [$OSX]
		"textcolor"		"Black"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"None"
		"textAlignment"	"south-west"

		"font"			"CoopHubTrackScreenFont_Bold"
	}	

	"Precache01"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache01"
		"visible"		"0"
		
		"image"		"screens/vgui_levelselect_icon_both"
	}
	"Precache02"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache02"
		"visible"		"0"
		
		"image"		"screens/vgui_levelselect_icon_blue_only"
	}
	"Precache03"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache03"
		"visible"		"0"
		
		"image"		"screens/vgui_levelselect_icon_orange_only"
	}
	"Precache04"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache04"
		"visible"		"0"
		
		"image"		"screens/vgui_levelselect_icon_unlock"
	}

	"Precache05"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache05"
		"visible"		"0"
		
		"image"		"screens/vgui_signage_blank_bg"
	}	
	"Precache06"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Precache06"
		"visible"		"0"
		
		"image"		"screens/vgui_signage_blank_bg_wide"
	}
}
