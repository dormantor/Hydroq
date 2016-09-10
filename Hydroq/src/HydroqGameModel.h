#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "MapObjectChangedEvent.h"
#include "Seedbed.h"
#include "StateMachine.h"
#include "Worker.h"
#include "GameTask.h"
#include "Move.h"

/**
* Hydroq game model
*/
class HydroqGameModel : public Component {

	OBJECT(HydroqGameModel)

private:
	// static objects (excluding buildings)
	HydMap* hydroqMap;
	// dynamic objects (including buildings)
	map<Vec2i, Node*> dynObjects;
	vector<Node*> movingObjects;
	Scene* gameScene;
	Node* rootNode;
	vector<spt<GameTask>> gameTasks;
	
public:

	~HydroqGameModel() {

	}

	void Init() {
		hydroqMap = new HydMap();
		gameScene = new Scene("gamescene", false);
		rootNode = gameScene->GetSceneNode();

	}

	void Init(spt<ofxXml> xml) {
		Init();
	}

	HydMap* GetMap() {
		return hydroqMap;
	}

	bool IsPositionFreeForBuilding(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		return node->mapNodeType == MapNodeType::GROUND && !node->occupied;
	}

	Node* CreateDynamicObject(Vec2i position, EntityType entityType) {
		auto hydMapNode = hydroqMap->GetNode(position.x, position.y);
		hydMapNode->occupied = true;
		auto gameNode = CreateNode(entityType, position);
		dynObjects[position] = gameNode;

		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0, 
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CREATED, hydMapNode, gameNode), nullptr);
		return gameNode;
	}

	void CreateMovingObject(ofVec2f position, EntityType entityType) {
		auto gameNode = CreateNode(EntityType::WORKER, Vec2i(position.x, position.y));
		movingObjects.push_back(gameNode);
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::MOVING_CREATED, nullptr, gameNode), nullptr);
	}

	bool IsPositionFreeForBridge(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::WATER && dynObjects.find(position) == dynObjects.end();

		if (isFree) {
			// at least one neighbor mustn't be water or it is already marked
			for (auto neighbor : node->GetNeighbors()) {
				if (neighbor->mapNodeType != MapNodeType::WATER || 
					(dynObjects.find(neighbor->pos) != dynObjects.end() && 
						dynObjects[neighbor->pos]->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == EntityType::BRIDGE_MARK))
					return true;
			}
		}

		return false;
	}

	bool PositionContainsBridgeMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == EntityType::BRIDGE_MARK);
	}

	void MarkPositionForBridge(Vec2i position) {
		auto node = CreateDynamicObject(position, EntityType::BRIDGE_MARK);
		auto newTask = spt<GameTask>(new GameTask(StringHash(TASK_BRIDGE_BUILD)));
		newTask->taskNode = node;
		gameTasks.push_back(newTask);
	}

	bool IsPositionFreeForForbid(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	bool PositionContainsForbidMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && 
			dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == EntityType::FORBID_MARK);
	}

	void MarkPositionForForbid(Vec2i position) {
		CreateDynamicObject(position, EntityType::FORBID_MARK);
	}

	bool IsPositionFreeForGuard(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	bool PositionContainsGuardMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && 
			dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == EntityType::GUARD_MARK);
	}

	void MarkPositionForGuard(Vec2i position) {
		CreateDynamicObject(position, EntityType::GUARD_MARK);
	}

	bool IsPositionFreeForDestroy(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = (node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end());
		return isFree;
	}

	bool PositionContainsDestroyMark(Vec2i position) {
		return (dynObjects.find(position) != dynObjects.end() && 
			dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == EntityType::DESTROY_MARK);
	}

	void MarkPositionForDestroy(Vec2i position) {
		CreateDynamicObject(position, EntityType::DESTROY_MARK);
	}

	void SpawnWorker(ofVec2f position) {
		CreateMovingObject(position, EntityType::WORKER);
	}

	void DestroyDynamicObject(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		node->occupied = false;
		node->forbidden = false;
		auto obj = dynObjects[position];
		dynObjects.erase(position);
		rootNode->RemoveChild(obj, true);

		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj), nullptr);
	}

	Node* CreateNode(EntityType entityType, ofVec2f position) {

		string name;
		Node* nd = new Node("");

		if (entityType == EntityType::BRIDGE_MARK) {
			nd->SetTag("bridgemark");
		}
		else if (entityType == EntityType::DESTROY_MARK) {
			nd->SetTag("destroymark");
		}
		else if (entityType == EntityType::FORBID_MARK) {
			nd->SetTag("forbidmark");
		}
		else if (entityType == EntityType::GUARD_MARK) {
			nd->SetTag("guardmark");
		}
		else if (entityType == EntityType::SEEDBED) {
			nd->SetTag("seedbed");
			auto nodeBeh = new Seedbed();
			nd->AddBehavior(nodeBeh);
		}
		else if (entityType == EntityType::WORKER) {
			nd->SetTag("worker");
			auto nodeBeh = new StateMachine();
			
			((StateMachine*)nodeBeh)->ChangeState(new WorkerIdleState());
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerBridgeBuildState());

			nd->AddBehavior(nodeBeh);
			// add moving behavior
			nd->AddBehavior(new Move());
		}

		nd->AddAttr(ATTR_GAME_ENTITY_TYPE, entityType);
		nd->GetTransform().localPos.x = position.x;
		nd->GetTransform().localPos.y = position.y;
		

		rootNode->AddChild(nd);
		return nd;
	}

	map<Vec2i, Node*>& GetDynamicObjects() {
		return this->dynObjects;
	}

	vector<Node*>& GetMovingObjects() {
		return this->movingObjects;
	}

	vector<spt<GameTask>>& GetGameTasks() {
		return this->gameTasks;
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
		rootNode->SubmitChanges(true);
		rootNode->Update(delta, absolute);
	}
};