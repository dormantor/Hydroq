#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "HydroqGameView.h"
#include "HydroqGameModel.h"


/**
* Informs other nodes that user clicked on brick
*/
class TileEventBehavior : public Behavior {

	Node* objectBoard;
	ResourceCache* cache;
	ofVec2f hitPos;
	int mapWidth = 0;
	int mapHeight = 0;
	HydroqGameView* gameView;

public:

	TileEventBehavior() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_OBJECT_HIT_STARTED, ACT_OBJECT_HIT_ENDED);

		cache = GETCOMPONENT(ResourceCache);
		gameView = owner->GetBehavior<HydroqGameView>();
	}


	void OnMessage(Msg& msg) {
		if (msg.GetContextNode()->GetId() == owner->GetId() || msg.GetContextNode()->GetId() == owner->GetParent()->GetId()) {

			auto touch = msg.GetData<InputEvent>();

			if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {
				hitPos = touch->input->position;
			}
			else if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {

				if (mapWidth == 0 || mapHeight == 0) {
					auto gameModel = owner->GetBehavior<HydroqGameModel>();
					mapWidth = gameModel->GetMap()->GetWidth();
					mapHeight = gameModel->GetMap()->GetHeight();
				}

				ofVec2f endPos = touch->input->position;

				if (touch->input->IsProcessed()) return;

				bool isPointerOver = isPointerOver = endPos.distance(hitPos) < CogGetScreenWidth() / TOUCHMOVE_TOLERANCE;

				if (isPointerOver) {
					auto shape = owner->GetMesh();
					
					touch->input->SetIsProcessed(true);

					// get pressed brick
					float shapeWidth = shape->GetWidth()*owner->GetTransform().absScale.x;
					float shapeHeight = shape->GetHeight()*owner->GetTransform().absScale.y;
					float absPosX = owner->GetTransform().absPos.x;
					float absPosY = owner->GetTransform().absPos.y;

					float distX = (endPos.x - absPosX);
					float distY = (endPos.y - absPosY);

					// get brick indices
					int brickX = (int)((distX / shapeWidth)*mapWidth);
					int brickY = (int)((distY / shapeHeight)*mapHeight);

					// suppose that the collection follows positional ordering
					auto pressedTileSprite = gameView->GetStaticSprites()->GetSprites()[brickY*mapWidth + brickX];

					SendMessage(ACT_BRICK_CLICKED, spt<TileClickEvent>(new TileClickEvent(brickX, brickY, pressedTileSprite)));

				}
			}
		}
	}



public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};