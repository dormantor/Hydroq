#include "MenuIconBehavior.h"

void MenuIconBehavior::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED, ACT_TRANSFORM_ENDED);
}

void MenuIconBehavior::OnStart() {
	Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
	originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(94.3f, 0), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(1), CalcType::LOC, 0));
	animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(100, 0), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));
}

void MenuIconBehavior::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_TRANSFORM_ENDED) && msg.GetContextNode()->GetTag().compare("rightpanel") == 0) {
		owner->ResetState(StrId(STATES_LOCKED));
	}

	if (msg.HasAction(ACT_BUTTON_CLICKED) && msg.GetContextNode()->GetId() == owner->GetId()) {
		if (!owner->HasState(StrId(STATES_LOCKED))) {
			owner->SetState(StrId(STATES_LOCKED));

			if (owner->HasState(StrId(STATES_ENABLED))) {
				owner->ResetState(StrId(STATES_ENABLED));
				// roll menu back
				Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
				TransformAnim* anim = new TransformAnim(animTrans, originTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
				/*
				// if user selected GATHER action, unselect it
				auto playerModel = GETCOMPONENT(PlayerModel);
				if (playerModel->GetHydroqAction() == HydroqAction::ATTRACT) {
				playerModel->SetHydroqAction(HydroqAction::NONE);
				}*/
			}
			else {
				owner->SetState(StrId(STATES_ENABLED));

				// roll the menu
				Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
				TransformAnim* anim = new TransformAnim(originTrans, animTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
			}
		}
	}
}
