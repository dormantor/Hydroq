#include "RightPanelSections.h"
#include "Node.h"
#include "Scene.h"
#include "ComponentStorage.h"

void RightPanelSections::OnInit() {
	SubscribeForMessages(ACT_OBJECT_HIT_ENDED, ACT_FUNC_SELECTED);
	playerModel = GETCOMPONENT(PlayerModel);
	nodeCommandBuildId = owner->GetScene()->FindNodeByTag("command_build")->GetId();
	nodeCommandDestroyId = owner->GetScene()->FindNodeByTag("command_destroy")->GetId();
	nodeCommandForbidId = owner->GetScene()->FindNodeByTag("command_forbid")->GetId();
	nodeCommandAttractId = owner->GetScene()->FindNodeByTag("command_attract")->GetId();
}

void RightPanelSections::OnMessage(Msg& msg) {
	if (!locked) {
		locked = true;
		// if someone else has unselected the node
		if (msg.HasAction(ACT_FUNC_SELECTED) && selectedNodeId != -1) {
			if (playerModel->GetHydroqAction() == HydroqAction::NONE) {
				UnselectNode(selectedNodeId);
				selectedNodeId = -1;
			}
		}

		if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {

			int targetId = msg.GetContextNode()->GetId();

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

		locked = false;
	}
}

void RightPanelSections::SelectNode(int nodeId) {
	if (selectedNodeId != -1) {
		UnselectNode(selectedNodeId);
	}
	selectedNodeId = nodeId;
	owner->GetScene()->FindNodeById(nodeId)->GetMesh<BoundingBox>()->SetIsRenderable(true);
}

void RightPanelSections::UnselectNode(int nodeId) {
	owner->GetScene()->FindNodeById(nodeId)->GetMesh<BoundingBox>()->SetIsRenderable(false);
}

