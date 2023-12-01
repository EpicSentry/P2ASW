///////////////////////////////////////////////////////////
// Tracker scheme resource file
//
// sections:
//		Colors			- all the colors used by the scheme
//		BaseSettings	- contains settings for app to use to draw controls
//		Fonts			- list of all the fonts used by app
//		Borders			- description of all the borders
//
///////////////////////////////////////////////////////////
Scheme
{
	//////////////////////// COLORS ///////////////////////////
	// color details
	// this is a list of all the colors used by the scheme
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

		"QualityColorNormal"					"178 178 178 255"
		"QualityColorrarity1"					"77 116 85 255"
		"QualityColorrarity2"					"141 131 75 255"
		"QualityColorrarity3"					"207 106 50 255"
		"QualityColorrarity4"					"134 80 172 255"
		"QualityColorVintage"					"71 98 145 255"
		"QualityColorUnique"					"255 215 0 255"
		"QualityColorCommunity"					"112 176 74 255"
		"QualityColorDeveloper"					"165 15 121 255"
		"QualityColorSelfMade"					"112 176 74 255"
		"QualityColorCustomized"				"71 98 145 255"
		"QualityColorStrange"					"205 155 29 255"
	}
	
	///////////////////// BASE SETTINGS ////////////////////////
	//
	// default settings for all panels
	// controls use these to determine their settings
	BaseSettings
	{
		Label.TextDullColor				"TanDark"
		Label.TextColor					"TanLight"
		Label.TextBrightColor			"TanLight"
		Label.SelectedTextColor			"White"
		Label.BgColor					"Blank"
		Label.DisabledFgColor1			"Blank"
		Label.DisabledFgColor2			"LightOrange"
	
		Rosette.DefaultFgColor			"White"
		Rosette.DefaultBgColor			"Blank"
		Rosette.ArmedBgColor			"Blank"
		Rosette.DisabledBgColor			"Blank"
		Rosette.DisabledBorderColor		"Blank"
		Rosette.LineColor				"192 192 192 128"
		Rosette.DrawBorder				"1"
		Rosette.DefaultFont				DefaultSmall
		Rosette.ArmedFont				Default

		Frame.TopBorderImage				"vgui/menu_backgroud_top"
		Frame.BottomBorderImage				"vgui/menu_backgroud_bottom"
		Frame.SmearColor					"0 0 0 225"		[$X360]
		Frame.SmearColor					"0 0 0 180"		[$WIN32]

		"FgColor"			"255 220 0 100"
		"BgColor"			"0 0 0 76"

		"Panel.FgColor"			"255 220 0 100"
		"Panel.BgColor"			"0 0 0 76"
		
		"BrightFg"		"255 220 0 255"

		"DamagedBg"			"180 0 0 200"
		"DamagedFg"			"180 0 0 230"
		"BrightDamagedFg"		"255 0 0 255"

		// checkboxes and radio buttons
		"BaseText"					"OffWhite"
		"BrightControlText"			"White"
		"CheckBgColor"				"TransparentBlack"
		"CheckButtonBorder1" 			"Border.Dark" 		// the left checkbutton border
		"CheckButtonBorder2"  			"Border.Bright"		// the right checkbutton border
		"CheckButtonCheck"				"White"				// color of the check itself
		
		// weapon selection colors
		"SelectionNumberFg"		"255 220 0 255"
		"SelectionTextFg"		"255 220 0 255"
		"SelectionEmptyBoxBg" 	"0 0 0 80"
		"SelectionBoxBg" 		"0 0 0 80"
		"SelectionSelectedBoxBg" "0 0 0 80"
		
		"ZoomReticleColor"	"255 220 0 255"

		// HL1-style HUD colors
		"Yellowish"			"255 160 0 255"
		"Normal"			"255 208 64 255"
		"Caution"			"255 48 0 255"

		// Top-left corner of the "Half-Life 2" on the main screen
		"Main.Title1.X"				"32"
		"Main.Title1.Y"				"280"
		"Main.Title1.Y_hidef"		"130"
		"Main.Title1.Color"	"255 255 255 0"

		// Top-left corner of secondary title e.g. "DEMO" on the main screen
		"Main.Title2.X"				"76"
		"Main.Title2.Y"				"190"
		"Main.Title2.Y_hidef"		"174"
		"Main.Title2.Color"	"255 255 255 0"

		// Top-left corner of the menu on the main screen
		"Main.Menu.X"			"32"
		"Main.Menu.X_hidef"		"76"
		"Main.Menu.Y"			"340"
		"Main.Menu.Color"		"168 97 64 255"
		"Menu.TextColor"		"0 0 0 255"
		"Menu.BgColor"			"125 125 125 255"

		// Blank space to leave beneath the menu on the main screen
		"Main.BottomBorder"	"32"

		ScrollBar.Wide						"12"

		ScrollBarButton.FgColor				"Black"
		ScrollBarButton.BgColor				"Blank"
		ScrollBarButton.ArmedFgColor		"White"
		ScrollBarButton.ArmedBgColor		"Blank"
		ScrollBarButton.DepressedFgColor	"White"
		ScrollBarButton.DepressedBgColor	"Blank"

		ScrollBarSlider.FgColor				"0 0 0 255"			// nob color
		ScrollBarSlider.BgColor				"0 0 0 40"			// slider background color
		ScrollBarSlider.NobFocusColor		"White"
		ScrollBarSlider.NobDragColor		"White"
		ScrollBarSlider.Inset				"3"
	}
	
	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace

		"DebugFixed"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"14"	[!$GAMECONSOLE]
				"tall"		"18"	[$GAMECONSOLE]
				"weight"	"400"
				"antialias" "1"
			}
		}

		"DebugFixedSmall" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"14"
				"weight"	"400"
				"antialias" "1"
			}
		}

		DebugOverlay [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"14"
				"weight"	"400"
				"outline"	"1"
			}
		}

		Default [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"9"
				"weight"	"700"
				"antialias" "1"
				"yres"	"1 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"700"
				"antialias" "1"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"900"
				"antialias" "1"
				"yres"	"768 1023"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"900"
				"antialias" "1"
				"yres"	"1024 1199"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"900"
				"antialias" "1"
				"yres"	"1200 10000"
				"additive"	"1"
			}
		}

		"DefaultSmall" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
			"6"
			{
				"name"		"Arial"
				"tall"		"12"
				"range" 		"0x0000 0x00FF"
				"weight"		"0"
			}
		}

		"DefaultVerySmall" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"	"1200 6000"
				"antialias"	"1"
			}
			"6"
			{
				"name"		"Verdana"
				"tall"		"12"
				"range" 		"0x0000 0x00FF"
				"weight"		"0"
			}
			"7"
			{
				"name"		"Arial"
				"tall"		"11"
				"range" 		"0x0000 0x00FF"
				"weight"		"0"
			}
		}

		WeaponIcons [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"64"
				"tall_hidef"	"58"
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1"
			}
		}

		WeaponIconsSelected [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"64"
				"tall_hidef"	"58"
				"weight"	"0"
				"antialias" "1"
				"blur"		"5"
				"scanlines"	"2"
				"additive"	"1"
				"custom"	"1"
			}
		}

		WeaponIconsSmall [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"32"
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1"
			}
		}

		Crosshairs [!$GAMECONSOLE]
		{
			"1"
			{
				"bitmap"	"1"
				"name"		"Buttons"
				"scalex"	"1"
				"scaley"	"1"
			}
		}

		QuickInfo [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HL2cross"
				"tall"		"28" [!$OSX]
				"tall"		"50" [$OSX]
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1" [!$OSX]
			}
		}

		HudNumbers [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"32"
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1"
			}
		}

		SquadIcon	[!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"50"
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1"
			}
		}

		HudNumbersGlow [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"32"
				"weight"	"0"
				"blur"		"4"
				"scanlines" "2"
				"antialias" "1"
				"additive"	"1"
				"custom"	"1"
			}
		}

		HudNumbersSmall [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"HalfLife2"
				"tall"		"16"
				"weight"	"1000"
				"additive"	"1"
				"antialias" "1"
				"custom"	"1"
			}
		}

		HudSelectionNumbers [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"11"
				"weight"	"700"
				"antialias" "1"
				"additive"	"1"
			}
		}

		HudHintTextLarge [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"1000"
				"antialias" "1"
				"additive"	"1"
			}
		}

		HudHintTextSmall [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"11"
				"weight"	"0"
				"antialias" "1"
				"additive"	"1"
			}
		}

		HudSelectionText [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"8"
				"weight"	"700"
				"antialias" "1"
				"yres"	"1 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"10"
				"weight"	"700"
				"antialias" "1"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"900"
				"antialias" "1"
				"yres"	"768 1023"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"900"
				"antialias" "1"
				"yres"	"1024 1199"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"17"
				"weight"	"1000"
				"antialias" "1"
				"yres"	"1200 10000"
			}
		}

		BudgetLabel [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"14"
				"weight"	"400"
				"outline"	"1"
			}
		}

		// this is the symbol font
		"Marlett" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
			}
		}

		"Trebuchet24" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"24"
				"weight"	"900"
				"range"		"0x0000 0x007F"	//	Basic Latin
				"antialias" "1"
				"additive"	"1"
			}
		}

		"Trebuchet18" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"18"
				"weight"	"900"
			}
		}

		ClientTitleFont [!$GAMECONSOLE]
		{
			"1"
			{
				"name"  "Impact"
				"tall"			"64"
				"tall_hidef"	"72"
				"weight" "0"
				"additive" "0"
				"antialias" "1"
			}
		}

		HDRDemoText [!$GAMECONSOLE]
		{
			// note that this scales with the screen resolution
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"24"
				"weight"	"900"
				"antialias" "1"
				"additive"	"1"
			}
		}
				
		"LargeHUDTitle" [!$GAMECONSOLE]
		{
			"1"
			{
				"name"		"Trade Gothic Bold"
				"tall"		"20"
				"weight"	"400"
				"antialias" "1"
			}
		}

		"ItemFontNameSmallest"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"10"
				"weight"	"500"
				"additive"	"0"
				"antialias" "1"
			}
		}
		"ItemFontNameSmall"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"11"
				"weight"	"500"
				"additive"	"0"
				"antialias" "1"
			}
		}
		"ItemFontNameLarge"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"15"
				"weight"	"400"
				"antialias"	"1"
				"yres"	"501 6000" [$OSX]
			}
		}
		"ItemFontAttribSmallest"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"7"
				"weight"	"500"
				"additive"	"0"
				"antialias" 	"1"
			}
		}
		"ItemFontAttribSmall"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"8"
				"weight"	"500"
				"additive"	"0"
				"antialias" 	"1"
			}
		}
		"ItemFontAttribLarge"
		{
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"11"
				"weight"	"500"
				"additive"	"0"
				"antialias" 	"1"
			}
		}	

		"EconButtonTextLargeTitle"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-BoldCn"
				"tall"		"30"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconButtonTextLarge"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"22"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconButtonText" [$GAMECONSOLE]
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn"
				"tall"		"18"
				"weight"	"400"
				"antialias"	"1"
			}
		}
	
		"EconButtonText" [!$GAMECONSOLE]
		{
			"isproportional"	"only"		
			"1" [$OSX]
			{
				"name"		"UniversLTStd-Cn"
				"tall"		"16"
				"weight"	"400"
				"antialias"	"1"
				"yres"	"1 500"
			}
			"2"
			{
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"name"		"UniversLTStd-Cn" [$OSX]
				"tall"		"15"
				"weight"	"400"
				"antialias"	"1"
				"yres"	"501 6000" [$OSX]
			}
		}

		"EconCartPrice"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"16"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconCartTotal"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"20"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconStorePrice"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"18"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconStorePriceOriginal"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"18"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontStoreTitle"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn"
				"tall"		"34"
				"weight"	"0"
				"antialias"	"1"
			}
		}

		"EconFontMediumBold"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"24"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontMediumSmallBold"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"14"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontSmall"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn"
				"tall"		"14"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontSmallBold"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"14"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontSmallestBold"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn" [$OSX]
				"name"		"UniversLTStd-BoldCn" [!$OSX]
				"tall"		"12"
				"weight"	"400"
				"antialias"	"1"
			}
		}

		"EconFontSmallest"
		{
			"isproportional"	"only"		
			"1"
			{
				"name"		"UniversLTStd-Cn"
				"tall"		"12"
				"weight"	"400"
				"antialias"	"1"
			}
		}

	}

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		LoadoutItemBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_item_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"	
		}
		LoadoutItemMouseOverBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_item_bg_highlight"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"	
		}
		ItemSelectionBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_item_sel_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"	
		}
		ItemPickupBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_item_pickup_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"	
		}
		LoadoutItemPopupBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_preview_bg"
		}
		EconButtonDefault
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"2"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"2"	
		}
		EconButtonMouseOver
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg_over"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		EconFooterButtonBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_bottom_bar_button_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"	
		}
		EconFooterButtonMouseOverBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_bottom_bar_button_highlight_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		EconDialogBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_default_dialog"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		BackpackItemBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
		BackpackItemMouseOverBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg_highlight"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
		BackpackItemSelectedBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg_highlight"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
		GrayDialogBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		OutlinedGreyBox
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		OutlinedDullGreyBox
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		StorePreviewTabSelected
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		StorePreviewTabUnselected
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"8"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"8"
		}
		StoreDiscountBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"			
			"image"					"store/store_discount_corner"
			"src_corner_height"		"32"				// pixels inside the image
			"src_corner_width"		"32"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		StorePreviewBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
		}
		StoreIconPreviewBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_preview_bg"
		}
		StorePreviewTooltip
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_tooltip_bg"
		}
		ItemBaseBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_bg"
			"src_corner_height"		"23"				// pixels inside the image
			"src_corner_width"		"23"
			"draw_corner_width"		"5"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"5"	
		}
		StoreTabActive
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_tab_selected"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		StoreTabNormal
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_tab_unselected"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		StoreBlueButton
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"btn_econ_blue"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}
		StoreBlueOverButton
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/button_econ_blue_over"
			"src_corner_height"		"16"				// pixels inside the image
			"src_corner_width"		"16"
			"draw_corner_width"		"4"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"4"	
		}

		StoreItemBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
		StoreItemMouseOverBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg_highlight"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
		StoreItemSelectedBorder
		{
			"bordertype"			"scalable_image"
			"backgroundtype"		"2"
			
			"image"					"store/store_backpack_bg_highlight"
			"src_corner_height"		"24"				// pixels inside the image
			"src_corner_width"		"24"
			"draw_corner_width"		"11"				// screen size of the corners ( and sides ), proportional
			"draw_corner_height" 	"11"	
		}
	}

	//////////////////////// CUSTOM FONT FILES /////////////////////////////
	//
	// specifies all the custom (non-system) font files that need to be loaded to service the above described fonts
	CustomFontFiles [!$GAMECONSOLE]
	{
		"1"		"resource/HALFLIFE2.vfont"
		"2"		"resource/HL2crosshairs.vfont"
	}

}
