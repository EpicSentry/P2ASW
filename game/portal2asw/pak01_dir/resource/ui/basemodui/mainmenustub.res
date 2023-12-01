"Resource/UI/MainMenuStub.res"
{
	"MainMenu"
	{
		"ControlName"			"Frame"
		"fieldName"				"MainMenu"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"f0"
		"tall"					"f0"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"PaintBackgroundType"	"0"
	}
	
	"BtnStub"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnStub"
		"xpos"						"88"	[!$GAMECONSOLE || ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"	[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"ypos"						"340"   
		"wide"						"220"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnQuit"
		"navDown"					"BtnQuit"
		"labelText"					"NO STEAM"
		"style"						"MainMenuButton"
		"command"					"BtnStub"
		"ActivationType"			"1"
	}

	"BtnQuit" [!$GAMECONSOLE]
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"					"BtnQuit"
		"xpos"						"88"	[!$GAMECONSOLE || ($GAMECONSOLEWIDE && !$ANAMORPHIC)]
		"xpos"						"63"	[$GAMECONSOLE && (!$GAMECONSOLEWIDE || $ANAMORPHIC)]	
		"ypos"						"370"
		"wide"						"220"
		"tall"						"20"
		"autoResize"				"1"
		"pinCorner"					"0"
		"visible"					"1"
		"enabled"					"1"
		"tabPosition"				"0"
		"navUp"						"BtnStub"
		"navDown"					"BtnStub"
		"labelText"					"#PORTAL2_MainMenu_Quit"
		"style"						"MainMenuButton"
		"command"					"QuitGame"
		"ActivationType"			"1"
	}
}
