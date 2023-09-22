DoIncludeScript( "choreo/sphere_choreo_include", self.GetScriptScope() )

//Map Name
curMapName <- GetMapName()

// Set wheatley idle on map spawn.  
if( ( curMapName == "sp_a2_bts2" ) || ( curMapName == "sp_a2_bts6" ) )
{
	printl("===== Setting Wheatley glance concerned idle")
	EntFire( "@sphere", "SetIdleSequence", "sphere_damaged_glance_concerned", 0 )
}
else
{
	printl("===== Setting Wheatley idle concerned")
	EntFire( "@sphere", "SetIdleSequence", "sphere_damaged_idle_concerned", 0 )
}
