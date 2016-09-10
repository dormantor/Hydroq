#pragma once

#include "Goal.h"
#include "HydroqDef.h"

using namespace Cog;

class GotoPositionGoal : public Goal {
	
public:
	ofVec2f targetPosition;

	GotoPositionGoal(ofVec2f targetPos) : Goal(StringHash(GOAL_GOTO_POSITION)), targetPosition(targetPos) {

	}

	void Init();

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};