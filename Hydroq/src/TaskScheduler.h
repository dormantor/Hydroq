#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "State.h"
#include "StringHash.h"
#include "HydroqGoals.h"
#include "GameTask.h"
#include "HydMap.h"
#include "HydroqGameModel.h"

class TaskScheduler : public Behavior {
	BEHAVIOR_UNIQUE()

private:
	HydroqGameModel* gameModel;

public:

	void OnInit(){
		this->gameModel = GETCOMPONENT(HydroqGameModel);
	}

	void ScheduleTasks();

	virtual void Update(const uint64 delta, const uint64 absolute) {
		
		if (CogGetFrameCounter() % 30 == 0) {
			ScheduleTasks();
		}
	}

};