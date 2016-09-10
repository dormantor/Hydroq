#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "PlayerModel.h"
#include "GameModel.h"


class PlayerController : public Behavior {
	
	GameModel* gameModel;
	PlayerModel* playerModel;

public:
	PlayerController() {
			
	}

	void OnInit();

	void OnMessage(Msg& msg);


public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};