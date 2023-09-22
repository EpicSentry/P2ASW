////////////////////////////////////  CONSTANTS //////////////////////////////////////////

const DBG = 1

const MIN = 0
const MAX = 1

const NPROPS = 5  // # of each props to make

const SPAWN_DELAY = 0.1 // delay between prop spawns

const PILE_MAX = 120  // The largest x and y of a successful pile region

const WORLD_MAX = 20000 // more than world extents

///////////////////////////////////  FUNCTION DEFINITIONS ////////////////////////////////  Functions need to be defined before they are called.  That's why CODE section is at the bottom.




////////////////////////////////////////// CODE ///////////////////////////////////////////// this code gets run when the entity activates
SortPuzzle <- {}
SortPuzzle.Bounds <- [] // 2 vectors that describe a volume within which to spawn entities with the prop spawners.
SortPuzzle.PropSpawners <- [] // a list of names of entities that will be called to spawn at map start.
SortPuzzle.Props <- {} // table of all props	

SP <- SortPuzzle // a shorter pointer to the SortPuzzle table
