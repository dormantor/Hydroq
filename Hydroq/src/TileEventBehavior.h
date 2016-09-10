#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "GameView.h"
#include "GameModel.h"


/**
* Informs other nodes that user clicked on brick
*/
class TileEventBehavior : public Behavior {

	Node* objectBoard;
	ResourceCache* cache;
	ofVec2f hitPos;
	int mapWidth = 0;
	int mapHeight = 0;
	GameView* gameView;

public:

	TileEventBehavior() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};