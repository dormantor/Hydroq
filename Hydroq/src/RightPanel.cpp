#include "RightPanel.h"
#include "Scene.h"

void RightPanel::OnInit() {
	SubscribeForMessages(ACT_STATE_CHANGED);

	scene = owner->GetScene();
	buildIconId = scene->FindNodeByTag("icon_build")->GetId();
	flagIconId = scene->FindNodeByTag("icon_flag")->GetId();

	buildSectionId = scene->FindNodeByTag("section_build")->GetId();
	flagSectionId = scene->FindNodeByTag("section_flag")->GetId();
}

void RightPanel::OnStart() {
	SelectBuildSection();
}

void RightPanel::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_STATE_CHANGED) && msg.GetContextNode()->HasState(StrId(STATES_SELECTED))) {
		if (msg.GetContextNode()->GetId() == buildIconId) {
			SelectBuildSection();
		}
		else if (msg.GetContextNode()->GetId() == flagIconId) {
			SelectCommandSection();
		}
	}
}

void RightPanel::SelectBuildSection() {
	UnSelectNodes();
	selectedSection = Section::BUILD;
	scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::RUNNING);
}

void RightPanel::SelectCommandSection() {
	UnSelectNodes();
	selectedSection = Section::COMMAND;
	scene->FindNodeById(flagSectionId)->SetRunningMode(RunningMode::RUNNING);
}

void RightPanel::UnSelectNodes() {
	scene->FindNodeById(buildSectionId)->SetRunningMode(RunningMode::DISABLED);
	scene->FindNodeById(flagSectionId)->SetRunningMode(RunningMode::DISABLED);
}