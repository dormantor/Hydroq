#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
#include "HydroqDef.h"
#include "HydroqGameModel.h"
#include "HydroqGameView.h"
#include "Shape.h"


class HydroqBoard : public Behavior {
	
private:
	ResourceCache* cache;
	Settings mapConfig;

public:

	HydroqBoard() {

	}

	void OnInit() {
		cache = GETCOMPONENT(ResourceCache);
		auto gameModel = owner->GetBehavior<HydroqGameModel>();
		
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);
		xml->popTag();

		// 3) load static sprites and assign it to the map_board node
		auto gameView = owner->GetBehavior<HydroqGameView>();
		gameView->LoadSprites(mapConfig.GetSetting("sprites"));

		auto staticSprites = gameView->GetStaticSprites();
		owner->GetScene()->FindNodeByTag("map_board")->SetShape(staticSprites);

		// 4) load dynamic sprites and assign it to the object_board node
		auto dynamicSprites = gameView->GetDynamicSprites();
		owner->GetScene()->FindNodeByTag("object_board")->SetShape(dynamicSprites);

		// ZOOM THE BOARD LITTLE BIT OUT
		owner->GetTransform().scale /= 2.5f;

		auto boardShape = owner->GetShape<spt<Cog::Rectangle>>();
		boardShape->SetWidth(staticSprites->GetWidth());
		boardShape->SetHeight(staticSprites->GetHeight());

		Faction fact = gameModel->GetFaction();
		auto ownerRig = gameModel->GetRigsByFaction(fact)[0];
		auto ownerRigPos = ownerRig->GetTransform().localPos;
		auto mapWidth = gameModel->GetMap()->GetWidth();
		auto mapHeight = gameModel->GetMap()->GetHeight();

		// zoom to rig position
		ZoomIntoPositionCenter(ofVec2f((ownerRigPos.x+1.0f) / mapWidth, (ownerRigPos.y+1.0f) / mapHeight));
	}


	void ZoomIntoPositionCenter(ofVec2f positionRelative) {
		
		auto& transform = owner->GetTransform();
		// recalculate transform
		transform.CalcAbsTransform(owner->GetParent()->GetTransform());

		float shapeWidth = owner->GetShape()->GetWidth();
		float shapeHeight = owner->GetShape()->GetHeight();
		float width = shapeWidth*owner->GetTransform().absScale.x;
		float height = shapeHeight*owner->GetTransform().absScale.y;
		auto screenSize = CogGetVirtualScreenSize();

		ofVec3f newAbsPos = ofVec3f(-width*positionRelative.x + screenSize.x/2, -height*positionRelative.y + screenSize.y/2,transform.absPos.z);
		SetNewPosition(transform, newAbsPos);
	}

	void SetNewPosition(Trans& transform, ofVec3f& newAbsPos) {

		CheckNewPosition(transform, newAbsPos);
		TransformMath math = TransformMath();
		// calc new local position from absolute position
		auto newLocalPos = math.CalcPosition(owner, owner->GetParent(), newAbsPos, CalcType::ABS, 0, 0);

		// set new local position
		transform.localPos.x = newLocalPos.x;
		transform.localPos.y = newLocalPos.y;
	}

	void CheckNewPosition(Trans& transform, ofVec3f& newPos) {
		float shapeWidth = owner->GetShape()->GetWidth();
		float shapeHeight = owner->GetShape()->GetHeight();

		// calculate absolute width and height
		float width = shapeWidth*transform.absScale.x;
		float height = shapeHeight*transform.absScale.y;

		// check bounds
		if (newPos.x > 0) newPos.x = 0;
		if (newPos.y > 0) newPos.y = 0;
		if (-newPos.x + CogGetScreenWidth() > (width)) newPos.x = CogGetScreenWidth() - width;
		if (-newPos.y + CogGetScreenHeight() > (height)) newPos.y = CogGetScreenHeight() - height;
	}


	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};