#pragma once

#include "Goal.h"
#include "HydroqDef.h"
#include "GameTask.h"

using namespace Cog;

/**
* Goal that moves target unit to the selected position
*/
class GotoPositionGoal : public Goal {
	Behavior* innerBehavior = nullptr;
public:
	// first cell (in integer)
	Vec2i startCell;
	// last cell
	Vec2i endCell;
	// precise starting position
	ofVec2f startPosition;
	// precise end position
	ofVec2f targetPosition;

	GotoPositionGoal(Vec2i startCell, Vec2i endCell, ofVec2f startPr, ofVec2f targetPr) : Goal(StringHash(GOAL_GOTO_POSITION)), 
		startCell(startCell), endCell(endCell), startPosition(startPr), targetPosition(targetPr) {

	}

	void Init();

	virtual void OnAbort();

	virtual void Update(const uint64 delta, const uint64 absolute);
};

/**
* Goal that builds a bridge
*/
class BuildBridgeGoal : public Goal {

public:
	spt<GameTask> task;
	// the time the goal started
	uint64 goalStarted = 0;

	BuildBridgeGoal(spt<GameTask> task) : Goal(StringHash(GOAL_BUILD_BRIDGE)), task(task) {

	}

	void Init() {

	}

	virtual void Update(const uint64 delta, const uint64 absolute);
};