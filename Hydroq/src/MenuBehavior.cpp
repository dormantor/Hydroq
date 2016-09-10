#include "MenuBehavior.h"
#include "Node.h"
#include "ComponentStorage.h"
#include "Tween.h"
#include "Stage.h"

void MenuBehavior::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED);
}

void MenuBehavior::OnMessage(Msg& msg) {

	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		if (msg.GetContextNode()->GetTag().compare("sgame_but") == 0) {
			// click on single game button
			auto sceneContext = GETCOMPONENT(Stage);
			// switch scene to main_menu_map with map and faction selection
			auto scene = sceneContext->FindSceneByName("main_menu_map");
			sceneContext->SwitchToScene(scene, TweenDirection::NONE);
		}
		else if (msg.GetContextNode()->GetTag().compare("multiplayer_but") == 0) {
			// click on multiplayer button
			auto sceneContext = GETCOMPONENT(Stage);
			// switch scene to multiplayer_map 
			auto scene = sceneContext->FindSceneByName("multiplayer_map");
			sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
}