#pragma once

#include "HydroqDef.h"
#include "StrId.h"
#include "Behavior.h"

using namespace Cog;
class GameModel;

class RigBehavior : public Behavior {

	// attributes sh
	StrId frequencySh;
	StrId lastSpawnSh;
	GameModel* gameModel;
	int totalWorkers = 0;
public:
	RigBehavior(GameModel* gameModel) : gameModel(gameModel) {

	}
	
	void OnStart();

	void OnMessage(Msg& msg) {

	}

public:
	
	virtual void Update(const uint64 delta, const uint64 absolute);
};

