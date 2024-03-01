"Resource/UI/ControllerOptionsButtons.res"
{
	"ControllerOptionsButtons"
	{
		"ControlName"		"Frame"
		"fieldName"			"ControllerOptionsButtons"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"10"
		"tall"				"5"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
		
	"BtnSpec1"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSpec1"
		"xpos"						"0"
		"ypos"						"25"
		"wide"						"100"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSpecCustom"
		"navDown"					"BtnSpec2"
		"labelText"					"#L4D360UI_Controller_Buttons_Config1"
		"style"						"LeftDialogButton"
		"command" 					"BtnSpec1"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}	
	
	"BtnSpec2"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSpec2"
		"xpos"						"0"
		"ypos"						"50"
		"wide"						"100"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSpec1"
		"navDown"					"BtnSpec3"
		"labelText"					"#L4D360UI_Controller_Buttons_Config2"
		"style"						"LeftDialogButton"
		"command" 					"BtnSpec2"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}	
	
	"BtnSpec3"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSpec3"
		"xpos"						"0"
		"ypos"						"75"
		"wide"						"100"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSpec2"
		"navDown"					"BtnSpec4"
		"labelText"					"#L4D360UI_Controller_Buttons_Config3"
		"style"						"LeftDialogButton"
		"command" 					"BtnSpec3"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}
	
	"BtnSpec4"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSpec4"
		"xpos"						"0"
		"ypos"						"100"
		"wide"						"100"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSpec3"
		"navDown"					"BtnSpecCustom"
		"labelText"					"#L4D360UI_Controller_Buttons_Config4"
		"style"						"LeftDialogButton"
		"command" 					"BtnSpec4"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}
	
	"BtnSpecCustom"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnSpecCustom"
		"xpos"						"0"
		"ypos"						"125"
		"wide"						"100"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnSpec4"
		"navDown"					"BtnSpec1"
		"labelText"					"#PORTAL2_Controller_Buttons_Custom"
		"style"						"LeftDialogButton"
		"command" 					"BtnSpecCustom"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"IgnoreButtonA"				"1"
		"FocusDisabledBorderSize"	"1"
	}	

	"bgpanel"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"bgpanel"
		"xpos"					"100"
		"ypos"					"0"
		"zpos"					"-999"	// otherwise it eats focus
		"wide"					"400"
		"tall"					"250"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"drawcolor_override"	"0 0 0 50"
		"scaleImage"			"1"
		"image"					"white"
	}

	"listpanel_custombindlist"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"			"listpanel_custombindlist"
		"xpos"				"100"
		"ypos"				"0"
		"wide"				"400"
		"tall"				"250"
		"autoResize"		"0"
		"visible"			"0"
		"enabled"			"1"
		"tabPosition"		"0"
		"bgcolor_override"	"0 0 0 0"
		"linespacing"		"25"
	}	
		
	"LabelContainer"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"LabelContainer"
		"xpos"				"50"
		"ypos"				"25"
		"wide"				"550"
		"tall"				"270"
		"visible"			"1"
		"enabled"			"1"
		
		"ControllerImage"
		{
			"ControlName"			"ImagePanel"
			"fieldName"				"ControllerImage"
			"xpos"					"110"
			"ypos"					"0"
			"wide"					"200"
			"tall"					"200"
			"autoResize"			"0"
			"visible"				"1"
			"enabled"				"1"		
			"scaleImage"			"1"
			"image"					"controller_layout_buttons" [!$PS3]
			"image"					"controller_layout_buttons_ps3" [$PS3]
		}
		
		"ControllerImageCallouts"
		{
			"ControlName"			"ImagePanel"
			"fieldName"				"ControllerImageCallouts"
			"xpos"					"110"
			"ypos"					"0"
			"wide"					"200"
			"tall"					"200"
			"autoResize"			"0"
			"visible"				"1"
			"enabled"				"1"		
			"scaleImage"			"1"
			"image"					"controller_stick_callouts_360" [!$PS3]
			"image"					"controller_stick_callouts_ps3" [$PS3]
		}

		// Xbox 360: Left Stick - Ping - Gesture
		// PS3 : Ping - Gesture - Left Stick
		
		"LblDPadPing"
		{
			"ControlName"				"Label"
			"fieldName"					"LblDPadPing"
			"xpos"						"-22"	[!$GAMECONSOLE]
			"ypos"						"164"	[!$GAMECONSOLE]
			"xpos"						"-22"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"166"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"-22"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"163"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"-22"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"166"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"-22"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"162"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"160"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"east"	[!$PS3]
			"textAlignment"				"east"	[$PS3]
			"Font"						"ControllerLayout"
			"LabelText"					"Ping"
		}
		
		"LblDPadGest"
		{
			"ControlName"				"Label"
			"fieldName"					"LblDPadGest"
			"xpos"						"178"	[!$GAMECONSOLE]
			"ypos"						"167"	[!$GAMECONSOLE]
			"xpos"						"178"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"170"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"178"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"166"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"-50"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"131"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"-50"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"127"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"160"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"	[!$PS3]
			"textAlignment"				"east"	[$PS3]
			"Font"						"ControllerLayout"
			"LabelText"					"Gesture"
		}
		
		"LblLStick"
		{
			"ControlName"				"Label"
			"fieldName"					"LblLStick"
			"xpos"						"-50"	[!$GAMECONSOLE]
			"ypos"						"129"	[!$GAMECONSOLE]
			"xpos"						"-50"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"133"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"-50"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"128"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"178"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"168"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"178"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"165"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"160"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"east"	[!$PS3]
			"textAlignment"				"west"	[$PS3]
			"Font"						"ControllerLayout"
			"LabelText"					"LeftStick"
		}
	
		"LblRStick"
		{
			"ControlName"				"Label"
			"fieldName"					"LblRStick"
			"xpos"						"235"	[!$GAMECONSOLE]
			"ypos"						"157"	[!$GAMECONSOLE]
			"xpos"						"235"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"160"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"235"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"156"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"235"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"159"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"235"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"154"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"215"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Sniper Rifle Zoom*"
		}
		
		"LblAButton"
		{
			"ControlName"				"Label"
			"fieldName"					"LblAButton"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"112"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"114"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"110"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"113"	[$PS3 && !$INPUTSWAPAB && $GAMECONSOLEHIDEF]
			"ypos"						"94"	[$PS3 && $INPUTSWAPAB && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"109"	[$PS3 && !$INPUTSWAPAB && $GAMECONSOLELODEF]
			"ypos"						"90"	[$PS3 && $INPUTSWAPAB && $GAMECONSOLELODEF]
			"wide"						"200"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Jump*"
		}
		
		"LblBButton"
		{
			"ControlName"				"Label"
			"fieldName"					"LblBButton"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"93"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"94"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"90"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"94"	[$PS3 && !$INPUTSWAPAB && $GAMECONSOLEHIDEF]
			"ypos"						"113"	[$PS3 && $INPUTSWAPAB && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"90"	[$PS3 && !$INPUTSWAPAB && $GAMECONSOLELODEF]
			"ypos"						"109"	[$PS3 && $INPUTSWAPAB && $GAMECONSOLELODEF]
			"wide"						"200"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Reload*"
		}
		
		"LblYButton"
		{
			"ControlName"				"Label"
			"fieldName"					"LblYButton"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"74"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"75"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"71"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"75"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"71"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"200"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Switch Weapons*"
		}
		
		"LblXButton"
		{
			"ControlName"				"Label"
			"fieldName"					"LblXButton"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"131"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"132"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"128"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"132"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"128"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"200"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Interact / Pick Up*"
		}
		
		"LblRShoulder"
		{
			"ControlName"				"Label"
			"fieldName"					"LblRShoulder"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"55"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"57"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"53"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"329"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"37"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"329"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"33"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"200"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Look Spin*"
		}
		
		"LblRShoulderHintPS3" [$PS3]
		{
			"ControlName"				"Label"
			"fieldName"					"LblRShoulderHintPS3"
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"37"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"33"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"200"
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"R1:"
		}
		
		"LblRTrigger"
		{
			"ControlName"				"Label"
			"fieldName"					"LblRTrigger"
			"xpos"						"310"	[!$GAMECONSOLE]
			"ypos"						"36"	[!$GAMECONSOLE]
			"xpos"						"310"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"38"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"310"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"34"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"329"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"56"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"329"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"52"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"220"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Fire*"
		}				
		
		"LblRTriggerHintPS3" [$PS3]
		{
			"ControlName"				"Label"
			"fieldName"					"LblRTriggerHintPS3"
			"xpos"						"309"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"56"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"309"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"52"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"200"
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"R2:"
		}				
		
		"LblLTrigger"
		{
			"ControlName"				"Label"
			"fieldName"					"LblLTrigger"
			"xpos"						"155"	[!$GAMECONSOLE]
			"ypos"						"8"	[!$GAMECONSOLE]
			"xpos"						"155"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"11"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"155"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"7"		[$X360 && $GAMECONSOLELODEF]
			"xpos"						"187"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"26"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"187"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"22"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"220"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Melee Shove*"
		}
				
		"LblLTriggerHintPS3" [$PS3]
		{
			"ControlName"				"Label"
			"fieldName"					"LblLTriggerHintPS3"
			"xpos"						"86"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"26"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"86"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"22"	[$PS3 && $GAMECONSOLELODEF]
			"wide"						"100"
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"east"
			"Font"						"ControllerLayout"
			"LabelText"					"L2: "
		}
				
		"LblLShoulder"
		{
			"ControlName"				"Label"
			"fieldName"					"LblLShoulder"
			"xpos"						"187"	[!$GAMECONSOLE]
			"ypos"						"24"	[!$GAMECONSOLE]
			"xpos"						"187"	[$X360 && $GAMECONSOLEHIDEF]
			"ypos"						"27"	[$X360 && $GAMECONSOLEHIDEF]
			"xpos"						"187"	[$X360 && $GAMECONSOLELODEF]
			"ypos"						"24"	[$X360 && $GAMECONSOLELODEF]
			"xpos"						"155"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"11"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"155"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"7"		[$PS3 && $GAMECONSOLELODEF]
			"wide"						"197"
			"tall"						"20"	[!$GAMECONSOLE]
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"west"
			"Font"						"ControllerLayout"
			"LabelText"					"Crouch*"
		}
		
		"LblLShoulderHintPS3" [$PS3]
		{
			"ControlName"				"Label"
			"fieldName"					"LblLShoulderHintPS3"
			"xpos"						"54"	[$PS3 && $GAMECONSOLEHIDEF]
			"ypos"						"11"	[$PS3 && $GAMECONSOLEHIDEF]
			"xpos"						"54"	[$PS3 && $GAMECONSOLELODEF]
			"ypos"						"7"		[$PS3 && $GAMECONSOLELODEF]
			"wide"						"100"
			"wrap"						"0"
			"autoResize"				"0"
			"visible"					"1"
			"enabled"					"1"
			"tabPosition"				"0"
			"textAlignment"				"east"
			"Font"						"ControllerLayout"
			"LabelText"					"L1: "
		}
	}
}
