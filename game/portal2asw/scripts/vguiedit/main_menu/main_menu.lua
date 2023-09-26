--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

local currentMenuItem = 0;
main_menu = {}

function main_menu:Version( )
	return 4639;	
end


function main_menu:OnLoad( params )
	print( "main_menu_OnLoad!" );
	self.scripthandle = params.scripthandle
	
	currentMenuItem = 0;
	
	-- blah we need these so we can change the fonts on them.
	menuitemtxt1 = "menubutton1:menuitemtxt_Instance"
	menuitemtxt2 = "menubutton2:menuitemtxt_Instance"
	menuitemtxt3 = "menubutton3:menuitemtxt_Instance"
	menuitemtxt4 = "menubutton4:menuitemtxt_Instance"
	menuitemtxt5 = "menubutton5:menuitemtxt_Instance"
	menuitemtxt6 = "menubutton6:menuitemtxt_Instance"
	menuitemtxt7 = "menubutton7:menuitemtxt_Instance"
	menuitemtxt8 = "menubutton8:menuitemtxt_Instance"
	menuitemtxt9 = "menubutton9:menuitemtxt_Instance"
	menuitemtxts  = { menuitemtxt1, menuitemtxt2, menuitemtxt3, menuitemtxt4, menuitemtxt5, menuitemtxt6, menuitemtxt7, menuitemtxt8, menuitemtxt9 }

	-- these are the hitareas inside the groups. 
	button1 = "menubutton1:button1_Instance" 
	button2 = "menubutton2:button1_Instance" 
	button3 = "menubutton3:button1_Instance" 
	button4 = "menubutton4:button1_Instance" 
	button5 = "menubutton5:button1_Instance" 
	button6 = "menubutton6:button1_Instance" 
	button7 = "menubutton7:button1_Instance" 
	button8 = "menubutton8:button1_Instance" 
	button9 = "menubutton9:button1_Instance" 
	buttons  = { button1, button2, button3, button4, button5, button6, button7, button8, button9 };

	-- These are the menu item sizes going away from the selected item.
	menuitemfontnames = { "Small48", "Small36", "Small24", "Small20", "Small16", "Small16", "Small16", "Small16", "Small16", "Small16" };
		
	-- start off with gamecollectiontxt selected, center item.
	main_menu:InitList( 5 )	
end

function main_menu:SetFontHelper( szGraphicName, szFontName )
	-- !!!!!!!!!!!!!!!!!!!!!!
	-- HACK: never do this! Always find your graphic controls and store their handles in OnLoad function!
	-- !!!!!!!!!!!!!!!!!!!!!!
	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic",
			{ graphic = szGraphicName }
			).graphichandle,
		"SetFont",
		{ fontname = szFontName }
		)
end

function main_menu:RequestFocusHelper( szGraphicName, szSoundName )
	-- !!!!!!!!!!!!!!!!!!!!!!
	-- HACK: never do this! Always find your graphic controls and store their handles in OnLoad function!
	-- !!!!!!!!!!!!!!!!!!!!!!
	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic",
			{ graphic = szGraphicName }
			).graphichandle,
		"RequestFocus",
		{ sound = szSoundName }
		)
end

function main_menu:InitList( centerMenuItem )
	
	-- Set centerMenuItem selected
	self:SetFontHelper( menuitemtxts[centerMenuItem], menuitemfontnames[1] )		

	-- unselect all others
	local i = centerMenuItem + 1
	local j = 2;
	while menuitemtxts[i] do	
		self:SetFontHelper( menuitemtxts[i], menuitemfontnames[j] )
		i = i + 1;
		j = j + 1;
	end
	
	
	i = centerMenuItem - 1
	j = 2;
	while menuitemtxts[i] do	
		self:SetFontHelper( menuitemtxts[i], menuitemfontnames[j] )
		j = j + 1;
		i = i - 1;
	end
	
	currentMenuItem = centerMenuItem;	
end


function main_menu:OnInit()
	print( "main_menu_OnInit!" );
	self:RequestFocusHelper( buttons[currentMenuItem], "" )	
end


function main_menu:LayoutList( centerMenuItem, bPlaySound )
	print "main_menu:LayoutList"	
	self:SetFontHelper( menuitemtxts[centerMenuItem], menuitemfontnames[1] )		

	-- unselect all others
	local i = centerMenuItem + 1
	local j = 2
	while menuitemtxts[i] do	
		self:SetFontHelper( menuitemtxts[i], menuitemfontnames[j] )
		i = i + 1
		j = j + 1
	end
	
	
	i = centerMenuItem - 1
	j = 2
	while menuitemtxts[i] do	
		self:SetFontHelper( menuitemtxts[i], menuitemfontnames[j] )
		j = j + 1
		i = i - 1
	end
	
	currentMenuItem = centerMenuItem	
end


function main_menu:OnUpdate()
end


function main_menu:OnKeyTyped( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER	then
		if graphic == buttons[5] then 
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_GAMELIB )
		end
		
	elseif code == ButtonCode.KEY_UP then
		if graphic ~= buttons[1] then
			self:RequestFocusHelper( buttons[currentMenuItem - 1], GameUIConstants.BUTTONMOVE_SOUND )
		end 	
			               
	elseif code == ButtonCode.KEY_DOWN then
		if graphic ~= buttons[9] then
			self:RequestFocusHelper( buttons[currentMenuItem + 1], GameUIConstants.BUTTONMOVE_SOUND )
		end	
	end
end


function main_menu:OnMouseClicked( params )
	graphic = params.graphic
	code = params.code

	if graphic == buttons[5] then 
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_GAMELIB )
	end	
end


-- this isn't great habit to get into, responding to focus events.
-- it is here so you can change other graphics in response to mouse focus.
function main_menu:OnFocusGained( graphic )
	print "main_menu:OnFocusGained"
	i = 1
	newMenuItem = -1;
	while buttons[i] do	
		print( graphic )
		print( buttons[i] )
		if graphic == buttons[i] then
			newMenuItem = i
			break
		end	
		i = i + 1
	end
	
	if newMenuItem ~= -1 then
		main_menu:LayoutList( newMenuItem )
	end	 
end


function main_menu:OnMouseFocusGained( params )
	print "main_menu:OnFocusGained"
	main_menu:OnFocusGained( params.graphic )
	-- in this menu we will move keyfocus to the mouse focused item when it changes.
	self:RequestFocusHelper( buttons[currentMenuItem ], GameUIConstants.BUTTONMOVE_SOUND ) 
end


function main_menu:OnMouseFocusLost( params )
end


function main_menu:OnKeyFocusGained( params )
	main_menu:OnFocusGained( params.graphic ) 
end


function main_menu:OnKeyFocusLost( params )
end


function main_menu:OnButtonReleased( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_XBUTTON_A then
		if graphic == buttons[5] then 
			c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
			c:LoadMenu( GameUIConstants.MENUTYPE_GAMELIB )
		end
		
	elseif code == ButtonCode.KEY_XBUTTON_B then
		c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONBACK_SOUND } )
		c:LoadMenu( GameUIConstants.MENUTYPE_HELLO )
		
	elseif code == ButtonCode.KEY_XBUTTON_UP then
		if graphic ~= buttons[1] then
			main_menu:LayoutList( currentMenuItem - 1 )
		end 
				               
	elseif code == ButtonCode.KEY_XBUTTON_DOWN then
		if graphic ~= buttons[table.getn( buttons )] then
			main_menu:LayoutList( currentMenuItem + 1 )
		end	
	end		
end

	



