#pragma once

#include "ofxCogMain.h"


class MenuBehavior : public Behavior {
	OBJECT_PROTOTYPE(MenuBehavior)

		void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED);
	}

	void OnMessage(Msg& msg) {

		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			if (msg.GetSourceObject()->GetTag().compare("sgame_but") == 0) {
				// click on single game button -> switch scene
				auto sceneContext = GETCOMPONENT(Stage);

				auto scene = sceneContext->FindSceneByName("main_menu_map");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
			else if (msg.GetSourceObject()->GetTag().compare("play_but") == 0) {
				// click on play button -> switch scene
				auto sceneContext = GETCOMPONENT(Stage);
				auto scene = sceneContext->FindSceneByName("game");
				sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

