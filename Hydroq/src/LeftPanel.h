#pragma once

#include "ofxCogMain.h"


class LeftPanel : public Behavior {
	OBJECT_PROTOTYPE(LeftPanel)

	spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;

	void OnInit() {
		RegisterListening(ACT_OBJECT_HIT_ENDED, ACT_TRANSFORM_ENDED);
	}

	void OnStart() {
		originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(0, 0), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(1), CalcType::LOC, 0));
		animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(6.0f, 0), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));

		owner->SetState(StringHash(STATES_ENABLED));
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_OBJECT_HIT_ENDED) && msg.GetSourceObject()->GetId() == owner->GetId()) {
			if (owner->HasState(StringHash(STATES_ENABLED))) {
				owner->ResetState(StringHash(STATES_ENABLED));
				// roll menu back
				Node* menu = owner->GetScene()->FindNodeByTag("leftpanel");
				TransformAnim* anim = new TransformAnim(originTrans, animTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
			}
			else {
				owner->SetState(StringHash(STATES_ENABLED));

				// roll the menu
				Node* menu = owner->GetScene()->FindNodeByTag("leftpanel");
				TransformAnim* anim = new TransformAnim(animTrans, originTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};