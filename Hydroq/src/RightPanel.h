#pragma once

#include "ofxCogMain.h"

enum class Section {
	NONE, BUILD, COMMAND, OTHER
};

class RightPanel : public Behavior {
	
	Section selectedSection = Section::NONE;
	int buildIconId;
	int flagIconId;

	int buildSectionId;
	int flagSectionId;
	
	Scene* scene;

public:

	RightPanel() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_STATE_CHANGED);
		
		scene = owner->GetScene();
		buildIconId = scene->FindNodeByTag("icon_build")->GetId();
		flagIconId = scene->FindNodeByTag("icon_flag")->GetId();

		buildSectionId = scene->FindNodeByTag("section_build")->GetId();
		flagSectionId = scene->FindNodeByTag("section_flag")->GetId();
	}

	void OnStart() {
		SelectBuildSection();
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_STATE_CHANGED) && msg.GetContextNode()->HasState(StrId(STATES_SELECTED))) {
			if (msg.GetContextNode()->GetId() == buildIconId) {
				SelectBuildSection();
			}
			else if (msg.GetContextNode()->GetId() == flagIconId) {
				SelectFlagSection();
			}
		}
	}

	void SelectBuildSection() {
		UnSelectNodes();
		selectedSection = Section::BUILD;
		scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::RUNNING);
	}

	void SelectFlagSection() {
		UnSelectNodes();
		selectedSection = Section::COMMAND;
		scene->FindNodeById(flagSectionId)->SetRunningMode(RunningMode::RUNNING);
	}

	void UnSelectNodes() {
		scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::DISABLED);
		scene->FindNodeById(flagSectionId)->SetRunningMode(RunningMode::DISABLED);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};