#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteEntity.h"
#include "HydroqGameView.h"
#include "HydroqGameModel.h"


/**
* Informs other nodes that user clicked on brick
*/
class BrickEventBehavior : public Behavior {
	OBJECT_PROTOTYPE(BrickEventBehavior)


	Node* objectBoard;
	ResourceCache* cache;
	ofVec2f hitPos;
	int mapWidth = 0;
	int mapHeight = 0;
	HydroqGameView* gameView;

	void OnInit() {
		RegisterListening(ACT_OBJECT_HIT_STARTED, ACT_OBJECT_HIT_ENDED);

		cache = GETCOMPONENT(ResourceCache);
		gameView = GETCOMPONENT(HydroqGameView);
	}


	void OnMessage(Msg& msg) {
		if (msg.GetSourceObject()->GetId() == owner->GetId() || msg.GetSourceObject()->GetId() == owner->GetParent()->GetId()) {

			InputEvent* touch = static_cast<InputEvent*>(msg.GetData());

			if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {
				hitPos = touch->input->position;
			}
			else if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {

				if (mapWidth == 0 || mapHeight == 0) {
					auto gameModel = GETCOMPONENT(HydroqGameModel);
					mapWidth = gameModel->GetMap()->GetWidth();
					mapHeight = gameModel->GetMap()->GetHeight();
				}

				ofVec2f endPos = touch->input->position;

				if (touch->input->IsProcessed()) return;

				bool isPointerOver = isPointerOver = endPos.distance(hitPos) < CogGetScreenWidth() / SCREEN_TOLERANCE;

				if (isPointerOver) {
					auto shape = owner->GetShape();
					
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
					auto pressedBrickSprite = gameView->GetStaticSprites()->GetSprites()[brickY*mapWidth + brickX];

					SendMessageToListeners(ACT_BRICK_CLICKED, 0, new BrickClickEvent(brickX, brickY, pressedBrickSprite), owner);

				}
			}
		}
	}



public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};