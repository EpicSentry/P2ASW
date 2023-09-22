--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

mainmenu = {}

function mainmenu:Version( )
	return 4639;	
end


function mainmenu:OnLoad( params )
	print( "mainmenu_OnLoad!" );
	self.scripthandle = params.scripthandle

	testnimbus = "layer_0_testnimbus:CDmeHitAreaGeometry_2"
	
	gamemode_campaign = "layer_1_gamemode:gamemode"
	gamemode_desc = "layer_1_gamemode:description"
	
	campaign_submenu = "campaign_submenu"
		
	-- request nuggets
	c:Nugget( "load:app" )
	c:Nugget( "load:playermanager" )
	c:Nugget( "load:sessions" )
	c:Nugget( "load:matchevents" )
	
	-- semaphores
	self.cbck1 = c:Nugget( "load:semaphore", { usename = "callback1" } )
	c:Nugget( "use:callback1:Configure", { signal = "OnCallback1", eventdata = "payload", gamemode = "MyGameMode" } )
	self.cbck2 = c:Nugget( "load:semaphore", { usename = "callback2" } )
	c:Nugget( "use:callback2:Configure", { signal = "OnCallback2" } )
	
	-- signal semaphores for a test
	print( "Signaling callback1:" )
	c:Nugget( "use:callback1:Signal", { xxx = "Signal-1 from mainmenu" } )
	print( "Signaling callback2:" )
	c:Nugget( "use:callback2:Signal", { yyy = "Signal-2 from mainmenu" } )
	
	-- now reference cbck1 and signal it
	c:Nugget( "ref:cbck1alias", self.cbck1 )
	print( "Signaling alias:" )
	c:Nugget( "use:cbck1alias:Signal", { xxx = "Signal for alias" } )
end

function mainmenu:OnCallback1( params )
	print( "mainmenu:OnCallback1" )
	print( params.gamemode )
	print( params.payload.xxx )
end

function mainmenu:OnCallback2( params )
	print( "mainmenu:OnCallback2" )
	print( params.eventdata.yyy )
end

function mainmenu:OnInit()
	print( "main_menu_OnInit!" )
	mainmenu:HideGameModeMenu()
	

	-- get some nugget data
	aResult = c:Nugget( "use:playermanager:GetLocalPlayer", {
			index = 0
		} )
	
	if aResult then
		print( "local player 0:" )
		print( aResult.name )
		print( aResult.xuid )
		print( aResult.index )
		print( aResult.state )
		print( "----" )
	end


	bResult = c:Nugget( "use:playermanager:GetLocalPlayer", {
			index = 3
		} )
	if bResult then
		print( "local player 3:" )
		print( bResult )
		print( "----" )
	end

end


function mainmenu:SetGameModeMenuVisible( bShow )
	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic", { graphic = campaign_submenu }
			).graphichandle,
		"SetVisible",
		{ visible = bShow }
		)
	
	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic", { graphic = gamemode_campaign }
			).graphichandle,
		"SetVisible",
		{ visible = not bShow }
		)

	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic", { graphic = gamemode_desc }
			).graphichandle,
		"SetVisible",
		{ visible = not bShow }
		)
end

function mainmenu:ShowGameModeMenu()
	self:SetGameModeMenuVisible( true )
end

function mainmenu:HideGameModeMenu()
	self:SetGameModeMenuVisible( false )
end


function mainmenu:OnKeyTyped( graphic, code )
	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER	then	
	elseif code == ButtonCode.KEY_UP then               
	elseif code == ButtonCode.KEY_DOWN then
	end
end


function mainmenu:OnMouseClicked( params )
	if  params.graphic == testnimbus then
		c:LoadMenu( GameUIConstants.MENUTYPE_MENU1 )
	end
end


function mainmenu:OnPlayCampaign()
	print( "OnPlayCampaign Function called" )
	mainmenu:ShowGameModeMenu();
end


function mainmenu:OnQuickMatch()
	print( "OnQuickMatch Function called" )
end

function mainmenu:OnPlayOnline()
	print( "OnPlayOnline Function called" )
end

function mainmenu:OnPlayWithFriends()
	print( "OnPlayWithFriends Function called" )
	if self.cbck1 == nil then
		c:LoadMenu( "campaignmenu" )
		return
	end

	local child = c:LoadMenu( "campaignmenu", { mainmenucbck = self.cbck1 } )
	if ( child and child.scripthandle ) then
		print( "Opened campaign menu " .. child.scripthandle )
		return
	end

	
	-- below is some test code with session commands
	c:Nugget( "use:sessions:CreateSession", {
			system = {
				network = "offline"
			},
			game = {
				map = "devtest"
			}
		} )
	-- peek at session details
	details = c:Nugget( "use:sessions:SessionSettings" )
	if details then  
		print( "--- session details ---" )
		print( details )
		print( details.system )
		print( details.system.network )
		print( details.system.access )
		print( details.game )
		print( details.game.map )
		print( details.game.state )
		print( details.game.missioninfo.version )
		print( details.members )
		print( details.members.numMachines )
		print( details.members.numPlayers )
		print( details.members.numSlots )
		print( details.members.machine0.player0.name )
		print( "---- end session details ---" )
	end
	-- call a session command
	self.extraPieceOfData = 234
	c:Nugget( "use:sessions:SessionCommand", {
		["Game::SampleCmd"] = { arg1 = "value" }
	} )
	-- close the session
	c:Nugget( "use:sessions:CloseSession", {
		Start = {}
	} )
	-- start the game
	c:Nugget( "use:sessions:SessionCommand", {
		Start = {}
	} )
end

mainmenu["Command::Game::SampleCmd"] = function(self, params)
	print( "Command::Game::SampleCmd" )
	print( params.arg1 )
	print( self.extraPieceOfData )
	print( self )
end

function mainmenu:OnMatchSessionUpdate( params )
	print( "mainmenu:OnMatchEvent_OnMatchSessionUpdate {" )
	for pkey,pvalue in pairs(params) do
		print( "   ", pkey, " = ", pvalue )
	end
	print( "}" )
end

function mainmenu:OnEngineDisconnectReason( params )
	print( "mainmenu:OnMatchEvent_OnEngineDisconnectReason {" )
	for pkey,pvalue in pairs(params) do
		print( "   ", pkey, " = ", pvalue )
	end
	print( "}" )
end

function mainmenu:OnPlayOnSteam()
	print( "OnPlayOnSteam Function called" )
	
	-- test, load a dyanmic menu.
	c:LoadMenu( "scriptingtest", { mainmenucbck = self.cbck1 } )
end




function mainmenu:OnQuit()
	print( "OnQuit Function called" )
	mainmenu:HideGameModeMenu()
	c:Nugget( "use:app:Quit" )
end












	



