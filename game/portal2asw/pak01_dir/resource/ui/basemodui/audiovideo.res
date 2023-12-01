"Resource/UI/AudioVideo.res"
{
	"AudioVideo"
	{
		"ControlName"		"Frame"
		"fieldName"			"AudioVideo"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"0"
		"tall"				"0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}
		
	"SldBrightness"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldBrightness"
		"xpos"					"0"
		"ypos"					"25"
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"usetitlesafe"			"0"
		"tabPosition"			"0"
		"minValue"				"1.8"
		"maxValue"				"2.6"
		"stepSize"				"0.05"
		"navUp"					"DrpLanguage"
		"navDown"				"DrpColorMode"
		"conCommand"			"mat_monitorgamma"
		"usetitlesafe"			"0"
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
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_VideoOptions_Brightness"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}

	"DrpColorMode"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpColorMode"
		"xpos"				"0"
		"ypos"				"50"
		"zpos"				"1"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1" [!$PS3]
		"visible"			"0" [$PS3]
		"enabled"			"1"
		"tabPosition"		"0"
		"navUp"				"SldBrightness"
		"navDown"			"DrpSplitScreenDirection"
		"labelText"			"#L4D360UI_VideoOptions_ColorMode"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_ColorMode_Television"	"#L4D360UI_ColorMode_Television"
			"#L4D360UI_ColorMode_LCD"			"#L4D360UI_ColorMode_LCD"
		}
	}
		
	"DrpSplitScreenDirection"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpSplitScreenDirection"
		"xpos"				"0"
		"ypos"				"75" [!$PS3]
		"ypos"				"50" [$PS3]
		"zpos"				"1"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"usetitlesafe"		"0"
		"tabPosition"		"0"
		"navUp"				"DrpColorMode"
		"navDown"			"SldGameVolume"
		"labelText"			"#L4D360UI_VideoOptions_SplitScreenDirection"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_SplitScreenDirection_Default"	"#L4D360UI_SplitScreenDirection_Default"
			"#L4D360UI_SplitScreenDirection_Horizontal"	"#L4D360UI_SplitScreenDirection_Horizontal"
			"#L4D360UI_SplitScreenDirection_Vertical"	"#L4D360UI_SplitScreenDirection_Vertical"
		}
	}
		
	"SldGameVolume"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldGameVolume"
		"xpos"					"0"
		"ypos"					"100" [!$PS3]
		"ypos"					"75" [$PS3]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"minValue"				"0.0"
		"maxValue"				"1.0"
		"stepSize"				"0.05"
		"navUp"					"DrpSplitScreenDirection"
		"navDown"				"SldMusicVolume"
		"conCommand"			"volume"
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
			"tall"					"20"
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_AudioOptions_Volume"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}
	
	"SldMusicVolume"
	{
		"ControlName"			"SliderControl"
		"fieldName"				"SldMusicVolume"
		"xpos"					"0"
		"ypos"					"125" [!$PS3]
		"ypos"					"100" [$PS3]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"usetitlesafe"			"0"
		"tabPosition"			"0"
		"minValue"				"0.0"
		"maxValue"				"1.0"
		"stepSize"				"0.05"
		"navUp"					"SldGameVolume"
		"navDown"				"DrpCaptioning"
		"conCommand"			"snd_musicvolume"
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
			"tall"					"20"
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_AudioOptions_MusicVolume"
			"style"					"LeftDialogButton"
			"command"				""
			"ActivationType"		"1"
			"usablePlayerIndex"		"nobody"
		}
	}
	
	"DrpCaptioning"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpCaptioning"
		"xpos"				"0"
		"ypos"				"150" [!$PS3]
		"ypos"				"125" [$PS3]
		"zpos"				"3"
		"wide"				"0"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"usetitlesafe"		"0"
		"tabPosition"		"0"
		"navUp"				"SldMusicVolume"
		"navDown"			"DrpLanguage"
		"labelText"			"#L4D360UI_AudioOptions_Caption"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_AudioOptions_CaptionOff"			"#L4D360UI_AudioOptions_CaptionOff"
			"#L4D360UI_AudioOptions_CaptionSubtitles"	"#L4D360UI_AudioOptions_CaptionSubtitles"
			"#L4D360UI_AudioOptions_CaptionOn"			"#L4D360UI_AudioOptions_CaptionOn"
		}
	}
		
	"DrpLanguage"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"DrpLanguage"
		"xpos"					"0"
		"ypos"					"175" [!$PS3]
		"ypos"					"150" [$PS3]
		"zpos"					"3"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"usetitlesafe"			"0"
		"tabPosition"			"0"
		"navUp"					"DrpCaptioning"
		"navDown"				"SldBrightness"
		"labelText"				"#L4D360UI_AudioOptions_Language"
		"style"					"DialogListButton"
		"list"
		{
			"#GameUI_Language_English"	"CurrentXBXLanguage"
			"#GameUI_Language_English"	"CurrentXBXLanguage_English"
		}
	}
}