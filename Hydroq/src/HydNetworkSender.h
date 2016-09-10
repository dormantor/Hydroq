#pragma once

#include "Component.h"
#include "DeltaInfo.h"
#include "MsgEvents.h"
#include "DeltaMessage.h"

namespace Cog {

	enum class HydroqNetworkState {
		NONE,SERVER, CLIENT
	};

	class HydNetworkSender : public Component {
		OBJECT(HydNetworkSender)

	private:
		HydroqNetworkState networkState = HydroqNetworkState::NONE;
	public:


		void Init() {
			RegisterGlobalListening(ACT_MAP_OBJECT_CHANGED);
		}

		void OnMessage(Msg& msg) {
			if (msg.HasAction(ACT_MAP_OBJECT_CHANGED)) {
				MapObjectChangedEvent* evt = static_cast<MapObjectChangedEvent*>(msg.GetData());

				if (evt->changeType == ObjectChangeType::DYNAMIC_CREATED || evt->changeType == ObjectChangeType::MOVING_CREATED) {
					// new dynamic or moving object
					auto trans = evt->changedNode->GetTransform();
					// create new sprite
					string spriteTag = evt->changedNode->GetTag();

					// moving objects will have its origin set to the middle of the cell
					if (evt->changeType == ObjectChangeType::MOVING_CREATED) {
					
					}
					else {

					}
				}
				else if (evt->changeType == ObjectChangeType::DYNAMIC_REMOVED || evt->changeType == ObjectChangeType::MOVING_REMOVED) {

					// dynamic or moving object removed
					int identifier = evt->changedNode->GetId();
				}
				else if (evt->changeType == ObjectChangeType::STATIC_CHANGED) {
					
					auto mapNode = evt->changedMapNode;
					auto position = mapNode->pos;
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
				auto communicator = GETCOMPONENT(NetworkCommunicator);

				// send delta message regularly
				spt<DeltaInfo> deltaInf = spt<DeltaInfo>(new DeltaInfo());

				auto model = GETCOMPONENT(HydroqGameModel);
				auto& dynObjects = model->GetMovingObjects();

				// update transformation of all objects
				for (auto& dynObj : dynObjects) {
					int id = dynObj->GetId();
					auto transform = dynObj->GetTransform();

					deltaInf->deltas[StringHash(id * 3 + 0)] = transform.rotation;
					deltaInf->deltas[StringHash(id * 3 + 1)] = transform.localPos.x;
					deltaInf->deltas[StringHash(id * 3 + 2)] = transform.localPos.y;
				}

				auto msg = spt<DeltaMessage>(new DeltaMessage(deltaInf));
				spt<NetOutputMessage> netMsg = spt<NetOutputMessage>(new NetOutputMessage(1));
				netMsg->SetData(msg);
				netMsg->SetAction(StringHash(NET_MSG_DELTA_UPDATE));

				communicator->GetClient()->SetMessageForSending(netMsg);
			}
		}

	private:

	};

}// namespace