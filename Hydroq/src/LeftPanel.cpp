#include "LeftPanel.h"
#include "GameModel.h"
#include "ComponentStorage.h"
#include "TransformAnim.h"
#include "TransformMath.h"

void LeftPanel::OnInit() {
	SubscribeForMessages(ACT_OBJECT_HIT_ENDED, ACT_OBJECT_HIT_OVER, ACT_TRANSFORM_ENDED, ACT_COUNTER_CHANGED);
}

void LeftPanel::OnStart() {
	originTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(0, 0), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(1), CalcType::LOC, 0));
	animTrans = spt<TransformEnt>(new TransformEnt("", ofVec2f(6.0f, 0), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(1), CalcType::LOC, 0));

	owner->SetState(StrId(STATES_ENABLED));
	playerModel = GETCOMPONENT(PlayerModel);

	// load all nodes by their names
	counterUnits = owner->GetScene()->FindNodeByTag("counter_units");
	counterBuildings = owner->GetScene()->FindNodeByTag("counter_buildings");

	mapIcon = owner->GetScene()->FindNodeByTag("mapboard");
	mapBorder = owner->GetScene()->FindNodeByTag("mapborder");
	gameBoard = owner->GetScene()->FindNodeByTag("gameboard");
}


void LeftPanel::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_OBJECT_HIT_ENDED) && msg.GetContextNode()->GetId() == owner->GetId()) {
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
		// refresh number of objects and rigs
		RefreshCounters();
	}
	else if (msg.HasAction(ACT_OBJECT_HIT_OVER) && msg.GetContextNode()->GetId() == mapIcon->GetId()) {
		auto evt = msg.GetData<InputEvent>();
		HandleMapDetailHit(evt);
	}
}

void LeftPanel::RefreshCounters() {
	counterUnits->GetMesh<Text>()->SetText(ofToString(playerModel->GetUnits()));
	counterBuildings->GetMesh<Text>()->SetText(ofToString(playerModel->GetRigs()));
}


void LeftPanel::ReloadMapIcon(string pathToFile) {
	auto image = CogGet2DImage(pathToFile);
	auto shape = this->mapIcon->GetMesh<Image>();
	shape->SetImage(image);

	// calculate size of the map icon
	ofVec2f size = shape->GetWidth() > shape->GetHeight() ? ofVec2f(0.55f, 0) : ofVec2f(0, 0.55f);

	// reload transform
	TransformEnt trans = TransformEnt(ofVec2f(0.345f, 0.55f), 10, CalcType::PER, ofVec2f(0.5f), size, CalcType::PER);
	TransformMath math = TransformMath();
	math.SetTransform(mapIcon, mapIcon->GetParent(), trans);
}


void LeftPanel::ReloadMapBorder() {
	// map icon position and size
	auto mapIconPos = mapIcon->GetTransform().absPos;
	float mapIconWidth = mapIcon->GetMesh()->GetWidth()*mapIcon->GetTransform().absScale.x;
	float mapIconHeight = mapIcon->GetMesh()->GetHeight()*mapIcon->GetTransform().absScale.y;

	// game board position and size
	auto gameBoardPos = gameBoard->GetTransform().absPos;
	float gameBoardWidth = gameBoard->GetMesh()->GetWidth()*gameBoard->GetTransform().absScale.x;
	float gameBoardHeight = gameBoard->GetMesh()->GetHeight()*gameBoard->GetTransform().absScale.y;


	// relative % position of the map
	float leftCornerX = (-gameBoardPos.x) / gameBoardWidth;
	float leftCornerY = (-gameBoardPos.y) / gameBoardHeight;
	float zoomX = gameBoardWidth / CogGetVirtualScreenSize().x;
	float zoomY = gameBoardHeight / CogGetVirtualScreenSize().y;

	// map border position and size
	auto borderShape = mapBorder->GetMesh<Plane>();
	auto mapBorderPos = mapBorder->GetTransform().absPos;
	float mapBorderWidth = borderShape->GetWidth()*mapBorder->GetTransform().absScale.x;
	float mapBorderHeight = borderShape->GetHeight()*mapBorder->GetTransform().absScale.y;

	// calculations that sets the border position and size
	float neededScaleX = mapIconWidth / mapBorderWidth / zoomX;
	float neededScaleY = mapIconHeight / mapBorderHeight / zoomY;

	float mapRatio = mapIconWidth / mapIconHeight;

	// set width and height of the border shape so that it will match the visible part of game board
	if (neededScaleX > 0.1f || neededScaleY > 0.1f) {
		borderShape->SetWidth(borderShape->GetWidth()*neededScaleX);
		borderShape->SetHeight(borderShape->GetHeight()*neededScaleY);
	}

	// calculate desired absolute position
	ofVec2f neededAbsPos = ofVec2f(mapIconPos.x + mapIconWidth*leftCornerX, mapIconPos.y + mapIconHeight*leftCornerY);

	TransformMath math = TransformMath();
	// calc new local position from absolute position
	auto newLocalPos = math.CalcPosition(mapBorder, mapBorder->GetParent(), neededAbsPos, CalcType::ABS, 0, 0);

	if (newLocalPos.distance(mapBorder->GetTransform().localPos) > 1) {
		mapBorder->GetTransform().localPos = ofVec3f(newLocalPos.x, newLocalPos.y, 120);
	}
}

void LeftPanel::HandleMapDetailHit(spt<InputEvent> evt) {
	auto mapIconPos = mapIcon->GetTransform().absPos;
	float mapIconWidth = mapIcon->GetMesh()->GetWidth()*mapIcon->GetTransform().absScale.x;
	float mapIconHeight = mapIcon->GetMesh()->GetHeight()*mapIcon->GetTransform().absScale.y;

	auto clickPos = evt->input->position;

	// calculate relative position on the game board
	auto relativeClickX = (clickPos.x - mapIconPos.x) / mapIconWidth;
	auto relativeClickY = (clickPos.y - mapIconPos.y) / mapIconHeight;

	// zoom to game board
	auto hydroqBoard = gameBoard->GetBehavior<GameBoard>();
	hydroqBoard->ZoomIntoPositionCenter(ofVec2f(relativeClickX, relativeClickY));
}

void LeftPanel::Update(const uint64 delta, const uint64 absolute) {
	if (this->gameModel == nullptr) {
		// first update -> load map icon
		gameModel = gameBoard->GetBehavior<GameModel>();
		auto& mapConfig = gameModel->GetMap()->GetMapConfig();
		string mapIconPath = mapConfig.GetSettingVal("maps_icons", gameModel->GetMapName());
		ReloadMapIcon(mapIconPath);
		ReloadMapBorder();
		RefreshCounters();
	}

	// reload map border according to the visible part of game board
	ReloadMapBorder();
}
