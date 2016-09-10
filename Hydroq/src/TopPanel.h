#pragma once

#include "ofxCogMain.h"
#include "GameModel.h"

#define SCORE_WIDTH 14

class TopPanel : public Behavior {
	
public:
	TopPanel() {

	}

	Node* blueBar;
	Node* redBar;
	int gridWidth;
	int gridHeight;
	GameModel* gameModel;

	void OnInit();

	void OnMessage(Msg& msg) {
		
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

