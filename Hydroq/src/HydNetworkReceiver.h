#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "HydroqNetMsg.h"
#include "ofxCogMain.h"
#include "DeltaUpdate.h"
#include "HydroqDef.h"
#include "MsgEvents.h"
#include "HydroqGameModel.h"

class HydNetworkReceiver : public Component {
	OBJECT(HydNetworkReceiver)

public:


	void Init() {
		RegisterGlobalListening(ACT_NET_MESSAGE_RECEIVED);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_NET_MESSAGE_RECEIVED)) {
			NetworkMsgEvent* msgEvent = msg.GetDataS<NetworkMsgEvent>();
			auto netMsg = msgEvent->msg;
			StringHash action = netMsg->GetAction();
			auto type = netMsg->GetMsgType();

			if (type == NetMsgType::DISCOVER_RESPONSE) {
				if (action == NET_MULTIPLAYER_INIT) {
					ProcessMultiplayerInit(netMsg);
				}
			}
			else if (type == NetMsgType::UPDATE) {
				if (action == NET_MSG_DELTA_UPDATE) {
					ProcessDeltaUpdate(netMsg);
				}
				else if (action == NET_MULTIPLAYER_COMMAND) {
					ProcessCommandMessage(netMsg);
				}
			}
		}
	}

	void ProcessDeltaUpdate(spt<NetInputMessage> netMsg) {
		COGLOGDEBUG("DeltaMessage", "Accepting delta message");
		spt<DeltaMessage> deltaMsg = netMsg->GetData<DeltaMessage>();
		spt<DeltaInfo> deltaInfo = spt<DeltaInfo>(new DeltaInfo());
		deltaInfo->deltas = deltaMsg->deltas;
		deltaInfo->teleports = deltaMsg->teleports;
		deltaInfo->time = netMsg->GetMsgTime();

		auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
		deltaUpdate->AcceptDeltaUpdate(deltaInfo);
	}

	void ProcessMultiplayerInit(spt<NetInputMessage> netMsg) {
		auto mpInit = netMsg->GetData<HydroqServerInitMsg>();
		mpInit->SetIpAddress(netMsg->GetSourceIp());
		SendMessageToListeners(StringHash(ACT_SERVER_FOUND), 0, new CommonEvent<HydroqServerInitMsg>(mpInit), nullptr);
	}

	void ProcessCommandMessage(spt<NetInputMessage> netMsg) {
		auto cmdMsg = netMsg->GetData<HydroqCommandMsg>();
		auto model = GETCOMPONENT(HydroqGameModel);

		switch (cmdMsg->GetEventType()) {
		case SyncEventType::OBJECT_CREATED:
			switch (cmdMsg->GetEntityType()) {
			case EntityType::SEEDBED:
				model->CreateSeedBed(cmdMsg->GetPosition(), cmdMsg->GetFaction(), cmdMsg->GetIdentifier());
				break;

			case EntityType::WORKER:
				model->SpawnWorker(cmdMsg->GetPosition(), cmdMsg->GetFaction(), cmdMsg->GetIdentifier());
				break;
			}
			break;
		case SyncEventType::OBJECT_REMOVED:
			break;
		}
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}

private:

};
