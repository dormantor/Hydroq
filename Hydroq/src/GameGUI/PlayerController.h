#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "PlayerModel.h"
#include "GameModel.h"

/**
* Behavior that responds to messages on click on the game board and
* transforms them to the desired actions
*/
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