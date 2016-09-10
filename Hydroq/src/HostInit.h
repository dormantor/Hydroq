#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"


class HostInit : public Behavior {
	OBJECT_PROTOTYPE(HostInit)

	
	void Init() {
		RegisterListening(owner->GetScene(), ACT_NET_CLIENT_CONNECTED);
	}

	// todo: REFACTOR EVERYTHING

	bool firstRun = true;

	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_NET_CLIENT_CONNECTED) {
			auto msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			string ipAddress = netMsg->GetSourceIp();
			auto textNode = owner->GetScene()->FindNodeByTag("host_status");
			textNode->GetShape<spt<Text>>()->SetText(string_format("Connected client %s", ipAddress.c_str()));

			TransformEnt ent = TransformEnt();
			TransformMath math = TransformMath();
			ent.pos = ofVec2f(0.5f);
			ent.pType = CalcType::PER;
			ent.anchor = ofVec2f(0.5f);
			math.SetTransform(textNode, textNode->GetParent(), ent);

			msgReceivedTime = 0;
		}
	}

	uint64 msgReceivedTime = 1;

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

		auto communicator = GETCOMPONENT(NetworkCommunicator);

		if (firstRun || !communicator->IsServer()) {
			firstRun = false;

			auto textNode = owner->GetScene()->FindNodeByTag("host_status");
			textNode->GetShape<spt<Text>>()->SetText("Listening...");
			TransformEnt ent = TransformEnt();
			TransformMath math = TransformMath();
			ent.pos = ofVec2f(0.5f);
			ent.pType = CalcType::PER;
			ent.anchor = ofVec2f(0.5f);
			math.SetTransform(textNode, textNode->GetParent(), ent);

			communicator->Init(HYDROQ_APPID, HYDROQ_PORT, true);
			communicator->SetMode(NetworkComMode::CHECKING);
		}

		if (msgReceivedTime == 0) {
			msgReceivedTime = absolute;
		}

		// wait 1500ms and finish
		if (msgReceivedTime != 0 && msgReceivedTime != 1 && (absolute - msgReceivedTime) > 1500) {
			msgReceivedTime = 1;
			//auto stage = GETCOMPONENT(Stage);
			//auto scene = stage->FindSceneByName("game");
			//stage->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
};