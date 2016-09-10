#pragma once

#include "ofxCogMain.h"

class RightPanelSections : public Behavior {
	OBJECT_PROTOTYPE(RightPanelSections)

	int buildNodeId;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED);
		buildNodeId = owner->GetScene()->FindNodeByTag("section_build")->GetId();
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			int targetId = msg.GetSourceObject()->GetId();

			if (targetId == buildNodeId) {
				SelectBuildNode();
			}
		}
	}

	void SelectBuildNode() {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};