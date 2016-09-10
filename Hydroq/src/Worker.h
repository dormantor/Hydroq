#pragma once

#include "HydroqDef.h"
#include "State.h"
#include "StrId.h"
#include "GameGoals.h"
#include "GameTask.h"
#include "GameMap.h"

namespace Cog {
	class ArriveBehavior;
}

class WorkerIdleState : public State {
	
public:
	ArriveBehavior* movingAround = nullptr;
	GameModel* gameModel;

	WorkerIdleState(GameModel* gameModel) : State(StrId(STATE_WORKER_IDLE)), gameModel(gameModel) {

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
	GameMapNode* nodeToBuildfrom;
	GameModel* gameModel;

	// composite of goals to build the bridge
	Goal* buildGoal;
	WorkerBridgeBuildState(GameModel* gameModel) :State(StrId(STATE_WORKER_BUILD)), gameModel(gameModel) {

	}

	void OnInit() {
		SubscribeForMessages(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);


	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	void SetNodeToBuildFrom(GameMapNode* node) {
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
	GameMapNode* nodeToFollow;
	GameModel* gameModel;
	// composite of goals to build the bridge
	Goal* followGoal;
	WorkerAttractorFollowState(GameModel* gameModel) :State(StrId(STATE_WORKER_ATTRACTOR_FOLLOW)), gameModel(gameModel) {

	}

	void OnInit() {
		SubscribeForMessages(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);


	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	void SetNodeToFollow(GameMapNode* node) {
		this->nodeToFollow = node;
	}

	spt<GameTask> GetGameTask() {
		return this->task;
	}

	virtual void OnStart();


	virtual void Update(const uint64 delta, const uint64 absolute);
};