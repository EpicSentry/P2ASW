// MATT'S HAND EDITED STUFF HERE - BOOYAH

//		SceneTable["coop_score_blue05_00"] <- {vcd=CreateSceneEntity("scenes/npc/glados/COOP_SCORING06.vcd"),postdelay=1.0,next="coop_score_blue05_01"}		
//		SceneTable["coop_score_blue05_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/COOP_NUMBERS15.vcd"),postdelay=1.0,next="coop_score_blue05_02"}		
//		SceneTable["coop_score_blue05_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/COOP_SCORING03.vcd"),postdelay=1.0,next=null}		



		//PAX ATTAX
		SceneTable["coop_gesture_intro_pax01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro05.vcd"),postdelay=1.0,next="coop_gesture_intro_pax02"}
		SceneTable["coop_gesture_intro_pax02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro02.vcd"),postdelay=0.5,next=null}
		//PAX ATTAX
		
		

		
		SceneTable["coop_portal_ping_intro_orange_nag"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro15.vcd"),postdelay=0.5,next=null}
		SceneTable["coop_portal_ping_intro_blue_nag"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro16.vcd"),postdelay=0.5,next=null}
		
		SceneTable["coop_portal_ping_reminder01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro17.vcd"),postdelay=0.5,next=null}
			
		// intro map exits dialogue
//		SceneTable["coop_start_results_1_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubeone01.vcd"),postdelay=0.5,next="coop_start_results_1_02"}
		//SceneTable["coop_start_results_1_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubeone02.vcd"),postdelay=0.5,next="coop_start_results_1_03"}
		//SceneTable["coop_start_results_1_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubeone03.vcd"),postdelay=0.5,next="coop_start_results_1_04"}
		//SceneTable["coop_start_results_1_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubeone04.vcd"),postdelay=0.5,next=null,fireentity="@relay_enable_exit",fireinput="Trigger",firedelay=0.0}
		
		SceneTable["coop_start_results_2_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubetwo01.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}

		SceneTable["coop_start_results_3_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubethree01.vcd"),postdelay=0.5,next="coop_start_results_3_02"}
		SceneTable["coop_start_results_3_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_tubethree02.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}

		SceneTable["coop_humansplanb_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humansplanb01.vcd"),postdelay=1.5,next="coop_humansplanb_02"}
		SceneTable["coop_humansplanb_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humansplanb02.vcd"),postdelay=0.5,next=null,fireentity="relay_initiate_plan_B",fireinput="Trigger",firedelay=0.0}

		SceneTable["coop_radar_room_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/coop/coop_radar_find_human_entry.vcd"),postdelay=0.5,next=null}

		SceneTable["coop_radar_room_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_intro05.vcd"),postdelay=2.5,next="coop_radar_room_03"}
		SceneTable["coop_radar_room_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humandetector01.vcd"),postdelay=0.5,next="coop_radar_room_04"}
		SceneTable["coop_radar_room_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humandetector02.vcd"),postdelay=3.5,next="coop_radar_room_05",fireentity="relay_radar_start_human",fireinput="Trigger",firedelay=1.0}
		SceneTable["coop_radar_room_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humandetector04.vcd"),postdelay=0.5,next="coop_radar_room_06"}
		SceneTable["coop_radar_room_06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_humandetector05.vcd"),postdelay=0.5,next="coop_radar_room_07"}
		SceneTable["coop_radar_room_07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/PreHub04.vcd"),postdelay=0.5,next=null,fireentity="@relay_explode_bots_transition",fireinput="Trigger",firedelay=0.5}

		SceneTable["coop_hub_first_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run01.vcd"),postdelay=0.5,next="coop_hub_first_02"}
		SceneTable["coop_hub_first_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run03.vcd"),postdelay=0.5,next="coop_hub_first_03"}
		SceneTable["coop_hub_first_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run04.vcd"),postdelay=0.5,next="coop_hub_first_04"}
		SceneTable["coop_hub_first_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run05.vcd"),postdelay=0.5,next="coop_hub_first_05"}
		SceneTable["coop_hub_first_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/coop/coop_explain_make_human.vcd"),postdelay=0.5,next=null,fireentity="relay_earn_first_gesture",fireinput="Trigger",firedelay=0.0}

		SceneTable["coop_hub_first_06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/coop/coop_explain_make_human_fail.vcd"),postdelay=0.5,next="coop_hub_first_07",fireentity="relay_enable_all_tracks",fireinput="Trigger",firedelay=0.0}
		SceneTable["coop_hub_first_07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run06.vcd"),postdelay=8.0,next="coop_hub_first_08"}
		SceneTable["coop_hub_first_08"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_hub_first_run07.vcd"),postdelay=0.5,next=null}

		SceneTable["coop_hub_artifact1_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub01.vcd"),postdelay=0.5,next="coop_hub_artifact1_02",fireentity="relay_screen_artifact01",fireinput="Trigger",firedelay=0.0}
		SceneTable["coop_hub_artifact1_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub02.vcd"),postdelay=0.5,next="coop_hub_artifact1_03"}
		SceneTable["coop_hub_artifact1_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub03.vcd"),postdelay=2.0,next="coop_hub_artifact1_04"}
		SceneTable["coop_hub_artifact1_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub04.vcd"),postdelay=1.5,next="coop_hub_artifact1_05"}
		SceneTable["coop_hub_artifact1_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub05.vcd"),postdelay=5.5,next="coop_hub_artifact1_06",fireentity="relay_earn_gesture_artifact_1",fireinput="Trigger",firedelay=0.0}
		SceneTable["coop_hub_artifact1_06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub06.vcd"),postdelay=1.5,next="coop_hub_artifact1_07"}
		SceneTable["coop_hub_artifact1_07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub07.vcd"),postdelay=1.5,next="coop_hub_artifact1_08"}
		SceneTable["coop_hub_artifact1_08"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone_hub08.vcd"),postdelay=1.5,next=null}

		// found artifact 1
		SceneTable["coop_artifact1_01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone01.vcd"),postdelay=0.5,next="coop_artifact1_02"}
		SceneTable["coop_artifact1_02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone02.vcd"),postdelay=0.5,next=null,fireentity="@relay_see_artifact",fireinput="Trigger",firedelay=2.0}
		
		// that's it, scan it
		SceneTable["coop_artifact1_03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone03.vcd"),postdelay=0.5,next=null,fireentity="@relay_make_artifact_glow",fireinput="Trigger",firedelay=0.0}

		SceneTable["coop_artifact1_04"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone04.vcd"),postdelay=0.5,next="coop_artifact1_05"}
		SceneTable["coop_artifact1_05"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone05.vcd"),postdelay=1.5,next="coop_artifact1_06"}
		SceneTable["coop_artifact1_06"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone06.vcd"),postdelay=0.5,next="coop_artifact1_07"}
		SceneTable["coop_artifact1_07"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone07.vcd"),postdelay=0.5,next="coop_artifact1_08"}
		SceneTable["coop_artifact1_08"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone08.vcd"),postdelay=1.5,next="coop_artifact1_09"}
		SceneTable["coop_artifact1_09"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_artifactone09.vcd"),postdelay=0.75,next="coop_artifact1_10"}
		SceneTable["coop_artifact1_10"] <- {vcd=CreateSceneEntity("scenes/npc/glados/PreHub04.vcd"),postdelay=0.5,next=null,fireentity="@relay_explode_bots_transition",fireinput="Trigger",firedelay=0.5}
							
		SceneTable["botcoop_posthub_tube_zero"] <- {vcd=CreateSceneEntity("scenes/npc/glados/coop/coop_posthub_tube_zero_temp.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}
		
//		SceneTable["botcoop_posthub_tube_one01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_posthub_tube_one01.vcd"),postdelay=0.5,next="botcoop_posthub_tube_one02"}
//		SceneTable["botcoop_posthub_tube_one02"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_posthub_tube_one02.vcd"),postdelay=0.5,next="botcoop_posthub_tube_one03"}
//		SceneTable["botcoop_posthub_tube_one03"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_posthub_tube_one03.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}
		
		//botcoop_competition01
		SceneTable["botcoop_competition01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_competition01.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}
		//botcoop_competition10
		SceneTable["botcoop_competition10"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_competition10.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}
		//botcoop_competition13
		SceneTable["botcoop_competition13"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_competition13.vcd"),postdelay=0.5,next=null,char="glados",fires=[{entity="@relay_enable_exit",input="Trigger",parameter="",delay=0.00}]}
		//botcoop_paxdemo01
		SceneTable["botcoop_paxdemo01"] <- {vcd=CreateSceneEntity("scenes/npc/glados/botcoop_paxdemo01.vcd"),postdelay=0.5,next=null}		





	SceneTableLookup[29] <- "coop_gesture_intro"
	//SceneTableLookup[30] <- "coop_ping_intro01"
	SceneTableLookup[31] <- "coop_ping_blue_start"
	//SceneTableLookup[32] <- "coop_ping_blue_success"
	//SceneTableLookup[33] <- "coop_ping_orange_start"
	//SceneTableLookup[34] <- "coop_ping_orange_success"
	
	SceneTableLookup[36] <- "coop_ping_intro02"
	SceneTableLookup[37] <- "coop_ping_blue_success02"
	SceneTableLookup[38] <- "coop_ping_blue_success03"
	SceneTableLookup[39] <- "coop_ping_orange_success02"
	SceneTableLookup[40] <- "coop_ping_orange_success03"
	SceneTableLookup[41] <- "coop_portal_ping_intro02"
	SceneTableLookup[42] <- "coop_portal_ping_intro03"
	SceneTableLookup[43] <- "coop_portal_ping_intro_orange_nag"
	SceneTableLookup[44] <- "coop_portal_ping_intro_blue_nag"
	SceneTableLookup[45] <- "coop_portal_ping_reminder01"
	
	//SceneTableLookup[50] <- "coop_start_results_1_01"
	SceneTableLookup[51] <- "coop_start_results_1_02"
	SceneTableLookup[52] <- "coop_start_results_1_03"
	SceneTableLookup[53] <- "coop_start_results_1_04"
	
//	SceneTableLookup[54] <- "coop_start_results_2_01"
	
	SceneTableLookup[55] <- "coop_start_results_3_01"
	SceneTableLookup[56] <- "coop_start_results_3_02"

	SceneTableLookup[60] <- "coop_humansplanb_01"
	SceneTableLookup[61] <- "coop_humansplanb_02"	
	
	SceneTableLookup[65] <- "coop_radar_room_01"

	SceneTableLookup[66] <- "coop_radar_room_02"
	SceneTableLookup[67] <- "coop_radar_room_03"
	SceneTableLookup[68] <- "coop_radar_room_04"
	SceneTableLookup[69] <- "coop_radar_room_05"
	SceneTableLookup[70] <- "coop_radar_room_06"
	SceneTableLookup[71] <- "coop_radar_room_07"

	SceneTableLookup[75] <- "coop_hub_first_01"
	SceneTableLookup[76] <- "coop_hub_first_02"
	SceneTableLookup[77] <- "coop_hub_first_03"
	SceneTableLookup[78] <- "coop_hub_first_04"
	SceneTableLookup[79] <- "coop_hub_first_05"
	SceneTableLookup[80] <- "coop_hub_first_06"	
	SceneTableLookup[81] <- "coop_hub_first_07"	
	SceneTableLookup[82] <- "coop_hub_first_08"	
	
	SceneTableLookup[85] <- "coop_hub_artifact1_01"		
	SceneTableLookup[86] <- "coop_hub_artifact1_02"		
	SceneTableLookup[87] <- "coop_hub_artifact1_03"		
	SceneTableLookup[88] <- "coop_hub_artifact1_04"		
	SceneTableLookup[89] <- "coop_hub_artifact1_05"		
	SceneTableLookup[90] <- "coop_hub_artifact1_06"		
	SceneTableLookup[91] <- "coop_hub_artifact1_07"		
	SceneTableLookup[92] <- "coop_hub_artifact1_08"		

	SceneTableLookup[100] <- "coop_artifact1_01"		
	SceneTableLookup[101] <- "coop_artifact1_02"	
	SceneTableLookup[102] <- "coop_artifact1_03"	
	SceneTableLookup[103] <- "coop_artifact1_04"	
	SceneTableLookup[104] <- "coop_artifact1_05"	
	SceneTableLookup[105] <- "coop_artifact1_06"	
	SceneTableLookup[106] <- "coop_artifact1_07"	
	SceneTableLookup[107] <- "coop_artifact1_08"	
	SceneTableLookup[108] <- "coop_artifact1_09"	
	SceneTableLookup[109] <- "coop_artifact1_10"		
	
	SceneTableLookup[120] <- "botcoop_posthub_tube_zero" //COOP_GESTURE1_01	

//	SceneTableLookup[130] <- "botcoop_posthub_tube_one01"
	SceneTableLookup[131] <- "botcoop_posthub_tube_one02"
	SceneTableLookup[132] <- "botcoop_posthub_tube_one03"
	SceneTableLookup[133] <- "botcoop_posthub_tube_one04"

	// PAX HERE
	SceneTableLookup[150] <- "botcoop_competition01"
	SceneTableLookup[151] <- "botcoop_competition10"
	SceneTableLookup[152] <- "botcoop_competition13"
	SceneTableLookup[153] <- "botcoop_paxdemo01"
	
	SceneTableLookup[155] <- "coop_gesture_intro_pax01"
	SceneTableLookup[156] <- "coop_gesture_intro_pax02"
	SceneTableLookup[2000] <- "coop_score_blue05_00"
