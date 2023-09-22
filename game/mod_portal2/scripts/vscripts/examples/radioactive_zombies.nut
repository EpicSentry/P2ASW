//=========================================================
// This function is optional. This is called immediately
// before the spawner examines the KeyValues table (above)
// to determine which keys, if any, you'd like to replace.
//=========================================================
function PreSpawnInstance( entityClass, entityName )
{
	if( RandomInt(0,1) == 0 )
	{
		return { rendercolor = RandomInt( 150, 255 ) + " " + RandomInt( 150, 255 ) + " " + 80 };	
	}
	else
	{
		return { rendercolor = 0 + " " + 0 + " " + 80 };
	}
	
}



function PostSpawn( entities )
{
	foreach( k, v in entities )
	{
		printl( k + ": " + v );
	}
	if( RandomInt(0,1) == 0)
	{
		
	}
}