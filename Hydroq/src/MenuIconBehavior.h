#pragma once

#include "ofxCogMain.h"
#include "Scene.h"
#include "HydroqPlayerModel.h"

class MenuIconBehavior : public Behavior {
	OBJECT_PROTOTYPE(MenuIconBehavior)

	
	spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;

	void OnInit() {
		RegisterListening(ACT_BUTTON_CLICKED, ACT_TRANSFORM_ENDED);
	}

	void OnStart() {
		Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
		originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(94.3f, 0), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(1), CalcType::LOC, 0));
		animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(100, 0), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_TRANSFORM_ENDED) && msg.GetSourceObject()->GetTag().compare("rightpanel") == 0) {
			owner->ResetState(StringHash(STATES_LOCKED));
		}

		if (msg.HasAction(ACT_BUTTON_CLICKED) && msg.GetSourceObject()->GetId() == owner->GetId()) {
			if (!owner->HasState(StringHash(STATES_LOCKED))) {
				owner->SetState(StringHash(STATES_LOCKED));

				if (owner->HasState(StringHash(STATES_ENABLED))) {
					owner->ResetState(StringHash(STATES_ENABLED));
					// roll menu back
					Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
					TransformAnim* anim = new TransformAnim(animTrans, originTrans, 250, 0, false, AnimBlend::OVERLAY);
					menu->AddBehavior(anim);
					/*
					// if user selected GATHER action, unselect it 
					auto playerModel = GETCOMPONENT(HydroqPlayerModel);
					if (playerModel->GetHydroqAction() == HydroqAction::ATTRACT) {
						playerModel->SetHydroqAction(HydroqAction::NONE);
					}*/
				}
				else {
					owner->SetState(StringHash(STATES_ENABLED));

					// roll the menu
					Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
					TransformAnim* anim = new TransformAnim(originTrans, animTrans, 250, 0, false, AnimBlend::OVERLAY);
					menu->AddBehavior(anim);
				}
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};