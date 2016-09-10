#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "NetMessage.h"
#include "HydroqDef.h"

class MultiplayerMenu : public Behavior {
	OBJECT_PROTOTYPE(MultiplayerMenu)

private:
	NetworkCommunicator* communicator;
	vector<string> foundIps;
	string selectedIp = "";
public:

	void OnInit() {
		RegisterListening(ACT_OBJECT_HIT_ENDED, ACT_OBJECT_SELECTED, ACT_NET_SERVER_FOUND, ACT_SCENE_SWITCHED);
		communicator = GETCOMPONENT(NetworkCommunicator);
	}

	void OnResume() {
		communicator->InitClient(HYDROQ_APPID, HYDROQ_CLIENTPORT, HYDROQ_SERVERPORT);
	}

	void OnStop() {
		communicator->CloseClient();
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_SCENE_SWITCHED)) {
			if (msg.GetSourceObject()->GetScene() == owner->GetScene()) {
				OnResume();
			}
			else {
				OnStop();
			}
		}
		else if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {
			if (msg.GetSourceObject()->GetTag().compare("host_but") == 0) {
				// click on HOST button
				auto sceneContext = GETCOMPONENT(Stage);
				auto scene = sceneContext->FindSceneByName("host_init");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
			else if (msg.GetSourceObject()->GetTag().compare("connect_but") == 0) {
				if (!selectedIp.empty()) {
					communicator->GetClient()->ConnectToServer();
				}
			}
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_MAP"))) {
			// if user selects a map, enable host button
			owner->GetScene()->FindNodeByTag("host_but")->ResetState(StringHash(STATES_DISABLED));
		}
		else if (msg.HasAction(ACT_OBJECT_SELECTED) && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_SERVER"))) {
			// if user selects a map, enable host button
			owner->GetScene()->FindNodeByTag("connect_but")->ResetState(StringHash(STATES_DISABLED));
			selectedIp = msg.GetSourceObject()->GetAttr<string>(ATTR_SERVER_IP);
		}
		else if (msg.HasAction(ACT_NET_SERVER_FOUND)) {
			NetworkMsgEvent* msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			string ipAddress = netMsg->GetSourceIp();

			if (find(foundIps.begin(), foundIps.end(), ipAddress) == foundIps.end()) {
				// add new ip address
				foundIps.push_back(ipAddress);
				AddServer(ipAddress);
			}
		}
	}

	void AddServer(string ip);
	void RefreshServers();

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (CogGetFrameCounter() % 100 == 0) {
			if (communicator->GetClient() != nullptr) {
				// check discovered servers
				auto discoveredServers = communicator->GetClient()->GetDiscoveredServers();

				for (auto& key : discoveredServers) {
					auto found = find(foundIps.begin(), foundIps.end(), key.first);
					if (found == foundIps.end()) {
						// add new ip address
						foundIps.push_back(key.first);
						AddServer(key.first);
					}
					else {
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

