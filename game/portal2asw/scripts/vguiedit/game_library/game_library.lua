--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

local currentTopMenuItem = 1;
local currentBottomMenuItem = 1;

game_library = {}

function game_library:Version( )
	return 4639;	
end


function game_library:OnLoad( params )
	print( "game_library_OnLoad!" )
	self.scripthandle = params.scripthandle
	
	currentTopMenuItem = 1
	currentBottomMenuItem = 1
	bTopActive = true;
	bHomeActive = false;
	bBackActive = false;
	bottomRowWidth = 4;

	topbutton1 = "layer_0_topbutton1:topbutton1" 
	topbutton2 = "layer_0_topbutton2:topbutton2" 
	topbutton3 = "layer_0_topbutton3:topbutton3" 
	topbutton4 = "layer_0_topbutton4:topbutton4" 
	topbuttons = { topbutton1, topbutton2, topbutton3, topbutton4 }

	bottombutton1 = "layer_0_button1:bottombutton1" 
	bottombutton2 = "layer_0_button2:bottombutton2" 
	bottombutton3 = "layer_0_button3:bottombutton3" 
	bottombutton4 = "layer_0_button4:bottombutton4" 
	bottombutton5 = "layer_0_button5:bottombutton5" 
	bottombutton6 = "layer_0_button6:bottombutton6" 
	bottombutton7 = "layer_0_button7:bottombutton7" 
	bottombuttons = { bottombutton1, bottombutton2, bottombutton3, bottombutton4, bottombutton5, bottombutton6, bottombutton7 };
	
	homebutton = "layer_0_homebutton:homebutton"
	backbutton = "layer_0_backbutton:backbutton"
	
	
	c:CallScript( self.scripthandle, "InitAnims" )
end


function game_library:OnInit()
	print( "game_library_OnInit!" );
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topbuttons[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ }
		)
end


function game_library:GetNormalizedIndex( tableobj, index )
	local nItems = table.getn( tableobj )
	if index > nItems then
		index = 1
	end
	
	if index < 1 then
		index = nItems
	end
	
	return index	
end

function game_library:LayoutTopList( selectedItem )
	selectedItem = game_library:GetNormalizedIndex( topbuttons, selectedItem )
	currentTopMenuItem = selectedItem
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topbuttons[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
end
		
		
function game_library:LayoutBottomList( selectedItem )
	selectedItem = game_library:GetNormalizedIndex( bottombuttons, selectedItem )
	currentBottomMenuItem = selectedItem
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = bottombuttons[currentBottomMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
end


function game_library:OnUpdate()
end


function game_library:IsTopButton( graphic )
	i = 1
	while topbuttons[i] do
		if graphic == topbuttons[i] then
			return true
		end
		i = i + 1
	end
	
	return false
end


function game_library:IsBottomButton( graphic )
	i = 1
	while bottombuttons[i] do
		if graphic == bottombuttons[i] then
			return true
		end
		i = i + 1
	end
	
	return false
end

function game_library:IsTopmostRowofBottom( graphic )
	i = 1
	while bottombuttons[i] and i <= bottomRowWidth do
		if graphic == bottombuttons[i] then
			return true
		end
		i = i + 1
	end
	
	return false
end


function game_library:HandleEnter( graphic )
	if graphic == backbutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_MAIN )
		
	elseif graphic == homebutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MAINMENU )
		
	elseif graphic == bottombuttons[5] then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
		local newMenu = c:LoadMenu( GameUIConstants.MENUTYPE_GAMEDETAILS, { settings = { game = "portal", library = self.scripthandle } } )
		local res = c:CallScript( newMenu.scripthandle, "HiFromLibrary", { arg1 = "portal" } )
		print( res )
	end	
end


function game_library:ForExternalScriptsLols( params )
	print( "game library ForExternalScriptsLols invoked!" )
	print( params.arg1 )
	print( params.arg2 )
	print( "---------" )

	return { ret1 = "lol", ret2 = params.arg2 }
end


function game_library:HandleRight( graphic )
	if graphic == backbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = homebutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif graphic == homebutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topbuttons[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif game_library:IsTopButton( graphic ) then
		game_library:LayoutTopList( currentTopMenuItem + 1 )
		
	elseif game_library:IsBottomButton( graphic ) then
		game_library:LayoutBottomList( currentBottomMenuItem + 1)
	end	
end

	
function game_library:HandleLeft( graphic )
	if graphic == homebutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = backbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif game_library:IsTopButton( graphic ) then
		game_library:LayoutTopList( currentTopMenuItem - 1 )
		
	elseif game_library:IsBottomButton( graphic ) then
		game_library:LayoutBottomList( currentBottomMenuItem - 1)
	end	
end


function game_library:HandleUp( graphic )
	if game_library:IsTopButton( graphic ) then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = homebutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
			
	elseif game_library:IsTopmostRowofBottom( graphic ) then
		game_library:LayoutTopList( currentTopMenuItem )
		
	elseif game_library:IsBottomButton( graphic ) then
		game_library:LayoutBottomList( currentBottomMenuItem - bottomRowWidth )
	end
end


function game_library:HandleDown( graphic )
	if graphic == backbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topbuttons[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif graphic == homebutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topbuttons[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif game_library:IsTopButton( graphic ) then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = bottombuttons[currentBottomMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
		
	elseif game_library:IsBottomButton( graphic ) then
		game_library:LayoutBottomList( currentBottomMenuItem + bottomRowWidth )
	end
end

function game_library:OnKeyTyped( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER	then
		game_library:HandleEnter( graphic )
		
	elseif code == ButtonCode.KEY_RIGHT then
		game_library:HandleRight( graphic )
					
	elseif code == ButtonCode.KEY_LEFT then
		game_library:HandleLeft( graphic )
		
	elseif code == ButtonCode.KEY_DOWN then
		game_library:HandleDown( graphic )
		
	elseif code == ButtonCode.KEY_UP then		
		game_library:HandleUp( graphic )
	end		
end


function game_library:OnMouseClicked( params )
	game_library:HandleEnter( params.graphic )	
end



function game_library:OnButtonReleased( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_XBUTTON_A then
		game_library:HandleEnter( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_B then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_MAIN )
			
	elseif code == ButtonCode.KEY_XBUTTON_RIGHT then
		game_library:HandleRight( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_LEFT then
		game_library:HandleLeft( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_DOWN then
		game_library:HandleDown( graphic )		
	
	elseif code == ButtonCode.KEY_XBUTTON_UP then		
		game_library:HandleUp( graphic )
	end		
end


	



