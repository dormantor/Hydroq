#pragma once

#include "ofxCogMain.h"


class MenuIconBehavior : public Behavior {
	OBJECT_PROTOTYPE(MenuIconBehavior)

		spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED, ACT_TRANSFORM_ENDED);

		Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
		originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(150, 6), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));
		animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(100, 6), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_TRANSFORM_ENDED && msg.GetSourceObject()->GetTag().compare("rightpanel") == 0) {
			owner->ResetState(StringHash(STATES_LOCKED));
		}


		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED && msg.GetSourceObject()->GetId() == owner->GetId()) {
			if (!owner->HasState(StringHash(STATES_LOCKED))) {
				owner->SetState(StringHash(STATES_LOCKED));

				if (owner->HasState(StringHash(STATES_ENABLED))) {
					owner->ResetState(StringHash(STATES_ENABLED));
					// roll menu back
					Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
					TransformAnim* anim = new TransformAnim(animTrans, originTrans, 250, 0, false, AnimBlend::OVERLAY);
					menu->AddBehavior(anim);
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