"Resource/UI/InGameMainMenu.res"
{
	"InGameMainMenu"
	{
		"ControlName"			"Frame"
		"fieldName"				"InGameMainMenu"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"7"
		"tall"					"3"
		"?online"
		{
			"tall"				"4"
		}
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
		"navDown"				"BtnGoToHub"
		"labelText"				"#PORTAL2_InGameMainMenu_ReturnToGame"
		"style"					"DefaultButton"
		"command"				"ReturnToGame"
		"ActivationType"		"1"
	}
		
	"BtnGoToHub"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnGoToHub"
		"xpos"					"0"
		"ypos"					"50"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnReturnToGame"
		"navDown"				"BtnOptions"
		"labelText"				"#Portal2UI_GoToHub"
		"style"					"DefaultButton"
		"command"				"GoToHub"
		"ActivationType"		"1"
		"EnableCondition"		"Never" [$DEMO]
	}	

	"BtnOptions"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnOptions"
		"xpos"					"0"
		"ypos"					"75"
		"wide"					"0"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnGoToHub"
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
		"ypos"					"100"
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

	"PnlGamerPicBorder"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"PnlGamerPicBorder"
		"xpos"					"0"
		"ypos"					"150"
		"wide"					"f0"
		"tall"					"50"
		"visible"				"0"
		"?online"
		{
			"visible"			"1"
		}
		"drawcolor_override"	"0 0 0 150"
		"scaleImage"			"1"
		"image"					"white"
	}

	"PnlGamerPic"
	{
		"ControlName"			"ImagePanel"
		"fieldName"				"PnlGamerPic"
		"xpos"					"0"
		"ypos"					"150"
		"wide"					"50"
		"tall"					"50"
		"visible"				"0"
		"?online"
		{
			"visible"			"1"
		}
		"enabled"				"1"
		"tabPosition"			"0"
		"scaleImage"			"1"
		"image"					"icon_lobby"
	}

	"LblGamerTag"
	{
		"ControlName"			"Label"
		"fieldName"				"LblGamerTag"
		"xpos"					"55"
		"ypos"					"157"
		"wide"					"0"
		"tall"					"25"
		"visible"				"0"
		"?online"
		{
			"visible"			"1"
		}
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsList"
		"labelText"				""
		"textAlignment"			"west"
		"fgcolor_override"		"255 255 255 255"
		"noshortcutsyntax"		"1"
	}
	
	"LblGamerTagStatus"
	{
		"ControlName"			"Label"
		"fieldName"				"LblGamerTagStatus"
		"xpos"					"55"
		"ypos"					"177"
		"wide"					"0"
		"tall"					"25"
		"visible"				"0"
		"?online"
		{
			"visible"			"1"
		}
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"FriendsListSmall"
		"labelText"				"#PORTAL2_Coop_YourPartnerInScience"
		"textAlignment"			"west"
		"fgcolor_override"		"255 255 255 255"
	}
}

