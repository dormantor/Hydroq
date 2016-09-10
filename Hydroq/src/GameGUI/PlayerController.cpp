#include "PlayerController.h"
#include "ComponentStorage.h"

void PlayerController::OnInit() {
	SubscribeForMessages(ACT_BRICK_CLICKED);
	gameModel = owner->GetBehavior<GameModel>();
	playerModel = GETCOMPONENT(PlayerModel);
}


void PlayerController::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_BRICK_CLICKED) && msg.GetContextNode()->GetId() == owner->GetId()) {
		auto clickEvent = msg.GetData<SpriteClickEvent>();

		// get clicked entity directly from model
		auto mapNode = gameModel->GetMap()->GetTile(clickEvent->spritePosX, clickEvent->spritePosY);

		if (playerModel->GetHydroqAction() != HydroqAction::NONE) {
			Vec2i pos = Vec2i(clickEvent->spritePosX, clickEvent->spritePosY);

			
			if (playerModel->GetHydroqAction() == HydroqAction::BUILD) {
				// mark position for building the bridge
				if (gameModel->IsPositionFreeForBridge(pos)) {
					gameModel->MarkPositionForBridge(pos, playerModel->GetFaction());
				}
			}
			else if (playerModel->GetHydroqAction() == HydroqAction::FORBID) {
				// mark position for forbidden area
				if (gameModel->IsPositionFreeForForbid(pos)) {
					gameModel->MarkPositionForForbid(pos);
				}
				else if (gameModel->PositionContainsForbidMark(pos)) {
					gameModel->DeleteForbidMark(pos);
				}
			}
			else if (playerModel->GetHydroqAction() == HydroqAction::DESTROY) {
				// mark position for destroy
				if (gameModel->IsPositionFreeForDestroy(pos)) {
					gameModel->MarkPositionForDestroy(pos, playerModel->GetFaction());
				}
				else if (gameModel->PositionContainsDestroyMark(pos) || gameModel->PositionContainsBridgeMark(pos)) {
					// if position that should be destroyed is already marked to be built, just remove the mark
					gameModel->DeleteBridgeMark(pos);
				}
			}
		}
	}
}
