#pragma once

#include "ofxCogMain.h"

class ConfirmDialog : public Behavior {
	
public:
	ConfirmDialog() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_BUTTON_CLICKED);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			auto sceneContext = GETCOMPONENT(Stage);
			if (msg.GetSourceObject()->GetTag().compare("yes_but") == 0) {
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
			}
			else if (msg.GetSourceObject()->GetTag().compare("no_but") == 0) {
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};