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
	// static objects (map, water, platforms)
	HydMap* hydroqMap;
	// dynamic objects (buildings)
	map<Vec2i, Node*> dynObjects;
	// moving objects (units)
	vector<Node*> movingObjects;
	
	// scene and its root node that run separately from the view
	// part of the game
	Scene* gameScene;
	Node* rootNode;

	// list of waiting tasks
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

	/**
	* Returns true, if a building can be built on selected position
	*/
	bool IsPositionFreeForBuilding(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		return node->mapNodeType == MapNodeType::GROUND && !node->occupied;
	}

	/**
	* Returns true, if a bridge (platform) can be built on selected position
	*/
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

	/**
	* Returns true, if a bridge mark is placed on selected position
	*/
	bool PositionContainsBridgeMark(Vec2i position) {
		return IsPositionOfType(position, EntityType::BRIDGE_MARK);
	}

	/**
	* Marks position for a new bridge
	*/
	void MarkPositionForBridge(Vec2i position) {
		auto node = CreateDynamicObject(position, EntityType::BRIDGE_MARK);
		auto newTask = spt<GameTask>(new GameTask(StringHash(TASK_BRIDGE_BUILD)));
		newTask->taskNode = node;
		gameTasks.push_back(newTask);
	}

	/**
	* Returns true, if the position can be forbidden
	*/
	bool IsPositionFreeForForbid(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	/**
	* Returns true, if the selected position is forbidden
	*/
	bool PositionContainsForbidMark(Vec2i position) {
		return IsPositionOfType(position, EntityType::FORBID_MARK);
	}

	/**
	* Marks forbidden position
	*/
	void MarkPositionForForbid(Vec2i position) {
		CreateDynamicObject(position, EntityType::FORBID_MARK);
	}

	/**
	* Returns true, if a guard mark can be placed on selected position
	*/
	bool IsPositionFreeForGuard(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
		return isFree;
	}

	/**
	* Returns true, if the position is marked for guard
	*/
	bool PositionContainsGuardMark(Vec2i position) {
		return IsPositionOfType(position, EntityType::GUARD_MARK);
	}

	/**
	* Marks position that will be guarded
	*/
	void MarkPositionForGuard(Vec2i position) {
		CreateDynamicObject(position, EntityType::GUARD_MARK);
	}

	/**
	* Returns true, if the position can be destroyed
	*/
	bool IsPositionFreeForDestroy(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		bool isFree = (node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end());
		return isFree;
	}

	/**
	* Returns true, if the position is marked for destroy
	*/
	bool PositionContainsDestroyMark(Vec2i position) {
		return IsPositionOfType(position, EntityType::DESTROY_MARK);
	}

	/**
	* Marks selected position to be intended for destroy
	*/
	void MarkPositionForDestroy(Vec2i position) {
		CreateDynamicObject(position, EntityType::DESTROY_MARK);
	}

	/**
	* Creates a new worker at selected position
	*/
	void SpawnWorker(ofVec2f position) {
		CreateMovingObject(position, EntityType::WORKER);
	}

	void CreateSeedBed(Vec2i position ) {
		CreateDynamicObject(position, EntityType::SEEDBED);
	}

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position) {
		// destroy bridge mark
		DestroyDynamicObject(position);
		// change node to ground
		auto node = this->hydroqMap->GetNode(position.x, position.y);
		node->ChangeMapNodeType(MapNodeType::GROUND);
		// refresh other models the node figures
		hydroqMap->RefreshNode(node);
		// send a message that the static object has been changed
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr), nullptr);
	}

	/**
	* Gets collection of dynamic objects
	*/
	map<Vec2i, Node*>& GetDynamicObjects() {
		return this->dynObjects;
	}

	/**
	* Gets collection of moving objects
	*/
	vector<Node*>& GetMovingObjects() {
		return this->movingObjects;
	}

	/**
	* Gets collection of game tasks
	*/
	vector<spt<GameTask>>& GetGameTasks() {
		return this->gameTasks;
	}

	/**
	* Gets copy of game tasks
	*/
	vector<spt<GameTask>> GetGameTaskCopy() {
		vector<spt<GameTask>> output = vector<spt<GameTask>>();
		for (auto task : gameTasks) output.push_back(task);
		return output;
	}

	bool RemoveGameTask(spt<GameTask> task) {
		auto found = find(gameTasks.begin(), gameTasks.end(), task);
		if (found != gameTasks.end()) {
			gameTasks.erase(found);
			return true;
		}
		else {
			return false;
		}
	}

	/**
	* Destroys dynamic object at selected position
	*/
	void DestroyDynamicObject(Vec2i position) {
		auto node = hydroqMap->GetNode(position.x, position.y);
		// change state of static node
		node->occupied = false;
		node->forbidden = false;
		// remove dynamic object
		auto obj = dynObjects[position];
		dynObjects.erase(position);
		rootNode->RemoveChild(obj, true);
		// send message
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj), nullptr);
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
		rootNode->SubmitChanges(true);
		rootNode->Update(delta, absolute);
	}

	private:

	bool IsPositionOfType(Vec2i position, EntityType type) {
		return (dynObjects.find(position) != dynObjects.end() &&
			dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == type);
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
		auto gameNode = CreateNode(EntityType::WORKER, position);
		movingObjects.push_back(gameNode);
		SendMessageNoBubbling(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::MOVING_CREATED, nullptr, gameNode), nullptr);
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
			nd->GetTransform().localPos.z = 20;
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
};