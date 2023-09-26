"Resource/UI/OptionsFlyout.res"
{
	"PnlBackground"
	{
		"ControlName"		"Panel"
		"fieldName"			"PnlBackground"
		"xpos"				"0"
		"ypos"				"0"
		"zpos"				"-1"
		"wide"				"156"
		"tall"				"65" [$X360]
		"tall"				"145" [$WIN32]
		"visible"			"1"
		"enabled"			"1"
		"paintbackground"	"1"
		"paintborder"		"1"
	}

	"BtnVideo"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnVideo"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnCloud"
		"navDown"				"BtnBrightness"
		"tooltiptext"			"#L4D_video_tip"
		"labelText"				"#GameUI_Video"
		"style"					"FlyoutMenuButton"
		"command"				"Video"
	}

	"BtnBrightness"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnBrightness"
		"xpos"					"0"
		"ypos"					"24"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnVideo"
		"navDown"				"BtnAudio"
		"tooltiptext"			"#GameUI_AdjustGamma"
		"disabled_tooltiptext"	"#L4D360UI_AdjustGamma_Disabled"
		"labelText"				"#GameUI_Brightness"
		"style"					"FlyoutMenuButton"
		"command"				"Brightness"
	}

	"BtnAudio"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnAudio"
		"xpos"					"0"
		"ypos"					"48"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnBrightness"
		"navDown"				"BtnController"
		"tooltiptext"			"#L4D_audio_tip"
		"labelText"				"#GameUI_Audio"
		"style"					"FlyoutMenuButton"
		"command"				"Audio"
	}

	"BtnController"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnController"
		"xpos"					"0"
		"ypos"					"72"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
		"navUp"					"BtnAudio"
		"navDown"				"BtnMultiplayer"
		"tooltiptext"			"#L4D_keyboard_tip"
		"labelText"				"#L4D360UI_KeyboardMouse"
		"style"					"FlyoutMenuButton"
		"command"				"KeyboardMouse"
	}

	"BtnMultiplayer"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnMultiplayer"
		"xpos"					"0"
		"ypos"					"96"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
		"navUp"					"BtnController"
		"navDown"				"BtnCloud"
		"tooltiptext"			"#L4D_multiplayer_tip"
		"labelText"				"#GameUI_Multiplayer"
		"style"					"FlyoutMenuButton"
		"command"				"MultiplayerSettings"
	}

	"BtnCloud"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnCloud"
		"xpos"					"0"
		"ypos"					"120"
		"wide"					"150"
		"tall"					"24"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
		"navUp"					"BtnMultiplayer"
		"navDown"				"BtnVideo"
		"tooltiptext"			"#L4D_steamcloud_tip"
		//"disabled_tooltiptext"	"#L4D_steamcloud_disabled_tip"
		"labelText"				"#L4D360UI_Cloud_Title"
		"style"					"FlyoutMenuButton"
		"command"				"CloudSettings"
	}
}
