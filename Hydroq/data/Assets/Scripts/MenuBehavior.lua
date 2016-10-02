 MenuBehavior = { 
   OnInit = function() 
     MenuBehavior_beh:SubscribeForMessages(StrId("BUTTON_CLICKED")) 
   end, 
   OnMessage = function(msg) 
     if(msg:HasAction(StrId("BUTTON_CLICKED")))
	 then
	   if(msg.contextNode.tag == "sgame_but")
	   then
	     CogSwitchToScene("main_menu_map", "none")
	   end
	   
	   if(msg.contextNode.tag == "multiplayer_but")
	   then
	     CogSwitchToScene("multiplayer_map", "none")
	   end
	 end
   end, 
   Update = function(delta, absolute) 
   end 
} 

MenuBehavior_beh = Behavior() 
MenuBehavior_beh:Register(MenuBehavior, "MenuBehavior")