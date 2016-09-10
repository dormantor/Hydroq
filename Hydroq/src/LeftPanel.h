#pragma once

#include "ofxCogMain.h"
#include "HydroqPlayerModel.h"
#include "HydroqDef.h"
#include "Board.h"

class LeftPanel : public Behavior {

	spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;
	HydroqPlayerModel* playerModel;
	HydroqGameModel* gameModel = nullptr;
public:
	LeftPanel() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_OBJECT_HIT_ENDED, ACT_OBJECT_HIT_OVER, ACT_TRANSFORM_ENDED, ACT_COUNTER_CHANGED);
	}

	void OnStart() {
		originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(0, 0), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(1), CalcType::LOC, 0));
		animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(6.0f, 0), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));

		owner->SetState(StrId(STATES_ENABLED));
		playerModel = GETCOMPONENT(HydroqPlayerModel);

		counterUnits = owner->GetScene()->FindNodeByTag("counter_units");
		counterBuildings = owner->GetScene()->FindNodeByTag("counter_buildings");

		mapIcon = owner->GetScene()->FindNodeByTag("mapboard");
		mapBorder = owner->GetScene()->FindNodeByTag("mapborder");
		gameBoard = owner->GetScene()->FindNodeByTag("gameboard");
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_OBJECT_HIT_ENDED) && msg.GetSourceObject()->GetId() == owner->GetId()) {
			if (owner->HasState(StrId(STATES_ENABLED))) {
				owner->ResetState(StrId(STATES_ENABLED));
				// roll menu back
				Node* menu = owner->GetScene()->FindNodeByTag("leftpanel");
				TransformAnim* anim = new TransformAnim(originTrans, animTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
			}
			else {
				owner->SetState(StrId(STATES_ENABLED));

				// roll the menu
				Node* menu = owner->GetScene()->FindNodeByTag("leftpanel");
				TransformAnim* anim = new TransformAnim(animTrans, originTrans, 250, 0, false, AnimBlend::OVERLAY);
				menu->AddBehavior(anim);
			}
		}
		else if (msg.HasAction(ACT_COUNTER_CHANGED)) {
			RefreshCounters();
		}
		else if (msg.HasAction(ACT_OBJECT_HIT_OVER) && msg.GetSourceObject()->GetId() == mapIcon->GetId()) {
			InputEvent* evt = msg.GetData<InputEvent>();
			HandleMapDetailHit(evt);
		}
	}

private:

	Node* counterUnits;
	Node* counterBuildings;

	void RefreshCounters() {
		counterUnits->GetShape<spt<Text>>()->SetText(ofToString(playerModel->GetUnits()));
		counterBuildings->GetShape<spt<Text>>()->SetText(ofToString(playerModel->GetBuildings()));
	}

	Node* mapIcon = nullptr;

	void ReloadMapIcon(string pathToFile) {
		auto image = CogGet2DImage(pathToFile);
		auto shape = this->mapIcon->GetShape<spt<Image>>();
		shape->SetImage(image);

		ofVec2f size = shape->GetWidth() > shape->GetHeight() ? ofVec2f(0.55f, 0) : ofVec2f(0, 0.55f);

		// reload transform
		TransformEnt trans = TransformEnt(ofVec2f(0.345f, 0.55f), 10, CalcType::PER, ofVec2f(0.5f), size, CalcType::PER);
		TransformMath math = TransformMath();
		math.SetTransform(mapIcon, mapIcon->GetParent(), trans);
	}

	Node* mapBorder = nullptr;
	Node* gameBoard = nullptr;

	void ReloadMapBorder() {
		// map icon position and size
		auto mapIconPos = mapIcon->GetTransform().absPos;
		float mapIconWidth = mapIcon->GetShape()->GetWidth()*mapIcon->GetTransform().absScale.x;
		float mapIconHeight = mapIcon->GetShape()->GetHeight()*mapIcon->GetTransform().absScale.y;

		// game board position and size
		auto gameBoardPos = gameBoard->GetTransform().absPos;
		float gameBoardWidth = gameBoard->GetShape()->GetWidth()*gameBoard->GetTransform().absScale.x;
		float gameBoardHeight = gameBoard->GetShape()->GetHeight()*gameBoard->GetTransform().absScale.y;


		// relative % position of the map
		float leftCornerX = (-gameBoardPos.x) / gameBoardWidth;
		float leftCornerY = (-gameBoardPos.y) / gameBoardHeight;
		float zoomX = gameBoardWidth / CogGetVirtualScreenSize().x;
		float zoomY = gameBoardHeight / CogGetVirtualScreenSize().y;

		// map border position and size
		auto borderShape = mapBorder->GetShape<spt<Plane>>();
		auto mapBorderPos = mapBorder->GetTransform().absPos;
		float mapBorderWidth = borderShape->GetWidth()*mapBorder->GetTransform().absScale.x;
		float mapBorderHeight = borderShape->GetHeight()*mapBorder->GetTransform().absScale.y;

		// calculations that sets the border position and size
		float neededScaleX = mapIconWidth / mapBorderWidth / zoomX;
		float neededScaleY = mapIconHeight / mapBorderHeight / zoomY;

		float mapRatio = mapIconWidth / mapIconHeight;

		if (neededScaleX > 0.1f || neededScaleY > 0.1f) {
			borderShape->SetWidth(borderShape->GetWidth()*neededScaleX);
			borderShape->SetHeight(borderShape->GetHeight()*neededScaleY);
		}

		ofVec2f neededAbsPos = ofVec2f(mapIconPos.x + mapIconWidth*leftCornerX, mapIconPos.y + mapIconHeight*leftCornerY);
		
		TransformMath math = TransformMath();
		// calc new local position from absolute position
		auto newLocalPos = math.CalcPosition(mapBorder, mapBorder->GetParent(), neededAbsPos, CalcType::ABS, 0, 0);
		
		if (newLocalPos.distance(mapBorder->GetTransform().localPos) > 1) {
			mapBorder->GetTransform().localPos = ofVec3f(newLocalPos.x, newLocalPos.y, 120);
		}
	}

	void HandleMapDetailHit(InputEvent* evt) {
		auto mapIconPos = mapIcon->GetTransform().absPos;
		float mapIconWidth = mapIcon->GetShape()->GetWidth()*mapIcon->GetTransform().absScale.x;
		float mapIconHeight = mapIcon->GetShape()->GetHeight()*mapIcon->GetTransform().absScale.y;

		auto clickPos = evt->input->position;

		auto relativeClickX = (clickPos.x - mapIconPos.x) / mapIconWidth;
		auto relativeClickY = (clickPos.y - mapIconPos.y) / mapIconHeight;

		auto hydroqBoard = gameBoard->GetBehavior<HydroqBoard>();
		hydroqBoard->ZoomIntoPositionCenter(ofVec2f(relativeClickX, relativeClickY));
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (this->gameModel == nullptr) {
			gameModel = gameBoard->GetBehavior<HydroqGameModel>();
			auto& mapConfig = gameModel->GetMap()->GetMapConfig();
			string mapIconPath = mapConfig.GetSettingVal("maps_icons", gameModel->GetMapName());
			ReloadMapIcon(mapIconPath);
			ReloadMapBorder();

			RefreshCounters();
		}

		ReloadMapBorder();
	}
};