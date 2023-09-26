--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );


hitarea_yes = {}

function hitarea_yes:Version( )
	return 4639;	
end


function hitarea_yes:OnLoad( )
	print( "hitarea_yes_OnLoad!" );
end

function hitarea_yes:OnMouseUp( )
	print( "hitarea_yes_OnMouseUp!" );
	c:SetNextMenu( GameUIConstants.MENUTYPE_HELLO );
end

	



