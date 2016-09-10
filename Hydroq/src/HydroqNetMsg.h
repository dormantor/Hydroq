#pragma once

#include "NetMessage.h"
#include "UpdateMessage.h"
#include "HydroqDef.h"
#include "Vec2i.h"

using namespace Cog;

/** 
* Initialization network message
*/
class HydroqServerInitMsg  : public NetData{
	string ipAddress; // external attribute, not sent by the network

	string map; // name of selected map
	Faction faction; // name of selected faction

public:
	HydroqServerInitMsg() {

	}

	HydroqServerInitMsg(string ipAddress) : ipAddress(ipAddress) {

	}

	void LoadFromStream(NetReader* reader);

	void SaveToStream(NetWriter* writer);

	int GetDataLength() {
		return sizeof(tBYTE) + // faction
			SIZE_STR(map);
	}

	Faction GetFaction() const {
		return faction;
	}

	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	string GetMap() const {
		return map;
	}

	void SetMap(string map) {
		this->map = map;
	}

	/**
	* Gets ip address of the sender
	*/
	string GetIpAddress() const {
		return ipAddress;
	}

	/**
	* Sets ip address of the sender
	*/
	void SetIpAddress(string address) {
		this->ipAddress = address;
	}

	/**
	* Transforms this object to the general network output message
	*/
	spt<NetOutputMessage> CreateMessage();
};

enum class SyncEventType;
enum class EntityType;

/**
* Network message carrying command
*/
class HydroqCommandMsg : public NetData {
	// type of event
	SyncEventType eventType;
	// type of entity involved
	EntityType entityType;
	// position where it happened (if defined)
	ofVec2f position;
	// faction involved
	Faction faction;
	// identifier (when an entity is created)
	int identifier;
	// position of a node that invoked the event
	Vec2i ownerPosition;

public:
	HydroqCommandMsg() {

	}

	void LoadFromStream(NetReader* reader);

	void SaveToStream(NetWriter* writer);

	int GetDataLength() {
		return sizeof(tBYTE) * 3 + sizeof(tDWORD) * 1 + sizeof(float)*2 + sizeof(tDWORD)*2;
	}

	/**
	* Gets type of the event that occurred
	*/
	SyncEventType GetEventType() const {
		return eventType;
	}

	/**
	* Sets type of the event that occurred
	*/
	void SetEventType(SyncEventType eventType) {
		this->eventType = eventType;
	}

	/**
	* Gets type of the entity involved
	*/
	EntityType GetEntityType() const {
		return entityType;
	}

	/**
	* Sets type of the entity involved
	*/
	void SetEntityType(EntityType entityType) {
		this->entityType = entityType;
	}

	/**
	* Gets position where the action happened (if defined)
	*/
	ofVec2f GetPosition() const {
		return position;
	}

	/**
	* Sets position where the action happened (if defined)
	*/
	void SetPosition(ofVec2f position) {
		this->position = position;
	}

	/**
	* Gets type of faction involved
	*/
	Faction GetFaction() const {
		return faction;
	}

	/**
	* Sets type of faction involved
	*/
	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	/**
	* Gets identifier of created entity (if any)
	*/
	int GetIdentifier() const {
		return identifier;
	}

	/**
	* Sets identifier of created entity (if any)
	*/
	void SetIdentifier(int identifier) {
		this->identifier = identifier;
	}

	/**
	* Gets position of a node that invoked the event
	*/
	Vec2i GetOwnerPosition() const {
		return ownerPosition;
	}

	/**
	* Sets position of a node that invoked the event
	*/
	void SetOwnerPosition(Vec2i ownerPosition) {
		this->ownerPosition = ownerPosition;
	}

	/**
	* Transforms this object to the general network output message
	*/
	spt<NetOutputMessage> CreateMessage();
};

