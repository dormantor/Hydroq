#include "MultiplayerMenu.h"
#include "Node.h"
#include "MultiSelection.h"
#include "TransformMath.h"
#include "Scene.h"
#include "ComponentStorage.h"
#include "Stage.h"
#include "Tween.h"

void MultiplayerMenu::OnInit() {
	SubscribeForMessages(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED, ACT_SCENE_SWITCHED, ACT_NET_MESSAGE_RECEIVED);
	communicator = GETCOMPONENT(NetworkCommunicator);

	// load map config
	auto xml = CogLoadXMLFile("mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);

	// load all maps into view
	LoadMaps();
}

void MultiplayerMenu::OnResume() {
	if (!keepConnected) {
		communicator->InitBroadcast(HYDROQ_APPID, HYDROQ_CLIENTPORT, HYDROQ_SERVERPORT);
	}
}

void MultiplayerMenu::OnStop() {
	if (!keepConnected) {
		communicator->Close();
	}
	keepConnected = false;
}


void MultiplayerMenu::OnMessage(Msg& msg) {
	if (!messagingLocked) {
		messagingLocked = true;

		if (msg.HasAction(ACT_SCENE_SWITCHED)) {
			if (msg.GetContextNode()->GetScene() == owner->GetScene()) {
				OnResume();
			}
			else {
				OnStop();
			}
		}
		else if (msg.HasAction(ACT_BUTTON_CLICKED)) {
			if (msg.GetContextNode()->GetTag().compare("host_but") == 0) {
				// click on HOST button
				auto sceneContext = GETCOMPONENT(Stage);
				keepConnected = true;
				auto scene = sceneContext->FindSceneByName("host_init");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
			else if (msg.GetContextNode()->GetTag().compare("connect_but") == 0) {
				// click on CONNECT button
				if (!selectedIp.empty()) {
					auto selectedHost = hostMessages[selectedIp];
					if (selectedHost) {
						// connect to host and start the game
						ConnectToHost(selectedHost);
					}
				}
			}
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetContextNode()->IsInGroup(StrId("SELECTION_MAP"))) {
			// if user has selected a map, enable host button
			EnableHostButton();
			DisableConnectButton();
			DeselectHost();
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetContextNode()->IsInGroup(StrId("SELECTION_SERVER"))) {
			// if user has selected a host, enable connect button
			EnableConnectButton();
			DisableHostButton();
			selectedIp = msg.GetContextNode()->GetAttr<string>(ATTR_SERVER_IP);
			auto netMsg = hostMessages[selectedIp];
			SelectFaction(netMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
			SelectMap(netMsg->GetMap());
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetContextNode()->IsInGroup(StrId("faction_select"))) {
			// user has selected faction
			EnableHostButton();
			DisableConnectButton();
			DeselectHost();
		}
		else if (msg.HasAction(ACT_NET_MESSAGE_RECEIVED)) {

			// received message from host -> update list of all hosts
			auto msgEvent = msg.GetData<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			StrId action = netMsg->GetAction();
			auto type = netMsg->GetMsgType();

			string ipAddress = netMsg->GetSourceIp();
			auto mpInit = netMsg->GetData<HydroqServerInitMsg>();
			mpInit->SetIpAddress(netMsg->GetSourceIp());

			// store message
			hostMessages[ipAddress] = mpInit;

			if (find(foundIps.begin(), foundIps.end(), ipAddress) == foundIps.end()) {
				// add new ip address
				foundIps.push_back(ipAddress);
				AddHost(ipAddress, foundIps.size() - 1);
			}
		}

		messagingLocked = false;
	}
}


void MultiplayerMenu::Update(const uint64 delta, const uint64 absolute) {

	// each 100th frame, check discovered hosts (some hosts could disconnect)
	if (CogGetFrameCounter() % 100 == 0) {
		auto discoveredHosts = communicator->GetDiscoveredPeers();

		for (auto& key : discoveredHosts) {
			auto found = find(foundIps.begin(), foundIps.end(), key.first);
			if (found != foundIps.end()) {
				// if no response from host for 4 seconds, remove its ip address from the view
				if ((absolute - key.second) > 4000) {
					foundIps.erase(found);
					RefreshHosts();
				}
			}
		}
	}
}


void MultiplayerMenu::AddHost(string ip, int index) {
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

void MultiplayerMenu::DeselectHost() {
	RefreshHosts();
	this->selectedIp = "";
}

void MultiplayerMenu::ConnectToHost(spt<HydroqServerInitMsg> serverMsg) {
	auto model = GETCOMPONENT(PlayerModel);
	// select the other faction than host
	Faction selectedFaction = (serverMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
	model->StartGame(selectedFaction, serverMsg->GetMap(), HydroqNetworkState::CLIENT);
	communicator->ConnectToPeer(serverMsg->GetIpAddress());
	auto stage = GETCOMPONENT(Stage);
	auto scene = stage->FindSceneByName("game");

	keepConnected = true;
	stage->SwitchToScene(scene, TweenDirection::LEFT);
}

void MultiplayerMenu::RefreshHosts() {
	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");

	// remove all textboxes and create them again
	for (auto& child : hosts_list->GetChildren()) {
		hosts_list->RemoveChild(child, true);
	}

	int counter = 0;
	for (auto& ip : foundIps) {
		AddHost(ip, counter++);
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
