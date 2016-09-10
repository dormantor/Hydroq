#include "ConfirmDialog.h"
#include "Soundfx.h"
#include "Msg.h"
#include "Tween.h"
#include "Facade.h"
#include "Node.h"
#include "ComponentStorage.h"
#include "Stage.h"

void ConfirmDialog::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED);
}

void ConfirmDialog::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		auto sceneContext = GETCOMPONENT(Stage);
		if (msg.GetContextNode()->GetTag().compare("yes_but") == 0) {
			
			// click on yes button, switch twice (first switch closes dialog)
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
		}
		else if (msg.GetContextNode()->GetTag().compare("no_but") == 0) {
			// click on no button, close the dialog
			sceneContext->SwitchBackToScene(TweenDirection::NONE);
		}
	}
}