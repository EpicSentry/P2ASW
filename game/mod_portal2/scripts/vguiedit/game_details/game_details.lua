--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

local currentTopMenuItem = 1;
local currentLeftMenuItem = 1;
local appname = "C:\\Program Files (x86)\\SteamBeta\\Steam.exe -applaunch 400"

SpecialCase = 2;

game_details = {}

function game_details:Version( )
	return 4639	
end


function game_details:OnLoad( params )
	print( "game_details_OnLoad!" )
	self.scripthandle = params.scripthandle

	print( specialCASE )
	print( currentTopMenuITEM )
	print( APPNAME )

	print( "game_details screen handle:" )
	print( params.scripthandle )

	if ( params ) then
		print( "OnLoad settings:" )
		print( params.settings )
		if ( params.settings ) then
			print( "OnLoad settings.game:" )
			print( params.settings.game )
		end
	end
	
	currentTopMenuItem = 1;
	currentLeftMenuItem = 1;
	
	
	topmenuitem1 = "layer_0_topbutton1:topmenubutton1" 
	topmenuitem2 = "layer_0_topbutton2:topmenubutton2" 
	topmenuitem3 = "layer_0_topbutton3:topmenubutton3" 
	topmenuitems = { topmenuitem1, topmenuitem2, topmenuitem3 }

	
	leftmenu1 = "layer_0_leftbutton1:leftmenubutton1" 
	leftmenu2 = "layer_0_leftbutton2:leftmenubutton2" 
	leftmenu3 = "layer_0_leftbutton3:leftmenubutton3" 
	leftmenu4 = "layer_0_leftbutton4:leftmenubutton4"  
	leftmenuitems = { leftmenu1, leftmenu2, leftmenu3, leftmenu4 }
	
	
	homebutton = "layer_0_homebutton:homebutton" 
	backbutton = "layer_0_backbutton:backbutton" 
	

	c:CallScript( self.scripthandle, "InitAnims" )

	-- test script handle calling
	
	local r1 = c:CallScript( self.scripthandle, "TESTFUNCTIONHERE", { aRG1 = "val1", ARg2 = "val2" } )
	print( r1.result )

	local r2 = c:CallScript( params.settings.library, "ForExternalScriptsLOLS", { ARG1 = "from game_details", ARG2 = "come back" } )
	print( r2.RET1 )
	print( R2.Ret2 )
end


function game_details:TestFunctionHere( PARAMS )
	print( "TestFunctionHere successfully invoked!" )
	print( params.arg1 )
	print( params.arg2 )
	print( "---------" )
	
	return { RESULT = "result test" }
end


function game_details:HiFromLibrary( params )
	print( "HiFromLibrary successfully invoked!" )
	print( params.arg1 )
	print( params.arg2 )
	print( "---------" )
end


function game_details:OnInit()
	print( "game_details_OnInit!" );
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = leftmenuitems[currentLeftMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ }
		)
end



function game_details:GetNormalizedIndex( tableobj, index )
	local nItems = table.getn( tableobj )
	if index > nItems then
		index = 1
	end
	
	if index < 1 then
		index = nItems
	end
	
	return index	
end


function game_details:LayoutTopList( selectedItem )
	selectedItem = game_details:GetNormalizedIndex( topmenuitems, selectedItem )	
	currentTopMenuItem = selectedItem;
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topmenuitems[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
end


function game_details:LayoutLeftList( selectedItem )
	selectedItem = game_details:GetNormalizedIndex( leftmenuitems, selectedItem )
	currentLeftMenuItem = selectedItem;
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = leftmenuitems[currentLeftMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
end


function game_details:OnUpdate()
end


function game_details:IsLeftButton( graphic )
	i = 1
	while leftmenuitems[i] do
		if graphic == leftmenuitems[i] then
			return true
		end
		i = i + 1
	end
	
	return false
end


function game_details:IsTopButton( graphic )
	i = 1
	while topmenuitems[i] do
		if graphic == topmenuitems[i] then
			return true
		end
		i = i + 1
	end
	
	return false
end
 
 
 
function game_details:HandleEnter( graphic )
	if graphic == homebutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MAINMENU )
		
	elseif graphic == backbutton then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_GAMELIB )
		
	end 
end


function game_details:HandleUp( graphic )
	if graphic == leftmenuitems[1] or game_details:IsTopButton( graphic ) then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = homebutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
	elseif game_details:IsLeftButton( graphic ) then
		game_details:LayoutLeftList( currentLeftMenuItem - 1 )		
	end
end

function game_details:HandleDown( graphic )
	if graphic == homebutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = leftmenuitems[currentLeftMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
	elseif game_details:IsLeftButton( graphic ) then
		game_details:LayoutLeftList( currentLeftMenuItem + 1 )
	end
end

function game_details:HandleLeft( graphic )
	if graphic == homebutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = backbutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
	elseif graphic == topmenuitems[1] then
		game_details:LayoutLeftList( currentLeftMenuItem )
	elseif game_details:IsTopButton( graphic ) then		
		game_details:LayoutTopList( currentTopMenuItem - 1 )
	end
end

function game_details:HandleRight( graphic )
	if graphic == backbutton then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = homebutton }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
	elseif graphic == homebutton or game_details:IsLeftButton( graphic ) then
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = topmenuitems[currentTopMenuItem] }
				).graphichandle,
			"RequestFocus",
			{ sound = GameUIConstants.BUTTONMOVE_SOUND }
		)
	elseif game_details:IsTopButton( graphic ) then
		game_details:LayoutTopList( currentTopMenuItem + 1 )
	end
end
 
 
function game_details:OnKeyTyped( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER	then
		game_details:HandleEnter( graphic )
		
	elseif code == ButtonCode.KEY_UP then 
		game_details:HandleUp( graphic )
		
	elseif code == ButtonCode.KEY_DOWN then
		game_details:HandleDown( graphic )
			
	elseif code == ButtonCode.KEY_LEFT then
		game_details:HandleLeft( graphic )
	
	elseif code == ButtonCode.KEY_RIGHT then
		game_details:HandleRight( graphic )
		
	end
end



function game_details:OnMouseClicked( params )
	game_details:HandleEnter( params.graphic )	
end



function game_details:OnButtonReleased( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_XBUTTON_A then
		game_details:HandleEnter( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_B then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_GAMELIB )
			
	elseif code == ButtonCode.KEY_XBUTTON_RIGHT then
		game_details:HandleRight( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_LEFT then
		game_details:HandleLeft( graphic )
		
	elseif code == ButtonCode.KEY_XBUTTON_DOWN then
		game_details:HandleDown( graphic )		
	
	elseif code == ButtonCode.KEY_XBUTTON_UP then		
		game_details:HandleUp( graphic )
	end		
end


	



