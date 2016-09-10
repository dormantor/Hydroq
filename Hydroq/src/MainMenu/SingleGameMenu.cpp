#include "SingleGameMenu.h"
#include "Facade.h"
#include "ofxXmlSettings.h"
#include "PlayerModel.h"
#include "Tween.h"
#include "Stage.h"
#include "Node.h"
#include "ComponentStorage.h"
#include "Scene.h"
#include "TransformEnt.h"
#include "TransformMath.h"

void SingleGameMenu::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED);

	// load map config
	auto xml = CogLoadXMLFile("config/mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	LoadMaps();
}


void SingleGameMenu::OnMessage(Msg& msg) {

	if (msg.HasAction(ACT_BUTTON_CLICKED)) {
		if (msg.GetContextNode()->GetTag().compare("play_but") == 0) {
			// click on play button -> switch scene
			auto model = GETCOMPONENT(PlayerModel);
			model->StartGame(GetSelectedFaction(), selectedMap);
			auto sceneContext = GETCOMPONENT(Stage);
			auto scene = sceneContext->FindSceneByName("game");
			sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
	else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetContextNode()->IsInGroup(StrId("SELECTION_MAP"))) {
		string map = msg.GetContextNode()->GetAttr<string>(ATTR_MAP);
		ShowMapPreview(map);
		selectedMap = map;
	}
}

void SingleGameMenu::LoadMaps() {
	auto set = this->mapConfig.GetSetting("maps_icons");
	auto items = set.items;

	Node* maps_list = owner->GetScene()->FindNodeByTag("maps_list");
	int index = 0;

	for (auto& key : items) {
		auto val = key.second;
		string mapName = val.key;
		Helper::SetPanelItem(owner, maps_list, index, StrId("SELECTION_MAP"), 
			ATTR_MAP, mapName, index == 0); // select first map

		if (index == 0) {
			selectedMap = mapName;
			ShowMapPreview(mapName);
		}
		index++;
	}
}

void SingleGameMenu::ShowMapPreview(string map) {
	auto path = mapConfig.GetSettingVal("maps_icons", map);
	auto image = CogGet2DImage(path);
	auto mapIconNode = owner->GetScene()->FindNodeByTag("map_icon");
	mapIconNode->GetMesh<Image>()->SetImage(image);

	// correct the size and position
	TransformEnt ent = TransformEnt(ofVec2f(0.5f), 10, CalcType::PER, ofVec2f(0.5f), ofVec2f(1), CalcType::LOC);
	TransformMath math = TransformMath();
	math.SetTransform(mapIconNode, mapIconNode->GetParent(), ent);

}

Faction SingleGameMenu::GetSelectedFaction() {
	auto factRed = owner->GetScene()->FindNodeByTag("faction_red");
	auto factBlue = owner->GetScene()->FindNodeByTag("faction_blue");

	if (factRed->HasState(StrId(STATES_SELECTED))) {
		return Faction::RED;
	}
	else {
		return Faction::BLUE;
	}
}
