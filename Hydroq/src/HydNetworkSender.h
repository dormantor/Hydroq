#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "MsgEvents.h"
#include "DeltaMessage.h"
#include "HydroqNetMsg.h"


class HydNetworkSender : public Behavior {
private:
	HydroqNetworkState networkState = HydroqNetworkState::NONE;
public:

	HydroqGameModel* model = nullptr;

	void OnInit() {
		auto playerModel = GETCOMPONENT(HydroqPlayerModel);
		this->networkState = playerModel->GetNetworkState();
		
		// remove if this is not a multiplayer
		if (networkState == HydroqNetworkState::NONE) {
			owner->RemoveBehavior(this, true);
		}

		SubscribeForMessages(ACT_SYNC_OBJECT_CHANGED);
		model = owner->GetBehavior<HydroqGameModel>();
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_SYNC_OBJECT_CHANGED)) {
			auto syncEvent = msg.GetData<SyncEvent>();

			// send sync message
			auto communicator = GETCOMPONENT(NetworkCommunicator);
			auto msg = new HydroqCommandMsg();
			msg->SetEntityType(syncEvent->entityType);
			msg->SetEventType(syncEvent->eventType);
			msg->SetFaction(syncEvent->faction);
			msg->SetOwnerPosition(syncEvent->ownerPosition);
			msg->SetIdentifier(syncEvent->internalId);
			msg->SetPosition(syncEvent->positionf);
			auto netMsg = msg->CreateMessage();

			if (syncEvent->eventType == SyncEventType::MAP_CHANGED) {
				cout << "sending info that bridge has changed " << endl;
			}

			communicator->PushMessageForSending(netMsg);
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
			if (CogGetFrameCounter() % 10 == 0) {
				auto communicator = GETCOMPONENT(NetworkCommunicator);

				// send delta message regularly
				spt<DeltaInfo> deltaInf = spt<DeltaInfo>(new DeltaInfo());

				auto& dynObjects = model->GetMovingObjects();

				// update transformation of all objects
				for (auto& dynObj : dynObjects) {
					if (dynObj->GetSecondaryId() == 0) {

						int id = dynObj->GetId();
						auto transform = dynObj->GetTransform();

						auto& deltas = deltaInf->GetDeltas();

						deltas[id*3+0] = transform.rotation;
						deltas[id*3+1] = transform.localPos.x;
						deltas[id*3+2] = transform.localPos.y;
					}
				}

				auto msg = new DeltaMessage(deltaInf);
				spt<NetOutputMessage> netMsg = spt<NetOutputMessage>(new NetOutputMessage(1));
				netMsg->SetData(msg);
				netMsg->SetAction(StrId(NET_MSG_DELTA_UPDATE));

				if (communicator->GetNetworkState() == NetworkComState::COMMUNICATING) {
					communicator->PushMessageForSending(netMsg);
				}
			}
		}
	}

private:

};
