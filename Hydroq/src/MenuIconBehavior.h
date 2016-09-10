#pragma once

#include "Scene.h"
#include "PlayerModel.h"
#include "TransformEnt.h"

/**
* Behavior for right panel icon that just rolls the 
* panel in front of the screen
*/
class MenuIconBehavior : public Behavior {
	spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;

public:

	MenuIconBehavior() {

	}

	void OnInit();

	void OnStart();

	void OnMessage(Msg& msg);
public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};