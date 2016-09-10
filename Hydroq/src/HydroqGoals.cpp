#pragma once

#include "HydroqGoals.h"
#include "HydroqGameModel.h"
#include "HydMap.h"
#include "SteeringBehavior.h"

using namespace Cog;

void GotoPositionGoal::Init() {
	auto model = GETCOMPONENT(HydroqGameModel);
	
	// find path
	vector<Vec2i> map = model->GetMap()->FindPath(startCell, endCell);

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
		MLOGDEBUG("Hydroq", "Couldn't find path! Exiting GotoPositionGoal");
		this->Complete();
		Finish();
	}
}


void GotoPositionGoal::Update(const uint64 delta, const uint64 absolute) {
	if (innerBehavior != nullptr && innerBehavior->Ended()) {
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
			Finish();
		}
	}
}