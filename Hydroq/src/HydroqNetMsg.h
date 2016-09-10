#pragma once

#include "ofxCogCommon.h"
#include "NetMessage.h"
#include "DeltaMessage.h"
#include "HydroqDef.h"

using namespace Cog;

enum class Faction;

/**
* Common hydroq net message without delta update
*/
class HydroqNetMsg : public NetData{
public:
	StringHash paramStrHash = StringHash();
	Vec2i paramVec2i = Vec2i(0);
	ofVec2f paramVec2f = ofVec2f(0);
	int paramInt = 0;
	string paramStr = "";

	void LoadFromStream(NetReader* reader);

	void SaveToStream(NetWriter* writer);

	int GetDataLength() {
		return sizeof(unsigned)+ // paramStrHash
			sizeof(paramVec2i)+ // paramVec2i
			sizeof(paramVec2f)+ // paramVec2f
			sizeof(int)+ // paramInt
			sizeof(tBYTE)*(paramStr.size())+ // paramStr
			sizeof(unsigned); // size of paramStr
	}
};

class HydroqServerInitMsg {
	spt<HydroqNetMsg> msg;
	string ipAddress;

public:
	HydroqServerInitMsg() {
		msg = spt<HydroqNetMsg>(new HydroqNetMsg());
	}

	HydroqServerInitMsg(spt<HydroqNetMsg> msg, string ipAddress) : msg(msg), ipAddress(ipAddress) {

	}

	Faction GetFaction() {
		return (Faction)msg->paramInt;
	}

	void SetFaction(Faction faction) {
		msg->paramInt = (int)faction;
	}

	string GetMap() {
		return msg->paramStr;
	}

	void SetMap(string map) {
		msg->paramStr = map;
	}

	string GetIpAddress() {
		return ipAddress;
	}

	spt<HydroqNetMsg> GetMsgData() {
		return msg;
	}

	spt<NetOutputMessage> CreateMessage() {
		auto outputMsg = spt<NetOutputMessage>(new NetOutputMessage(0));
		outputMsg->SetAction(NET_MULTIPLAYER_INIT);
		outputMsg->SetData(msg);
		return outputMsg;
	}
};