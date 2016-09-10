#include "GameModel.h"
#include "Component.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "MsgEvents.h"
#include "RigBehavior.h"
#include "StateMachine.h"
#include "Worker.h"
#include "GameTask.h"
#include "Move.h"
#include "Scene.h"
#include "DeltaUpdate.h"
#include "TaskScheduler.h"
#include "PlayerModel.h"
#include "GameAI.h"
#include "CompositeBehavior.h"
#include "ComponentStorage.h"

void GameModel::OnInit() {	
	
	hydroqMap = new GameMap();
	playerModel = GETCOMPONENT(PlayerModel);
	gameScene = new Scene("gamescene", false);
	rootNode = gameScene->GetSceneNode();
	rootNode->AddBehavior(new TaskScheduler(this));

	this->mapName = playerModel->GetMap();

	if (!playerModel->IsMultiplayer()) {
		rootNode->AddBehavior(new GameAI(this, playerModel->GetFaction() == Faction::RED ? Faction::BLUE : Faction::RED));
	}

	Settings mapConfig = Settings();
	auto xml = CogLoadXMLFile("mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();

	this->hydroqMap->LoadMap(mapConfig, mapName);
	this->cellSpace = new GridSpace<NodeCellObject>(ofVec2f(hydroqMap->GetWidth(), hydroqMap->GetHeight()), 1);

	DivideRigsIntoFactions();
}


bool GameModel::IsPositionFreeForBuilding(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	return node->mapNodeType == MapNodeType::GROUND && !node->occupied;
}

bool GameModel::IsPositionFreeForBridge(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = node->mapNodeType == MapNodeType::WATER && dynObjects.find(position) == dynObjects.end();

	if (isFree) {
		// at least one neighbor mustn't be water or it is already marked
		for (auto neighbor : node->GetNeighborsFourDirections()) {
			if (neighbor->mapNodeType != MapNodeType::WATER ||
				(dynObjects.find(neighbor->pos) != dynObjects.end() &&
					dynObjects[neighbor->pos]->GetAttr<EntityType>(ATTR_ENTITYTYPE) == EntityType::BRIDGE_MARK))
				return true;
		}
	}

	return false;
}


bool GameModel::PositionContainsBridgeMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::BRIDGE_MARK);
}

void GameModel::MarkPositionForBridge(Vec2i position, Faction faction) {
	COGLOGDEBUG("Hydroq", "Placing bridge mark at [%d, %d]", position.x, position.y);
	auto node = CreateDynamicObject(position, EntityType::BRIDGE_MARK, playerModel->GetFaction(), 0);
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::BRIDGE_BUILD, faction));
	newTask->SetTaskNode(node);
	gameTasks.push_back(newTask);
}

void GameModel::DeleteBridgeMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Deleting bridge mark at [%d, %d]", position.x, position.y);
	auto obj = dynObjects[position];

	for (auto task : gameTasks) {
		if (task->GetTaskNode()->GetId() == obj->GetId()) {
			COGLOGDEBUG("Hydroq", "Aborting building task because of deleted bridge mark");
			SendMessageToModel(StrId(ACT_TASK_ABORTED), 0, spt<TaskAbortEvent>(new TaskAbortEvent(task)));
			task->SetIsEnded(true); // for sure
			RemoveGameTask(task);
			break;
		}
	}

	this->hydroqMap->GetNode(position)->forbidden = false;
	this->hydroqMap->RefreshNode(position);

	DestroyDynamicObject(position);
}

bool GameModel::IsPositionFreeForForbid(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end();
	return isFree;
}

bool GameModel::PositionContainsForbidMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::FORBID_MARK);
}


void GameModel::MarkPositionForForbid(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Forbidden position at [%d, %d]", position.x, position.y);
	CreateDynamicObject(position, EntityType::FORBID_MARK, playerModel->GetFaction(), 0);
	this->hydroqMap->GetNode(position)->forbidden = true;
	this->hydroqMap->RefreshNode(position);
}

void GameModel::DeleteForbidMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Canceling forbidden position at [%d, %d]", position.x, position.y);
	DestroyDynamicObject(position);
}

bool GameModel::IsPositionFreeForDestroy(Vec2i position) {
	auto node = hydroqMap->GetNode(position.x, position.y);
	bool isFree = (node->mapNodeType == MapNodeType::GROUND && !node->occupied && dynObjects.find(position) == dynObjects.end());
	return isFree;
}

bool GameModel::PositionContainsDestroyMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::DESTROY_MARK);
}

void GameModel::MarkPositionForDestroy(Vec2i position, Faction faction) {
	COGLOGDEBUG("Hydroq", "Marked for destroy: at [%d, %d]", position.x, position.y);
	auto node = CreateDynamicObject(position, EntityType::DESTROY_MARK, playerModel->GetFaction(), 0);
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::BRIDGE_DESTROY, faction));
	newTask->SetTaskNode(node);
	gameTasks.push_back(newTask);
	this->hydroqMap->GetNode(position)->forbidden = true;
	this->hydroqMap->RefreshNode(position);
}


void GameModel::SpawnWorker(ofVec2f position, Vec2i rigPosition) {
	SpawnWorker(position, playerModel->GetFaction(), 0, rigPosition);
}

void GameModel::SpawnWorker(ofVec2f position, Faction faction, int identifier, Vec2i rigPosition) {
	CogLogInfo("Hydroq", "Creating worker for %s faction at [%.2f, %.2f]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	auto node = CreateMovingObject(position, EntityType::WORKER, faction, identifier);
	
	cellSpace->AddObject(new NodeCellObject(node));

	this->workers[rigPosition].push_back(node);

	if (faction == playerModel->GetFaction()) {
		playerModel->AddUnit(1);
	}

	if (playerModel->IsMultiplayer() && identifier == 0) {
		SendMessageOutside(StrId(ACT_SYNC_OBJECT_CHANGED), 0,
			spt<SyncEvent>(new SyncEvent(SyncEventType::OBJECT_CREATED, EntityType::WORKER, faction, position, node->GetId(), 0, rigPosition)));
	}
}


void GameModel::BuildPlatform(Vec2i position) {
	BuildPlatform(position, playerModel->GetFaction(), 0);

}

void GameModel::BuildPlatform(Vec2i position, Faction faction, int identifier) {
	CogLogInfo("Hydroq", "Creating platform for %s faction at [%d, %d]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	// destroy bridge mark
	DestroyDynamicObject(position);
	// change node to ground
	auto node = this->hydroqMap->GetNode(position.x, position.y);
	node->ChangeMapNodeType(MapNodeType::GROUND);
	// refresh other models the node figures
	hydroqMap->RefreshNode(node);
	// send a message that the static object has been changed
	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0, spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr)));

	// for all tasks, update their DELAY status; this is because some building tasks are not reachable yet but if some new platform is built,
	// the situation may be different
	for (auto& task : gameTasks) {
		task->SetIsDelayed(false);
	}

	if (playerModel->IsMultiplayer() && identifier == 0) {
		SendMessageOutside(StrId(ACT_SYNC_OBJECT_CHANGED), 0,
			spt<SyncEvent>(new SyncEvent(SyncEventType::MAP_CHANGED, EntityType::BRIDGE, faction, position, 0, 0, Vec2i(0))));
	}
}

void GameModel::DestroyPlatform(Vec2i position) {
	DestroyPlatform(position, playerModel->GetFaction(), 0);
}

void GameModel::DestroyPlatform(Vec2i position, Faction faction, int identifier) {
	CogLogInfo("Hydroq", "Destroying platform for %s faction at [%d, %d]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	// destroy delete mark
	DestroyDynamicObject(position);
	// change node to water
	auto node = this->hydroqMap->GetNode(position.x, position.y);
	node->ChangeMapNodeType(MapNodeType::WATER);
	// refresh other models the node figures
	hydroqMap->RefreshNode(node);

	// when a platform is destroyed, all path-finding tasks must be recalculated
	for (auto task : gameTasks) {
		task->SetNeedRecalculation(true);
	}

	// send a message that the static object has been changed
	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0, spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::STATIC_CHANGED, node, nullptr)));

	if (playerModel->IsMultiplayer() && identifier == 0) {
		SendMessageOutside(StrId(ACT_SYNC_OBJECT_CHANGED), 0,
			spt<SyncEvent>(new SyncEvent(SyncEventType::MAP_CHANGED, EntityType::WATER, faction, position, 0, 0, Vec2i(0))));
	}
}


void GameModel::AddAttractor(Vec2i position, Faction faction, float cardinality) {
	CogLogInfo("Hydroq", "Adding attractor at [%d, %d]", position.x, position.y);
	
	auto gameNode = CreateNode(EntityType::ATTRACTOR, position, faction, 0);
	gameNode->AddAttr(ATTR_CARDINALITY, cardinality);
	attractors[faction][position] = gameNode;

	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
		spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::ATTRACTOR_CREATED, nullptr, gameNode)));
	auto newTask = spt<GameTask>(new GameTask(GameTaskType::ATTRACT, faction));
	newTask->SetTaskNode(gameNode);
	gameTasks.push_back(newTask);
}

void GameModel::DestroyAttractor(Vec2i position, Faction faction) {
	if (attractors.find(faction) != attractors.end() && attractors[faction].find(position) != attractors[faction].end()) {

		Node* gameNode = attractors[faction][position];

		CogLogInfo("Hydroq", "Removing attractor at [%d, %d]", position.x, position.y);

		attractors[faction].erase(position);

		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::ATTRACTOR_REMOVED, nullptr, gameNode)));

		for (auto task : gameTasks) {
			if (task->GetTaskNode()->GetId() == gameNode->GetId()) {
				COGLOGDEBUG("Hydroq", "Aborting attractor task because of deleted attractor");
				SendMessageToModel(StrId(ACT_TASK_ABORTED), 0, spt<TaskAbortEvent>(new TaskAbortEvent(task)));
				task->SetIsEnded(true); // for sure
				RemoveGameTask(task);
				break;
			}
		}
	}
}

void GameModel::DestroyAllAttractors(Faction faction) {
	vector<Vec2i> positions;
	for (auto& attr : attractors[faction]) {
		positions.push_back(attr.first);
	}

	for (auto pos : positions) {
		DestroyAttractor(pos, faction);
	}
}

void GameModel::ChangeAttractorCardinality(Vec2i position, Faction faction, float cardinality) {
	auto gameNode = attractors[faction][position];
	gameNode->ChangeAttr(ATTR_CARDINALITY, cardinality);
}

float GameModel::CalcAttractorAbsCardinality(Faction faction, int attractorId) {
	
	float attrCardinality = 0;
	float cardinalitySum = 0;
	
	for (auto& key : attractors[faction]) {
		float card = key.second->GetAttr<float>(ATTR_CARDINALITY);
		if (key.second->GetId() == attractorId) {
			attrCardinality = card;
		}
		cardinalitySum += card;
	}
	return attrCardinality / cardinalitySum;
}

void GameModel::ChangeRigOwner(Node* rig, Faction faction) {
	auto oldFaction = rig->GetAttr<Faction>(ATTR_FACTION);
	if (oldFaction == Faction::NONE) {
		if(faction == playerModel->GetFaction()) playerModel->AddBuildings(1);
		rig->ChangeAttr(ATTR_FACTION, faction);
		if(!playerModel->IsMultiplayer()) rig->AddBehavior(new RigBehavior(this));
		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::RIG_TAKEN, nullptr, rig)));

		// send message about game state change
		SendMessageToModel(StrId(ACT_GAMESTATE_CHANGED), 0,
			spt<GameStateChangedEvent>(new GameStateChangedEvent(GameChangeType::EMPTY_RIG_CAPTURED, faction)));
	}
	else {
		if (oldFaction == playerModel->GetFaction()) playerModel->RemoveBuilding(1);
		else playerModel->AddBuildings(1);

		rig->ChangeAttr(ATTR_FACTION, faction);
		
		auto rigPos = Vec2i(rig->GetTransform().localPos.x, rig->GetTransform().localPos.y);
		for (auto worker : workers[rigPos]) {
			// change workers faction according to the new rig owner
			worker->ChangeAttr(ATTR_FACTION, faction);
			worker->SetTag(faction == Faction::RED ? "worker_red" : "worker_blue");
			if (oldFaction == playerModel->GetFaction()) playerModel->RemoveUnit(1);
			else playerModel->AddUnit(1);
		}

		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::RIG_CAPTURED, nullptr, rig)));

		// send message about game state change
		SendMessageToModel(StrId(ACT_GAMESTATE_CHANGED), 0,
			spt<GameStateChangedEvent>(new GameStateChangedEvent(GameChangeType::ENEMY_RIG_CAPTURED, faction)));

		if (GetRigsByFaction(oldFaction).size() == 0) {
			playerModel->SetGameEnded(true);
			playerModel->SetPlayerWin(oldFaction != playerModel->GetFaction());
		}
	}
}


vector<spt<GameTask>> GameModel::GetGameTasksByFaction(Faction faction) {
	vector<spt<GameTask>> output;
	for (auto task : gameTasks) {
		if (task->GetFaction() == faction) {
			output.push_back(task);
		}
	}
	return output;
}

vector<Node*> GameModel::GetMovingObjectsByType(EntityType type, Faction faction) {
	
	vector<Node*> output;
	
	for (auto& obj : movingObjects) {
		if (obj->GetAttr<Faction>(StrId(ATTR_FACTION)) == faction) {
			EntityType enttype = obj->GetAttr<EntityType>(ATTR_ENTITYTYPE);
			if (enttype == type) {
				output.push_back(obj);
			}
		}
	}

	return output;

}

bool GameModel::RemoveGameTask(spt<GameTask> task) {
	auto found = find(gameTasks.begin(), gameTasks.end(), task);
	if (found != gameTasks.end()) {
		gameTasks.erase(found);
		return true;
	}
	else {
		return false;
	}
}

Node* GameModel::FindNearestRigByFaction(Faction fact, ofVec2f startPos) {
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

vector<Node*> GameModel::GetRigsByFaction(Faction fact) {
	vector<Node*> output;

	for (auto rig : rigs) {
		if (rig.second->GetAttr<Faction>(ATTR_FACTION) == fact) output.push_back(rig.second);
	}

	return output;
}

vector<Node*> GameModel::GetAttractorsByFaction(Faction fact) {
	vector<Node*> output;

	for (auto attractor : attractors[fact]) {
		output.push_back(attractor.second);
	}

	return output;
}

void GameModel::Update(const uint64 delta, const uint64 absolute) {

	if (playerModel->GameEnded()) return;

	vector<spt<GameTask>> tasksForRecalc;

	// push tasks for recalculation so that when update is finished, the indicator will be restarted
	for (auto task : gameTasks) {
		if (task->NeedRecalculation()) tasksForRecalc.push_back(task);
	}

	if (playerModel->IsMultiplayer()) {
		// update deltas
		auto deltaUpdate = GETCOMPONENT(DeltaUpdate);
		auto actual = deltaUpdate->GetActualDelta();

		if (actual) {
			for (auto& node : this->movingObjects) {
				if (node->GetSecondaryId() != 0) {
					int subType = node->GetSecondaryId();

					auto& deltas = actual->GetDeltas();

					if (deltas.find(subType*3) != deltas.end()) {

						float rotation = deltas[subType * 3 + 0];
						float posX = deltas[subType * 3 + 1];
						float posY = deltas[subType * 3 + 2];

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

	this->cellSpace->UpdateObjects();

	if (CogGetFrameCounter() % 4 == 0) {
		StrId factionAttr = StrId(ATTR_FACTION);
		for (auto& rig : this->rigs) {
			int totalForBlue = 0;
			int totalForRed = 0;

			auto rigHoldings = rig.second->GetAttr<spt<vector<RigPlatform>>>(ATTR_PLATFORMS);
			vector<NodeCellObject*> cellObjects;
			for (auto& rigHolding : *rigHoldings) {
				rigHolding.factionHoldings.clear();
				rigHolding.factionHoldings[Faction::BLUE] = 0;
				rigHolding.factionHoldings[Faction::RED] = 0;
				cellSpace->CalcNeighbors(ofVec2f(rigHolding.position.x + 0.5f, rigHolding.position.y + 0.5f), 0.5f, cellObjects);

				for (auto obj : cellObjects) {
					auto node = obj->node;
					Faction fc = node->GetAttr<Faction>(factionAttr);
					rigHolding.factionHoldings[fc]++;
					if (fc == Faction::BLUE) totalForBlue++;
					else totalForRed++;
				}
				cellObjects.clear();
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

	// restart recalc indicator
	for (auto task : tasksForRecalc) {
		task->SetNeedRecalculation(false);
	}
}

bool GameModel::IsPositionOfType(Vec2i position, EntityType type) {
	return (dynObjects.find(position) != dynObjects.end() &&
		dynObjects.find(position)->second->GetAttr<EntityType>(ATTR_ENTITYTYPE) == type);
}

Node* GameModel::CreateDynamicObject(Vec2i position, EntityType entityType, Faction faction, int identifier) {
	auto hydMapNode = hydroqMap->GetNode(position.x, position.y);
	hydMapNode->occupied = true;
	auto gameNode = CreateNode(entityType, position, faction, identifier);
	dynObjects[position] = gameNode;

	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
		spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CREATED, hydMapNode, gameNode)));
	return gameNode;
}

void GameModel::DestroyDynamicObject(Vec2i position) {
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
		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj)));
	}
	else {
		this->hydroqMap->RefreshNode(position);
	}
}

Node* GameModel::CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier) {
	auto gameNode = CreateNode(EntityType::WORKER, position, faction, identifier);
	movingObjects.push_back(gameNode);
	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0, spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::MOVING_CREATED, nullptr, gameNode)));
	return gameNode;
}

void GameModel::SendMessageOutside(StrId action, int subaction, spt<MsgPayload> data) {
	Msg msg(action, MsgObjectType::BEHAVIOR, this->id, MsgObjectType::SUBSCRIBERS, nullptr, data);
	msg.SetParameter(subaction);
	owner->GetScene()->SendMessage(msg);
}

void GameModel::SendMessageToModel(StrId action, int subaction, spt<MsgPayload> data) {
	Msg msg(action, MsgObjectType::BEHAVIOR, this->id, MsgObjectType::SUBSCRIBERS, nullptr, data);
	msg.SetParameter(subaction);
	gameScene->SendMessage(msg);
}

Node* GameModel::CreateNode(EntityType entityType, ofVec2f position, Faction faction, int identifier) {

	string name;
	Node* nd = new Node("");
	nd->SetSecondaryId(identifier);
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
		
		if (faction == playerModel->GetFaction() || (!playerModel->IsMultiplayer() && faction != Faction::NONE)) {
			nd->AddBehavior(new RigBehavior(this));
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
			((StateMachine*)nodeBeh)->ChangeState(new WorkerIdleState(this));
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerBridgeBuildState(this));
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerAttractorFollowState(this));

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

void GameModel::DivideRigsIntoFactions() {

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
	// for multiplayer, the selected rig should be deterministic
	int rigIndex = playerModel->IsMultiplayer() ? 0 : randomIndex;

	Vec2i blueRig = allRigs[rigIndex];
	
	// now find the rig that is furthest from the blue one
	// sort them according to their distance to nearest border
	sort(allRigs.begin(), allRigs.end(),
		[blueRig](Vec2i& a, Vec2i& b) -> bool
	{
		auto dist1 = Vec2i::Distancef(blueRig, a);
		auto dist2 = Vec2i::Distancef(blueRig, b);
		return dist1 > dist2;
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
		vector<Vec2i> platforms;
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

		spt<vector<RigPlatform>> rigPlatforms = spt<vector<RigPlatform>>(new vector <RigPlatform>());

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