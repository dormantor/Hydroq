#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "SpriteEntity.h"
#include "HydroqPlayerModel.h"
#include "HydroqGameModel.h"


class HydroqPlayerController : public Behavior {
	OBJECT_PROTOTYPE(HydroqPlayerController)

	HydroqGameModel* gameModel;
	HydroqPlayerModel* playerModel;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_BRICK_CLICKED);

		gameModel = GETCOMPONENT(HydroqGameModel);
		playerModel = GETCOMPONENT(HydroqPlayerModel);
	}


	void OnMessage(Msg& msg) {
		if (msg.GetSourceObject()->GetId() == owner->GetId()) {
			BrickClickEvent* clickEvent = static_cast<BrickClickEvent*>(msg.GetData());

			// get clicked entity directly from model
			auto mapNode = gameModel->GetMap()->GetNode(clickEvent->brickPosX, clickEvent->brickPosY);

			if (playerModel->GetHydroqAction() != HydroqAction::NONE) {
				Vec2i pos = Vec2i(clickEvent->brickPosX, clickEvent->brickPosY);

				// if user selected a function, apply it
				if (playerModel->GetHydroqAction() == HydroqAction::SEEDBED && 
					gameModel->IsPositionFreeForBuilding(pos)) {
					gameModel->CreateBuilding(pos, spt<HydEntity>(new SeebedBuilding()));
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::BUILD &&
					gameModel->IsPositionFreeForBridge(pos)) {
					// build a bridge
					gameModel->MarkPositionForBridge(pos);
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::FORBID &&
					gameModel->IsPositionFreeForForbid(pos)) {
					gameModel->MarkPositionForForbid(pos);
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::GUARD &&
					gameModel->IsPositionFreeForGuard(pos)) {
					gameModel->MarkPositionForGuard(pos);
				}
			}
		}
	}



public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};