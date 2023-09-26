//=========================================================
// This script is attached to a func_door which will then
// enable it to be detected as a "portal door" by sign.nut
//=========================================================

// debugging
DBG <- 1

//---------------------------------------------------------
// flag this entity to be "a portal door"  
//---------------------------------------------------------
is_portal_door <- true

//---------------------------------------------------------
// These variables keep track of the total buttons and
// the number of buttons that are unpressed
//---------------------------------------------------------
number_buttons_total <- 0
number_buttons_pressed <- 0

//---------------------------------------------------------
// the button count function
//---------------------------------------------------------
function IncrementTotalButtons()
{
	number_buttons_total += 1
}
//---------------------------------------------------------
// This function will open this door if all buttons
// are pressed
//---------------------------------------------------------
function CloseButton()
{
	number_buttons_pressed += 1
	
	if ( number_buttons_total == number_buttons_pressed )
	{
		if(DBG) printl ( number_buttons_pressed + "/" + number_buttons_total )
		EntFire( self.GetName(), "open" )
	}
}
//---------------------------------------------------------
// This function will close this door.  This function will
// get called every time a button becomes unpressed
//---------------------------------------------------------
function OpenButton()
{
	number_buttons_pressed -= 1
	EntFire( self.GetName(), "close" )
	if(DBG) printl ( number_buttons_pressed + "/" + number_buttons_total )
}