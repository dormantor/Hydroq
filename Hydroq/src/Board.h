#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
#include "HydroqDef.h"
#include "HydroqGameModel.h"
#include "HydroqGameView.h"

class HydroqBoard : public Behavior {
	OBJECT_PROTOTYPE(HydroqBoard)

private:
	ResourceCache* cache;
	Settings mapConfig;

public:

	BrickMap* LoadBricks(string selectedMap) {
		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);
		xml->popTag();

		string mapPath = mapConfig.GetSettingVal("maps_files", selectedMap);

		if (mapPath.empty()) throw ConfigErrorException(string_format("Path to map %s not found",  selectedMap.c_str()));

		// load map
		MapLoader mapLoad = MapLoader();
		return mapLoad.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names"));
	}

	void OnInit() {
		cache = GETCOMPONENT(ResourceCache);
		auto gameModel = GETCOMPONENT(HydroqGameModel);
		string selectedMap = gameModel->GetMapName();

		// 1) load collection of bricks
		auto bricks = LoadBricks(selectedMap);

		// 2) create game map
		gameModel->GetMap()->LoadMap(bricks);

		// 3) load static sprites and assign it to the map_board node
		auto gameView = GETCOMPONENT(HydroqGameView);
		gameView->LoadSprites(mapConfig.GetSetting("sprites"));

		auto staticSprites = gameView->GetStaticSprites();
		owner->GetScene()->FindNodeByTag("map_board")->SetShape(staticSprites);

		// 4) load dynamic sprites and assign it to the object_board node
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