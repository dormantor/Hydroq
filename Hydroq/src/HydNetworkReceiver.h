#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "HydroqNetMsg.h"
#include "ofxCogMain.h"
#include "DeltaUpdate.h"
#include "HydroqDef.h"

namespace Cog {

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
				else if (type == NetMsgType::UPDATE || type == NetMsgType::CALLBACK_UPDATE) {
					if (action == NET_MSG_DELTA_UPDATE) {
						ProcessDeltaUpdate(netMsg);
					}
				}
			}
		}

		void ProcessDeltaUpdate(spt<NetInputMessage> netMsg) {
			spt<DeltaMessage> deltaMsg = netMsg->GetData<DeltaMessage>();
			spt<DeltaInfo> deltaInfo = spt<DeltaInfo>(new DeltaInfo());
			deltaInfo->deltas = deltaMsg->deltas;
			deltaInfo->teleports = deltaMsg->teleports;
			deltaInfo->time = netMsg->GetMsgTime();

			auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
			deltaUpdate->AcceptDeltaUpdate(deltaInfo);
		}

		void ProcessMultiplayerInit(spt<NetInputMessage> netMsg) {
			auto mpInit = netMsg->GetData<HydroqNetMsg>();
			auto wrapper = spt<HydroqServerInitMsg>(new HydroqServerInitMsg(mpInit, netMsg->GetSourceIp()));
			SendMessageToListeners(StringHash(ACT_SERVER_FOUND), 0, new CommonEvent<HydroqServerInitMsg>(wrapper), nullptr);
		}

		virtual void Update(const uint64 delta, const uint64 absolute) {

		}

	private:

	};

}// namespace