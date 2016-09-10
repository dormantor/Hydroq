#pragma once

#include "ofxCogMain.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "GameTask.h"
#include "GameEntity.h"
#include "HydroqDef.h"

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

class MapObjectChangedEvent : public MsgPayload {
public:
	ObjectChangeType changeType;
	GameMapNode* changedMapNode;
	Node* changedNode = nullptr;

	MapObjectChangedEvent(ObjectChangeType changeType, GameMapNode* changedMapNode) :
		changedMapNode(changedMapNode), changeType(changeType)
	{

	}

	MapObjectChangedEvent(ObjectChangeType changeType, GameMapNode* changedMapNode, Node* changedNode) :
		changedMapNode(changedMapNode), changeType(changeType), changedNode(changedNode)
	{

	}
};

enum class GameChangeType {
	EMPTY_RIG_CAPTURED,
	ENEMY_RIG_CAPTURED
};

class GameStateChangedEvent : public MsgPayload {
public:
	GameChangeType changeType;
	Faction ownerFaction;

	GameStateChangedEvent(GameChangeType changeType, Faction ownerFaction) : changeType(changeType), ownerFaction(ownerFaction) {

	}
};

class TaskAbortEvent : public MsgPayload {
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

// multiplayer synchronization event
class SyncEvent : public MsgPayload {
public:
	SyncEventType eventType;
	EntityType entityType;
	ofVec2f positionf;
	Faction faction;
	int internalId;
	int externalId;
	Vec2i ownerPosition;

	SyncEvent() {

	}

	SyncEvent(SyncEventType eventType, EntityType entityType, Faction faction, ofVec2f position, int internalId, int externalId, Vec2i ownerPosition) :
		eventType(eventType), entityType(entityType), faction(faction), 
		positionf(position), internalId(internalId), externalId(externalId), ownerPosition(ownerPosition) {
	}
};

class TileClickEvent : public MsgPayload {
public:

	int brickPosX;
	int brickPosY;
	spt<SpriteInst> brick;

	TileClickEvent(int brickPosX, int brickPosY, spt<SpriteInst> brick) :
		brickPosX(brickPosX), brickPosY(brickPosY), brick(brick)
	{

	}
};