#include "HydroqGameModel.h"
#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "MsgEvents.h"
#include "RigBehavior.h"
#include "StateMachine.h"
#include "Worker.h"
#include "GameTask.h"
#include "Move.h"
#include "Scene.h"
#include "DeltaUpdate.h"
#include "TaskScheduler.h"
#include "HydroqPlayerModel.h"

void HydroqGameModel::Init() {
	hydroqMap = new HydMap();
	playerModel = GETCOMPONENT(HydroqPlayerModel);
	gameScene = new Scene("gamescene", false);
	rootNode = gameScene->GetSceneNode();
	rootNode->AddBehavior(new TaskScheduler());
}

void HydroqGameModel::StartGame(Faction faction, string map, bool isMultiplayer) {
	this->faction = faction;
	this->mapName = map;
	this->multiplayer = isMultiplayer;

	Settings mapConfig = Settings();
	auto xml = CogLoadXMLFile("mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();

	this->hydroqMap->LoadMap(mapConfig, map);
	this->cellSpace = new CellSpace(ofVec2f(hydroqMap->GetWidth(),hydroqMap->GetHeight()),1);

	DivideRigsIntoFactions();
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
					dynObjects[neighbor->pos]->GetAttr<EntityType>(ATTR_ENTITYTYPE) == EntityType::BRIDGE_MARK))
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
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::BRIDGE_BUILD));
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

	this->hydroqMap->GetNode(position)->forbidden = false;
	this->hydroqMap->RefreshNode(position);

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
	auto node = CreateDynamicObject(position, EntityType::DESTROY_MARK, this->faction, 0);
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::BRIDGE_DESTROY));
	newTask->taskNode = node;
	gameTasks.push_back(newTask);
	this->hydroqMap->GetNode(position)->forbidden = true;
	this->hydroqMap->RefreshNode(position);
}


void HydroqGameModel::SpawnWorker(ofVec2f position) {
	SpawnWorker(position, this->faction, 0);
}

void HydroqGameModel::SpawnWorker(ofVec2f position, Faction faction, int identifier) {
	CogLogInfo("Hydroq", "Creating worker for %s faction at [%.2f, %.2f]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	auto node = CreateMovingObject(position, EntityType::WORKER, faction, identifier);
	cellSpace->AddNode(node);

	if (faction == this->faction) {
		playerModel->AddUnit(1);
	}

	if (multiplayer && identifier == 0) {
		SendMessageOutside(StringHash(ACT_SYNC_OBJECT_CHANGED), 0,
			new SyncEvent(SyncEventType::OBJECT_CREATED, EntityType::WORKER, faction, position, node->GetId(), 0));
	}
}


void HydroqGameModel::BuildPlatform(Vec2i position) {
	BuildPlatform(position, this->faction, 0);

}

void HydroqGameModel::BuildPlatform(Vec2i position, Faction faction, int identifier) {
	CogLogInfo("Hydroq", "Creating platform for %s faction at [%d, %d]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	// destroy bridge mark
	DestroyDynamicObject(position);
	// change node to ground
	auto node = this->hydroqMap->GetNode(position.x, position.y);
	node->ChangeMapNodeType(MapNodeType::GROUND);
	// refresh other models the node figures
	hydroqMap->RefreshNode(node);
	// send a message that the static object has been changed
	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0, new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr));

	// for all tasks, update their DELAY status; this is because some building tasks are not reachable yet but if some new platform is built,
	// the situation may be different
	for (auto& task : gameTasks) {
		task->isDelayed = false;
	}

	if (multiplayer && identifier == 0) {
		SendMessageOutside(StringHash(ACT_SYNC_OBJECT_CHANGED), 0,
			new SyncEvent(SyncEventType::MAP_CHANGED, EntityType::BRIDGE, faction, position, 0, 0));
	}
}

void HydroqGameModel::DestroyPlatform(Vec2i position) {
	DestroyPlatform(position, this->faction, 0);
}

void HydroqGameModel::DestroyPlatform(Vec2i position, Faction faction, int identifier) {
	CogLogInfo("Hydroq", "Destroying platform for %s faction at [%d, %d]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	// destroy delete mark
	DestroyDynamicObject(position);
	// change node to water
	auto node = this->hydroqMap->GetNode(position.x, position.y);
	node->ChangeMapNodeType(MapNodeType::WATER);
	// refresh other models the node figures
	hydroqMap->RefreshNode(node);
	// send a message that the static object has been changed
	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0, new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr));

	if (multiplayer && identifier == 0) {
		SendMessageOutside(StringHash(ACT_SYNC_OBJECT_CHANGED), 0,
			new SyncEvent(SyncEventType::MAP_CHANGED, EntityType::WATER, faction, position, 0, 0));
	}
}


void HydroqGameModel::AddAttractor(Vec2i position, float cardinality) {
	CogLogInfo("Hydroq", "Adding attractor at [%d, %d]", position.x, position.y);
	
	auto gameNode = CreateNode(EntityType::ATTRACTOR, position, faction, 0);
	gameNode->AddAttr(ATTR_CARDINALITY, cardinality);
	attractors[position] = gameNode;

	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
		new MapObjectChangedEvent(ObjectChangeType::ATTRACTOR_CREATED, nullptr, gameNode));
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::ATTRACT));
	newTask->taskNode = gameNode;
	gameTasks.push_back(newTask);
}

void HydroqGameModel::DestroyAttractor(Vec2i position) {
	if (attractors.find(position) != attractors.end()) {
		auto gameNode = attractors[position];
		
		CogLogInfo("Hydroq", "Removing attractor at [%d, %d]", position.x, position.y);
		attractors.erase(position);

		SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::ATTRACTOR_REMOVED, nullptr, gameNode));

		for (auto task : gameTasks) {
			if (task->taskNode->GetId() == gameNode->GetId()) {
				COGLOGDEBUG("Hydroq", "Aborting attractor task because of deleted attractor");
				SendMessageToModel(StringHash(ACT_TASK_ABORTED), 0, new TaskAbortEvent(task));
				task->isEnded = true; // for sure
				RemoveGameTask(task);
				break;
			}
		}
	}
}

void HydroqGameModel::ChangeAttractorCardinality(Vec2i position, float cardinality) {
	if (attractors.find(position) != attractors.end()) {
		auto gameNode = attractors[position];
		gameNode->ChangeAttr(ATTR_CARDINALITY, cardinality);
	}
}

float HydroqGameModel::CalcAttractorAbsCardinality(int attractorId) {
	
	float attrCardinality = 0;
	float cardinalitySum = 0;
	
	for (auto& key : attractors) {
		float card = key.second->GetAttr<float>(ATTR_CARDINALITY);
		if (key.second->GetId() == attractorId) {
			attrCardinality = card;
		}
		cardinalitySum += card;
	}
	return attrCardinality / cardinalitySum;
}

void HydroqGameModel::ChangeRigOwner(Node* rig, Faction faction) {
	auto oldFaction = rig->GetAttr<Faction>(ATTR_FACTION);
	if (oldFaction == Faction::NONE) {
		playerModel->AddBuildings(1);
		rig->ChangeAttr(ATTR_FACTION, faction);
		SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::RIG_TAKEN, nullptr, rig));
	}
	else {
		// todo ...
		playerModel->AddBuildings(1);
		rig->ChangeAttr(ATTR_FACTION, faction);
		SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0,
			new MapObjectChangedEvent(ObjectChangeType::RIG_CAPTURED, nullptr, rig));
	}
}

vector<spt<GameTask>> HydroqGameModel::GetGameTaskCopy() {
	vector<spt<GameTask>> output = vector<spt<GameTask>>();
	for (auto task : gameTasks) output.push_back(task);
	return output;
}

vector<Node*> HydroqGameModel::GetMovingObjectsByType(EntityType type, Faction faction) {
	
	vector<Node*> output = vector<Node*>();
	
	for (auto& obj : movingObjects) {
		if (obj->GetAttr<Faction>(StringHash(ATTR_FACTION)) == faction) {
			EntityType enttype = obj->GetAttr<EntityType>(ATTR_ENTITYTYPE);
			if (enttype == type) {
				output.push_back(obj);
			}
		}
	}

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

Node* HydroqGameModel::FindNearestRigByFaction(Faction fact, ofVec2f startPos) {
	Node* nearestSoFar = nullptr;
	ofVec2f nearestPosSoFar = ofVec2f(0);

	for (auto& key : dynObjects) {
		auto node = key.second;

		if (node->HasAttr(ATTR_ENTITYTYPE) && node->GetAttr<EntityType>(ATTR_ENTITYTYPE) == EntityType::RIG 
			&& node->GetAttr<Faction>(ATTR_FACTION) == fact) {
			if (nearestSoFar == nullptr) {
				nearestSoFar = node;
				nearestPosSoFar = node->GetTransform().localPos;
			}
			else {
				auto pos = node->GetTransform().localPos;
				if (startPos.distanceSquared(pos) < startPos.distanceSquared(nearestPosSoFar)) {
					nearestSoFar = node;
					nearestPosSoFar = pos;
				}
			}
		}
	}

	return nearestSoFar;
}

vector<Node*> HydroqGameModel::GetRigsByFaction(Faction fact) {
	vector<Node*> output = vector<Node*>();

	for (auto rig : rigs) {
		if (rig.second->GetAttr<Faction>(ATTR_FACTION) == fact) output.push_back(rig.second);
	}

	return output;
}

void HydroqGameModel::Update(const uint64 delta, const uint64 absolute) {

	if (multiplayer) {
		// update deltas
		auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
		auto actual = deltaUpdate->actual;

		if (actual) {
			for (auto& node : this->movingObjects) {
				if (node->GetSubType() != 0) {
					int subType = node->GetSubType();

					if (actual->deltas.find(subType*3) != actual->deltas.end()) {

						float rotation = actual->deltas[subType * 3 + 0];
						float posX = actual->deltas[subType * 3 + 1];
						float posY = actual->deltas[subType * 3 + 2];

						node->GetTransform().localPos.x = posX;
						node->GetTransform().localPos.y = posY;
						node->GetTransform().rotation = rotation;
					}
				}
			}
		}
	}

	rootNode->SubmitChanges(true);
	rootNode->Update(delta, absolute);

	for (auto& node : this->movingObjects) {
		// update cellspace
		this->cellSpace->UpdateNode(node);
	}

	if (CogGetFrameCounter() % 4 == 0) {
		StringHash factionAttr = StringHash(ATTR_FACTION);
		for (auto& rig : this->rigs) {
			int totalForBlue = 0;
			int totalForRed = 0;

			auto rigHoldings = rig.second->GetAttr<spt<vector<RigPlatform>>>(ATTR_PLATFORMS);
			vector<Node*> nodes = vector<Node*>();
			for (auto& rigHolding : *rigHoldings) {
				rigHolding.factionHoldings.clear();
				rigHolding.factionHoldings[Faction::BLUE] = 0;
				rigHolding.factionHoldings[Faction::RED] = 0;
				cellSpace->CalcNeighbors(ofVec2f(rigHolding.position.x + 0.5f, rigHolding.position.y + 0.5f), 0.5f, nodes);

				for (auto node : nodes) {
					Faction fc = node->GetAttr<Faction>(factionAttr);
					rigHolding.factionHoldings[fc]++;
					if (fc == Faction::BLUE) totalForBlue++;
					else totalForRed++;
				}
				nodes.clear();
			}

			if (totalForBlue != totalForRed) {
				auto rigFaction = rig.second->GetAttr<Faction>(ATTR_FACTION);

				if (totalForBlue > totalForRed && rigFaction != Faction::BLUE) {
					ChangeRigOwner(rig.second, Faction::BLUE);
				}
				else if(totalForBlue < totalForRed && rigFaction != Faction::RED) {
					ChangeRigOwner(rig.second, Faction::RED);
				}
			}
		}
	}
}

bool HydroqGameModel::IsPositionOfType(Vec2i position, EntityType type) {
	return (dynObjects.find(position) != dynObjects.end() &&
		dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_ENTITYTYPE) == type);
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

Node* HydroqGameModel::CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier) {
	auto gameNode = CreateNode(EntityType::WORKER, position, faction, identifier);
	movingObjects.push_back(gameNode);
	SendMessageOutside(StringHash(ACT_MAP_OBJECT_CHANGED), 0, new MapObjectChangedEvent(ObjectChangeType::MOVING_CREATED, nullptr, gameNode));
	return gameNode;
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
	nd->AddAttr(ATTR_ENTITYTYPE, entityType);

	if (entityType == EntityType::BRIDGE_MARK) {
		nd->SetTag("bridgemark");
	}
	else if (entityType == EntityType::DESTROY_MARK) {
		nd->SetTag("destroymark");
	}
	else if (entityType == EntityType::FORBID_MARK) {
		nd->SetTag("forbidmark");
	}
	else if (entityType == EntityType::RIG) {
		nd->SetTag("rig");
		
		if (faction == this->faction) {
			nd->AddBehavior(new RigBehavior());
		}
	}
	else if (entityType == EntityType::WORKER) {
		if (faction == Faction::BLUE) {
			nd->SetTag("worker_blue");
		}
		else {
			nd->SetTag("worker_red");
		}

		nd->GetTransform().localPos.z = 20;

		if (identifier == 0) {
			auto nodeBeh = new StateMachine();
			((StateMachine*)nodeBeh)->ChangeState(new WorkerIdleState());
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerBridgeBuildState());
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerAttractorFollowState());

			nd->AddBehavior(nodeBeh);
			// add moving behavior
			nd->AddBehavior(new Move());
		}
	}

	nd->GetTransform().localPos.x = position.x;
	nd->GetTransform().localPos.y = position.y;


	rootNode->AddChild(nd);
	return nd;
}

void HydroqGameModel::DivideRigsIntoFactions() {

	// find empty rigs
	auto allRigs = this->hydroqMap->GetRigsPositions();

	int size = allRigs.size();
	int width = this->hydroqMap->GetWidth();
	int height = this->hydroqMap->GetHeight();

	// sort them according to their distance to nearest border
	sort(allRigs.begin(), allRigs.end(),
		[width, height](Vec2i& a, Vec2i& b) -> bool
	{
		int dist1x = a.x;
		int dist1y = a.y;
		int dist2x = width - a.x;
		int dist2y = height - a.y;

		int shortestDistA = min(min(min(dist1x, dist1y),dist2x),dist2y);
		
		dist1x = b.x;
		dist1y = b.y;
		dist2x = width - b.x;
		dist2y = height - b.y;

		int shortestDistB = min(min(min(dist1x, dist1y), dist2x), dist2y);

		return shortestDistA < shortestDistB;
	});

	int randomIndex = allRigs.size() >= 4 ? ((int)(ofRandom(0, 1)*allRigs.size() / 2)) : ((int)(ofRandom(0, 1)*allRigs.size()));

	Vec2i blueRig = allRigs[randomIndex];
	
	// now find the rig that is furthest from the blue one
	// sort them according to their distance to nearest border
	sort(allRigs.begin(), allRigs.end(),
		[blueRig](Vec2i& a, Vec2i& b) -> bool
	{
		auto dist1 = Vec2i::Distancef(blueRig, a);
		auto dist2 = Vec2i::Distancef(blueRig, b);
		return dist1 >= dist2;
	});

	// pick the first one for red
	Vec2i redRig = allRigs[0];

	// create dynamic objects from rigs
	for (auto rig : allRigs) {
		Faction fact = Faction::NONE;
		if (rig == redRig) fact = Faction::RED;
		else if (rig == blueRig) fact = Faction::BLUE;
		
		auto hydMapNode = hydroqMap->GetNode(rig.x, rig.y);
		hydMapNode->occupied = true;
		auto gameNode = CreateNode(EntityType::RIG, rig, fact, 0);
		dynObjects[rig] = gameNode;
		rigs[rig] = gameNode;

		// create platform collection
		vector<Vec2i> platforms = vector<Vec2i>();
		auto pos = hydMapNode->pos;
		platforms.push_back(Vec2i(pos.x - 1, pos.y - 1));
		platforms.push_back(Vec2i(pos.x, pos.y - 1));
		platforms.push_back(Vec2i(pos.x + 1, pos.y - 1));
		platforms.push_back(Vec2i(pos.x + 2, pos.y - 1));
		platforms.push_back(Vec2i(pos.x + 2, pos.y));
		platforms.push_back(Vec2i(pos.x + 2, pos.y + 1));
		platforms.push_back(Vec2i(pos.x + 2, pos.y + 2));
		platforms.push_back(Vec2i(pos.x + 1, pos.y + 2));
		platforms.push_back(Vec2i(pos.x, pos.y + 2));
		platforms.push_back(Vec2i(pos.x - 1, pos.y + 2));
		platforms.push_back(Vec2i(pos.x - 1, pos.y + 1));
		platforms.push_back(Vec2i(pos.x - 1, pos.y));

		spt<vector<RigPlatform>> rigPlatforms = spt<vector<RigPlatform>>(new vector <RigPlatform> ());

		for (auto platformPos : platforms) {
			RigPlatform rg = RigPlatform();
			rg.position = platformPos;
			rigPlatforms->push_back(rg);
		}

		gameNode->AddAttr(ATTR_PLATFORMS, rigPlatforms);
	}

	// add rig into player model
	playerModel->AddBuildings(1);
}