#pragma once

#include "ofxCogCommon.h"
#include "NetMessage.h"
#include "DeltaMessage.h"
#include "HydroqDef.h"

using namespace Cog;

enum class Faction;


class HydroqServerInitMsg  : public NetData{
	string ipAddress; // external attribute

	string map;
	Faction faction;

public:
	HydroqServerInitMsg() {

	}

	HydroqServerInitMsg(string ipAddress) : ipAddress(ipAddress) {

	}

	void LoadFromStream(NetReader* reader) {
		this->faction = (Faction)reader->ReadByte();
		this->map = reader->ReadString();
	}

	void SaveToStream(NetWriter* writer) {
		writer->WriteByte((tBYTE)faction);
		writer->WriteString(map);
	}


	int GetDataLength() {
		return
			sizeof(tBYTE) + // faction
			SIZE_STR(map);
	}

	Faction GetFaction() {
		return faction;
	}

	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	string GetMap() {
		return map;
	}

	void SetMap(string map) {
		this->map = map;
	}

	string GetIpAddress() {
		return ipAddress;
	}

	void SetIpAddress(string address) {
		this->ipAddress = address;
	}

	spt<NetOutputMessage> CreateMessage() {
		auto outputMsg = spt<NetOutputMessage>(new NetOutputMessage(0));
		outputMsg->SetAction(NET_MULTIPLAYER_INIT);
		outputMsg->SetData(this);
		return outputMsg;
	}
};

enum class SyncEventType;
enum class EntityType;
enum class Faction;

class HydroqCommandMsg : public NetData {
	SyncEventType eventType;
	EntityType entityType;
	ofVec2f position;
	Faction faction;
	int identifier;

public:
	HydroqCommandMsg() {

	}

	void LoadFromStream(NetReader* reader) {
		this->eventType = (SyncEventType)reader->ReadByte();
		this->entityType = (EntityType)reader->ReadByte();
		this->position = ofVec2f(reader->ReadDWord(), reader->ReadDWord());
		this->faction = (Faction)reader->ReadByte();
		this->identifier = reader->ReadDWord();
	}

	void SaveToStream(NetWriter* writer) {
		writer->WriteByte((tBYTE)eventType);
		writer->WriteByte((tBYTE)entityType);
		writer->WriteDWord(position.x);
		writer->WriteDWord(position.y);
		writer->WriteByte((tBYTE)faction);
		writer->WriteDWord(identifier);
	}

	int GetDataLength() {
		return sizeof(tBYTE) * 3 + sizeof(tDWORD) * 3;
	}

	SyncEventType GetEventType() {
		return eventType;
	}

	void SetEventType(SyncEventType eventType) {
		this->eventType = eventType;
	}

	EntityType GetEntityType() {
		return entityType;
	}

	void SetEntityType(EntityType entityType) {
		this->entityType = entityType;
	}

	ofVec2f GetPosition() {
		return position;
	}

	void SetPosition(ofVec2f position) {
		this->position = position;
	}

	Faction GetFaction() {
		return faction;
	}

	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	int GetIdentifier() {
		return identifier;
	}

	void SetIdentifier(int identifier) {
		this->identifier = identifier;
	}

	spt<NetOutputMessage> CreateMessage() {
		auto outputMsg = spt<NetOutputMessage>(new NetOutputMessage(0));
		outputMsg->SetAction(NET_MULTIPLAYER_COMMAND);
		outputMsg->SetData(this);
		return outputMsg;
	}
};

