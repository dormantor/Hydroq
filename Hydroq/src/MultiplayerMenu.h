#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "HydroqNetMsg.h"
#include "HydroqGameModel.h"
#include "HydNetworkSender.h"

class MultiplayerMenu : public Behavior {
	OBJECT_PROTOTYPE(MultiplayerMenu)

private:
	NetworkCommunicator* communicator;
	vector<string> foundIps;
	map<string, spt<HydroqServerInitMsg>> serverMessages;
	string selectedIp = "";
	Settings mapConfig;
	bool keepConnected = false;
public:

	void OnInit() {
		RegisterListening(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED, ACT_SCENE_SWITCHED, ACT_SERVER_FOUND);
		communicator = GETCOMPONENT(NetworkCommunicator);

		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);


		LoadMaps();
	}

	void OnResume() {
		communicator->InitClient(HYDROQ_APPID, HYDROQ_CLIENTPORT, HYDROQ_SERVERPORT);
	}

	void OnStop() {
		if (!keepConnected) {
			communicator->CloseClient();
		}
		keepConnected = false;
	}

	bool messagingLocked = false;

	void OnMessage(Msg& msg) {
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
					communicator->CloseClient();
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
			else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_MAP"))) {
				// if user selects a map, enable host button
				EnableHostButton();
				DisableConnectButton();
				DeselectServer();
			}
			else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_SERVER"))) {
				// if user selects a server, enable connect button
				EnableConnectButton();
				DisableHostButton();
				selectedIp = msg.GetSourceObject()->GetAttr<string>(ATTR_SERVER_IP);
				auto netMsg = serverMessages[selectedIp];
				SelectFaction(netMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
				SelectMap(netMsg->GetMap());
			}
			else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("faction_select"))) {
				// user selected faction
				EnableHostButton();
				DisableConnectButton();
				DeselectServer();
			}
			else if (msg.HasAction(ACT_SERVER_FOUND)) {
				auto msgEvent = msg.GetDataS<CommonEvent<HydroqServerInitMsg>>();
				auto netMsg = msgEvent->value;
				string ipAddress = netMsg->GetIpAddress();
				// store message
				serverMessages[ipAddress] = netMsg;

				if (find(foundIps.begin(), foundIps.end(), ipAddress) == foundIps.end()) {
					// add new ip address
					foundIps.push_back(ipAddress);
					AddServer(ipAddress, foundIps.size()-1);
				}
			}

			messagingLocked = false;
		}
	}

	void AddServer(string ip, int index);
	void RefreshServers();
	void SelectFaction(Faction fact);
	void SelectMap(string map);
	void LoadMaps();
	void DeselectServer();

	void ConnectToServer(spt<HydroqServerInitMsg> serverMsg) {
		auto model = GETCOMPONENT(HydroqGameModel);
		// select the other faction than server did
		model->SetFaction(serverMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
		model->SetMapName(serverMsg->GetMap());
		model->SetIsMultiplayer(true);
		communicator->GetClient()->ConnectToServer();

		// set other properties and switch the scene
		auto sender = GETCOMPONENT(HydNetworkSender);
		sender->SetNetworkState(HydroqNetworkState::CLIENT);
		auto stage = GETCOMPONENT(Stage);
		auto scene = stage->FindSceneByName("game");

		keepConnected = true;
		stage->SwitchToScene(scene, TweenDirection::LEFT);
	}

	void EnableConnectButton() {
		owner->GetScene()->FindNodeByTag("connect_but")->ResetState(StringHash(STATES_DISABLED));
	}

	void DisableConnectButton() {
		owner->GetScene()->FindNodeByTag("connect_but")->SetState(StringHash(STATES_DISABLED));
	}

	void EnableHostButton() {
		owner->GetScene()->FindNodeByTag("host_but")->ResetState(StringHash(STATES_DISABLED));
	}

	void DisableHostButton() {
		owner->GetScene()->FindNodeByTag("host_but")->SetState(StringHash(STATES_DISABLED));
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (CogGetFrameCounter() % 100 == 0) {
			if (communicator->GetClient() != nullptr) {
				// check discovered servers
				auto discoveredServers = communicator->GetClient()->GetDiscoveredServers();

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
	}
};

