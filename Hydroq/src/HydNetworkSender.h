#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "MsgEvents.h"
#include "DeltaMessage.h"
#include "HydroqNetMsg.h"
#include "HydroqDef.h"
#include "Behavior.h"

using namespace Cog;

class GameModel;

/**
* Behavior that sends messages to the other peer
*/
class HydNetworkSender : public Behavior {
private:
	HydroqNetworkState networkState = HydroqNetworkState::NONE;
	GameModel* model = nullptr;
	uint64 lastUpdateMsgTime = 0;
	float updateFrequency = 5;

public:
	void OnInit();

	void OnMessage(Msg& msg);

	HydroqNetworkState GetNetworkState() {
		return this->networkState;
	}

	void SetNetworkState(HydroqNetworkState networkState) {
		this->networkState = networkState;
	}

	/**
	* Gets frequency how many times the update message should be sent per second
	*/
	float GetUpdateFrequency() {
		return updateFrequency;
	}

	/**
	* Sets frequency how many times the update message should be sent per second
	*/
	void SetUpdateFrequency(float freq) {
		this->updateFrequency = freq;
	}

	virtual void Update(const uint64 delta, const uint64 absolute);

private:

};
