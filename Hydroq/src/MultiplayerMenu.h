#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "NetMessage.h"

class MultiplayerMenu : public Behavior {
	OBJECT_PROTOTYPE(MultiplayerMenu)



		void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED, ACT_OBJECT_SELECTED, ACT_NET_MESSAGE_RECEIVED);

		auto communicator = GETCOMPONENT(NetworkCommunicator);
		communicator->Init(HYDROQ_APPID, HYDROQ_CLIENTPORT, false);
		communicator->SetMode(NetworkComMode::CHECKING);
	}

	vector<string> foundIps;

	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			if (msg.GetSourceObject()->GetTag().compare("host_but") == 0) {
				// click on HOST button
				auto sceneContext = GETCOMPONENT(Stage);
				auto scene = sceneContext->FindSceneByName("host_init");
				sceneContext->SwitchToScene(scene, TweenDirection::NONE);
			}
		}
		else if (msg.GetAction() == ACT_OBJECT_SELECTED && msg.GetSourceObject()->IsInGroup(StringHash("SELECTION_MAP"))) {
			// if user selects a map, enable host button
			owner->GetScene()->FindNodeByTag("host_but")->ResetState(StringHash(STATES_DISABLED));
		}
		else if (msg.GetAction() == ACT_NET_MESSAGE_RECEIVED) {
			NetworkMsgEvent* msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			string ipAddress = netMsg->GetSourceIp();

			if (find(foundIps.begin(), foundIps.end(), ipAddress) == foundIps.end()) {
				// add new ip address

			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

