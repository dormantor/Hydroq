#include "ConfirmDialog.h"


void ConfirmDialog::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED);
}

void ConfirmDialog::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		auto sceneContext = GETCOMPONENT(Stage);
		if (msg.GetContextNode()->GetTag().compare("yes_but") == 0) {

			// stop all sounds
			auto sounds = CogGetPlayedSounds();
			for (auto sound : sounds) {
				sound->Stop();
			}

			sceneContext->SwitchBackToScene(TweenDirection::NONE);
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
		}
		else if (msg.GetContextNode()->GetTag().compare("no_but") == 0) {
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
		}
	}
}