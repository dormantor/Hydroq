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

/**
* Scheduler that searches in a set of not assigned tasks and tries to
* assign them to available workers
*/
class TaskScheduler : public Behavior {

private:
	GameModel* gameModel;
	PlayerModel* playerModel;
public:

	TaskScheduler(GameModel* gameModel) :gameModel(gameModel) {

	}

	void OnInit() {
		playerModel = GETCOMPONENT(PlayerModel);
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (CogGetFrameCounter() % 60 == 0) {
			ScheduleTasks(absolute);
		}
	}


protected:
	/**
	* Calculates for each worker a number of assigned tasks
	* @param tasks collection of tasks
	* @param output map of worker identifiers and number of tasks they have assigned
	*/
	void CalcAssignedTasks(vector<spt<GameTask>>& tasks, map<int, int>& output);

	/**
	* Schedules tasks for all workers
	*/
	void ScheduleTasks(uint64 absolute);

	/**
	* Schedules tasks for selected faction (only for games against AI)
	*/
	void ScheduleTasksForFaction(uint64 absolute, Faction faction);

};