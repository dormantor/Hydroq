#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"
#include "HydroqNetMsg.h"

class HostInit : public Behavior {

private:
	uint64 msgReceivedTime = 0;
	TransformMath math;
	NetworkCommunicator* communicator;
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