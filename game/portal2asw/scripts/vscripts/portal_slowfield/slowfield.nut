// constants
DBG 			<- 1
SLOWSCALE 	<- 0.2 // host_timescale
EFFECT_DURATION <- 5.5
hasThunk 		<- false

//---------------------------------------------------------
// the function that starts the slowfield
//---------------------------------------------------------
function SlowFieldStart()
{
	SendToConsole( "host_timescale " + SLOWSCALE )
	SendToConsole( "joy_pitchsensitivity 10" )
	SendToConsole( "joy_yawsensitivity -10" )
	SendToConsole( "portalgun_fire_delay .05" )
	
	// map entity IO to fire
	EntFire( zoom_in.GetName(), "Zoom" )
	EntFire( slowtime_color_correction.GetName(), "enable" )
	EntFire( slowtime_sound_time_start.GetName(), "playsound" )
	EntFire( slowtime_sound_loop.GetName(), "playsound" )
			
	// set the stop time for the slowfield
	::GlobalSlowfieldStarted = true
	::GlobalSlowfieldStopTime = Time() + ( SLOWSCALE * EFFECT_DURATION )
}

//---------------------------------------------------------
// the function that stops the slowfield
//---------------------------------------------------------
function SlowFieldEnd()
{
	SendToConsole( "host_timescale 0" )
	SendToConsole( "joy_pitchsensitivity 1.0" )
	SendToConsole( "joy_yawsensitivity -1.4" )
	SendToConsole( "portalgun_fire_delay .5" )
	
	// map entity IO to fire
	EntFire( zoom_out.GetName(), "zoom" )
	EntFire( slowtime_color_correction.GetName(), "disable" )
	EntFire( slowtime_sound_time_end.GetName(), "playsound" )
	EntFire( slowtime_sound_loop.GetName(), "stopsound" )
	
}

//---------------------------------------------------------
// When a portal is in this field, modify it's 
// behaviour on teleport to start the slow field.
//---------------------------------------------------------
function AddPortalSlowfieldEffect()
{
	activator.ValidateScriptScope()
	local portal_scope = activator.GetScriptScope()
	if ( !("SlowFieldStart" in portal_scope ) )
	{
		portal_scope.SlowFieldStart <- SlowFieldStart.bindenv( this )
	}
	
	activator.ConnectOutput( "OnPlayerTeleportFromMe", "SlowFieldStart" )
	activator.ConnectOutput( "OnPlayerTeleportToMe", "SlowFieldStart" )
}

self.ConnectOutput( "OnStartTouchPortal", "AddPortalSlowfieldEffect" )

//---------------------------------------------------------
// When a portal leaves this field, remove our behaviour modification.
//---------------------------------------------------------
function CleanupPortalSlowFieldEffect()
{
	activator.DisconnectOutput( "OnPlayerTeleportFromMe", "SlowFieldStart" )
	activator.DisconnectOutput( "OnPlayerTeleportToMe", "SlowFieldStart" )
	
	// TODO: Is this cleanup needed? Cost of delete/re-add vs bloating the portal's scope table
	activator.ValidateScriptScope()
	local portal_scope = activator.GetScriptScope()
	if ( ("SlowFieldStart" in portal_scope ) )
	{
		delete portal_scope.SlowFieldStart
	}
}

self.ConnectOutput( "OnEndTouchPortal", "CleanupPortalSlowFieldEffect" )

//---------------------------------------------------------
// THINK function
//---------------------------------------------------------
function Think()
{
	// Is the slowfield running?
	if ( ::GlobalSlowfieldStarted )
	{
		// is it time to stop the slowfield?
		if ( ::GlobalSlowfieldStopTime < Time() )
		{
			::GlobalSlowfieldStarted = false
			SlowFieldEnd()
		}
	}
	
	// do this once
	if ( !hasThunk )
	{
		// when this entity spawns stomp the cvars back to their default state
		// hack hack hack this is a horrible thing to do since it is slamming the state blindly
		// but it fixes the problem of the cvars getting stuck in the wrong state in the event of map restart during slowfield
		
		SendToConsole( "host_timescale 0" )
		SendToConsole( "joy_pitchsensitivity 1.0" )
		SendToConsole( "joy_yawsensitivity -1.4" )
		SendToConsole( "portalgun_fire_delay .5" )
		
		hasThunk = true
	}
}

if( !( "GlobalSlowfieldStopTime" in this ) )
{
	// set up globals
	::GlobalSlowfieldStopTime <- 0
	::GlobalSlowfieldStarted <- false
}