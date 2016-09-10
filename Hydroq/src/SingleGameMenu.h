#pragma once

#include "ofxCogMain.h"


class SingleGameMenu : public Behavior {
	OBJECT_PROTOTYPE(SingleGameMenu)

	void OnInit() {
		RegisterListening(ACT_OBJECT_HIT_ENDED);
	}

	void OnMessage(Msg& msg) {
		
		if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
			 if (msg.GetSourceObject()->GetTag().compare("play_but") == 0) {
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

