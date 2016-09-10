#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "MsgEvents.h"
#include "StateMachine.h"
#include "GameTask.h"
#include "Scene.h"
#include "NodeCellObject.h"
#include "PlayerModel.h"

class RigPlatform {
public:
	Vec2i position;
	map<Faction, int> factionHoldings;
};

/**
* Hydroq game model
*/
class GameModel : public Behavior {

private:
	// static objects (map, water, platforms)
	GameMap* hydroqMap;
	// cell partitioner for moving objects
	GridSpace<NodeCellObject>* cellSpace;
	
	// dynamic objects (buildings)
	map<Vec2i, Node*> dynObjects;
	// moving objects (units)
	vector<Node*> movingObjects;
	// subset of dynamic objects -> rigs
	map<Vec2i, Node*> rigs;
	// attractors
	map<Faction, map<Vec2i, Node*>> attractors;

	// rigs positions and workers the rig created
	map<Vec2i, vector<Node*>> workers;
	
	// scene and its root node that run separately from the view
	// part of the game
	Scene* gameScene;
	Node* rootNode;

	
	string mapName;
	// list of waiting tasks
	vector<spt<GameTask>> gameTasks;
	
	PlayerModel* playerModel;

public:

	~GameModel() {

	}

	void OnInit();

	Node* GetRootNode() {
		return rootNode;
	}

	GameMap* GetMap() {
		return hydroqMap;
	}

	GridSpace<NodeCellObject>* GetCellSpace() {
		return cellSpace;
	}

	string GetMapName() {
		return mapName;
	}

	map<Faction, map<Vec2i, Node*>>& GetAttractors() {
		return attractors;
	}

	vector<Node*>& GetWorkersOfRigAtPosition(Vec2i pos) {
		return workers[pos];
	}

	/**
	* Returns true, if a building can be built on selected position
	*/
	bool IsPositionFreeForBuilding(Vec2i position);

	/**
	* Returns true, if a bridge (platform) can be built on selected position
	*/
	bool IsPositionFreeForBridge(Vec2i position);

	/**
	* Returns true, if a bridge mark is placed on selected position
	*/
	bool PositionContainsBridgeMark(Vec2i position);
	
	/**
	* Marks position for a new bridge
	*/
	void MarkPositionForBridge(Vec2i position, Faction faction);

	void DeleteBridgeMark(Vec2i position);

	/**
	* Returns true, if the position can be forbidden
	*/
	bool IsPositionFreeForForbid(Vec2i position);

	/**
	* Returns true, if the selected position is forbidden
	*/
	bool PositionContainsForbidMark(Vec2i position);

	/**
	* Marks forbidden position
	*/
	void MarkPositionForForbid(Vec2i position);

	/**
	* Delete forbidden mark
	*/
	void DeleteForbidMark(Vec2i position);

	/**
	* Returns true, if the position can be destroyed
	*/
	bool IsPositionFreeForDestroy(Vec2i position);

	/**
	* Returns true, if the position is marked for destroy
	*/
	bool PositionContainsDestroyMark(Vec2i position);

	/**
	* Marks selected position to be intended for destroy
	*/
	void MarkPositionForDestroy(Vec2i position, Faction faction);

	/**
	* Creates a new worker at selected position
	*/
	void SpawnWorker(ofVec2f position, Vec2i rigPosition);

	/**
	* Creates a new worker at selected position for selected faction
	*/
	void SpawnWorker(ofVec2f position, Faction faction, int identifier, Vec2i rigPosition);

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position);

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position, Faction faction, int identifier);

	/**
	* Destroys a platform on selected position
	*/
	void DestroyPlatform(Vec2i position);

	/**
	* Destroys a platform on selected position
	*/
	void DestroyPlatform(Vec2i position, Faction faction, int identifier);

	void AddAttractor(Vec2i position, Faction faction, float cardinality);

	void DestroyAttractor(Vec2i position, Faction faction);

	void DestroyAllAttractors(Faction faction);

	void ChangeAttractorCardinality(Vec2i position, Faction faction, float cardinality);

	float CalcAttractorAbsCardinality(Faction faction, int attractorId);

	void ChangeRigOwner(Node* rig, Faction faction);

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

	map<Vec2i, Node*>& GetRigs() {
		return rigs;
	}

	/**
	* Gets copy of game tasks
	*/
	vector<spt<GameTask>> GetGameTasksByFaction(Faction faction);

	vector<Node*> GetMovingObjectsByType(EntityType type, Faction faction);

	bool RemoveGameTask(spt<GameTask> task);

	Node* FindNearestRigByFaction(Faction fact, ofVec2f startPos);

	vector<Node*> GetRigsByFaction(Faction fact);

	vector<Node*> GetAttractorsByFaction(Faction fact);


	virtual void Update(const uint64 delta, const uint64 absolute);

	private:

	bool IsPositionOfType(Vec2i position, EntityType type);

	Node* CreateDynamicObject(Vec2i position, EntityType entityType, Faction faction, int identifier);

	/**
	* Destroys dynamic object at selected position
	*/
	void DestroyDynamicObject(Vec2i position);

	Node* CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier);

	void SendMessageOutside(StrId action, int subaction, spt<MsgPayload> data);

	void SendMessageToModel(StrId action, int subaction, spt<MsgPayload> data);

	Node* CreateNode(EntityType entityType, ofVec2f position, Faction faction, int identifier);

	void DivideRigsIntoFactions();
};