//============================================================================
// To change what movie plays during the level transition, change the entry named MOVIE_NAME
// valid movies to play are in the "portal2\media" folder.  They must exist in this directory
// and you only need to specify the file name, not the path.
//============================================================================

MOVIE_NAME<- [
	"coop_bots_load",
	"coop_bots_load_wave",
	"coop_bluebot_load",
	"coop_orangebot_load",
]
//MOVIE_NAME <- "gd_babysit"

//============================================================================

function PreSpawnInstance( entityClass, entityName )
{
	local nPick = 0;
	local nBranch = GetCoopSectionIndex()

	if ( nBranch > 0 )
	{
		// Use the current branch/level to cycle through
		local nCurrentLevel = GetCoopBranchLevelIndex( nBranch )   
		local nMovielength = MOVIE_NAME.len()
		nPick = (nBranch + nCurrentLevel) % nMovielength
	}
	
	local movie = MOVIE_NAME[ nPick ]   
    //printl("================PLAYING " + movie + ", length = " + movielength )
    return { MovieFilename = movie }
}

