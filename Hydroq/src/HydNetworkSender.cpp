#include "HydNetworkSender.h"
#include "PlayerModel.h"
#include "GameModel.h"
#include "NetworkCommunicator.h"
#include "ComponentStorage.h"
#include "Node.h"
#include "Behavior.h"
#include "Utils.h"

void HydNetworkSender::OnInit() {
	auto playerModel = GETCOMPONENT(PlayerModel);
	this->connectionType = playerModel->GetConnectionType();

	// remove sender from the scene node if the game is not a multiplayer game
	if (connectionType == HydroqConnectionType::NONE) {
		owner->RemoveBehavior(this, true);
	}

	SubscribeForMessages(ACT_SYNC_OBJECT_CHANGED);
	model = owner->GetBehavior<GameModel>();
}

void HydNetworkSender::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_SYNC_OBJECT_CHANGED)) {
		auto syncEvent = msg.GetData<SyncEvent>();

		// send synchronization message
		auto communicator = GETCOMPONENT(NetworkCommunicator);
		auto msg = new HydroqCommandMsg();
		msg->SetEntityType(syncEvent->entityType);
		msg->SetEventType(syncEvent->eventType);
		msg->SetFaction(syncEvent->faction);
		msg->SetOwnerPosition(syncEvent->ownerPosition);
		msg->SetIdentifier(syncEvent->internalId);
		msg->SetPosition(syncEvent->positionf);
		auto netMsg = msg->CreateMessage();
		communicator->PushMessageForSending(netMsg);
	}
}

void HydNetworkSender::Update(const uint64 delta, const uint64 absolute) {

	if (connectionType == HydroqConnectionType::CLIENT || connectionType == HydroqConnectionType::SERVER) {
		if (IsProperTime(lastUpdateMsgTime, absolute, this->updateFrequency)) {

			lastUpdateMsgTime = absolute;
			auto communicator = GETCOMPONENT(NetworkCommunicator);

			// send update message regularly
			spt<UpdateInfo> updateInf = spt<UpdateInfo>(new UpdateInfo());

			auto& dynObjects = model->GetMovingObjects();

			// update transformation of all objects
			for (auto& dynObj : dynObjects) {
				if (dynObj->GetSecondaryId() == 0) {

					int id = dynObj->GetId();
					auto transform = dynObj->GetTransform();

					auto& updates = updateInf->GetContinuousValues();

					updates[id * 3 + 0] = transform.rotation;
					updates[id * 3 + 1] = transform.localPos.x;
					updates[id * 3 + 2] = transform.localPos.y;
				}
			}

			auto msg = new UpdateMessage(updateInf);
			spt<NetOutputMessage> netMsg = spt<NetOutputMessage>(new NetOutputMessage(1));
			netMsg->SetData(msg);
			netMsg->SetMsgTime(absolute);
			netMsg->SetAction(StrId(NET_MSG_UPDATE));

			if (communicator->GetNetworkState() == NetworkComState::COMMUNICATING) {
				communicator->PushMessageForSending(netMsg);
			}
		}
	}
}
