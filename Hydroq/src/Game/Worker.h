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

/**
* Functional logic for idle state (wander around local position)
*/
class WorkerIdleState : public State {
private:
	ArriveBehavior* movingAround = nullptr;
	GameModel* gameModel;
	spt<GameTask> lastFoundTask = spt<GameTask>();
public:
	
	WorkerIdleState(GameModel* gameModel) 
		: State(StrId(STATE_WORKER_IDLE)), gameModel(gameModel) {
	}

	virtual void OnStart();

	void OnFinish();

	/**
	* Pick a task that could be done
	*/
	bool FindTaskToDo();

	/**
	* Wander around local position
	*/
	void WanderAround();

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

/**
* Functional logic for building state
*/
class WorkerBuildState : public State {
private:
	// link to task
	spt<GameTask> task;
	// tile the worker should stand while building
	GameMapTile* tileToBuildfrom;
	GameModel* gameModel;
public:
	// goal composite that ensures construction of the bridge
	Goal* buildGoal;

	WorkerBuildState(GameModel* gameModel) 
		: State(StrId(STATE_WORKER_BUILD)), gameModel(gameModel) {
	}

	void OnInit() {
		SubscribeForMessages(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);

	/**
	* Gets link to the game task
	*/
	spt<GameTask> GetGameTask() const {
		return this->task;
	}

	/**
	* Sets link to game task
	*/
	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	/**
	* Sets tile the worker should stand while building
	*/
	void SetTileToBuildFrom(GameMapTile* tile) {
		this->tileToBuildfrom = tile;
	}

	virtual void OnStart();


	virtual void Update(const uint64 delta, const uint64 absolute);
};

/**
* Functional logic for attractor follow state
*/
class WorkerAttractState : public State {
private:
	// link to task
	spt<GameTask> task;
	// tile to attract
	GameMapTile* tileToFollow;
	GameModel* gameModel;
public:
	// composite goal that ensures this job
	Goal* followGoal;

	WorkerAttractState(GameModel* gameModel) 
		: State(StrId(STATE_WORKER_ATTRACTOR_FOLLOW)), gameModel(gameModel) {

	}

	void OnInit() {
		SubscribeForMessages(ACT_TASK_ABORTED);
	}

	void OnFinish();

	void OnMessage(Msg& msg);

	/**
	* Gets link to the game task
	*/
	spt<GameTask> GetGameTask() {
		return this->task;
	}

	/**
	* Sets link to the game task
	*/
	void SetGameTask(spt<GameTask> task) {
		this->task = task;
	}

	/**
	* Sets tile to attract
	*/
	void SetTileToFollow(GameMapTile* tile) {
		this->tileToFollow = tile;
	}

	virtual void OnStart();


	virtual void Update(const uint64 delta, const uint64 absolute);
};