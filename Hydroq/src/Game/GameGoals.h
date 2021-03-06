#pragma once

#include "Goal.h"
#include "HydroqDef.h"
#include "GameTask.h"
#include "Vec2i.h"

using namespace Cog;

// ========================================================
//			    Goals for goal-driven behavior
// ========================================================

class GameModel;

/**
* Goal that moves target unit to the selected position
*/
class GotoPositionGoal : public Goal {
	Behavior* innerBehavior = nullptr;
	spt<GameTask> task;
public:
	// location of the first cell
	Vec2i startCell;
	// location of the last cell
	Vec2i endCell;
	// precise starting position
	ofVec2f startPosition;
	// precise end position
	ofVec2f targetPosition;

	GameModel* gameModel;

	GotoPositionGoal(GameModel* gameModel, spt<GameTask> task, Vec2i startCell, Vec2i endCell, ofVec2f startPr, ofVec2f targetPr) 
		: Goal(StrId(GOAL_GOTO_POSITION)), task(task), startCell(startCell), endCell(endCell), startPosition(startPr), targetPosition(targetPr), gameModel(gameModel) {

	}

	void OnStart();

	void RecalcPath();

	virtual void OnGoalAbort();

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
	GameModel* gameModel;
	int buildingDelay = 0;
	
	
	BuildBridgeGoal(GameModel* gameModel, spt<GameTask> task) : Goal(StrId(GOAL_BUILD_BRIDGE)), gameModel(gameModel), task(task) {

	}

	virtual void OnStart();

	virtual void Update(const uint64 delta, const uint64 absolute);
};

/**
* Goal that destroys a bridge
*/
class DestroyBridgeGoal : public Goal {

public:
	spt<GameTask> task;
	// the time the goal started
	uint64 goalStarted = 0;
	GameModel* gameModel;
	int destroyDelay = 0;

	virtual void OnStart();

	DestroyBridgeGoal(GameModel* gameModel, spt<GameTask> task) : Goal(StrId(GOAL_DESTROY_BRIDGE)), task(task), gameModel(gameModel) {

	}


	virtual void Update(const uint64 delta, const uint64 absolute);
};