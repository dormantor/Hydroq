#pragma once

#include "ofxCogMain.h"

enum class Section {
	NONE, BUILD, COMMAND, OTHER
};

class RightPanel : public Behavior {
	OBJECT_PROTOTYPE(RightPanel)

	Section selectedSection = Section::NONE;
	int buildIconId;
	int flagIconId;
	int otherIconId;

	int buildSectionId;
	int commandSectionId;
	int otherSectionId;
	
	Scene* scene;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_STATE_CHANGED);
		
		scene = owner->GetScene();
		buildIconId = scene->FindNodeByTag("icon_build")->GetId();
		flagIconId = scene->FindNodeByTag("icon_flag")->GetId();
		otherIconId = scene->FindNodeByTag("icon_oth")->GetId();

		buildSectionId = scene->FindNodeByTag("section_build")->GetId();
		commandSectionId = scene->FindNodeByTag("section_command")->GetId();

		SelectBuildSection();
	}

	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_STATE_CHANGED && msg.GetSourceObject()->HasState(StringHash(STATES_SELECTED))) {
			if (msg.GetSourceObject()->GetId() == buildIconId) {
				SelectBuildSection();
			}
			else if (msg.GetSourceObject()->GetId() == flagIconId) {
				SelectCommandSection();
			}
			else if (msg.GetSourceObject()->GetId() == otherIconId) {
				SelectOtherSection();
			}
		}
	}

	void SelectBuildSection() {
		UnSelectNodes();
		selectedSection = Section::BUILD;
		scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::RUNNING);
	}

	void SelectCommandSection() {
		UnSelectNodes();
		selectedSection = Section::COMMAND;
		scene->FindNodeById(commandSectionId)->SetRunningMode(RunningMode::RUNNING);
	}

	void SelectOtherSection() {
		UnSelectNodes();
		selectedSection = Section::OTHER;
		scene->FindNodeById(commandSectionId)->SetRunningMode(RunningMode::RUNNING);
	}

	void UnSelectNodes() {
		scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::DISABLED);
		scene->FindNodeById(commandSectionId)->SetRunningMode(RunningMode::DISABLED);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};