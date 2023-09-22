--package.path = "./?.lua;../modules/?.lua";

loadingbar = {}

function loadingbar:Version( )
	return 4639;	
end


function loadingbar:OnLoad( params )
	c:Nugget( "load:loadingprogress" );
	self.scripthandle = params.scripthandle
	self.hLoadingBar = c:CallScript( self.scripthandle, "FindGraphic", { graphic = "progressbar" } ).graphichandle
end


function loadingbar:OnInit()
	print( "loadingbar_OnInit!" )
end


function loadingbar:OnLevelLoadingProgress( params )
	c:CallGraphic( self.hLoadingBar, "SetState", {
		state = "progress",
		play = 0,
		time = 100 * params.progress
		} )
end
	

