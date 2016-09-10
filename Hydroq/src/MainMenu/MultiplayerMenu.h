#pragma once

#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "HydroqNetMsg.h"
#include "GameModel.h"

/**
* Behavior for multiplayer menu
*/
class MultiplayerMenu : public Behavior {
private:
	NetworkCommunicator* communicator;
	// found ip addresses of hosts
	vector<string> foundIps;
	// server messages, mapped by ip addresses of hosts
	map<string, spt<HydroqServerInitMsg>> hostMessages;
	// ip address of selected host
	string selectedIp = "";
	Settings mapConfig;
	// indicator that ensures the communicator is connected when
	// user closes the scene
	bool keepConnected = false;
	
	// lock for messaging (when a message is received, another message can
	// be sent with the same type and this indicator prevents from stack overflow behavior)
	bool messagingLocked = false;
public:

	MultiplayerMenu() {

	}

	void OnInit();

	void OnResume();

	void OnStop();

	
	void OnMessage(Msg& msg);

	virtual void Update(const uint64 delta, const uint64 absolute);

protected:
	/**
	* Adds a new host
	*/
	void AddHost(string ip, int index);
	
	/**
	* Refreshes view of all hosts
	*/
	void RefreshHosts();

	/**
	* Sets selected faction based on selected node
	*/
	void SelectFaction(Faction fact);

	/**
	* Sets selected map based on selected node
	*/
	void SelectMap(string map);

	/**
	* Loads list of all maps into view
	*/
	void LoadMaps();

	/**
	* Deselects selected host
	*/
	void DeselectHost();

	/**
	* Connects to host
	*/
	void ConnectToHost(spt<HydroqServerInitMsg> hostMsg);

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
};

