 //=========================================================
 // This simple script simulates a coin flip by picking a 
 // a random number between 0 and 1. 0 = heads, 1 = tails.
 // The function also keeps track of how many times we
 // picked heads or tails.
 //=========================================================
 
 //---------------------------------------------------------
 // These variables keep track of how many times heads or
 // tails was picked.
 //---------------------------------------------------------
 numHeads <- 0;
 numTails <- 0;
 
 //---------------------------------------------------------
 // The coin flip function
 //---------------------------------------------------------
 function CoinFlip()
 {
 	if( RandomInt(0,1) == 0 )
 	{
 		printl("Heads!");
 		numHeads += 1;
 	}
 	else
 	{
 		printl("Tails!");
 		numTails += 1;
 	}
 };
