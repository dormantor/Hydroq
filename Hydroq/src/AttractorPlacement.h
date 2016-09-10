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


	void OnMessage(Msg& msg) {
		if (msg.HasAction(StringHash(ACT_FUNC_SELECTED))) {
			auto which = msg.GetDataS<ValueChangeEvent<HydroqAction>>();

			selectedAction = which->after;

			if (which->after == HydroqAction::GATHER) {
				floatingGameScene->SetListenerState(ListenerState::DISABLED);
			}
			else if (which->after == HydroqAction::ATTACK) {
				floatingGameScene->SetListenerState(ListenerState::DISABLED);
			}
			else if (which->after == HydroqAction::NONE) {
				floatingGameScene->SetListenerState(ListenerState::ACTIVE_ALL);
			}
		}
		else {
			if (selectedAction == HydroqAction::GATHER || selectedAction == HydroqAction::ATTACK) {

				if (msg.GetSourceObject()->GetId() == owner->GetId()) {

					if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {
						InputEvent* touch = msg.GetDataS<InputEvent>();
						ofVec2f endPos = touch->input->position;
						auto shape = owner->GetShape();
						touch->input->SetIsProcessed(true);

						if (TryRemoveAttractor(endPos)) return;

						// get pressed brick
						float shapeWidth = shape->GetWidth()*owner->GetTransform().absScale.x;
						float shapeHeight = shape->GetHeight()*owner->GetTransform().absScale.y;
						float absPosX = owner->GetTransform().absPos.x;
						float absPosY = owner->GetTransform().absPos.y;

						float distX = (endPos.x - absPosX);
						float distY = (endPos.y - absPosY);

						// get brick indices
						int brickX = (int)((distX / shapeWidth)*gameModel->GetMap()->GetWidth());
						int brickY = (int)((distY / shapeHeight)*gameModel->GetMap()->GetHeight());

						auto clickedNode = gameModel->GetMap()->GetNode(brickX, brickY);

						if (attrPlaced < 3) {
							InsertAttractor(endPos);
						}
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
								trans.scale = newScale;

								RefreshAttractorPosition(absPos, trans.scale);
							}
						}
					}
					else if (msg.HasAction(ACT_OBJECT_HIT_LOST) || msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
						placedAttractor = nullptr;
						attractorPlaced = false;
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

	bool TryRemoveAttractor(ofVec2f position) {
		for (auto it = attractors.begin(); it != attractors.end(); ++it) {
			auto node = (*it);
			auto shape = node->GetShape<spt<Image>>();
			float x = node->GetTransform().absPos.x;
			float y = node->GetTransform().absPos.y;

			auto absLength = owner->GetTransform().absScale.x*node->GetTransform().scale.x*shape->GetWidth();
			float width = x + absLength;
			float height = y + absLength;

			if (position.x >= x && position.x <= width &&position.y >= y &&position.y <= height) {
				attractors.erase(it);
				owner->RemoveChild(node, true);
				attrPlaced--;
				return true;
			}

		}

		return false;
	}

	void InsertAttractor(ofVec2f position) {
		
		placedAttractor = new Node("attractor");
		if (selectedAction == HydroqAction::GATHER) {
			placedAttractor->SetShape(spt<Image>(new Image(CogGet2DImage("game/functions_cmd_gather.png"))));
		}
		else {
			placedAttractor->SetShape(spt<Image>(new Image(CogGet2DImage("game/functions_cmd_attack.png"))));
		}

		attrPlaced++;
		attractors.push_back(placedAttractor);
		RefreshAttractorPosition(position, ofVec2f(1));
		owner->AddChild(placedAttractor);

		attractorPlaced = true;
	}


public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};