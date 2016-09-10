#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class Seedbed : public Behavior {
	OBJECT_PROTOTYPE(Seedbed)

	// attributes sh
	StringHash frequencySh;
	StringHash lastSpawnSh;

	void Init();

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

