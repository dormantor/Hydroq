#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "HydroqNetMsg.h"
#include "ofxCogMain.h"
#include "DeltaUpdate.h"
#include "HydroqDef.h"
#include "MsgEvents.h"
#include "GameModel.h"

class HydNetworkReceiver : public Behavior {
	
public:

	void OnInit();

	void OnMessage(Msg& msg);

	void ProcessDeltaUpdate(spt<NetInputMessage> netMsg);

	void ProcessMultiplayerInit(spt<NetInputMessage> netMsg);

	GameModel* model = nullptr;
	

	void ProcessCommandMessage(spt<NetInputMessage> netMsg);

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}

private:

};
