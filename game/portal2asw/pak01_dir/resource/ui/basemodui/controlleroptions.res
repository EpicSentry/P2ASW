"Resource/UI/ControllerOptions.res"
{
	"ControllerOptions"
	{
		"ControlName"		"Frame"
		"fieldName"			"ControllerOptions"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"7"
		"tall"				"4"		[$GAMECONSOLE]
		"tall"				"5"		[!$GAMECONSOLE]
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
	
	"DrpController"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"DrpController"
		"xpos"						"0"
		"ypos"						"25"
		"zpos"						"3"
		"wide"						"0"
		"tall"						"20"
		"visible"					"0"		[$GAMECONSOLE]
		"enabled"					"0"		[$GAMECONSOLE]
		"visible"					"1"		[!$GAMECONSOLE]
		"enabled"					"1"		[!$GAMECONSOLE]
		"tabPosition"				"0"
		"navUp"						"BtnDefaults"
		"navDown"					"BtnEditButtons"
		"labelText"					"#L4D360UI_Controller"
		"style"						"DialogListButton"
		"OnlyActiveUser"			"1"
		"list"
		{
			"#L4D360UI_Disabled"	"ControllerDisabled"
			"#L4D360UI_Enabled"		"ControllerEnabled"
		}
	}		
		
	"BtnEditButtons"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnEditButtons"
		"xpos"						"0"
		"ypos"						"25"	[$GAMECONSOLE]
		"ypos"						"50"	[!$GAMECONSOLE]
		"zpos"						"3"
		"wide"						"0"
		"tall"						"20"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"DrpController"
		"navDown"					"BtnEditSticks"
		"labelText"					"#L4D360UI_Controller_Edit_Buttons"
		"style"						"LeftDialogButton"
		"command" 					"EditButtons"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"FocusDisabledBorderSize"	"1"
	}	
		
	"BtnEditSticks"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnEditSticks"
		"xpos"						"0"
		"ypos"						"50"	[$GAMECONSOLE]
		"ypos"						"75"	[!$GAMECONSOLE]
		"zpos"						"3"
		"wide"						"0"
		"tall"						"20"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnEditButtons"
		"navDown"					"SldVertSens"
		"labelText"					"#L4D360UI_Controller_Edit_Sticks"
		"style"						"LeftDialogButton"
		"command" 					"EditSticks"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"FocusDisabledBorderSize"	"1"
	}	
		
	"SldVertSens"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldVertSens"
		"xpos"					"0"
		"ypos"					"75"	[$GAMECONSOLE]
		"ypos"					"100"	[!$GAMECONSOLE]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"0.3"
		"maxValue"				"3.05"
		"inverseFill"			"0"
		"stepSize"				"0.25"
		"navUp"					"BtnEditSticks"
		"navDown"				"SldHorizSens"
		"conCommand"			"joy_pitchsensitivity"
		"conCommandDefault"		"joy_pitchsensitivity_default"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"0"
			"wide"					"0"
			"tall"					"20"
			"autoResize"			"1"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_Controller_Pitch"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"	
			"OnlyActiveUser"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}	
	
	"SldHorizSens"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldHorizSens"
		"xpos"					"0"
		"ypos"					"100"	[$GAMECONSOLE]
		"ypos"					"125"	[!$GAMECONSOLE]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"-3.0"
		"maxValue"				"-0.3"
		"stepSize"				"0.25"
		"inverseFill"			"1"
		"navUp"					"SldVertSens"
		"navDown"				"DrpLookType"
		"conCommand"			"joy_yawsensitivity"
		"conCommandDefault"		"joy_yawsensitivity_default"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"0"
			"wide"					"0"
			"tall"					"20"
			"autoResize"			"1"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_Controller_Yaw"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"OnlyActiveUser"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}

	"DrpLookType"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpLookType"
		"xpos"				"0"
		"ypos"				"125"	[$GAMECONSOLE]
		"ypos"				"150"	[!$GAMECONSOLE]
		"zpos"				"3"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"navUp"				"SldHorizSens"
		"navDown"			"DrpVibration"
		"labelText"			"#L4D360UI_Controller_LookType"
		"style"				"DialogListButton"
		"OnlyActiveUser"	"1"
		"list"
		{
			"#L4D360UI_Controller_Normal"	"#L4D360UI_Controller_Normal"
			"#L4D360UI_Controller_Inverted"	"#L4D360UI_Controller_Inverted"
		}
	}
	
	"DrpVibration"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpVibration"
		"xpos"				"0"
		"ypos"				"175"
		"zpos"				"3"
		"wide"				"0"
		"tall"				"20"
		"visible"			"0"		[$GAMECONSOLE]
		"enabled"			"0"		[$GAMECONSOLE]
		"visible"			"1"		[!$GAMECONSOLE]
		"enabled"			"1"		[!$GAMECONSOLE]
		"tabPosition"		"0"
		"navUp"				"DrpLookType"
		"navDown"			"SldAcceleration"
		"labelText"			"#L4D360UI_Vibration"
		"style"				"DialogListButton"
		"OnlyActiveUser"	"1"
		"list"
		{
			"#L4D360UI_Disabled"	"VibrationDisabled"
			"#L4D360UI_Enabled"		"VibrationEnabled"
		}
	}

	"SldAcceleration"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldAcceleration"
		"xpos"					"0"
		"ypos"					"225"	[$GAMECONSOLE]
		"ypos"					"225"	[!$GAMECONSOLE]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"0"		[$GAMECONSOLE]
		"visible"				"1"		[!$GAMECONSOLE]
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"0"
		"maxValue"				"0.0125"
		"stepSize"				"0.00125"
		"inverseFill"			"0"
		"navUp"					"DrpVibration"
		"navDown"				"BtnDefaults"
		"conCommand"			"sc_accel_scale"
		"conCommandDefault"		"sc_accel_scale_default"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"0"
			"wide"					"0"
			"tall"					"20"
			"autoResize"			"1"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_Controller_Acceleration"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"OnlyActiveUser"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}


	"BtnDefaults"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnDefaults"
		"xpos"						"0"
		"ypos"						"150"
		"zpos"						"3"
		"wide"						"0"
		"tall"						"20"
		"visible"					"1"		[$GAMECONSOLE]
		"visible"					"0"		[!$GAMECONSOLE]
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"SldAcceleration"
		"navDown"					"DrpController"
		"labelText"					"#L4D360UI_Controller_Default"
		"style"						"LeftDialogButton"
		"command" 					"Defaults"
		"ActivationType"			"1"
		"OnlyActiveUser"			"1"
		"FocusDisabledBorderSize"	"1"
	}
}
