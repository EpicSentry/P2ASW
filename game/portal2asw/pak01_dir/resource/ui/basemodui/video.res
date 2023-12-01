"Resource/UI/Video.res"
{
	"Video"
	{
		"ControlName"		"Frame"
		"fieldName"			"Video"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"7"
		"tall"				"4"
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
		"IgnoreButtonA"		"1"
	}
	
	"SldBrightness"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldBrightness"
		"xpos"					"0"
		"ypos"					"25"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"IgnoreButtonA"			"1"
		"?windowed"
		{
			"visible"				"0"
			"enabled"				"0"
		}
		"usetitlesafe"			"0"
		"tabPosition"			"0"
		"minValue"				"1.8"
		"maxValue"				"2.6"
		"stepSize"				"0.05"
		"navUp"					"BtnAdvanced"
		"navDown"				"DrpAspectRatio"
		"conCommand"			"mat_monitorgamma"
		"inverseFill"			"1"

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
			"IgnoreButtonA"			"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_VideoOptions_Brightness"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}

	"DrpAspectRatio"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpAspectRatio"
		"xpos"				"0"
		"ypos"				"50"
		"?windowed"
		{
			"ypos"				"25"
		}
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"SldBrightness"
		"navDown"			"DrpResolution"
		"labelText"			"#GameUI_AspectRatio"
		"style"				"DialogListButton"
		"list"
		{
			"#GameUI_AspectNormal"		"#GameUI_AspectNormal"
			"#GameUI_AspectWide16x9"	"#GameUI_AspectWide16x9"
			"#GameUI_AspectWide16x10"	"#GameUI_AspectWide16x10"
		}
	}
	
	"DrpResolution"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpResolution"
		"xpos"				"0"
		"ypos"				"75"
		"?windowed"
		{
			"ypos"				"50"
		}
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpAspectRatio"
		"navDown"			"DrpDisplayMode"
		"labelText"			"#GameUI_Resolution"
		"style"				"DialogListButton"
		"list"
		{
			"_res0"		"_res0"
			"_res1"		"_res1"
			"_res2"		"_res2"
			"_res3"		"_res3"
			"_res4"		"_res4"
			"_res5"		"_res5"
			"_res6"		"_res6"
			"_res7"		"_res7"
			"_res8"		"_res8"
			"_res9"		"_res9"
			"_res:"		"_res:"
			"_res;"		"_res;"
			"_res<"		"_res<"
			"_res="		"_res="
			"_res>"		"_res>"
			"_res?"		"_res?"
			"_res@"		"_res@"
			"_resA"		"_resA"
			"_resB"		"_resB"	
			"_resC"		"_resC"
			"_resD"		"_resD"
			"_resE"		"_resE"
			"_resF"		"_resF"
			"_resG"		"_resG"
			"_resH"		"_resH"
			"_resI"		"_resI"
			"_resJ"		"_resJ"
			"_resK"		"_resK"
			"_resL"		"_resL"
			"_resM"		"_resM"
			"_resN"		"_resN"
			"_resO"		"_resO"
			"_resP"		"_resP"
			"_resQ"		"_resQ"
			"_resR"		"_resR"
			"_resS"		"_resS"
			"_resT"		"_resT"
			"_resU"		"_resU"
			"_resV"		"_resV"
			"_resW"		"_resW"
			"_resX"		"_resX"
			"_resY"		"_resY"
			"_resZ"		"_resZ" 
		}
	}
	
	"DrpDisplayMode"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpDisplayMode"
		"xpos"				"0"
		"ypos"				"100"
		"?windowed"
		{
			"ypos"				"75"
		}
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpResolution"
		"navDown"			"DrpPowerSavingsMode"
		"labelText"			"#GameUI_DisplayMode"
		"style"				"DialogListButton"
		"list"
		{
			"#GameUI_Fullscreen"						"#GameUI_Fullscreen"
			"#GameUI_Windowed"							"#GameUI_Windowed"
			"#L4D360UI_VideoOptions_Windowed_NoBorder"	"#L4D360UI_VideoOptions_Windowed_NoBorder"	[!$OSX]
		}
	}
	
	"DrpPowerSavingsMode"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpPowerSavingsMode"
		"xpos"				"0"
		"ypos"				"125"
		"?windowed"
		{
			"ypos"				"100"
		}
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpDisplayMode"
		"navDown"			"DrpSplitScreenDirection"
		"labelText"			"#GameUI_PowerSavingsMode"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"			"PowerSavingsDisabled"
			"#L4D360UI_Enabled"				"PowerSavingsEnabled"
		}
	}

	"DrpSplitScreenDirection"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpSplitScreenDirection"
		"xpos"				"0"
		"ypos"				"150"
		"?windowed"
		{
			"ypos"				"125"
		}
		"zpos"				"1"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"usetitlesafe"		"0"
		"tabPosition"		"0"
		"IgnoreButtonA"		"1"
		"navUp"				"DrpPowerSavingsMode"
		"navDown"			"BtnAdvanced"
		"labelText"			"#L4D360UI_VideoOptions_SplitScreenDirection"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_SplitScreenDirection_Default"	"#L4D360UI_SplitScreenDirection_Default"
			"#L4D360UI_SplitScreenDirection_Horizontal"	"#L4D360UI_SplitScreenDirection_Horizontal"
			"#L4D360UI_SplitScreenDirection_Vertical"	"#L4D360UI_SplitScreenDirection_Vertical"
		}
	}

	"BtnAdvanced"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnAdvanced"
		"xpos"					"0"
		"ypos"					"175"
		"?windowed"
		{
			"ypos"				"150"
		}
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"DrpSplitScreenDirection"
		"navDown"				"SldBrightness"
		"labelText"				"#PORTAL2_AdvancedVideo"
		"style"					"DefaultButton"
		"command"				"ShowAdvanced"
	}
}
	
