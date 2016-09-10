#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "State.h"
#include "StringHash.h"
#include "HydroqGoals.h"
#include "GameTask.h"
#include "HydMap.h"

class WorkerIdleState : public State {
	
public:

	WorkerIdleState() : State(StringHash(STATE_WORKER_IDLE)) {

	}

	void Init() {
		
	}

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

class WorkerBridgeBuildState : public State {

public:
	spt<GameTask> task;

	// composite of goals to build the bridge
	Goal* buildGoal;
	WorkerBridgeBuildState() :State(StringHash(STATE_WORKER_BUILD)) {

	}

	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	spt<GameTask> GetGameTask() {
		return this->task;
	}

	virtual void EnterState();

	virtual void LeaveState() {

	}

	virtual void Update(const uint64 delta, const uint64 absolute);
};