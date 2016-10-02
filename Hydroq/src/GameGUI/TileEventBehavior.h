#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "GameView.h"
#include "GameModel.h"

namespace Cog {
	class Resources;
}

using namespace Cog;

/**
* Informs other nodes that user clicked on a game tile
*/
class TileEventBehavior : public Behavior {

	Node* objectBoard;
	Resources* cache;
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