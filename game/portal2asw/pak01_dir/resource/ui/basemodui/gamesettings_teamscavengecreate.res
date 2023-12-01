"Resource/UI/GameSettings_ScavengeCreate.res"
{
	"GameSettings"
	{
		"ControlName"				"Frame"
		"fieldName"					"GameSettings"
		"xpos"						"0"
		"ypos"						"0"
		"wide"						"f0"
		"tall"						"260"
		"autoResize"				"0"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
	}
	
	"ImgBackground"
	{
		"ControlName"			"L4DMenuBackground"
		"fieldName"				"ImgBackground"
		"xpos"					"0"
		"ypos"					"179"
		"zpos"					"-1"
		"wide"					"f0"
		"tall"					"144"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"		[$WIN32]
		"visible"				"0"		[$GAMECONSOLE]
		"enabled"				"1"
		"tabPosition"			"0"
		"fillColor"				"0 0 0 0"
	} 

	"ImgLevelImage"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"ImgLevelImage"
		"xpos"					"c51"
		"ypos"					"125"
		"wide"					"200"
		"tall"					"100"
		"scaleImage"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"maps/any"
		"scaleImage"			"1"
	}
	"ImgLevelImageFrame"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"ImgLevelImageFrame"
		"xpos"					"c45"
		"ypos"					"113"
		"wide"					"246"
		"tall"					"123"
		"scaleImage"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"campaignFrame"
		"scaleImage"			"1"
	}
			
	// Campaign dropdown
	"DrpMission"
	{
		"ControlName"			"DropDownMenu"
		"fieldName"				"DrpMission"
		"xpos"					"c-250"
		"ypos"					"115"
		"zpos"					"1"
		"wide"					"280"
		"tall"					"15"			[$WIN32]
		"tall"					"20"			[$GAMECONSOLE]
		"visible"				"1"
		"enabled"				"1"
		"navUp"					"BtnCancel"		[$WIN32]
		"navUp"					"BtnStartLobby" [$GAMECONSOLE]
		"navDown"				"DrpMissionExtended"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"					"BaseModHybridButton"
			"fieldName"						"BtnDropButton"
			"xpos"							"0"
			"ypos"							"0"
			"zpos"							"2"
			"wide"							"280"
			"wideatopen"					"160"
			"tall"							"15"	[$WIN32]
			"tall"							"20"	[$GAMECONSOLE]
			"autoResize"					"1"
			"pinCorner"						"0"
			"visible"						"1"
			"enabled"						"1"
			"tabPosition"					"0"
			"labelText"						"#L4D360UI_GameSettings_Mission"
			"tooltiptext"					"#L4D360UI_GameSettings_Tooltip_Mission"
			"disabled_tooltiptext"			"#L4D360UI_GameSettings_Tooltip_Mission_Disabled"
			"style"							"DropDownButton"
			"command"						"FlmMission"
			"ActivationType"				"1"		[$GAMECONSOLE]
			"EnableCondition"				"Never" [$DEMO]
		}
	}
	
	// Campaign flyout	
	"FlmMission"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmMission"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"4"
		"InitialFocus"			"BtnCampaign1"
		"ResourceFile"			"resource/UI/basemodui/DropDownMissionScavenge.res"
	}
	
	"DrpMissionExtended"
	{
		"ControlName"			"DropDownMenu"
		"fieldName"				"DrpMissionExtended"
		"xpos"					"c-250"	
		"ypos"					"115"			[$WIN32]
		"ypos"					"115"			[$GAMECONSOLE]
		"zpos"					"1"
		"wide"					"280"
		"tall"					"15"			[$WIN32]
		"tall"					"20"			[$GAMECONSOLE]
		"visible"				"0"
		"enabled"				"1"
		"navUp"					"DrpMission"
		"navDown"				"DrpChapter"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"					"BaseModHybridButton"
			"fieldName"						"BtnDropButton"
			"xpos"							"0"
			"ypos"							"0"
			"zpos"							"2"
			"wide"							"280"
			"wideatopen"					"160"
			"tall"							"15"	[$WIN32]
			"tall"							"20"	[$GAMECONSOLE]
			"autoResize"					"1"
			"pinCorner"						"0"
			"visible"						"1"
			"enabled"						"1"
			"tabPosition"					"0"
			"labelText"						"#L4D360UI_GameSettings_Mission"
			"tooltiptext"					"#L4D360UI_GameSettings_Tooltip_Mission"
			"disabled_tooltiptext"			"#L4D360UI_GameSettings_Tooltip_Mission_Disabled"
			"style"							"DropDownButton"
			"command"						"FlmMissionExtended"
			"ActivationType"				"1"		[$GAMECONSOLE]
			"EnableCondition"				"Never" [$DEMO]
		}
	}
	
	"FlmMissionExtended"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmMissionExtended"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"4"
		"InitialFocus"			"BtnCampaign1"
		"ResourceFile"			"resource/UI/basemodui/DropDownMissionScavengeExtended.res"
	}

	// Chapter Dropdown
	"DrpChapter"
	{
		"ControlName"			"DropDownMenu"
		"fieldName"				"DrpChapter"
		"xpos"					"c-250"
		"ypos"					"135"			[$WIN32]
		"ypos"					"140"			[$GAMECONSOLE]
		"zpos"					"1"
		"wide"					"280"
		"tall"					"15"			[$WIN32]
		"tall"					"20"			[$GAMECONSOLE]
		"visible"				"1"
		"enabled"				"1"
		"navUp"					"DrpMissionExtended"
		"navDown"				"DrpGameAccess"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"					"BaseModHybridButton"
			"fieldName"						"BtnDropButton"
			"xpos"							"0"
			"ypos"							"0"
			"zpos"							"2"
			"wide"							"280"
			"wideatopen"					"160"
			"tall"							"15"	[$WIN32]
			"tall"							"20"	[$GAMECONSOLE]
			"autoResize"					"1"
			"pinCorner"						"0"
			"visible"						"1"
			"enabled"						"1"
			"tabPosition"					"0"
			"labelText"						"#L4D360UI_GameSettings_Chapter"
			"tooltiptext"					"#L4D360UI_GameSettings_Tooltip_Chapter"
			"disabled_tooltiptext"			"#L4D360UI_GameSettings_Tooltip_Chapter_Disabled"
			"style"							"DropDownButton"
			"command"						""
			"ActivationType"				"1" [$GAMECONSOLE]
		}
	}
	
	//flyouts		
	"FlmChapterXXautogenerated"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmChapterFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"4"
		"InitialFocus"			"BtnChapter1"
		"ResourceFile"			"resource/UI/basemodui/DropDownChapter.res"
	}

	"DrpGameAccess"
	{
		"ControlName"			"DropDownMenu"
		"fieldName"				"DrpGameAccess"
		"xpos"					"c-250"
		"ypos"					"155"			[$WIN32]
		"ypos"					"165"			[$GAMECONSOLE]
		"zpos"					"1"
		"wide"					"280"
		"tall"					"15"			[$WIN32]
		"tall"					"20"			[$GAMECONSOLE]
		"visible"				"1"				[$GAMECONSOLE]
		"visible"				"0"				[$WIN32]
		"enabled"				"1"
		"navUp"					"DrpChapter"
		"navDown"				"DrpServerType"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"2"
			"wide"					"280"
			"wideatopen"			"160"
			"tall"					"15"	[$WIN32]
			"tall"					"20"	[$GAMECONSOLE]
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_Lobby_Change_GameAccess"
			"tooltiptext"			"#L4D360UI_Lobby_Change_GameAccess_Tip"
			"disabled_tooltiptext"	"#L4D360UI_GameSettings_Tooltip_Access_Disabled"
			"style"					"DropDownButton"
			"command"				"FlmGameAccess"
			"ActivationType"		"1" [$GAMECONSOLE]
		}
	}
		
	"FlmGameAccess"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmGameAccess"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnFriends"
		"ResourceFile"			"resource/UI/basemodui/DropDownGameAccessTeam.res"
	}

	"DrpServerType"
	{
		"ControlName"			"DropDownMenu"
		"fieldName"				"DrpServerType"
		"xpos"					"c-250"
		"ypos"					"155"	[$WIN32]
		"ypos"					"190"	[$GAMECONSOLE]
		"zpos"					"1"
		"wide"					"280"
		"tall"					"15"	[$WIN32]
		"tall"					"20"	[$GAMECONSOLE]
		"visible"				"1"		[$WIN32]
		"visible"				"0"		[$GAMECONSOLE]
		"enabled"				"1"
		"navUp"					"DrpGameAccess"
		"navDown"				"BtnStartLobby"
		
		//button and label
		"BtnDropButton"
		{
			"ControlName"			"BaseModHybridButton"
			"fieldName"				"BtnDropButton"
			"xpos"					"0"
			"ypos"					"0"
			"zpos"					"2"
			"wide"					"280"
			"wideatopen"			"160"
			"tall"					"15"	[$WIN32]
			"tall"					"20"	[$GAMECONSOLE]
			"autoResize"			"1"
			"pinCorner"				"0"
			"visible"				"1"
			"enabled"				"1"
			"tabPosition"			"0"
			"labelText"				"#L4D360UI_Lobby_Change_ServerType"
			"tooltiptext"			"#L4D360UI_Lobby_Change_ServerType_Tip"
			"style"					"DropDownButton"
			"command"				"FlmServerType"
		}
	}
		
	"FlmServerType"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmServerType"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnOfficial"
		"ResourceFile"			"resource/UI/basemodui/DropDownServerType.res"
	}
	
    "IconForwardArrow"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"IconForwardArrow"
		"xpos"					"c-265"	[$WIN32]
		"xpos"					"c-275"	[$GAMECONSOLE]
		"ypos"					"175"	[$WIN32]
		"ypos"					"190"	[$GAMECONSOLE]
		"wide"					"15"	[$WIN32]
		"tall"					"15"	[$WIN32]
		"wide"					"20"	[$GAMECONSOLE]
		"tall"					"20"	[$GAMECONSOLE]
		"scaleImage"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"icon_button_arrow_right"
		"scaleImage"			"1"
	}
	"BtnStartLobby"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnStartLobby"
		"xpos"					"c-250"
		"ypos"					"175"	[$WIN32]
		"ypos"					"190"	[$GAMECONSOLE]
		"zpos"					"0"
		"wide"					"220"
		"tall"					"15"	[$WIN32]
		"tall"					"20"	[$GAMECONSOLE]
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"1"
		"wrap"					"1"
		"navUp"					"DrpServerType"
		"navDown"				"DrpMission"	[$GAMECONSOLE]
		"navDown"				"BtnCancel"		[$WIN32]
		"labelText"				"#L4D360UI_GameSettings_Create_Lobby"
		"tooltiptext"			"#L4D360UI_GameSettings_Tooltip_Create_Lobby"
		"style"					"DefaultButton"
		"command"				"StartLobby"
		EnabledTextInsetX		"2"
		DisabledTextInsetX		"2"
		FocusTextInsetX			"2"
		OpenTextInsetX			"2"
	}
	
    "IconBackArrow" [$WIN32]
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"IconBackArrow"
		"xpos"					"c-265"
		"ypos"					"195"
		"wide"					"15"
		"tall"					"15"
		"scaleImage"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"icon_button_arrow_left"
		"scaleImage"			"1"
	}
	"BtnCancel"	[$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnCancel"
		"xpos"					"c-250"
		"ypos"					"195"
		"zpos"					"0"
		"wide"					"220"
		"tall"					"15"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"navUp"					"BtnStartLobby"
		"navDown"				"DrpMission"
		"labelText"				"#L4D360UI_Back_Caps"
		"tooltiptext"			"#L4D360UI_Tooltip_Back"
		"style"					"DefaultButton"
		"command"				"Back"
		EnabledTextInsetX		"2"
		DisabledTextInsetX		"2"
		FocusTextInsetX			"2"
		OpenTextInsetX			"2"
	}
}
