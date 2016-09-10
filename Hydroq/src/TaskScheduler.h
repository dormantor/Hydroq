#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "State.h"
#include "StrId.h"
#include "HydroqGoals.h"
#include "GameTask.h"
#include "HydMap.h"
#include "HydroqGameModel.h"

class TaskScheduler : public Behavior {

private:
	HydroqGameModel* gameModel;

public:

	TaskScheduler(HydroqGameModel* gameModel) :gameModel(gameModel) {

	}


	map<int, int> CalcAssignedTasks(vector<spt<GameTask>>& tasks);

	void ScheduleTasks(uint64 absolute);

	void ScheduleTasksForFaction(uint64 absolute, Faction faction);

	virtual void Update(const uint64 delta, const uint64 absolute) {
		
		if (CogGetFrameCounter() % 30 == 0) {
			ScheduleTasks(absolute);
		}
	}

};