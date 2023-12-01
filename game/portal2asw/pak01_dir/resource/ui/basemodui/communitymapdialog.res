"Resource/UI/CommunityMapDialog.res"
{
	"CommunityMapDialog"
	{
		"ControlName"			"Frame"
		"fieldName"				"CommunityMapDialog"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"10"
		"tall"					"5"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
		
	"BtnQuickPlay"
	{
		"ControlName"			"HybridBitmapButton"
		"fieldName"				"BtnQuickPlay"
		"xpos"					"10"
		"ypos"					"8"
		"wide"					"120" [$GERMAN]
		"wide"					"100"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"5"
		"labelText"				"#GAMEUI_QuickPlay"
		"style"						"BitmapButton"
		"command"					"QuickPlay"
		"ActivationType"			"0"
		"bitmap_enabled"			"vgui/quickplay_icon"
		"bitmap_focus"				"vgui/quickplay_icon"
		"default"				"0"
	}

	"ListBtnQueueType"
	{
		"ControlName"			"CDialogListButton"
		"fieldName"				"ListBtnQueueType"
		"xpos"					"0"
		"ypos"					"38"
		"zpos"					"1"
		"wide"					"250"
		"tall"					"20"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"font"					"NewGameChapterName"
		"style"					"AltButton"
		
		"list"
		{
			"#PORTAL2_CommunityPuzzle_Queue"             "CommunityMaps_Queue"
			"#PORTAL2_CommunityPuzzle_History"           "CommunityMaps_History"
		}
	}

	"CommunityMapList"
	{
		"ControlName"					"GenericPanelList"
		"fieldName"						"CommunityMapList"
		"xpos"							"0"
		"ypos"							"62"
		"zpos"							"1"
		"wide"							"250"
		"tall"							"172"	[$GAMECONSOLE]
		"tall"							"172"	[!$GAMECONSOLE]
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"NoWrap"						"1"
		"panelBorder"					"0"
	}

	"ThumbnailSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ThumbnailSpinner"
		"xpos"							"326"
		"ypos"							"25"
		"zpos"							"999"
		"wide"							"100"
		"tall"							"100"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}	

	"DownloadingSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"DownloadingSpinner"
		"xpos"							"326"
		"ypos"							"27"
		"zpos"							"999"
		"wide"							"33"
		"tall"							"33"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}	

	"CommunityMapImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"CommunityMapImage"
		"xpos"							"264"
		"ypos"							"10"
		"zpos"							"999"
		"wide"							"225"
		"tall"							"126"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
	}
	
	"NoMapsLabel1"
	{
		"ControlName"					"Label"
		"fieldName"						"NoMapsLabel1"
		"xpos"							"60"	
		"ypos"							"55"
		"zpos"							"2"
		"wide"							"375"
		"tall"							"150"
		"autoResize"					"0"
		"visible"						"0"
		"enabled"						"1"
		"textAlignment"					"northwest"
		"labelText"						"#PORTAL2_CommunityPuzzle_NoMaps1"
		"font"							"ConfirmationText"
		"wrap"							"1"
		"fgcolor_override"				"201 211 207 255"
	}
	
	"CommunityMapInfo"
	{
		"ControlName"					"CommunityMapInfoLabel"
		"fieldName"						"CommunityMapInfo"
		"xpos"							"264"
		"ypos"							"142"
		"zpos"							"2"
		"wide"							"150"
		"tall"							"20"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
	}			

	"VoteSpinner"
	{
		"ControlName"				"ImagePanel"
		"fieldName"					"VoteSpinner"
		"xpos"							"270"
		"ypos"							"158"
		"zpos"							"999"
		"wide"							"42"
		"tall"							"42"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"				"0"
		"scaleImage"				"1"
		"image"							"spinner"
	}	

	"QueueSpinner"
	{
		"ControlName"				"ImagePanel"
		"fieldName"					"QueueSpinner"
		// "xpos"							"80"
		// "ypos"							"45"
		"xpos"							"200"
		"ypos"							"80"
		"zpos"							"999"
		"wide"							"100"
		"tall"							"100"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"				"0"
		"scaleImage"				"1"
		"image"							"spinner"
	}	

	"RatingsItem"
	{
		"ControlName"				"IconRatingItem"
		"fieldName"					"RatingsItem"
		"xpos"							"262"
		"ypos"							"172"
		"zpos"							"999"
		"wide"							"60"
		"tall"							"32"
		"visible"						"0"
		"enabled"						"1"
		"tabPosition"				"0"
		"on_image"					"rating_on"
		"off_image"					"rating_off"
		"half_image"				"rating_half"
	}	

	"TotalVotesLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"TotalVotesLabel"
		"font"							"FriendsListStatusLine"
		"fgcolor_override"		"178 194 189 255"
		"xpos"							"264"	
		"ypos"							"158"
		"zpos"							"2"
		"wide"							"200"
		"tall"							"32"
		"autoResize"				"0"
		"visible"						"0"
		"enabled"						"1"
		"textAlignment"			"west"
		"labelText"					""
	}

	"TotalTestChambersLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"TotalTestChambersLabel"
		"font"							"FriendsListStatusLine"
		"xpos"							"188"	
		"ypos"							"158"
		"zpos"							"2"
		"wide"							"200"
		"tall"							"32"
		"autoResize"				"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"			"east"
		"labelText"					""
		"fgcolor_override"	"255 255 255 255"
	}
	
	"NoBaselineLabel1"
	{
		"ControlName"					"Label"
		"fieldName"						"NoBaselineLabel1"
		"font"							"ConfirmationText"
		"xpos"							"60"	
		"ypos"							"85"
		"zpos"							"2"
		"wide"							"375"
		"tall"							"50"
		"autoResize"					"0"
		"visible"						"0"
		"enabled"						"1"
		"textAlignment"					"northwest"
		"wrap"							"1"
		"labelText"						"#PORTAL2_CommunityPuzzle_NoBaseline1"
		"fgcolor_override"				"201 211 207 255"
	}
	
	"NoBaselineLabel2"
	{
		"ControlName"					"Label"
		"fieldName"						"NoBaselineLabel2"
		"font"							"ConfirmationText"
		"xpos"							"60"	
		"ypos"							"135"
		"zpos"							"2"
		"wide"							"375"
		"tall"							"50"
		"autoResize"					"0"
		"visible"						"0"
		"enabled"						"1"
		"textAlignment"					"northwest"
		"wrap"							"1"
		"labelText"						"#PORTAL2_CommunityPuzzle_NoBaseline2"
		"fgcolor_override"				"201 211 207 255"
	}
	
	//
	// Employee badge
	//
	
	"ImgEmployeeBadge"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgEmployeeBadge"
		"xpos"							"412"
		"ypos"							"92"
		"zpos"							"900"
		"wide"							"136"
		"tall"							"136"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"badgeBg"
	}

	"ImgBadgeOverlay"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgBadgeOverlay"
		"xpos"							"412"
		"ypos"							"92"
		"zpos"							"999"
		"wide"							"136"
		"tall"							"136"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"badgeOverlay"
	}

	"AuthorAvatarImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"AuthorAvatarImage"
		"xpos"							"424"	
		"ypos"							"110"
		"zpos"							"901"
		"wide"							"48"
		"tall"							"48"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
	}
	
	"ImgBadgeLogo"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ImgBadgeLogo"
		"xpos"							"409"
		"ypos"							"171"
		"zpos"							"910"
		"wide"							"64"
		"tall"							"16"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"logo_aperture_badge"
	}
	
	"AvatarSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"AvatarSpinner"
		"xpos"							"426"	
		"ypos"							"110"
		"zpos"							"999"
		"wide"							"48"
		"tall"							"48"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"				"0"
		"scaleImage"				"1"
		"image"							"spinner"
	}

}