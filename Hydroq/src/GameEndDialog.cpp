#include "GameEndDialog.h"

void GameEndDialog::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED);
	model = GETCOMPONENT(PlayerModel);
}

void GameEndDialog::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		auto sceneContext = GETCOMPONENT(Stage);
		if (msg.GetContextNode()->GetTag().compare("ok_but") == 0) {
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
		}
	}
}

void GameEndDialog::Update(const uint64 delta, const uint64 absolute) {
	if (!firstInit && model->GameEnded()) {
		// stop all sounds
		auto sounds = CogGetPlayedSounds();
		for (auto sound : sounds) {
			sound->Stop();
		}

		auto msgNode = owner->GetScene()->FindNodeByTag("gameend_msg");
		auto player = GETCOMPONENT(PlayerModel);

		msgNode->GetMesh<Text>()->SetText(!player->PlayerWin() ? "You lost the game!" : "You win the game!");
		firstInit = true;
	}
}