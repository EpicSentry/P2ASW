// Creating and using logic_choreographed_scene entities in script 

// Scene entity creation is only allowed when the script initially runs on entity spawn.
// Precaching is done automatically. Create everything you plan to use here.
scene_Sequential_01 						<- CreateSceneEntity("scenes/test_chamber/ai_13_a_nag01.vcd")
scene_Sequential_02 						<- CreateSceneEntity("scenes/test_chamber/ai_13_a_nag02.vcd")
scene_BoxFried 								<- CreateSceneEntity("scenes/general/ai_box_fried.vcd")

/*
scene_FlingSuccess 							<- CreateSceneEntity("scenes/test_chamber/ai_03_b_success.vcd")
scene_SecurityCamDestroyed_01 				<- CreateSceneEntity("scenes/general/generic_security_camera_destroyed-1.vcd")
scene_SecurityCamDestroyed_02 				<- CreateSceneEntity("scenes/general/generic_security_camera_destroyed-2.vcd")
scene_SecurityCamDestroyed_03 				<- CreateSceneEntity("scenes/general/generic_security_camera_destroyed-3.vcd")
scene_SecurityCamDestroyed_04 				<- CreateSceneEntity("scenes/general/generic_security_camera_destroyed-4.vcd")
scene_SecurityCamDestroyed_05 				<- CreateSceneEntity("scenes/general/generic_security_camera_destroyed-5.vcd")
*/
// Playing specific scene
function OnBoxFried()
{
	EntFireByHandle( scene_BoxFried, "Start", "", 0, null, null )
}

// Canceling specific scene
function CancelBoxFriedVO()
{
	EntFireByHandle( scene_BoxFried, "Cancel", "", 0, null, null )
}

// Sending inputs to whatever scene entity is currently running events on this actor
function PauseSpeech()
{
	local curscene = self.GetCurrentScene()
	if ( curscene != null )	
		EntFireByHandle( curscene, "Pause", "", 0, null, null )
}
function ResumeSpeech()
{
	local curscene = self.GetCurrentScene()
	if ( curscene != null ) 
		EntFireByHandle( curscene, "Resume", "", 0, null, null )
}
function PitchShift( shiftammount )
{
	local curscene = self.GetCurrentScene()
	if ( curscene != null ) 
		EntFireByHandle( curscene, "PitchShift", shiftammount.tostring(), 0, null, null )
}

// Do something when an output is fired by the scene
function PlayTwoScenesQueued()
{
	EntFireByHandle( scene_Sequential_01, "Start", "", 0, null, null )
	
	//TODO: I want to refactor this... It works but there are cleaner and safer ways to do it.
	scene_Sequential_01.ValidateScriptScope()	// Make sure we've created our script scope for this ent
	
	// Copy over this scene's instance. 
	// NOTE: This isnt nessicary and we could use BindEnv to keep from needing it
	// but this could give some extra power (like keeping a generic 'nextscene' that other funcions assume is there and will work on)
	scene_Sequential_01.GetScriptScope().nextscene <- scene_Sequential_02  
	
	// Add a function to this LCD's scope to call when OnCompletion fires
	scene_Sequential_01.GetScriptScope().PlayNextScene <- function() { EntFireByHandle( nextscene, "Start", "", 0, null, null ) }
	
	// Bind that function to the OnCompletion output
	scene_Sequential_01.ConnectOutput( "OnCompletion", "PlayNextScene" )
}