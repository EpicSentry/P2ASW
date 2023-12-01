"Resource/UI/InGameMainMenu.res"
{
	"InGameMainMenu"
	{
		"ControlName"			"Frame"
		"fieldName"				"InGameMainMenu"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"5"
		"tall"					"4"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"PaintBackgroundType"	"0"
		"dialogstyle"			"1"
	}
				
	"BtnReturnToGame"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnReturnToGame"
		"xpos"					"0"
		"ypos"					"25"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"1"
		"navUp"					"BtnExitToMainMenu"
		"navDown"				"BtnSaveGame"
		"labelText"				"#PORTAL2_InGameMainMenu_ReturnToGame"
		"style"					"DefaultButton"
		"command"				"ReturnToGame"
		"ActivationType"		"1"
	}
	
	"BtnSaveGame"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnSaveGame"
		"xpos"					"0"
		"ypos"					"50"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnReturnToGame"
		"navDown"				"BtnLoadLastSave"
		"labelText"				"#PORTAL2_SaveGame"
		"style"					"DefaultButton"
		"command"				"OpenSaveGameDialog"
		"ActivationType"		"1"
		"EnableCondition"		"Never" [$DEMO]
	}	
			
	"BtnLoadLastSave"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnLoadLastSave"
		"xpos"					"0"
		"ypos"					"75"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnSaveGame"
		"navDown"				"BtnOptions"
		"labelText"				"#PORTAL2_InGameMainMenu_LoadLastSave"	[$GAMECONSOLE]
		"labelText"				"#PORTAL2_LoadGame"						[!$GAMECONSOLE]
		"style"					"DefaultButton"
		"command"				"LoadLastSave"							[$GAMECONSOLE]
		"command"				"OpenLoadGameDialog"					[!$GAMECONSOLE]
		"ActivationType"		"1"
		"EnableCondition"		"Never" [$DEMO]
	}

	"BtnOptions"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnOptions"
		"xpos"					"0"
		"ypos"					"100"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnLoadLastSave"
		"navDown"				"BtnExitToMainMenu"
		"labelText"				"#PORTAL2_MainMenu_Options"
		"style"					"DefaultButton"
		"command"				"Options"
		"ActivationType"		"1"
	}

	"BtnExitToMainMenu"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnExitToMainMenu"
		"xpos"					"0"
		"ypos"					"125"
		"wide"					"0"	
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnOptions"
		"navDown"				"BtnReturnToGame"
		"labelText"				"#PORTAL2_InGameMainMenu_ExitToMainMenu"
		"style"					"DefaultButton"
		"command"				"ExitToMainMenu"
		"ActivationType"		"1"
	}
}
