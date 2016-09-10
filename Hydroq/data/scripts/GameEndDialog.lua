 GameEndDialog = { 
   firstInit = false,
   model = nil,
   OnInit = function() 
     GameEndDialog_beh:SubscribeForMessages(StrId("BUTTON_CLICKED"))
	 model = HydroqGetPlayerModel()
   end, 
   OnMessage = function(msg) 
     if(msg:HasAction(StrId("BUTTON_CLICKED")))
	 then
	   if(msg.contextNode.tag == "ok_but")
	   then
	     CogSwitchBackToScene("none")
		 CogSwitchBackToScene("none")
	   end
	 end
   end, 
   Update = function(delta, absolute)
     if(not firstInit and model:GameEnded())
	 then
	   CogStopAllSounds();
	   scene = GameEndDialog_beh:GetScene()
	   msgNode = scene:FindNodeByTag("gameend_msg")
	   playerWin = model:PlayerWin()
	   
	   meshText = msgNode:GetTextMesh()
	   if(playerWin)
	   then
	     meshText:SetText("You have won the game!")
	   else
	     meshText:SetText("You have lost the game!")
	   end
	   
	 end
   end 
} 

GameEndDialog_beh = Behavior() 
GameEndDialog_beh:Register(GameEndDialog, "GameEndDialog")