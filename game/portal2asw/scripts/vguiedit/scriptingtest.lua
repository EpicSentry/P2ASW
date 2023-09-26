--package.path = "./?.lua;../modules/?.lua";
GameUIConstants = require( "gameui_constants" );
ButtonCode = require( "buttoncode" );


local chapters = {  "particles/feather01",
					"signage/camera_manual",
					"photos/gnome",
					"signage/size_manual",
					"photos/camera", 
					"photos/gnome" }
local timer = 0;
local chapterIndex = 1;

scriptingtest = {}

function scriptingtest:Version( )
	return 4639;	
end


function scriptingtest:CreateText1()
	c:CreateGraphic( "text", { name = "TESTTEXT",
								["setcenter"] = { x = 0, y = 0 },
								["setscale"] = { x = .5, y = .5 },
								["setrotation"] = { rotation = 10 },
								["setvisible"] = { visible = 1 },
								["settext"] = { text = "Text set at creation" }, 
								["setallcaps"] = { allcaps = 1 },
								["setfont"] = { fontname = "Large" },
								["setjustification"] = { justfication = 1 },
								["setcolor"] = { color = "178 82 22 255" }
								} )
	hText = c:CallScript( self.scripthandle, "FindGraphic", { graphic = "testText" } ).graphichandle;
	c:CallGraphic( hText, "settext", { 
						text = "Text set from script"
						} )
end

function scriptingtest:CreateText2()
	c:CreateGraphic( "text", { name = "buttontxt",
									["setcenter"] = { x = -300, y = -300 },
									["settext"] = { text = "Button text" },
									} )

end


function scriptingtest:CreateRect1()
	c:CreateGraphic( "rect", { name = "testRect",
									["setcenter"] = { x = -300, y = 300 },
									["setcolor"] = { color = "178 82 22 255" }
									} )

end

function scriptingtest:CreateRect2()
	c:CreateGraphic( "rect", { name = "buttonbg",
									["setcenter"] = { x = -300, y = -300 },
									["setscale"] = { x = 256, y = 128 },
									["setalias"] = { alias = "testrect2alias" },
									 } )

end

function scriptingtest:CreateRect3()
	c:CreateGraphic( "rect", { name = "testRect3",	
									["setcenter"] = { x = -300, y = -130 },
									["setscale"] = { x = 100, y = 200 }
									 } )

end


function scriptingtest:CreateHitArea1()
	c:CreateGraphic( "hitarea", { name = "buttonhitarea",
								["setcenter"] = { x = -300, y = -300 },
								["setscale"] = { x = 256, y = 128 },
								["setmouseleftclickedcommand"] = { command = "OnHitAreaClicked" }
								 } )						
end


function scriptingtest:OnLoad( params )
	print( "scriptingtest_OnLoad!" );
	self.scripthandle = params.scripthandle
	
	
	scriptingtest:CreateText1()						
	scriptingtest:CreateRect1()
	scriptingtest:CreateRect2()
	scriptingtest:CreateRect3();
	scriptingtest:CreateText2()
	scriptingtest:CreateHitArea1()
						
	c:CallScript( self.scripthandle, "setdynamictexture", { aliasname = "testrect2alias", texturename = chapters[chapterIndex] } )

	c:CallScript( self.scripthandle, "setdynamictexture", { aliasname = "testrect", texturename = chapters[chapterIndex] } )

end


function scriptingtest:OnHitAreaClicked()
	print( "scriptingtest_OnHitAreaClicked!" )
	c:LoadMenu( GameUIConstants.MAINMENU )
end

function scriptingtest:OnInit()
	print( "scriptingtest_OnInit!" )
end


function scriptingtest:OnUpdate()
	timer = timer + 1
	if timer > 1000 then
		timer = 0
		chapterIndex = chapterIndex + 1
		if  chapterIndex > 6  then
			chapterIndex = 1
		end
		c:CallScript( self.scripthandle, "setdynamictexture", { aliasname = "testrect2alias", texturename = chapters[chapterIndex] } )
	end
end













	



