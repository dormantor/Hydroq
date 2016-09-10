#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "MsgEvents.h"
#include "DeltaMessage.h"
#include "HydroqNetMsg.h"

enum class HydroqNetworkState {
	NONE, SERVER, CLIENT
};

class HydNetworkSender : public Component {
	OBJECT(HydNetworkSender)

private:
	HydroqNetworkState networkState = HydroqNetworkState::NONE;
public:


	void Init() {
		RegisterGlobalListening(ACT_SYNC_OBJECT_CHANGED);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_SYNC_OBJECT_CHANGED)) {
			auto syncEvent = msg.GetDataS<SyncEvent>();

			// send sync message
			auto communicator = GETCOMPONENT(NetworkCommunicator);
			auto msg = new HydroqCommandMsg();
			msg->SetEntityType(syncEvent->entityType);
			msg->SetEventType(syncEvent->eventType);
			msg->SetFaction(syncEvent->faction);
			msg->SetIdentifier(syncEvent->internalId);
			msg->SetPosition(syncEvent->positionf);
			auto netMsg = msg->CreateMessage();

			if (networkState == HydroqNetworkState::CLIENT) {
				communicator->GetClient()->PushMessageForSending(netMsg);
			}
			else {
				communicator->GetServer()->PushMessageForSending(netMsg);
			}
		}
	}

	HydroqNetworkState GetNetworkState() {
		return this->networkState;
	}

	void SetNetworkState(HydroqNetworkState networkState) {
		this->networkState = networkState;
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {

		if (networkState == HydroqNetworkState::CLIENT || networkState == HydroqNetworkState::SERVER) {
			/*auto communicator = GETCOMPONENT(NetworkCommunicator);

			// send delta message regularly
			spt<DeltaInfo> deltaInf = spt<DeltaInfo>(new DeltaInfo());

			auto model = GETCOMPONENT(HydroqGameModel);
			auto& dynObjects = model->GetMovingObjects();

			// update transformation of all objects
			for (auto& dynObj : dynObjects) {
				if (dynObj->GetSubType() == 0) {

					int id = dynObj->GetId();
					auto transform = dynObj->GetTransform();

					deltaInf->deltas[StringHash(id * 3 + 0)] = transform.rotation;
					deltaInf->deltas[StringHash(id * 3 + 1)] = transform.localPos.x;
					deltaInf->deltas[StringHash(id * 3 + 2)] = transform.localPos.y;
				}
			}

			auto msg = spt<DeltaMessage>(new DeltaMessage(deltaInf));
			spt<NetOutputMessage> netMsg = spt<NetOutputMessage>(new NetOutputMessage(1));
			netMsg->SetData(msg);
			netMsg->SetAction(StringHash(NET_MSG_DELTA_UPDATE));

			if (networkState == HydroqNetworkState::CLIENT) {
				communicator->GetClient()->SetMessageForSending(netMsg);
			}
			else {
				communicator->GetServer()->SetMessageForSending(netMsg);
			}*/
		}
	}

private:

};
