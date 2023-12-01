"Resource/UI/KeyboardMouse.res"
{
	"KeyboardMouse"
	{
		"ControlName"		"Frame"
		"fieldName"			"KeyboardMouse"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"7"
		"tall"				"4"		[$OSX]
		"tall"				"5"		[!$OSX]
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
	
	"BtnEditBindings"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnEditBindings"
		"xpos"					"0"
		"ypos"					"25"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"navUp"					"DrpDeveloperConsole"
		"navDown"				"DrpMouseYInvert"
		"labelText"				"#L4D360UI_Controller_Edit_Keys_Buttons"
		"style"					"DefaultButton"
		"command"				"#L4D360UI_Controller_Edit_Keys_Buttons"
	}
	
	"DrpMouseYInvert"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpMouseYInvert"
		"xpos"				"0"
		"ypos"				"50"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"navUp"				"BtnEditBindings"
		"navDown"			"SldMouseSensitivity"
		"labelText"			"#GameUI_ReverseMouse"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"	"MouseYInvertDisabled"
			"#L4D360UI_Enabled"		"MouseYInvertEnabled"
		}
	}
	
	"SldMouseSensitivity"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldMouseSensitivity"
		"xpos"					"0"
		"ypos"					"75"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"1.0f"
		"maxValue"				"20.0f"
		"stepSize"				"1.0"
		"navUp"					"DrpMouseYInvert"
		"navDown"				"DrpRawMouse"
		"conCommand"			"sensitivity"
		"inverseFill"			"0"
				
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"0"
			"wide"					"0"
			"tall"					"0"
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#GameUI_MouseSensitivity"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}
	
	"DrpRawMouse"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpRawMouse"
		"xpos"				"0"
		"ypos"				"100"
		"wide"				"0"
		"tall"				"20"
		"visible"			"0"		[$OSX]
		"enabled"			"0"		[$OSX]
		"visible"			"1"		[!$OSX]
		"enabled"			"1"		[!$OSX]
		"tabPosition"		"0"
		"navUp"				"SldMouseSensitivity"
		"navDown"			"DrpMouseAcceleration"
		"labelText"			"#GameUI_RawMouse"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"	"RawMouseDisabled"
			"#L4D360UI_Enabled"		"RawMouseEnabled"
		}
	}

	"DrpMouseAcceleration"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpMouseAcceleration"
		"xpos"				"0"
		"ypos"				"100"	[$OSX]
		"ypos"				"125"	[!$OSX]
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"navUp"				"DrpRawMouse"
		"navDown"			"SldMouseAcceleration"
		"labelText"			"#GameUI_MouseAcceleration"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"	"MouseAccelerationDisabled"
			"#L4D360UI_Enabled"		"MouseAccelerationEnabled"
		}
	}

	"SldMouseAcceleration"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldMouseAcceleration"
		"xpos"					"0"
		"ypos"					"125"	[$OSX]
		"ypos"					"150"	[!$OSX]
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"1.0f"
		"maxValue"				"1.4f"
		"stepSize"				"0.01"
		"navUp"					"DrpMouseAcceleration"
		"navDown"				"DrpDeveloperConsole"
		"conCommand"			"m_customaccel_exponent"
		"inverseFill"			"0"
				
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"0"
			"wide"					"0"
			"tall"					"0"
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#GameUI_MouseAccelerationAmount"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}

	"DrpDeveloperConsole"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpDeveloperConsole"
		"xpos"				"0"
		"ypos"				"150"	[$OSX]
		"ypos"				"175"	[!$OSX]
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"navUp"				"SldMouseAcceleration"
		"navDown"			"BtnEditBindings"
		"labelText"			"#L4D360UI_KeyboardOptions_Developer"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"	"DeveloperConsoleDisabled"
			"#L4D360UI_Enabled"		"DeveloperConsoleEnabled"
		}
	}
}
