 ConfirmDialog = { 
   OnInit = function() 
     ConfirmDialog_beh:SubscribeForMessages(StrId("BUTTON_CLICKED")) 
   end, 
   OnMessage = function(msg) 
     if(msg:HasAction(StrId("BUTTON_CLICKED")))
	 then
	   if(msg.contextNode.tag == "yes_but")
	   then
	     CogSwitchBackToScene("none")
		 CogSwitchBackToScene("none")
	   end
	   
	   if(msg.contextNode.tag == "no_but")
	   then
	     CogSwitchBackToScene("none")
	   end
	 end
   end, 
   Update = function(delta, absolute) 
   end 
} 

ConfirmDialog_beh = Behavior() 
ConfirmDialog_beh:Register(ConfirmDialog, "ConfirmDialog")