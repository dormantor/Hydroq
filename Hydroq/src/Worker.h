#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class Worker : public Behavior {
	OBJECT_PROTOTYPE(Worker)

	void Init() {
	
	}

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		owner->GetTransform().localPos.x += 0.1f;
		owner->GetTransform().localPos.y += 0.1f;
	}
};

