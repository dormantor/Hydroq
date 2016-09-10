#pragma once

#include "ofxCogMain.h"
#include "HydroqPlayerModel.h"


class RightPanelSections : public Behavior {
	OBJECT_PROTOTYPE(RightPanelSections)

	HydroqPlayerModel* playerModel;

	int nodeBuildSeedbedId;
	int nodeCommandBuildId;
	int nodeCommandDestroyId;
	int nodeCommandForbidId;
	int nodeCommandGuardId;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED);
		playerModel = GETCOMPONENT(HydroqPlayerModel);
		nodeBuildSeedbedId = owner->GetScene()->FindNodeByTag("build_seedbed")->GetId();
		nodeCommandBuildId = owner->GetScene()->FindNodeByTag("command_build")->GetId();
		nodeCommandDestroyId = owner->GetScene()->FindNodeByTag("command_destroy")->GetId();
		nodeCommandForbidId = owner->GetScene()->FindNodeByTag("command_forbid")->GetId();
		nodeCommandGuardId = owner->GetScene()->FindNodeByTag("command_guard")->GetId();
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			int targetId = msg.GetSourceObject()->GetId();

			if (targetId == nodeBuildSeedbedId) {
				SelectBuildSeedbed();
			}
			else if (targetId == nodeCommandBuildId) {
				SelectCommandBuild();
			}
			else if (targetId == nodeCommandDestroyId) {
				SelectCommandDestroy();
			}
			else if (targetId == nodeCommandForbidId) {
				SelectCommandForbid();
			}
			else if (targetId == nodeCommandGuardId) {
				SelectCommandGuard();
			}
		}
	}

	void SelectBuildSeedbed() {
		playerModel->SetHydroqAction(HydroqAction::SEEDBED);
	}

	void SelectCommandBuild() {
		playerModel->SetHydroqAction(HydroqAction::BUILD);
	}

	void SelectCommandDestroy() {
		playerModel->SetHydroqAction(HydroqAction::DESTROY);
	}

	void SelectCommandForbid() {
		playerModel->SetHydroqAction(HydroqAction::FORBID);
	}

	void SelectCommandGuard() {
		playerModel->SetHydroqAction(HydroqAction::GUARD);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};