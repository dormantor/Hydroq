#include "MultiplayerMenu.h"
#include "Node.h"
#include "MultiSelection.h"
#include "TransformMath.h"
#include "Scene.h"

void MultiplayerMenu::AddServer(string ip, int index) {

	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");
	Helper::SetPanelItem(owner, hosts_list, index, StringHash("SELECTION_SERVER"), ATTR_SERVER_IP, ip);
}

void MultiplayerMenu::LoadMaps() {
	auto set = this->mapConfig.GetSetting("maps_icons");
	auto items = set.items;

	Node* maps_list = owner->GetScene()->FindNodeByTag("maps_list");
	int index = 0;

	for (auto& key : items) {
		auto val = key.second;
		string mapName = val.key;
		Helper::SetPanelItem(owner, maps_list, index, StringHash("SELECTION_MAP"), ATTR_MAP, mapName);
		index++;
	}
}

void MultiplayerMenu::DeselectServer() {
	RefreshServers();
	this->selectedIp = "";
}

void MultiplayerMenu::RefreshServers() {
	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");

	// remove all textboxes and refresh them
	for (auto& child : hosts_list->GetChildren()) {
		hosts_list->RemoveChild(child, true);
	}

	int counter = 0;
	for (auto& ip : foundIps) {
		AddServer(ip, counter++);
	}

	// disable CONNECT button
	owner->GetScene()->FindNodeByTag("connect_but")->SetState(StringHash(STATES_DISABLED));
}

void MultiplayerMenu::SelectFaction(Faction fact) {
	auto node = owner->GetScene()->FindNodeByTag((fact == Faction::BLUE ? "faction_blue" : "faction_red"));
	node->SetState(StringHash(STATES_SELECTED));
}

void MultiplayerMenu::SelectMap(string map) {
	auto list = owner->GetScene()->FindNodeByTag("maps_list");
	auto children = list->GetChildren();

	for (auto& child : children) {
		if (child->HasAttr(ATTR_MAP) && child->GetAttr<string>(ATTR_MAP).compare(map) == 0) {
			// select map
			child->SetState(StringHash(STATES_SELECTED));
		}
	}

}