#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
#include "HydroqDef.h"
#include "HydroqGameModel.h"
#include "HydroqSpriteManager.h"
#include "HydroqGameView.h"

class HydroqBoard : public Behavior {
	OBJECT_PROTOTYPE(HydroqBoard)

private:
	ResourceCache* cache;
	Settings mapConfig;

public:

	BrickMap* LoadBricks() {
		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);
		xml->popTag();

		// load map
		MapLoader mapLoad = MapLoader();
		return mapLoad.LoadFromPNGImage("map_2.png", mapConfig.GetSetting("names"));
	}

	void Init() {
		cache = GETCOMPONENT(ResourceCache);

		// 1) load collection of bricks
		auto bricks = LoadBricks();

		// 2) create game map
		auto gameModel = GETCOMPONENT(HydroqGameModel);
		gameModel->GetMap()->LoadMap(bricks);

		// 3) create collection of all sprites
		auto spriteManager = GETCOMPONENT(HydroqSpriteManager);
		spriteManager->LoadSprites(mapConfig.GetSetting("sprites"));
		
		// 4) load static sprites and assign it to the map_board node
		auto gameView = GETCOMPONENT(HydroqGameView);
		gameView->LoadSprites();

		auto staticSprites = gameView->GetStaticSprites();
		owner->GetScene()->FindNodeByTag("map_board")->SetShape(staticSprites);

		// 5) load dynamic sprites and assign it to the object_board node
		auto dynamicSprites = gameView->GetDynamicSprites();
		owner->GetScene()->FindNodeByTag("object_board")->SetShape(dynamicSprites);

		// ZOOM THE BOARD LITTLE BIT OUT
		owner->GetTransform().scale /= 1.5f;

		auto boardShape = owner->GetShape<spt<Cog::Rectangle>>();
		boardShape->SetWidth(staticSprites->GetWidth());
		boardShape->SetHeight(staticSprites->GetHeight());
	}

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};