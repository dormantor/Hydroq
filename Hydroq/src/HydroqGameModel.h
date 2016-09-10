#pragma once

#include "Component.h"
#include "ofxSQLite.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"

#include "MapObjectChangedEvent.h"

/**
* Hydroq game model
*/
class HydroqGameModel : public Component {

	OBJECT(HydroqGameModel)

private:
	// static objects (excluding buildings)
	HydMap* hydroqMap;
	// dynamic objects (including buildings)
	map<Vec2i, spt<HydEntity>> dynObjects;

public:

	~HydroqGameModel() {

	}

	void Init() {
		hydroqMap = new HydMap();
	}

	void Init(spt<ofxXml> xml) {
		hydroqMap = new HydMap();
	}

	HydMap* GetMap() {
		return hydroqMap;
	}

	bool IsPositionFreeForBuilding(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		return node->mapNodeType == MapNodeType::GROUND && !node->occupied;
	}

	void CreateBuilding(Vec2i position, spt<HydEntity> building) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		node->occupied = true;
		dynObjects[position] = building;
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0, 
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CHANGED, node,building), nullptr);
	}

	bool IsPositionFreeForBridge(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::WATER && dynObjects.find(position) == dynObjects.end();

		if (isFree) {
			// at least one neighbor mustn't be water or it is already marked
			for (auto neighbor : node->GetNeighbors()) {
				if (neighbor->mapNodeType != MapNodeType::WATER || 
					(dynObjects.find(neighbor->pos) != dynObjects.end() && 
						dynObjects[neighbor->pos]->entityType == EntityType::BRIDGE_MARK))
					return true;
			}
		}

		return false;
	}

	bool PositionContainsBridgeMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->entityType == EntityType::BRIDGE_MARK);
	}

	void MarkPositionForBridge(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		dynObjects[position] = spt<HydEntity>(new HydBridge());
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CHANGED, node, dynObjects[position]), nullptr);
	}

	bool IsPositionFreeForForbid(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	bool PositionContainsForbidMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->entityType == EntityType::FORBID_MARK);
	}

	void MarkPositionForForbid(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		dynObjects[position] = spt<HydEntity>(new ForbidMark());
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CHANGED, node, dynObjects[position]), nullptr);
	}

	bool IsPositionFreeForGuard(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	bool PositionContainsGuardMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->entityType == EntityType::GUARD_MARK);
	}

	void MarkPositionForGuard(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		dynObjects[position] = spt<HydEntity>(new GuardMark());
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CHANGED, node, dynObjects[position]), nullptr);
	}

	bool IsPositionFreeForDestroy(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = (node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end());
		return isFree;
	}

	bool PositionContainsDestroyMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->entityType == EntityType::BRIDGE_MARK);
	}

	void MarkPositionForDestroy(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);

		if (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->entityType == EntityType::BRIDGE_MARK) {
			// delete bridge mark
			DestroyDynamicObject(position);
		}
		else {
			dynObjects[position] = spt<HydEntity>(new DestroyMark());
			SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
				new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CHANGED, node, dynObjects[position]), nullptr);
		}
	}

	void DestroyDynamicObject(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		auto obj = dynObjects[position];
		dynObjects.erase(position);
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj), nullptr);
	}
};