#pragma once

#include "ofxCogMain.h"
#include "Events.h"
#include "HydMap.h"
#include "GameTask.h"
#include "HydEntity.h"

enum class ObjectChangeType {
	STATIC_CHANGED = 0,
	DYNAMIC_CREATED = 1,
	DYNAMIC_REMOVED = 2,
	MOVING_CREATED = 3,
	MOVING_REMOVED = 4,
	ATTRACTOR_CREATED = 5,
	ATTRACTOR_REMOVED = 6,
	RIG_CAPTURED = 7,
	RIG_TAKEN = 8
};

class MapObjectChangedEvent : public MsgEvent {
public:
	ObjectChangeType changeType;
	HydMapNode* changedMapNode;
	Node* changedNode = nullptr;

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedMapNode) :
		changedMapNode(changedMapNode), changeType(changeType)
	{

	}

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedMapNode, Node* changedNode) :
		changedMapNode(changedMapNode), changeType(changeType), changedNode(changedNode)
	{

	}
};

class TaskAbortEvent : public MsgEvent {
public:
	spt<GameTask> taskToAbort;

	TaskAbortEvent(spt<GameTask> taskToAbort) :
		taskToAbort(taskToAbort)
	{

	}
};

enum class SyncEventType {
	OBJECT_CREATED,
	OBJECT_REMOVED,
	MAP_CHANGED
};

enum class Faction;

// multiplayer synchronization event
class SyncEvent : public MsgEvent {
public:
	SyncEventType eventType;
	EntityType entityType;
	ofVec2f positionf;
	Faction faction;
	int internalId;
	int externalId;

	SyncEvent() {

	}

	SyncEvent(SyncEventType eventType, EntityType entityType, Faction faction, ofVec2f position, int internalId, int externalId) :
		eventType(eventType), entityType(entityType), faction(faction), 
		positionf(position), internalId(internalId), externalId(externalId) {
	}
};