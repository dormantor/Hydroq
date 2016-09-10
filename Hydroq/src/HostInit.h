#pragma once

#include "ofxCogMain.h"
#include "NetworkCommunicator.h"
#include "HydroqDef.h"
#include "Node.h"
#include "NetMessage.h"
#include "HydroqNetMsg.h"

class HostInit : public Behavior {
	OBJECT_PROTOTYPE(HostInit)

private:
	uint64 msgReceivedTime = 0;
	TransformMath math;
	NetworkCommunicator* communicator;
	bool keepConnected = false;
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
		// set message to be sent
		auto msg = new HydroqServerInitMsg();
		msg->SetFaction(GetSelectedFaction());
		msg->SetMap(GetSelectedMap());
		communicator->GetServer()->PushMessageForSending(msg->CreateMessage());
	}

	void OnStop() {
		if (!keepConnected) {
			communicator->CloseServer();
		}
		keepConnected = false;
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

	Faction GetSelectedFaction() {
		auto parentScene = owner->GetScene()->GetParentScene();
		auto factRed = parentScene->FindNodeByTag("faction_red");
		auto factBlue = parentScene->FindNodeByTag("faction_blue");

		if (factRed->HasState(StringHash(STATES_SELECTED))) {
			return Faction::RED;
		}
		else {
			return Faction::BLUE;
		}
	}

	string GetSelectedMap() {
		auto parentScene = owner->GetScene()->GetParentScene();

		auto list = parentScene->FindNodeByTag("maps_list");
		auto children = list->GetChildren();

		for (auto& child : children) {
			if (child->HasAttr(ATTR_MAP) && child->HasState(StringHash(STATES_SELECTED))) {
				return child->GetAttr<string>(ATTR_MAP);
			}
		}

		return "";
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

		// wait 1500ms and finish
		if (msgReceivedTime != 0 && msgReceivedTime != 1 && (absolute - msgReceivedTime) > 1500) {
			auto model = GETCOMPONENT(HydroqGameModel);
			// select the other faction than server did
			model->SetFaction(GetSelectedFaction());
			model->SetMapName(GetSelectedMap());
			model->SetIsMultiplayer(true);
			// set other properties and switch the scene
			auto sender = GETCOMPONENT(HydNetworkSender);
			sender->SetNetworkState(HydroqNetworkState::SERVER);
			auto stage = GETCOMPONENT(Stage);
			auto scene = stage->FindSceneByName("game");

			keepConnected = true;
			stage->SwitchToScene(scene, TweenDirection::LEFT);
		}
	}
};