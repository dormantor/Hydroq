#include "HydNetworkReceiver.h"
#include "ComponentStorage.h"
#include "GameView.h"
#include "Stage.h"

void HydNetworkReceiver::OnInit() {
	auto playerModel = GETCOMPONENT(PlayerModel);
	if (playerModel->GetConnectionType() == HydroqConnectionType::NONE) {
		owner->RemoveBehavior(this, true);
	}

	SubscribeForMessages(ACT_NET_MESSAGE_RECEIVED, ACT_NET_CONNECTION_LOST, ACT_NET_CONNECTED, ACT_NET_DISCONNECTED);
	model = owner->GetBehavior<GameModel>();
	view = owner->GetBehavior<GameView>();
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
			if (action == NET_MSG_UPDATE) {
				// update message
				ProcessUpdateMsg(netMsg);
			}
			else if (action == NET_MULTIPLAYER_COMMAND) {
				// command message
				ProcessCommandMessage(netMsg);
			}
		}
	}
	else if (msg.HasAction(ACT_NET_CONNECTION_LOST) || msg.HasAction(ACT_NET_DISCONNECTED)) {
			this->view->OnMultiplayerConnectionLost();
	}
}

void HydNetworkReceiver::ProcessUpdateMsg(spt<NetInputMessage> netMsg) {
	// just transform message and resend it to the Interpolator component
	spt<UpdateMessage> updateMsg = netMsg->GetData<UpdateMessage>();
	spt<UpdateInfo> deltaInfo = spt<UpdateInfo>(new UpdateInfo(netMsg->GetMsgTime(), updateMsg->GetContinuousValues(), updateMsg->GetDiscreteValues()));

	auto deltaUpdate = GETCOMPONENT(Interpolator);
	deltaUpdate->AcceptUpdateMessage(deltaInfo);
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
