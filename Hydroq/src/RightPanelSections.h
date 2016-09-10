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

	int selectedNodeId = -1;

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
				SelectNode(nodeBuildSeedbedId);
			}
			else if (targetId == nodeCommandBuildId) {
				SelectCommandBuild();
				SelectNode(nodeCommandBuildId);
			}
			else if (targetId == nodeCommandDestroyId) {
				SelectCommandDestroy();
				SelectNode(nodeCommandDestroyId);
			}
			else if (targetId == nodeCommandForbidId) {
				SelectCommandForbid();
				SelectNode(nodeCommandForbidId);
			}
			else if (targetId == nodeCommandGuardId) {
				SelectCommandGuard();
				SelectNode(nodeCommandGuardId);
			}
		}
	}

	void UnselectNode(int nodeId) {
		owner->GetScene()->FindNodeById(nodeId)->GetShape<spt<BoundingBox>>()->SetIsRenderable(false);
	}

	void SelectNode(int nodeId) {
		if (selectedNodeId != -1) {
			UnselectNode(selectedNodeId);
		}
		selectedNodeId = nodeId;
		owner->GetScene()->FindNodeById(nodeId)->GetShape<spt<BoundingBox>>()->SetIsRenderable(true);
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