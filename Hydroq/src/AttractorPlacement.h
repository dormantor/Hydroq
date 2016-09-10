#pragma once

#include "ofxCogMain.h"
#include "FloatingScene.h"
#include "HydroqDef.h"
#include "HydroqPlayerModel.h"
#include "HydroqGameView.h"
#include "HydroqGameModel.h"

class AttractorPlacement : public Behavior {
	OBJECT_PROTOTYPE(AttractorPlacement)


public:
	FloatingScene* floatingGameScene;
	bool attractorPlaced = false;
	Node* placedAttractor = nullptr;

	HydroqAction selectedAction;
	HydroqGameView* gameView;
	HydroqGameModel* gameModel;
	int attrPlaced = 0;

	vector<Node*> attractors;

	void OnInit() {
		RegisterListening(ACT_FUNC_SELECTED, ACT_OBJECT_HIT_OVER, ACT_OBJECT_HIT_ENDED, ACT_OBJECT_HIT_LOST, ACT_OBJECT_HIT_STARTED);
		floatingGameScene = owner->GetBehavior<FloatingScene>();
		gameView = GETCOMPONENT(HydroqGameView);
		gameModel = GETCOMPONENT(HydroqGameModel);
	}

	Vec2i GetBrickPosition(ofVec2f objectAbsPos) {
		auto shape = owner->GetShape();
		// get pressed brick
		float shapeWidth = shape->GetWidth()*owner->GetTransform().absScale.x;
		float shapeHeight = shape->GetHeight()*owner->GetTransform().absScale.y;
		float absPosX = owner->GetTransform().absPos.x;
		float absPosY = owner->GetTransform().absPos.y;

		float distX = (objectAbsPos.x - absPosX);
		float distY = (objectAbsPos.y - absPosY);
		int brickX = (int)((distX / shapeWidth)*gameModel->GetMap()->GetWidth());
		int brickY = (int)((distY / shapeHeight)*gameModel->GetMap()->GetHeight());
		
		return Vec2i(brickX, brickY);
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(StringHash(ACT_FUNC_SELECTED))) {
			auto which = msg.GetDataS<ValueChangeEvent<HydroqAction>>();

			selectedAction = which->after;

			if (which->after == HydroqAction::ATTRACT) {
				floatingGameScene->SetListenerState(ListenerState::DISABLED);
			}
			else if (which->after == HydroqAction::NONE) {
				floatingGameScene->SetListenerState(ListenerState::ACTIVE_ALL);
			}
		}
		else {
			if (selectedAction == HydroqAction::ATTRACT) {

				if (msg.GetSourceObject()->GetId() == owner->GetId()) {

					if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {
						InputEvent* touch = msg.GetDataS<InputEvent>();
						ofVec2f endPos = touch->input->position;
						Vec2i brickPos = GetBrickPosition(endPos);

						auto clickedNode = gameModel->GetMap()->GetNode(brickPos.x,brickPos.y);

						if (attrPlaced >= 3) {
							// remove first attractor
							auto firstAttr = attractors[0];
							attractors.erase(attractors.begin());
							Vec2i brickPosition = firstAttr->GetAttr<Vec2i>(ATTR_BRICK_POS);
							auto gameModel = GETCOMPONENT(HydroqGameModel);
							gameModel->DestroyAttractor(brickPosition, gameModel->GetFaction());
							owner->RemoveChild(firstAttr, true);
							attrPlaced--;
						}

						InsertAttractor(endPos, brickPos);
					}
					else if (msg.HasAction(ACT_OBJECT_HIT_OVER)) {
						if (attractorPlaced) {
							InputEvent* touch = msg.GetDataS<InputEvent>();
							ofVec2f endPos = touch->input->position;
							auto& trans = placedAttractor->GetTransform();
							auto shape = placedAttractor->GetShape<spt<Image>>();

							auto absRadius = 0.5f * owner->GetTransform().absScale.x*trans.scale.x*shape->GetWidth();
							auto absPos = trans.absPos + absRadius;

							auto distance = endPos.distance(absPos);
							auto radiusRatio = distance / absRadius;
							auto newScale = trans.scale*radiusRatio;

							if (newScale.x >= 1.0f && newScale.x <= 3.0f) {
								
								Vec2i brickPosition = placedAttractor->GetAttr<Vec2i>(ATTR_BRICK_POS);
								auto gameModel = GETCOMPONENT(HydroqGameModel);
								gameModel->ChangeAttractorCardinality(brickPosition, gameModel->GetFaction(), newScale.x / 3.0f);

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
							auto playerModel = GETCOMPONENT(HydroqPlayerModel);
							playerModel->SetHydroqAction(HydroqAction::NONE);
						}
						placedAttractor = nullptr;
						attractorPlaced = false;
					}
				}
			}
			else {
				if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
					if (msg.GetSourceObject()->GetId() == owner->GetId()) {
						InputEvent* touch = msg.GetDataS<InputEvent>();
						ofVec2f endPos = touch->input->position;

						// remove on double-click
						Node* clicked = GetClickedAttractor(endPos);

						if (clickedAttractor != nullptr && clicked != nullptr && clicked->GetId() == clickedAttractor->GetId() && 
							clickedAttrPos.distance(endPos) <= SCREEN_TOLERANCE) {
							touch->input->SetIsProcessed(true);
							RemoveAttractor(endPos);
						}
						else {
							clickedAttractor = clicked;
							clickedAttrPos = endPos;
						}
					}
				}
			}
		}
	}

	void RefreshAttractorPosition(ofVec2f absPosition, ofVec2f scale) {
		TransformMath math = TransformMath();
		TransformEnt ent = TransformEnt(absPosition, 1, CalcType::ABS, ofVec2f(0.5f), scale, CalcType::LOC);
		math.SetTransform(placedAttractor, owner, ent, 0, 0);
	}

	ofVec2f clickedAttrPos = ofVec2f();
	Node* clickedAttractor = nullptr;

	Node* GetClickedAttractor(ofVec2f position) {
		for (auto it = attractors.begin(); it != attractors.end(); ++it) {
			auto node = (*it);
			auto shape = node->GetShape<spt<Image>>();
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

	bool RemoveAttractor(ofVec2f position) {
		for (auto it = attractors.begin(); it != attractors.end(); ++it) {
			auto node = (*it);
			auto shape = node->GetShape<spt<Image>>();
			float x = node->GetTransform().absPos.x;
			float y = node->GetTransform().absPos.y;

			auto absLength = owner->GetTransform().absScale.x*node->GetTransform().scale.x*shape->GetWidth();
			float width = x + absLength;
			float height = y + absLength;

			if (position.x >= x && position.x <= width &&position.y >= y &&position.y <= height) {
				
				Vec2i brickPosition = node->GetAttr<Vec2i>(ATTR_BRICK_POS);
				attractors.erase(it);

				auto gameModel = GETCOMPONENT(HydroqGameModel);
				gameModel->DestroyAttractor(brickPosition, gameModel->GetFaction());
				owner->RemoveChild(node, true);
				attrPlaced--;
				return true;
			}

		}

		return false;
	}

	void InsertAttractor(ofVec2f position, Vec2i brickPos) {
		
		placedAttractor = new Node("attractor");
		if (selectedAction == HydroqAction::ATTRACT) {
			placedAttractor->SetShape(spt<Image>(new Image(CogGet2DImage("game/functions_cmd_attract_placement.png"))));
		}

		attrPlaced++;

		placedAttractor->AddAttr(ATTR_BRICK_POS, brickPos);
		auto gameModel = GETCOMPONENT(HydroqGameModel);
		gameModel->AddAttractor(brickPos, gameModel->GetFaction(), 1);

		attractors.push_back(placedAttractor);
		RefreshAttractorPosition(position, ofVec2f(1));
		owner->AddChild(placedAttractor);

		attractorPlaced = true;
	}


public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		for (auto& attr : attractors) {
			attr->GetTransform().rotation += 0.4f;
		}
	}
};