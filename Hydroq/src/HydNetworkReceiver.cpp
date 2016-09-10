#include "HydNetworkReceiver.h"

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
		auto msgEvent = msg.GetData<NetworkMsgEvent>();
		auto netMsg = msgEvent->msg;
		StrId action = netMsg->GetAction();
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

void HydNetworkReceiver::ProcessDeltaUpdate(spt<NetInputMessage> netMsg) {
	spt<DeltaMessage> deltaMsg = netMsg->GetData<DeltaMessage>();
	spt<DeltaInfo> deltaInfo = spt<DeltaInfo>(new DeltaInfo(netMsg->GetMsgTime(), deltaMsg->GetDeltas(), deltaMsg->GetTeleports()));

	auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
	deltaUpdate->AcceptDeltaUpdate(deltaInfo);
}

void HydNetworkReceiver::ProcessMultiplayerInit(spt<NetInputMessage> netMsg) {
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
		break;
	case SyncEventType::MAP_CHANGED:
		switch (cmdMsg->GetEntityType()) {
		case EntityType::BRIDGE:
			cout << "creating platform as it says message " << (int)netMsg->GetSyncId() << " send by " << netMsg->GetSourceIp() << " from port " << netMsg->GetSourcePort() << endl;
			model->BuildPlatform(cmdMsg->GetPosition(), cmdMsg->GetFaction(), 1); // there is no identifier required for such an action
			break;
		case EntityType::WATER:
			model->DestroyPlatform(cmdMsg->GetPosition(), cmdMsg->GetFaction(), 1); // there is no identifier required for such an action
			break;
		}
		break;
	}
}
