#pragma once

#include "HydroqDef.h"
#include "StrId.h"
#include "Behavior.h"

using namespace Cog;
class GameModel;

/**
* Functional logic for drilling rigs
*/
class RigBehavior : public Behavior {

	// attributes stringId
	GameModel* gameModel;
	// total number of workers the rig has created
	int totalWorkers = 0;
	float spawnFrequency;
	uint64 lastSpawn = 0;
	int maxWorkers = 20;

public:
	RigBehavior(GameModel* gameModel, float spawnFrequency) : gameModel(gameModel), spawnFrequency(spawnFrequency){

	}
	
	void OnMessage(Msg& msg) {
	}

public:
	
	virtual void Update(const uint64 delta, const uint64 absolute);
};

