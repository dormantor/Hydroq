#pragma once

#include "ofxCogMain.h"

enum class Section {
	NONE, BUILD, FLAG, OTHER
};

class RightPanel : public Behavior {
	OBJECT_PROTOTYPE(RightPanel)

	Section selectedSection = Section::NONE;
	int buildIconId;
	int flagIconId;
	int otherIconId;

	int buildSectionId;
	int flagSectionId;
	int otherSectionId;
	
	Scene* scene;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_STATE_CHANGED);
		
		scene = owner->GetScene();
		buildIconId = scene->FindNodeByTag("icon_build")->GetId();
		flagIconId = scene->FindNodeByTag("icon_flag")->GetId();
		otherIconId = scene->FindNodeByTag("icon_oth")->GetId();

		scene->FindNodeById(buildIconId)->SetState(StringHash(STATES_SELECTED));
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_STATE_CHANGED && msg.GetSourceObject()->HasState(StringHash(STATES_SELECTED))) {
			if (msg.GetSourceObject()->GetId() == buildIconId) {
				SelectBuildSection();
			}
			else if (msg.GetSourceObject()->GetId() == flagIconId) {
				SelectFlagSection();
			}
			else if (msg.GetSourceObject()->GetId() == otherIconId) {
				SelectOtherSection();
			}
		}
	}

	void SelectBuildSection() {
		UnSelectNodes();
		selectedSection = Section::BUILD;
	}

	void SelectFlagSection() {
		UnSelectNodes();
		selectedSection = Section::FLAG;
	}

	void SelectOtherSection() {
		UnSelectNodes();
		selectedSection = Section::OTHER;
	}

	void UnSelectNodes() {
		scene->FindNodeById(buildIconId)->GetStates().ResetState(StringHash(STATES_SELECTED));
		scene->FindNodeById(flagIconId)->GetStates().ResetState(StringHash(STATES_SELECTED));
		scene->FindNodeById(otherIconId)->GetStates().ResetState(StringHash(STATES_SELECTED));
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};