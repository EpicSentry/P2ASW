//
// TRACKER SCHEME RESOURCE FILE
//
// sections:
//		colors			- all the colors used by the scheme
//		basesettings	- contains settings for app to use to draw controls
//		fonts			- list of all the fonts used by app
//		borders			- description of all the borders
//
//
Scheme
{
	//Name - currently overriden in code
	//{
	//	"Name"	"ClientScheme"
	//}

	//////////////////////// COLORS ///////////////////////////
	Colors
	{
		// base colors
		"Orange"			"178 82 22 255"
		"OrangeDim"			"178 82 22 120"
		"LightOrange"		"188 112 0 128"
		"GoalOrange"		"255 133 0"
		
		"White"				"235 235 235 255"
		"Red"				"192 28 0 140"
		"RedSolid"			"192 28 0 255"
		"Blue"				"0 28 162 140"
		"Yellow"			"251 235 202 255"
		"TransparentYellow"	"251 235 202 140"
		//"Black"				"0 0 0 255"
		//Changed black to a NTSC safe color
		
		"Black"				"46 43 42 255"
		"TransparentBlack"	"0 0 0 196"
		"TransparentLightBlack"	"0 0 0 90"
		"FooterBGBlack"		"52 48 55 255"
		
		"HUDBlueTeam"		"104 124 155 127"
		"HUDRedTeam"		"180 92 77 127"
		"HUDSpectator"		"124 124 124 127"
		"HUDBlueTeamSolid"	"104 124 155 255"
		"HUDRedTeamSolid"	"180 92 77 255"
		"HUDDeathWarning"	"255 0 0 255"
		"HudWhite"			"255 255 255 255"
		"HudOffWhite"		"200 187 161 255"
		
		"Gray"				"178 178 178 255"

		"Blank"				"0 0 0 0"
		"ForTesting"		"255 0 0 32"
		"ForTesting_Magenta"	"255 0 255 255"
		"ForTesting_MagentaDim"	"255 0 255 120"

		"HudPanelForeground"		"123 110 59 184"
		"HudPanelBackground"		"123 110 59 184"
		"HudPanelBorder"			"255 255 255 102"

		"HudProgressBarActive"		"240 207 78 255"
		"HudProgressBarInActive"	"140 120 73 255"
		"HudProgressBarActiveLow"	"240 30 30 255"
		"HudProgressBarInActiveLow"	"240 30 30 99"	

		"HudTimerProgressActive"	"251 235 202 255"
		"HudTimerProgressInActive"	"52 48 45 255"
		"HudTimerProgressWarning"	"240 30 30 255"
		
		"TanDark"				"117 107 94 255"
		"TanLight"				"235 226 202 255"
		"TanDarker"				"46 43 42 255"
		
		// Building HUD Specific
		"LowHealthRed"		"255 0 0 255"
		"ProgressOffWhite"	"251 235 202 255"
		"ProgressBackground"	"250 234 201 51"
		"HealthBgGrey"		"72 71 69 255"
		
		"ProgressOffWhiteTransparent"	"251 235 202 128"
		
		"LabelDark"			"48 43 42 255"
		"LabelTransparent"	"109 96 80 180"
		
		"BuildMenuActive"	"248 231 198 255"
		
		"DisguiseMenuIconRed"	"192 56 63 255"
		"DisguiseMenuIconBlue"	"92 128 166 255"

 		"MatchmakingDialogTitleColor"			"200 184 151 255"
 		"MatchmakingMenuItemBackground"			"46 43 42 255"
 		"MatchmakingMenuItemBackgroundActive"	"150 71 0 255"	
		"MatchmakingMenuItemTitleColor"			"200 184 151 255"
		"MatchmakingMenuItemDescriptionColor"	"200 184 151 255"
		
		"HTMLBackground"						"95 92 101 255"
	}

	///////////////////// BASE SETTINGS ////////////////////////
	// default settings for all panels
	// controls use these to determine their settings
	BaseSettings
	{	
		// scheme-specific colors
		"FgColor"		"Orange"
		"BgColor"		"TransparentBlack"
		
		"VguiScreenCursor"			"255 208 64 255"
	}

	//////////////////////// BITMAP FONT FILES /////////////////////////////
	//
	// Bitmap Fonts are ****VERY*** expensive static memory resources so they are purposely sparse
	BitmapFontFiles
	{
		// UI buttons, custom font, (256x64)
		"Buttons"		"materials/vgui/fonts/buttons_32.vbf"
	}


	//
	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		"Default"
		{
			"1"
			{
				"name"		"Calibri"
				"tall"		"12"
				"weight"	"900"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
			}
		}
		
		
		"Thin28"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"28"
				"weight"	"700"
				"antialias"		"1"
			}
		}
		
		"Small16"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"16"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small18"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"18"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small20"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"20"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small24"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"24"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small28"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"28"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small36"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"36"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Small48"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"48"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		"Normal"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"36"
				"weight"	"800"
				"antialias"		"1"
			}
		}
		
		"Medium"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"48"
				"weight"	"800"
				"antialias"		"1"
			}
		}
		
		"Large"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"72"
				"weight"	"800"
				"antialias"		"1"
			}
		}
		
		/////////////////
		// THIN
		"AbsaraSansThin8"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"8"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin9"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"9"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin10"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"10"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin11"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"11"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin12"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"12"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin14"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"14"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin16"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"16"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin18"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"18"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin20"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"20"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin22"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"22"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin24"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"24"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin26"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"26"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin28"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"28"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin36"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"36"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin48"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"48"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansThin72"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Thin"
				"tall"		"72"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		
		//////////////////////
		// REGULAR
		"AbsaraSansReg8"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"8"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg9"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"9"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg10"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"10"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg11"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"11"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg12"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"12"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg14"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"14"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg16"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"16"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg18"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"18"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg20"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"20"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg22"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"22"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg24"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"24"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg26"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"26"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg28"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"28"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg36"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"36"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg48"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"48"
				"weight"	"100"
				"antialias"		"1"
			}
		}
		"AbsaraSansReg72"
		{
			"1"
			{
				"name"		"AbsaraSansOT-Regular"
				"tall"		"72"
				"weight"	"100"
				"antialias"		"1"
			}
		}
	}

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		NoBorder
		{
			"inset" "0 0 0 0"
			Left
			{
				"1"
				{
					"color" "Blank"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "Blank"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Blank"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Blank"
					"offset" "0 0"
				}
			}
		}
	}
}
