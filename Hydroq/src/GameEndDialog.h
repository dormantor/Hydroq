#pragma once

#include "ofxCogMain.h"
#include "HydroqPlayerModel.h"

class GameEndDialog : public Behavior {	
	
	HydroqPlayerModel* model;

public:
	GameEndDialog() {

	}

	void OnInit() {


		SubscribeForMessages(ACT_BUTTON_CLICKED);
		model = GETCOMPONENT(HydroqPlayerModel);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			auto sceneContext = GETCOMPONENT(Stage);
			if (msg.GetSourceObject()->GetTag().compare("ok_but") == 0) {
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
				sceneContext->SwitchBackToScene(TweenDirection::NONE);
			}
		}
	}

	bool firstInit = false;
public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (!firstInit && model->GameEnded()) {
			// stop all sounds
			auto sounds = CogGetPlayedSounds();
			for (auto sound : sounds) {
				sound->Stop();
			}

			auto msgNode = owner->GetScene()->FindNodeByTag("gameend_msg");
			auto player = GETCOMPONENT(HydroqPlayerModel);
			
			msgNode->GetShape<spt<Text>>()->SetText(!player->PlayerWin() ? "You lost the game!" : "You win the game!");
			firstInit = true;
		}
	}
};