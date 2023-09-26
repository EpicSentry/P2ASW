function PlayStuckSound()
{
	printl("=================I AM TURRET NAMED: " + self.GetName() )
	
	TurretVoManager.TurretStuck(self)
}

// -------------------------------------------------------------------
// Called when a functioning turret starts the self test and will pass
// -------------------------------------------------------------------
function FunctioningTurretPass()
{
	printl("=================TURRET NAME: " + self.GetName() )
	TurretVoManager.GoodTurretPass(self)
	//ScannerPassTurret()
}

// -------------------------------------------------------------------
// Called when a functioning turret starts the self test and will fail
// -------------------------------------------------------------------
function FunctioningTurretFail()
{
	TurretVoManager.GoodTurretTest(self)
	//ScannerRejectTurret()
}

// -------------------------------------------------------------------
// Called when a malfunctioning turret starts the self test and will pass
// -------------------------------------------------------------------
function MalfunctioningTurretPass()
{
	//self.EmitSound("turret.TurretStuckInTube01")
	TurretVoManager.DefectTurretPass(self)
	//ScannerPassTurret()
}

// -------------------------------------------------------------------
// Called when a malfunctioning turret starts the self test and will fail
// -------------------------------------------------------------------
function MalfunctioningTurretFail()
{
	//self.EmitSound("turret.TurretStuckInTubeGoodbye01")
	TurretVoManager.DefectTurretTest(self)
	//ScannerRejectTurret()
}

function MalfunctioningTurretFling()
{
	TurretVoManager.DefectTurretFail(self)
}

function FunctioningTurretFling()
{
	TurretVoManager.GoodTurretFail(self)
}

function grabbedDefect()
{
	printl("==========GRABBED A TURRET!")
	TurretVoManager.grabbedDefectTurret(self)
}

function MalfunctioningTurretSneakBy()
{
	printl("===STARTING SNEAK!!!")
	TurretVoManager.DefectTurretSneakBy(self)
}

// =======================================================================
// =======================================================================


// -------------------------------------------------------------------
// Precache sounds we will emit
// -------------------------------------------------------------------
function Precache()
{
	self.PrecacheSoundScript( "turret.TurretStuckInTube01" )
	self.PrecacheSoundScript( "turret.TurretStuckInTubeGoodbye01" )
}

// -------------------------------------------------------------------
// Allow turret to pass scanner
// -------------------------------------------------------------------
function ScannerPassFunctionTurret()
{
	EntFire( "accept_turret_relay", "trigger", 0, 4 )
}

// -------------------------------------------------------------------
// Catapults the turret away 
// -------------------------------------------------------------------
function ScannerRejectFunctioningTurret()
{
	EntFire( "reject_turret_relay", "trigger", 0, 4 )
}

// -------------------------------------------------------------------
// Allow turret to pass scanner
// -------------------------------------------------------------------
function ScannerPassMalfunctionTurret()
{
	EntFire( "accept_turret_relay", "trigger", 0, 4 )
}

// -------------------------------------------------------------------
// Catapults the turret away
// -------------------------------------------------------------------
function ScannerRejectMalfunctioningTurret()
{
	EntFire( "reject_turret_relay", "trigger", 0, 4 )
}



// =======================================================================
// =======================================================================

// -------------------------------------------------------------------
// Called when a functioning turret is in position to shoot a dummy
// -------------------------------------------------------------------
function FunctioningTurretReachedDummyShootPosition()
{
	TurretVoManager.GoodTurretShootPosition(self)
}

// -------------------------------------------------------------------
// Called when a malfunctioning turret is in position to shoot a dummy
// -------------------------------------------------------------------
function MalfunctioningTurretReachedDummyShootPosition()
{
	TurretVoManager.DefectTurretShootPosition(self)
}


// -------------------------------------------------------------------
// Spawn a turret to shoot at dummy
// -------------------------------------------------------------------
function SpawnDummyShootTurret()
{
	printl("***Spawning a turret to shoot at dummy!")
	
	// spawn a new turret
	//EntFire("dummyshoot_conveyor_1_spawn_rl", "trigger", 0, 2 )
	
	// send current train on down the line to exit
	//EntFire("dummyshoot_conveyor_1_advance_train_relay", "trigger", 0, 2 )
}


//--------------------------------------------------------------------------------------------------------------
//Turret Queue Functions
//--------------------------------------------------------------------------------------------------------------
DBG <- 0

if( !( "GlobalTurretFactoryQueue" in this ) )
{
	// set up global queue
	::GlobalTurretFactoryQueue <- []
}

function OnPostSpawn()
{
	if( !( "GlobalTurretFactoryQueue" in this ) )
	{
		QueueInitialize()
		if (DBG) printl("===== Initializing QUEUE.  Length: " + GlobalTurretFactoryQueue.len() )
	}
}

//Initialize the queue
function QueueInitialize()
{
	GlobalTurretFactoryQueue.clear()
}

//Add a scene to the queue
function QueueAdd()
{
	GlobalTurretFactoryQueue.append( self.GetName() )
	if (DBG) printl("====== Adding " + self.GetName() + " to queue. Length = " + GlobalTurretFactoryQueue.len() )
	if( GlobalTurretFactoryQueue.len() > 4 )
	{
		if (DBG) printl("====== Turret queue reached " + GlobalTurretFactoryQueue.len() + ". Removing turret " + GlobalTurretFactoryQueue[0] + " New queue Length = " + GlobalTurretFactoryQueue.len() )
		
		// detonate turret
		EntFire( GlobalTurretFactoryQueue[0], "selfdestruct", 0, 0 )
		
		// remove the turret from the queue
		QueueDeleteFirstItem()
	}
}

//Returns number of items in the queue
function QueueLen()
{
	return GlobalTurretFactoryQueue.len()
}

//Delete a single item by index from the queue
function QueueDelete( index )
{
	if (QueueLen()==0)
	{
		return false
	}
	// remove from queue
 	GlobalTurretFactoryQueue.remove( index )
}

//Sort through queue and remove self if found
function RemoveSelfFromQueue()
{
	turretname <- self.GetName()
	if (DBG) printl("===== ATTEMPTING to remove self from queue: " + turretname )
	
	if (QueueLen()==0)
	{
		return false
	}	
	foreach (index, val in GlobalTurretFactoryQueue)
	{
		if (turretname == GlobalTurretFactoryQueue[index])
		{
			if (DBG) printl(" === removing #" + index + " named: " + GlobalTurretFactoryQueue[index] )
 			GlobalTurretFactoryQueue.remove(index)
 			return true
 		}
	}
	return false
}

//Delete first item from the queue
function QueueDeleteFirstItem()
{
	QueueDelete(0)
}