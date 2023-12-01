"Resource/UI/RateMapDialog.res"
{
	"RateMapDialog"
	{
		"ControlName"			"Frame"
		"fieldName"				"RateMapDialog"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"10"
		"tall"					"5"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"dialogstyle"			"1"
	}
		
	"MapImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"MapImage"
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

	"AuthorAvatarImage"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"AuthorAvatarImage"
		"xpos"							"264"	
		"ypos"							"200"
		"zpos"							"2"
		"wide"							"42"
		"tall"							"42"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
	}

	"RatingsItem"
	{
		"ControlName"				"IconRatingItem"
		"fieldName"					"RatingsItem"
		"xpos"							"430"
		"ypos"							"220"
		"zpos"							"999"
		"wide"							"60"
		"tall"							"32"
		"visible"						"1"
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
		"xpos"							"288"	
		"ypos"							"208"
		"zpos"							"2"
		"wide"							"200"
		"tall"							"32"
		"autoResize"				"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"			"east"
		"labelText"					""
	}

	"BtnVoteUp"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"BtnVoteUp"
		"visible"						"1"
		"scaleImage"					"1"
		"image"							"thumbs_up_default"
		"xpos"							"25"
		"ypos"							"10"
		"wide"							"90"
		"tall"							"90"
		"zpos"							"998"
		"enabled"						"0"
	}

	"BtnVoteDown"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"BtnVoteDown"
		"visible"						"1"
		"scaleImage"					"1"
		"image"							"thumbs_down_default"
		"xpos"							"138"
		"ypos"							"10"
		"wide"							"90"
		"tall"							"90"
		"zpos"							"998"
		"enabled"						"0"
	}

	"BtnFollowAuthor"
	{
		"ControlName"				"CDialogListButton"
		"fieldName"					"BtnFollowAuthor"
		"xpos"						"0"
		"ypos"						"200"
		"wide"						"232"
		"tall"						"20"
		"autoResize"				"0"
		"pinCorner"					"0"
		"visible"					"0"
		"enabled"					"0"
		"tabPosition"				"0"
		"navUp"						""
		"navDown"					""
		"labelText"					""
		"command"					"Follow"
		"style"					"AltButton"
		"ActivationType"			"1"
		"list"
		{
			"#PORTAL2_CommunityPuzzle_FollowingAuthor"		"Follow"
			"#PORTAL2_CommunityPuzzle_NotFollowingAuthor"	"StopFollowing"
		}
	}

	"MapTitleLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"MapTitleLabel"
		"xpos"							"264"
		"ypos"							"135"
		"zpos"							"999"
		"wide"							"175"
		"tall"							"50"
		"autoResize"					"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"			"northwest"
		"wrap"							"1"
		"font"							"FriendsListSmall"
		"labelText"						"#PORTAL2_CommunityPuzzle_Loading"
		"fgcolor_override"	"255 255 255 255" // FIXME: Remove this
	}			
	
	
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
	
	
	
	
	"MapAuthorLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"MapAuthorLabel"
		"font"							"FriendsListStatusLine"
		"xpos"							"264"
		"ypos"							"175"
		"zpos"							"2"
		"wide"							"225"
		"tall"							"20"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"				"0"
		"fgcolor_override"	"255 255 255 255" // FIXME: Remove this
	}			
	
	"MapInfoLabel"
	{
		"ControlName"					"Label"
		"fieldName"						"MapInfoLabel"
		"font"							"NewGameChapter"
		"xpos"							"284"
		"ypos"							"50"
		"zpos"							"999"
		"wide"							"200"
		"tall"							"30"
		"autoResize"					"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"					"west"
		"labelText"						""
		"fgcolor_override"	"255 255 255 255" // FIXME: Remove this
	}			

	"RatingExplanationLabel1"
	{
		"ControlName"					"Label"
		"fieldName"						"RatingExplanationLabel1"
		"font"							"FriendsListStatusLine"
		"xpos"							"24"
		"ypos"							"111"
		"zpos"							"999"
		"wide"							"210"
		"tall"							"50"
		"autoResize"					"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"			"northwest"
		"wrap"							"1"
		"labelText"						"#PORTAL2_CommunityPuzzle_RatingDescLine1"
		"fgcolor_override"	"255 255 255 255" // FIXME: Remove this
	}			
	
	"RatingExplanationLabel2"
	{
		"ControlName"					"Label"
		"fieldName"						"RatingExplanationLabel2"
		"font"							"FriendsListStatusLine"
		"xpos"							"24"
		"ypos"							"163"
		"zpos"							"999"
		"wide"							"210"
		"tall"							"50"
		"autoResize"					"0"
		"visible"						"1"
		"enabled"						"1"
		"textAlignment"			"northwest"
		"wrap"							"1"
		"labelText"						"#PORTAL2_CommunityPuzzle_RatingDescLine2"
		"fgcolor_override"	"255 255 255 255" // FIXME: Remove this
	}			
	
	"ThumbsUpSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ThumbsUpSpinner"
		"xpos"							"38"
		"ypos"							"25"
		"zpos"							"999"
		"wide"							"64"
		"tall"							"64"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}
	
	"ThumbsDownSpinner"
	{
		"ControlName"					"ImagePanel"
		"fieldName"						"ThumbsDownSpinner"
		"xpos"							"150"
		"ypos"							"25"
		"zpos"							"999"
		"wide"							"64"
		"tall"							"64"
		"visible"						"1"
		"enabled"						"1"
		"tabPosition"					"0"
		"scaleImage"					"1"
		"image"							"spinner"
	}
}