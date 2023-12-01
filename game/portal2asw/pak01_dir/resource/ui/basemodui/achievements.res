"Resource/UI/Achievements.res"
{
	"Achievements"
	{
		"ControlName"	"Frame"
		"fieldName"		"Achievements"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"f0"
		"tall"			"460"	[$WIN32]
		"tall"			"380"	[$GAMECONSOLE]
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"usetitlesafe"	"1"
	}
	
	"ProTotalProgress" 
	{
		"ControlName"			"ContinuousProgressBar"
		"fieldName"				"ProTotalProgress"
		"xpos"					"c-212"	[$GAMECONSOLE]
		"ypos"					"130"	[$GAMECONSOLE]
		"wide"					"420"	[$GAMECONSOLE]
		"xpos"					"c-180"	[$WIN32]
		"ypos"					"110"	[$WIN32]
		"wide"					"390"	[$WIN32]
		"zpos"					"1"
		"tall"					"9"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"		[$WIN32]
		"visible"				"0"		[$GAMECONSOLE]
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
	}
	
	"LblComplete" 
	{
		"ControlName"			"Label"
		"fieldName"				"LblComplete"
		"xpos"					"c-212"	[$GAMECONSOLE]
		"ypos"					"110"	[$GAMECONSOLE]
		"xpos"					"c-180"	[$WIN32]
		"ypos"					"115"	[$WIN32]
		"wide"					"150"
		"zpos"					"1"
		"tall"					"24" [$WIN32]
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"0"		[$GAMECONSOLE]
		"visible"				"1"		[$WIN32]
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
		"textAlignment"			"west"
	}

	"LblGamerscore" 
	{
		"ControlName"			"Label"
		"fieldName"				"LblGamerscore"
		"xpos"					"c-0"
		"ypos"					"100"	[$WIN32]
		"ypos"					"48"	[$GAMECONSOLE]
		"wide"					"172"
		"zpos"					"1"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1" [$GAMECONSOLE]
		"visible"				"0" [$WIN32]
		"enabled"				"1"
		"tabPosition"			"0"
		"proportionalToParent"	"1"
		"textAlignment"			"east"
		"Font"					"DefaultLarge"
	}
	
	"Divider1" [$WIN32]
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Divider1"
		"xpos"					"c-238"	
		"ypos"					"140"	
		"zpos"					"2"
		"wide"					"450"
		"tall"					"2"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"divider_gradient"
		"scaleImage"			"1"
	}

	"GplAwards"	[$GAMECONSOLE]
	{
		"ControlName"			"GenericPanelList"
		"fieldName"				"GplAwards"
		"xpos"					"c-226"
		"ypos"					"110"
		"wide"					"450"	
		"tall"					"255"
		"zpos"					"1"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"1"
		"proportionalToParent"	"1"
	}
	
	"GplAchievements"
	{
		"ControlName"			"GenericPanelList"
		"fieldName"				"GplAchievements"
		"xpos"					"c-226"
		"ypos"					"110"	[$GAMECONSOLE]
		"ypos"					"140"	[$WIN32]
		"wide"					"450"	
		"tall"					"270"	[$WIN32]	
		"tall"					"255"	[$GAMECONSOLE]	
		"zpos"					"1"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"1"
		"proportionalToParent"	"1"
	}
	
	"Divider2" [$WIN32]
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"Divider2"
		"xpos"					"c-238"	
		"ypos"					"408"	
		"zpos"					"2"
		"wide"					"450"
		"tall"					"2"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"image"					"divider_gradient"
		"scaleImage"			"1"
	}

	"PnlLowerGarnish" [$WIN32]
	{
		"ControlName"		"Panel"
		"fieldName"		"PnlLowerGarnish"
		"xpos"			"0"
		"ypos"			"r45"
		"zpos"			"-1"
		"wide"			"f0"
		"tall"			"45"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"tabPosition"		"0"
		"proportionalToParent"	"1"
	}
    "IconBackArrow" [$WIN32]
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"IconBackArrow"
		"xpos"					"c-195"
		"ypos"					"420"
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
	"BtnCancel" [$WIN32]
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnCancel"
		"xpos"					"c-180"
		"ypos"					"420"
		"zpos"					"1"
		"wide"					"250"
		"tall"					"15"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"wrap"					"1"
		"labelText"				"#L4D360UI_Done"
		"tooltiptext"			"#L4D360UI_Tooltip_Back"
		"style"					"MainMenuSmallButton"
		"command"				"Back"
		"proportionalToParent"	"1"
		"usetitlesafe" 			"0"
		EnabledTextInsetX		"2"
		DisabledTextInsetX		"2"
		FocusTextInsetX			"2"
		OpenTextInsetX			"2"
		"allcaps"				"1"
	}
}
