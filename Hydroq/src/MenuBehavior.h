#pragma once

#include "ofxCogMain.h"


class MenuBehavior : public Behavior {
public:
	MenuBehavior() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_BUTTON_CLICKED);
	}

	void OnMessage(Msg& msg) {
		
		if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			if (msg.GetSourceObject()->GetTag().compare("sgame_but") == 0) {
				// click on single game button -> switch scene
				auto sceneContext = GETCOMPONENT(Stage);

				auto scene = sceneContext->FindSceneByName("main_menu_map");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
			else if (msg.GetSourceObject()->GetTag().compare("multiplayer_but") == 0) {
				// click on multiplayer button -> switch scene
				auto sceneContext = GETCOMPONENT(Stage);
				auto scene = sceneContext->FindSceneByName("multiplayer_map");
				sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

