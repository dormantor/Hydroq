#include "MenuBehavior.h"


void MenuBehavior::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED);
}

void MenuBehavior::OnMessage(Msg& msg) {

	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		if (msg.GetContextNode()->GetTag().compare("sgame_but") == 0) {
			// click on single game button -> switch scene
			auto sceneContext = GETCOMPONENT(Stage);

			auto scene = sceneContext->FindSceneByName("main_menu_map");
			sceneContext->SwitchToScene(scene, TweenDirection::NONE);
		}
		else if (msg.GetContextNode()->GetTag().compare("multiplayer_but") == 0) {
			// click on multiplayer button -> switch scene
			auto sceneContext = GETCOMPONENT(Stage);
			auto scene = sceneContext->FindSceneByName("multiplayer_map");
			sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
}