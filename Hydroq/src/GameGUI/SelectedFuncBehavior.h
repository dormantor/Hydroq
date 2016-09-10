#pragma once

#include "HydroqDef.h"
#include "Behavior.h"

using namespace Cog;

/**
* Behavior that just sets icon of currently selected action 
* in the lower left corner of the game board
*/
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