#pragma once

#include "ofxCogMain.h"
#include "HydroqPlayerModel.h"


class RightPanelSections : public Behavior {
	OBJECT_PROTOTYPE(RightPanelSections)

	HydroqPlayerModel* playerModel;

	int nodeCommandBuildId;
	int nodeCommandDestroyId;
	int nodeCommandForbidId;
	int nodeCommandAttractId;

	int selectedNodeId = -1;

	void OnInit() {
		RegisterListening(ACT_OBJECT_HIT_ENDED);
		playerModel = GETCOMPONENT(HydroqPlayerModel);
		nodeCommandBuildId = owner->GetScene()->FindNodeByTag("command_build")->GetId();
		nodeCommandDestroyId = owner->GetScene()->FindNodeByTag("command_destroy")->GetId();
		nodeCommandForbidId = owner->GetScene()->FindNodeByTag("command_forbid")->GetId();
		nodeCommandAttractId = owner->GetScene()->FindNodeByTag("command_attract")->GetId();
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
			int targetId = msg.GetSourceObject()->GetId();

			if (targetId == selectedNodeId) {
				// user clicked twice -> unselect the node
				UnselectNode(targetId);
				playerModel->SetHydroqAction(HydroqAction::NONE);
				selectedNodeId = -1;
			}
			else {
				if (targetId == nodeCommandBuildId) {
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
				else if (targetId == nodeCommandAttractId) {
					SelectCommandAttract();
					SelectNode(nodeCommandAttractId);
				}
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

	void SelectCommandBuild() {
		playerModel->SetHydroqAction(HydroqAction::BUILD);
	}

	void SelectCommandDestroy() {
		playerModel->SetHydroqAction(HydroqAction::DESTROY);
	}

	void SelectCommandForbid() {
		playerModel->SetHydroqAction(HydroqAction::FORBID);
	}

	void SelectCommandAttract() {
		playerModel->SetHydroqAction(HydroqAction::ATTRACT);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};