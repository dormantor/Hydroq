#pragma once

#include "Behavior.h"

using namespace Cog;

/**
* Behavior for main menu
*/
class MenuBehavior : public Behavior {
public:
	MenuBehavior() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

