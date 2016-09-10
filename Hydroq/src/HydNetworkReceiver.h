#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "HydroqNetMsg.h"
#include "DeltaUpdate.h"
#include "HydroqDef.h"
#include "MsgEvents.h"
#include "GameModel.h"

/**
* Network receiver for Hydroq, processes received network messages
*/
class HydNetworkReceiver : public Behavior {
private:
	GameModel* model = nullptr;

public:

	void OnInit();

	void OnMessage(Msg& msg);

	/**
	* Processes delta update message
	*/
	void ProcessDeltaUpdate(spt<NetInputMessage> netMsg);

	/**
	* Processes multiplayer initialization message
	*/
	void ProcessMultiplayerInit(spt<NetInputMessage> netMsg);
	
	/**
	* Processes command message
	*/
	void ProcessCommandMessage(spt<NetInputMessage> netMsg);

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};
