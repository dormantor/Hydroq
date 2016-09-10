#pragma once

#include "MsgPayloads.h"
#include "GameMap.h"
#include "GameTask.h"
#include "HydroqDef.h"
#include "SpriteInst.h"

/**
* Type of change
*/
enum class ObjectChangeType {
	STATIC_CHANGED = 0,		/** static object changed (water) */
	DYNAMIC_CREATED = 1,	/** dynamic object created */
	DYNAMIC_REMOVED = 2,	/** dynamic object removed */
	MOVING_CREATED = 3,		/** moving object created */
	MOVING_REMOVED = 4,		/** moving object removed */
	ATTRACTOR_CREATED = 5,	/** attractor created */
	ATTRACTOR_REMOVED = 6,	/** attractor removed */
	RIG_CAPTURED = 7,		/** enemy rig has been captured */
	RIG_TAKEN = 8			/** empty rig has been taken */
};

/**
* Event which occurs when a map object has been changed
*/
class MapObjectChangedEvent : public MsgPayload {
public:
	// type of change
	ObjectChangeType changeType;
	// changed map tile
	GameMapTile* changedMapTile;
	// changed game node
	Node* changedNode = nullptr;

	MapObjectChangedEvent(ObjectChangeType changeType, GameMapTile* changedMapTile) :
		changedMapTile(changedMapTile), changeType(changeType)
	{

	}

	MapObjectChangedEvent(ObjectChangeType changeType, GameMapTile* changedMapTile, Node* changedNode) :
		changedMapTile(changedMapTile), changeType(changeType), changedNode(changedNode)
	{

	}
};

/**
* Type of game state change
*/
enum class GameChangeType {
	EMPTY_RIG_CAPTURED,		/** empty rig captured */
	ENEMY_RIG_CAPTURED		/** enemy rig captured */
};

/**
* Event which occurs when a game state has been changed
*/
class GameStateChangedEvent : public MsgPayload {
public:
	GameChangeType changeType;
	Faction ownerFaction;

	GameStateChangedEvent(GameChangeType changeType, Faction ownerFaction) : changeType(changeType), ownerFaction(ownerFaction) {

	}
};

/**
* Event which occurs when a task has been aborted
*/
class TaskAbortEvent : public MsgPayload {
public:
	spt<GameTask> taskToAbort;

	TaskAbortEvent(spt<GameTask> taskToAbort) :
		taskToAbort(taskToAbort)
	{

	}
};

/**
* Event that occurs when a user clicks on a sprite inside the gameboard
*/
class SpriteClickEvent : public MsgPayload {
public:
	// x position of clicked sprite 
	int spritePosX;
	// y position of clicked sprite
	int spritePosY;
	// clicked sprite
	spt<SpriteInst> sprite;

	SpriteClickEvent(int spritePosX, int spritePosY, spt<SpriteInst> sprite) :
		spritePosX(spritePosX), spritePosY(spritePosY), sprite(sprite)
	{

	}
};

/**
* Type of synchronization event
*/
enum class SyncEventType {
	OBJECT_CREATED,		/** object has been created */
	OBJECT_REMOVED,		/** object has been removed */
	MAP_CHANGED			/** map has been changed */
};

/**
* Multiplayer synchronization event
*/
class SyncEvent : public MsgPayload {
public:
	// type of event
	SyncEventType eventType;
	// type of entity involved
	EntityType entityType;
	// float position of entity
	ofVec2f positionf;
	// faction involved
	Faction faction;
	// id of internal entity (if created on the same peer)
	int internalId;
	// id of external entity (if created on another peer)
	int externalId;
	// position of node that invoked the event
	Vec2i ownerPosition;

	SyncEvent() {

	}

	SyncEvent(SyncEventType eventType, EntityType entityType, Faction faction, ofVec2f position, int internalId, int externalId, Vec2i ownerPosition) :
		eventType(eventType), entityType(entityType), faction(faction), 
		positionf(position), internalId(internalId), externalId(externalId), ownerPosition(ownerPosition) {
	}
};
