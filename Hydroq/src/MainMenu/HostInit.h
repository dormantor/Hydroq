#pragma once

#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"
#include "HydroqNetMsg.h"
#include "TransformMath.h"

/**
* Behavior that waits for clients to connect
*/
class HostInit : public Behavior {

private:
	// the time the client has connected
	uint64 clientConnectedTime = 0;
	TransformMath math;
	NetworkCommunicator* communicator;
	// indicator that ensures the communicator will be closed
	// when user closes the dialog
	bool keepConnected = false;
public:
	
	HostInit() {

	}

	void OnInit();

	void OnResume();

	void OnStop();

	void OnMessage(Msg& msg);

	Faction GetSelectedFaction();

	string GetSelectedMap();
public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};