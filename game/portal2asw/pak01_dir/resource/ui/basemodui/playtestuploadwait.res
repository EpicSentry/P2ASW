"Resource/UI/playtestuploadwait.res"
{
	"PlaytestUploadWait"
	{
		"ControlName"		"Frame"
		"fieldName"			"PlaytestUploadWait"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"7"
		"tall"				"2"
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
	}

	"WaitTextLabel"
	{
		"ControlName"		"Label"
		"fieldName"			"WaitTextLabel"
		"xpos"				"120"
		"ypos"				"25"
		"wide"				"200"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"#PORTAL2_CommunityPuzzle_WaitForPlaytestUpload"
		"textAlignment"		"Left"
		//"Font"				"ControllerLayout" [$GAMECONSOLE && ($JAPANESE || $KOREAN || $SCHINESE || $TCHINESE)]
		"Font"				"DialogMenuItem"//"GamerTag"
	}

	"WorkingAnim"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"WorkingAnim"
		"xpos"							"10"
		"ypos"							"0"
		"zpos"							"999"
		"wide"							"100"
		"tall"							"100"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}
}
