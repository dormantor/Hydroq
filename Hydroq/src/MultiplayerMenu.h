#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "HydroqNetMsg.h"
#include "HydroqGameModel.h"

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
		SubscribeForMessages(ACT_BUTTON_CLICKED, ACT_OBJECT_SELECTED, ACT_SCENE_SWITCHED, ACT_NET_MESSAGE_RECEIVED);
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

	void OnMessage(Msg& msg);
	void AddServer(string ip, int index);
	void RefreshServers();
	void SelectFaction(Faction fact);
	void SelectMap(string map);
	void LoadMaps();
	void DeselectServer();

	void ConnectToServer(spt<HydroqServerInitMsg> serverMsg);

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
	virtual void Update(const uint64 delta, const uint64 absolute);
};

