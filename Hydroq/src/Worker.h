#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "State.h"
#include "StringHash.h"
#include "HydroqGoals.h"
#include "GameTask.h"
#include "HydMap.h"

namespace Cog {
	class ArriveBehavior;
}

class WorkerIdleState : public State {
	
public:
	ArriveBehavior* movingAround = nullptr;

	WorkerIdleState() : State(StringHash(STATE_WORKER_IDLE)) {

	}

	void OnMessage(Msg& msg) {

	}

	virtual void OnStart();

	void OnFinish();

	bool FindTaskToDo();

	void MoveAround();

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

class WorkerBridgeBuildState : public State {

public:
	spt<GameTask> task;
	HydMapNode* nodeToBuildfrom;

	// composite of goals to build the bridge
	Goal* buildGoal;
	WorkerBridgeBuildState() :State(StringHash(STATE_WORKER_BUILD)) {

	}

	void OnInit() {
		RegisterListening(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);


	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	void SetNodeToBuildFrom(HydMapNode* node) {
		this->nodeToBuildfrom = node;
	}

	spt<GameTask> GetGameTask() {
		return this->task;
	}

	virtual void OnStart();


	virtual void Update(const uint64 delta, const uint64 absolute);
};

class WorkerAttractorFollowState : public State {

public:
	spt<GameTask> task;
	HydMapNode* nodeToFollow;

	// composite of goals to build the bridge
	Goal* followGoal;
	WorkerAttractorFollowState() :State(StringHash(STATE_WORKER_ATTRACTOR_FOLLOW)) {

	}

	void OnInit() {
		RegisterListening(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);


	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	void SetNodeToFollow(HydMapNode* node) {
		this->nodeToFollow = node;
	}

	spt<GameTask> GetGameTask() {
		return this->task;
	}

	virtual void OnStart();


	virtual void Update(const uint64 delta, const uint64 absolute);
};