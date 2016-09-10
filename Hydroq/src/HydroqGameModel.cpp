#include "HydroqGameModel.h"
#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "MsgEvents.h"
#include "Seedbed.h"
#include "StateMachine.h"
#include "Worker.h"
#include "GameTask.h"
#include "Move.h"
#include "Scene.h"

void HydroqGameModel::Init() {
	hydroqMap = new HydMap();
	gameScene = new Scene("gamescene", false);
	rootNode = gameScene->GetSceneNode();
}


bool HydroqGameModel::IsPositionFreeForBuilding(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	return node->mapNodeType == MapNodeType::GROUND && !node->occupied;
}

bool HydroqGameModel::IsPositionFreeForBridge(Vec2i position) {
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


bool HydroqGameModel::PositionContainsBridgeMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::BRIDGE_MARK);
}

void HydroqGameModel::MarkPositionForBridge(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Placing bridge mark at [%d, %d]", position.x, position.y);
	auto node = CreateDynamicObject(position, EntityType::BRIDGE_MARK, this->faction, 0);
	auto newTask = spt<GameTask>(new GameTask(StringHash(TASK_BRIDGE_BUILD)));
	newTask->taskNode = node;
	gameTasks.push_back(newTask);
}

void HydroqGameModel::DeleteBridgeMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Deleting bridge mark at [%d, %d]", position.x, position.y);
	auto obj = dynObjects[position];

	for (auto task : gameTasks) {
		if (task->taskNode->GetId() == obj->GetId()) {
			COGLOGDEBUG("Hydroq", "Aborting building task because of deleted bridge mark");
			SendMessageToModel(StringHash(ACT_TASK_ABORTED), 0, new TaskAbortEvent(task));
			task->isEnded = true; // for sure
			RemoveGameTask(task);
			break;
		}
	}

	DestroyDynamicObject(position);
}

bool HydroqGameModel::IsPositionFreeForForbid(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
	return isFree;
}

bool HydroqGameModel::PositionContainsForbidMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::FORBID_MARK);
}


void HydroqGameModel::MarkPositionForForbid(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Forbidden position at [%d, %d]", position.x, position.y);
	CreateDynamicObject(position, EntityType::FORBID_MARK, this->faction, 0);
	this->hydroqMap->GetNode(position)->forbidden = true;
	this->hydroqMap->RefreshNode(position);
}

void HydroqGameModel::DeleteForbidMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Canceling forbidden position at [%d, %d]", position.x, position.y);
	DestroyDynamicObject(position);
}

bool HydroqGameModel::IsPositionFreeForGuard(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
	return isFree;
}


bool HydroqGameModel::PositionContainsGuardMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::GUARD_MARK);
}

void HydroqGameModel::MarkPositionForGuard(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Position for guard at [%d, %d]", position.x, position.y);
	CreateDynamicObject(position, EntityType::GUARD_MARK, this->faction, 0);
}

void HydroqGameModel::DeleteGuardMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Deleted guard position at [%d, %d]", position.x, position.y);
	auto obj = dynObjects[position];
	for (auto task : gameTasks) {
		if (task->taskNode->GetId() == obj->GetId()) {
			COGLOGDEBUG("Hydroq", "Aborting guard task because of deleted guard mark");
			SendMessageOutside(StringHash(ACT_TASK_ABORTED), 0, new TaskAbortEvent(task));
			task->isEnded = true; // for sure
			RemoveGameTask(task);
			break;
		}
	}
	DestroyDynamicObject(position);
}


bool HydroqGameModel::IsPositionFreeForDestroy(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = (node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end());
	return isFree;
}

bool HydroqGameModel::PositionContainsDestroyMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::DESTROY_MARK);
}

void HydroqGameModel::MarkPositionForDestroy(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Marked for destroy: at [%d, %d]", position.x, position.y);
	CreateDynamicObject(position, EntityType::DESTROY_MARK, this->faction, 0);
}


void HydroqGameModel::SpawnWorker(ofVec2f position) {
	SpawnWorker(position, this->faction, 0);
}

void HydroqGameModel::SpawnWorker(ofVec2f position, Faction faction, int identifier) {
	COGLOGDEBUG("Hydroq", "Creating worker for %s faction with identifier %d at [%.2f, %.2f]", 
		faction == Faction::BLUE ? "blue" : "red", identifier, position.x, position.y);
	CreateMovingObject(position, EntityType::WORKER, faction, identifier);

	if (multiplayer) {
		SendMessageOutside(StringHash(ACT_SYNC_OBJECT_CHANGED), 0,
			new SyncEvent(SyncEventType::OBJECT_CREATED, EntityType::WORKER, faction, position, identifier, 0));
	}
}

void HydroqGameModel::CreateSeedBed(Vec2i position) {
	CreateSeedBed(position, this->faction, 0);
}

void HydroqGameModel::CreateSeedBed(Vec2i position, Faction faction, int identifier) {
	COGLOGDEBUG("Hydroq", "Creating seedbed for %s faction with identifier %d at [%.2f, %.2f]",
		faction == Faction::BLUE ? "blue" : "red", identifier, position.x, position.y);
	CreateDynamicObject(position, EntityType::SEEDBED, faction, identifier);

	if (multiplayer) {
		SendMessageOutside(StringHash(ACT_SYNC_OBJECT_CHANGED), 0,
			new SyncEvent(SyncEventType::OBJECT_CREATED, EntityType::SEEDBED, faction, position, identifier, 0));
	}
}

void HydroqGameModel::BuildPlatform(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Creating platform at [%d, %d]", position.x, position.y);
	// destroy bridge mark
	DestroyDynamicObject(position);
	// change node to ground
	auto node = this->hydroqMap->GetNode(position.x, position.y);
	node->ChangeMapNodeType(MapNodeType::GROUND);
	// refresh other models the node figures
	hydroqMap->RefreshNode(node);
	// send a message that the static object has been changed
	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0, new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr));
}


vector<spt<GameTask>> HydroqGameModel::GetGameTaskCopy() {
	vector<spt<GameTask>> output = vector<spt<GameTask>>();
	for (auto task : gameTasks) output.push_back(task);
	return output;
}

bool HydroqGameModel::RemoveGameTask(spt<GameTask> task) {
	auto found = find(gameTasks.begin(), gameTasks.end(), task);
	if (found != gameTasks.end()) {
		gameTasks.erase(found);
		return true;
	}
	else {
		return false;
	}
}

void HydroqGameModel::Update(const uint64 delta, const uint64 absolute) {
	rootNode->SubmitChanges(true);
	rootNode->Update(delta, absolute);
}

bool HydroqGameModel::IsPositionOfType(Vec2i position, EntityType type) {
	return (dynObjects.find(position) != dynObjects.end() &&
		dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_GAME_ENTITY_TYPE) == type);
}

Node* HydroqGameModel::CreateDynamicObject(Vec2i position, EntityType entityType, Faction faction, int identifier) {
	auto hydMapNode = hydroqMap->GetNode(position.x, position.y);
	hydMapNode->occupied = true;
	auto gameNode = CreateNode(entityType, position, faction, identifier);
	dynObjects[position] = gameNode;

	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
		new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CREATED, hydMapNode, gameNode));
	return gameNode;
}

void HydroqGameModel::DestroyDynamicObject(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	// change state of static node
	node->occupied = false;
	node->forbidden = false;
	// remove dynamic object
	if (dynObjects.find(position) != dynObjects.end()) {
		auto obj = dynObjects[position];
		dynObjects.erase(position);
		rootNode->RemoveChild(obj, true);

		// refresh node
		this->hydroqMap->RefreshNode(position);

		// send message
		SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj));
	}
	else {
		this->hydroqMap->RefreshNode(position);
	}
}

void HydroqGameModel::CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier) {
	auto gameNode = CreateNode(EntityType::WORKER, position, faction, identifier);
	movingObjects.push_back(gameNode);
	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0, new MapObjectChangedEvent(ObjectChangeType::MOVING_CREATED, nullptr, gameNode));
}

void HydroqGameModel::SendMessageOutside(StringHash action, int subaction, MsgEvent* data) {
	SendMessageToListeners(action, subaction, data, nullptr);
}

void HydroqGameModel::SendMessageToModel(StringHash action, int subaction, MsgEvent* data) {
	Msg msg(HandlingType(Scope::DIRECT_NO_TRAVERSE, true, true), action, subaction, this->GetId(), nullptr, data);
	gameScene->SendMessage(msg, nullptr);
}

Node* HydroqGameModel::CreateNode(EntityType entityType, ofVec2f position, Faction faction, int identifier) {

	string name;
	Node* nd = new Node("");
	nd->SetSubType(identifier);
	nd->AddAttr(ATTR_FACTION, faction);

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
		if (faction == Faction::BLUE) {
			nd->SetTag("seedbed_blue");
		}
		else {
			nd->SetTag("seedbed_red");
		}

		auto nodeBeh = new Seedbed();
		nd->AddBehavior(nodeBeh);
	}
	else if (entityType == EntityType::WORKER) {
		if (faction == Faction::BLUE) {
			nd->SetTag("worker_blue");
		}
		else {
			nd->SetTag("worker_red");
		}

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