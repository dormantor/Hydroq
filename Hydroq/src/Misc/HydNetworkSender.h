#pragma once

#include "Component.h"
#include "UpdateInfo.h"
#include "MsgEvents.h"
#include "UpdateMessage.h"
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
	HydroqConnectionType connectionType = HydroqConnectionType::NONE;
	GameModel* model = nullptr;
	uint64 lastUpdateMsgTime = 0;
	float updateFrequency = 5;

public:
	void OnInit();

	void OnMessage(Msg& msg);

	HydroqConnectionType GetConnectionType() const {
		return this->connectionType;
	}

	void SetConnectionType(HydroqConnectionType connectionType) {
		this->connectionType = connectionType;
	}

	/**
	* Gets frequency how many times the update message should be sent per second
	*/
	float GetUpdateFrequency() const {
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
