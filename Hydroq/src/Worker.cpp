#include "Worker.h"
#include "HydroqGameModel.h"


void WorkerIdleState::Update(const uint64 delta, const uint64 absolute) {
	if (CogGetFrameCounter() % 10 == 0) {
		// each 10 frame check if this is something to do...
		auto model = GETCOMPONENT(HydroqGameModel);
		for (auto& task : model->GetGameTasks()) {
			if (!task->isProcessing && !task->isEnded && task->taskSh == StringHash(TASK_BRIDGE_BUILD)) {
				auto position = task->taskNode->GetTransform().localPos;
				
				auto map = model->GetMap();
				auto mapNode = map->GetNode((int)position.x, (int)position.y);
				auto start = owner->GetTransform().localPos;
				auto nodeToBuildfrom = mapNode->FindNeighborByType(MapNodeType::GROUND, Vec2i(start.x, start.y));

				if (nodeToBuildfrom != nullptr) {

					CogLogDebug("Worker", "Got task for bridge building at position [%d,%d]", position.x, position.y);

					auto stateToChange = GetParent()->FindLocalState(StringHash(STATE_WORKER_BUILD));
					static_cast<WorkerBridgeBuildState*>(stateToChange)->SetGameTask(task);
					this->GetParent()->ChangeState(StringHash(STATE_WORKER_BUILD));
				}
				break;
			}
		}
	}
}


void WorkerBridgeBuildState::EnterState() {
	task->handlerNode = owner;
	task->isProcessing = true;

	auto model = GETCOMPONENT(HydroqGameModel);
	// create goal that will go to target location and starts building
	auto position = task->taskNode->GetTransform().localPos;

	auto composite = new GoalComposite(StringHash(), false);
	composite->AddSubgoal(new GotoPositionGoal(position));
	composite->AddSubgoal(new BuildBridgeGoal(task));
	this->buildGoal = composite;
	owner->AddBehavior(composite);
}

void WorkerBridgeBuildState::Update(const uint64 delta, const uint64 absolute) {
	if (buildGoal != nullptr && (buildGoal->IsFailed() || buildGoal->IsCompleted())) {

		// change back to idle state
		auto stateToChange = GetParent()->FindLocalState(StringHash(STATE_WORKER_IDLE));
		GetParent()->ChangeState(stateToChange);
	}
}