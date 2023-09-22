--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );

campaignmenu = {}

function campaignmenu:Version( )
	return 4639;	
end


local selected_campaignoption = 1
local selected_diffoption = 1
local selected_permsoption = 1
local selected_serveroption = 1

function campaignmenu:OnLoad( params )
	print( "campaignmenu_OnLoad!" );
	self.scripthandle = params.scripthandle
	
	gamemode = "gamemodetxt"
	
	campaignbaroption = "campaign_bar:righttxt_Instance"
	hCampaignBar = c:CallScript( self.scripthandle, "FindGraphic", { graphic = campaignbaroption } ).graphichandle;
	
	campaignoptions = { "option1", "option2", "option3" }
	selected_campaignoption = 1
	c:CallGraphic( hCampaignBar, "SetText", { 
						text = campaignoptions[selected_campaignoption] 
						} )
						
	
	difficultybaroption = "difficulty_bar:righttxt_Instance"
	hDifficultyBar = c:CallScript( self.scripthandle, "FindGraphic", { graphic = difficultybaroption } ).graphichandle;
	
	difficultyoptions = { "Easy", "Normal", "Hard" }
	selected_diffoption = 1					
	
	c:CallGraphic( hDifficultyBar, "SetText", { 
						text = difficultyoptions[selected_diffoption] 
						} )
	
	
	permissionsbaroption = "permissions_bar:righttxt_Instance"
	hPermissionsBar = c:CallScript( self.scripthandle, "FindGraphic", { graphic = permissionsbaroption } ).graphichandle;
	-- localization test.
	permissionoptions = { "#Portal2_Rosette_Box_PickUp", 
						"#Portal2_Rosette_Box_PutDown", 
						"#Portal2_Rosette_Go_Here", 
						"#Portal2_Rosette_Portal_Enter",
						"#Portal2_Rosette_Portal_Exit" }
	c:CallGraphic( hPermissionsBar, "SetText", { 
						text = permissionoptions[selected_permsoption] 
						} )

	
	servertypebaroption = "servertype_bar:righttxt_Instance"

	
	-- request nuggets
	c:Nugget( "load:app" )
	c:Nugget( "load:playermanager" )
	c:Nugget( "load:sessions" )
	
	
	-- ref semaphore nugget supplied from main menu, signal it and free it
	self.parentcallback = c:Nugget( "ref:parentcallback", params.mainmenucbck )
	c:Nugget( "use:parentcallback:EnableEvents", { scripthandle = self.scripthandle, enable = 0 } )
	print( "Signaling parent callback:" )
	c:Nugget( "use:parentcallback:Signal" )
	c:Nugget( "free:parentcallback" )
	self.parentcallback = nil
end

function campaignmenu:OnCallback1( params )
	print( "campaignmenu:OnCallback1" )
	print( params.gamemode )
	print( params.payload.xxx )
end

function campaignmenu:OnInit()
	print( "main_menu_OnInit!" )
	

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



function campaignmenu:OnKeyTyped( graphic, code )
	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER	then	
	elseif code == ButtonCode.KEY_UP then               
	elseif code == ButtonCode.KEY_DOWN then
	end
end


function campaignmenu:OnMouseClicked( params )
end



function campaignmenu:OnCampaign()
	print( "OnCampaign Function called" )
	selected_campaignoption = selected_campaignoption + 1
	if ( selected_campaignoption > table.getn(campaignoptions) ) then
		selected_campaignoption = 1
	end
		               
	c:CallGraphic( hCampaignBar, "SetText", { 
						text = campaignoptions[selected_campaignoption] 
						} ) 
end


function campaignmenu:OnDifficulty()
	print( "OnDifficulty Function called" )
	
	selected_diffoption = selected_diffoption + 1
	if ( selected_diffoption > table.getn(difficultyoptions) ) then
		selected_diffoption = 1
	end
		               
	c:CallGraphic( hDifficultyBar, "SetText", { 
						text = difficultyoptions[selected_diffoption] 
						} ) 
end

function campaignmenu:OnPermissions()
	print( "OnPermissions Function called" ) 
	selected_permsoption = selected_permsoption + 1
	if ( selected_permsoption > table.getn(permissionoptions) ) then
		selected_permsoption = 1
	end
		               
	c:CallGraphic( hPermissionsBar, "SetText", { 
						text = permissionoptions[selected_permsoption] 
						} ) 
end

function campaignmenu:OnServertype()
	print( "OnServertype Function called" )
end


function campaignmenu:OnCreateLobby()
	print( "OnCreateLobby Function called" )
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
		-- start the game
		c:Nugget( "use:sessions:SessionCommand", {
			Start = {}
		} )
end


function campaignmenu:OnBack()
	print( "OnBack Function called" )
	c:LoadMenu( GameUIConstants.MAINMENU )
end














	



