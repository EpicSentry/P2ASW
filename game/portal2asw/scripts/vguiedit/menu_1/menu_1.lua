--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );


menu_1 = {}

function menu_1:Version( )
	return 4639;
end

function menu_1:OnLoad( params )
	print( "menu_1_OnLoad!" );
	self.scripthandle = params.scripthandle
		
	stage = "stage" 
	cloudbg = "cloudbg" 
	nimbussharp = "nimbussharp" 
	nimbusblur = "nimbusblur" 
	entertainmentText = "entertainment" 
	steamlogo = "steamlogo" 
	
	-- set up hit area to catch keys
	inputarea = "CDmeHitAreaGeometry_4"
	
	c:CallScript( self.scripthandle, "InitAnims" )
	
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = cloudbg }
				).graphichandle,
			"SetState",
			{ state = "fadein" }
		)

	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = nimbussharp }
				).graphichandle,
			"SetState",
			{ state = "fadein" }
		)

	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = nimbusblur }
				).graphichandle,
			"SetState",
			{ state = "fadein" }
		)

	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = entertainmentText }
				).graphichandle,
			"SetState",
			{ state = "fadein" }
		)

	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = steamlogo }
				).graphichandle,
			"SetState",
			{ state = "fadein" }
		)

	c:CallScript( 0, "Sound", { sound = GameUIConstants.STARTUP_SOUND } )
end

function menu_1:OnInit()
	print( "menu_1_OnInit!" );
	c:CallGraphic(
		c:CallScript( self.scripthandle,
			"FindGraphic",
			{ graphic = inputarea }
			).graphichandle,
		"RequestFocus"
		)	
end

function menu_1:OnUpdate( )
end

function menu_1:OnExit()
	print( "menu_1_Exit!" )
	c:CallScript( 0, "Sound", {
		sound = GameUIConstants.STARTUP_SOUND,
		play = 0
		} )
	
	c:CallGraphic(
			c:CallScript( self.scripthandle,
					"FindGraphic",
					{ graphic = stage }
				).graphichandle,
			"SetState",
			{ state = "fadeout" }
		)
end

function menu_1:HandleEnter()
	c:CallScript( 0, "Sound", { sound = GameUIConstants.BUTTONNEXT_SOUND } )
	c:LoadMenu( GameUIConstants.MENUTYPE_HELLO )
end

function menu_1:OnKeyTyped( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_ENTER or code == ButtonCode.KEY_PAD_ENTER then
		menu_1:HandleEnter()	
	end
end

function menu_1:OnMouseClicked( params )
	menu_1:HandleEnter()
end

function menu_1:OnButtonReleased( params )
	graphic = params.graphic
	code = params.code

	if code == ButtonCode.KEY_XBUTTON_A then
		menu_1:HandleEnter()
	end
end




	



