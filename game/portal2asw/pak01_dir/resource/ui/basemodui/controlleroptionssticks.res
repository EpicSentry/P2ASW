"Resource/UI/ControllerOptionsSticks.res"
{
	"ControllerOptionsSticks"
	{
		"ControlName"		"Frame"
		"fieldName"			"ControllerOptionsSticks"
		"xpos"				"0" 
		"ypos"				"0"
		"wide"				"8" 
		"tall"				"4"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
		
	"BtnDefault"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnDefault"
		"xpos"						"0"
		"ypos"						"25"
		"zpos"						"3"
		"wide"						"150"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSouthpaw"
		"navDown"					"BtnSouthpaw"
		"labelText"					"#L4D360UI_Controller_Sticks_Default"
		"style"						"LeftDialogButton"
		"command" 					"BtnDefault"
		"navToCommand"				"BtnDefault"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}	
	
	"BtnSouthpaw"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSouthpaw"
		"xpos"						"0"
		"ypos"						"50"
		"zpos"						"3"
		"wide"						"150"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnDefault"
		"navDown"					"BtnDefault"
		"labelText"					"#L4D360UI_Controller_Sticks_Southpaw"
		"style"						"LeftDialogButton"
		"command" 					"BtnSouthpaw"
		"navToCommand"				"BtnSouthpaw"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}	
	
	"ControllerImage"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"ControllerImage"
		"xpos"					"162"
		"ypos"					"40"
		"zpos"					"3"
		"wide"					"320"
		"tall"					"140"
		"autoResize"			"0"
		"visible"				"1"
		"enabled"				"1"		
		"scaleImage"			"1"
		"image"					"controller_layout_sticks" [!$PS3]
		"image"					"controller_layout_sticks_ps3" [$PS3]
	}
	
	"Normal_Move"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Normal_Move"
		"xpos"					"174"	[!$PS3]
		"ypos"					"40"	[!$PS3]
		"xpos"					"204"	[$PS3]
		"ypos"					"62"	[$PS3]
		"zpos"					"4"
		"wide"					"75"
		"tall"					"75"
		"autoResize"			"0"
		"visible"				"0"
		"enabled"				"1"		
		"scaleImage"			"1"
		"image"					"controller_layout_sticks_move"
	}

	"Normal_Look"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Normal_Look"
		"xpos"					"270"	[!$PS3]
		"ypos"					"72"	[!$PS3]
		"xpos"					"271"	[$PS3]
		"ypos"					"60"	[$PS3]
		"zpos"					"4"
		"wide"					"75"
		"tall"					"75"
		"autoResize"			"0"
		"visible"				"0"
		"enabled"				"1"		
		"scaleImage"			"1"
		"image"					"controller_layout_sticks_look"
	}

	"Southpaw_Move"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Southpaw_Move"
		"xpos"					"270"	[!$PS3]
		"ypos"					"75"	[!$PS3]
		"xpos"					"271"	[$PS3]
		"ypos"					"60"	[$PS3]
		"zpos"					"4"
		"wide"					"75"
		"tall"					"75"
		"autoResize"			"0"
		"visible"				"0"
		"enabled"				"1"		
		"scaleImage"			"1"
		"image"					"controller_layout_sticks_move"
	}

	"Southpaw_Look"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Southpaw_Look"
		"xpos"					"174"	[!$PS3]
		"ypos"					"38"	[!$PS3]
		"xpos"					"204"	[$PS3]
		"ypos"					"62"	[$PS3]
		"zpos"					"4"
		"wide"					"75"
		"tall"					"75"
		"autoResize"			"0"
		"visible"				"0"
		"enabled"				"1"		
		"scaleImage"			"1"
		"image"					"controller_layout_sticks_look"
	}	

	"LegacyExplanation" [$PS3]
	{
		"ControlName"				"Label"
		"fieldName"					"LegacyExplanation"
		"xpos"						"177"
		"ypos"						"133"
		"zpos"						"5"
		"wide"						"200"
		"wrap"						"0"
		"autoResize"				"0"
		"visible"					"0"
		"enabled"					"1"
		"tabPosition"				"0"
		"textAlignment"				"center"
		"Font"						"ControllerLayout"
		"LabelText"					"#L4D360UI_Controller_Sticks_Legacy"
	}
}
