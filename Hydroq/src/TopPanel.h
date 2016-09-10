#pragma once

#include "Behavior.h"
#include "GameModel.h"

using namespace Cog;

#define SCORE_WIDTH 14

/**
* Behavior that displays balance of power on the top of the game board
*/
class TopPanel : public Behavior {
	Node* blueBar;
	Node* redBar;
	int gridWidth;
	int gridHeight;
	GameModel* gameModel;

public:
	TopPanel() {

	}

	void OnInit();

	void OnMessage(Msg& msg) {
		
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

