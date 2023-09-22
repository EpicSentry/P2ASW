DBG <- 0

// set up global counter
if( !( "GlobalMusicalCatcherCount" in this ) )
{
	::GlobalMusicalCatcherCount <- 0
	if (DBG) printl("===== INITIALIZING GlobalMusicalCatcherCount")
}


// increment the count
GlobalMusicalCatcherCount++
if (DBG) printl("===== GlobalMusicalCatcherCount is now " + GlobalMusicalCatcherCount )


// set up global containing currently active catchers
if( !( "GlobalMusicalCatcherPowered" in this ) )
{
	::GlobalMusicalCatcherPowered <- 0
}

// connect to catcher entity IO
self.ConnectOutput( "OnPowered", "CatcherPowerOn" )
self.ConnectOutput( "OnUnPowered", "CatcherPowerOff" )

//---------------------------------------------------------
// called when catcher becomes powered
//---------------------------------------------------------
function CatcherPowerOn()
{
	::GlobalMusicalCatcherPowered++
	if (DBG) printl("===== Catcher " + self.GetName() + " powered ON!  Playing sound " + self.GetName() + "_music_" + GlobalMusicalCatcherPowered )
	EntFire( self.GetName() + "_music_" + GlobalMusicalCatcherPowered, "playsound", 0, 0 )
	QueueAdd()
}

//---------------------------------------------------------
// called when catcher becomes unpowered
//---------------------------------------------------------
function CatcherPowerOff()
{
	::GlobalMusicalCatcherPowered--
	if (DBG) printl("===== Catcher " + self.GetName() + " powered OFF!")
	EntFire( self.GetName() + "_music_*", "stopsound", 0, 0 )
	RemoveSelfFromQueue()
	
	// update music playlist
	RefreshMusicPlaylist()
}

function RefreshMusicPlaylist()
{
	foreach (index, val in GlobalCatcherPlayList)
	{
		if( GlobalCatcherPlayList[index].song != GlobalCatcherPlayList[index].catcher + "_music_" + (index + 1 ) )
		{
			if (DBG) printl(" =====  SONG " + GlobalCatcherPlayList[index].song + " DOES NOT MATCH " + GlobalCatcherPlayList[index].catcher + "_music_" + (index + 1) + " RESETTING ====" )
			EntFire( GlobalCatcherPlayList[index].catcher + "_music_*", "stopsound", 0, 0 )
			
			// playing correct sound
			if (DBG) printl("==== playing correct sound: " + GlobalCatcherPlayList[index].catcher + "_music_" + (index + 1) )
			EntFire( GlobalCatcherPlayList[index].catcher + "_music_" + (index + 1), "playsound", 0, 0 )
			
			// update table with new song name
			GlobalCatcherPlayList[index].song = GlobalCatcherPlayList[index].catcher + "_music_" + (index + 1)
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
//Queue Functions
//--------------------------------------------------------------------------------------------------------------
if( !( "GlobalCatcherPlayList" in this ) )
{
	// set up global queue
	::GlobalCatcherPlayList <- []
}

function OnPostSpawn()
{
	if( !( "GlobalCatcherPlayList" in this ) )
	{
		QueueInitialize()
		if (DBG) printl("===== Initializing QUEUE.  Length: " + GlobalCatcherPlayList.len() )
	}
}

//Initialize the queue
function QueueInitialize()
{
	GlobalCatcherPlayList.clear()
}

//Add a scene to the queue
function QueueAdd()
{
	entry <-[]
	
	entry <- 
	{ 
		catcher = self.GetName(), 
		song = self.GetName() + "_music_" + GlobalMusicalCatcherPowered
	}

	GlobalCatcherPlayList.append( entry )
	
	if (DBG) printl("====== Adding " + self.GetName() + " to queue. Queue Length = " + GlobalCatcherPlayList.len() )
}

//Display the queue
function QueueDisplay()
{
	if (DBG)
	{
		foreach (index, val in GlobalCatcherPlayList)
		{
			printl("===== " + index + " - " + GlobalCatcherPlayList[index].catcher + "   |Music - " + GlobalCatcherPlayList[index].song )
		}
	}
}

//Returns number of items in the queue
function QueueLen()
{
	return GlobalCatcherPlayList.len()
}

//Delete a single item by index from the queue
function QueueDelete( index )
{
	if (QueueLen()==0)
	{
		return false
	}
	// remove from queue
 	GlobalCatcherPlayList.remove( index )
}

//Sort through queue and remove self if found
function RemoveSelfFromQueue()
{
	catchername <- self.GetName()
	if (DBG) printl("===== ATTEMPTING to remove self from queue: " + catchername )
	
	if (QueueLen()==0)
	{
		return false
	}	
	foreach (index, val in GlobalCatcherPlayList)
	{
		if (catchername == GlobalCatcherPlayList[index].catcher)
		{
			if (DBG) printl("====== removing #" + index + " named: " + GlobalCatcherPlayList[index].catcher )
 			GlobalCatcherPlayList.remove(index)
 			return true
 		}
	}
	return false
}
