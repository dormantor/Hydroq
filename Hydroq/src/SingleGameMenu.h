#pragma once

#include "ofxCogMain.h"
#include "HydroqGameModel.h"

class SingleGameMenu : public Behavior {
	OBJECT_PROTOTYPE(SingleGameMenu)

	Settings mapConfig;
	string selectedMap;

	void OnInit() {
		RegisterListening(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED);


		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);
		LoadMaps();
	}


	void OnMessage(Msg& msg) {
		
		if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			 if (msg.GetSourceObject()->GetTag().compare("play_but") == 0) {
				// click on play button -> switch scene
				auto model = GETCOMPONENT(HydroqGameModel);
				model->InitModel(GetSelectedFaction(), selectedMap, false);
				auto sceneContext = GETCOMPONENT(Stage);
				auto scene = sceneContext->FindSceneByName("game");
				sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
			}
		} else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_MAP"))) {
			string map = msg.GetSourceObject()->GetAttr<string>(ATTR_MAP);
			ShowMapPreview(map);
			selectedMap = map;
		}
	}

	void LoadMaps() {
		auto set = this->mapConfig.GetSetting("maps_icons");
		auto items = set.items;

		Node* maps_list = owner->GetScene()->FindNodeByTag("maps_list");
		int index = 0;

		for (auto& key : items) {
			auto val = key.second;
			string mapName = val.key;
			Helper::SetPanelItem(owner, maps_list, index, StringHash("SELECTION_MAP"), ATTR_MAP, mapName, index == 0); // select first map
			
			if (index == 0) {
				selectedMap = mapName;
				ShowMapPreview(mapName);
			}
			index++;
		}
	}

	void ShowMapPreview(string map) {
		auto path = mapConfig.GetSettingVal("maps_icons", map);
		auto image = CogGet2DImage(path);
		auto mapIconNode = owner->GetScene()->FindNodeByTag("map_icon");
		mapIconNode->GetShape<spt<Image>>()->SetImage(image);

		TransformEnt ent = TransformEnt(ofVec2f(0.5f), 10, CalcType::PER, ofVec2f(0.5f), ofVec2f(1), CalcType::LOC);
		TransformMath math = TransformMath();
		math.SetTransform(mapIconNode, mapIconNode->GetParent(), ent);

	}

	Faction GetSelectedFaction() {
		auto factRed = owner->GetScene()->FindNodeByTag("faction_red");
		auto factBlue = owner->GetScene()->FindNodeByTag("faction_blue");

		if (factRed->HasState(StringHash(STATES_SELECTED))) {
			return Faction::RED;
		}
		else {
			return Faction::BLUE;
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

