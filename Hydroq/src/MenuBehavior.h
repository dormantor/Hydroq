#pragma once

#include "ofxCogMain.h"
#include "MultiAnim.h"

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

