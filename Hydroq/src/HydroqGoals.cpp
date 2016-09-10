#pragma once

#include "HydroqGoals.h"
#include "HydroqGameModel.h"
#include "HydMap.h"
#include "SteeringBehavior.h"

using namespace Cog;

void GotoPositionGoal::OnStart() {
	auto model = GETCOMPONENT(HydroqGameModel);
	
	// find path
	vector<Vec2i> map = model->GetMap()->FindPath(startCell, endCell, true, 0);

	if (!map.empty()) {
		ofVec2f firstPoint = map.size() >= 2 ? ofVec2f(map[1].x, map[1].y) + 0.5f : targetPosition;
		Path* pth = new Path(startPosition, firstPoint);

		for (int i = 2; i < map.size(); i++) {
			auto point = map[i];
			pth->AddSegment(ofVec2f(point.x + 0.5f, point.y + 0.5f));
		}

		if (map.size() >= 2) {
			// add the last point
			pth->AddSegment(targetPosition);
		}

		// run follow behavior
		innerBehavior = new FollowBehavior(pth, 60, 0.25f, 0.1f);
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

	task->isProcessing = false;
	task->isReserved = false;
	task->reserverNode = nullptr;
}

void GotoPositionGoal::Update(const uint64 delta, const uint64 absolute) {
	if (innerBehavior != nullptr && innerBehavior->IsFinished()) {
		this->SetGoalState(GoalState::COMPLETED);
		Finish();
	}
}

void BuildBridgeGoal::Update(const uint64 delta, const uint64 absolute) {
	if (goalStarted == 0) {
		goalStarted = absolute;
	}
	else {
		if ((absolute - goalStarted) > 3500) {

			CogLogDebug("Hydroq", "Building bridge");
			
			// build bridge each 3.5s
			auto model = GETCOMPONENT(HydroqGameModel);
			auto position = Vec2i(task->taskNode->GetTransform().localPos);
			model->BuildPlatform(position);
			task->isEnded = true;
			this->Complete();
		}
	}
}


void DestroyBridgeGoal::Update(const uint64 delta, const uint64 absolute) {
	if (goalStarted == 0) {
		goalStarted = absolute;
	}
	else {
		if ((absolute - goalStarted) > 3500) {
			
			if (CogGetFrameCounter() % 10 == 0) {
				auto model = GETCOMPONENT(HydroqGameModel);

				// check if nobody is inside the square
				auto position = ofVec2f(task->taskNode->GetTransform().localPos + 0.5f);
				auto neighbours = model->GetCellSpace()->CalcNeighbors(position, 0.5f);

				if (neighbours.empty() || (neighbours.size() == 1 && neighbours[0]->GetId() == owner->GetId())) {

					CogLogDebug("Hydroq", "Destroying bridge");

					// destroy bridge at 3.5s
					auto position = Vec2i(task->taskNode->GetTransform().localPos);
					model->DestroyPlatform(position);
					task->isEnded = true;
					this->Complete();
				}
			}
		}
	}
}
