--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

local bBackActive = false

hello_menu = {}

function hello_menu:Version( )
	return 4639;	
end


function hello_menu:OnLoad( )
	print( "hello_menu OnLoad!" );
	
	bBackActive = false
		
	backbutton = "layer_0_backbutton:backbutton"
	nextbutton = "layer_0_nextbutton:nextbutton"
			
end

function hello_menu:OnInit()
	print( "hello_menu_OnInit!" );
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = nextbutton }
				).graphichandle,
			"RequestFocus",
			{ }
		)
end


function hello_menu:OnKeyTyped( params )
	graphicScopedName = params.graphic
	code = params.code

	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER then
		if graphicScopedName == backbutton then
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_MENU1 )
		else
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_MAIN )
		end
		
	elseif code == ButtonCode.KEY_RIGHT then
		if graphicScopedName == backbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = nextbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		end
		
	elseif code == ButtonCode.KEY_LEFT then
		if graphicScopedName == nextbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = backbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		end	
	end			
end


function hello_menu:OnMouseClicked( params )
	graphic = params.graphic
	code = params.code

	if graphic == backbutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_MENU1 );
		
	elseif graphic == nextbutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_MAIN );	
	end		
end


function hello_menu:OnButtonReleased( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_XBUTTON_A then
		if graphic == backbutton then
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_MENU1 )
		else
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_MAIN )
		end
		
	elseif code == ButtonCode.KEY_XBUTTON_RIGHT then
		if graphic == backbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = nextbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		end
		
	elseif code == ButtonCode.KEY_XBUTTON_LEFT then
		if graphic == nextbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = backbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		end	
	end		
end

	



