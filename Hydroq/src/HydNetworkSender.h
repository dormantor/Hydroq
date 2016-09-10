#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "MsgEvents.h"
#include "DeltaMessage.h"
#include "HydroqNetMsg.h"
#include "HydroqDef.h"

class GameModel;


class HydNetworkSender : public Behavior {
private:
	HydroqNetworkState networkState = HydroqNetworkState::NONE;
public:

	GameModel* model = nullptr;

	void OnInit();

	void OnMessage(Msg& msg);

	HydroqNetworkState GetNetworkState() {
		return this->networkState;
	}

	void SetNetworkState(HydroqNetworkState networkState) {
		this->networkState = networkState;
	}

	virtual void Update(const uint64 delta, const uint64 absolute);

private:

};
