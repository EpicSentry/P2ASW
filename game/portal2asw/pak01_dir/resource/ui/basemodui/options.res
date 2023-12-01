"Resource/UI/options.res"
{
	"Options"
	{
		"ControlName"		"Frame"
		"fieldName"			"Options"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"4"		[$GAMECONSOLE]
		"wide"				"5"		[!$GAMECONSOLE]
		"tall"				"2"		[$GAMECONSOLE]
		"tall"				"3"		[!$GAMECONSOLE]
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
	
	"BtnAudioVideo"	[$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnAudioVideo"
		"xpos"						"0"
		"ypos"						"25"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnController"
		"navDown"					"BtnController"
		"labelText"					"#L4D360UI_AudioVideo"
		"style"						"DefaultButton"
		"command"					"AudioVideo"
		"ActivationType"			"1"
	}

	"BtnController" [$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnController"
		"xpos"						"0"
		"ypos"						"50"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnAudioVideo"	
		"navDown"					"BtnAudioVideo"
		"labelText"					"#L4D360UI_Controller"
		"style"						"DefaultButton"
		"command"					"Controller"
		"ActivationType"			"1"
	}

	"BtnAudio"	[!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnAudio"
		"xpos"						"0"
		"ypos"						"25"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnController"
		"navDown"					"BtnVideo"
		"labelText"					"#GameUI_Audio"
		"style"						"DefaultButton"
		"command"					"Audio"
		"ActivationType"			"1"
	}

	"BtnVideo"	[!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnVideo"
		"xpos"						"0"
		"ypos"						"50"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnAudio"
		"navDown"					"BtnKeyboardMouse"
		"labelText"					"#GameUI_Video"
		"style"						"DefaultButton"
		"command"					"Video"
		"ActivationType"			"1"
	}

	"BtnKeyboardMouse"	[!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnKeyboardMouse"
		"xpos"						"0"
		"ypos"						"75"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnVideo"
		"navDown"					"BtnController"
		"labelText"					"#L4D360UI_KeyboardMouse"
		"style"						"DefaultButton"
		"command"					"KeyboardMouse"
		"ActivationType"			"1"
	}

	"BtnController" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnController"
		"xpos"						"0"
		"ypos"						"100"
		"wide"						"0"
		"tall"						"20"
		"autoResize"				"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnKeyboardMouse"	
		"navDown"					"BtnAudio"
		"labelText"					"#L4D360UI_Controller"
		"style"						"DefaultButton"
		"command"					"Controller"
		"ActivationType"			"1"
	}
}