//********************************************************************************************
//Turret scanner screens
//********************************************************************************************
DBG <- 1

stock_scanner_turret_type <- 0
master_scanner_turret_type <- 1  //  master starts out as functional turret

player_in_scanner <- 0 // whether player is standing in scanner or not

// -------------------------------------------------------------------
// Returns true if template turret is function, false if it was swapped
// -------------------------------------------------------------------
function IsTemplateTurretFunctional()
{
	return master_scanner_turret_type
}

// -------------------------------------------------------------------
// Returns true if template turret is function, false if it was swapped
// -------------------------------------------------------------------
function ConveyorTurretFunctional()
{
	return stock_scanner_turret_type
}

// -------------------------------------------------------------------
// Template turret has been swapped for a broken variety
// -------------------------------------------------------------------
function TemplateTurretBroken()
{
	if (DBG) printl("===== Switching master template to broken.")	
	master_scanner_turret_type = 0;
}

// -------------------------------------------------------------------
// Turret about to be scanned is a functional turret
// -------------------------------------------------------------------
function StockTurretFunctional()
{
	if (DBG) printl("===== Switching stock scanner to functional.")
	stock_scanner_turret_type = 1;
}


// -------------------------------------------------------------------
// Turret about to be scanned is a broken turret
// -------------------------------------------------------------------
function StockTurretBroken()
{
	if (DBG) printl("===== Switching stock scanner to broken.")
	stock_scanner_turret_type = 0;
}

// -------------------------------------------------------------------
// Change the display to the pass screen
// -------------------------------------------------------------------
function SetStockDisplayPass()
{
	EntFire( "stock_scanner_display", "skin", "3", 0.5)
}

// -------------------------------------------------------------------
// Change the display to the fail screen
// -------------------------------------------------------------------
function SetStockDisplayFail()
{
	EntFire( "stock_scanner_display", "skin", "2", 0.5)
}

// -------------------------------------------------------------------
// Change the display to the reset screen
// -------------------------------------------------------------------
function SetStockDisplayReset()
{
	EntFire( "stock_scanner_display", "skin", "0", 0)
}

// -------------------------------------------------------------------
// Change the display to the pass screen
// -------------------------------------------------------------------
function SetMasterDisplayPass()
{
	if( IsPlayerInScanner() )
	{
		if (DBG) printl("===== Player in scanner. Overriding pass screen with error screen.")
		EntFire( "master_scanner_display", "skin", "1", 0.5)
	}
	else
	{
		EntFire( "master_scanner_display", "skin", "3", 0.5)
	}
}

// -------------------------------------------------------------------
// Change the display to the fail screen
// -------------------------------------------------------------------
function SetMasterDisplayFail()
{
	EntFire( "master_scanner_display", "skin", "2", 0.5)
}

// -------------------------------------------------------------------
// Change the display to the reset screen
// -------------------------------------------------------------------
function SetMasterDisplayReset()
{
	EntFire( "master_scanner_display", "skin", "0", 0)
}

// -------------------------------------------------------------------
// Change the display to the bad match screen
// -------------------------------------------------------------------
function SetResultDisplayBadMatch()
{
	EntFire( "result_scanner_display", "skin", "2", 0)
}

// -------------------------------------------------------------------
// Change the display to the good match screen
// -------------------------------------------------------------------
function SetResultDisplayGoodMatch()
{
	EntFire( "result_scanner_display", "skin", "3", 0)
}

// -------------------------------------------------------------------
// Change the display to the mixed match screen
// -------------------------------------------------------------------
function SetResultDisplayNoMatchLeftGreen()
{
	EntFire( "result_scanner_display", "skin", "5", 0)
}

// -------------------------------------------------------------------
// Change the display to the mixed match screen
// -------------------------------------------------------------------
function SetResultDisplayNoMatchRightGreen()
{
	EntFire( "result_scanner_display", "skin", "4", 0)
}

// -------------------------------------------------------------------
// Change the display to the reset screen
// -------------------------------------------------------------------
function SetResultDisplayReset()
{
	EntFire( "result_scanner_display", "skin", "0", 0)
}

// -------------------------------------------------------------------
// Reset all of the displays
// -------------------------------------------------------------------
function ResetScannerDisplays()
{
	if (DBG) printl("===== RESETTING ALL DISPLAYS.")
	SetResultDisplayReset()
	SetStockDisplayReset()
	SetMasterDisplayReset()
}

// -------------------------------------------------------------------
// Set whether player is standing in scanner or not
// -------------------------------------------------------------------
function SetPlayerInScanner( bool )
{
	if (DBG) printl("===== Setting Player in scanner to " + bool )
	player_in_scanner = bool
}

// -------------------------------------------------------------------
// Returns true if player is standing in scanner
// -------------------------------------------------------------------
function IsPlayerInScanner()
{
	return player_in_scanner
}


// -------------------------------------------------------------------
// Scan master turret
// -------------------------------------------------------------------
function ScanMasterTurret()
{
	// play scanner animation
	EntFire( "master_scanner_model", "SetAnimation", "turret_scanner_master_scan", 0)	
	
	// update the display with the results
	if ( IsTemplateTurretFunctional() )
	{
		SetMasterDisplayPass()
	}
	else
	{
		SetMasterDisplayFail()
	}
}

// -------------------------------------------------------------------
// Scan stock turret
// -------------------------------------------------------------------
function ScanStockTurret()
{
	// play scanner animation
	EntFire( "stock_scanner_model", "SetAnimation", "turret_scanner_master_scan", 0)
	
	// update the display with the results
	if ( ConveyorTurretFunctional() )
	{
		SetStockDisplayPass()
	}
	else
	{
		SetStockDisplayFail()
	}
}


// -------------------------------------------------------------------
// Display results of scan on the board
// -------------------------------------------------------------------
function DisplayScanResults()
{
	// test with functional master
	if( IsTemplateTurretFunctional() )
	{
		if( ConveyorTurretFunctional() )
		{
			SetResultDisplayGoodMatch()
		}
		else
		{
			SetResultDisplayNoMatchRightGreen()
		}
	}
	else
	{
		if( ConveyorTurretFunctional() )
		{
			SetResultDisplayNoMatchLeftGreen()
		}
		else
		{
			SetResultDisplayBadMatch()
		}
	}
}

