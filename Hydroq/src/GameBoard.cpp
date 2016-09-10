#include "GameBoard.h"
#include "HydroqDef.h"
#include "GameModel.h"
#include "GameView.h"
#include "Mesh.h"


void GameBoard::OnInit() {
	cache = GETCOMPONENT(ResourceCache);
	auto gameModel = owner->GetBehavior<GameModel>();

	auto xml = CogLoadXMLFile("mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();

	// 3) load static sprites and assign it to the map_board node
	auto gameView = owner->GetBehavior<GameView>();
	gameView->LoadSprites(mapConfig.GetSetting("sprites"));

	auto staticSprites = gameView->GetStaticSprites();
	owner->GetScene()->FindNodeByTag("map_board")->SetMesh(staticSprites);

	// 4) load dynamic sprites and assign it to the object_board node
	auto dynamicSprites = gameView->GetDynamicSprites();
	owner->GetScene()->FindNodeByTag("object_board")->SetMesh(dynamicSprites);

	// ZOOM THE BOARD LITTLE BIT OUT
	owner->GetTransform().scale /= 2.5f;

	auto boardShape = owner->GetMesh<Cog::Rectangle>();
	boardShape->SetWidth(staticSprites->GetWidth());
	boardShape->SetHeight(staticSprites->GetHeight());

	Faction fact = gameModel->GetFaction();
	auto ownerRig = gameModel->GetRigsByFaction(fact)[0];
	auto ownerRigPos = ownerRig->GetTransform().localPos;
	auto mapWidth = gameModel->GetMap()->GetWidth();
	auto mapHeight = gameModel->GetMap()->GetHeight();

	// zoom to rig position
	ZoomIntoPositionCenter(ofVec2f((ownerRigPos.x + 1.0f) / mapWidth, (ownerRigPos.y + 1.0f) / mapHeight));
}


void GameBoard::ZoomIntoPositionCenter(ofVec2f positionRelative) {

	auto& transform = owner->GetTransform();
	// recalculate transform
	transform.CalcAbsTransform(owner->GetParent()->GetTransform());

	float shapeWidth = owner->GetMesh()->GetWidth();
	float shapeHeight = owner->GetMesh()->GetHeight();
	float width = shapeWidth*owner->GetTransform().absScale.x;
	float height = shapeHeight*owner->GetTransform().absScale.y;
	auto screenSize = CogGetVirtualScreenSize();

	ofVec3f newAbsPos = ofVec3f(-width*positionRelative.x + screenSize.x / 2, -height*positionRelative.y + screenSize.y / 2, transform.absPos.z);
	SetNewPosition(transform, newAbsPos);
}

void GameBoard::SetNewPosition(Trans& transform, ofVec3f& newAbsPos) {

	CheckNewPosition(transform, newAbsPos);
	TransformMath math = TransformMath();
	// calc new local position from absolute position
	auto newLocalPos = math.CalcPosition(owner, owner->GetParent(), newAbsPos, CalcType::ABS, 0, 0);

	// set new local position
	transform.localPos.x = newLocalPos.x;
	transform.localPos.y = newLocalPos.y;
}

void GameBoard::CheckNewPosition(Trans& transform, ofVec3f& newPos) {
	float shapeWidth = owner->GetMesh()->GetWidth();
	float shapeHeight = owner->GetMesh()->GetHeight();

	// calculate absolute width and height
	float width = shapeWidth*transform.absScale.x;
	float height = shapeHeight*transform.absScale.y;

	// check bounds
	if (newPos.x > 0) newPos.x = 0;
	if (newPos.y > 0) newPos.y = 0;
	if (-newPos.x + CogGetScreenWidth() > (width)) newPos.x = CogGetScreenWidth() - width;
	if (-newPos.y + CogGetScreenHeight() > (height)) newPos.y = CogGetScreenHeight() - height;
}

