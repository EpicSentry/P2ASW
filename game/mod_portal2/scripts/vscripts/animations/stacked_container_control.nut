
// --------------------------------------------------------
function PrecacheContainerAnimations()
{
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part1.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part3.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part5.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part7.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part9.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part11.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part12.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/finedebris_part13.ps3.ani")

	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/container_lorez.ps3.ani")
	SendToConsole("fs_fios_prefetch_file_in_pack models/container_ride/container_lolorez.ps3.ani")
}

function StartContainerAnimations()
{
	EntFire("@container_stacks_1", "setanimation", "anim1", 0 )
	EntFire("@container_stacks_2", "setanimation", "anim1", 0 )
	EntFire("@container_stacks_2", "DisableDraw", "", 0 )
	
//	for(local i=1;i<=92;i+=1)
//	{
//		EntFire("container_stacked_" + i,"setparentattachment", "vstattachment", 0 )
//		EntFire("@container_stacked_" + i,"setanimation", "container" + i, 0 )
//		
//		if( i > 56 )
//		{
//			EntFire("@container_stacked_" + i,"DisableDraw", "", 0 )	
//		}	
//	}
}

function ShowHiddenContainers()
{
	EntFire("@container_stacks_2","EnableDraw", "", 0 )
//	for(local i=57;i<=92;i+=1)
//	{
//		EntFire("@container_stacked_" + i,"EnableDraw", "", 0 )
//	}
}

function SetupContainerAttachments()
{
//	for(local i=1;i<=92;i+=1)
//	{
//		EntFire("container_stacked_" + i,"SetParentAttachmentMaintainOffset", "vstAttachment_noOrient", 0 )
//	}
}
