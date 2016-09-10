#pragma once

#include "ofxCogMain.h"
#include "PlayerModel.h"

class GameEndDialog : public Behavior {	
	
	PlayerModel* model;

public:
	GameEndDialog() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	bool firstInit = false;
public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};