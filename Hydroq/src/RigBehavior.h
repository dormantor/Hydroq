#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class HydroqGameModel;

class RigBehavior : public Behavior {

	// attributes sh
	StrId frequencySh;
	StrId lastSpawnSh;
	HydroqGameModel* gameModel;
	int totalWorkers = 0;
public:
	RigBehavior(HydroqGameModel* gameModel) : gameModel(gameModel) {

	}
	
	void OnStart();

	void OnMessage(Msg& msg) {

	}

public:
	
	virtual void Update(const uint64 delta, const uint64 absolute);
};

