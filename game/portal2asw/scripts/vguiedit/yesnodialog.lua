--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );


yesnodialog = {}

function yesnodialog:Version( )
	return 4639;	
end


function yesnodialog:OnLoad( )
	print( "yesnodialog_OnLoad!" );
end


function yesnodialog:OnInit()
	print( "yesnodialog_OnInit!" )
end


function yesnodialog:OnYes()
	print( "OnYes Function called" )
end


function yesnodialog:OnNo()
	print( "OnNo Function called" )
end










	



