#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class SelectedFuncBehavior : public Behavior {
	
	int selectedFuncSeedBed;
	int selectedFunc;

	unsigned actFuncSelected;

public:
	SelectedFuncBehavior() {

	}

	void OnInit();

	void OnStart();

	void OnMessage(Msg& msg);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}

};