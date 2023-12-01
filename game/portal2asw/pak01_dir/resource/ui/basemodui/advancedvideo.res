"Resource/UI/AdvancedVideo.res"
{
	"AdvancedVideo"
	{
		"ControlName"		"Frame"
		"fieldName"			"Video"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"11" [$WIN32WIDE]
		"wide"				"10" [!$WIN32WIDE]
		"tall"				"5"
		"autoResize"		"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"dialogstyle"		"1"
		"IgnoreButtonA"		"1"
	}

	"DrpAntialias"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpAntialias"
		"xpos"				"0"
		"ypos"				"25"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpPagedPoolMem"
		"navDown"			"DrpFiltering"
		"labelText"			"#L4D360UI_VideoOptions_Antialiasing"
		"style"				"DialogListButton"
		"list"
		{
			"_antialias0"	"_antialias0"
			"_antialias1"	"_antialias1"
			"_antialias2"	"_antialias2"
			"_antialias3"	"_antialias3"
			"_antialias4"	"_antialias4"
			"_antialias5"	"_antialias5"
			"_antialias6"	"_antialias6"
			"_antialias7"	"_antialias7"
			"_antialias8"	"_antialias8"
			"_antialias9"	"_antialias9"
		}
	}
	
	"DrpFiltering"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpFiltering"
		"xpos"				"0"
		"ypos"				"50"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpAntialias"
		"navDown"			"DrpVSync"
		"labelText"			"#GameUI_Filtering_Mode"
		"style"				"DialogListButton"
		"list"
		{
			"#GameUI_Bilinear"			"#GameUI_Bilinear"
			"#GameUI_Trilinear"			"#GameUI_Trilinear"
			"#GameUI_Anisotropic2X"		"#GameUI_Anisotropic2X"
			"#GameUI_Anisotropic4X"		"#GameUI_Anisotropic4X"
			"#GameUI_Anisotropic8X"		"#GameUI_Anisotropic8X"
			"#GameUI_Anisotropic16X"	"#GameUI_Anisotropic16X"
		}
				
	}
	
	"DrpVSync"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpVSync"
		"xpos"				"0"
		"ypos"				"75"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpFiltering"
		"navDown"			"DrpQueuedMode"
		"labelText"			"#GameUI_Wait_For_VSync"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"							"VSyncDisabled"
			"#L4D360UI_VideoOptions_VSync_DoubleBuffered"	"VSyncEnabled"
			"#L4D360UI_VideoOptions_VSync_TripleBuffered"	"VSyncTripleBuffered"
		}
	}
	
	"DrpQueuedMode"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpQueuedMode"
		"xpos"				"0"
		"ypos"				"100"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpVSync"
		"navDown"			"DrpShaderDetail"
		"labelText"			"#L4D360UI_VideoOptions_Queued_Mode"
		"style"				"DialogListButton"
		"list"
		{
			"#L4D360UI_Disabled"	"QueuedModeDisabled"
			"#L4D360UI_Enabled"		"QueuedModeEnabled"
		}
	}
	
	"DrpShaderDetail"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpShaderDetail"
		"xpos"				"0"
		"ypos"				"125"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpQueuedMode"
		"navDown"			"DrpCPUDetail"
		"labelText"			"#GameUI_Shader_Detail"
		"style"				"DialogListButton"
		"list"	
		{
			"#GameUI_Low"		"ShaderDetailLow"
			"#GameUI_Medium"	"ShaderDetailMedium"
			"#GameUI_High"		"ShaderDetailHigh"		
			"#GameUI_Ultra"		"ShaderDetailVeryHigh"
		}
	}
	
	"DrpCPUDetail"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpCPUDetail"
		"xpos"				"0"
		"ypos"				"150"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpShaderDetail"
		"navDown"			"DrpModelDetail"
		"labelText"			"#L4D360UI_VideoOptions_CPU_Detail"
		"style"				"DialogListButton"
		"list"	
		{
			"#GameUI_Low"		"CPUDetailLow"
			"#GameUI_Medium"	"CPUDetailMedium"
			"#GameUI_High"		"CPUDetailHigh"
		}
	}
	
	"DrpModelDetail"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpModelDetail"
		"xpos"				"0"
		"ypos"				"175"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpCPUDetail"
		"navDown"			"DrpPagedPoolMem"
		"labelText"			"#L4D360UI_VideoOptions_Model_Texture_Detail"
		"style"				"DialogListButton"
		"list"
		{
			"#GameUI_Low"		"ModelDetailLow"
			"#GameUI_Medium"	"ModelDetailMedium"
			"#GameUI_High"		"ModelDetailHigh"
		}		
	}
	
	"DrpPagedPoolMem"
	{
		"ControlName"		"BaseModHybridButton"
		"fieldName"			"DrpPagedPoolMem"
		"xpos"				"0"
		"ypos"				"200"
		"wide"				"300" [!$WIN32WIDE]
		"wide"				"350" [$WIN32WIDE]
		"tall"				"20"
		"visible"			"1"		[!$OSX]
		"enabled"			"1"		[!$OSX]
		"visible"			"0"		[$OSX]
		"enabled"			"0"		[$OSX]
		"IgnoreButtonA"		"1"
		"tabPosition"		"0"
		"navUp"				"DrpModelDetail"
		"navDown"			"DrpAntialias"
		"labelText"			"#L4D360UI_VideoOptions_Paged_Pool_Mem"
		"style"				"DialogListButton"
		"list"
		{
			"#GameUI_Low"		"PagedPoolMemLow"
			"#GameUI_Medium"	"PagedPoolMemMedium"
			"#GameUI_High"		"PagedPoolMemHigh"
		}	
	}

	"LblDescriptionTitle"
	{
		"ControlName"			"Label"
		"fieldName"				"LblDescriptionTitle"
		"xpos"					"310" [!$WIN32WIDE]
		"xpos"					"360" [$WIN32WIDE]
		"ypos"					"25"
		"wide"					"180"
		"tall"					"25"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"VideoSettingsDescriptionTitle"
		"labelText"				"#L4D360UI_VideoOptions_Antialiasing"
		"textAlignment"			"west"
		"fgcolor_override"		"0 0 0 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}

	"LblDescription"
	{
		"ControlName"			"Label"
		"fieldName"				"LblDescription"
		"xpos"					"310" [!$WIN32WIDE]
		"xpos"					"360" [$WIN32WIDE]
		"ypos"					"60"
		"wide"					"180"
		"tall"					"180"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"VideoSettingsDescription"
		"labelText"				"#PORTAL2_VideoOptions_Antialiasing_Info"
		"textAlignment"			"north-west"
		"wrap"					"1"
		"fgcolor_override"		"0 0 0 255"
		"bgcolor_override"		"0 0 0 0"
		"noshortcutsyntax"		"1"
	}
}
