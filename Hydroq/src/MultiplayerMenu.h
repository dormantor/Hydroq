#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "HydroqNetMsg.h"
#include "HydroqGameModel.h"
#include "HydNetworkSender.h"

class MultiplayerMenu : public Behavior {
private:
	NetworkCommunicator* communicator;
	vector<string> foundIps;
	map<string, spt<HydroqServerInitMsg>> serverMessages;
	string selectedIp = "";
	Settings mapConfig;
	bool keepConnected = false;
public:

	MultiplayerMenu() {

	}

	void OnInit() {
		SubscribeForMessages(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED, ACT_SCENE_SWITCHED, ACT_SERVER_FOUND);
		communicator = GETCOMPONENT(NetworkCommunicator);

		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);


		LoadMaps();
	}

	void OnResume() {
		if (!keepConnected) {
			communicator->InitBroadcast(HYDROQ_APPID, HYDROQ_CLIENTPORT, HYDROQ_SERVERPORT);
		}
	}

	void OnStop() {
		if (!keepConnected) {
			communicator->Close();
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
			else if (msg.HasAction(ACT_SERVER_FOUND)) {
				auto msgEvent = msg.GetData<CommonEvent<HydroqServerInitMsg>>();
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
		auto model = GETCOMPONENT(HydroqPlayerModel);
		// select the other faction than server did
		Faction selectedFaction = (serverMsg->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE);
		model->StartGame(selectedFaction, serverMsg->GetMap(), true);
		communicator->ConnectToPeer(serverMsg->GetIpAddress());

		// set other properties and switch the scene
		auto sender = GETCOMPONENT(HydNetworkSender);
		sender->SetNetworkState(HydroqNetworkState::CLIENT);
		auto stage = GETCOMPONENT(Stage);
		auto scene = stage->FindSceneByName("game");

		keepConnected = true;
		stage->SwitchToScene(scene, TweenDirection::LEFT);
	}

	void EnableConnectButton() {
		owner->GetScene()->FindNodeByTag("connect_but")->ResetState(StrId(STATES_DISABLED));
	}

	void DisableConnectButton() {
		owner->GetScene()->FindNodeByTag("connect_but")->SetState(StrId(STATES_DISABLED));
	}

	void EnableHostButton() {
		owner->GetScene()->FindNodeByTag("host_but")->ResetState(StrId(STATES_DISABLED));
	}

	void DisableHostButton() {
		owner->GetScene()->FindNodeByTag("host_but")->SetState(StrId(STATES_DISABLED));
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
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
};

