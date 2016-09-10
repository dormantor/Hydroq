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
#include "Interpolator.h"
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
	auto xml = CogLoadXMLFile("config/mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();

	this->hydroqMap->LoadMap(mapConfig, mapName);
	this->cellSpace = new GridSpace<NodeCellObject>(ofVec2f(hydroqMap->GetWidth(), hydroqMap->GetHeight()), 1);

	DivideRigsIntoFactions();
}


bool GameModel::IsPositionFreeForBuilding(Vec2i position) {
	auto tile = hydroqMap->GetTile(position.x, position.y);
	return tile->GetMapTileType() == MapTileType::GROUND && !tile->IsOccupied();
}

bool GameModel::IsPositionFreeForBridge(Vec2i position) {
	auto tile = hydroqMap->GetTile(position.x, position.y);
	bool isFree = tile->GetMapTileType() == MapTileType::WATER && dynObjects.find(position) == dynObjects.end();

	if (isFree) {
		vector<GameMapTile*> neighbors;
		tile->GetNeighborsFourDirections(neighbors);
		// at least one neighbor mustn't be water or it is already marked
		for (auto neighbor : neighbors) {
			if (neighbor->GetMapTileType() != MapTileType::WATER ||
				(dynObjects.find(neighbor->GetPosition()) != dynObjects.end() &&
					dynObjects[neighbor->GetPosition()]->GetAttr<EntityType>(ATTR_ENTITYTYPE) == EntityType::BRIDGE_MARK))
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
	auto node = CreateDynamicObject(position, EntityType::BRIDGE_MARK, faction, 0);
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

	this->hydroqMap->GetTile(position)->SetIsForbidden(false);
	this->hydroqMap->RefreshTile(position);

	DestroyDynamicObject(position);
}

bool GameModel::IsPositionFreeForForbid(Vec2i position) {
	auto node = hydroqMap->GetTile(position.x, position.y);
	bool isFree = node->GetMapTileType() == MapTileType::GROUND && !node->IsOccupied() && dynObjects.find(position) == dynObjects.end();
	return isFree;
}

bool GameModel::PositionContainsForbidMark(Vec2i position) {
	return IsPositionOfType(position, EntityType::FORBID_MARK);
}


void GameModel::MarkPositionForForbid(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Forbidden position at [%d, %d]", position.x, position.y);
	CreateDynamicObject(position, EntityType::FORBID_MARK, playerModel->GetFaction(), 0);
	this->hydroqMap->GetTile(position)->SetIsForbidden(true);
	this->hydroqMap->RefreshTile(position);
}

void GameModel::DeleteForbidMark(Vec2i position) {
	COGLOGDEBUG("Hydroq", "Canceling forbidden position at [%d, %d]", position.x, position.y);
	DestroyDynamicObject(position);
}

bool GameModel::IsPositionFreeForDestroy(Vec2i position) {
	auto node = hydroqMap->GetTile(position.x, position.y);
	bool isFree = (node->GetMapTileType() == MapTileType::GROUND && !node->IsOccupied() && dynObjects.find(position) == dynObjects.end());
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
	this->hydroqMap->GetTile(position)->SetIsForbidden(true);
	this->hydroqMap->RefreshTile(position);
}


void GameModel::SpawnWorker(ofVec2f position, Vec2i rigPosition) {
	SpawnWorker(position, playerModel->GetFaction(), 0, rigPosition);
}

void GameModel::SpawnWorker(ofVec2f position, Faction faction, int identifier, Vec2i rigPosition) {
	CogLogInfo("Hydroq", "Creating worker for %s faction at [%.2f, %.2f]", (faction == Faction::BLUE ? "blue" : "red"), position.x, position.y);
	auto node = CreateMovingObject(position, EntityType::WORKER, faction, identifier);
	
	cellSpace->AddObject(new NodeCellObject(node));
	this->rigs[rigPosition]->spawnedWorkers.push_back(node);
	
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
	auto node = this->hydroqMap->GetTile(position.x, position.y);
	node->SetMapTileType(MapTileType::GROUND);
	// refresh other models the node figures
	hydroqMap->RefreshTile(node);
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
	auto node = this->hydroqMap->GetTile(position.x, position.y);
	node->SetMapTileType(MapTileType::WATER);
	// refresh other models the node figures
	hydroqMap->RefreshTile(node);

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
	return isEqual(cardinalitySum, attrCardinality) ? attrCardinality : attrCardinality / cardinalitySum;
}

void GameModel::ChangeRigOwner(Node* rig, Faction faction) {
	auto oldFaction = rig->GetAttr<Faction>(ATTR_FACTION);
	if (oldFaction == Faction::NONE) {
		if(faction == playerModel->GetFaction()) playerModel->AddRigs(1);
		rig->ChangeAttr(ATTR_FACTION, faction);
		if(!playerModel->IsMultiplayer()) rig->AddBehavior(new RigBehavior(this, 0.3f));
		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::RIG_TAKEN, nullptr, rig)));

		// send message about game state change
		SendMessageToModel(StrId(ACT_GAMESTATE_CHANGED), 0,
			spt<GameStateChangedEvent>(new GameStateChangedEvent(GameChangeType::EMPTY_RIG_CAPTURED, faction)));
	}
	else {
		if (oldFaction == playerModel->GetFaction()) playerModel->RemoveRigs(1);
		else playerModel->AddRigs(1);

		rig->ChangeAttr(ATTR_FACTION, faction);
		
		auto rigPos = Vec2i(rig->GetTransform().localPos.x, rig->GetTransform().localPos.y);
		auto& workers = rigs[rigPos]->spawnedWorkers;

		for (auto worker : workers) {
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
		
		vector<Node*> rigs;
		GetRigsByFaction(oldFaction, rigs);
		if (rigs.size() == 0) {
			playerModel->SetGameEnded(true);
			playerModel->SetPlayerWin(oldFaction != playerModel->GetFaction());
		}
	}
}


void GameModel::GetGameTasksByFaction(Faction faction, vector<spt<GameTask>>& output) {
	for (auto task : gameTasks) {
		if (task->GetFaction() == faction) {
			output.push_back(task);
		}
	}
}

void GameModel::GetMovingObjectsByType(EntityType type, Faction faction, vector<Node*>& output) {
	for (auto& obj : movingObjects) {
		if (obj->GetAttr<Faction>(StrId(ATTR_FACTION)) == faction) {
			EntityType enttype = obj->GetAttr<EntityType>(ATTR_ENTITYTYPE);
			if (enttype == type) {
				output.push_back(obj);
			}
		}
	}
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

void GameModel::GetRigsByFaction(Faction fact, vector<Node*>& output) {
	for (auto rig : rigs) {
		if (rig.second->gameNode->GetAttr<Faction>(ATTR_FACTION) == fact) output.push_back(rig.second->gameNode);
	}
}

void GameModel::GetAttractorsByFaction(Faction fact, vector<Node*>& output) {
	for (auto attractor : attractors[fact]) {
		output.push_back(attractor.second);
	}

}

void GameModel::Update(const uint64 delta, const uint64 absolute) {

	if (playerModel->GameEnded()) return;

	vector<spt<GameTask>> tasksForRecalc;

	// push tasks for recalculation so that when update is finished, the indicator will be restarted
	for (auto task : gameTasks) {
		if (task->NeedRecalculation()) tasksForRecalc.push_back(task);
	}

	if (playerModel->IsMultiplayer()) {
		UpdateFromInterpolator();
	}

	rootNode->SubmitChanges(true);
	rootNode->Update(delta, absolute);

	this->cellSpace->UpdateObjects();

	if (CogGetFrameCounter() % 4 == 0) {
		CheckRigCapturing();
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
	auto hydMapNode = hydroqMap->GetTile(position.x, position.y);
	hydMapNode->SetIsOccupied(true);
	auto gameNode = CreateNode(entityType, position, faction, identifier);
	dynObjects[position] = gameNode;

	SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
		spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_CREATED, hydMapNode, gameNode)));
	return gameNode;
}

void GameModel::DestroyDynamicObject(Vec2i position) {
	auto node = hydroqMap->GetTile(position.x, position.y);
	// change state of static node
	node->SetIsOccupied(false);
	node->SetIsForbidden(false);
	// remove dynamic object
	if (dynObjects.find(position) != dynObjects.end()) {
		auto obj = dynObjects[position];
		dynObjects.erase(position);
		rootNode->RemoveChild(obj, true);

		// refresh node
		this->hydroqMap->RefreshTile(position);

		// send message
		SendMessageOutside(StrId(ACT_MAP_OBJECT_CHANGED), 0,
			spt<MapObjectChangedEvent>(new MapObjectChangedEvent(ObjectChangeType::DYNAMIC_REMOVED, node, obj)));
	}
	else {
		this->hydroqMap->RefreshTile(position);
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
		float frequency = CogGetProjectSettings().GetSettingValFloat("hydroq_set", "rig_spawn_frequency");
		nd->AddBehavior(new RigBehavior(this, frequency));
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
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerBuildState(this));
			((StateMachine*)nodeBeh)->AddLocalState(new WorkerAttractState(this));

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

		auto hydMapNode = hydroqMap->GetTile(rig.x, rig.y);
		hydMapNode->SetIsOccupied(true);
		auto gameNode = CreateNode(EntityType::RIG, rig, fact, 0);

		auto rigEntity = spt<Rig>(new Rig());
		rigEntity->gameNode = gameNode;
		rigEntity->position = rig;

		dynObjects[rig] = gameNode;
		
		// create platform collection
		vector<Vec2i> platforms;
		auto pos = hydMapNode->GetPosition();
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

		for (auto platformPos : platforms) {
			rigEntity->platforms.push_back(platformPos);
		}

		rigs[rig] = rigEntity;
		// add entity into game object
		gameNode->AddAttr(ATTR_RIGENTITY, rigEntity);
	}

	// add rig into player model
	playerModel->AddRigs(1);
}

void GameModel::UpdateFromInterpolator() {
	// update from network interpolator
	auto interpolator = GETCOMPONENT(Interpolator);
	auto actual = interpolator->GetActualUpdate();

	if (actual) {
		for (auto& node : this->movingObjects) {
			if (node->GetSecondaryId() != 0) {
				int subType = node->GetSecondaryId();

				auto& values = actual->GetContinuousValues();

				if (values.find(subType * 3) != values.end()) {

					float rotation = values[subType * 3 + 0];
					float posX = values[subType * 3 + 1];
					float posY = values[subType * 3 + 2];

					node->GetTransform().localPos.x = posX;
					node->GetTransform().localPos.y = posY;
					node->GetTransform().rotation = rotation;
				}
			}
		}
	}
}

void GameModel::CheckRigCapturing() {
	StrId factionAttr = StrId(ATTR_FACTION);
	for (auto& rig : this->rigs) {
		int totalForBlue = 0;
		int totalForRed = 0;

		auto& platforms = rig.second->platforms;
		vector<NodeCellObject*> cellObjects;
		rig.second->factionHoldings.clear();

		for (auto platform : platforms) {
			FactionHolding holding;
			rig.second->factionHoldings[platform] = holding;

			cellSpace->CalcNeighbors(ofVec2f(platform.x + 0.5f, platform.y + 0.5f), 0.5f, cellObjects);

			for (auto obj : cellObjects) {
				auto node = obj->node;
				Faction fc = node->GetAttr<Faction>(factionAttr);
				if (fc == Faction::BLUE) {
					rig.second->factionHoldings[platform].blueNumber++;
					totalForBlue++;
				}
				else {
					rig.second->factionHoldings[platform].redNumber++;
					totalForRed++;
				}
			}
			cellObjects.clear();
		}

		if (totalForBlue != totalForRed) {
			auto rigFaction = rig.second->gameNode->GetAttr<Faction>(ATTR_FACTION);

			if (totalForBlue > totalForRed && rigFaction != Faction::BLUE) {
				ChangeRigOwner(rig.second->gameNode, Faction::BLUE);
			}
			else if (totalForBlue < totalForRed && rigFaction != Faction::RED) {
				ChangeRigOwner(rig.second->gameNode, Faction::RED);
			}
		}
	}
}