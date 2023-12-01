"Resource/UI/StartCoopGame.res"
{
	"StartCoopGame"
	{
		"ControlName"					"Frame"
		"fieldName"						"StartCoopGame"
		"xpos"							"0"
		"ypos"							"0"
		"wide"							"4"
		"tall"							"2"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"dialogstyle"					"1"
	}
		
	"BtnPlayOnline"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnPlayOnline"
		"command"				"BtnPlayOnline"
		"xpos"					"0"
		"ypos"					"25"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnPlaySplitscreen"
		"navDown"				"BtnPlaySplitscreen"
		"labelText"				"#Portal2UI_PlayOnline"
		"style"					"DefaultButton"
	}
	
	"BtnPlaySplitscreen"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnPlaySplitscreen"
		"command"				"BtnPlaySplitscreen"
		"xpos"					"0"
		"ypos"					"50"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"	
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnPlayOnline"
		"navDown"				"BtnPlayOnline"
		"labelText"				"#Portal2UI_PlaySplitscreen"
		"style"					"DefaultButton"
	}
}