#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"


class HostInit : public Behavior {
	OBJECT_PROTOTYPE(HostInit)

private:
	uint64 msgReceivedTime = 0;
	TransformMath math;
	NetworkCommunicator* communicator;
public:
	
	void OnInit() {
		RegisterListening(ACT_NET_CLIENT_CONNECTED, ACT_SCENE_SWITCHED);
		communicator = GETCOMPONENT(NetworkCommunicator);
	}

	void OnResume() {
		msgReceivedTime = 0;

		auto textNode = owner->GetScene()->FindNodeByTag("host_status");
		textNode->GetShape<spt<Text>>()->SetText("Listening...");
		TransformEnt ent = TransformEnt();
		ent.pos = ofVec2f(0.5f);
		ent.pType = CalcType::PER;
		ent.anchor = ofVec2f(0.5f);
		math.SetTransform(textNode, textNode->GetParent(), ent);

		communicator->InitServer(HYDROQ_APPID, HYDROQ_SERVERPORT);
	}

	void OnStop() {
		communicator->CloseServer();
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_SCENE_SWITCHED)) {
			if (msg.GetSourceObject()->GetScene() == owner->GetScene()) {
				OnResume();
			}
			else {
				OnStop();
			}
		}
		if (msg.HasAction(ACT_NET_CLIENT_CONNECTED)) {
			auto msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			string ipAddress = netMsg->GetSourceIp();
			auto textNode = owner->GetScene()->FindNodeByTag("host_status");
			textNode->GetShape<spt<Text>>()->SetText(string_format("Connected client %s", ipAddress.c_str()));

			TransformEnt ent = TransformEnt();
			ent.pos = ofVec2f(0.5f);
			ent.pType = CalcType::PER;
			ent.anchor = ofVec2f(0.5f);
			math.SetTransform(textNode, textNode->GetParent(), ent);

			msgReceivedTime = CogGetAbsoluteTime();
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

		// wait 1500ms and finish
		if (msgReceivedTime != 0 && msgReceivedTime != 1 && (absolute - msgReceivedTime) > 1500) {
			auto stage = GETCOMPONENT(Stage);
			auto scene = stage->FindSceneByName("game");
			stage->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
};