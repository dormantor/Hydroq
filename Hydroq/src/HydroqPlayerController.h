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

	void OnInit() {
		RegisterListening(ACT_BRICK_CLICKED);

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
					gameModel->CreateSeedBed(pos);
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::BUILD){ 
					if (gameModel->IsPositionFreeForBridge(pos)) {
						gameModel->MarkPositionForBridge(pos);
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
				else if (playerModel->GetHydroqAction() == HydroqAction::GUARD){
					if (gameModel->IsPositionFreeForGuard(pos)) {
						gameModel->MarkPositionForGuard(pos);
					}
					else if (gameModel->PositionContainsGuardMark(pos)) {
						gameModel->DeleteGuardMark(pos);
					}
				}
				else if (playerModel->GetHydroqAction() == HydroqAction::DESTROY){
					if (gameModel->IsPositionFreeForDestroy(pos)) {
						gameModel->MarkPositionForDestroy(pos);
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