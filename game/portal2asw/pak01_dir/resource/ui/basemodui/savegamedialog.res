"resource/UI/basemodui/savegamedialog.res"
{
	"SaveGameDialog"
	{
		"ControlName"			"SaveGameDialog"
		"fieldName"				"SaveGameDialog"
		"xpos"					"c-220"
		"ypos"					"c-110"
		"wide"					"440"
		"tall"					"420"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"settitlebarvisible"	"1"
		"title"					"#GameUI_SaveGame"
		
		"borderimage"			"vgui/menu_mode_border"

		"leftarrow"				"vgui/arrow_left"	
		"rightarrow"			"vgui/arrow_right"	
		"arrowwidth"			21
		"arrowheight"			21

		// main pic, centered
		"picwidth"				153
		"picheight"				153

		// menu text, underneath pic
		"menutitlex"			100
		"menutitley"			16
		
		// the 360 doesn't clip text, these can be more exact to the fat main menu button
		"menutitlewide"			180	[$GAMECONSOLE && (!$GERMAN && !$FRENCH && !$POLISH)]
		"menutitlewide"			230	[$GAMECONSOLE && ($GERMAN || $FRENCH)]
		"menutitlewide"			240	[$GAMECONSOLE && $POLISH]
		
		// the pc clips text, these need to be wider for the hint, but then use wideatopen to foreshorten the flyout
		"menutitlewide"			500	[$WIN32]
		"wideatopen"			180	[$WIN32 && (!$GERMAN && !$FRENCH && !$POLISH)]
		"wideatopen"			230	[$WIN32 && ($GERMAN || $FRENCH)]
		"wideatopen"			240	[$WIN32 && $POLISH]
		"menutitletall"			80

		// small pics, vertically centered, to the right of the main pic
		"subpicgap"				6		// between pics
		"subpicoffsetx"			20		[$GAMECONSOLE]
		"subpicoffsetx"			50		[$WIN32]
		"subpicoffsety"			-10
		"subpicwidth"			86	
		"subpicheight"			86
		"subpicnamefont"		"Default"
		"hidelabels"			"1"		[!$ENGLISH]			
	}
	"BtnSave"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"BtnSave"
		"xpos"				"302"
		"ypos"				"200"
		"wide"				"82"
		"tall"				"24"
		"autoResize"		"0"
		"visible"			"1"			[!$GAMECONSOLE]
		"visible"			"0"			[$GAMECONSOLE]
		"enabled"			"0"
		"tabPosition"		"1"
		"labelText"			"#GameUI_Save"
		"textAlignment"		"west"
		"dulltext"			"0"
		"brighttext"		"0"
		"wrap"				"0"
		"style"				"DialogButton"
		"command"			"Save"
		"Default"			"1"
	}
	"BtnCancel"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"BtnCancel"
		"xpos"				"394"
		"ypos"				"200"
		"wide"				"70"
		"tall"				"24"
		"autoResize"		"0"
		//"pinCorner"			"3"
		"visible"			"1"			[!$GAMECONSOLE]
		"visible"			"0"			[$GAMECONSOLE]
		"enabled"			"1"
		"tabPosition"		"2"
		"labelText"			"#GameUI_Cancel"
		"textAlignment"		"west"
		"dulltext"			"0"
		"brighttext"		"0"
		"wrap"				"0"
		"style"				"DialogButton"
		"command"			"Back"
		"Default"			"0"
	}
	"LblHelpText"
	{
		"ControlName"		"Label"
		"fieldName"			"LblHelpText"
		"xpos"				"20"
		"ypos"				"26"
		"wide"				"444"
		"tall"				"36"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"labelText"			"#GameUI_SaveGameHelp"
		"textAlignment"		"west"
		"dulltext"			"0"
		"brighttext"		"0"
		"wrap"				"1"
	}
}