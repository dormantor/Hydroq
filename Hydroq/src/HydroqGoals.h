#pragma once

#include "Goal.h"
#include "HydroqDef.h"
#include "GameTask.h"

using namespace Cog;

class GotoPositionGoal : public Goal {
	Behavior* innerBehavior = nullptr;
public:
	ofVec2f targetPosition;

	GotoPositionGoal(ofVec2f targetPos) : Goal(StringHash(GOAL_GOTO_POSITION)), targetPosition(targetPos) {

	}

	void Init();

	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (innerBehavior != nullptr && innerBehavior->Ended()) {
			this->SetGoalState(GoalState::COMPLETED);
			Finish();
		}
	}
};

class BuildBridgeGoal : public Goal {

public:
	spt<GameTask> task;
	uint64 goalStarted = 0;

	BuildBridgeGoal(spt<GameTask> task) : Goal(StringHash(GOAL_BUILD_BRIDGE)), task(task) {

	}

	void Init() {

	}



	virtual void Update(const uint64 delta, const uint64 absolute);
};