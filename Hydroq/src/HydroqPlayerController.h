#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteInst.h"
#include "HydroqPlayerModel.h"
#include "HydroqGameModel.h"


class HydroqPlayerController : public Behavior {
	
	HydroqGameModel* gameModel;
	HydroqPlayerModel* playerModel;

public:
	HydroqPlayerController() {
			
	}

	void OnInit() {
		SubscribeForMessages(ACT_BRICK_CLICKED);
		gameModel = owner->GetBehavior<HydroqGameModel>();
		playerModel = GETCOMPONENT(HydroqPlayerModel);
	}


	void OnMessage(Msg& msg) {
		if (msg.GetContextNode()->GetId() == owner->GetId()) {
			auto clickEvent = msg.GetData<TileClickEvent>();

			// get clicked entity directly from model
			auto mapNode = gameModel->GetMap()->GetNode(clickEvent->brickPosX, clickEvent->brickPosY);

			if (playerModel->GetHydroqAction() != HydroqAction::NONE) {
				Vec2i pos = Vec2i(clickEvent->brickPosX, clickEvent->brickPosY);

				// if user selected a function, apply it
				if (playerModel->GetHydroqAction() == HydroqAction::BUILD){ 
					if (gameModel->IsPositionFreeForBridge(pos)) {
						gameModel->MarkPositionForBridge(pos, gameModel->GetFaction());
					}
					else if (gameModel->PositionContainsBridgeMark(pos)) {
						// nothing to do here
					}
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::FORBID){
					if (gameModel->IsPositionFreeForForbid(pos)) {
						gameModel->MarkPositionForForbid(pos);
					}
					else if (gameModel->PositionContainsForbidMark(pos)) {
						gameModel->DeleteForbidMark(pos);
					}
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::DESTROY){
					if (gameModel->IsPositionFreeForDestroy(pos)) {
						gameModel->MarkPositionForDestroy(pos, gameModel->GetFaction());
					}
					else if (gameModel->PositionContainsDestroyMark(pos) || gameModel->PositionContainsBridgeMark(pos)) {
						gameModel->DeleteBridgeMark(pos);
					}
				}
			}
		}
	}



public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};