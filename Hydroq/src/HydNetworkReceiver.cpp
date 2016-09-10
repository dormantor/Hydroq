#include "HydNetworkReceiver.h"
#include "ComponentStorage.h"

void HydNetworkReceiver::OnInit() {
	auto playerModel = GETCOMPONENT(PlayerModel);
	if (playerModel->GetNetworkState() == HydroqNetworkState::NONE) {
		owner->RemoveBehavior(this, true);
	}

	SubscribeForMessages(ACT_NET_MESSAGE_RECEIVED);
	model = owner->GetBehavior<GameModel>();
}

void HydNetworkReceiver::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_NET_MESSAGE_RECEIVED)) {

		// distinguish the type of this message and call proper method
		auto msgEvent = msg.GetData<NetworkMsgEvent>();
		auto netMsg = msgEvent->msg;
		StrId action = netMsg->GetAction();
		auto type = netMsg->GetMsgType();

		if (type == NetMsgType::DISCOVER_RESPONSE) {
			if (action == NET_MULTIPLAYER_INIT) {
				// initialization message
				ProcessMultiplayerInit(netMsg);
			}
		}
		else if (type == NetMsgType::UPDATE) {
			if (action == NET_MSG_DELTA_UPDATE) {
				// update message
				ProcessDeltaUpdate(netMsg);
			}
			else if (action == NET_MULTIPLAYER_COMMAND) {
				// command message
				ProcessCommandMessage(netMsg);
			}
		}
	}
}

void HydNetworkReceiver::ProcessDeltaUpdate(spt<NetInputMessage> netMsg) {
	// just transform message and resend it to the DeltaUpdate component
	spt<DeltaMessage> deltaMsg = netMsg->GetData<DeltaMessage>();
	spt<DeltaInfo> deltaInfo = spt<DeltaInfo>(new DeltaInfo(netMsg->GetMsgTime(), deltaMsg->GetDeltas(), deltaMsg->GetTeleports()));

	auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
	deltaUpdate->AcceptDeltaUpdate(deltaInfo);
}

void HydNetworkReceiver::ProcessMultiplayerInit(spt<NetInputMessage> netMsg) {
	// send message that a host has been found
	auto mpInit = netMsg->GetData<HydroqServerInitMsg>();
	mpInit->SetIpAddress(netMsg->GetSourceIp());
	SendMessage(StrId(ACT_SERVER_FOUND), spt<CommonEvent<HydroqServerInitMsg>>(new CommonEvent<HydroqServerInitMsg>(mpInit)));
}

void HydNetworkReceiver::ProcessCommandMessage(spt<NetInputMessage> netMsg) {
	auto cmdMsg = netMsg->GetData<HydroqCommandMsg>();

	switch (cmdMsg->GetEventType()) {
	case SyncEventType::OBJECT_CREATED:
		switch (cmdMsg->GetEntityType()) {
		case EntityType::WORKER:
			model->SpawnWorker(cmdMsg->GetPosition(), cmdMsg->GetFaction(), cmdMsg->GetIdentifier(), cmdMsg->GetOwnerPosition());
			break;
		}
		break;
	case SyncEventType::OBJECT_REMOVED:
		// nothing to do here
		break;
	case SyncEventType::MAP_CHANGED:
		switch (cmdMsg->GetEntityType()) {
		case EntityType::BRIDGE:
			model->BuildPlatform(cmdMsg->GetPosition(), cmdMsg->GetFaction(), 1); // there is no identifier required for such an action
			break;
		case EntityType::WATER:
			model->DestroyPlatform(cmdMsg->GetPosition(), cmdMsg->GetFaction(), 1); // there is no identifier required for such an action
			break;
		}
		break;
	}
}
