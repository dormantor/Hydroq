#include "Worker.h"
#include "HydroqGameModel.h"


void WorkerIdleState::Update(const uint64 delta, const uint64 absolute) {
	if (CogGetFrameCounter() % 10 == 0) {
		// each 10 frame check if this is something to do...
		auto model = GETCOMPONENT(HydroqGameModel);
		for (auto& task : model->GetGameTasks()) {
			if (!task->isProcessing && !task->isEnded && task->taskSh == StringHash(TASK_BRIDGE_BUILD)) {
				auto position = task->taskNode->GetTransform().localPos;
				CogLogDebug("Worker","Got task for bridge building at position [%d,%d]", position.x, position.y);
				
				auto stateToChange = GetParent()->FindLocalState(StringHash(STATE_WORKER_BUILD));
				static_cast<WorkerBridgeBuildState*>(stateToChange)->SetGameTask(task);
				this->GetParent()->ChangeState(StringHash(STATE_WORKER_BUILD));
				
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
	this->buildGoal = composite;
	owner->AddBehavior(composite);
}