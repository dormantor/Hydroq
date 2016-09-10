#pragma once

#include "ofxCogMain.h"
#include "HydroqGameModel.h"

class GameEndDialog : public Behavior {
	OBJECT_PROTOTYPE(GameEndDialog)

	
	
	HydroqGameModel* model;

	void OnInit() {
		RegisterListening(ACT_BUTTON_CLICKED);
		model = GETCOMPONENT(HydroqGameModel);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			auto sceneContext = GETCOMPONENT(Stage);
			if (msg.GetSourceObject()->GetTag().compare("gameend_msg") == 0) {
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
			}
		}
	}

	bool firstInit = false;
public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (!firstInit && model->GameEnded()) {
			auto msgNode = owner->GetScene()->FindNodeByTag("gameend_msg");
			auto player = GETCOMPONENT(HydroqPlayerModel);
			
			msgNode->GetShape<spt<Text>>()->SetText(player->GetBuildings() == 0 ? "You lost the game!" : "You win the game!");
			firstInit = true;
		}
	}
};