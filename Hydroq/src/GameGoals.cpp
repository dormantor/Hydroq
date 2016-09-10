#pragma once

#include "GameGoals.h"
#include "GameModel.h"
#include "GameMap.h"
#include "SteeringBehavior.h"

using namespace Cog;

void GotoPositionGoal::OnStart() {
	RecalcPath();
}

void GotoPositionGoal::RecalcPath() {

	// find path
	vector<Vec2i> map;
	gameModel->GetMap()->FindPath(startCell, endCell, true,map, 0);

	if (!map.empty()) {
		ofVec2f firstPoint = map.size() >= 2 ? ofVec2f(map[1].x, map[1].y) + 0.5f : targetPosition;
		Path* pth = new Path(startPosition, firstPoint);

		for (int i = 2; i < map.size(); i++) {
			auto point = map[i];
			// always keep at the center
			pth->AddSegment(ofVec2f(point.x + 0.5f, point.y + 0.5f));
		}

		if (map.size() >= 2) {
			// add the last segment
			pth->AddSegment(targetPosition);
		}

		// run follow behavior
		innerBehavior = new FollowBehavior(pth, 60, 10, 0.25f, 0.1f);
		owner->AddBehavior(innerBehavior);
	}
	else {
		COGLOGDEBUG("Hydroq", "Couldn't find path! Exiting GotoPositionGoal");
		this->Abort();
	}

}

void GotoPositionGoal::OnGoalAbort() {
	if (innerBehavior != nullptr) {
		innerBehavior->Finish();
		// stop moving
		Movement& movement = owner->GetAttr<Movement>(ATTR_MOVEMENT);
		movement.Stop();
	}

	task->SetIsProcessing(false);
	task->SetIsReserved(false);
	task->RemoveReserverNode(owner->GetId());
}

void GotoPositionGoal::Update(const uint64 delta, const uint64 absolute) {
	if (innerBehavior != nullptr && this->task->NeedRecalculation()) {
		innerBehavior->Finish();
		RecalcPath();
	}
	else {
		if (innerBehavior != nullptr && innerBehavior->HasFinished()) {
			this->SetGoalState(GoalState::COMPLETED);
			Finish();
		}
	}
}

void BuildBridgeGoal::Update(const uint64 delta, const uint64 absolute) {
	if (goalStarted == 0) {
		goalStarted = absolute;
	}
	else {
		// building lasts 350ms
		if ((absolute - goalStarted) > 350) {

			CogLogDebug("Hydroq", "Building bridge");
			
			// build bridge each 3.5s
			auto position = Vec2i(task->GetTaskNode()->GetTransform().localPos);
			gameModel->BuildPlatform(position);
			task->SetIsEnded(true);
			this->Complete();
		}
	}
}


void DestroyBridgeGoal::Update(const uint64 delta, const uint64 absolute) {
	if (goalStarted == 0) {
		goalStarted = absolute;
	}
	else {
		// destroying lasts 2 sec
		if ((absolute - goalStarted) > 2000) {
			
			if (CogGetFrameCounter() % 10 == 0) {
				
				// check if nobody is inside the area that will be destroyed
				auto position = ofVec2f(task->GetTaskNode()->GetTransform().localPos + 0.5f);
				vector<NodeCellObject*> neighbours;
				gameModel->GetCellSpace()->CalcNeighbors(position, 0.5f, neighbours);

				if (neighbours.empty() || (neighbours.size() == 1 && neighbours[0]->node->GetId() == owner->GetId())) {

					CogLogDebug("Hydroq", "Destroying bridge");

					auto position = Vec2i(task->GetTaskNode()->GetTransform().localPos);
					gameModel->DestroyPlatform(position);
					task->SetIsEnded(true);
					this->Complete();
				}
			}
		}
	}
}
