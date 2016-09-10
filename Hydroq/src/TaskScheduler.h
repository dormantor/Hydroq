#pragma once

#include "HydroqDef.h"
#include "State.h"
#include "StrId.h"
#include "GameGoals.h"
#include "GameTask.h"
#include "GameMap.h"
#include "GameModel.h"
#include "PlayerModel.h"
#include "ComponentStorage.h"

class TaskScheduler : public Behavior {

private:
	GameModel* gameModel;
	PlayerModel* playerModel;
public:

	TaskScheduler(GameModel* gameModel) :gameModel(gameModel) {

	}

	virtual void OnInit() {
		playerModel = GETCOMPONENT(PlayerModel);
	}

	void CalcAssignedTasks(vector<spt<GameTask>>& tasks, map<int, int>& output);

	void ScheduleTasks(uint64 absolute);

	void ScheduleTasksForFaction(uint64 absolute, Faction faction);

	virtual void Update(const uint64 delta, const uint64 absolute) {
		
		if (CogGetFrameCounter() % 30 == 0) {
			ScheduleTasks(absolute);
		}
	}

};