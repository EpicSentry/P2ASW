//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//=============================================================================

// No spaces in event names, max length 32
// All strings are case sensitive
//
// valid data key types are:
//   string : a zero terminated string
//   bool   : unsigned int, 1 bit
//   byte   : unsigned int, 8 bit
//   short  : signed int, 16 bit
//   long   : signed int, 32 bit
//   float  : float, 32 bit
//   local  : any data, but not networked to clients
//
// following key names are reserved:
//   local      : if set to 1, event is not networked to clients
//   unreliable : networked, but unreliable
//   suppress   : never fire this event
//   time	: firing server time
//   eventid	: holds the event ID

"ModEvents"
{
	"portal_player_touchedground"	// player landed
	{
		"userid"	"short"		// user ID on server
	}

	"portal_player_ping"	// player pinged
	{
		"userid"	"short"		// user ID on server
		"ping_x"	"float"
		"ping_y"	"float"
		"ping_z"	"float"
	}
	
	"portal_player_portaled"		// player traveled through a portal
	{
		"userid"	"short"		// user ID on server
		"portal2"	"bool"		// false for portal1 (blue)
	}
	"turret_hit_turret"
	{
	}
	"security_camera_detached"
	{
	}
	"challenge_map_complete"
	{
		"numbronze"	"short"
		"numsilver"	"short"
		"numgold"	"short"
	}
	"advanced_map_complete"
	{
		"numadvanced"	"short"
	}
	"quicksave"
	{
	}
	"autosave"
	{
	}
	"slowtime"
	{
	}
	
	"portal_enabled"
	{
		"userid"		"short"		// user ID on server
		"leftportal"	"bool"
	}
	
	"portal_fired"
	{
		"userid"		"short"		// user ID on server
		"leftportal"	"bool"
	}
	
	"gesture_earned"
	{
		"userid"	"short"		// user ID on server
		"teamtaunt"		"bool"
	}
	
	"player_gesture"
	{
		"userid"	"short"		// user ID on server
		"air"		"bool"
	}

	"player_zoomed"
	{
		"userid" "short"
	}

	"player_unzoomed"
	{
		"userid" "short"
	}
	
	"player_countdown"
	{
		"userid"	"short"		// user ID on server
	}
	
	"player_touched_ground"
	{
		"userid"	"short"
	}
	
	"player_long_fling"
	{
		"userid"	"short"
	}
	
	"remote_view_activated"
	{
	}
	
	"touched_paint"
	{
		"userid" "short"
		"painttype" "short"
		"surfacedir" "short"
	}
	"player_paint_jumped"
	{
		"userid" "short"
	}
	"move_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"movedone_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"counter_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"zoom_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"jump_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"partnerview_hint_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"paint_cleanser_visible"
	{
		"userid"	"short"	//The player who sees the entity
		"subject"	"long"	//Entindex of the entity they see
	}
	"paint_cleanser_not_visible"
	{
		"userid"	"short"
	}
	"player_touch_paint_cleanser"
	{
		"userid"	"short"
		"subject"	"long"	//Entindex of the cleanser
	}
	"bounce_count"
	{
		"userid"	"short"
		"bouncecount"	"short"
	}
	"player_landed"
	{
		"userid"	"short"
	}
	"player_suppressed_bounce"
	{
		"userid"	"short"
	}
	"OpenRadialMenu"
	{
		"target" "string"
	}
	"AddLocator"
	{
		"userid" "short"
		"subject" "short"
		"caption" "string"
	}
	"player_spawn_blue"
	{
	}
	"player_spawn_orange"
	{
	}
	"map_already_completed"
	{
	}
	
	"achievement_earned"
	{
		"player"	"byte"		// entindex of the player
		"achievement"	"short"		// achievement ID
	}
	
	"replay_status"
	{
	}
	
	"spec_target_updated"
	{
	}
	
	"player_fullyjoined"
	{
		"userid"	"byte"		// entindex of the player
		"name" "string"
	}
	
	"achievement_write_failed"
	{
	}
	
	"player_stats_updated"
	{
		"forceupload" "bool"
	}
	
	"round_start_pre_entity"
	{
	}
	
	"teamplay_round_start"			// round restart
	{
		"full_reset"	"bool"		// is this a full reset of the map
	}
	
	"client_disconnect"
	{
	}
	
	"server_pre_shutdown" 		// server is about to be shut down	
	{
		"reason"	"string"	// reason why server is about to be shut down
	}
	
	"difficulty_changed"
	{
		"newDifficulty"	"short"
		"oldDifficulty"	"short"
		"strDifficulty" "string" // new difficulty as string
	}
	
	"finale_start"
	{
		"rushes"		"short"
	}

	"finale_win"
	{
		"map_name"		"string"
		"difficulty"	"short"
	}

	"vote_passed"
	{
		"details"		"string"
		"param1"		"string"
		"team"			"byte"
		"reliable"		"1"		// this event is reliable
	}

//////////////////////////////////////////////////////////////////////
// Economy events
//////////////////////////////////////////////////////////////////////
	"inventory_updated"
	{
	}
	"cart_updated"
	{
	}
	"store_pricesheet_updated"
	{
	}
	"gc_connected"
	{
	}
	"item_schema_initialized"
	{
	}
	"client_loadout_changed"
	{
	}
	"gameui_activated"
	{
	}
	"gameui_hidden"
	{
	}

}

