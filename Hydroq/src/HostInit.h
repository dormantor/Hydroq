#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"


class HostInit : public Behavior {
	OBJECT_PROTOTYPE(HostInit)

	
	void Init() {
		RegisterListening(owner->GetScene(), ACT_NET_MESSAGE_RECEIVED);

		auto communicator = GETCOMPONENT(NetworkCommunicator);
		communicator->Init(HYDROQ_APPID, HYDROQ_SERVERPORT, true);
		communicator->SetMode(NetworkComMode::CHECKING);
	}

	bool received = false;

	void OnMessage(Msg& msg) {
		if (!received && msg.GetAction() == ACT_NET_MESSAGE_RECEIVED) {
			received = true;
			auto msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			string ipAddress = netMsg->GetSourceIp();
			auto textNode = owner->GetScene()->FindNodeByTag("host_status");
			textNode->GetShape<spt<Text>>()->SetText(string_format("Connected client %s", ipAddress.c_str()));
		}
	}

	uint64 msgReceivedTime = 0;

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		if (received && msgReceivedTime == 0) {
			msgReceivedTime = absolute;
		}

		// wait 1500ms and finish
		if (received && (absolute - msgReceivedTime) > 1500) {
			Finish();
			auto stage = GETCOMPONENT(Stage);
			stage->SwitchBackToScene(TweenDirection::UP);
		}
	}
};