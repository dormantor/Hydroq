#include "MultiplayerMenu.h"
#include "Node.h"
#include "MultiSelection.h"
#include "TransformMath.h"
#include "Scene.h"

void MultiplayerMenu::OnMessage(Msg& msg) {
	if (!messagingLocked) {
		messagingLocked = true;

		if (msg.HasAction(ACT_SCENE_SWITCHED)) {
			if (msg.GetSourceObject()->GetScene() == owner->GetScene()) {
				OnResume();
			}
			else {
				OnStop();
			}
		}
		else if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			if (msg.GetSourceObject()->GetTag().compare("host_but") == 0) {
				// click on HOST button
				auto sceneContext = GETCOMPONENT(Stage);
				keepConnected = true;
				auto scene = sceneContext->FindSceneByName("host_init");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
			else if (msg.GetSourceObject()->GetTag().compare("connect_but") == 0) {
				// connect to server
				if (!selectedIp.empty()) {
					auto selectedServer = serverMessages[selectedIp];
					if (selectedServer) {
						ConnectToServer(selectedServer);
					}
				}
			}
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StrId("SELECTION_MAP"))) {
			// if user selects a map, enable host button
			EnableHostButton();
			DisableConnectButton();
			DeselectServer();
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StrId("SELECTION_SERVER"))) {
			// if user selects a server, enable connect button
			EnableConnectButton();
			DisableHostButton();
			selectedIp = msg.GetSourceObject()->GetAttr<string>(ATTR_SERVER_IP);
			auto netMsg = serverMessages[selectedIp];
			SelectFaction(netMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
			SelectMap(netMsg->GetMap());
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StrId("faction_select"))) {
			// user selected faction
			EnableHostButton();
			DisableConnectButton();
			DeselectServer();
		}
		else if (msg.HasAction(ACT_NET_MESSAGE_RECEIVED)) {

			NetworkMsgEvent* msgEvent = msg.GetData<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			StrId action = netMsg->GetAction();
			auto type = netMsg->GetMsgType();

			string ipAddress = netMsg->GetSourceIp();
			auto mpInit = netMsg->GetData<HydroqServerInitMsg>();
			mpInit->SetIpAddress(netMsg->GetSourceIp());

			// store message
			serverMessages[ipAddress] = mpInit;

			if (find(foundIps.begin(), foundIps.end(), ipAddress) == foundIps.end()) {
				// add new ip address
				foundIps.push_back(ipAddress);
				AddServer(ipAddress, foundIps.size() - 1);
			}
		}

		messagingLocked = false;
	}
}


void MultiplayerMenu::AddServer(string ip, int index) {

	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");
	Helper::SetPanelItem(owner, hosts_list, index, StrId("SELECTION_SERVER"), ATTR_SERVER_IP, ip);
}

void MultiplayerMenu::LoadMaps() {
	auto set = this->mapConfig.GetSetting("maps_icons");
	auto items = set.items;

	Node* maps_list = owner->GetScene()->FindNodeByTag("maps_list");
	int index = 0;

	for (auto& key : items) {
		auto val = key.second;
		string mapName = val.key;
		Helper::SetPanelItem(owner, maps_list, index, StrId("SELECTION_MAP"), ATTR_MAP, mapName);
		index++;
	}
}

void MultiplayerMenu::DeselectServer() {
	RefreshServers();
	this->selectedIp = "";
}

void MultiplayerMenu::ConnectToServer(spt<HydroqServerInitMsg> serverMsg) {
	auto model = GETCOMPONENT(HydroqPlayerModel);
	// select the other faction than server did
	Faction selectedFaction = (serverMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
	model->StartGame(selectedFaction, serverMsg->GetMap(), HydroqNetworkState::CLIENT);
	communicator->ConnectToPeer(serverMsg->GetIpAddress());
	auto stage = GETCOMPONENT(Stage);
	auto scene = stage->FindSceneByName("game");

	keepConnected = true;
	stage->SwitchToScene(scene, TweenDirection::LEFT);
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
	owner->GetScene()->FindNodeByTag("connect_but")->SetState(StrId(STATES_DISABLED));
}

void MultiplayerMenu::SelectFaction(Faction fact) {
	auto node = owner->GetScene()->FindNodeByTag((fact == Faction::BLUE ? "faction_blue" : "faction_red"));
	node->SetState(StrId(STATES_SELECTED));
}

void MultiplayerMenu::SelectMap(string map) {
	auto list = owner->GetScene()->FindNodeByTag("maps_list");
	auto children = list->GetChildren();

	for (auto& child : children) {
		if (child->HasAttr(ATTR_MAP) && child->GetAttr<string>(ATTR_MAP).compare(map) == 0) {
			// select map
			child->SetState(StrId(STATES_SELECTED));
		}
	}

}

void MultiplayerMenu::Update(const uint64 delta, const uint64 absolute) {
	if (CogGetFrameCounter() % 100 == 0) {
		// check discovered servers
		auto discoveredServers = communicator->GetDiscoveredPeers();

		for (auto& key : discoveredServers) {
			auto found = find(foundIps.begin(), foundIps.end(), key.first);
			if (found != foundIps.end()) {
				if ((absolute - key.second) > 4000) {
					// server lost
					foundIps.erase(found);
					RefreshServers();
				}
			}
		}
	}
}