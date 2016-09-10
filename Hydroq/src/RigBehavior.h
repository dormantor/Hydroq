#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class RigBehavior : public Behavior {
	OBJECT_PROTOTYPE(RigBehavior)

	// attributes sh
	StringHash frequencySh;
	StringHash lastSpawnSh;

	void OnStart();

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

