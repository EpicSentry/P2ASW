--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

ingamemenu = {}

function ingamemenu:Version( )
	return 4639;	
end


function ingamemenu:OnLoad( )
	print( "ingamemenu_OnLoad!" );
	resumegame = "layer_0_menubutton:CDmeHitAreaGeometry"
	exittomainmenu = "layer_0_menubutton_1:CDmeHitAreaGeometry_1"
	testnimbus = "layer_0_testnimbus:CDmeHitAreaGeometry_2"

	-- request nuggets
	c:Nugget( "load:app" )
end


function ingamemenu:OnInit()
	print( "ingamemenu_OnInit!" )
end


function ingamemenu:OnMouseClicked( params )
	if ( params.graphic == resumegame ) then
		c:Nugget( "use:app:ResumeGame" )
	elseif ( params.graphic == exittomainmenu ) then
		c:Nugget( "use:app:ExitToMainMenu" )
	elseif ( params.graphic == testnimbus ) then
		c:LoadMenu( GameUIConstants.MENUTYPE_MENU1 )
	end
end

