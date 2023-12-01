"Resource/UI/SliderControl.res"
{
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
		"usetitlesafe"			"0"
		"style"					"DefaultButton"
		"command"				"FlmTestFlyout"
		"ActivationType"		"1"	
	}
	
	"LblSliderText"
	{
		"ControlName"					"Label"
		"fieldName"						"LblSliderText"
		"xpos"							"0"
		"ypos"							"0"
		"wide"							"200"
		"tall"							"20"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"usetitlesafe"					"0"
		"labelText"						""
	}
	
	"PrgValue"
	{
		"ControlName"					"ContinuousProgressBar"
		"fieldName"						"PrgValue"
		"xpos"							"0"
		"ypos"							"0"
		"zpos"							"5"
		"wide"							"130"
		"tall"							"6"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
	}
	
	"PnlDefaultMark"	[$GAMECONSOLE]
	{
		"ControlName"					"Panel"
		"fieldName"						"PnlDefaultMark"
		"xpos"							"0"
		"ypos"							"0"
		"zpos"							"4"
		"wide"							"4"
		"tall"							"13"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
	}

	"PnlDefaultMark"	[!$GAMECONSOLE]
	{
		"ControlName"					"Label"
		"fieldName"						"PnlDefaultMark"
		"xpos"							"0"
		"ypos"							"0"
		"zpos"							"4"
		"wide"							"10" [!$OSX]
		"wide"							"14" [$OSX]
		"tall"							"30"
		"autoResize"					"0"
		"pinCorner"						"0"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"LabelText"						"u"
		"Font"							"Marlett"
		"textAlignment"					"north"
	}
}