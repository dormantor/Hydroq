#include "AttractorPlacement.h"
#include "ComponentStorage.h"

void AttractorPlacement::OnInit() {
	SubscribeForMessages(ACT_FUNC_SELECTED, ACT_OBJECT_HIT_OVER, ACT_OBJECT_HIT_ENDED, ACT_OBJECT_HIT_LOST, ACT_OBJECT_HIT_STARTED);
	floatingGameScene = owner->GetBehavior<FloatingScene>();
	gameView = owner->GetBehavior<GameView>();
	gameModel = owner->GetBehavior<GameModel>();
	playerModel = GETCOMPONENT(PlayerModel);
}

Vec2i AttractorPlacement::GetTilePosition(ofVec2f objectAbsPos) {
	auto shape = owner->GetMesh();
	// get pressed tile
	float shapeWidth = shape->GetWidth()*owner->GetTransform().absScale.x;
	float shapeHeight = shape->GetHeight()*owner->GetTransform().absScale.y;
	float absPosX = owner->GetTransform().absPos.x;
	float absPosY = owner->GetTransform().absPos.y;

	float distX = (objectAbsPos.x - absPosX);
	float distY = (objectAbsPos.y - absPosY);
	int tileX = (int)((distX / shapeWidth)*gameModel->GetMap()->GetWidth());
	int tileY = (int)((distY / shapeHeight)*gameModel->GetMap()->GetHeight());

	return Vec2i(tileX, tileY);
}


void AttractorPlacement::OnMessage(Msg& msg) {
	if (msg.HasAction(StrId(ACT_FUNC_SELECTED))) {
		auto which = msg.GetData<ValueChangeEvent<HydroqAction>>();

		// when user selects attractor in game menu, the game scene should be disabled, because
		// user can now use the mouse or finger to select and expand placed attractor

		// when user selects another item than attractor, the game scene will be enabled again

		selectedAction = which->after;

		if (which->after == HydroqAction::ATTRACT) {
			floatingGameScene->SetComponentState(ComponentState::DISABLED);
		}
		else if (which->after == HydroqAction::NONE) {
			floatingGameScene->SetComponentState(ComponentState::ACTIVE_ALL);
		}
	}
	else {
		if (selectedAction == HydroqAction::ATTRACT) {

			if (msg.GetContextNode()->GetId() == owner->GetId()) {

				if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {

					// place a new attractor
					auto touch = msg.GetData<InputEvent>();
					ofVec2f endPos = touch->input->position;
					Vec2i brickPos = GetTilePosition(endPos);

					auto clickedNode = gameModel->GetMap()->GetTile(brickPos.x, brickPos.y);

					// don't place attractor nearby another
					for (auto& attr : attractors) {
						if (Vec2i::Distance(attr->GetAttr<Vec2i>(ATTR_BRICK_POS), brickPos) < 4) return;
					}

					if (attrPlaced >= maxAttractors) {
						// remove first attractor
						auto firstAttr = attractors[0];
						attractors.erase(attractors.begin());
						Vec2i brickPosition = firstAttr->GetAttr<Vec2i>(ATTR_BRICK_POS);
						gameModel->DestroyAttractor(brickPosition, playerModel->GetFaction());
						owner->RemoveChild(firstAttr, true);
						attrPlaced--;
					}

					InsertAttractor(endPos, brickPos);
				}
				else if (msg.HasAction(ACT_OBJECT_HIT_OVER)) {
					// user can drag the created attractor to expand its area

					if (attractorPlaced) {
						auto touch = msg.GetData<InputEvent>();
						ofVec2f endPos = touch->input->position;
						auto& trans = placedAttractor->GetTransform();
						auto shape = placedAttractor->GetMesh<Image>();

						// recalculate attractor position and scale
						auto absRadius = 0.5f * owner->GetTransform().absScale.x*trans.scale.x*shape->GetWidth();
						auto absPos = trans.absPos + absRadius;

						auto distance = endPos.distance(absPos);
						auto radiusRatio = distance / absRadius;
						auto newScale = trans.scale*radiusRatio;

						if (newScale.x >= 1.0f && newScale.x <= 3.0f) {
							Vec2i brickPosition = placedAttractor->GetAttr<Vec2i>(ATTR_BRICK_POS);
							gameModel->ChangeAttractorCardinality(brickPosition, playerModel->GetFaction(), newScale.x / 3.0f);
							trans.scale = newScale;

							RefreshAttractorPosition(absPos, trans.scale);
						}
						else {
							placedAttractor->GetTransform().rotation = 0;
						}
					}
				}
				else if (msg.HasAction(ACT_OBJECT_HIT_LOST) || msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
					if (placedAttractor != nullptr) {
						auto playerModel = GETCOMPONENT(PlayerModel);
						playerModel->SetHydroqAction(HydroqAction::NONE);
					}
					placedAttractor = nullptr;
					attractorPlaced = false;
				}
			}
		}
		else {
			if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
				// when user double clicks on already created attractor, remove it from the game scene

				if (msg.GetContextNode()->GetId() == owner->GetId()) {
					auto touch = msg.GetData<InputEvent>();
					ofVec2f endPos = touch->input->position;

					Node* clicked = GetClickedAttractor(endPos);

					if (clickedAttractor != nullptr && clicked != nullptr && clicked->GetId() == clickedAttractor->GetId() &&
						clickedAttrPos.distance(endPos) <= TOUCHPOINT_TOLERANCE) {
						// second click -> remove attractor
						touch->input->SetIsProcessed(true);
						RemoveAttractor(endPos);
					}
					else {
						// first click -> just set attributes and go to the next state
						clickedAttractor = clicked;
						clickedAttrPos = endPos;
					}
				}
			}
		}
	}
}

void AttractorPlacement::RefreshAttractorPosition(ofVec2f absPosition, ofVec2f scale) {
	TransformMath math = TransformMath();
	TransformEnt ent = TransformEnt(absPosition, 1, CalcType::ABS, ofVec2f(0.5f), scale, CalcType::LOC);
	math.SetTransform(placedAttractor, owner, ent, 0, 0);
}

Node* AttractorPlacement::GetClickedAttractor(ofVec2f position) {
	for (auto it = attractors.begin(); it != attractors.end(); ++it) {
		auto node = (*it);
		auto shape = node->GetMesh<Image>();

		// calculate absolute positions
		float x = node->GetTransform().absPos.x;
		float y = node->GetTransform().absPos.y;

		auto absLength = owner->GetTransform().absScale.x*node->GetTransform().scale.x*shape->GetWidth();
		float width = x + absLength;
		float height = y + absLength;

		if (position.x >= x && position.x <= width &&position.y >= y &&position.y <= height) {
			return node;
		}
	}
	return nullptr;
}

bool AttractorPlacement::RemoveAttractor(ofVec2f position) {
	for (auto it = attractors.begin(); it != attractors.end(); ++it) {
		auto node = (*it);
		auto shape = node->GetMesh<Image>();
		float x = node->GetTransform().absPos.x;
		float y = node->GetTransform().absPos.y;

		auto absLength = owner->GetTransform().absScale.x*node->GetTransform().scale.x*shape->GetWidth();
		float width = x + absLength;
		float height = y + absLength;

		if (position.x >= x && position.x <= width &&position.y >= y &&position.y <= height) {

			Vec2i brickPosition = node->GetAttr<Vec2i>(ATTR_BRICK_POS);
			attractors.erase(it);

			gameModel->DestroyAttractor(brickPosition, playerModel->GetFaction());
			owner->RemoveChild(node, true);
			attrPlaced--;
			return true;
		}

	}

	return false;
}

void AttractorPlacement::InsertAttractor(ofVec2f position, Vec2i tilePos) {

	placedAttractor = new Node("attractor");
	if (selectedAction == HydroqAction::ATTRACT) {
		placedAttractor->SetMesh(spt<Image>(new Image(CogGet2DImage("game/functions_cmd_attract_placement.png"))));
	}

	attrPlaced++;

	placedAttractor->AddAttr(ATTR_BRICK_POS, tilePos);
	gameModel->AddAttractor(tilePos, playerModel->GetFaction(), 1);

	attractors.push_back(placedAttractor);
	RefreshAttractorPosition(position, ofVec2f(1));
	owner->AddChild(placedAttractor);

	attractorPlaced = true;
}
