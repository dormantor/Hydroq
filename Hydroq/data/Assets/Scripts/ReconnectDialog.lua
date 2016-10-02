 ReconnectDialog = { 
   OnInit = function() 
     ReconnectDialog_beh:SubscribeForMessages(StrId("BUTTON_CLICKED")) 
	 ReconnectDialog_beh:SubscribeForMessages(StrId("NET_CONNECTED")) 
   end, 
   OnMessage = function(msg) 
     if(msg:HasAction(StrId("BUTTON_CLICKED")))
	 then
	   if(msg.contextNode.tag == "disconnect_but")
	   then
	     CogSwitchBackToScene("none")
		 CogSwitchBackToScene("none")
	   end
	 end
	 if(msg:HasAction(StrId("NET_CONNECTED")))
	 then
	   CogSwitchBackToScene("none")
	 end
   end, 
   Update = function(delta, absolute) 
   end 
} 

ReconnectDialog_beh = Behavior() 
ReconnectDialog_beh:Register(ReconnectDialog, "ReconnectDialog")