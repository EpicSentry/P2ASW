//=========================================================
// This script is attached to a env_entity_maker.  It will
// spawn a template at a specifically named info_target
// and will then move the sign to a specifically named
// info_target.
//
// button info_targets must be named with the format:
// door01_button01
//
// sign info_targets must be named with the format:
// door01_sign01 
//=========================================================

// debugging
DBG <- 1



//---------------------------------------------------------
// OnPostSpawn
//---------------------------------------------------------
function OnPostSpawn()
{
	// object sizes
	enum size
	{
		small
		normal
		large
	}

	local cur_ent = null
	
	do
	{
		// find info_target that marks the position to spawn the button template
		cur_ent = Entities.FindByClassname( cur_ent, "info_target" )
		
		if ( cur_ent != null && cur_ent.GetName().find("button")!= null && "is_button_target" in cur_ent.GetScriptScope() )
		{
			local butt_pos = cur_ent.GetName().find("button")
			local system_name = cur_ent.GetName().slice( 0 ,butt_pos )
			local butt_numb = cur_ent.GetName().slice( butt_pos + 6, cur_ent.GetName().len() )
			
			if(DBG) printl(butt_pos)
			if(DBG) printl( system_name )
			if(DBG) printl( butt_numb )
			
			printl( EntityGroup[size.normal] )
				
			switch ( cur_ent.GetScriptScope().button_size )
			{
				case size.small: if(DBG) printl( "spawning button of small size" )
				EntityGroup[size.small].SpawnEntityAtEntityOrigin( cur_ent )
				break
				case size.normal: if(DBG) printl( "spawning button of normal size" )
				EntityGroup[size.normal].SpawnEntityAtEntityOrigin( cur_ent )
				break
				case size.large: if(DBG) printl( "spawning button of large size" )
				EntityGroup[size.large].SpawnEntityAtEntityOrigin( cur_ent )
				break
			}

			
			local sign = null
			local cur_brush = null
			
			
			// find info_target that marks the position to move the sign to
			do 
			{
				cur_brush = Entities.FindByClassnameWithin( cur_brush, "func_brush", cur_ent.GetOrigin(), 1000 )

				if ( cur_brush != null && "is_sign" in cur_brush.GetScriptScope() && cur_brush.GetScriptScope().is_sign )
				{
					if(DBG) printl(" found a brush: " + cur_brush )
					sign = cur_brush
				}
				
			} while ( cur_brush != null )
			
			if ( sign == null )
			{
				if(DBG) printl (" No sign for " + cur_ent )
				return
			}

			local sign_dest_name = system_name + "sign" + butt_numb
			if(DBG) printl ("sign pos: " + sign_dest_name)

			sign.SetOrigin( Entities.FindByName( null, sign_dest_name ).GetOrigin() )
			
			sign.SetForwardVector( Entities.FindByName( null, sign_dest_name).GetForwardVector() )
				
			
			// getforwardvector
			// setforwardvector

		}
	} while ( cur_ent != null )	
}