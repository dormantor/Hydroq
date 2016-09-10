#pragma once

#include "ofxCogMain.h"
#include "Scene.h"
#include "PlayerModel.h"

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