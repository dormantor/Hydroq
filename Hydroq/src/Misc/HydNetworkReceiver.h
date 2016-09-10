#pragma once

#include "Component.h"
#include "UpdateInfo.h"
#include "HydroqNetMsg.h"
#include "Interpolator.h"
#include "HydroqDef.h"
#include "MsgEvents.h"
#include "GameModel.h"
#include "GameView.h"
/**
* Network receiver for Hydroq, processes received network messages
*/
class HydNetworkReceiver : public Behavior {
private:
	GameModel* model = nullptr;
	GameView* view = nullptr;
public:

	void OnInit();

	void OnMessage(Msg& msg);

	/**
	* Processes update message
	*/
	void ProcessUpdateMsg(spt<NetInputMessage> netMsg);

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
