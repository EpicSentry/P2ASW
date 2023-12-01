"GameMenu"
{
    "1"
	{
		"label" "#GameUI_GameMenu_ResumeGame"
		"command" "ResumeGame"
		"OnlyInGame" "1"
	}
	"2"
	{
		"label" "IAF TRAINING"
		"command" "engine disconnect\nwait\nwait\nwait\nmaxplayers 1\nskill 1\nasw_started_tutorial 1\nasw_tutorial_save_stage 0\nprogress_enable\nmap tutorial\n"
	}
	"3"
	{
		"label" "START NEW MISSION"
		"command" "engine asw_main_menu_option 0 2"
	}
	"4"
	{
		"label" "START NEW CAMPAIGN"
		"command" "engine asw_main_menu_option 0 0"
	}
	"5"
	{
		"label" "LOAD SAVED CAMPAIGN"
		"command" "engine asw_main_menu_option 0 1"
	}
	"7"
	{
		"label" "HOST NEW MISSION"
		"command" "engine asw_main_menu_option 1 2"
	}
	"8"
	{
		"label" "HOST NEW CAMPAIGN"
		"command" "engine asw_main_menu_option 1 0"
	}
	"9"
	{
		"label" "HOST SAVED CAMPAIGN"
		"command" "engine asw_main_menu_option 1 1"
	}
	"11"
	{
		"label" "#GameUI_GameMenu_FindServers"
		"command" "OpenServerBrowser"
	}
	"13"
	{
		"label" "#GameUI_GameMenu_Options"
		"command" "OpenOptionsDialog"
	}
	"14"
	{
		"label" "#GameUI_GameMenu_Quit"
		"command" "Quit"
	}
}

